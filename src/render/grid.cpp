#include "grid.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "shader3d.h"


static constexpr int GRID_H_COUNT = 10;
static constexpr int GRID_V_COUNT = 10;
static constexpr int GRID_H_LINE_COUNT = GRID_H_COUNT + 1;
static constexpr int GRID_V_LINE_COUNT = GRID_V_COUNT + 1;
static constexpr int NUM_VERTEX = GRID_H_LINE_COUNT * 2 + GRID_V_LINE_COUNT * 2;

static ID3D11Buffer* g_pVertexBuffer = nullptr;

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

struct Vertex3d {
	XMFLOAT3 position;
	XMFLOAT4 color;
};

static Vertex3d g_GridVertex[NUM_VERTEX]{};


void Grid_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	g_pDevice = pDevice;
	g_pContext = pContext;


	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex3d) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;


	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = g_GridVertex;



	float x = -5.0f;
	float z = -5.0f;
	for (int i = 0; i < NUM_VERTEX; i += 2) {
		if (i < GRID_H_LINE_COUNT * 2) {
			g_GridVertex[i] = { { x, 0.0f,  5.0f }, {0.0f, 1.0f, 0.0f, 1.0f} };
			g_GridVertex[i + 1] = { { x, 0.0f, -5.0f }, {0.0f, 1.0f, 0.0f, 1.0f} };
			x += 1.0f;
		}
		else {
			g_GridVertex[i] = { {  5.0f, 0.0f, z }, {0.0f, 1.0f, 0.0f, 1.0f} };
			g_GridVertex[i + 1] = { { -5.0f, 0.0f, z }, {0.0f, 1.0f, 0.0f, 1.0f} };
			z += 1.0f;
		}
	}
	

	g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);
}

void Grid_Finalize(void){
	SAFE_RELEASE(g_pVertexBuffer);
}

void Grid_Draw(void){
	Shader3d_Begin();

	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	XMMATRIX mtxWorld = XMMatrixIdentity();
	Shader3d_SetWorldMatrix(mtxWorld);


	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	g_pContext->Draw(NUM_VERTEX, 0);
}
