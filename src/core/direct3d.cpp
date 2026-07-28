
#include <d3d11.h>
#include "direct3d.h"
#include "debug_ostream.h"
using namespace DirectX;
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11BlendState* g_pBlendStateMultiply = nullptr;
static ID3D11BlendState* g_pBlendStateAdd = nullptr;
static ID3D11DepthStencilState* g_pDepthStencilStateDepthDisable = nullptr;
static ID3D11DepthStencilState* g_pDepthStencilStateDepthEnable = nullptr;
static ID3D11DepthStencilState * g_pDepthStencilStateDepthWhiteDisable = nullptr;
static ID3D11RasterizerState* g_pRasterizerState = nullptr;


static ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
static ID3D11Texture2D* g_pDepthStencilBuffer = nullptr;
static ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
static D3D11_TEXTURE2D_DESC g_BackBufferDesc{};
static D3D11_VIEWPORT g_Viewport{};

static bool configureBackBuffer();
static void releaseBackBuffer();

static ID3D11Texture2D* g_pOffscreenBuffer = nullptr;
static ID3D11RenderTargetView* g_pOffscreenRenderTargetView = nullptr;
static ID3D11ShaderResourceView* g_pOffscreenShaderResourceView = nullptr;
static ID3D11Texture2D* g_pOffscreenDepthStencilBuffer = nullptr;
static ID3D11DepthStencilView* g_pOffscreenDepthStencilView = nullptr;
static D3D11_TEXTURE2D_DESC   g_OffscreenDesc{};
static D3D11_VIEWPORT         g_OffscreenViewport{};


static bool configureOffscreenBuffer();
static void releaseOffScreenBuffer(); //

static ID3D11Texture2D* g_pDepthBuffer = nullptr;
static ID3D11RenderTargetView* g_pDepthRenderTargetView = nullptr;
static ID3D11ShaderResourceView* g_pDepthShaderResourceView = nullptr;
static ID3D11Texture2D* g_pDepthDepthStencilBuffer = nullptr;
static ID3D11DepthStencilView* g_pDepthDepthStencilView = nullptr;
static D3D11_TEXTURE2D_DESC   g_DepthDesc{};
static D3D11_VIEWPORT         g_DepthViewport{};
static ID3D11Buffer* g_pVSConstantBuffer3 = nullptr;

static bool configureDepthBuffer();
static void releaseDepthBuffer(); //
bool Direct3D_Initialize(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
    swap_chain_desc.Windowed = TRUE;
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

    swap_chain_desc.OutputWindow = hWnd;

	UINT device_flags = 0;


    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };
    
    D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
 
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        device_flags,
        levels,
        ARRAYSIZE(levels),
        D3D11_SDK_VERSION,
        &swap_chain_desc,
        &g_pSwapChain,
        &g_pDevice,
        &feature_level,
        &g_pDeviceContext);

    if (FAILED(hr)) {
		MessageBox(hWnd, "Direct3D‚МЏ‰Љъ‰»‚ЙЋё”s‚µ‚Ь‚µ‚Ѕ", "ѓGѓ‰Ѓ[", MB_OK);
        return false;
    }

	if (!configureBackBuffer()) {
		MessageBox(hWnd, "ѓoѓbѓNѓoѓbѓtѓ@‚МђЭ’и‚ЙЋё”s‚µ‚Ь‚µ‚Ѕ", "ѓGѓ‰Ѓ[", MB_OK);
		return false;
	}
	configureOffscreenBuffer();
	configureDepthBuffer();
	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable = FALSE;
	bd.IndependentBlendEnable = FALSE;

	bd.RenderTarget[0].BlendEnable = TRUE; 


	

	//RGB	
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; 	//SrcRGB*SrcA
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; //DestRGB * (1-SrcA)
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//SrcRGB * SrcA + DestRGB * (1-SrcA)


	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; //1
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; //0
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//SrcA * 1 + DestA * 0
	
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	g_pDevice->CreateBlendState(&bd, &g_pBlendStateMultiply);

	//----------------

		//RGB
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE; //DestRGB * (1-SrcA)
	//SrcRGB * SrcA + DestRGB * (1-SrcA)
	
	//SrcA * 1 + DestA * 0

	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	g_pDevice->CreateBlendState(&bd, &g_pBlendStateAdd);
	//--------------


	Direct3D_SetAlphaBlendTransparent();


	D3D11_DEPTH_STENCIL_DESC dsd{};
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	dsd.StencilEnable = FALSE;
	dsd.DepthEnable = FALSE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	g_pDevice->CreateDepthStencilState(&dsd, &g_pDepthStencilStateDepthDisable);

	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	g_pDevice->CreateDepthStencilState(&dsd, &g_pDepthStencilStateDepthEnable);

	dsd.StencilEnable = FALSE;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	g_pDevice->CreateDepthStencilState(&dsd, &g_pDepthStencilStateDepthWhiteDisable);
	Direct3D_SetDepthEnable(true);


	D3D11_RASTERIZER_DESC rd = {};
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthClipEnable = TRUE;
	rd.MultisampleEnable = FALSE;
	g_pDevice->CreateRasterizerState(&rd, &g_pRasterizerState);

	g_pDeviceContext->RSSetState(g_pRasterizerState);

    return true;
}

