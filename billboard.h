/*==============================================================================

   ビルボード描画 [billboard.h]
														 Author :
														 Date   : 2025/11/14
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef BILLBOARD_H
#define BILLBOARD_H
#include <DirectXMath.h>
#include "collision.h"

void Billboard_Initialize();
void Billboard_Finalize();
void Billboard_Draw(int texId, const DirectX::XMFLOAT3& position, float scale_x, float scale_y, const DirectX::XMFLOAT2& pivot = {0.0f, 0.0f});


#endif // BILLBOARD_H
