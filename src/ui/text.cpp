#include "text.h"
#include "sprite.h"
#include "texture.h"
#include <cstring>

using namespace DirectX;

static constexpr int FONT_COLS = 16;
static constexpr int FONT_ROWS = 8;

static constexpr int FONT_CELL_W = 32;
static constexpr int FONT_CELL_H = 32;

static int g_FontTexId = -1;
static int g_NumberTexId = -1;
int GetFontIndex(char c);
int GetNumberIndex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    return -1;
}
void Text_Initialize()
{
    g_FontTexId = Texture_Load(L"resource/texture/consolab_ascii_512.png");
    if (g_FontTexId < 0)
    {
        OutputDebugStringA("FAILED TO LOAD FONT TEXTURE\n");
    }
}

void Text_Finalize()
{
}

static void DrawChar(
    float x,
    float y,
    char c,
    const XMFLOAT4& color,
    float size
)
{
    if (c < 32 || c > 127) return;

    int index = c - 32;
    int col = index % FONT_COLS;
    int row = index / FONT_COLS;

    int px = col * FONT_CELL_W;
    int py = row * FONT_CELL_H;

    Sprite_Draw(
        g_FontTexId,
        x, y,
        FONT_CELL_W * size,
        FONT_CELL_H * size,
        px, py,
        FONT_CELL_W,
        FONT_CELL_H,
        0.0f,
        color
    );
}

//{static void DrawChar(float x, float y, char c, const XMFLOAT4& color, float size)
//        Sprite_Draw(
//            g_NumberTexId,
//            x, y,
//            NUM_CELL_W * size,
//            NUM_CELL_H * size,
//            px, 0,
//            NUM_CELL_W,
//            NUM_CELL_H,
//            0.0f,
//            color
//    Sprite_Draw(
//        g_FontTexId,
//        x, y,
//        FONT_CELL_W * size,
//        FONT_CELL_H * size,
//        px, py,
//        FONT_CELL_W,
//        FONT_CELL_H,
//        0.0f,
//        color
void Text_Draw(
    float x,
    float y,
    const char* text,
    const XMFLOAT4& color,
    float size
)
{
    if (!text) return; 
    float cx = x;
    while (*text)
    {
        DrawChar(cx, y, *text, color, size);
    cx += FONT_CELL_W * size; text++;
    }
}
int GetFontIndex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0'; // 0..9

    if (c >= 'A' && c <= 'Z')
        return 10 + (c - 'A');

    return -1;
}

