#ifndef TEXT_H
#define TEXT_H

#include <DirectXMath.h>

void Text_Initialize();
void Text_Finalize();

void Text_Draw(
    float x,
    float y,
    const char* text,
    const DirectX::XMFLOAT4& color = { 1,1,1,1 },
    float size = 1
);
#endif