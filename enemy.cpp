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
static std::vector<Enemy*> g_Enemies;
void Enemy_Initialize()
{
	EnemyNormal_Initialize();
}

void Enemy_Finalize()
{
	for (auto* e : g_Enemies) delete e;
	g_Enemies.clear();
	EnemyNormal_Finalize();
}

void Enemy_Update(double elapsed_time)
{
	for (auto* e : g_Enemies) e->Update(elapsed_time);

	for (auto* e : g_Enemies) e->UpdateState();

	g_Enemies.erase(std::remove_if(g_Enemies.begin(), g_Enemies.end(),
		[](Enemy* e) {
			if (e->isDestroy()) {
				delete e;
				return true;
			}
			return false;
		}), g_Enemies.end());
}

void Enemy_UpdateState()
{
}

void Enemy_Draw()
{
	for (auto* e : g_Enemies) { 
		e->Draw();
	}
}

void Enemy_DepthDraw()
{
	for (auto* e : g_Enemies) {
		e->DepthDraw();
	}
}

Enemy* Enemy_Create(const DirectX::XMFLOAT3& position)
{
	Enemy* newEnemy = new EnemyNormal(position);
	g_Enemies.push_back(newEnemy);
	return newEnemy;
}

int Enemy_GetEnemyCount()
{
	return (int)g_Enemies.size();
}

Enemy* Enemy_GetEnemy(int index)
{
	return g_Enemies[index];
}

const std::vector<Enemy*>& Enemy_GetEnemies()
{
	return g_Enemies;
}

void Enemy::State::DepthDraw() const
{
	//for (int i = 0; i < g_EnemyCount; i++) {
	//	g_Enemies[i]->Draw();
	//}
}
