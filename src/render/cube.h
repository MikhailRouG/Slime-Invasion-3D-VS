#ifndef CUBE_H
#define CUBE_H

#include <d3d11.h>
#include <DirectXMath.h>
#include "collision.h"

void Cube_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Cube_Finalize(void);
void Cube_Draw(int texid,const DirectX::XMMATRIX mtxWorld);
void Cube_Draw(int texid, const DirectX::XMMATRIX mtxWorld, const DirectX::XMFLOAT4& color);
void Cube_DepthDraw(int texid, const DirectX::XMMATRIX mtxWorld);
const AABB Cube_GetAABB(const DirectX::XMFLOAT3& position);

#endif //CUBE_H