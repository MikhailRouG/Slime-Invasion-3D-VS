#include "sky.h"
using namespace DirectX;
#include "model.h"
#include "shader3d_unlit.h"
#include "texture.h"
static MODEL* g_pModelSky{nullptr};
static XMFLOAT3 g_Position{};
static float g_RotateAngle = 0.0f;
int g_texSky;
void Sky_Initialize()
{
    g_pModelSky = ModelLoad("resource/model/Sky2.fbx",1.0f,true);
    g_texSky = Texture_Load(L"resource/texture/sky2.png");
}

void Sky_Finalize()
{
    ModelRelease(g_pModelSky);
}

void Sky_Update(double elapsed_time)
{
    g_RotateAngle += (float)elapsed_time * XM_PIDIV2/ 300;
}

void Sky_SetPosition(const DirectX::XMFLOAT3& position)
{
    g_Position = position;
}

void Sky_Draw()
{
    Shader3DUnlit_Begin();
    g_Position.y = 0;
    XMMATRIX mtxRotate =
       // XMMatrixRotationZ(XM_PIDIV2) *
        XMMatrixRotationY(g_RotateAngle)
        * XMMatrixTranslationFromVector(XMLoadFloat3(&g_Position));
    //Sampler_SetFilterLinear();
    ModelUnlitDraw(g_pModelSky, mtxRotate, g_texSky);
}
