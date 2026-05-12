#include "enemy.h"
#include "enemy_normal.h"

void Enemy::Update(double elapsed_time)
{
	m_pState->Update(elapsed_time);
}
void Enemy::Draw() const
{
	m_pState->Draw();
}

void Enemy::DepthDraw() const
{
	m_pState->DepthDraw();
}

void Enemy::UpdateState()
{
	if (m_pNextState != m_pState)
	{
		delete m_pState;
		m_pState = m_pNextState;
	}
}
void Enemy::ChangeState(State* pNext)
{
	m_pNextState = pNext;
}
static constexpr int MAX_ENEMY = 16;
static Enemy* g_Enemys[MAX_ENEMY]{};
static int g_EnemyCount{ 0 };
void Enemy_Initialize()
{
	EnemyNormal_Initialize();
	g_EnemyCount = 0;
}

void Enemy_Finalize()
{
	for (int i = 0; i < g_EnemyCount; i++)
	{
		delete g_Enemys[i];
		g_Enemys[i] = nullptr; 
	}
	g_EnemyCount = 0;
	EnemyNormal_Finalize();
}

void Enemy_Update(double elapsed_time)
{
	for (int i = 0; i < g_EnemyCount; i++) {
		g_Enemys[i]->Update(elapsed_time);
	}
	for (int i = 0; i < g_EnemyCount; i++) {
		g_Enemys[i]->UpdateState();
	}
	for (int i = g_EnemyCount - 1; i >= 0; i--) {
		if (g_Enemys[i]->isDestroy())
		{
			delete g_Enemys[i];
			g_Enemys[i] = g_Enemys[--g_EnemyCount];
		}
	}
}

void Enemy_UpdateState()
{
}

void Enemy_Draw()
{
	for (int i = 0; i < g_EnemyCount; i++) {
		g_Enemys[i]->Draw();
	}
}

void Enemy_DepthDraw()
{
	for (int i = 0; i < g_EnemyCount; i++) {
		g_Enemys[i]->DepthDraw();
	}
}

void Enemy_Create(const DirectX::XMFLOAT3& position)
{
	if (g_EnemyCount >= MAX_ENEMY)
		return;

	g_Enemys[g_EnemyCount++] = new EnemyNormal(position);
}

int Enemy_GetEnemyCount()
{
	return g_EnemyCount;
}

Enemy* Enemy_GetEnemy(int index)
{
	return g_Enemys[index];
}

void Enemy::State::DepthDraw() const
{
	for (int i = 0; i < g_EnemyCount; i++) {
		g_Enemys[i]->Draw();
	}
}
