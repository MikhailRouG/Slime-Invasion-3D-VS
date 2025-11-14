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
#include "light.h"
#include "meshfield.h"
#include "player_camera.h"
#include "model.h"

static MapObject g_MapObjects[]{
	{field,{0.0f,0.0f,0.0f},{{ -25.0f,-1.0f,-25.0f},{25.0f,0.0f,25.0f}}},
	{cube1,{1.0f,0.5f,0.0f}},
	{cube1,{-1.0f,0.5f,0.0f}},
	{cube1,{0.0f,0.5f,1.0f}},
	{cube1,{1.0f,0.5f,1.0f}},
	{cube2,{-1.0f,0.5f,1.0f}},
	{cube2,{0.0f,0.5f,2.0f}},
	{cube2,{1.0f,0.5f,2.0f}},
	{cube2,{-1.0f,0.5f,2.0f}},
	{cube2,{1.0f,1.5f,1.0f}},
	{tree,{-10.0f,-3.5f,-5.0f}},
	{rock,{5.0f,0.5f,-5.0f}}
};

static int g_Cube1TexId = -1;
static int g_Cube2TexId = -1;
static MODEL* g_pCup01{};
static MODEL* g_pTree01{};
static MODEL* g_pRock01{};

void Map_Initialize(){
	g_Cube1TexId = Texture_Load(L"resource/texture/BoxTestTexture.png");
	g_Cube2TexId = Texture_Load(L"resource/texture/BoxTestTexture2.png");
	g_pCup01 = ModelLoad("resource/model/cup.fbx", 1.0f);
	g_pTree01 = ModelLoad("resource/model/lowpolytree.fbx", 3.0f);
	//g_pRock01 = ModelLoad("resource/model/Rock1.fbx", 1.0f);

	for (MapObject& o : g_MapObjects) {
		if (o.KindId == cube1 || o.KindId == cube2) {
			o.aabb = Cube_GetAABB(o.Position);
		}
		else if (o.KindId == tree) {
			o.aabb = Model_GetAABB(g_pTree01, o.Position);
		}
		else if (o.KindId == rock) {
			o.aabb = Model_GetAABB(g_pCup01, o.Position);
		}
	}
}

void Map_Finalize(){
	ModelRelease(g_pCup01);
	ModelRelease(g_pTree01);
	//ModelRelease(g_pRock01);

}

void Map_Draw(){
	XMMATRIX mtxWorld;
	for (const MapObject& mo : g_MapObjects) {
		switch (mo.KindId) {
		case field:
			Light_SetSpecularWorld(PlayerCamera_GetPosition(), 5.0f, { 0.2f,0.0f,0.2f,1.0f });
			Meshfield_Draw();
			break;
		case cube1: 
			mtxWorld = XMMatrixTranslation(mo.Position.x, mo.Position.y, mo.Position.z);
			Cube_Draw(g_Cube1TexId,mtxWorld);
			break;

		case cube2:
			mtxWorld = XMMatrixTranslation(mo.Position.x, mo.Position.y, mo.Position.z);
			Cube_Draw(g_Cube2TexId, mtxWorld);
			break;

		case tree: //ñÿ
			mtxWorld = XMMatrixTranslation(mo.Position.x, mo.Position.y, mo.Position.z);
			//ModelDraw(g_pTree01,mtxWorld);
			ModelDraw(g_pCup01, mtxWorld);
			break;

		case rock: //ä‚
			mtxWorld = XMMatrixTranslation(mo.Position.x, mo.Position.y, mo.Position.z);
			ModelDraw(g_pCup01, mtxWorld);
			//ModelDraw(g_pRock01, mtxWorld);
			break;

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
