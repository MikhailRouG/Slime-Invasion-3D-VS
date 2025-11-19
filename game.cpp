/*==============================================================================

   ゲーム本体[game.cpp]
														 Author : Harada Ren
														 Date   : 2025/06/27
--------------------------------------------------------------------------------

==============================================================================*/
#include "game.h"
#include "key_logger.h"
#include "game_window.h"
#include "cube.h"
#include "shader3d.h"
#include "camera.h"
#include "player_camera.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "sampler.h"
#include "meshfield.h"
#include "light.h"
#include "model.h"
#include "player.h"
#include "map.h"
#include "texture.h"
#include "bullet.h"
#include "billboard.h"
#include "sprite_anim.h"
#include "bullet_hit_effect.h"


static int g_AnimPatternId = -1;
static int g_AnimPlayId = -1;

static bool g_IsDebug = false;

void Game_Initialize(){
	PlayerCamera_Initialize();
	Camera_Initialize({10.0f,10.0f,-10.0f},{-0.6f,-0.4f,0.6f},{0.7f,0.0f,0.7f});
	Map_Initialize();
	Player_Initialize({ 0.0f,0.0f,-5.0f }, { 0.0f,0.0f,1.0f });
	Bullet_Initialize();
	Billboard_Initialize();

	BulletHitEffect_Initialize();

	g_IsDebug = false;
}

void Game_Finalize(){
	Billboard_Finalize();
	PlayerCamera_Finalize();
	Camera_Finalize();
	Player_Finalize();
	Bullet_Finalize();
	Map_Finalize();
}

void Game_Update(double elapsed_time){
	Player_Update(elapsed_time);

	if (g_IsDebug) {
		Camera_Update(elapsed_time);
	}
	else {
		PlayerCamera_Update(elapsed_time);
	}

	Bullet_Update(elapsed_time);

	//マップに弾が当たったら消す
	for (int j = 0;j < Map_GetObjectsCount();j++) {
		for (int i = 0;i < Bullet_GetCount();i++) {
			AABB bullet = Bullet_GetAABB(i);
			AABB object = Map_GetObject(j)->aabb;
			if (Collision_IsOverlapAABB(bullet, object)) {
				BukketHitEffect_Create(Bullet_GetPosition(i));
				Bullet_Destroy(i);
			}
		}
	}

	SpriteAnim_Update(elapsed_time);
	BulletHitEffect_Update();

	if (KeyLogger_IsTrigger(KK_ESCAPE)) {
		SendMessage(GameWindow_GetHWND(), WM_CLOSE, 0, 0);
	}

	if (KeyLogger_IsTrigger(KK_L)) {
		g_IsDebug = !g_IsDebug;
	}
}

void Game_Draw(){
	//トレイル(剣とかの軌跡)
	XMFLOAT4X4 mtxView = g_IsDebug ? Camera_GetViewMatrix() : PlayerCamera_GetViewMatrix();
	XMMATRIX view = XMLoadFloat4x4(&mtxView);
	XMMATRIX proj = g_IsDebug ? XMLoadFloat4x4(&Camera_GetPerspectiveMatrix()) : XMLoadFloat4x4(&PlayerCamera_GetPerspectiveMatrix());
	XMFLOAT3 camera_position = g_IsDebug ? Camera_GetPosition() : PlayerCamera_GetPosition();
	//カメラに関する行列をシェーダーに設定
	Camera_SetMatrix(view, proj);

	//ビルボードにカメラの行列を設定する
	Billboard_SetViewMatrix(mtxView);

	//サンプラー設定
	Sampler_SetFilterAnisotropic();

	//ライト設定
	Light_SetAmbient({ 0.2f,0.2f,0.2f });
	XMVECTOR v{ -1.0f,-1.0f,1.0f};
	v = XMVector3Normalize(v);
	XMFLOAT4 dir;
	XMStoreFloat4(&dir, v);
	Light_SetDirectionalWorld(dir, { 0.2f,0.2f,0.2f,1.0f });

	Light_SetSpecularWorld(camera_position, 50.0f, { 0.8f,0.8f,0.8f,1.0f });
	/*
	Light_SetPointLightcount(4);
	Light_SetPointLight(0, { 10.0f,5.0f,10.0f }, 5.0f, { 0.3f,0.3f,0.9f });
	Light_SetPointLight(1, { -10.0f,5.0f,10.0f }, 5.0f, { 0.3f,0.9f,0.3f });
	Light_SetPointLight(2, { 10.0f,5.0f,-10.0f }, 5.0f, { 0.9f,0.3f,0.3f });
	Light_SetPointLight(3, { -10.0f,5.0f,-10.0f }, 5.0f, { 0.9f,0.9f,0.3f });
	*/
	Player_Draw();

	Map_Draw();

	Bullet_Draw();

	if (g_IsDebug) {
		Camera_DebugDraw();
	}

	BulletHitEffect_Draw();
}

