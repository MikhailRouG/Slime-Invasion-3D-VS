/*==============================================================================

   ビルボード描画 [billboard.cpp]
														 Author :
														 Date   : 2025/11/14
--------------------------------------------------------------------------------

==============================================================================*/
#include "billboard.h"
#include "shader_billboard.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "texture.h"
#include "player_camera.h"

static constexpr int NUM_VERTEX = 4; // 頂点数

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ


// 3D頂点構造体
struct Vertex3d
{
	XMFLOAT3 position; // 頂点座標
	XMFLOAT4 color;    // 色
	XMFLOAT2 texcoord; // UV
};

void Billboard_Initialize()
{
	ShaderBillboard_Initialize();

	Vertex3d vertex[]{
		{ {-0.5f, 0.5f, 0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.0f, 0.0f } },
		{ { 0.5f, 0.5f, 0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 1.0f, 0.0f } },
		{ {-0.5f,-0.5f, 0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.0f, 1.0f } },
		{ { 0.5f,-0.5f, 0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 1.0f, 1.0f } }
	};

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex3d) * NUM_VERTEX; // sizeof(vertex);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

}

void Billboard_Finalize()
{
	SAFE_RELEASE(g_pVertexBuffer);
	ShaderBillboard_Finalize();
}

void Billboard_Draw(int texId, const DirectX::XMFLOAT3& position, float scale_x, float scale_y, const DirectX::XMFLOAT2& pivot)
{
	ShaderBillboard_SetUVParameter({ { 1.0f / 7.0f, 1.0f }, { 3.0f / 7.0f, 0.0f } });

	// シェーダーを描画パイプラインに設定
	ShaderBillboard_Begin();

	// ピクセルシェーダーに色を設定
	ShaderBillboard_SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	//テクスチャの設定
	Texture_SetTexture(texId);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	Direct3D_GetContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// インデックスバッファを描画パイプラインに設定
	//Direct3D_GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0); // unsigned int _R32

	// プリミティブトポロジ設定
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);//POINTLIST //LINESTRIP //TRIANGLELIST //LINELIST

	// 頂点シェーダーにワールド座標変換行列を設定
	// 回転軸までのオフセット行列
	XMMATRIX pivot_offset = XMMatrixTranslation(-pivot.x, -pivot.y, 1.0f);
	// カメラ行列の回転だけ逆行列を作る
	XMFLOAT4X4 mtxCamera = PlayerCamera_GetViewMatrix();
	mtxCamera._41 = mtxCamera._42 = mtxCamera._43 = 0.0f; // 平行移動行列を消す
	//XMMATRIX iv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&mtxCamera)); // 重い演算
	// 直交行列の逆行列は転置行列に等しい
	XMMATRIX iv = XMMatrixTranspose(XMLoadFloat4x4(&mtxCamera));

	XMMATRIX s = XMMatrixScaling(scale_x, scale_y, 1.0f);
	XMMATRIX t = XMMatrixTranslation(position.x + pivot.x, position.y + pivot.y, position.z);
	ShaderBillboard_SetWorldMatrix(pivot_offset * s * iv * t);

	// ポリゴン描画命令発行
	Direct3D_GetContext()->Draw(NUM_VERTEX, 0);
}
