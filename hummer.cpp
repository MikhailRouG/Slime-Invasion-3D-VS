#include "hummer.h"
#include "model.h"
#include <DirectXMath.h>
#include "player.h"
using namespace DirectX;
#include "collision.h"
#include <vector>
#include "cube.h"
#include "enemy.h"
#include "enemy_normal.h"
#include "score.h"
#include "bullet_hit_effect.h"
#include <ctime>
#include "particle_test.h"
#include "direct3d.h"
static float g_PrevAngle = 0.0f;

static constexpr float HAMMER_HIT_RADIUS = 1.0f;
static const XMFLOAT3 HAMMER_HIT_LOCAL = { 0.0f, 4.5f, 0.0f };
float g_CarrentRadius;
XMFLOAT3 g_HammerHitWorld;
static NormalEmitter* g_HitEmitter = nullptr;

static MODEL* g_HummerModel;
static float g_RotateAngle = 0.0f;
float g_RotateSpeed;
int g_Speed;
int g_Damage;
float g_Scale,g_CurrentScale, g_ScalingTime;
void Hummer_Initialize()
{
	g_HummerModel = ModelLoad("resource/model/Hummer.fbx",0.3f);
    g_Speed = 200;
    g_Damage = 20;
    srand((unsigned int)time(nullptr));
    g_CarrentRadius = HAMMER_HIT_RADIUS;
    g_Scale = 1;
    g_CurrentScale = g_Scale;
    g_HitEmitter = new NormalEmitter(
        2000,
        { 0.0f,2.0f,0.0f },
        2000.0,
        false
    );
}

void Hummer_Finalize()
{
	ModelRelease(g_HummerModel);
    delete g_HitEmitter;
}

void Hummer_Update(double elapsed_time)
{
    g_PrevAngle = g_RotateAngle;
    g_RotateSpeed += elapsed_time * g_Speed/100;
    g_RotateAngle += (float)elapsed_time * XM_PIDIV2 * g_Speed / 100;
    g_RotateAngle = g_RotateAngle;

    Sphere hammer = Hummer_GetHitSphere();

    for (int i = 0; i < Enemy_GetEnemyCount(); i++)
    {
        Enemy* enemy = Enemy_GetEnemy(i);

        if (enemy->isHit())
            continue;

        Sphere enemySphere = enemy->GetCollision();
        XMFLOAT3 hitPoint, hitNormal;
        float penetration;
        if (Collision_SphereContact(hammer, enemySphere, hitPoint, hitNormal, penetration))
        {
            BulletHitEffect_Create(hitPoint);
            g_HitEmitter->SetPosition({hitPoint.x,hitPoint.y,hitPoint.z});
            g_HitEmitter->EmitOnce(300);

            enemy->Damage(g_Damage, g_Speed/100);
            if (enemy->isDestroy())
            {
                Score_AddScore(enemy ->GetLvl() * 2);
                Hummer_SetScale();
            }
        }
    }
    g_HitEmitter->Update(elapsed_time);
    if (elapsed_time == 0) return;
    char buf[128];
    sprintf_s(buf, "value = %.3f\n", elapsed_time);
    OutputDebugStringA(buf);
}

void Hummer_Draw()
{
    Direct3D_SetDepthEnable(true);
    XMMATRIX mtxWorldPos = XMMatrixTranslation(Player_GetPosition().x, Player_GetPosition().y +1.5f, Player_GetPosition().z);

    XMMATRIX mtxToPivot = XMMatrixTranslation(0.0f, 0.5f, 0.0f);

    if (g_CurrentScale < g_Scale)
    {
        g_CurrentScale += 0.001f;
        if (g_CurrentScale > g_Scale) g_CurrentScale = g_Scale;
        Player_SetScaling(g_CurrentScale);
    }
    XMMATRIX mtxToScale = XMMatrixScaling(g_CurrentScale, g_CurrentScale, g_CurrentScale);
    g_CarrentRadius = HAMMER_HIT_RADIUS * g_CurrentScale;
    XMMATRIX mtxRotate =
        XMMatrixRotationZ(XM_PIDIV2) *   
        XMMatrixRotationY(g_RotateAngle); 

    XMMATRIX mtxBack = XMMatrixTranslation(0.0f, -0.5f, 0.0f);

    XMMATRIX world =
        mtxToScale*
        mtxToPivot *
        mtxRotate *
        mtxBack *
        mtxWorldPos;

    XMVECTOR localHit = XMLoadFloat3(&HAMMER_HIT_LOCAL);
    XMVECTOR worldHit = XMVector3Transform(localHit, world);
    XMStoreFloat3(&g_HammerHitWorld, worldHit);
    ModelDraw(g_HummerModel, world);
    Direct3D_SetDepthStencilStateDepthWhiteDisable();
    Direct3D_SetAlphaBlendAdd();

    g_HitEmitter->Draw();
    Direct3D_SetAlphaBlendTransparent();
    Direct3D_SetDepthEnable(false);
#ifdef _DEBUG
    XMMATRIX dbg =
        XMMatrixScaling(
            g_CarrentRadius * 2.0f,
            g_CarrentRadius * 2.0f,
            g_CarrentRadius * 2.0f
        ) *
        XMMatrixTranslation(
            g_HammerHitWorld.x,
            g_HammerHitWorld.y,
            g_HammerHitWorld.z
        );

    Cube_Draw(0, dbg);
#endif
}
Sphere Hummer_GetHitSphere()
{
    return { g_HammerHitWorld, g_CarrentRadius };
}
int Hummer_GetSpeedAtk()
{
    return g_Speed;
}
int Hummer_GetAttackDamage()
{
    return g_Damage;
}

void Hummer_SetScale()
{
    float i = Score_GetScore() / 10;
    float b = 1 + i / 20;
    g_Scale = b;
    char buf[64];

    sprintf_s(buf, "Scale: %.2f\n", g_Scale);

    OutputDebugStringA(buf);
}

void Hummer_SetSpeed(int speed)
{
    g_Speed += speed;
    if (g_Speed > 999) g_Speed = 999;
}

void Hummer_SetDamage(int dmg)
{
    g_Damage += dmg;
}


