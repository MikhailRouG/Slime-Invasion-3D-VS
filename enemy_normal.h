#ifndef ENEMY_NORMAL_H
#define ENEMY_NORMAL_H

#include "enemy.h"
#include <DirectXMath.h>
#include "texture.h"
#include "model.h"
#include "game.h"
class EnemyNormal : public Enemy
{
private:
    DirectX::XMFLOAT3 m_Position{};
    DirectX::XMFLOAT3 m_TargetPos;
    DirectX::XMFLOAT4 m_Color;
    DirectX::XMFLOAT4 m_CurrentColor;
    bool m_hasTarget = false;
    float m_DetectionRadius{10.0f};
    static constexpr int MAX_START_HP = 100;
    int m_MaxHp{ MAX_START_HP };
    int m_Hp{ MAX_START_HP };
    int m_Lvl;
    float m_ScalebyLvl;
    float m_HitTimer{ 0.0f };
    float m_HitColorTimer{ 0.0f };
    void Enemy_LvlStats();
public:
    EnemyNormal(const DirectX::XMFLOAT3& position) : m_Position(position) {
        m_Lvl = 0;
        ChangeState(new EnemyNormalStatePatrol(this));
        Enemy_LvlStats();
        UpdateState();
    };
    void Damage(int damage) override {
        m_Hp -= damage;
        m_HitTimer = 1.5f;
        m_HitColorTimer = 0.15f;
    }
    void Damage(int damage,float speed) override {
        m_Hp -= damage;
        m_HitTimer = 1.5f / speed;;
        m_HitColorTimer = 0.15f;
    }
    bool isDestroy() const override {
        return m_Hp <= 0;
    }
    Sphere GetCollision() const {
        return{ m_Position, 1.6f * m_ScalebyLvl };
    }
    float GetHpRate() const
    {
        return (float)m_Hp / (float)m_MaxHp;
    }
    int GetLvl() const
    {
        return m_Lvl;
    }
    int GetScaleByLvl() const
    {
        return m_ScalebyLvl;
    }
    bool isHit() const override {
        if (m_HitTimer > 0) return true;
        else return false;
    }
    const DirectX::XMFLOAT3& GetPosition() const
    {
        return m_Position;
    }
    class EnemyNormalStatePatrol : public State
    {
    private:
        EnemyNormal* m_pOwner{};
        float m_PoimtX{};
        double g_AccumulatedTime;
    public:
        EnemyNormalStatePatrol(EnemyNormal* pOwner) : m_pOwner(pOwner)
            , m_PoimtX(pOwner->m_Position.x)
        {
        }
        void Update(double elapsed_time) override;
        void Draw() const override;
        void DepthDraw() const override;
    };

    class EnemyNormalStateChase : public State
    {
    private:
        EnemyNormal* m_pOwner{};
        double g_AccumulatedTime;
        double m_ChaseTime{};
        int m_TargetRouteIndex{};
    public:
        EnemyNormalStateChase(EnemyNormal* pOwner) : m_pOwner(pOwner)
        {
        }
        void Update(double elapsed_time) override;
        void Draw() const override;
        void DepthDraw() const override;
    };
};
void EnemyNormal_Initialize();
void EnemyNormal_Finalize();
#endif // ENEMY_NORMAL_H
