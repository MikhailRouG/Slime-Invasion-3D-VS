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
//#include "camera.h"
#include "player_camera.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "sampler.h"
#include "meshfield.h"
#include "light.h"
#include "model.h"
#include "player.h"

static float g_x = 0.0f;
static float g_angle = 0.0f;
static float g_scale = 1.0f;
static double g_AccumulatedTime = 0.0;
static XMFLOAT3 g_CubePosition{};
static XMFLOAT3 g_CubeVelocity{};

//static MODEL* g_pModelTest = nullptr;
static MODEL* g_pModelTest2 = nullptr;
static MODEL* g_pModelTest3 = nullptr;
static MODEL* g_pModelTest4 = nullptr;
static MODEL* g_pModelTest5 = nullptr;

void Game_Initialize(){
	PlayerCamera_Initialize();
	//Camera_Initialize({10.0f,10.0f,-10.0f},{-0.6f,-0.4f,0.6f},{0.7f,0.0f,0.7f});
	//Camera_Initialize();
	//g_pModelTest = ModelLoad("resource/model/test.fbx", 0.1f);
	g_pModelTest2 = ModelLoad("resource/model/spoon.fbx", 0.2f);
	g_pModelTest3 = ModelLoad("resource/model/BOTTLE HIGH POLY.fbx", 0.01f);
	g_pModelTest4 = ModelLoad("resource/model/glass-fbx.FBX", 0.01f);
	//g_pModelTest5 = ModelLoad("resource/model/EmptyRoom(FBX).fbx", 1.0f);
	Player_Initialize({ 0.0f,0.0f,-5.0f }, { 0.0f,0.0f,1.0f });

}

void Game_Finalize(){
	PlayerCamera_Finalize();
	Player_Finalize();
	ModelRelease(g_pModelTest4);
	ModelRelease(g_pModelTest3);
	ModelRelease(g_pModelTest2);
	//Camera_Finalize();
}

