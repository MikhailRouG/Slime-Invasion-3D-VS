#include "player.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "model.h"
#include "key_logger.h"
#include "light.h"
#include "player_camera.h"
#include "cube.h"
#include "map.h"
#include "shader_depth.h"
#include <iostream>
#include <algorithm>
#include "circle_shadow.h"
#include "scene.h"
#include "score.h"
#include "game_Ui.h"
#include "SkinnedModel.h"

static ID3D11Device* device;
static ID3D11DeviceContext* context;
static XMFLOAT3 g_PlayerPosition{};
static XMFLOAT3 g_PlayerFront{0.0f,0.0f,1.0f};
static XMFLOAT3 g_PlayerVelocity{};
static MODEL* g_pPlayerModel{ nullptr };
static bool g_IsJump = false;
static bool g_IsGrounded = false;
constexpr int g_PlayerMaxHealth = 100;
int g_PlayerHealth;
float g_PlayerScale;
const float MOVE_SPEED = 2.0f;
const float ROTATE_SPEED = XM_PIDIV2* 2;
float m_HitColorTimer{ 0.0f };
SkinnedModel player;

void Player_SetDevice(ID3D11Device* div, ID3D11DeviceContext* con)
{
	device = div;
	context = con;
}

void Player_Initialize(const XMFLOAT3& position,const XMFLOAT3& front){
	g_PlayerHealth = g_PlayerMaxHealth;
	g_PlayerPosition = position;
	g_PlayerVelocity = { 0.0f,0.0f,0.0f };
	XMStoreFloat3(&g_PlayerFront, XMVector3Normalize(XMLoadFloat3(&front)));
	g_PlayerScale = 1;
	g_pPlayerModel = ModelLoad("resource/model/slime.fbx", 1);
	//player.Init(device);
	//player.Load("resource/model/slime.fbx");
	//player.Play(0);
}

void Player_Finalize(){
	ModelRelease(g_pPlayerModel);
	CircleShadow_Finalize();
}

void Player_Update(double elapsed_time){
	if(m_HitColorTimer > 0.0f)
{
	m_HitColorTimer -= (float)elapsed_time;
}
	player.Update(elapsed_time);
	g_IsGrounded = false;
	XMVECTOR position = XMLoadFloat3(&g_PlayerPosition);
	XMVECTOR velocity = XMLoadFloat3(&g_PlayerVelocity);
	XMVECTOR front = XMLoadFloat3(&g_PlayerFront);
	XMVECTOR gvelocity{};

	if (KeyLogger_IsTrigger(KK_SPACE) && !g_IsJump) {
		velocity += { 0.0f,50.0f,0.0f };
		g_IsJump = true;
	}

	XMVECTOR moveDir = XMVectorZero();
	if (KeyLogger_IsPressed(KK_W)) moveDir += front;
	if (KeyLogger_IsPressed(KK_S)) moveDir -= front;
	moveDir = XMVectorSetY(moveDir, 0.0f);
	moveDir = XMVector3Normalize(moveDir);

	float acceleration = 0.5f;
	if (!XMVector3Equal(moveDir, XMVectorZero())) {
		velocity += moveDir * acceleration * MOVE_SPEED;
	}
	float rotate = 0.0f;

	if (KeyLogger_IsPressed(KK_D)) {
		rotate += ROTATE_SPEED * (float)elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_A)) {
		rotate -= ROTATE_SPEED * (float)elapsed_time;
	}

	if (rotate != 0.0f) {
		XMMATRIX rotY = XMMatrixRotationY(rotate);
		front = XMVector3Normalize(
			XMVector3TransformNormal(front, rotY)
		);
	}
	XMStoreFloat3(&g_PlayerFront, front);
	XMStoreFloat3(&g_PlayerVelocity, velocity);

	float friction = 5.0f;
	XMVECTOR horizontalVel = XMVectorSetY(velocity, 0.0f); 
	horizontalVel -= horizontalVel * friction * (float)elapsed_time;
	velocity = XMVectorSetY(horizontalVel, XMVectorGetY(velocity));

	XMFLOAT3 gdir{ 0.0f, 1.0f , 0.0f};
	float gravity = -9.8f * 15.0f * (float)elapsed_time;
	if (!g_IsGrounded)
	{
		velocity += XMLoadFloat3(&gdir) * gravity;
	}

	XMVECTOR vertical_move = XMVectorSet(0.0f, XMVectorGetY(velocity) * (float)elapsed_time, 0.0f, 0.0f);
	position += vertical_move;

	AABB player = Player_ConvertPositionToAABB(position);

	float highestY = -FLT_MAX;
	bool landed = false;
	int objCount = Map_GetObjectsCount();

		for (int i = 0; i < Map_GetObjectsCount(); i++)
		{
			AABB object = Map_GetObject(i)->aabb;
			Hit hit = Collision_IsHitAABB(object, player);

			if (hit.isHit && hit.normal.y > 0.0f)
			{
				highestY = std::max(highestY, object.max.y);
				landed = true;
			}
		}

		if (landed)
		{
			position = XMVectorSetY(position, highestY + 0.001f);
			velocity = XMVectorSetY(velocity, 0.0f);
			g_IsGrounded = true;
			g_IsJump = false;
		}

	XMVECTOR horizontal_move = XMVectorSet(XMVectorGetX(velocity) * (float)elapsed_time, 0.0f, XMVectorGetZ(velocity) * (float)elapsed_time, 0.0f);
	position += horizontal_move;

	player =
	Player_ConvertPositionToAABB(position);
	XMFLOAT3 half = g_pPlayerModel->local_aabb.GetHalf();
	for (int i = 0;i < Map_GetObjectsCount();i++) {
		AABB object = Map_GetObject(i)->aabb;
		Hit hit = Collision_IsHitAABB(object, player);

		if (hit.isHit) {
			if (hit.normal.x > 0.0f) {
				position = XMVectorSetX(position, object.max.x + half.x);
				velocity *= { 0.0f, 1.0f, 1.0f };
			}
			else if (hit.normal.x < 0.0f) {
				position = XMVectorSetX(position, object.min.x - half.x);
				velocity *= { 0.0f, 1.0f, 1.0f };
			}
			else if (hit.normal.z > 0.0f) {
				position = XMVectorSetZ(position, object.max.z + half.z);
				velocity *= { 1.0f, 1.0f, 0.0f };
			}
			else if (hit.normal.z < 0.0f) {
				position = XMVectorSetZ(position, object.min.z - half.z);
				velocity *= { 1.0f, 1.0f, 0.0f };
			}

		}
	}
	//if (KeyLogger_IsTrigger(KK_J)) {
	//	XMFLOAT3 shot_position = g_PlayerPosition;
	//	XMFLOAT3 shot_velocity;
	//	shot_position.y += 1.0f;
	//	XMStoreFloat3(&shot_velocity, XMLoadFloat3(&g_PlayerFront) * 10.0f);
	//	Bullet_Create(shot_position, shot_velocity);
	//}
	if (XMVectorGetY(position) < 0.0f)
	{
		position = XMVectorSetY(position, 0.0f);
		velocity = XMVectorSetY(velocity, 0.0f);

		g_IsGrounded = true;
		g_IsJump = false;
	}
	XMStoreFloat3(&g_PlayerVelocity, velocity);
	XMStoreFloat3(&g_PlayerPosition, position);

}

