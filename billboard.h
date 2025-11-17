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
void Billboard_Draw(int texId, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& scale, const DirectX::XMFLOAT2& pivot = {0.0f, 0.0f});
//テクスチャカット
void Billboard_Draw(int texId, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& scale,
	const DirectX::XMUINT4& tex_cut,
	const DirectX::XMFLOAT2& pivot = { 0.0f, 0.0f });//切り取りの座標、幅高さ );


#endif // BILLBOARD_H
