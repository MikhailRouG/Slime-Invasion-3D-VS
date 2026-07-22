#ifndef PLAYER_H
#define PLAYER_H

#include <DirectXMath.h>
#include "collision.h"
void Player_SetDevice(ID3D11Device* div, ID3D11DeviceContext* con);
void Player_Initialize(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front);
void Player_Finalize();
void Player_Update(double elapsed_time);
void Player_Draw();
void Player_DepthDraw();
void Player_TakeDamage(int damage);
int Player_GetHp();
int Player_GetMaxHp();
const DirectX::XMFLOAT3& Player_GetPosition();
const DirectX::XMFLOAT3& Player_GetFront();
void Player_SetScaling(float scale);
AABB Player_GetAABB();
AABB Player_ConvertPositionToAABB(const DirectX::XMVECTOR& position);

#endif //PLAYER_H