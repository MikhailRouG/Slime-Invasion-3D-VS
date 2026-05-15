#ifndef ENEMY_H
#define ENEMY_H
#include "DirectXMath.h"
#include "collision.h"
#include <vector>
class Enemy
{
protected:
    class State
    {
    private:
        Enemy* m_pOwner{};

    public:
        virtual ~State() = default;
        virtual void Update(double elapsed_time) = 0;
        virtual void Draw() const=0;
        virtual void DepthDraw() const= 0;
    protected:
        Enemy* GetOwner() const { return m_pOwner; }
    };
private:
    DirectX::XMFLOAT3 m_Position{};
    State* m_pState{};
    State* m_pNextState{};
    bool m_isDestroy{};
public:
    virtual ~Enemy() = default;
     virtual void Update(double elapsed_time);
     virtual void Draw() const;
     virtual void DepthDraw() const;
    void UpdateState();
    virtual void Damage(int damage) {};
    virtual void Damage(int damage, float speed) {};
   virtual bool isDestroy()const { return false; }
   virtual bool isHit() const { return false; }
   virtual int GetLvl() const { return 0; }
   virtual bool isDemaged()const { return false; }
   virtual Sphere GetCollision() const {return{}; };
   virtual float GetRadius() const { return 1.0f; }
   virtual const DirectX::XMFLOAT3& GetPosition() const
   {
       return {0,0,0};
   }
protected:
    void ChangeState(State* pNext);
};

void Enemy_Initialize();
void Enemy_Finalize();
void Enemy_Update(double elapsed_time);
void Enemy_Draw();
void Enemy_DepthDraw();
Enemy* Enemy_Create(const DirectX::XMFLOAT3& position);
int Enemy_GetEnemyCount();
Enemy* Enemy_GetEnemy(int index);
 const std::vector<Enemy*>& Enemy_GetEnemies();
#endif // ENEMY_H
