/*==============================================================================

   マップの管理 [map.h]
														 Author : Harada Ren
														 Date   : 2025/11/10
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef MAP_H
#define MAP_H
#include <DirectXMath.h>
#include "collision.h"

enum ObjectKind {
	field,
	cube1,
	cube2,
	tree,
	rock
};

struct MapObject {
	ObjectKind KindId;
	DirectX::XMFLOAT3 Position;
	AABB aabb;
};

void Map_Initialize();
void Map_Finalize();
//void Map_Update(double elapsed_time);
void Map_Draw();

int Map_GetObjectsCount();

//マップの先頭アドレスを返す
const MapObject* Map_GetObject(int index);

#endif //MAP_H