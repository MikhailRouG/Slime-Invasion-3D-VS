#include "player_camera.h"
#include <DirectXMath.h>
#include <cmath>
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
static float       g_CameraOffsetY = 20.0f;
static float       g_CameraOffsetZ = 30.0f;
static float       g_CameraScale = 1.0f;

void PlayerCamera_Initialize(){
    g_CameraScale = 1.0f;
    g_currentCameraPosition = XMVectorSet(00.0f, 4.0f, 0.0f, 1.0f);
}

void PlayerCamera_Finalize()
{
}
void PlayerCamera_Update(double elapsed_time)
{
    XMVECTOR playerPos = XMLoadFloat3(&Player_GetPosition());
    float distBack = g_CameraOffsetZ * g_CameraScale;
    float height = g_CameraOffsetY * g_CameraScale;
    XMVECTOR fixedDirection = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMVECTOR desiredPos = playerPos - (fixedDirection * distBack);
    desiredPos = XMVectorSetY(desiredPos, XMVectorGetY(desiredPos) + height);

    // Exponential smoothing. A plain `LERP_SPEED * dt` exceeds 1.0 once the
    // frame time goes above 1/LERP_SPEED, which makes the camera overshoot
    // and jitter on slow frames; this form stays in [0,1) at any frame rate
    // and gives the same feel at 60 fps.
    float t = 1.0f - expf(-LERP_SPEED * static_cast<float>(elapsed_time));

    g_currentCameraPosition = XMVectorLerp(g_currentCameraPosition, desiredPos, t);
    g_currentCameraPosition = XMVectorSetW(g_currentCameraPosition, 1.0f);

    // Aim a little above the player's origin so they sit in the middle of
    // the frame rather than at the bottom edge.
    XMVECTOR lookAtTarget = playerPos + XMVectorSet(0, TARGET_Y_OFFSET, 0, 0);

    XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(lookAtTarget, g_currentCameraPosition));

    // When the camera sits directly overhead (offset Z == 0) the world up
    // axis is parallel to the view direction and XMMatrixLookAtLH degenerates
    // into NaNs. Fall back to +Z as the up axis in that case.
    const float verticality = fabsf(XMVectorGetY(forward));
    XMVECTOR upVector = (verticality > 0.99f) ? XMVectorSet(0, 0, 1, 0)
                                              : XMVectorSet(0, 1, 0, 0);

    // Actually look at the player. The previous code built the view from a
    // hard-coded -90 degree X rotation, i.e. it always stared straight down:
    // once the camera was pulled back (offset Z != 0) the player was no
    // longer in front of it. lookAtTarget/upVector were computed and unused.
    XMMATRIX view = XMMatrixLookAtLH(g_currentCameraPosition, lookAtTarget, upVector);

    float aspect = (float)Direct3D_GetBackBufferWidth() / (float)Direct3D_GetBackBufferHeight();
    XMMATRIX proj = XMMatrixPerspectiveFovLH(FOV_ANGLE, aspect, NEAR_Z, FAR_Z);

    XMStoreFloat3(&g_CameraPosition, g_currentCameraPosition);
    XMStoreFloat3(&g_CameraFront, forward);
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
