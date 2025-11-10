/*==============================================================================

   É}ÉbÉvÇÃä«óù [map.cpp]
														 Author : Harada Ren
														 Date   : 2025/11/10
--------------------------------------------------------------------------------

==============================================================================*/
#include "map.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "cube.h"
#include "texture.h"


static MapObject g_MapObjects[]{
	{1,{1.0f,0.5f,0.0f}},
	{1,{-1.0f,0.5f,0.0f}},
	{1,{0.0f,0.5f,1.0f}},
	{1,{1.0f,0.5f,1.0f}},
	{2,{-1.0f,0.5f,1.0f}},
	{2,{0.0f,0.5f,2.0f}},
	{2,{1.0f,0.5f,2.0f}},
	{2,{-1.0f,0.5f,2.0f}},
	{2,{1.0f,1.5f,1.0f}},
};

static int g_Cube1TexId = -1;
static int g_Cube2TexId = -1;

void Map_Initialize(){
	g_Cube1TexId = Texture_Load(L"resource/texture/BoxTestTexture.png");
	g_Cube2TexId = Texture_Load(L"resource/texture/BoxTestTexture2.png");

}

void Map_Finalize()
{
}

void Map_Draw(){
	XMMATRIX mtxWorld;
	for (const MapObject& mo : g_MapObjects) {
		switch (mo.KindId) {
		case 1:
			mtxWorld = XMMatrixTranslation(mo.Position.x, mo.Position.y, mo.Position.z);
			Cube_Draw(g_Cube1TexId,mtxWorld);
			break;

		case 2:
			mtxWorld = XMMatrixTranslation(mo.Position.x, mo.Position.y, mo.Position.z);
			Cube_Draw(g_Cube2TexId, mtxWorld);
		default:
			break;
		}
	}
}

int Map_GetObjectsCount(){
	return sizeof(g_MapObjects)/sizeof(g_MapObjects[0]);
}

const MapObject* Map_GetObject(int index){
	return &g_MapObjects[index];
}
