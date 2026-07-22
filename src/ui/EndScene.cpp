#include "EndScene.h"
#include "time.h"
#include "texture.h"
#include "sprite.h"
#include "key_logger.h"
#include "scene.h"
#include "score.h"
#include "text.h"
#include "mouse.h" 
#include "direct3d.h"

static int g_TitleEndBgTexId2 = -1;

// Координаты кнопок (такие же, как в Title)
const int menuBtnX = 760;
const int menuBtnY = 600;
const int menuBtnW = 400;
const int menuBtnH = 150;
static float menuBtnAlpha = 0.7f;

const int exitBtnX = 760;
const int exitBtnY = 800;
const int exitBtnW = 400;
const int exitBtnH = 150;
static float exitBtnAlpha = 0.7f;

// Функция для обработки логики кнопок
void EndUI_HandleClick(int mx, int my, bool isClick)
{
    // Сброс подсветки
    menuBtnAlpha = 0.7f;
    exitBtnAlpha = 0.7f;

    // Проверка кнопки Menu (вместо Play)
    if (mx >= menuBtnX && mx <= (menuBtnX + menuBtnW) &&
        my >= menuBtnY && my <= (menuBtnY + menuBtnH))
    {
        if (isClick) {
            Scene_Change(SCENE_TITLE);
        }
        else {
            menuBtnAlpha = 0.4f; // Эффект наведения
        }
    }

    // Проверка кнопки Exit
    if (mx >= exitBtnX && mx <= (exitBtnX + exitBtnW) &&
        my >= exitBtnY && my <= (exitBtnY + exitBtnH))
    {
        if (isClick) {
            PostQuitMessage(0);
        }
        else {
            exitBtnAlpha = 0.4f; // Эффект наведения
        }
    }
}

void EndScene_Initialize()
{
    g_TitleEndBgTexId2 = Texture_Load(L"resource/texture/EndMenu.png");
}

void EndScene_Finalize()
{
    if (g_TitleEndBgTexId2 >= 0) {
        g_TitleEndBgTexId2 = -1;
    }
}

void EndScene_Update(double elapsed_time)
{
}

void EndScene_Draw()
{
    Direct3D_SetDepthEnable(false);
    Sprite_Begin();
    Sprite_Draw(
        Direct3D_GetBackBufferWidth() * 0.8f,
        Direct3D_GetBackBufferHeight() * 0.04f,
        256, 256
    );
    Direct3D_SetOffscreenTexture(0);
    Sprite_Draw(g_TitleEndBgTexId2, 0.0f, 0.0f, 0, 0, 1920, 1080);

    char buf[64];
    sprintf_s(buf, "Score: %d", Score_GetScore());
    Text_Draw(600, 450, buf, { 1,1,1,1 }, 2.5f); 

    Sprite_Draw(0, menuBtnX, menuBtnY, menuBtnW, menuBtnH, 0, 0, 1, 1, 0.0f, { 0, 0, 0, menuBtnAlpha });
    Text_Draw(860, 660, "Menu", { 1,1,1,1 }, 1.3f);

    Sprite_Draw(0, exitBtnX, exitBtnY, exitBtnW, exitBtnH, 0, 0, 1, 1, 0.0f, { 0, 0, 0, exitBtnAlpha });
    Text_Draw(880, 860, "Exit", { 1,1,1,1 }, 1.3f);
    Direct3D_SetDepthEnable(true);
}