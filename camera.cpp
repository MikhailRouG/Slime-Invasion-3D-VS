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
#include "debug_text.h"
#include <sstream>

static XMFLOAT3 g_CameraPosition = { 15.0f,15.0f,-15.0f };
//姿勢ベクトル3つ
//これを基に移動や回転を行う
static XMFLOAT3 g_CameraVecFront = { 0.0f,0.0f,1.0f };
static XMFLOAT3 g_CameraVecRight = { 1.0f,0.0f,0.0f };
static XMFLOAT3 g_CameraVecUp = { 0.0f,1.0f,0.0f };

static constexpr float CAMERA_MOVE_SPEED = 10.0f;
static constexpr float CAMERA_ROTATION_SPEED = XMConvertToRadians(60.0f);
static XMFLOAT4X4 g_CameraMatrix{};
static XMFLOAT4X4 g_PerspectiveMatrix{};
static float g_Fov = XMConvertToRadians(60.0f);

static hal::DebugText* g_pDT = nullptr;

void Camera_Initialize(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& right){
	Camera_Initialize();

	g_CameraPosition = position;

	XMVECTOR f= XMVector3Normalize(XMLoadFloat3(&front));
	XMVECTOR r= XMVector3Normalize(XMLoadFloat3(&right) * XMVECTOR { 1.0f, 0.0f, 1.0f });
	XMVECTOR u = XMVector3Normalize(XMVector3Cross(f, r));
	XMStoreFloat3(&g_CameraVecFront, f);
	XMStoreFloat3(&g_CameraVecRight, r);
	XMStoreFloat3(&g_CameraVecUp, u);

	XMStoreFloat4x4(&g_CameraMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&g_PerspectiveMatrix, XMMatrixIdentity());
}

void Camera_Initialize(){
	g_CameraPosition = { 0.0f,10.0f,-10.0f };
	g_CameraVecFront = { 0.0f,0.0f,1.0f };
	g_CameraVecRight = { 1.0f,0.0f,0.0f };
	g_CameraVecUp = { 0.0f,1.0f,0.0f };
	g_Fov = XMConvertToRadians(60.0f);

	XMStoreFloat4x4(&g_CameraMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&g_PerspectiveMatrix, XMMatrixIdentity());

#if defined(DEBUG)||defined(_DEBUG)
	g_pDT = new hal::DebugText(Direct3D_GetDevice(), Direct3D_GetContext(),
		L"resource/texture/consolab_ascii_512.png",
		Direct3D_GetBackBufferWidth(), Direct3D_GetBackBufferHeight(),
		0.0f, 28.0f, //調整
		0, 0,
		0.0f, 14.0f); //調整
#endif
}