void Direct3D_Finalize()
{
	SAFE_RELEASE(g_pRasterizerState);
	
	SAFE_RELEASE(g_pDepthStencilStateDepthEnable);
	SAFE_RELEASE(g_pDepthStencilStateDepthDisable);

	SAFE_RELEASE(g_pBlendStateMultiply);

	releaseBackBuffer();
	releaseOffScreenBuffer();
	releaseDepthBuffer();
	SAFE_RELEASE(g_pSwapChain);

	SAFE_RELEASE(g_pDeviceContext);
	
	SAFE_RELEASE(g_pDevice);
}

void Direct3D_Present()
{
	g_pSwapChain->Present(1, 0);
}

unsigned int Direct3D_GetBackBufferWidth()
{
	return g_BackBufferDesc.Width;
}

unsigned int Direct3D_GetBackBufferHeight()
{
	return g_BackBufferDesc.Height;
}

ID3D11Device* Direct3D_GetDevice()
{
	return g_pDevice;
}

ID3D11DeviceContext* Direct3D_GetContext()
{
	return g_pDeviceContext;
}

void Direct3D_SetAlphaBlendTransparent(){
	float blend_factor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
	g_pDeviceContext->OMSetBlendState(g_pBlendStateMultiply, blend_factor, 0xffffffff);

}

void Direct3D_SetAlphaBlendAdd(){
	float blend_factor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
	g_pDeviceContext->OMSetBlendState(g_pBlendStateAdd, blend_factor, 0xffffffff);
}

DirectX::XMMATRIX Direct3D_MatrixViewport()
{
	float half_width = Direct3D_GetBackBufferWidth() * 0.5f;
	float half_height = Direct3D_GetBackBufferHeight() * 0.5f;
	float max_depth = g_Viewport.MaxDepth;
	float min_depth = g_Viewport.MinDepth;

	return DirectX::XMMATRIX(
		half_width, 0.0f, 0.0f, 0.0f,
		0.0f, -half_height, 0.0f, 0.0f,
		0.0f, 0.0f, (max_depth - min_depth), 0.0f,
		half_width, half_height, min_depth, 1.0f
	);
}

DirectX::XMFLOAT3 Direct3D_ScreenToWorld(int x, int y, float depth, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{

	XMMATRIX xview{ XMLoadFloat4x4(&view) };
	XMMATRIX xproj{ XMLoadFloat4x4(&projection) };
	XMVECTOR xpoint{ x, y, depth, 1.0f };

	XMMATRIX inv{ XMMatrixInverse(nullptr, xview * xproj * Direct3D_MatrixViewport()) };

	xpoint = XMVector3TransformCoord(xpoint, inv);

	XMFLOAT3 ret;

	XMStoreFloat3(&ret, xpoint);

	return ret;
}

DirectX::XMFLOAT2 Direct3D_WorldToScreen(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	XMMATRIX xview{ XMLoadFloat4x4(&view) };
	XMMATRIX xproj{ XMLoadFloat4x4(&projection) };
	XMVECTOR xpoint{ XMLoadFloat3(&position) };

	XMMATRIX inv{ XMMatrixInverse(nullptr, xview * xproj * Direct3D_MatrixViewport()) };

	XMFLOAT2 ret;

	XMStoreFloat2(&ret, xpoint);

	return ret;

}

void Direct3D_ClearBackBuffer()
{
	float clear_color[4] = { 0.97f, 0.67f, 0.65f, 1.0f }; 

	g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, clear_color);
	g_pDeviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

}

