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

static XMFLOAT3 g_PlayerPosition{};
static XMFLOAT3 g_PlayerFront{0.0f,0.0f,1.0f};
static XMFLOAT3 g_PlayerVelocity{};
static MODEL* g_pPlayerModel{ nullptr };
static bool g_IsJump = false;

void Player_Initialize(const XMFLOAT3& position,const XMFLOAT3& front){
	g_PlayerPosition = position;
	g_PlayerVelocity = { 0.0f,0.0f,0.0f };
	XMStoreFloat3(&g_PlayerFront, XMVector3Normalize(XMLoadFloat3(&front)));

	//g_pPlayerModel = ModelLoad("resource/model/slime.fbx", 1.0f);
	//g_pPlayerModel = ModelLoad("resource/model/glass-fbx.FBX", 0.01f);
	g_pPlayerModel = ModelLoad("resource/model/spoon.fbx", 0.5f);
	
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
	XMVECTOR front = XMLoadFloat3(&PlayerCamera_GetFront());
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

	direction = XMVector3Normalize(direction);
	
	velocity += direction * (float)(80000.0 / 50.0 * elapsed_time);
	velocity += front * XMVECTOR{ -1.0f, 0.0f, -1.0f }*(float)(5.0 * elapsed_time);
	position += velocity * (float)elapsed_time;
	
	XMStoreFloat3(&g_PlayerPosition, position);
	XMStoreFloat3(&g_PlayerVelocity, velocity);
}

void Player_Draw(){
	XMMATRIX t = XMMatrixTranslation(g_PlayerPosition.x, g_PlayerPosition.y+1.0f, g_PlayerPosition.z);
	XMMATRIX world=t;
	ModelDraw(g_pPlayerModel, world);

	Light_SetSpecularWorld(PlayerCamera_GetPosition(), 10.0f, { 0.3f,0.3f,0.3f,1.0f });

}

const DirectX::XMFLOAT3& Player_GetPosition(){
	return g_PlayerPosition;
}

const DirectX::XMFLOAT3& Player_GetFront(){
	return g_PlayerFront;
}
