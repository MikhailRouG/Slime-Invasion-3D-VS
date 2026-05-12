#include "player_camera.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "direct3d.h"
#include "player.h"


static XMFLOAT3 g_CameraFront = { 0.0f,0.0f,1.0f };
static XMFLOAT3 g_CameraPosition{ 0.0f,0.0f,0.0f };
static XMFLOAT4X4 g_CameraViewMatrix{};
static XMFLOAT4X4 g_CameraPerspectiveMatrix{};

XMVECTOR g_currentCameraPosition;
float g_CameraScale;
void PlayerCamera_Initialize(){
    g_CameraScale = 1;
    g_currentCameraPosition = XMVectorSet(0.0f, 4.0f, -8.0f, 1.0f);
}

void PlayerCamera_Finalize()
{
}
void PlayerCamera_Update(double elapsed_time)
{
    XMVECTOR pos = XMLoadFloat3(&Player_GetPosition());
    XMVECTOR playerFront = XMLoadFloat3(&Player_GetFront());
    playerFront = XMVector3Normalize(playerFront);

    const float DIST_BACK = 8.0f * g_CameraScale;
    const float HEIGHT = 4.0f * g_CameraScale;

    XMVECTOR cameraPos =
        pos
        - playerFront * DIST_BACK
        + XMVectorSet(0, HEIGHT, 0, 0);

    XMVECTOR target = pos + XMVectorSet(0, 2, 0, 0);

    g_currentCameraPosition =
        XMVectorLerp(g_currentCameraPosition, cameraPos, 5.0f * elapsed_time);

    g_currentCameraPosition = XMVectorSetW(g_currentCameraPosition, 1.0f);

    XMStoreFloat3(&g_CameraPosition, g_currentCameraPosition);

    XMMATRIX view = XMMatrixLookAtLH(
        g_currentCameraPosition,
        target,
        XMVectorSet(0, 1, 0, 0)
    );

    XMStoreFloat4x4(&g_CameraViewMatrix, view);

    float aspect =
        (float)Direct3D_GetBackBufferWidth() /
        (float)Direct3D_GetBackBufferHeight();

    XMMATRIX proj = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,
        aspect,
        0.1f,
        200.0f
    );

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
