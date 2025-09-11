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

void Camera_Initialize()
{
}

void Camera_Finalize()
{
}

void Camera_Update(){
	//ビュー変換行列の作成
	//(カメラの座標、視点、固定用の真上方向)
	XMMATRIX mtxView = XMMatrixLookAtLH({ 2.0f,2.0f,-5.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f });
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