void Direct3D_SetBackBuffer()
{
	g_pDeviceContext->RSSetViewports(1, &g_Viewport);
	UnbindAllPSResources();
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
}

void Direct3D_ClearOffscreen()
{
	float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_pDeviceContext->ClearRenderTargetView(g_pOffscreenRenderTargetView, clear_color);
	g_pDeviceContext->ClearDepthStencilView(g_pOffscreenDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

}

void Direct3D_SetDepthEnable(bool enable){
	if (enable) {
		g_pDeviceContext->OMSetDepthStencilState(g_pDepthStencilStateDepthEnable, NULL);
	}
	else {
		g_pDeviceContext->OMSetDepthStencilState(g_pDepthStencilStateDepthDisable, NULL);
	}
}

void Direct3D_SetDepthStencilStateDepthWhiteDisable()
{
	g_pDeviceContext->OMSetDepthStencilState(g_pDepthStencilStateDepthWhiteDisable, NULL);
}

void Direct3D_SetOffscreen()
{
	g_pDeviceContext->RSSetViewports(1, &g_OffscreenViewport);  
	g_pDeviceContext->OMSetRenderTargets(1, &g_pOffscreenRenderTargetView, g_pOffscreenDepthStencilView);

}
void Direct3D_SetOffscreenTexture(int slot)
{
	g_pDeviceContext->PSSetShaderResources(slot, 1, &g_pOffscreenShaderResourceView);
}
void Direct3D_ClearDepth()
{
	// Clear to 1.0 = the far plane, i.e. "nothing occludes this texel".
	// Clearing to 0.0 (the near plane) made every texel with no caster read
	// as maximally close, so the shadow test failed everywhere and the whole
	// lit scene was darkened instead of just the shadowed parts.
	float clear_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	g_pDeviceContext->ClearRenderTargetView(
		g_pDepthRenderTargetView,
		clear_color
	);

	g_pDeviceContext->ClearDepthStencilView(
		g_pDepthDepthStencilView,
		D3D11_CLEAR_DEPTH,
		1.0f,
		0
	);
}

void Direct3D_SetDepth()
{
	UnbindAllPSResources();
	g_pDeviceContext->RSSetViewports(1, &g_DepthViewport);
	UnbindAllPSResources();
	g_pDeviceContext->OMSetRenderTargets(
		1,
		&g_pDepthRenderTargetView,
		g_pDepthDepthStencilView
	);
}

void Direct3D_SetDepthTexture(int slot)
{
	g_pDeviceContext->PSSetShaderResources(slot, 1, &g_pDepthShaderResourceView);
}

void Direct3D_SetLightViewProjctionMatrix(const DirectX::XMMATRIX& matrix)
{
	XMFLOAT4X4 transpose;
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));
	g_pDeviceContext->UpdateSubresource(
		g_pVSConstantBuffer3,
		0, nullptr,
		&transpose,
		0, 0);

	Direct3D_GetContext()->VSSetConstantBuffers(
		3,
		1,
		&g_pVSConstantBuffer3
	);
}



