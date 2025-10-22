/*==============================================================================

   モデル表示 [model.cpp]
														 Author : Harada Ren
														 Date   : 2025/10/20
--------------------------------------------------------------------------------

==============================================================================*/
#include <assert.h>
#include "direct3d.h"
#include "texture.h"
#include "model.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "WICTextureLoader11.h"
#include "shader3d.h"

struct Vertex3d {
	XMFLOAT3 position; // 頂点座標
	XMFLOAT3 normal; //法線
	XMFLOAT4 color; //頂点カラー
	XMFLOAT2 texcoord; //UV
};

static int g_TextureWhite = -1;

MODEL* ModelLoad( const char *FileName, float scale, bool bBlender){
	MODEL* model = new MODEL;

	const std::string modelPath( FileName );

	model->AiScene = aiImportFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(model->AiScene);

	model->VertexBuffer = new ID3D11Buffer*[model->AiScene->mNumMeshes];
	model->IndexBuffer = new ID3D11Buffer*[model->AiScene->mNumMeshes];


	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = model->AiScene->mMeshes[m];

		// 頂点バッファ生成
		{
			Vertex3d* vertex = new Vertex3d[mesh->mNumVertices];

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				if (bBlender) {
					vertex[v].position = XMFLOAT3(mesh->mVertices[v].x * scale, -mesh->mVertices[v].z * scale, mesh->mVertices[v].y * scale);
					vertex[v].normal = XMFLOAT3(mesh->mNormals[v].x, -mesh->mNormals[v].z, mesh->mNormals[v].y);
				}
				else {
					vertex[v].position = XMFLOAT3(mesh->mVertices[v].x * scale, mesh->mVertices[v].y * scale, mesh->mVertices[v].z * scale);
					vertex[v].normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
				}
				vertex[v].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[v].texcoord = XMFLOAT2( mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(Vertex3d) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertex;

			Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &model->VertexBuffer[m]);

			delete[] vertex;
		}


		// インデックスバッファ生成
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
			{
				const aiFace* face = &mesh->mFaces[f];

				assert(face->mNumIndices == 3);

				index[f * 3 + 0] = face->mIndices[0];
				index[f * 3 + 1] = face->mIndices[1];
				index[f * 3 + 2] = face->mIndices[2];
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;

			Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &model->IndexBuffer[m]);

			delete[] index;
		}

	}

	//テクスチャが読み込まれてなかったら
	if (model->AiScene->mNumTextures == 0) {
		g_TextureWhite = Texture_Load(L"resource/texture/white.png");
	}
	else {
		//テクスチャ読み込み
		for (unsigned int i = 0; i < model->AiScene->mNumTextures; i++) {
			aiTexture* aitexture = model->AiScene->mTextures[i];

			ID3D11ShaderResourceView* texture;
			ID3D11Resource* resource;

			CreateWICTextureFromMemory(
				Direct3D_GetDevice(),
				Direct3D_GetContext(),
				(const uint8_t*)aitexture->pcData,
				(size_t)aitexture->mWidth,
				&resource, //releaseする
				&texture);

			assert(texture);

			model->Texture[aitexture->mFilename.data] = texture;
		}

	}

	return model;
}




void ModelRelease(MODEL* model)
{
	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		model->VertexBuffer[m]->Release();
		model->IndexBuffer[m]->Release();
	}

	delete[] model->VertexBuffer;
	delete[] model->IndexBuffer;


	for (std::pair<const std::string, ID3D11ShaderResourceView*> pair : model->Texture)
	{
		pair.second->Release();
	}


	aiReleaseImport(model->AiScene);


	delete model;
}

void ModelDraw(MODEL* model, const DirectX::XMMATRIX& mtxWorld){
	// シェーダーを描画パイプラインに設定
	Shader3d_Begin();

	// プリミティブトポロジ設定
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点シェーダーにワールド座標変換行列を設定
	Shader3d_SetWorldMatrix(mtxWorld);

	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++) {
		//テクスチャの設定
		if (model->AiScene->mNumTextures) {
			aiString texture;
			aiMaterial* aimaterial = model->AiScene->mMaterials[model->AiScene->mMeshes[m]->mMaterialIndex];
			aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);

			if (texture.length != 0) {
			//if (texture != aiString("")) {
				Direct3D_GetContext()->PSSetShaderResources(0, 1, &model->Texture[texture.data]);
			}
		}
		else {
			Texture_SetTexture(g_TextureWhite);
		}

		//マテリアル設定
		MATERIAL material

		// 頂点バッファを描画パイプラインに設定
		UINT stride = sizeof(Vertex3d);
		UINT offset = 0;
		Direct3D_GetContext()->IASetVertexBuffers(0, 1, &model->VertexBuffer[m], &stride, &offset);

		// インデックスバッファを描画パイプラインに設定
		Direct3D_GetContext()->IASetIndexBuffer(model->IndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// ポリゴン描画命令発行
		Direct3D_GetContext()->DrawIndexed(model->AiScene->mMeshes[m]->mNumFaces * 3, 0, 0);
	}
}





