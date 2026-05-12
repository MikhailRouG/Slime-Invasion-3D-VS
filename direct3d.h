#ifndef DIRECT3D_H
#define DIRECT3D_H


#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

// ѓZЃ[ѓtѓЉѓЉЃ[ѓXѓ}ѓNѓЌ
#define SAFE_RELEASE(o) if (o) { (o)->Release(); o = NULL; }


bool Direct3D_Initialize(HWND hWnd); // Direct3D‚МЏ‰Љъ‰»
void Direct3D_Finalize(); // Direct3D‚МЏI—№Џ€—ќ

void Direct3D_Present(); // ѓoѓbѓNѓoѓbѓtѓ@‚М•\Ћ¦

//ѓoѓbѓNѓoѓbѓtѓ@‚М‘е‚«‚і‚МЋж“ѕ
unsigned int Direct3D_GetBackBufferWidth(); //•ќ
unsigned int Direct3D_GetBackBufferHeight(); //Ќ‚‚і

//Direct3dѓfѓoѓCѓX‚МЋж“ѕ
ID3D11Device* Direct3D_GetDevice();
//Direct3dѓfѓoѓCѓXѓRѓ“ѓeѓLѓXѓg‚МЋж“ѕ
ID3D11DeviceContext* Direct3D_GetContext();

//ѓїѓuѓЊѓ“ѓhђЭ’иЉЦђ”
void Direct3D_SetAlphaBlendTransparent(); //“§‰ЯЏ€—ќ
void Direct3D_SetAlphaBlendAdd();// ‰БЋZЌ‡ђ¬
DirectX::XMMATRIX Direct3D_MatrixViewport();
DirectX::XMFLOAT3 Direct3D_ScreenToWorld(int x, int y, float depth, const DirectX::XMFLOAT4X4 & view, const DirectX::XMFLOAT4X4 & projection);
// 3D座標変換 → スクリーン座標
DirectX::XMFLOAT2 Direct3D_WorldToScreen(const DirectX::XMFLOAT3 & position,
    const DirectX::XMFLOAT4X4 & view,
    const DirectX::XMFLOAT4X4 & projection);

// Clear the back buffer
void Direct3D_ClearBackBuffer();

// Switch rendering to the back buffer
void Direct3D_SetBackBuffer();

// Clear the offscreen buffer
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