void Camera_Finalize(){
	delete g_pDT;
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
		front = XMVector3Normalize(front); //二つのベクトルを回転させるが一方を計算して
		up = XMVector3Normalize(XMVector3Cross(front, right)); //もう一方を外積で出す
	}
	//下を向く
	if (KeyLogger_IsPressed(KK_DOWN)) {
		//任意軸(
		XMMATRIX rotation = XMMatrixRotationAxis(right, CAMERA_ROTATION_SPEED * (float)elapsed_time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		up = XMVector3Normalize(XMVector3Cross(front, right));
	}
	//右を向く
	if (KeyLogger_IsPressed(KK_RIGHT)) {
		//任意軸(
		//XMMATRIX rotation = XMMatrixRotationAxis(up, CAMERA_ROTATION_SPEED * (float)elapsed_time);
		XMMATRIX rotation = XMMatrixRotationY(CAMERA_ROTATION_SPEED * (float)elapsed_time);//
		up= XMVector3Normalize(XMVector3TransformNormal(up, rotation));//
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		right = XMVector3Normalize(XMVector3Cross(up, front) * XMVECTOR { 1.0f, 0.0f, 1.0f });
	}
	//左を向く
	if (KeyLogger_IsPressed(KK_LEFT)) {
		//任意軸(
		//XMMATRIX rotation = XMMatrixRotationAxis(up, -CAMERA_ROTATION_SPEED * (float)elapsed_time);
		XMMATRIX rotation = XMMatrixRotationY(-CAMERA_ROTATION_SPEED * (float)elapsed_time);//
		up = XMVector3Normalize(XMVector3TransformNormal(up, rotation));//
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		right = XMVector3Normalize(XMVector3Cross(up, front) * XMVECTOR { 1.0f, 0.0f, 1.0f });
	}


	//前移動
	if (KeyLogger_IsPressed(KK_W)) {
		position += XMVector3Normalize(front * XMVECTOR{ 1.0f,0.0f,1.0f }) * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	//後移動
	if (KeyLogger_IsPressed(KK_S)) {
		position += XMVector3Normalize(-front * XMVECTOR{ 1.0f,0.0f,1.0f }) * CAMERA_MOVE_SPEED * (float)elapsed_time;;
	}
	//右移動
	if (KeyLogger_IsPressed(KK_D)) {
		position += right * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	//左移動
	if (KeyLogger_IsPressed(KK_A)) {
		position += -right * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	//上昇
	if (KeyLogger_IsPressed(KK_Q)) {
		position += XMVECTOR{ 0.0f,1.0f,0.0f }*CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	//下降
	if (KeyLogger_IsPressed(KK_E)) {
		position += XMVECTOR{ 0.0f,-1.0f,0.0f }*CAMERA_MOVE_SPEED * (float)elapsed_time;
	}

	if (KeyLogger_IsPressed(KK_Z)) {
		g_Fov -= XMConvertToRadians(10.0f) * (float)elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_C)) {
		g_Fov += XMConvertToRadians(10.0f) * (float)elapsed_time;
	}


	//更新結果の保存
	XMStoreFloat3(&g_CameraPosition,position);
	XMStoreFloat3(&g_CameraVecFront, front);
	XMStoreFloat3(&g_CameraVecUp, up);
	XMStoreFloat3(&g_CameraVecRight, right);


	//ビュー変換行列の作成
	//(カメラの座標、注視点、固定用の真上方向) LH...LeftHand
	XMMATRIX mtxView = XMMatrixLookAtLH(position, position+front, up );

	XMStoreFloat4x4(&g_CameraMatrix, mtxView);
	//頂点シェーダーにビュー変換行列を設定
	Shader3d_SetViewMatrix(mtxView);


	// 頂点シェーダーに変換行列を設定
	// パースペクティブ行列の作成
	//(カメラアングルをラジアン角で、画面の幅/高さ、カメラからスクリーンまでの距離、カメラから視錐台の端まで)
	//constexpr float fovAnglerY = XMConvertToRadians(60.0f);
	float aspextRatio = (float)Direct3D_GetBackBufferWidth() / (float)Direct3D_GetBackBufferHeight();
	float nearz = 0.1f;
	float farz = 100.0f;
	XMMATRIX mtxPerspective = XMMatrixPerspectiveFovLH(g_Fov, aspextRatio, nearz, farz);

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

const DirectX::XMFLOAT3& Camera_GetPosition(){
	return g_CameraPosition;
}

const DirectX::XMFLOAT3& Camera_GetFront(){
	return g_CameraVecFront;
}

float Camera_GetFov(){
	return 0.0f;
}

void Camera_DebugDraw(){
#if defined(DEBUG)||defined(_DEBUG)//デバッグの時だけ有効
	std::stringstream ss;
	ss << "Camera Position: x=" << g_CameraPosition.x;
	ss << " y=" << g_CameraPosition.y;
	ss << " z=" << g_CameraPosition.z << std::endl;

	ss << "Camera Front   : x=" << g_CameraVecFront.x;
	ss << " y=" << g_CameraVecFront.y;
	ss << " z=" << g_CameraVecFront.z << std::endl;

	ss << "Camera Right   : x=" << g_CameraVecRight.x;
	ss << " y=" << g_CameraVecRight.y;
	ss << " z=" << g_CameraVecRight.z << std::endl;

	ss << "Camera Up      : x=" << g_CameraVecUp.x;
	ss << " y=" << g_CameraVecUp.y;
	ss << " z=" << g_CameraVecUp.z << std::endl;

	ss << "Camera Fov     : " << g_Fov << std::endl;


	g_pDT->SetText(ss.str().c_str(),{0.0f,1.0f,0.0f,1.0f});

	g_pDT->Draw();
	g_pDT->Clear();
#endif
}
