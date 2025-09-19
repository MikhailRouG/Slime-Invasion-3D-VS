/*==============================================================================

   メッシュフィールド表示 [meshfield.cpp]
														 Author : Harada Ren
														 Date   : 2025/09/19
--------------------------------------------------------------------------------

==============================================================================*/
#include "meshfield.h"
#include "direct3d.h"
#include "shader3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "texture.h"


static constexpr int NUM_INDEX = 2 * 3 * 6; // 頂点数(一面に三角形2個=6個 * 6面分)
static constexpr int NUM_VERTEX = 4 * 6;

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11Buffer* g_pIndexBuffer = nullptr; // インデックスバッファ

static int g_CubeTexId = -1;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

// 3d頂点構造体
struct Vertex3d {
	XMFLOAT3 position; // 頂点座標
	XMFLOAT4 color; //頂点カラー
	XMFLOAT2 texcoord; //UV
};


static Vertex3d g_CubeVertex[NUM_VERTEX]{};

static unsigned short g_CubeIndex[NUM_INDEX]{};


void Meshfield_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	//書き換えない(3dでは元の座標は変わらない)
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex3d) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//1回描いたら書き換えない
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = g_CubeVertex;

	g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);


	//インデックスバッファ生成
	bd.ByteWidth = sizeof(unsigned short) * NUM_INDEX;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	sd.pSysMem = g_CubeIndex;

	g_pDevice->CreateBuffer(&bd, &sd, &g_pIndexBuffer);



	g_CubeTexId = Texture_Load(L"resource/texture/BoxTestTexture2.png");
}

void Meshfield_Finalize(void)
{
}

void Meshfield_Draw(const DirectX::XMMATRIX mtxWorld)
{
}
