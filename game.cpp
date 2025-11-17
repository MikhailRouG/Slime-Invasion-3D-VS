/*==============================================================================

   ÉQÅ[ÉÄñ{ëÃ[game.cpp]
														 Author : Harada Ren
														 Date   : 2025/06/27
--------------------------------------------------------------------------------

==============================================================================*/
#include "game.h"
#include "key_logger.h"
#include "game_window.h"
#include "cube.h"
#include "shader3d.h"
//#include "camera.h"
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

static float g_x = 0.0f;
static float g_angle = 0.0f;
static float g_scale = 1.0f;
static double g_AccumulatedTime = 0.0;
static XMFLOAT3 g_CubePosition{};
static XMFLOAT3 g_CubeVelocity{};

static int g_CubeTexId = -1;
static int g_TestTexId = -1;

static int g_AnimPatternId = -1;
static int g_AnimPlayId = -1;

//static MODEL* g_pModelTest = nullptr;
//static MODEL* g_pModelTest3 = nullptr;
static MODEL* g_pModelSpoon = nullptr;
static MODEL* g_pModelBottle = nullptr;
static MODEL* g_pModelCup = nullptr;
static MODEL* g_pModelTemple = nullptr;

void Game_Initialize(){
	PlayerCamera_Initialize();
	//Camera_Initialize({10.0f,10.0f,-10.0f},{-0.6f,-0.4f,0.6f},{0.7f,0.0f,0.7f});
	//Camera_Initialize();
	//g_pModelTest = ModelLoad("resource/model/test.fbx", 0.1f);
	g_pModelBottle = ModelLoad("resource/model/BOTTLE HIGH POLY.fbx", 0.01f);
	//g_pModelTest3 = ModelLoad("resource/model/glass-fbx.FBX", 0.01f);
	g_pModelSpoon = ModelLoad("resource/model/spoon.fbx", 0.2f);
	g_pModelCup = ModelLoad("resource/model/cup.fbx", 1.0f);
	g_pModelTemple = ModelLoad("resource/model/temple.fbx", 1.0f);
	Map_Initialize();
	Player_Initialize({ 0.0f,0.0f,-5.0f }, { 0.0f,0.0f,1.0f });
	Bullet_Initialize();
	Billboard_Initialize();
	
	g_CubeTexId = Texture_Load(L"resource/texture/BoxTestTexture2.png");
	g_TestTexId = Texture_Load(L"resource/texture/explosion.png");

	g_AnimPatternId = SpriteAnim_RegisterPattern(g_TestTexId, 7, 7, 0.2, { 300,400 }, { 0,0 });
	g_AnimPlayId = SpriteAnim_CreatePlayer(g_AnimPatternId);
}

void Game_Finalize(){
	Billboard_Finalize();
	PlayerCamera_Finalize();
	Player_Finalize();
	Bullet_Finalize();
	Map_Finalize();
	ModelRelease(g_pModelTemple);
	ModelRelease(g_pModelCup);
	ModelRelease(g_pModelBottle);
	ModelRelease(g_pModelSpoon);
	//Camera_Finalize();
}

void Game_Update(double elapsed_time){
	SpriteAnim_Update(elapsed_time);

	Player_Update(elapsed_time);
	PlayerCamera_Update(elapsed_time);
	//Camera_Update(elapsed_time);

	Bullet_Update(elapsed_time);

	//É}ÉbÉvÇ…íeÇ™ìñÇΩÇ¡ÇΩÇÁè¡Ç∑
	for (int j = 0;j < Map_GetObjectsCount();j++) {
		for (int i = 0;i < Bullet_GetCount();i++) {
			AABB bullet = Bullet_GetAABB(i);
			AABB object = Map_GetObject(j)->aabb;
			if (Collision_IsOverlapAABB(bullet, object)) {
				Bullet_Destroy(i);
				//BillboardAnim_Draw(g_AnimPlayId, , { 6.0f, 8.0f }, { 0.0f, 0.0f });
			}
		}
	}

	if (KeyLogger_IsTrigger(KK_ESCAPE)) {
		SendMessage(GameWindow_GetHWND(), WM_CLOSE, 0, 0);
	}

	g_AccumulatedTime += elapsed_time;
	g_x = (float)sin(g_AccumulatedTime) * 4.5f; //-4.5~4.5
	g_angle = (float)g_AccumulatedTime * 2.0f; //1ïbä‘Ç…90ìx
	g_scale = ((float)sin(g_AccumulatedTime) + 1.0f) * 0.5f * 5.0f; //0~5


	if (KeyLogger_IsTrigger(KK_SPACE)) {
		g_CubePosition = PlayerCamera_GetPosition();
		XMStoreFloat3(&g_CubeVelocity, XMLoadFloat3(&PlayerCamera_GetFront()) * 20.0f);
	}

	XMVECTOR cube_position = XMLoadFloat3(&g_CubePosition);
	cube_position += XMLoadFloat3(&g_CubeVelocity) * (float)elapsed_time;
	XMStoreFloat3(&g_CubePosition, cube_position);
}

void Game_Draw(){
	Light_SetAmbient({ 0.2f,0.2f,0.2f });
	XMVECTOR v{ -1.0f,-1.0f,1.0f};
	v = XMVector3Normalize(v);
	XMFLOAT4 dir;
	XMStoreFloat4(&dir, v);
	Light_SetDirectionalWorld(dir, { 0.2f,0.2f,0.2f,1.0f });

	Light_SetPointLightcount(4);

	Light_SetPointLight(0, { 10.0f,5.0f,10.0f }, 5.0f, { 0.3f,0.3f,0.9f });
	Light_SetPointLight(1, { -10.0f,5.0f,10.0f }, 5.0f, { 0.3f,0.9f,0.3f });
	Light_SetPointLight(2, { 10.0f,5.0f,-10.0f }, 5.0f, { 0.9f,0.3f,0.3f });
	Light_SetPointLight(3, { -10.0f,5.0f,-10.0f }, 5.0f, { 0.9f,0.9f,0.3f });

	Player_Draw();

	Map_Draw();

	//àÍî‘ç≈å„Ç…èëÇ©Ç»Ç¢Ç∆îwåiÇ™å©Ç¶ÇÈ
	BillboardAnim_Draw(g_AnimPlayId, { -10.0, 2.5f, -10.0f }, { 6.0f, 8.0f }, { 0.0f, 0.0f });

	Bullet_Draw();
	//Camera_DebugDraw();
}

