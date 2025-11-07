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
}

void Player_Finalize(){
	ModelRelease(g_pPlayerModel);
}

void Player_Update(double elapsed_time){
	XMVECTOR position = XMLoadFloat3(&g_PlayerPosition);
	XMVECTOR velocity = XMLoadFloat3(&g_PlayerVelocity);
	XMVECTOR gvelocity{};

	//ジャンプ
	if (KeyLogger_IsTrigger(KK_SPACE) && !g_IsJump) {
		// Y方向の速度をジャンプ力で上書き
		velocity += { 0.0f,50.0f,0.0f };
		g_IsJump = true;
	}


	// 重力
	XMFLOAT3 gdir{ 0.0f, 1.0f , 0.0f};
	float gravity = -9.8f * 15.0f * (float)elapsed_time;
	velocity += XMLoadFloat3(&gdir) * gravity;



	// -----当たり判定-----
	// 横方向に移動して壁との当たり判定
	XMVECTOR horizontal_move = XMVectorSet(XMVectorGetX(velocity) * (float)elapsed_time, 0.0f, XMVectorGetZ(velocity) * (float)elapsed_time, 0.0f);
	position += horizontal_move;
	XMStoreFloat3(&g_PlayerPosition, position);

	AABB player = Player_GetAABB();
	AABB cube = Cube_GetAABB({ 3.0f, 0.5f, 2.0f });
	Hit hit = Collision_IsHitAABB(cube, player);

	if (Collision_IsOverlapAABB(player, cube)) {
		position -= horizontal_move;
		velocity *= {0.0f, 1.0f, 0.0f};
		XMStoreFloat3(&g_PlayerPosition, position);		
	}
	else {
		if (XMVectorGetY(velocity) > 0.0f) {
			g_IsJump = true;
		}
	}

	// 縦方向に移動して地面/天井との当たり判定
	XMVECTOR vertical_move = XMVectorSet(0.0f, XMVectorGetY(velocity) * (float)elapsed_time, 0.0f, 0.0f);
	position += vertical_move;
	XMStoreFloat3(&g_PlayerPosition, position);

	player = Player_GetAABB();

	hit = Collision_IsHitAABB(cube, player);
	if (Collision_IsOverlapAABB(player, cube)) {
	//if (hit.isHit) {
		//if (hit.normal.y > 0.0f) {
			position -= vertical_move;
			//position = XMVectorSetY(position, cube.max.y);

			// 落下中に地面にぶつかったら着地とみなす
			if (XMVectorGetY(velocity) > 0) {
				g_IsJump = false;
			}
			velocity *= {1.0f, 0.0f, 1.0f}; // 天井か地面にぶつかったら縦方向の速度を0に
			XMStoreFloat3(&g_PlayerPosition, position);
		//}
	}

	//地面より低かったら着地
	if (XMVectorGetY(position) < 0.0f) {
		position -= vertical_move;
		velocity *= {1.0f, 0.0f, 1.0f};
		g_IsJump = false;
	}

	// 最終的な速度と位置を保存
	XMStoreFloat3(&g_PlayerVelocity, velocity);
	XMStoreFloat3(&g_PlayerPosition, position);

	/*
	//重力
	XMVECTOR gdir{ 0.0f, 1.0f ,0.0f };
	velocity += gdir * -9.8f * 10.0f * (float)elapsed_time;
	gvelocity = velocity * (float)elapsed_time;
	//position += velocity * (float)elapsed_time;
	position += gvelocity;

	XMStoreFloat3(&g_PlayerPosition, position);
	//XMStoreFloat3(&g_PlayerVelocity, velocity);

	//当たり判定
	AABB player = Player_GetAABB();
	AABB cube = Cube_GetAABB({ 3.0f, 4.0f, 2.0f });
	//重力により物体と当たったか
	if (Collision_IsOverlapAABB(player, cube)) {
		if (XMVectorGetY(velocity) < 0.0f) {
			position -= gvelocity;
			//dgvelocity = {};
			velocity *= {1.0f, 0.0f, 1.0f};
			g_IsJump = false;
		}
	}
	//地面に当たったか
	else if (XMVectorGetY(position) < 0.0f) {
		position -= gvelocity;
		//gvelocity = {};
		velocity *= {1.0f, 0.0f, 1.0f};
		g_IsJump = false;
	}
	*/

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

	//摩擦
	velocity += -velocity * (float)(4.0 * elapsed_time);

	XMStoreFloat3(&g_PlayerPosition, position);
	XMStoreFloat3(&g_PlayerVelocity, velocity);

	/*
	if (hit.isHit) {
		if (hit.normal.x > 0.0f) {
			position = XMVectorSetX(position, cube.max.x + 1.0f);
			XMStoreFloat3(&g_PlayerVelocity, velocity * XMVECTOR{ 0.0f,1.0f,1.0f });
		}
		else if (hit.normal.x < 0.0f) {
			position = XMVectorSetX(position, cube.min.x - 1.0f);
			XMStoreFloat3(&g_PlayerVelocity, velocity * XMVECTOR{ 0.0f,1.0f,1.0f });
		}
	}

	XMStoreFloat3(&g_PlayerPosition, position);
	XMStoreFloat3(&g_PlayerVelocity, velocity);
	*/

	/*
	player = Player_GetAABB();
	
	if (Collision_IsOverlapAABB(player, cube)) {
		position -= velocity * (float)elapsed_time - gvelocity;
		velocity = { 0.0f,0.0f,0.0f };
		XMStoreFloat3(&g_PlayerPosition, position);
		XMStoreFloat3(&g_PlayerVelocity, velocity);
	}
	*/
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
