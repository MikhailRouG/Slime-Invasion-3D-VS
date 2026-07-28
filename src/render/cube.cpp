#include "cube.h"
#include "direct3d.h"
#include "shader3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "texture.h"
#include "shader_depth.h"
static constexpr int NUM_INDEX = 2 * 3 * 6;
static constexpr int NUM_VERTEX = 4 * 6;

static ID3D11Buffer* g_pVertexBuffer = nullptr;
static ID3D11Buffer* g_pIndexBuffer = nullptr;

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

struct Vertex3d{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 texcoord; //UV
};

static Vertex3d g_CubeVertex[NUM_VERTEX]{
	{{-0.5f,  0.5f, -0.5f}, {0.0f,0.0f,-1.0f}, { 1.0f,1.0f,1.0f,1.0f },{0.0f,0.0f}},  //0
	{{ 0.5f, -0.5f, -0.5f}, {0.0f,0.0f,-1.0f}, {1.0f,1.0f,1.0f,1.0f},{0.25f,0.25f}}, //1
	{{-0.5f, -0.5f, -0.5f}, {0.0f,0.0f,-1.0f}, {1.0f,1.0f,1.0f,1.0f},{0.0f,0.25f}}, //2
	//{{-0.5f,  0.5f, -0.5f},{0.0f,0.0f,-1.0f}, {1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f}},
	{{ 0.5f,  0.5f, -0.5f}, {0.0f,0.0f,-1.0f}, {1.0f,1.0f,1.0f,1.0f},{0.25f,0.0f}}, //3
	//{{ 0.5f, -0.5f, -0.5f}, {0.0f,0.0f,-1.0f}, {1.0f,1.0f,1.0f,1.0f},{0.25f,0.25f}},

	{{0.5f,  0.5f, -0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.0f}}, //4
	{{0.5f, -0.5f,  0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.25f}},
	{{0.5f, -0.5f, -0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.25f}},
	//{{0.5f,  0.5f, -0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.0f}},
	{{0.5f,  0.5f,  0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.0f}},
	//{{0.5f, -0.5f,  0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.25f}},

	{{-0.5f,  0.5f, -0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.25f}}, //8
	{{ 0.5f,  0.5f,  0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.75f,0.0f}},
	{{ 0.5f,  0.5f, -0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.75f,0.25f}},
	//{{-0.5f,  0.5f, -0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.25f}},
	{{-0.5f,  0.5f,  0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.0f}},
	//{{ 0.5f,  0.5f,  0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.75f,0.0f}},

	{{-0.5f,  0.5f, -0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{1.0f,0.0f}}, //12
	{{-0.5f, -0.5f, -0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{1.0f,0.25f}},
	{{-0.5f, -0.5f,  0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.75f,0.25f}},
	//{{-0.5f,  0.5f, -0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{1.0f,0.0f}},
	//{{-0.5f, -0.5f,  0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.75f,0.25f}},
	{{-0.5f,  0.5f,  0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.75f,0.0f}},
	
	{{-0.5f, -0.5f,  0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.0f,0.25f}}, //16
	{{ 0.5f, -0.5f, -0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.5f}},
	{{ 0.5f, -0.5f,  0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.25f}},
	//{{-0.5f, -0.5f,  0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.0f,0.25f}},
	{{-0.5f, -0.5f, -0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.0f,0.5f}},
	//{{ 0.5f, -0.5f, -0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.5f}},

	{{-0.5f, -0.5f,  0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.5f}}, //20
	{{ 0.5f, -0.5f,  0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.5f}},
	{{ 0.5f,  0.5f,  0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.25f}},
	//{{-0.5f, -0.5f,  0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.5f}},
	//{{ 0.5f,  0.5f,  0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f,1.0f,1.0f},{0.25f,0.25f}},
	{{-0.5f,  0.5f,  0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f,1.0f,1.0f},{0.5f,0.25f}}
};

static unsigned short g_CubeIndex[NUM_INDEX]{
	0,1,2,0,3,1,
	4,5,6,4,7,5,
	8,9,10,8,11,9,
	12,13,14,12,14,15,
	16,17,18,16,19,17,
	20,21,22,20,22,23
};

void Cube_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	g_pDevice = pDevice;
	g_pContext = pContext;

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex3d) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = g_CubeVertex;

	g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);


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
	Shader3d_Begin();

	Shader3d_SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	Texture_SetTexture(texid);


	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	g_pContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	Shader3d_SetWorldMatrix(mtxWorld);

	g_pContext->DrawIndexed(NUM_INDEX, 0, 0);

}

void Cube_Draw(int texid, const DirectX::XMMATRIX mtxWorld, const DirectX::XMFLOAT4& color)
{
	Shader3d_Begin();

	Shader3d_SetColor(color);

	Texture_SetTexture(texid);


	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	g_pContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	Shader3d_SetWorldMatrix(mtxWorld);

	g_pContext->DrawIndexed(NUM_INDEX, 0, 0);
}

void Cube_DepthDraw(int texid, const DirectX::XMMATRIX mtxWorld)
{
	ShaderDepth_Begin();

	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	g_pContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	ShaderDepth_SetWorldMatrix(mtxWorld);

	g_pContext->DrawIndexed(NUM_INDEX, 0, 0);

}

const AABB Cube_GetAABB(const DirectX::XMFLOAT3& position){
	return {
		{position.x - 0.5f,position.y - 0.5f,position.z - 0.5f},
		{position.x + 0.5f,position.y + 0.5f,position.z + 0.5f}
	};
}