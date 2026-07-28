#include "billboard.h"
#include "shader_billboard.h"
#include "direct3d.h"
#include "player_camera.h"
#include "texture.h"
#include "sampler.h"

using namespace DirectX;

static ID3D11Buffer* g_pVertexBuffer = nullptr;

void Billboard_Initialize() {
    ShaderBillboard_Initialize();

    Vertex3d vertex[] = {
        { {-0.5f,  0.5f, 0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.0f, 0.0f} },
        { { 0.5f,  0.5f, 0.0f}, {1.0f,1.0f,1.0f,1.0f}, {1.0f, 0.0f} },
        { {-0.5f, -0.5f, 0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.0f, 1.0f} },
        { { 0.5f, -0.5f, 0.0f}, {1.0f,1.0f,1.0f,1.0f}, {1.0f, 1.0f} }
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex3d) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA sd = { vertex };
    Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);
}

void Billboard_Draw(int texId, const XMFLOAT3& position, const XMFLOAT2& scale, const XMFLOAT4& color, const XMFLOAT2& pivot) {
    if (g_pVertexBuffer == nullptr) return;

    auto ctx = Direct3D_GetContext();

    XMMATRIX view = XMLoadFloat4x4(&PlayerCamera_GetViewMatrix());
    XMMATRIX proj = XMLoadFloat4x4(&PlayerCamera_GetPerspectiveMatrix());

    XMMATRIX invView = XMMatrixInverse(nullptr, view);
    invView.r[3] = XMVectorSet(0, 0, 0, 1);

    XMMATRIX mWorld = XMMatrixScaling(scale.x, scale.y, 1.0f) * XMMatrixTranslation(-pivot.x, -pivot.y, 0.0f) * invView * XMMatrixTranslation(position.x, position.y, position.z);

    BillboardVSConstant cb;
    XMStoreFloat4x4(&cb.world, XMMatrixTranspose(mWorld));
    XMStoreFloat4x4(&cb.view, XMMatrixTranspose(view));
    XMStoreFloat4x4(&cb.proj, XMMatrixTranspose(proj));
    cb.scale = { 1.0f, 1.0f };
    cb.translation = { 0.0f, 0.0f };

    ShaderBillboard_Begin();
    ShaderBillboard_SetVSConstant(cb);
    ShaderBillboard_SetColor(color);

    Texture_SetTexture(texId);
    Sampler_SetFilterLinear();

    UINT stride = sizeof(Vertex3d);
    UINT offset = 0;
    ctx->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    ctx->Draw(4, 0);
}

void Billboard_Draw(int texId, const XMFLOAT3& position, const XMFLOAT2& scale, const XMFLOAT4& color, const XMFLOAT2& pivot, const XMFLOAT2& uvScale, const XMFLOAT2& uvOffset)
{
    if (g_pVertexBuffer == nullptr) return;

    auto ctx = Direct3D_GetContext();

    XMMATRIX view = XMLoadFloat4x4(&PlayerCamera_GetViewMatrix());
    XMMATRIX proj = XMLoadFloat4x4(&PlayerCamera_GetPerspectiveMatrix());

    XMMATRIX invView = XMMatrixInverse(nullptr, view);
    invView.r[3] = XMVectorSet(0, 0, 0, 1);

    XMMATRIX mWorld = XMMatrixScaling(scale.x, scale.y, 1.0f) * XMMatrixTranslation(-pivot.x, -pivot.y, 0.0f) * invView * XMMatrixTranslation(position.x, position.y, position.z);

    BillboardVSConstant cb;
    XMStoreFloat4x4(&cb.world, XMMatrixTranspose(mWorld)); 
    XMStoreFloat4x4(&cb.view, XMMatrixTranspose(view));
    XMStoreFloat4x4(&cb.proj, XMMatrixTranspose(proj));

    cb.scale = uvScale;
    cb.translation = uvOffset;

    ShaderBillboard_Begin();
    ShaderBillboard_SetVSConstant(cb);
    ShaderBillboard_SetColor(color);

    Texture_SetTexture(texId);
    Sampler_SetFilterLinear();

    UINT stride = sizeof(Vertex3d);
    UINT offset = 0;
    ctx->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    ctx->Draw(4, 0);
}

void Billboard_Finalize() {
    if (g_pVertexBuffer) g_pVertexBuffer->Release();
    ShaderBillboard_Finalize();
}