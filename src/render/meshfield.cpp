#include "meshfield.h"
#include "direct3d.h"
#include "shader_field.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "texture.h"
#include "sampler.h"
#include "cube.h"
#include "camera.h"

static constexpr float FIELD_SIZE = 3.0f;
static constexpr int FIELD_H_COUNT = 300;
static constexpr int FIELD_V_COUNT = 300;
static constexpr int FIELD_H_VERTEX_COUNT = FIELD_H_COUNT + 1;
static constexpr int FIELD_V_VERTEX_COUNT = FIELD_V_COUNT + 1;

static constexpr int NUM_VERTEX = FIELD_H_VERTEX_COUNT * FIELD_V_VERTEX_COUNT;
static constexpr int NUM_INDEX = 3 * 2 * FIELD_H_COUNT * FIELD_V_COUNT;

static ID3D11Buffer* g_pVertexBuffer = nullptr;
static ID3D11Buffer* g_pIndexBuffer = nullptr;

static int g_CubeTexId0 = -1;
static int g_CubeTexId1 = -1;

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

struct Vertex3d {
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 texcoord; //UV
};


static Vertex3d g_MeshfieldVertex[NUM_VERTEX]{};

static unsigned short g_MeshfieldIndex[NUM_INDEX]{};


void Meshfield_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	g_pDevice = pDevice;
	g_pContext = pContext;

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex3d) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;


	for (int z = 0;z < FIELD_V_VERTEX_COUNT;z++) {
		for (int x = 0;x < FIELD_H_VERTEX_COUNT;x++) {
			int index = x + FIELD_H_VERTEX_COUNT * z;
			g_MeshfieldVertex[index].position = { x * FIELD_SIZE,0.0f,z * FIELD_SIZE };
			g_MeshfieldVertex[index].normal = { 0.0f,1.0f,0.0f };
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

	
	int index = 0;
	for (int v = 0;v < FIELD_V_COUNT;v++) {
		for (int h = 0;h < FIELD_H_COUNT;h++) {
			unsigned short v0 = (unsigned short)(h + (v + 0) * FIELD_H_VERTEX_COUNT);
			unsigned short v1 = (unsigned short)(h + (v + 1) * FIELD_H_VERTEX_COUNT);
			unsigned short v2 = (unsigned short)(v1 + 1);
			unsigned short v3 = (unsigned short)(v0 + 1);

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


	bd.ByteWidth = sizeof(unsigned short) * NUM_INDEX;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	sd.pSysMem = g_MeshfieldIndex;

	g_pDevice->CreateBuffer(&bd, &sd, &g_pIndexBuffer);



	g_CubeTexId0 = Texture_Load(L"resource/texture/grass.png");
	g_CubeTexId1 = Texture_Load(L"resource/texture/grass2.png");

	ShaderField_Initialize(pDevice, pContext);
}

void Meshfield_Finalize(void){
	ShaderField_Finalize();
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
}

void Meshfield_Draw() {
	ShaderField_Begin();

	Texture_SetTexture(g_CubeTexId0, 0);
	Texture_SetTexture(g_CubeTexId1, 1);


	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	g_pContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float offset_x = FIELD_H_COUNT * FIELD_SIZE * 0.5f;
	float offset_z = FIELD_V_COUNT * FIELD_SIZE * 0.5f;
	XMMATRIX mtxWorld = XMMatrixTranslation(-offset_x, 0.0f, -offset_z);

	ShaderField_SetWorldMatrix(mtxWorld);


	ShaderField_SetColor({ 2.0f,2.0f,2.0f,1.0f });

	g_pContext->DrawIndexed(NUM_INDEX, 0, 0);
}
