/*==============================================================================

   プレイヤー制御 [player.cpp]
														 Author : Harada Ren
														 Date   : 2025/10/31
--------------------------------------------------------------------------------

==============================================================================*/
#include "player.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "model.h"
#include "key_logger.h"
#include "light.h"
#include "player_camera.h"
#include "cube.h"

static XMFLOAT3 g_PlayerPosition{};
static XMFLOAT3 g_PlayerFront{0.0f,0.0f,1.0f};
static XMFLOAT3 g_PlayerVelocity{};
static MODEL* g_pPlayerModel{ nullptr };
static bool g_IsJump = false;

void Player_Initialize(const XMFLOAT3& position,const XMFLOAT3& front){
	g_PlayerPosition = position;
	g_PlayerVelocity = { 0.0f,0.0f,0.0f };
	XMStoreFloat3(&g_PlayerFront, XMVector3Normalize(XMLoadFloat3(&front)));

	g_pPlayerModel = ModelLoad("resource/model/slime.fbx", 1.0f);
	//g_pPlayerModel = ModelLoad("resource/model/glass-fbx.FBX", 0.01f);
	//g_pPlayerModel = ModelLoad("resource/model/spoon.fbx", 0.5f);
	
}

void Player_Finalize(){
	ModelRelease(g_pPlayerModel);
}

void Player_Update(double elapsed_time){
	XMVECTOR position = XMLoadFloat3(&g_PlayerPosition);
	XMVECTOR velocity = XMLoadFloat3(&g_PlayerVelocity);

	//ジャンプ
	if (KeyLogger_IsTrigger(KK_SPACE) && !g_IsJump) {
		// Y方向の速度をジャンプ力で上書き
		velocity += { 0.0f,30.0f,0.0f };
		g_IsJump = true;
	}

	//重力
	XMVECTOR gdir{ 0.0f, 1.0f ,0.0f };
	velocity += gdir * -9.8f * 10.0f * (float)elapsed_time;
	position += velocity * (float)elapsed_time;

	if (XMVectorGetY(position) < 0.0f) {
		position -= velocity * (float)elapsed_time;
		velocity *= {1.0f, 0.0f, 1.0f};
		g_IsJump = false;
	}

	XMVECTOR direction{};
	XMVECTOR front = XMLoadFloat3(&PlayerCamera_GetFront()) * XMVECTOR { 1.0f, 0.0f, 1.0f };
	if (KeyLogger_IsPressed(KK_W)) {
		direction += front;
	}
	if (KeyLogger_IsPressed(KK_S)) {
		direction -= front;
	}
	if (KeyLogger_IsPressed(KK_D)) {
		direction += XMVector3Cross({ 0.0f,1.0f,0.0f }, front);
	}
	if (KeyLogger_IsPressed(KK_A)) {
		direction -= XMVector3Cross({ 0.0f,1.0f,0.0f }, front);
	}
	
	if (XMVectorGetX(XMVector3LengthSq(direction)) > 0.0f) {
		direction = XMVector3Normalize(direction);
		//XMStoreFloat3(&g_PlayerFront, direction);
		
		//2つのべえくとるのなす角
		float dot = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&g_PlayerFront), direction));
		float angle = acosf(dot);

		//回転速度
		const float ROTATION_SPEED = XM_2PI * 1.5f * (float)elapsed_time;

		//frontを通り過ぎるならそのまま
		if(angle<ROTATION_SPEED){
			front = direction;
		}
		//frontを通り過ぎないなら回転
		else {
			//向きたい方向が右回りか、左回りか調べる
			XMMATRIX r = XMMatrixIdentity();
			if (XMVectorGetY(XMVector3Cross(XMLoadFloat3(&g_PlayerFront), direction)) < 0.0f) {
				r = XMMatrixRotationY(-ROTATION_SPEED);
			}
			else {
				r = XMMatrixRotationY(ROTATION_SPEED);
			}

			front = XMVector3TransformNormal(XMLoadFloat3(&g_PlayerFront), r);
		}

		velocity += XMLoadFloat3(&g_PlayerFront) * (float)(2000.0 / 50.0 * elapsed_time);
		XMStoreFloat3(&g_PlayerFront, front);
	}
	
	velocity += -velocity * (float)(4.0 * elapsed_time);
	position += velocity * (float)elapsed_time;
	
	XMStoreFloat3(&g_PlayerPosition, position);
	XMStoreFloat3(&g_PlayerVelocity, velocity);

	//当たり判定
	AABB player = Player_GetAABB();
	AABB cube = Cube_GetAABB({ 3.0f, 0.5f, 2.0f });

	if (Collision_IsOverlapAABB(player, cube)) {
		position -= velocity * (float)elapsed_time;
		velocity = { 0.0f,0.0f,0.0f };
		XMStoreFloat3(&g_PlayerPosition, position);
		XMStoreFloat3(&g_PlayerVelocity, velocity);
	}
}

void Player_Draw(){
	//float dot = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&g_PlayerFront), XMVECTOR{ 1.0f,0.0f,0.0f }));
	float angle = -atan2f(g_PlayerFront.z,g_PlayerFront.x) + XMConvertToRadians(270);

	XMMATRIX r = XMMatrixRotationY(angle);
	XMMATRIX t = XMMatrixTranslation(g_PlayerPosition.x, g_PlayerPosition.y+1.0f, g_PlayerPosition.z);
	XMMATRIX world=r * t;
	ModelDraw(g_pPlayerModel, world);

	Light_SetSpecularWorld(PlayerCamera_GetPosition(), 10.0f, { 0.3f,0.3f,0.3f,1.0f });

}

const DirectX::XMFLOAT3& Player_GetPosition(){
	return g_PlayerPosition;
}

const DirectX::XMFLOAT3& Player_GetFront(){
	return g_PlayerFront;
}

AABB Player_GetAABB(){
	return {
		{g_PlayerPosition.x - 1.0f,g_PlayerPosition.y,       g_PlayerPosition.z - 1.0f},
		{g_PlayerPosition.x + 1.0f,g_PlayerPosition.y + 2.0f,g_PlayerPosition.z + 1.0f}
	};
}
