
#ifndef PARTICLE_H
#define PARTICLE_H

#include <DirectXMath.h>
class Particle
{
private:
    DirectX::XMVECTOR m_position;
    DirectX::XMVECTOR m_velocity;
    // float m_mass;
    double m_life_tIme{};
    double m_spawn_time{};
    double m_accumulated_time = 0.0f;
public:
    Particle(const DirectX::XMVECTOR& position, const  DirectX::XMVECTOR& velocity, double life_time, double spawn_time)
    : m_position(position), m_velocity(velocity),
        m_life_tIme(life_time), m_spawn_time(spawn_time){

    }
    virtual ~Particle() = default;
    bool IsDestroy() const {
        return m_life_tIme  <= 0.0f;
    }
    virtual void Update(double elapsed_time) {
        m_accumulated_time += elapsed_time;
        if (m_accumulated_time > m_life_tIme) {
            Destroy();
        }
    };
    virtual void Draw()  = 0;
    double GetLifeTime() const
    {
        return m_life_tIme;
    }

    double GetSpawnTime() const
    {
        return m_spawn_time;
    }

    double GetAccumulatedTime() const
    {
        return m_accumulated_time;
    }
    void SetPosition(const DirectX::XMVECTOR& position) {
        m_position = position;
    }
protected:
    void Destroy()
    {
        m_life_tIme = 0.0f;
    }

    void SetVelocity(const DirectX::XMVECTOR& velocity) {
        m_velocity = velocity;
    }
    const DirectX::XMVECTOR GetPosition() const {
        return m_position;
    }

    const DirectX::XMVECTOR GetVelocity() const {
        return m_velocity;
    }

    void AddPosition(DirectX::FXMVECTOR v) {
        m_position = DirectX::XMVectorAdd(m_position, v);
    }

    void AddVelocity(DirectX::FXMVECTOR v) {
        m_velocity = DirectX::XMVectorAdd(m_velocity, v);
    }
};

class Emitter 
{
private:
    DirectX::XMVECTOR m_position;
    double m_particles_per_second;
    double m_accumulated_time= 0.0;
    bool m_is_emmit{};
    size_t m_capacity{};
    size_t m_count{};
    Particle** m_particles{};
protected:
    virtual Particle* createParticle() = 0;

    double GetParticlesPerSecond() const {
        return m_particles_per_second;
    }
    double GetAccumulatedTime() const {
        return m_accumulated_time;
    }
    DirectX::XMVECTOR GetPosition() const {
        return m_position;
    }
    void SetParticlesPerSecond(double particles_per_second) {
        m_particles_per_second = particles_per_second;
    }
public:
    Emitter(size_t capacity, const DirectX::XMVECTOR position, double particles_per_second,bool isEmmit):
        m_position(position),
        m_particles_per_second(particles_per_second),
        m_is_emmit(isEmmit),
        m_capacity(capacity),
        m_count(0)
    {
        m_particles = new Particle* [m_capacity] {};
    }
    virtual void Update(double elapsed_time);
    virtual void Draw();

    void Emmit(bool isEmit) {
        m_is_emmit = isEmit;
    }

    bool IsEmmit() const {
        return m_is_emmit;
    }
    void EmitOnce(size_t count);
    void SetPosition(DirectX::FXMVECTOR position) {
        m_position = position;
    };
};

#endif //PARTICLE_H