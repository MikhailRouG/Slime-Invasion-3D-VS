#include "sampler.h"
#include "direct3d.h"

// ���ӁI�������ŊO������ݒ肳�����́BRelease�s�v�B
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static ID3D11SamplerState* g_pSamplerFilterPoint = nullptr;
static ID3D11SamplerState* g_pSamplerFilterLinear = nullptr;
static ID3D11SamplerState* g_pSamplerFilterAnisotropic = nullptr;
static ID3D11SamplerState* g_pSamplerShadow = nullptr;


void Sampler_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	// �f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̕ۑ�
	g_pDevice = pDevice;
	g_pContext = pContext;

	//�T���v���[�X�e�[�g�ݒ�
	D3D11_SAMPLER_DESC sampler_desc{};
	//�t�B���^�����O(LINEAR�c�摜���������΂����߁A���̎��肪�ڂ��������ɂȂ�@���@�k���Ȃ��Y��
	//				 POINT�c��\�_�����ׂ邽�߁A�������茩����@���@�h�b�g�G�Ȃ��Y��
	//				 MIPMAP�c���O�ɃT�C�Y�ʂ̉摜��p�ӂ��A�g���@���@�i�q��̊G�ɋ���
	//				 ANISOTROPIC�c3d���Y��B����
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	//UV�Q�ƊO�̎戵��(UV�A�h���b�V���O���[�h)�@UV�l���O<=x<=1�̎��Ȃ�
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.BorderColor[0] = 0.0f;
	sampler_desc.BorderColor[1] = 0.0f;
	sampler_desc.BorderColor[2] = 0.0f;
	sampler_desc.BorderColor[3] = 0.0f;

	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP; //W�͎g��Ȃ�
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

	// Shadow-map sampler.
	// POINT: PCF must compare each texel against the reference depth and
	// average the *results*. Averaging depth values first (linear/aniso)
	// and comparing once is mathematically wrong and causes light leaks.
	// BORDER + white: anything sampled outside the light frustum returns
	// the far depth 1.0, i.e. "not occluded", so the world beyond the
	// shadow map stays lit instead of tiling the map across the level.
	D3D11_SAMPLER_DESC shadow_desc{};
	shadow_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	shadow_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadow_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadow_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadow_desc.BorderColor[0] = 1.0f;
	shadow_desc.BorderColor[1] = 1.0f;
	shadow_desc.BorderColor[2] = 1.0f;
	shadow_desc.BorderColor[3] = 1.0f;
	shadow_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	shadow_desc.MinLOD = 0;
	shadow_desc.MaxLOD = D3D11_FLOAT32_MAX;

	g_pDevice->CreateSamplerState(&shadow_desc, &g_pSamplerShadow);
}

void Sampler_Finalize(){
	SAFE_RELEASE(g_pSamplerShadow);
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

void Sampler_SetShadow(){
	g_pContext->PSSetSamplers(1, 1, &g_pSamplerShadow);
}
