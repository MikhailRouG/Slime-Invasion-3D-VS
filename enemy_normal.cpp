#include "enemy_normal.h"
#include "collision.h"
#include "player.h"
using namespace DirectX;
#include "texture.h"
#include "cube.h"
#include "shader3d_unlit.h"
#include "player_camera.h"
#include <random>
#include "billboard.h"

 static MODEL* g_EnemyNormalModel = nullptr;
 float g_Speed = 3.0f;
 float m_CurrentSpeed;
 static void DrawEnemyHpBar(const EnemyNormal* enemy);
 void EnemyNormal_Initialize()
 {
     g_EnemyNormalModel = ModelLoad("resource/model/slime.fbx", 1.0f);
 }
 void EnemyNormal_Finalize()
 {
     ModelRelease(g_EnemyNormalModel);
 }
 void EnemyNormal::Update(double elapsed_time) {
     Enemy::Update(elapsed_time);
     float dt = (float)elapsed_time;

     m_CurrentSpeed = std::min(12.0f, 3.0f + Get_GamePassedTime() / 100.0f);

     if (m_HitTimer > 0.0f)      m_HitTimer -= dt;
     if (m_HitColorTimer > 0.0f) m_HitColorTimer -= dt;

     if (Collision_IsOverlapSphere({ m_Position, 2.0f }, Player_GetPosition())) {
         this->Damage(100);
         Player_TakeDamage(20);
     }
     ResolveEnemyCollisions();
 }
 void EnemyNormal::ResolveEnemyCollisions() {

     XMVECTOR posA = XMLoadFloat3(&m_Position);
     float radiusA = 1.0f * m_ScalebyLvl;

     const auto& allEnemies = Enemy_GetEnemies();

     for (Enemy* other : allEnemies) {
         if (other == this) continue;

         XMVECTOR posB = XMLoadFloat3(&other->GetPosition());
         XMVECTOR diff = posA - posB;

         XMVECTOR distSqVec = XMVector3LengthSq(diff);
         float distSq;
         XMStoreFloat(&distSq, distSqVec);

         float minDist = radiusA + other->GetRadius();

         if (distSq < minDist * minDist && distSq > 0.0001f) {
             float distance = sqrtf(distSq);
             XMVECTOR pushDir = diff / distance;
             float overlap = minDist - distance;

             posA += pushDir * overlap * 0.5f;
             XMStoreFloat3(&m_Position, posA);
         }
     }
 }
void EnemyNormal::EnemyNormalStatePatrol::Update(double elapsed_time)
{
    g_Speed = 3 + Get_GamePassedTime() / 100;
    if (g_Speed > 12) g_Speed = 12;

    if (m_pOwner->m_HitTimer > 0.0f)
    {
        m_pOwner->m_HitTimer -= (float)elapsed_time;
    }
    if (m_pOwner->m_HitColorTimer > 0.0f)
    {
        m_pOwner->m_HitColorTimer -= (float)elapsed_time;
    }
    if (!m_pOwner->m_hasTarget) {
        XMFLOAT3 playerPos = {0,0,0};

        float randomAngle = (float)rand() / RAND_MAX * 2.0f * 3.14159f;
        float radius = 50.0f;

        m_pOwner->m_TargetPos.x = playerPos.x + cosf(randomAngle) * radius;
        m_pOwner->m_TargetPos.z = playerPos.z + sinf(randomAngle) * radius;
        m_pOwner->m_TargetPos.y = playerPos.y;
        m_pOwner->m_hasTarget = true;
    }


    XMVECTOR ownerPos = XMLoadFloat3(&m_pOwner->m_Position);
    XMVECTOR targetPos = XMLoadFloat3(&m_pOwner->m_TargetPos);
    XMVECTOR toTarget = targetPos - ownerPos;
    float lenSq;
    XMStoreFloat(&lenSq, XMVector3LengthSq(toTarget));
    if (lenSq > 0.0001f)
    {
        XMVECTOR direction = XMVector3Normalize(toTarget);
        XMVECTOR newPos = ownerPos + direction * g_Speed * (float)elapsed_time;
        XMVECTOR maskY = XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f);
        newPos = XMVectorMultiply(newPos, maskY);
        XMStoreFloat3(&m_pOwner->m_Position, newPos);


        float distSq;
        XMStoreFloat(&distSq, XMVector3LengthSq(targetPos - newPos));

        if (distSq < 5.0f) {
            m_pOwner->m_hasTarget = false;
        }
    }
}

