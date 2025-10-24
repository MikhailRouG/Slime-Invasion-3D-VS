/*==============================================================================

   ライトの設定 [light.cpp]
														 Author : Harada Ren
														 Date   : 2025/09/30
--------------------------------------------------------------------------------

==============================================================================*/
#include "light.h"
using namespace DirectX;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static ID3D11Buffer* g_pPSConstantBuffer1 = nullptr; //定数バッファb1
static ID3D11Buffer* g_pPSConstantBuffer2 = nullptr; //定数バッファb2

struct DirectionalLight {
	XMFLOAT4 directional;
	XMFLOAT4 color;
	XMFLOAT4 CameraPosition;
};

void Light_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// 頂点シェーダー用定数バッファの作成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(XMFLOAT4); // バッファのサイズ
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ

	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer1); //ambient


	buffer_desc.ByteWidth = sizeof(DirectionalLight); // バッファのサイズ
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer2); //directional
}

void Light_Finalize(void)
{
}

void Light_SetAmbient(const XMFLOAT3& color){
	// 定数バッファにアンビエントをセット
	g_pContext->UpdateSubresource(g_pPSConstantBuffer1, 0, nullptr, &color, 0, 0);
	g_pContext->PSSetConstantBuffers(1, 1, &g_pPSConstantBuffer1);
}

void Light_SetDirectionalWorld(const XMFLOAT4& world_directional, const XMFLOAT4& color,const XMFLOAT3 camera_position){
	DirectionalLight light{
		world_directional,
		color,
		{
			camera_position.x,
			camera_position.y,
			camera_position.z,
			0.0f
		}
	};

	// 定数バッファにディレクショナルをセット
	g_pContext->UpdateSubresource(g_pPSConstantBuffer2, 0, nullptr, &light, 0, 0);
	g_pContext->PSSetConstantBuffers(2, 1, &g_pPSConstantBuffer2);
}
