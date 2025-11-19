/*==============================================================================

   íeä€ÇÃä«óù [bullet.h]
														 Author : Harada Ren
														 Date   : 2025/11/12
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef BULLET_H
#define BULLET_H

#include <DirectXMath.h>
#include "collision.h"
#include "map.h"

void Bullet_Initialize();
void Bullet_Finalize();
void Bullet_Update(double elapsed_time);
void Bullet_Draw();

void Bullet_Create(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity);
void Bullet_Destroy(int index);

int Bullet_GetCount();

AABB Bullet_GetAABB(int index);
const DirectX::XMFLOAT3& Bullet_GetPosition(int index);

#endif //BULLET_H