void EnemyNormal::EnemyNormalStatePatrol::Draw() const
{
    float scalingByLvl = m_pOwner->GetScaleByLvl();
    XMMATRIX s = XMMatrixScaling(scalingByLvl, scalingByLvl, scalingByLvl);
    XMMATRIX world = s *
        XMMatrixTranslation(
            m_pOwner->m_Position.x,
            m_pOwner->m_Position.y,
            m_pOwner->m_Position.z
        );
    if (m_pOwner->m_HitColorTimer > 0.0f)
        ModelDraw(g_EnemyNormalModel, world, { 1,0,0,0.75f });
    else
        ModelDraw(g_EnemyNormalModel, world, m_pOwner->m_Color);
    DrawEnemyHpBar(m_pOwner);
}

void EnemyNormal::EnemyNormalStatePatrol::DepthDraw() const
{
    ModelDepthDraw(g_EnemyNormalModel
        , XMMatrixTranslation(m_pOwner->m_Position.x, m_pOwner->m_Position.y, m_pOwner->m_Position.z));
}

void EnemyNormal::EnemyNormalStateChase::Update(double elapsed_time)
{
    g_Speed = 3+Get_GamePassedTime()/100;
    if (g_Speed > 12) g_Speed = 12;
    if (m_pOwner->m_HitTimer > 0.0f)
    {
        m_pOwner->m_HitTimer -= (float)elapsed_time;
    }
    if (m_pOwner->m_HitColorTimer > 0.0f)
    {
        m_pOwner->m_HitColorTimer -= (float)elapsed_time;
    }
        XMVECTOR toPlayer =
            XMLoadFloat3(&Player_GetPosition()) -
            XMLoadFloat3(&m_pOwner->m_Position);
        toPlayer = XMVector3Normalize(toPlayer);

        XMVECTOR posotion = XMLoadFloat3(&m_pOwner->m_Position) + toPlayer * g_Speed * elapsed_time;
        XMStoreFloat3(&m_pOwner->m_Position, posotion);
        if (!Collision_IsOverlapSphere(
            { m_pOwner->m_Position, m_pOwner->m_DetectionRadius * 2},
            Player_GetPosition()))
        {
            g_AccumulatedTime += elapsed_time;
            if (g_AccumulatedTime >= 3.0f) {
                g_AccumulatedTime = 0.0;
                m_pOwner->ChangeState(new EnemyNormalStatePatrol(m_pOwner));
            }

        }
        else
        {
            g_AccumulatedTime = 0.0;
        }
}

void EnemyNormal::EnemyNormalStateChase::Draw() const
{
    float scalingByLvl = m_pOwner->GetScaleByLvl();
    XMMATRIX s = XMMatrixScaling(scalingByLvl, scalingByLvl, scalingByLvl);
    XMMATRIX world = s *
        XMMatrixTranslation(
            m_pOwner->m_Position.x,
            m_pOwner->m_Position.y,
            m_pOwner->m_Position.z
        );
    if (m_pOwner->m_HitColorTimer > 0.0f)
    ModelDraw(g_EnemyNormalModel, world, { 1,0,0,0.75f });
    else
    ModelDraw(g_EnemyNormalModel, world, m_pOwner->m_Color);
    DrawEnemyHpBar(m_pOwner);
}

void EnemyNormal::EnemyNormalStateChase::DepthDraw() const
{
    ModelDepthDraw(g_EnemyNormalModel
        , XMMatrixTranslation(m_pOwner->m_Position.x, m_pOwner->m_Position.y, m_pOwner->m_Position.z));
}

static void DrawEnemyHpBar(const EnemyNormal* enemy)
{
    float rate = enemy->GetHpRate();
    if (rate <= 0.0f) return;

    XMFLOAT3 pos = enemy->GetPosition();
    float y = pos.y + (2.0f* enemy->GetScaleByLvl());

    const float totalWidth = 2.0f * enemy->GetScaleByLvl();
    float currentWidth = totalWidth * rate;
    float height = 0.15f;

    XMFLOAT4 color;
    if (rate > 0.6f)      color = { 0, 1, 0, 1 };
    else if (rate > 0.3f) color = { 1, 1, 0, 1 }; 
    else                  color = { 1, 0, 0, 1 };

    float leftEdge = pos.x - (totalWidth * 0.5f);
    float currentCenterX = leftEdge + (currentWidth * 0.5f);

    Billboard_Draw(
        0,
        { currentCenterX, y, pos.z }, 
        { currentWidth, height },
        color,
        { 0.0f, 0.0f }     
    );
}

void EnemyNormal::Enemy_LvlStats()
{
    int timePassed = Get_GamePassedTime();
    m_Lvl = (timePassed / 10) + 1; 
    m_ScalebyLvl = 1 + ((float)m_Lvl / 10);
    m_MaxHp *= m_ScalebyLvl;
    m_Hp = m_MaxHp;
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.f, 1.f);

m_Color = { dist(gen),dist(gen),dist(gen),0.75 };
}
