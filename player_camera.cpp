#include "player_camera.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "direct3d.h"
#include "player.h"

static const float FOV_ANGLE = XM_PIDIV4;
static const float NEAR_Z = 0.1f;
static const float FAR_Z = 200.0f;
static const float LERP_SPEED = 5.0f;
static const float TARGET_Y_OFFSET = 2.0f;

static XMFLOAT3    g_CameraFront = { 0.0f, 0.0f, 1.0f };
static XMFLOAT3    g_CameraPosition = { 0.0f, 0.0f, 0.0f };
static XMFLOAT4X4  g_CameraViewMatrix = {};
static XMFLOAT4X4  g_CameraPerspectiveMatrix = {};

static XMVECTOR    g_currentCameraPosition;
static float       g_CameraOffsetY = 40.0f;
static float       g_CameraOffsetZ = 0.0f;
static float       g_CameraScale = 1.0f;

static bool canRotate = false;

void PlayerCamera_Initialize(){
    g_CameraScale = 1.0f;
    g_currentCameraPosition = XMVectorSet(0.0f, 4.0f, 0.0f, 1.0f);
}

void PlayerCamera_Finalize()
{
}
void PlayerCamera_Update(double elapsed_time)
{
    XMVECTOR playerPos = XMLoadFloat3(&Player_GetPosition());
    XMVECTOR playerFront;
    playerFront = { 0.0f,0.0f ,1.0f,0.0f };
    //if(canRotate == true)
    //playerFront = XMVector3Normalize(XMLoadFloat3(&Player_GetFront()));
    //else
    //{
    //    playerFront = {0.0f,0.0f ,1.0f,0.0f };
    //}
    float distBack = g_CameraOffsetZ * g_CameraScale;
    float height = g_CameraOffsetY * g_CameraScale;
    XMVECTOR fixedDirection = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMVECTOR desiredPos = playerPos - (fixedDirection * distBack);
    desiredPos = XMVectorSetY(desiredPos, XMVectorGetY(desiredPos) + height);

    float t = static_cast<float>(LERP_SPEED * elapsed_time);

    g_currentCameraPosition = XMVectorLerp(g_currentCameraPosition, desiredPos, t);
    g_currentCameraPosition = XMVectorSetW(g_currentCameraPosition, 1.0f);

    XMVECTOR lookAtTarget = playerPos + XMVectorSet(0, TARGET_Y_OFFSET, 0, 0);
    XMVECTOR upVector = XMVectorSet(0, 1, 0, 0);

    float tiltAngle = XMConvertToRadians(-90.0f); 
    XMMATRIX rotation = XMMatrixRotationX(tiltAngle);
    XMMATRIX translation = XMMatrixTranslationFromVector(XMVectorNegate(g_currentCameraPosition));
    XMMATRIX view = translation * rotation;
    float aspect = (float)Direct3D_GetBackBufferWidth() / (float)Direct3D_GetBackBufferHeight();
    XMMATRIX proj = XMMatrixPerspectiveFovLH(FOV_ANGLE, aspect, NEAR_Z, FAR_Z);

    XMStoreFloat3(&g_CameraPosition, g_currentCameraPosition);
    XMStoreFloat4x4(&g_CameraViewMatrix, view);
    XMStoreFloat4x4(&g_CameraPerspectiveMatrix, proj);
}

void PlayerCamera_SetScale(float scale)
{
    g_CameraScale = scale;
}


const DirectX::XMFLOAT3& PlayerCamera_GetFront(){
	return g_CameraFront;
}

const DirectX::XMFLOAT3& PlayerCamera_GetPosition(){
	return g_CameraPosition;
}

const DirectX::XMFLOAT4X4& PlayerCamera_GetViewMatrix(){
	return g_CameraViewMatrix;
}

const DirectX::XMFLOAT4X4& PlayerCamera_GetPerspectiveMatrix()
{
	return g_CameraPerspectiveMatrix;
}
