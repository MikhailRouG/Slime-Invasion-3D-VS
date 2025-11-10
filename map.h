/*==============================================================================

   マップの管理 [map.h]
														 Author : Harada Ren
														 Date   : 2025/11/10
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef MAP_H
#define MAP_H
#include <DirectXMath.h>

struct MapObject {
	int KindId;
	DirectX::XMFLOAT3 Position;
};

void Map_Initialize();
void Map_Finalize();
//void Map_Update(double elapsed_time);
void Map_Draw();

int Map_GetObjectsCount();

//マップの先頭アドレスを返す
const MapObject* Map_GetObject(int index);

#endif //MAP_H