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
#include "grid.h"
#include "camera.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "sampler.h"
#include "meshfield.h"

static float g_x = 0.0f;
static float g_angle = 0.0f;
static float g_scale = 1.0f;
static double g_AccumulatedTime = 0.0;
static XMFLOAT3 g_CubePosition{};
static XMFLOAT3 g_CubeVelocity{};


void Game_Initialize(){
	Camera_Initialize({10.0f,10.0f,-10.0f},{-0.6f,-0.4f,0.6f},{0.7f,0.0f,0.7f});
	//Camera_Initialize();
}

void Game_Finalize(){
	Camera_Finalize();
}

void Game_Update(double elapsed_time){
	Camera_Update(elapsed_time);

	if (KeyLogger_IsTrigger(KK_ESCAPE)) {
		SendMessage(GameWindow_GetHWND(), WM_CLOSE, 0, 0);
	}

	g_AccumulatedTime += elapsed_time;
	g_x = (float)sin(g_AccumulatedTime) * 4.5f; //-4.5~4.5
	g_angle = (float)g_AccumulatedTime * 2.0f; //1秒間に90度
	g_scale = ((float)sin(g_AccumulatedTime) + 1.0f) * 0.5f * 5.0f; //0~5


	if (KeyLogger_IsTrigger(KK_SPACE)) {
		g_CubePosition = Camera_GetPosition();
		XMStoreFloat3(&g_CubeVelocity, XMLoadFloat3(&Camera_GetFront()) * 20.0f);
	}

	XMVECTOR cube_position = XMLoadFloat3(&g_CubePosition);
	cube_position += XMLoadFloat3(&g_CubeVelocity) * (float)elapsed_time;
	XMStoreFloat3(&g_CubePosition, cube_position);
}

void Game_Draw(){
	Grid_Draw();
	
	Sampler_SetFilterAnisotropic();
	XMMATRIX mtxWorldShot = XMMatrixRotationY(g_angle*2.0f);
	mtxWorldShot *= XMMatrixRotationX(g_angle * 2.0f);
	mtxWorldShot *= XMMatrixTranslationFromVector(XMLoadFloat3(&g_CubePosition));
	Cube_Draw(mtxWorldShot);

	/*
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
	*/

	Meshfield_Draw();

	
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
	

	Camera_DebugDraw();
}

