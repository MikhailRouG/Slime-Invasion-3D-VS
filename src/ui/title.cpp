#include "title.h"
#include "time.h"
#include "texture.h"
#include "sprite.h"
#include "key_logger.h"
#include "scene.h"
#include "text.h"
#include "direct3d.h"
static int g_TitleBgTexId2 = -1;
const int playBtnX = 760;
const int playBtnY = 600;
const int playBtnW = 400;
const int playBtnH = 150;
 float playBtn = 0.7f;

 const int exitBtnX = 760;
 const int exitBtnY = 800;
 const int exitBtnW = 400;
 const int exitBtnH = 150;
 float exitBtn = 0.7f;
void Title_Initialize()
{
	g_TitleBgTexId2 = Texture_Load(L"resource/texture/MainMenu.png");
}
void Title_Finalize()
{
	if (g_TitleBgTexId2 >= 0) {
		g_TitleBgTexId2 = -1;
	}
}
void Title_Update(double elapsed_time)
{
	if (KeyLogger_IsTrigger(KK_ENTER))
	{
		Scene_Change(SCENE_GAME);
	}
}
void Title_Draw()
{
	Direct3D_SetDepthEnable(false);
	Sprite_Begin();
	Sprite_Draw(
		Direct3D_GetBackBufferWidth() * 0.8f,
		Direct3D_GetBackBufferHeight() * 0.04f,
		256, 256
	);
	Direct3D_SetOffscreenTexture(0);
	Sprite_Draw(g_TitleBgTexId2,
		0.0f, 0.0f,
		0,0,1920,1080);
	//Text_Draw(500, 810, "Press Enter to continue", { 1,1,1,1 }, 1.3f);
	Sprite_Draw(
		0, 
		playBtnX, playBtnY,
		playBtnW, playBtnH,
		0, 0, 1, 1,
		0.0f,
		{ 0, 0, 0, playBtn}
	);
	Text_Draw(880, 660, "Play", { 1,1,1,1 }, 1.3f);

	Sprite_Draw(
		0,
		exitBtnX, exitBtnY,
		exitBtnW, exitBtnH,
		0, 0, 1, 1,
		0.0f,
		{ 0, 0, 0, exitBtn }
	);
	Text_Draw(880, 860, "Exit", { 1,1,1,1 }, 1.3f);
	Direct3D_SetDepthEnable(true);
}

void TitleUI_HandleClick(int mx, int my, bool  isClick)
{
	playBtn = 0.7f;
	exitBtn = 0.7f;
	if (mx >= playBtnX && mx <= (playBtnX + playBtnW) &&
		my >= playBtnY && my <= (playBtnY + playBtnH))
	{
		if (!isClick) playBtn = 0.5f;
		else Scene_Change(SCENE_GAME);
	}
	if (mx >= exitBtnX && mx <= (exitBtnX + exitBtnW) &&
		my >= exitBtnY && my <= (exitBtnY + exitBtnH))
	{
		if (isClick) {
			PostQuitMessage(0); 
		}
		else {
			exitBtn = 0.4f; // Ёффект наведени€
		}
	}
}