#include "particle_test.h"
#include "billboard.h"
using namespace DirectX;

double easeInCirc(double t)
{
	return 1 - sqrt(1 - t);
}
void NormalParticle::Update(double elapsed_time)
{
	float ratio = std::min(
		static_cast<float>(GetAccumulatedTime() / GetLifeTime()),
		1.0f
	);

	m_scale = 1.0f - ratio*0.4;
	m_scale *= 0.2f;
	m_alptha = easeInCirc(1.0f - ratio);
	AddPosition(GetVelocity() * elapsed_time);
	AddVelocity(XMVECTOR{ 0.0f, -9.8f, 0.0f } *elapsed_time);

	Particle::Update(elapsed_time);

}

void NormalParticle::Draw()
{
	XMFLOAT3 position{};
	XMStoreFloat3(&position, GetPosition());
	Billboard_Draw(m_texture_id, position, { m_scale,m_scale },{1.0f,0.0f,0.0f,m_alptha} , {0,0,});
}

Particle* NormalEmitter::createParticle()
{
	std::uniform_real_distribution<float> dist_phi{ 0.0f, DirectX::XM_2PI };
	std::uniform_real_distribution<float> dist_theta{ 0.0f, DirectX::XM_PI };

	std::uniform_real_distribution<float> m_speed_dist{ 5.0f, 8.0f };
	std::uniform_real_distribution<float> m_lifetime_dist{ 0.5f, 3.0f };
	std::uniform_real_distribution<float> m_color_dist{ 0.0f, 1.0f };
	float phi = dist_phi(m_mt);
	float theta = dist_theta(m_mt);
	float color = m_color_dist(m_mt);
	float sinTheta = sinf(theta);
	XMVECTOR direction = XMVectorSet(
		sinTheta * cosf(phi), 
		cosf(theta), 
		sinTheta * sinf(phi), 
		0.0f
	);

	XMVECTOR velocity = direction * m_speed_dist(m_mt);

	return new NormalParticle(
		m_texture_id,
		GetPosition(),
		velocity,
		m_lifetime_dist(m_mt)
	);
}
