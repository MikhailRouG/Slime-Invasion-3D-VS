/*==============================================================================

   弾衝突エフェクト [bullet_hit_effect.h]
														 Author : Harada Ren
														 Date   : 2025/11/19
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef BULLET_HIT_EFFECT_H
#define BULLET_HIT_EFFECT_H

#include <DirectXMath.h>

void BulletHitEffect_Initialize();
void BulletHitEffect_Finalize();
void BulletHitEffect_Update();
void BulletHitEffect_Draw();

void BukketHitEffect_Create(const DirectX::XMFLOAT3& position);

#endif //BULLET_HIT_EFFECT_H