#include "camera.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "key_logger.h"
#include "debug_text.h"
#include <sstream>

static XMFLOAT3 g_CameraPosition = { 15.0f,15.0f,-15.0f };
static XMFLOAT3 g_CameraVecFront = { 0.0f,0.0f,1.0f };
static XMFLOAT3 g_CameraVecRight = { 1.0f,0.0f,0.0f };
static XMFLOAT3 g_CameraVecUp = { 0.0f,1.0f,0.0f };

static constexpr float CAMERA_MOVE_SPEED = 10.0f;
static constexpr float CAMERA_ROTATION_SPEED = XMConvertToRadians(60.0f);
static XMFLOAT4X4 g_CameraViewMatrix{};
static XMFLOAT4X4 g_CameraPerspectiveMatrix{};
static float g_Fov = XMConvertToRadians(60.0f);

static hal::DebugText* g_pDT = nullptr;

static ID3D11Buffer* g_pVSConstantBuffer1 = nullptr;
static ID3D11Buffer* g_pVSConstantBuffer2 = nullptr;

void Camera_Initialize(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& right){
	Camera_Initialize();

	g_CameraPosition = position;

	XMVECTOR f= XMVector3Normalize(XMLoadFloat3(&front));
	XMVECTOR r= XMVector3Normalize(XMLoadFloat3(&right) * XMVECTOR { 1.0f, 0.0f, 1.0f });
	XMVECTOR u = XMVector3Normalize(XMVector3Cross(f, r));
	XMStoreFloat3(&g_CameraVecFront, f);
	XMStoreFloat3(&g_CameraVecRight, r);
	XMStoreFloat3(&g_CameraVecUp, u);

	XMStoreFloat4x4(&g_CameraViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&g_CameraPerspectiveMatrix, XMMatrixIdentity());

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(XMFLOAT4X4);
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Direct3D_GetDevice()->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer1);
	Direct3D_GetDevice()->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer2);
}

void Camera_Initialize(){
	g_CameraPosition = { 0.0f,10.0f,-10.0f };
	g_CameraVecFront = { 0.0f,0.0f,1.0f };
	g_CameraVecRight = { 1.0f,0.0f,0.0f };
	g_CameraVecUp = { 0.0f,1.0f,0.0f };
	g_Fov = XMConvertToRadians(60.0f);

	XMStoreFloat4x4(&g_CameraViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&g_CameraPerspectiveMatrix, XMMatrixIdentity());

}

void Camera_Finalize(){
	delete g_pDT;
	SAFE_RELEASE(g_pVSConstantBuffer2);
	SAFE_RELEASE(g_pVSConstantBuffer1);
}

void Camera_Update(double elapsed_time){
	XMVECTOR front = XMLoadFloat3(&g_CameraVecFront);
	XMVECTOR up = XMLoadFloat3(&g_CameraVecUp);
	XMVECTOR right = XMLoadFloat3(&g_CameraVecRight);
	XMVECTOR position = XMLoadFloat3(&g_CameraPosition);

	if (KeyLogger_IsPressed(KK_UP)) {
		XMMATRIX rotation = XMMatrixRotationAxis(right, -CAMERA_ROTATION_SPEED * (float)elapsed_time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		up = XMVector3Normalize(XMVector3Cross(front, right));
	}
	if (KeyLogger_IsPressed(KK_DOWN)) {
		XMMATRIX rotation = XMMatrixRotationAxis(right, CAMERA_ROTATION_SPEED * (float)elapsed_time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		up = XMVector3Normalize(XMVector3Cross(front, right));
	}
	if (KeyLogger_IsPressed(KK_RIGHT)) {
		XMMATRIX rotation = XMMatrixRotationY(CAMERA_ROTATION_SPEED * (float)elapsed_time);//
		up= XMVector3Normalize(XMVector3TransformNormal(up, rotation));//
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		right = XMVector3Normalize(XMVector3Cross(up, front) * XMVECTOR { 1.0f, 0.0f, 1.0f });
	}
	if (KeyLogger_IsPressed(KK_LEFT)) {
		XMMATRIX rotation = XMMatrixRotationY(-CAMERA_ROTATION_SPEED * (float)elapsed_time);//
		up = XMVector3Normalize(XMVector3TransformNormal(up, rotation));//
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		right = XMVector3Normalize(XMVector3Cross(up, front) * XMVECTOR { 1.0f, 0.0f, 1.0f });
	}


	if (KeyLogger_IsPressed(KK_W)) {
		position += XMVector3Normalize(front * XMVECTOR{ 1.0f,0.0f,1.0f }) * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_S)) {
		position += XMVector3Normalize(-front * XMVECTOR{ 1.0f,0.0f,1.0f }) * CAMERA_MOVE_SPEED * (float)elapsed_time;;
	}
	if (KeyLogger_IsPressed(KK_D)) {
		position += right * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_A)) {
		position += -right * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_Q)) {
		position += XMVECTOR{ 0.0f,1.0f,0.0f }*CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_E)) {
		position += XMVECTOR{ 0.0f,-1.0f,0.0f }*CAMERA_MOVE_SPEED * (float)elapsed_time;
	}

	if (KeyLogger_IsPressed(KK_Z)) {
		g_Fov -= XMConvertToRadians(10.0f) * (float)elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_C)) {
		g_Fov += XMConvertToRadians(10.0f) * (float)elapsed_time;
	}


	XMStoreFloat3(&g_CameraPosition,position);
	XMStoreFloat3(&g_CameraVecFront, front);
	XMStoreFloat3(&g_CameraVecUp, up);
	XMStoreFloat3(&g_CameraVecRight, right);


	XMMATRIX mtxView = XMMatrixLookAtLH(position, position+front, up );

	XMStoreFloat4x4(&g_CameraViewMatrix, mtxView);

	float aspextRatio = (float)Direct3D_GetBackBufferWidth() / (float)Direct3D_GetBackBufferHeight();
	float nearz = 0.1f;
	float farz = 100.0f;
	XMMATRIX mtxPerspective = XMMatrixPerspectiveFovLH(g_Fov, aspextRatio, nearz, farz);

	XMStoreFloat4x4(&g_CameraPerspectiveMatrix, mtxPerspective);
}

const DirectX::XMFLOAT4X4& Camera_GetViewMatrix(){
	return g_CameraViewMatrix;
}

const DirectX::XMFLOAT4X4& Camera_GetPerspectiveMatrix(){
	return g_CameraPerspectiveMatrix;
}

const DirectX::XMFLOAT3& Camera_GetPosition(){
	return g_CameraPosition;
}

const DirectX::XMFLOAT3& Camera_GetFront(){
	return g_CameraVecFront;
}

float Camera_GetFov(){
	return g_Fov;
}

void Camera_SetMatrix(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection){
	XMFLOAT4X4 v, p{};
	XMStoreFloat4x4(&v, XMMatrixTranspose(view));
	XMStoreFloat4x4(&p, XMMatrixTranspose(projection));;

	Direct3D_GetContext()->UpdateSubresource(g_pVSConstantBuffer1, 0, nullptr, &v, 0, 0);
	Direct3D_GetContext()->UpdateSubresource(g_pVSConstantBuffer2, 0, nullptr, &p, 0, 0);
	Direct3D_GetContext()->VSSetConstantBuffers(1, 1, &g_pVSConstantBuffer1);
	Direct3D_GetContext()->VSSetConstantBuffers(2, 1, &g_pVSConstantBuffer2);
}

void Camera_DebugDraw(){
}
