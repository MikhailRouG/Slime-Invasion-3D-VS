#include "cube.h"
#include "direct3d.h"
#include "shader3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "texture.h"
#include "shader_depth.h"
static constexpr int NUM_INDEX = 2 * 3 * 6; // 頂点数(一面に三角形2個=6個 * 6面分)
static constexpr int NUM_VERTEX = 4 * 6;

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11Buffer* g_pIndexBuffer = nullptr; // インデックスバッファ

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

// 3d頂点構造体
struct Vertex3d{
	XMFLOAT3 position; // 頂点座標
	XMFLOAT3 normal; //法線
	XMFLOAT4 color; //頂点カラー
	XMFLOAT2 texcoord; //UV
};

//キューブの頂点情報
//原点0、1*1*1
//右回り
static Vertex3d g_CubeVertex[NUM_VERTEX]{
	//6行で1面分
	//正面
	{{-0.5f,  0.5f, -0.5f}, {0.0f,0.0f,-1.0f}, { 1.0f,1.0f,1.0f,1.0f },{0.0f,0.0f}},  //0
	{{ 0.5f, -0.5f, -0.5f}, {0.0f,0.0f,-1.0f}, {1.0f,1.0f,1.0f,1.0f},{0.25f,0.25f}}, //1
	{{-0.5f, -0.5f, -0.5f}, {0.0f,0.0f,-1.0f}, {1.0f,1.0f,1.0f,1.0f},{0.0f,0.25f}}, //2
	//{{-0.5f,  0.5f, -0.5f},{0.0f,0.0f,-1.0f}, {1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f}},
	{{ 0.5f,  0.5f, -0.5f}, {0.0f,0.0f,-1.0f}, {1.0f,1.0f,1.0f,1.0f},{0.25f,0.0f}}, //3
	//{{ 0.5f, -0.5f, -0.5f}, {0.0f,0.0f,-1.0f}, {1.0f,1.0f,1.0f,1.0f},{0.25f,0.25f}},

	//右
	{{0.5f,  0.5f, -0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.0f}}, //4
	{{0.5f, -0.5f,  0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.25f}},
	{{0.5f, -0.5f, -0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.25f}},
	//{{0.5f,  0.5f, -0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.0f}},
	{{0.5f,  0.5f,  0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.0f}},
	//{{0.5f, -0.5f,  0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.25f}},

	//上
	{{-0.5f,  0.5f, -0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.25f}}, //8
	{{ 0.5f,  0.5f,  0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.75f,0.0f}},
	{{ 0.5f,  0.5f, -0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.75f,0.25f}},
	//{{-0.5f,  0.5f, -0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.25f}},
	{{-0.5f,  0.5f,  0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.0f}},
	//{{ 0.5f,  0.5f,  0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.75f,0.0f}},

	//左
	{{-0.5f,  0.5f, -0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{1.0f,0.0f}}, //12
	{{-0.5f, -0.5f, -0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{1.0f,0.25f}},
	{{-0.5f, -0.5f,  0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.75f,0.25f}},
	//{{-0.5f,  0.5f, -0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{1.0f,0.0f}},
	//{{-0.5f, -0.5f,  0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.75f,0.25f}},
	{{-0.5f,  0.5f,  0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.75f,0.0f}},
	
	//下
	{{-0.5f, -0.5f,  0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.0f,0.25f}}, //16
	{{ 0.5f, -0.5f, -0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.5f}},
	{{ 0.5f, -0.5f,  0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.25f}},
	//{{-0.5f, -0.5f,  0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.0f,0.25f}},
	{{-0.5f, -0.5f, -0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.0f,0.5f}},
	//{{ 0.5f, -0.5f, -0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.5f}},

	//後ろ
	{{-0.5f, -0.5f,  0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.5f}}, //20
	{{ 0.5f, -0.5f,  0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.5f}},
	{{ 0.5f,  0.5f,  0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.25f}},
	//{{-0.5f, -0.5f,  0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.5f}},
	//{{ 0.5f,  0.5f,  0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.25f}},
	{{-0.5f,  0.5f,  0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.25f}}
};

//インデックス...結び方リスト
//Unsigned short は 0~65535
static unsigned short g_CubeIndex[NUM_INDEX]{
	0,1,2,0,3,1,
	4,5,6,4,7,5,
	8,9,10,8,11,9,
	12,13,14,12,14,15,
	16,17,18,16,19,17,
	20,21,22,20,22,23
};

void Cube_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
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
}

void Cube_Finalize(void){
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
}


void Cube_Draw(int texid, const DirectX::XMMATRIX mtxWorld) {
	// シェーダーを描画パイプラインに設定
	Shader3d_Begin();

	//ピクセルシェーダーに色を設定
	Shader3d_SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	//テクスチャの設定
	Texture_SetTexture(texid);


	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// インデックスバッファを描画パイプラインに設定
	g_pContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//ワールド座標変換行列の作成
	//XMMATRIX mtxWorld = XMMatrixIdentity(); //単位行列の作成(データに対して何もしない)

	//頂点シェーダーにワールド座標変換行列を設定
	Shader3d_SetWorldMatrix(mtxWorld);

	// ポリゴン描画命令発行
	//g_pContext->Draw(NUM_VERTEX, 0);
	g_pContext->DrawIndexed(NUM_INDEX, 0, 0);

}

void Cube_Draw(int texid, const DirectX::XMMATRIX mtxWorld, const DirectX::XMFLOAT4& color)
{
	Shader3d_Begin();

	//ピクセルシェーダーに色を設定
	Shader3d_SetColor(color);

	//テクスチャの設定
	Texture_SetTexture(texid);


	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// インデックスバッファを描画パイプラインに設定
	g_pContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//ワールド座標変換行列の作成
	//XMMATRIX mtxWorld = XMMatrixIdentity(); //単位行列の作成(データに対して何もしない)

	//頂点シェーダーにワールド座標変換行列を設定
	Shader3d_SetWorldMatrix(mtxWorld);

	// ポリゴン描画命令発行
	//g_pContext->Draw(NUM_VERTEX, 0);
	g_pContext->DrawIndexed(NUM_INDEX, 0, 0);
}

void Cube_DepthDraw(int texid, const DirectX::XMMATRIX mtxWorld)
{
	ShaderDepth_Begin();

	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// インデックスバッファを描画パイプラインに設定
	g_pContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//ワールド座標変換行列の作成
	//XMMATRIX mtxWorld = XMMatrixIdentity(); //単位行列の作成(データに対して何もしない)

	//頂点シェーダーにワールド座標変換行列を設定
	ShaderDepth_SetWorldMatrix(mtxWorld);

	// ポリゴン描画命令発行
	//g_pContext->Draw(NUM_VERTEX, 0);
	g_pContext->DrawIndexed(NUM_INDEX, 0, 0);

}

const AABB Cube_GetAABB(const DirectX::XMFLOAT3& position){
	return {
		{position.x - 0.5f,position.y - 0.5f,position.z - 0.5f},
		{position.x + 0.5f,position.y + 0.5f,position.z + 0.5f}
	};
}