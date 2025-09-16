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
static XMFLOAT3 g_CameraVecFront = { 0.0f,0.0f,1.0f };
static XMFLOAT3 g_CameraVecUp = { 0.0f,1.0f,0.0f };
static XMFLOAT3 g_CameraVecRight = { 1.0f,0.0f,0.0f };
static constexpr float CAMERA_MOVE_SPEED = 10.0f;
static constexpr float CAMERA_ROTATION_SPEED = XMConvertToRadians(30.0f);
static XMFLOAT4X4 g_CameraMatrix{};
static XMFLOAT4X4 g_PerspectiveMatrix{};


void Camera_Initialize(){
	g_CameraPosition = { 0.0f,10.0f,-10.0f };
	g_CameraVecFront = { 0.0f,0.0f,1.0f };
	g_CameraVecUp = { 0.0f,1.0f,0.0f };
	g_CameraVecRight = { 1.0f,0.0f,0.0f };

	XMStoreFloat4x4(&g_CameraMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&g_PerspectiveMatrix, XMMatrixIdentity());

}

void Camera_Finalize(){
}

void Camera_Update(double elapsed_time){
	XMVECTOR front = XMLoadFloat3(&g_CameraVecFront);
	XMVECTOR up = XMLoadFloat3(&g_CameraVecUp);
	XMVECTOR right = XMLoadFloat3(&g_CameraVecRight);
	XMVECTOR position = XMLoadFloat3(&g_CameraPosition);

	//上を向く
	if (KeyLogger_IsPressed(KK_UP)) {
		//任意軸(
		XMMATRIX rotation = XMMatrixRotationAxis(right, -CAMERA_ROTATION_SPEED * (float)elapsed_time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		up = XMVector3Cross(front, right);
	}
	//下を向く
	if (KeyLogger_IsPressed(KK_DOWN)) {
		//任意軸(
		XMMATRIX rotation = XMMatrixRotationAxis(right, CAMERA_ROTATION_SPEED * (float)elapsed_time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		up = XMVector3Cross(front, right);
	}
	//右を向く
	if (KeyLogger_IsPressed(KK_RIGHT)) {
		//任意軸(
		XMMATRIX rotation = XMMatrixRotationAxis(up, -CAMERA_ROTATION_SPEED * (float)elapsed_time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		right = XMVector3Cross(front, up);
	}
	//左を向く
	if (KeyLogger_IsPressed(KK_LEFT)) {
		//任意軸(
		XMMATRIX rotation = XMMatrixRotationAxis(up, CAMERA_ROTATION_SPEED * (float)elapsed_time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		up = XMVector3Cross(front, right);
	}


	//前
	if (KeyLogger_IsPressed(KK_W)) {
		position += front * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	//後
	if (KeyLogger_IsPressed(KK_S)) {
		position += -front * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	//右
	if (KeyLogger_IsPressed(KK_D)) {
		position += right * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	//左
	if (KeyLogger_IsPressed(KK_A)) {
		position += -right * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}

	//更新結果の保存
	XMStoreFloat3(&g_CameraPosition,position);
	XMStoreFloat3(&g_CameraVecFront, front);
	XMStoreFloat3(&g_CameraVecUp, up);
	XMStoreFloat3(&g_CameraVecRight, right);


	//ビュー変換行列の作成
	//(カメラの座標、注視点、固定用の真上方向)
	XMMATRIX mtxView = XMMatrixLookAtLH(position, position+front, up );

	XMStoreFloat4x4(&g_CameraMatrix, mtxView);
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

	XMStoreFloat4x4(&g_PerspectiveMatrix, mtxPerspective);
	//頂点シェーダーにプロジェクション変換行列を設定
	Shader3d_SetProjectionMatrix(mtxPerspective);
}

const DirectX::XMFLOAT4X4& Camera_GetMatrix(){
	return g_CameraMatrix;
}

const DirectX::XMFLOAT4X4& Camera_GetPerspectiveMatrix(){
	return g_PerspectiveMatrix;
}
