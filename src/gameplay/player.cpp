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
float g_PlayerScale;
constexpr int g_PlayerMaxHealth = 100;
int g_PlayerHealth;

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
}

void Player_Finalize(){
	ModelRelease(g_pPlayerModel);
}

void Player_Update(double elapsed_time) {
    float dt = (float)elapsed_time;

    if (m_HitColorTimer > 0.0f) {
        m_HitColorTimer -= dt;
    }

    player.Update(elapsed_time);
    g_IsGrounded = false;

    XMVECTOR position = XMLoadFloat3(&g_PlayerPosition);
    XMVECTOR velocity = XMLoadFloat3(&g_PlayerVelocity);
    XMVECTOR front = XMLoadFloat3(&g_PlayerFront);

    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR right = XMVector3Cross(up, front);
    XMVECTOR moveDir = XMVectorZero();

    if (KeyLogger_IsTrigger(KK_SPACE) && !g_IsJump) {
        velocity = XMVectorSetY(velocity, 50.0f); 
        g_IsJump = true;
    }

    if (KeyLogger_IsPressed(KK_W)) moveDir += front;
    if (KeyLogger_IsPressed(KK_S)) moveDir -= front;
    if (KeyLogger_IsPressed(KK_D)) moveDir += right;
    if (KeyLogger_IsPressed(KK_A)) moveDir -= right;

    if (!XMVector3Equal(moveDir, XMVectorZero())) {
        moveDir = XMVectorSetY(moveDir, 0.0f);
        moveDir = XMVector3Normalize(moveDir);
        velocity += moveDir * 0.5f * MOVE_SPEED;
    }

    float friction = 5.0f;
    XMVECTOR horizontalVel = XMVectorSetY(velocity, 0.0f);
    horizontalVel -= horizontalVel * friction * dt;
    velocity = XMVectorSetY(horizontalVel, XMVectorGetY(velocity));

    float gravity = -9.8f * 15.0f;
    velocity += XMVectorSet(0.0f, gravity * dt, 0.0f, 0.0f);

    position += XMVectorSet(0.0f, XMVectorGetY(velocity) * dt, 0.0f, 0.0f);
    AABB playerAABB = Player_ConvertPositionToAABB(position);

    float highestY = -FLT_MAX;
    bool landed = false;

    for (int i = 0; i < Map_GetObjectsCount(); i++) {
        AABB object = Map_GetObject(i)->aabb;
        Hit hit = Collision_IsHitAABB(object, playerAABB);
        if (hit.isHit && hit.normal.y > 0.0f) {
            highestY = std::max(highestY, object.max.y);
            landed = true;
        }
    }

    if (landed || XMVectorGetY(position) < 0.0f) {
        float floorY = landed ? highestY + 0.001f : 0.0f;
        position = XMVectorSetY(position, floorY);
        velocity = XMVectorSetY(velocity, 0.0f);
        g_IsGrounded = true;
        g_IsJump = false;
    }

    XMVECTOR hMove = XMVectorMultiply(velocity, XMVectorSet(dt, 0.0f, dt, 0.0f));
    position += hMove;

    playerAABB = Player_ConvertPositionToAABB(position);
    XMFLOAT3 half = g_pPlayerModel->local_aabb.GetHalf();

    for (int i = 0; i < Map_GetObjectsCount(); i++) {
        AABB object = Map_GetObject(i)->aabb;
        Hit hit = Collision_IsHitAABB(object, playerAABB);

        if (hit.isHit) {
            if (hit.normal.x > 0.0f)      position = XMVectorSetX(position, object.max.x + half.x);
            else if (hit.normal.x < 0.0f) position = XMVectorSetX(position, object.min.x - half.x);

            if (hit.normal.z > 0.0f)      position = XMVectorSetZ(position, object.max.z + half.z);
            else if (hit.normal.z < 0.0f) position = XMVectorSetZ(position, object.min.z - half.z);

            velocity *= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        }
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
	Light_SetSpecularWorld(PlayerCamera_GetPosition(), 10.0f, { 0.3f,0.3f,0.3f,1.0f });
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
