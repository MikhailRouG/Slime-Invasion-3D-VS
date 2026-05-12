#include "enemy_spawner.h"
#include "enemy.h"
#include "player.h"
#include <cstdlib>
#include <cmath>

using namespace DirectX;

static double g_SpawnTimer = 0.0;
static constexpr double SPAWN_INTERVAL = 3.0;
static constexpr float SPAWN_RADIUS = 30.0f;
static constexpr int MAX_ENEMIES = 15;

static XMFLOAT3 GetRandomSpawnPosition()
{
    XMFLOAT3 player = { 0,0,0 };

    float angle = (float)(rand() / (double)RAND_MAX) * XM_2PI;

    float radius =
        10.0f +
        sqrtf((float)(rand() / (double)RAND_MAX)) * (SPAWN_RADIUS - 10.0f);

    XMFLOAT3 pos;
    pos.x = player.x + cosf(angle) * radius;
    pos.z = player.z + sinf(angle) * radius;
    pos.y = 0;

    return pos;
}


void EnemySpawner_Initialize()
{
    g_SpawnTimer = 0.0;
    for (int i = 0; i < MAX_ENEMIES - 1; i++) {
        Enemy_Create(GetRandomSpawnPosition());
    }
}

void EnemySpawner_Update(double elapsedTime)
{
    g_SpawnTimer += elapsedTime;

    if (g_SpawnTimer < SPAWN_INTERVAL)
        return;

    g_SpawnTimer = 0.0;

    if (Enemy_GetEnemyCount() >= MAX_ENEMIES)
        return;

    XMFLOAT3 spawnPos = GetRandomSpawnPosition();
    Enemy_Create(spawnPos);
}
