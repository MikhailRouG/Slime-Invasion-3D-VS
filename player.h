/*==============================================================================

   ÉvÉåÉCÉÑÅ[êßå‰ [player.h]
														 Author : Harada Ren
														 Date   : 2025/10/31
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef PLAYER_H
#define PLAYER_H

#include <DirectXMath.h>
#include "collision.h"

void Player_Initialize(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front);
void Player_Finalize();
void Player_Update(double elapsed_time);
void Player_Draw();

const DirectX::XMFLOAT3& Player_GetPosition();
const DirectX::XMFLOAT3& Player_GetFront();

AABB Player_GetAABB();

#endif //PLAYER_H