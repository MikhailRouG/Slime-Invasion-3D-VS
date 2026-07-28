#ifndef DIRECT3D_H
#define DIRECT3D_H


#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#define SAFE_RELEASE(o) if (o) { (o)->Release(); o = NULL; }


bool Direct3D_Initialize(HWND hWnd); 
void Direct3D_Finalize();

void Direct3D_Present();

unsigned int Direct3D_GetBackBufferWidth(); 
unsigned int Direct3D_GetBackBufferHeight(); 

ID3D11Device* Direct3D_GetDevice();
ID3D11DeviceContext* Direct3D_GetContext();

void Direct3D_SetAlphaBlendTransparent();
void Direct3D_SetAlphaBlendAdd();
DirectX::XMMATRIX Direct3D_MatrixViewport();
DirectX::XMFLOAT3 Direct3D_ScreenToWorld(int x, int y, float depth, const DirectX::XMFLOAT4X4 & view, const DirectX::XMFLOAT4X4 & projection);

DirectX::XMFLOAT2 Direct3D_WorldToScreen(const DirectX::XMFLOAT3 & position,
    const DirectX::XMFLOAT4X4 & view,
    const DirectX::XMFLOAT4X4 & projection);

void Direct3D_ClearBackBuffer();

void Direct3D_SetBackBuffer();

void Direct3D_ClearOffscreen();

void Direct3D_SetDepthEnable(bool enable);
void Direct3D_SetDepthStencilStateDepthWhiteDisable();

void Direct3D_SetOffscreen();
void Direct3D_SetOffscreenTexture(int slot);

void Direct3D_ClearDepth();

void Direct3D_SetDepth();

void Direct3D_SetDepthTexture(int slot);
void Direct3D_SetLightViewProjctionMatrix(const DirectX::XMMATRIX & matrix);
void UnbindAllPSResources();
#endif // DIRECT3D_H
