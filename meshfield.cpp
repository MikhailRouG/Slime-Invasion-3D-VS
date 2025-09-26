/*==============================================================================

   メッシュフィールド表示 [meshfield.cpp]
														 Author : Harada Ren
														 Date   : 2025/09/19
--------------------------------------------------------------------------------

==============================================================================*/
#include "meshfield.h"
#include "direct3d.h"
#include "shader_field.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "texture.h"
#include "sampler.h"
#include "cube.h"
#include "camera.h"

static constexpr float FIELD_SIZE = 1.0f;//メッシュ1枚分のサイズ
static constexpr int FIELD_H_COUNT = 50; //横のメッシュ数
static constexpr int FIELD_V_COUNT = 50; //縦のメッシュ数
static constexpr int FIELD_H_VERTEX_COUNT = FIELD_H_COUNT + 1; //横の頂点数
static constexpr int FIELD_V_VERTEX_COUNT = FIELD_V_COUNT + 1; //縦の頂点数

static constexpr int NUM_VERTEX = FIELD_H_VERTEX_COUNT * FIELD_V_VERTEX_COUNT;// 頂点数
static constexpr int NUM_INDEX = 3 * 2 * FIELD_H_COUNT * FIELD_V_COUNT; //インデックス数

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11Buffer* g_pIndexBuffer = nullptr; // インデックスバッファ

static int g_CubeTexId0 = -1;
static int g_CubeTexId1 = -1;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

// 3d頂点構造体
struct Vertex3d {
	XMFLOAT3 position; // 頂点座標
	XMFLOAT4 color; //頂点カラー
	XMFLOAT2 texcoord; //UV
};


static Vertex3d g_MeshfieldVertex[NUM_VERTEX]{};

static unsigned short g_MeshfieldIndex[NUM_INDEX]{};


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


	//頂点情報を配列に作る
	for (int z = 0;z < FIELD_V_VERTEX_COUNT;z++) {
		for (int x = 0;x < FIELD_H_VERTEX_COUNT;x++) {
			//二次元を一次元に
			//横＋横の最大数*縦
			int index = x + FIELD_H_VERTEX_COUNT * z;
			g_MeshfieldVertex[index].position = { x * FIELD_SIZE,0.0f,z * FIELD_SIZE };
			g_MeshfieldVertex[index].color = { 0.0f,1.0f,0.0f,1.0f };
			g_MeshfieldVertex[index].texcoord = { x * 1.0f,z * 1.0f };
		}
	}

	for (int z = 0;z < FIELD_H_VERTEX_COUNT;z++) {
		int index = 25 + FIELD_H_VERTEX_COUNT * z;
		g_MeshfieldVertex[index].color = { 1.0f,0.0f,0.0f,1.0f };
		index = 24 + FIELD_H_VERTEX_COUNT * z;
		g_MeshfieldVertex[index].color = { 1.0f,0.0f,0.0f,1.0f };
		index = 26 + FIELD_H_VERTEX_COUNT * z;
		g_MeshfieldVertex[index].color = { 1.0f,0.0f,0.0f,1.0f };
	}

	
	//インデックス情報を配列に作る
	int index = 0;
	for (int v = 0;v < FIELD_V_COUNT;v++) {
		for (int h = 0;h < FIELD_H_COUNT;h++) {
			unsigned short v0 = (unsigned short)(h + (v + 0) * FIELD_H_VERTEX_COUNT);     // 左下
			unsigned short v1 = (unsigned short)(h + (v + 1) * FIELD_H_VERTEX_COUNT); // 左上
			unsigned short v2 = (unsigned short)(v1 + 1);  // 右上
			unsigned short v3 = (unsigned short)(v0 + 1); //右下

			g_MeshfieldIndex[index + 0] = v0;
			g_MeshfieldIndex[index + 1] = v2;
			g_MeshfieldIndex[index + 2] = v3;

			g_MeshfieldIndex[index + 3] = v0;
			g_MeshfieldIndex[index + 4] = v1;
			g_MeshfieldIndex[index + 5] = v2;

			index += 6;
		}
	}


	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = g_MeshfieldVertex;

	g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);


	//インデックスバッファ生成
	bd.ByteWidth = sizeof(unsigned short) * NUM_INDEX;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	sd.pSysMem = g_MeshfieldIndex;

	g_pDevice->CreateBuffer(&bd, &sd, &g_pIndexBuffer);



	g_CubeTexId0 = Texture_Load(L"resource/texture/blockground2.png");
	g_CubeTexId1 = Texture_Load(L"resource/texture/blockground.png");

	ShaderField_Initialize(pDevice, pContext);
}

void Meshfield_Finalize(void){
	ShaderField_Finalize();
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
}

void Meshfield_Draw() {
	// シェーダーを描画パイプラインに設定
	ShaderField_Begin();

	//テクスチャの設定
	Texture_SetTexture(g_CubeTexId0, 0);
	Texture_SetTexture(g_CubeTexId1, 1);


	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// インデックスバッファを描画パイプラインに設定
	g_pContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//ワールド座標変換行列の作成
	float offset_x = FIELD_H_COUNT * FIELD_SIZE * 0.5f;
	float offset_z = FIELD_V_COUNT * FIELD_SIZE * 0.5f;
	XMMATRIX mtxWorld = XMMatrixTranslation(-offset_x, 0.0f, -offset_z);

	//頂点シェーダーにワールド座標変換行列を設定
	ShaderField_SetWorldMatrix(mtxWorld);

	//カメラとパースペクティブ行列
	ShaderField_SetViewMatrix(XMLoadFloat4x4(&Camera_GetMatrix()));
	ShaderField_SetProjectionMatrix(XMLoadFloat4x4(&Camera_GetPerspectiveMatrix()));

	// ポリゴン描画命令発行
	//g_pContext->Draw(NUM_VERTEX, 0);
	g_pContext->DrawIndexed(NUM_INDEX, 0, 0);
}
