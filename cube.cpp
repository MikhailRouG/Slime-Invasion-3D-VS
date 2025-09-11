/*==============================================================================

   3Dキューブ表示[cube.cpp]
														 Author : Harada Ren
														 Date   : 2025/09/09
--------------------------------------------------------------------------------

==============================================================================*/
#include "cube.h"
#include "direct3d.h"
#include "shader3d.h"
#include <DirectXMath.h>
using namespace DirectX;


static constexpr int NUM_VERTEX = 3*2*6; // 頂点数(一面に三角形2個=6個 * 6面分

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

// 3d頂点構造体
struct Vertex3d{
	XMFLOAT3 position; // 頂点座標
	XMFLOAT4 color; //頂点カラー
};

//キューブの頂点情報
//原点0、1*1*1
//右回り
static Vertex3d g_CubeVertex[NUM_VERTEX]{
	//6行で1面分
	{{-0.5f,  0.5f, -0.5f},{1.0f,0.0f,0.0f,1.0f}}, //正面
	{{ 0.5f, -0.5f, -0.5f},{1.0f,0.0f,0.0f,1.0f}}, //赤
	{{-0.5f, -0.5f, -0.5f},{1.0f,0.0f,0.0f,1.0f}},
	{{-0.5f,  0.5f, -0.5f},{1.0f,0.0f,0.0f,1.0f}},
	{{ 0.5f,  0.5f, -0.5f},{1.0f,0.0f,0.0f,1.0f}},
	{{ 0.5f, -0.5f, -0.5f},{1.0f,0.0f,0.0f,1.0f}},

	{{0.5f,  0.5f, -0.5f},{0.0f,1.0f,0.0f,1.0f}}, //右
	{{0.5f, -0.5f,  0.5f},{0.0f,1.0f,0.0f,1.0f}}, //緑
	{{0.5f, -0.5f, -0.5f},{0.0f,1.0f,0.0f,1.0f}},
	{{0.5f,  0.5f, -0.5f},{0.0f,1.0f,0.0f,1.0f}},
	{{0.5f,  0.5f,  0.5f},{0.0f,1.0f,0.0f,1.0f}},
	{{0.5f, -0.5f,  0.5f},{0.0f,1.0f,0.0f,1.0f}},

	{{-0.5f,  0.5f, -0.5f},{0.0f,0.0f,1.0f,1.0f}}, //上
	{{ 0.5f,  0.5f,  0.5f},{0.0f,0.0f,1.0f,1.0f}}, //青
	{{ 0.5f,  0.5f, -0.5f},{0.0f,0.0f,1.0f,1.0f}},
	{{-0.5f,  0.5f, -0.5f},{0.0f,0.0f,1.0f,1.0f}},
	{{-0.5f,  0.5f,  0.5f},{0.0f,0.0f,1.0f,1.0f}},
	{{ 0.5f,  0.5f,  0.5f},{0.0f,0.0f,1.0f,1.0f}},

	{{-0.5f,  0.5f, -0.5f},{1.0f,1.0f,0.0f,1.0f}}, //左
	{{-0.5f, -0.5f, -0.5f},{1.0f,1.0f,0.0f,1.0f}}, //黄
	{{-0.5f, -0.5f,  0.5f},{1.0f,1.0f,0.0f,1.0f}},
	{{-0.5f,  0.5f, -0.5f},{1.0f,1.0f,0.0f,1.0f}},
	{{-0.5f, -0.5f,  0.5f},{1.0f,1.0f,0.0f,1.0f}},
	{{-0.5f,  0.5f,  0.5f},{1.0f,1.0f,0.0f,1.0f}},
	
	{{-0.5f, -0.5f,  0.5f},{1.0f,0.0f,1.0f,1.0f}}, //下
	{{ 0.5f, -0.5f, -0.5f},{1.0f,0.0f,1.0f,1.0f}}, //紫
	{{ 0.5f, -0.5f,  0.5f},{1.0f,0.0f,1.0f,1.0f}},
	{{-0.5f, -0.5f,  0.5f},{1.0f,0.0f,1.0f,1.0f}},
	{{-0.5f, -0.5f, -0.5f},{1.0f,0.0f,1.0f,1.0f}},
	{{ 0.5f, -0.5f, -0.5f},{1.0f,0.0f,1.0f,1.0f}},

	{{ 0.5f,  0.5f,  0.5f},{0.0f,1.0f,1.0f,1.0f}}, //後ろ
	{{-0.5f,  0.5f,  0.5f},{0.0f,1.0f,1.0f,1.0f}}, //水色
	{{ 0.5f, -0.5f,  0.5f},{0.0f,1.0f,1.0f,1.0f}},
	{{-0.5f,  0.5f,  0.5f},{0.0f,1.0f,1.0f,1.0f}},
	{{-0.5f, -0.5f,  0.5f},{0.0f,1.0f,1.0f,1.0f}},
	{{ 0.5f, -0.5f,  0.5f},{0.0f,1.0f,1.0f,1.0f}}
};

void Cube_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	//点の数を算出
	//g_NumVertex = (int)(g_Radius * 2.0f + XM_PI);

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

}

void Cube_Finalize(void){
	SAFE_RELEASE(g_pVertexBuffer);
}

void Cube_Draw(void){
	// シェーダーを描画パイプラインに設定
	Shader3d_Begin();

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	//ワールド座標変換行列の作成
	//XMMATRIX mtxWorld = XMMatrixIdentity(); //単位行列の作成
	XMMATRIX mtxWorld = XMMatrixTranslation(3.0f, 0.5f, 1.0f);
	//頂点シェーダーにワールド座標変換行列を設定
	Shader3d_SetWorldMatrix(mtxWorld);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0); 
}
