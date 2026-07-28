#include "sprite_anim.h"
#include "sprite.h"
#include "texture.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "billboard.h"
#include <cstring>
#include <stdio.h>
struct AnimPatternData {
	int m_TextureId = -1;
	int m_PatternMax = 0;
	int m_HPatternMax = 0;
	XMUINT2 m_StartPosition{ 0,0 };
	XMUINT2 m_PatternSize{ 0,0 };
	double m_seconds_per_pattern = 0.1;
	bool m_IsLooped = true;
};

struct AnimPlayData {
	int m_PatternId = -1;
	int m_PatternNum = 0;
	double m_accumulated_time = 0.0;
	bool m_IsStopped = false;
};


static constexpr int ANIM_PATTERN_MAX = 1024;
static AnimPatternData g_AnimPattern[ANIM_PATTERN_MAX];
static constexpr int ANIM_PLAY_MAX = 512;
static AnimPlayData g_AnimPlay[ANIM_PLAY_MAX];



void SpriteAnim_Initialize() {
	for (AnimPatternData& data : g_AnimPattern) {
		data.m_TextureId = -1;
	}

	for (AnimPlayData& data : g_AnimPlay) {
		data.m_PatternId = -1;
		data.m_IsStopped = false;
	}

}

void SpriteAnim_Finalize() {
	for (int i = 0; i < ANIM_PLAY_MAX; i++) {
		g_AnimPlay[i].m_PatternId = -1;
	}
	for (int i = 0; i < ANIM_PATTERN_MAX; i++) {
		g_AnimPattern[i].m_TextureId = -1;
	}
}

void SpriteAnim_Update(double elapsed_time) {
	for (int i = 0; i < ANIM_PLAY_MAX; i++) {
		if (g_AnimPlay[i].m_PatternId < 0) {
			continue;
		}

		AnimPatternData* pAnimPatternData = &g_AnimPattern[g_AnimPlay[i].m_PatternId];

		if (g_AnimPlay[i].m_accumulated_time >= pAnimPatternData->m_seconds_per_pattern) {
			g_AnimPlay[i].m_PatternNum++;



			if (g_AnimPlay[i].m_PatternNum >= pAnimPatternData->m_PatternMax) {

				if (pAnimPatternData->m_IsLooped) {
					g_AnimPlay[i].m_PatternNum = 0;
				}
				else {
					g_AnimPlay[i].m_PatternNum = pAnimPatternData->m_PatternMax - 1;
					g_AnimPlay[i].m_IsStopped = true;
				}
			}

			g_AnimPlay[i].m_accumulated_time -= pAnimPatternData->m_seconds_per_pattern;

		}
		g_AnimPlay[i].m_accumulated_time += elapsed_time;
	}
}

void SpriteAnim_Draw(int playid, float dx, float dy, float dw, float dh,bool IsFlipped,const XMFLOAT4 color) {

	int anim_pattern_id = g_AnimPlay[playid].m_PatternId;
	AnimPatternData* pAnimPatternData = &g_AnimPattern[anim_pattern_id];
	Sprite_Draw(pAnimPatternData->m_TextureId,
		dx, dy, dw, dh,
		pAnimPatternData->m_StartPosition.x
		+ pAnimPatternData->m_PatternSize.x
		* (g_AnimPlay[playid].m_PatternNum % pAnimPatternData->m_HPatternMax),

		pAnimPatternData->m_StartPosition.y + pAnimPatternData->m_PatternSize.y
		* (g_AnimPlay[playid].m_PatternNum / pAnimPatternData->m_HPatternMax),

		pAnimPatternData->m_PatternSize.x,
		pAnimPatternData->m_PatternSize.y,
		IsFlipped,
		false,
		color
	);

}

void BillboardAnim_Draw(int playid, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& scale, const DirectX::XMFLOAT2& pivot) {
	int anim_pattern_id = g_AnimPlay[playid].m_PatternId;
	if (anim_pattern_id < 0) return;

	AnimPatternData* p = &g_AnimPattern[anim_pattern_id];
	int frame = g_AnimPlay[playid].m_PatternNum;

	int col = frame % p->m_HPatternMax;
	int row = frame / p->m_HPatternMax;

	float uw = (float)p->m_PatternSize.x / Texture_Width(p->m_TextureId);
	float vh = (float)p->m_PatternSize.y / Texture_Height(p->m_TextureId);
	float u = (float)(p->m_StartPosition.x + col * p->m_PatternSize.x) / Texture_Width(p->m_TextureId);
	float v = (float)(p->m_StartPosition.y + row * p->m_PatternSize.y) / Texture_Height(p->m_TextureId);

	Billboard_Draw(
		p->m_TextureId,
		position,
		scale,
		{ 1, 1, 1, 1 },
		pivot,
		{ uw, vh }, 
		{ u, v }    
	);
}

int SpriteAnim_RegisterPattern(int texid, int pattern_max, int h_pattern_max, double m_seconds_per_pattern,
	const DirectX::XMUINT2& pattern_size, const DirectX::XMUINT2& start_position, bool is_looped) {
	for (int i = 0; i < ANIM_PATTERN_MAX; i++) {
		if (g_AnimPattern[i].m_TextureId >= 0) continue;

		g_AnimPattern[i].m_TextureId = texid;
		g_AnimPattern[i].m_PatternMax = pattern_max;
		g_AnimPattern[i].m_HPatternMax = h_pattern_max;
		g_AnimPattern[i].m_seconds_per_pattern = m_seconds_per_pattern;
		g_AnimPattern[i].m_PatternSize = pattern_size;
		g_AnimPattern[i].m_StartPosition = start_position;
		g_AnimPattern[i].m_IsLooped = is_looped;
		return i;
	}
	return -1;
}

int SpriteAnim_CreatePlayer(int anim_pattern_id) {
	for (int i = 0; i < ANIM_PLAY_MAX; i++) {
		if (g_AnimPlay[i].m_PatternId >= 0) continue;

		g_AnimPlay[i].m_PatternId = anim_pattern_id;
		g_AnimPlay[i].m_accumulated_time = 0.0;
		g_AnimPlay[i].m_PatternNum = 0;

		g_AnimPlay[i].m_IsStopped = false;
		return i;
	}
	return -1;
}

bool SpriteAnim_IsStopped(int index){
	return g_AnimPlay[index].m_IsStopped;
}

void SpriteAnim_DestroyPlayer(int index){
	g_AnimPlay[index].m_PatternId = -1; 
}

int SpriteAnim_GetPatternNum(int playid)
{
	if (playid < 0 || playid >= ANIM_PLAY_MAX || g_AnimPlay[playid].m_PatternId < 0) {
		return -1;
	}
	return g_AnimPlay[playid].m_PatternNum;
}