bool configureBackBuffer()
{
    HRESULT hr;

    ID3D11Texture2D* back_buffer_pointer = nullptr;

	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer_pointer);

    if (FAILED(hr)) {
		hal::dout << "ѓoѓbѓNѓoѓbѓtѓ@‚МЋж“ѕ‚ЙЋё”s‚µ‚Ь‚µ‚Ѕ" << std::endl;
        return false;
    }

	hr = g_pDevice->CreateRenderTargetView(back_buffer_pointer, nullptr, &g_pRenderTargetView);

    if (FAILED(hr)) {
        back_buffer_pointer->Release();
        hal::dout << "ѓoѓbѓNѓoѓbѓtѓ@‚МѓЊѓ“ѓ_Ѓ[ѓ^Ѓ[ѓQѓbѓgѓrѓ…Ѓ[‚Мђ¶ђ¬‚ЙЋё”s‚µ‚Ь‚µ‚Ѕ" << std::endl;
        return false;
    }

    back_buffer_pointer->GetDesc(&g_BackBufferDesc);

	back_buffer_pointer->Release(); 

	D3D11_TEXTURE2D_DESC depth_stencil_desc{};
	depth_stencil_desc.Width = g_BackBufferDesc.Width;
	depth_stencil_desc.Height = g_BackBufferDesc.Height;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_desc.SampleDesc.Count = 1;
	depth_stencil_desc.SampleDesc.Quality = 0;
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;
	hr = g_pDevice->CreateTexture2D(&depth_stencil_desc, nullptr, &g_pDepthStencilBuffer);

	if (FAILED(hr)) {
		hal::dout << "ѓfѓvѓXѓXѓeѓ“ѓVѓ‹ѓoѓbѓtѓ@‚Мђ¶ђ¬‚ЙЋё”s‚µ‚Ь‚µ‚Ѕ" << std::endl;
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	depth_stencil_view_desc.Format = depth_stencil_desc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	depth_stencil_view_desc.Flags = 0;
	hr = g_pDevice->CreateDepthStencilView(g_pDepthStencilBuffer, &depth_stencil_view_desc, &g_pDepthStencilView);

	if (FAILED(hr)) {
		hal::dout << "ѓfѓvѓXѓXѓeѓ“ѓVѓ‹ѓrѓ…Ѓ[‚Мђ¶ђ¬‚ЙЋё”s‚µ‚Ь‚µ‚Ѕ" << std::endl;
		return false;
	}


	g_Viewport.TopLeftX = 0.0f;
	g_Viewport.TopLeftY = 0.0f;
	g_Viewport.Width = static_cast<FLOAT>(g_BackBufferDesc.Width);
	g_Viewport.Height = static_cast<FLOAT>(g_BackBufferDesc.Height);
	g_Viewport.MinDepth = 0.0f;
	g_Viewport.MaxDepth = 1.0f;
	g_pDeviceContext->RSSetViewports(1, &g_Viewport);

    return true;
}

void releaseBackBuffer()
{
	SAFE_RELEASE(g_pRenderTargetView);

	SAFE_RELEASE(g_pDepthStencilBuffer);

	SAFE_RELEASE(g_pDepthStencilView);
}

