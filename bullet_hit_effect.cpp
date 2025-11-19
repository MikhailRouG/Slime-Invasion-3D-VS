/*==============================================================================

   弾衝突エフェクト [bullet_hit_effect.cpp]
														 Author : Harada Ren
														 Date   : 2025/11/19
--------------------------------------------------------------------------------

==============================================================================*/
#include "bullet_hit_effect.h"
#include "texture.h"
#include "sprite_anim.h"
#include "billboard.h"
using namespace DirectX;


class BulletHitEffect {
private:
	XMFLOAT3 m_position{};

public:
	BulletHitEffect(const XMFLOAT3& position)
		: m_position(position) {
	}

	void Update();
	void Draw() const;
	bool IsDestroy() const {

	}
};

void BulletHitEffect_Initialize()
{
}

void BulletHitEffect_Finalize()
{
}

void BulletHitEffect_Update()
{
}

void BulletHitEffect_Draw()
{
}

void BukketHitEffect_Create(const DirectX::XMFLOAT3& position)
{
}
