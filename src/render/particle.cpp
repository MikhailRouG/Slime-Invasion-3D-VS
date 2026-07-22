#include "particle.h"
void Emitter::Update(double elapsed_time)
{
    for (int i = 0; i < m_count; i++) {
        m_particles[i]->Update(elapsed_time);
    }

    for (int i = m_count - 1; i >= 0; i--) {
        if (m_particles[i]->IsDestroy()) {
            delete m_particles[i];
            m_particles[i] = m_particles[--m_count];
        }
    }
    if (!m_is_emmit)
    {
        m_accumulated_time = 0.0;
        return;
    }
	m_accumulated_time += elapsed_time;

	const double sec_per_particle = 1.0 / m_particles_per_second;
    while (m_accumulated_time >= sec_per_particle) {
        if (m_count >= m_capacity) break;
        m_particles[m_count++] = createParticle();
        m_accumulated_time -= sec_per_particle;
    }

}

void Emitter::Draw()
{
    for (size_t i = 0; i < m_count; ++i)
    {
        m_particles[i]->Draw();
    }
}

void Emitter::EmitOnce(size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        if (m_count >= m_capacity)
            break;

        m_particles[m_count++] = createParticle();
    }
}