bool configureOffscreenBuffer()
{
	g_OffscreenDesc.Width = 512;
	g_OffscreenDesc.Height = 512;
	g_OffscreenDesc.MipLevels = 1;
	g_OffscreenDesc.ArraySize = 1;
	g_OffscreenDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	g_OffscreenDesc.SampleDesc.Count = 1;
	g_OffscreenDesc.SampleDesc.Quality = 0;
	g_OffscreenDesc.Usage = D3D11_USAGE_DEFAULT;
	g_OffscreenDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	g_OffscreenDesc.CPUAccessFlags = 0;
	g_OffscreenDesc.MiscFlags = 0;
	g_pDevice->CreateTexture2D(&g_OffscreenDesc, nullptr, &g_pOffscreenBuffer);
	Direct3D_GetDevice()->CreateRenderTargetView(
		g_pOffscreenBuffer, nullptr, &g_pOffscreenRenderTargetView);

	Direct3D_GetDevice()->CreateShaderResourceView(
		g_pOffscreenBuffer, nullptr, &g_pOffscreenShaderResourceView);

	D3D11_TEXTURE2D_DESC depth_stencil_desc{};
	depth_stencil_desc.Width = g_OffscreenDesc.Width;
	depth_stencil_desc.Height = g_OffscreenDesc.Height;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = DXGI_FORMAT_D32_FLOAT;
	depth_stencil_desc.SampleDesc.Count = 1;
	depth_stencil_desc.SampleDesc.Quality = 0;
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;
	g_pDevice->CreateTexture2D(&depth_stencil_desc, nullptr, &g_pOffscreenDepthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	depth_stencil_view_desc.Format = depth_stencil_desc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	depth_stencil_view_desc.Flags = 0;
	g_pDevice->CreateDepthStencilView(g_pOffscreenDepthStencilBuffer, &depth_stencil_view_desc, &g_pOffscreenDepthStencilView);

	g_OffscreenViewport.TopLeftX = 0.0f;
	g_OffscreenViewport.TopLeftY = 0.0f;
	g_OffscreenViewport.Width = static_cast<FLOAT>(g_OffscreenDesc.Width);
	g_OffscreenViewport.Height = static_cast<FLOAT>(g_OffscreenDesc.Height);
	g_OffscreenViewport.MinDepth = 0.0f;
	g_OffscreenViewport.MaxDepth = 1.0f;

	return true;
}

void releaseOffScreenBuffer()
{
	SAFE_RELEASE(g_pOffscreenBuffer);
	SAFE_RELEASE(g_pOffscreenRenderTargetView);
	SAFE_RELEASE(g_pOffscreenShaderResourceView);
	SAFE_RELEASE(g_pOffscreenDepthStencilBuffer);
	SAFE_RELEASE(g_pOffscreenDepthStencilView);

}

bool configureDepthBuffer()
{
	g_DepthDesc.Width =2048;
	g_DepthDesc.Height = 2048;
	g_DepthDesc.MipLevels = 1;
	g_DepthDesc.ArraySize = 1;
	g_DepthDesc.Format = DXGI_FORMAT_R32_FLOAT;
	g_DepthDesc.SampleDesc.Count = 1;
	g_DepthDesc.SampleDesc.Quality = 0;
	g_DepthDesc.Usage = D3D11_USAGE_DEFAULT;
	g_DepthDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	g_DepthDesc.CPUAccessFlags = 0;
	g_DepthDesc.MiscFlags = 0;
	g_pDevice->CreateTexture2D(&g_DepthDesc, nullptr, &g_pDepthBuffer);
	Direct3D_GetDevice()->CreateRenderTargetView(
		g_pDepthBuffer, nullptr, &g_pDepthRenderTargetView);

	Direct3D_GetDevice()->CreateShaderResourceView(
		g_pDepthBuffer, nullptr, &g_pDepthShaderResourceView);

	D3D11_TEXTURE2D_DESC depth_stencil_desc{};
	depth_stencil_desc.Width = g_DepthDesc.Width;
	depth_stencil_desc.Height = g_DepthDesc.Height;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = DXGI_FORMAT_D32_FLOAT;
	depth_stencil_desc.SampleDesc.Count = 1;
	depth_stencil_desc.SampleDesc.Quality = 0;
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;
	g_pDevice->CreateTexture2D(&depth_stencil_desc, nullptr, &g_pDepthDepthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	depth_stencil_view_desc.Format = depth_stencil_desc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	depth_stencil_view_desc.Flags = 0;
	g_pDevice->CreateDepthStencilView(g_pDepthDepthStencilBuffer, &depth_stencil_view_desc, &g_pDepthDepthStencilView);

	g_DepthViewport.TopLeftX = 0.0f;
	g_DepthViewport.TopLeftY = 0.0f;
	g_DepthViewport.Width = static_cast<FLOAT>(g_DepthDesc.Width);
	g_DepthViewport.Height = static_cast<FLOAT>(g_DepthDesc.Height);
	g_DepthViewport.MinDepth = 0.0f;
	g_DepthViewport.MaxDepth = 1.0f;
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(XMFLOAT4X4);
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer3);

	return true;
}

void releaseDepthBuffer()
{
	SAFE_RELEASE(g_pDepthBuffer);
	SAFE_RELEASE(g_pDepthRenderTargetView);
	SAFE_RELEASE(g_pDepthShaderResourceView);
	SAFE_RELEASE(g_pDepthDepthStencilBuffer);
	SAFE_RELEASE(g_pDepthDepthStencilView);
	SAFE_RELEASE(g_pVSConstantBuffer3);

}
void UnbindAllPSResources()
{
	ID3D11ShaderResourceView* nullSRV[16] = {};
	Direct3D_GetContext()->PSSetShaderResources(0, 16, nullSRV);
}
