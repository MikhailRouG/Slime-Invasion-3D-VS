#include "texture.h"
#include "direct3d.h"
#include <string>
#include "WICTextureLoader11.h"
using namespace DirectX;


static constexpr int TEXTURE_MAX = 1024;

struct Texture {
	std::wstring filename;
	unsigned int width;
	unsigned int height;
	ID3D11Resource* pTexture = nullptr;
	ID3D11ShaderResourceView* pTextureView = nullptr;
};

static Texture g_Textures[TEXTURE_MAX]{};
static int g_SetTextureIndex = -1;

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	for (Texture& t : g_Textures) {
		t.pTexture = nullptr;
		t.pTextureView = nullptr;
	}

	g_SetTextureIndex = -1;

	g_pDevice = pDevice;
	g_pContext = pContext;

}

void Texture_Finalize(void){
	Texture_AllRelease();
}

int Texture_Load(const wchar_t* pFilename){
	for (int i = 0; i < TEXTURE_MAX; i++) {
		if (g_Textures[i].filename == pFilename) {
			return i;
		}
	}

	for (int i = 0; i < TEXTURE_MAX; i++) {
		if (g_Textures[i].pTexture) {
			continue;
		}
		HRESULT hr;

		hr = CreateWICTextureFromFile(g_pDevice, g_pContext, pFilename, &g_Textures[i].pTexture, &g_Textures[i].pTextureView);

		ID3D11Texture2D* pTexture = (ID3D11Texture2D*)g_Textures[i].pTexture;
		D3D11_TEXTURE2D_DESC t2desc;
		if (!pTexture)
		{
			return -1; 
		}
		pTexture->GetDesc(&t2desc);
		g_Textures[i].width = t2desc.Width;
		g_Textures[i].height = t2desc.Height;

		if (FAILED(hr)) {
			MessageBoxW(nullptr, L"テクスチャの読み込みに失敗しました", pFilename, MB_OK);
			return -1;
		}

		g_Textures[i].filename = pFilename;

		return i;
	}

	return -1;

}

void Texture_AllRelease(){
	for (Texture& t : g_Textures) {
		t.filename.clear();
		SAFE_RELEASE(t.pTexture);
		SAFE_RELEASE(t.pTextureView);
	}
}


void Texture_SetTexture(int texid, int slot){
	if (texid < 0) {
		return;
	}

	g_SetTextureIndex = texid;
	g_pContext->PSSetShaderResources(slot, 1, &g_Textures[texid].pTextureView);
}

unsigned int Texture_Width(int texid){
	if (texid < 0) {
		return 0;
	}

	return g_Textures[texid].width;
}

unsigned int Texture_Height(int texid){
	if (texid < 0) {
		return 0;
	}

	return g_Textures[texid].height;
}
