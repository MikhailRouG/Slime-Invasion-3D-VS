#ifndef SHADER_BILLBOARD_H
#define SHADER_BILLBOARD_H

#include <d3d11.h>
#include <DirectXMath.h>

struct BillboardVSConstant {
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 proj;
    DirectX::XMFLOAT2   scale;
    DirectX::XMFLOAT2   translation;
    float padding[2];
};

bool ShaderBillboard_Initialize();
void ShaderBillboard_Finalize();
void ShaderBillboard_SetVSConstant(const BillboardVSConstant& cb);
void ShaderBillboard_SetColor(const DirectX::XMFLOAT4& color);
void ShaderBillboard_Begin();

#endif