void Game_Update(double elapsed_time){
	Player_Update(elapsed_time);
	PlayerCamera_Update(elapsed_time);
	//Camera_Update(elapsed_time);

	if (KeyLogger_IsTrigger(KK_ESCAPE)) {
		SendMessage(GameWindow_GetHWND(), WM_CLOSE, 0, 0);
	}

	g_AccumulatedTime += elapsed_time;
	g_x = (float)sin(g_AccumulatedTime) * 4.5f; //-4.5~4.5
	g_angle = (float)g_AccumulatedTime * 2.0f; //1秒間に90度
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

	Light_SetPointLightcount(3);
	//XMMATRIX rot = XMMatrixRotationY(g_angle);
	//XMFLOAT3 pp0,pp1,pp2;
	//XMStoreFloat3(&pp0, XMVector3Transform({ 0.0f,0.3f,-3.0f }, rot));
	//XMStoreFloat3(&pp1, XMVector3Transform({ 0.0f,0.3f,3.0f }, rot));
	//XMStoreFloat3(&pp2, XMVector3Transform({ 3.0f,0.3f,-3.0f }, rot));

	//Light_SetPointLight(0, pp0, 5.0f, {1.0f,0.0f,0.0f});
	//Light_SetPointLight(1, pp1, 5.0f, { 0.0f,1.0f,0.0f });
	//Light_SetPointLight(2, pp2, 5.0f, { 0.0f,0.0f,1.0f });

	Light_SetPointLight(0, {7.5f,10.0f,7.5f}, 3.0f, {0.5f,0.5f,0.7f});

	Light_SetSpecularWorld(PlayerCamera_GetPosition(), 5.0f, { 0.3f,0.3f,0.3f,1.0f });

	Sampler_SetFilterAnisotropic();
	
	Meshfield_Draw();

	Player_Draw();

	XMMATRIX mtxWorld1 = XMMatrixTranslation(1.0f, 5.5f, 0.0f);
	Cube_Draw(mtxWorld1);

	XMMATRIX mtxWorld2 = XMMatrixTranslation(3.0f, 0.5f, 2.0f);
	Cube_Draw(mtxWorld2);

	XMMATRIX mtxWorld3 = XMMatrixTranslation(5.0f, 1.5f, -2.0f);
	Cube_Draw(mtxWorld3);

	XMMATRIX mtxWorld4 = XMMatrixTranslation(8.0f, 3.0f, -4.0f);
	Cube_Draw(mtxWorld4);

	ModelDraw(g_pModelTest2, XMMatrixTranslation(-5.0f, 3.0f, -2.0f));
	ModelDraw(g_pModelTest3, XMMatrixTranslation(0.0f, 1.0f, 2.0f));
	ModelDraw(g_pModelTest4, XMMatrixTranslation(-5.0f, 1.0f, 0.0f));
	//ModelDraw(g_pModelTest5, XMMatrixTranslation(-10.0f, 1.0f, 5.0f));

	/*
	Light_SetSpecularWorld(Camera_GetPosition(), 50.0f, { 0.3f,0.3f,0.3f,1.0f });
	ModelDraw(g_pModelTest, XMMatrixTranslation(-2.0f, 1.0f, 0.0f));

	Light_SetSpecularWorld(Camera_GetPosition(), 50.0f, { 1.0f,0.9f,0.7f,1.0f });
	ModelDraw(g_pModelTest2, XMMatrixTranslation(-5.0f, 1.0f, 0.0f));
	ModelDraw(g_pModelTest3, XMMatrixTranslation(-8.0f, 1.0f, 0.0f));
	//ModelDraw(g_pModelTest4, XMMatrixTranslation(-11.0f, 1.0f, 0.0f));

	//Grid_Draw();
	
	Sampler_SetFilterAnisotropic();
	XMMATRIX mtxWorldShot = XMMatrixRotationY(g_angle*2.0f);
	mtxWorldShot *= XMMatrixRotationX(g_angle * 2.0f);
	mtxWorldShot *= XMMatrixTranslationFromVector(XMLoadFloat3(&g_CubePosition));
	Cube_Draw(mtxWorldShot);

	
	Sampler_SetFilterAnisotropic();
	XMMATRIX mtxWorld = XMMatrixIdentity();
	Cube_Draw(mtxWorld);

	Sampler_SetFilterLinear();
	XMMATRIX mtxWorldLinear = XMMatrixIdentity();
	mtxWorldLinear = XMMatrixTranslation(1.5f, 0.0f, 0.0f);
	Cube_Draw(mtxWorldLinear);

	Sampler_SetFilterPoint();
	XMMATRIX mtxWorldPoint = XMMatrixIdentity();
	mtxWorldPoint = XMMatrixTranslation(3.0f, 0.0f, 0.0f);
	Cube_Draw(mtxWorldPoint);
	
	
	/*float xtrans = 4.5f;
	float ztrans = 4.5f;
	XMMATRIX mtxRotate = XMMatrixRotationY(g_angle);*/

	//for (int y = 0;y < 10;y++) {
	//	for (int z = 0;z < 10 - y;z++) {
	//		for (int x = 0;x < 10 - y;x++) {
	//			//ワールド座標変換行列の作成
	//			XMMATRIX mtxOffset = XMMatrixTranslation((float)x - xtrans, (float)y, (float)z - ztrans);
	//			XMMATRIX mtxWorld = mtxOffset * mtxRotate;// *mtxTrans;

	//			/*
	//			XMMATRIX mtxTrans = XMMatrixTranslation(0.5f, 0.5f, 0.0f);//平行移動行列の作成
	//			XMMATRIX mtxTransInv = XMMatrixTranslation(-0.5f, 0.0f, 0.0f);//平行移動行列の作成
	//			XMMATRIX mtxRotate = XMMatrixRotationY(XMConvertToRadians(g_angle));//回転行列の作成
	//			XMMATRIX mtxScale = XMMatrixScaling(4.0f,0.5f,0.5f);//拡大行列の作成
	//			XMMATRIX mtxWorld = mtxTrans * mtxScale * mtxTransInv* mtxRotate;
	//			*/

	//			Cube_Draw(mtxWorld);
	//		}

	//	}
	//	xtrans -= 0.5f;
	//	ztrans -= 0.5f;
	//}
	

	//Camera_DebugDraw();
}