void Player_Draw() {
	float angle = -atan2f(g_PlayerFront.z,g_PlayerFront.x) + XMConvertToRadians(270);
	XMMATRIX s = XMMatrixScaling(g_PlayerScale, g_PlayerScale, g_PlayerScale);
	XMMATRIX r = XMMatrixRotationY(angle);
	XMMATRIX t = XMMatrixTranslation(g_PlayerPosition.x, g_PlayerPosition.y, g_PlayerPosition.z);
	XMMATRIX world= s *r * t;
	XMFLOAT4 color;
	if (m_HitColorTimer > 0.0f)
		color = { 1,0,0,0.75f };
	else
		color = { 1.0f, 0.84f, 0.0f, 0.80f };
	ModelDraw(g_pPlayerModel, world, color);
	//player.Draw(context);
	Light_SetSpecularWorld(PlayerCamera_GetPosition(), 10.0f, { 0.3f,0.3f,0.3f,1.0f });
	CircleShadow_Draw(g_PlayerPosition);
}

void Player_DepthDraw()
{
	float angle = -atan2f(g_PlayerFront.z, g_PlayerFront.x) + XMConvertToRadians(270);

	XMMATRIX r = XMMatrixRotationY(angle);
	XMMATRIX t = XMMatrixTranslation(g_PlayerPosition.x, g_PlayerPosition.y, g_PlayerPosition.z);
	XMMATRIX world = r * t;
	ModelDepthDraw(g_pPlayerModel, world);

	Light_SetSpecularWorld(PlayerCamera_GetPosition(), 10.0f, { 0.3f,0.3f,0.3f,1.0f });

}

void Player_TakeDamage(int damage)
{

	g_PlayerHealth -= damage;
	m_HitColorTimer = 0.25f;
	if (g_PlayerHealth <= 0) Scene_Change(SCENE_RESULT);
}

int Player_GetHp()
{
	return g_PlayerHealth;
}

int Player_GetMaxHp()
{
	return g_PlayerMaxHealth;
}

const DirectX::XMFLOAT3& Player_GetPosition(){
	return g_PlayerPosition;
}

const DirectX::XMFLOAT3& Player_GetFront(){
	return g_PlayerFront;
}

AABB Player_GetAABB(){
	XMFLOAT3 half = g_pPlayerModel->local_aabb.GetHalf();
	return {
		{g_PlayerPosition.x - half.x,g_PlayerPosition.y,       g_PlayerPosition.z - half.z},
		{g_PlayerPosition.x + half.x,g_PlayerPosition.y + half.y,g_PlayerPosition.z + half.z}
	};
}

AABB Player_ConvertPositionToAABB(const XMVECTOR& position)
{
	XMFLOAT3 half = g_pPlayerModel->local_aabb.GetHalf();
	XMFLOAT3 p;
	XMStoreFloat3(&p, position);

	return {
		{ p.x - half.x, p.y,          p.z - half.z },
		{ p.x + half.x, p.y + half.y, p.z + half.z }
	};
}

void Player_SetScaling(float scale)
{
	g_PlayerScale = scale;
	PlayerCamera_SetScale(scale);
}
