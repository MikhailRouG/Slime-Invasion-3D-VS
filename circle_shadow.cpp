#include "circle_shadow.h"
#include "shader3d_unlit.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "texture.h"
#include "player_camera.h"
#include "collision.h"
#include "map.h"
static constexpr int NUM_VERTEX = 4; // 頂点数

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static XMFLOAT4X4 g_mtxView{};
static int g_TexId = -1;
struct Vertex3d
{
	XMFLOAT3 position; 
	XMFLOAT3 normal; 
	XMFLOAT4 color;   
	XMFLOAT2 texcoord; 
};

void CircleShadow_Initialize()
{
	Vertex3d vertex[]{
	{ {-0.5f, 0.0f, 0.5f }, {0.0f, 1.01f, 0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.0f, 0.0f } },
	{ { 0.5f, 0.0f, 0.5f }, {0.0f, 1.01f, 0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 1.0f, 0.0f } },
	{ {-0.5f,-0.0f, -0.5f }, {0.0f, 1.01f, 0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.0f, 1.0f } },
	{ { 0.5f,-0.0f, -0.5f }, {0.0f, 1.01f, 0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 1.0f, 1.0f } }
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
	g_TexId = Texture_Load(L"resource/texture/shadow.png");
}

void CircleShadow_Finalize()
{
	SAFE_RELEASE(g_pVertexBuffer);
}

void CircleShadow_Draw(const DirectX::XMFLOAT3& position)
{
	float y = -1000.0f;
	AABB shadow_aabb{ {-0.5f + position.x, position.y-10.0f, position.z-0.5f},
	 {position.x +0.5f, position.y, position.z + 0.5f} };

	for (int i = 0; i < Map_GetObjectsCount(); i++) {
		AABB object = Map_GetObject(i)->aabb;
		bool isHit = Collision_IsOverlapAABB(object, shadow_aabb);
		if (isHit) {
			if (y < object.max.y)
			{
				y = object.max.y;
			}
		}
	}

	Shader3DUnlit_Begin();
	Shader3DUnlit_SetColor({ 1.0f , 1.0f, 1.0f, 1.0f });

	Texture_SetTexture(g_TexId);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	Direct3D_GetContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Shader3DUnlit_SetWorldMatrix(
		XMMatrixScaling(2.0f, 2.0f, 2.0f) *
		XMMatrixTranslation(position.x, y + 0.01f, position.z)
	);

	Direct3D_GetContext()->Draw(NUM_VERTEX, 0);
}
