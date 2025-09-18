/*==============================================================================

   サンプラーの設定ユーティリティー [sampler.cpp]
														 Author : Harada Ren
														 Date   : 2025/09/18
--------------------------------------------------------------------------------

==============================================================================*/
#include "sampler.h"
#include "direct3d.h"

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static ID3D11SamplerState* g_pSamplerFilterPoint = nullptr;
static ID3D11SamplerState* g_pSamplerFilterLinear = nullptr;
static ID3D11SamplerState* g_pSamplerFilterAnisotropic = nullptr;


void Sampler_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	//サンプラーステート設定
	D3D11_SAMPLER_DESC sampler_desc{};
	//フィルタリング(LINEAR…画像を引き延ばすため、線の周りがぼけた感じになる　→　縮小なら綺麗
	//				 POINT…代表点一つを並べるため、くっきり見える　→　ドット絵なら綺麗
	//				 MIPMAP…事前にサイズ別の画像を用意し、使う　→　格子状の絵に強い
	//				 ANISOTROPIC…3dで綺麗。高級
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	//UV参照外の取扱い(UVアドレッシングモード)　UV値が０<=x<=1の時など
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.BorderColor[0] = 0.0f;
	sampler_desc.BorderColor[1] = 0.0f;
	sampler_desc.BorderColor[2] = 0.0f;
	sampler_desc.BorderColor[3] = 0.0f;

	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP; //Wは使わない
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerFilterPoint);

	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerFilterLinear);

	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerFilterAnisotropic);
}

void Sampler_Finalize(){
	SAFE_RELEASE(g_pSamplerFilterAnisotropic);
	SAFE_RELEASE(g_pSamplerFilterLinear);
	SAFE_RELEASE(g_pSamplerFilterPoint);
}

void Sampler_SetFilterPoint(){
	g_pContext->PSSetSamplers(0, 1, &g_pSamplerFilterPoint);
}

void Sampler_SetFilterLinear(){
	g_pContext->PSSetSamplers(0, 1, &g_pSamplerFilterLinear);
}

void Sampler_SetFilterAnisotropic(){
	g_pContext->PSSetSamplers(0, 1, &g_pSamplerFilterAnisotropic);
}
