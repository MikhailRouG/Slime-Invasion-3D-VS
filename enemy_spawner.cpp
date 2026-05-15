#include "enemy_spawner.h"
#include "enemy.h"
#include "player.h"
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>
using namespace DirectX;
double m_spawnTimer = 0.0;
const double SPAWN_INTERVAL = 2.0;
const float SPAWN_RADIUS = 30.0f;
const int MAX_ENEMIES = 15;

static XMFLOAT3 GetRandomSpawnPosition(float r)
{
    XMFLOAT3 player = { 0,0,0 };

    float angle = (float)(rand() / (double)RAND_MAX) * XM_2PI;

    float radius =
        10.0f +
        sqrtf((float)(rand() / (double)RAND_MAX)) * (r - 10.0f);

    XMFLOAT3 pos;
    pos.x = player.x + cosf(angle) * radius;
    pos.z = player.z + sinf(angle) * radius;
    pos.y = 0;

    return pos;
}


 void EnemySpawner_Initialize()
{
     m_spawnTimer = 0.0;
    for (int i = 0; i < MAX_ENEMIES - 1; i++) {
        Enemy_Create(GetRandomSpawnPosition(SPAWN_RADIUS));
    }
}

void EnemySpawner_Update(double elapsedTime)
{
    m_spawnTimer += elapsedTime;

    if (m_spawnTimer < SPAWN_INTERVAL)
        return;

    m_spawnTimer = 0.0;

    if (Enemy_GetEnemyCount() >= MAX_ENEMIES)
        return;

    if (Enemy_GetEnemyCount() < MAX_ENEMIES)
    {
        XMFLOAT3 spawnPos = GetRandomSpawnPosition(SPAWN_RADIUS);
        Enemy_Create(spawnPos);
    }
}
