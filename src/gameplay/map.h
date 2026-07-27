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

void Map_DepthDraw();

int Map_GetObjectsCount();

//�}�b�v�̐擪�A�h���X��Ԃ�
const MapObject* Map_GetObject(int index);

#endif //MAP_H