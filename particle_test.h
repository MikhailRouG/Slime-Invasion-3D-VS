#ifndef  PARTICLE_TEST_H

#define PARTICLE_TEST_H

#include "particle.h"
#include "texture.h"
#include <random>
class NormalParticle :public Particle
{
private:
    int m_texture_id{ -1 };
    float m_scale{};
    float m_alptha{};
public:
    NormalParticle(int texture_id, const DirectX::XMVECTOR& position, const  DirectX::XMVECTOR& velocity, double life_time, double spawn_time = 0.0f)
        :m_texture_id(texture_id), Particle(position, velocity, life_time, spawn_time)
    {
    }
     void Update(double elapsed_time) override ;
     void Draw() override;
};
class NormalEmitter : public Emitter
{
private:
    int m_texture_id{ -1 };
    std::mt19937 m_mt;
    std::uniform_real_distribution<float> m_dir_dist{ -DirectX::XM_PI, DirectX::XM_PI };
    std::uniform_real_distribution<float> m_speed_dist{ 1.0f, 5.0f};
    std::uniform_real_distribution<float> m_lifetime_dist{ 1.0f, 5.0f };
public:
    NormalEmitter(size_t capacity,
        const DirectX::XMVECTOR& position,
        double particles_per_second,
        bool isEmmit)
        : Emitter(capacity, position, particles_per_second, isEmmit),
        m_mt(std::random_device{}()),
    m_texture_id(Texture_Load(L"resource/texture/dangerzone.png")) {

    }
protected :
    Particle* createParticle() override;
};

#endif // PARTICLE_TEST_H
