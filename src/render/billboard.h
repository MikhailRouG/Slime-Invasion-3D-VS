#ifndef BILLBOARD_H
#define BILLBOARD_H

#include <DirectXMath.h>
#include <d3d11.h>

struct Vertex3d {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT2 texcoord;
};

void Billboard_Initialize();
void Billboard_Finalize();
void Billboard_Draw(int texId, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& scale, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& pivot = { 0.0f, 0.0f });
void Billboard_Draw(int texId, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& scale,
    const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& pivot,
    const DirectX::XMFLOAT2& uvScale, const DirectX::XMFLOAT2& uvOffset);
#endif