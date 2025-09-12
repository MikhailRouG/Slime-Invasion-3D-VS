/*==============================================================================

   カメラ制御 [camera.cpp]
														 Author : Harada Ren
														 Date   : 2025/09/11
--------------------------------------------------------------------------------

==============================================================================*/
#include "camera.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "shader3d.h"
#include "key_logger.h"

static XMFLOAT3 g_CameraPosition = { 15.0f,15.0f,-15.0f };

void Camera_Initialize()
{
}

void Camera_Finalize()
{
}

void Camera_Update(){
	if (KeyLogger_IsPressed(KK_W)) {
		g_CameraPosition.z += 0.2f;
	}
	if (KeyLogger_IsPressed(KK_S)) {
		g_CameraPosition.z -= 0.2f;
	}
	if (KeyLogger_IsPressed(KK_D)) {
		g_CameraPosition.x += 0.2f;
	}
	if (KeyLogger_IsPressed(KK_A)) {
		g_CameraPosition.z -= 0.2f;
	}

	XMVECTOR eyePosition = XMLoadFloat3(&g_CameraPosition);

	//ビュー変換行列の作成
	//(カメラの座標、注視点、固定用の真上方向)
	XMMATRIX mtxView = XMMatrixLookAtLH(eyePosition, { 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f });
	//頂点シェーダーにビュー変換行列を設定
	Shader3d_SetViewMatrix(mtxView);


	// 頂点シェーダーに変換行列を設定
	// パースペクティブ行列の作成
	//(カメラアングルをラジアン角で、画面の幅/高さ、カメラからスクリーンまでの距離、カメラから視錐台の端まで)
	constexpr float fovAnglerY = XMConvertToRadians(60.0f);
	float aspextRatio = (float)Direct3D_GetBackBufferWidth() / (float)Direct3D_GetBackBufferHeight();
	float nearz = 0.1f;
	float farz = 100.0f;
	XMMATRIX mtxPerspective = XMMatrixPerspectiveFovLH(fovAnglerY, aspextRatio, nearz, farz);

	//頂点シェーダーにプロジェクション変換行列を設定
	Shader3d_SetProjectionMatrix(mtxPerspective);
}
