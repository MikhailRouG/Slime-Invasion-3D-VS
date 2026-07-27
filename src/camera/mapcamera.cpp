#include "mapcamera.h"
using namespace DirectX;

static XMFLOAT3 g_Position{};
static XMFLOAT3 g_Front{ 0.0f, 1.0f, 0.0f };
void MapCamera_Initialize()
{
}

void MapCamera_Finalize()
{
}

void MapCamera_SetPosition(const XMFLOAT3& position)
{
    g_Position = position;
}

void MapCamera_SetFront(const XMFLOAT3& front)
{
    g_Front = front;
}

DirectX::XMFLOAT4X4 MapCamera_GetViewMatrix()
{
    XMFLOAT4X4 mtxView;

    XMVECTOR eye = XMLoadFloat3(&g_Position);
    XMVECTOR front = XMLoadFloat3(&g_Front);
    XMVECTOR up = XMVectorSet(0.0f,- 1.0f, 0.0f,0.0F);

    // Compute LookAt target (eye + front)
   // XMVECTOR target = XMVectorAdd(eye, front);

    // Create matrix
    XMMATRIX view = XMMatrixLookToLH(eye, up, front);

    // Store result
    XMStoreFloat4x4(&mtxView, view);

    return mtxView;
}


DirectX::XMFLOAT4X4 MapCamera_GetPerspectiveMatrix()
{
    XMFLOAT4X4 mtxProj;

    XMMATRIX proj = XMMatrixOrthographicOffCenterLH(
        -10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 1000.0f
    );

    XMStoreFloat4x4(&mtxProj, proj);

    return mtxProj;

}
