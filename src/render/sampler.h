
#ifndef SAMPLER_H
#define SAMPLER_H

#include <d3d11.h>

void Sampler_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Sampler_Finalize();

void Sampler_SetFilterPoint();
void Sampler_SetFilterLinear();
void Sampler_SetFilterAnisotropic();

// Binds the shadow-map sampler to s1 (point filtering + white border).
// Must stay on its own slot: the shadow map needs point sampling and
// clamped edges, while s0 keeps anisotropic filtering for diffuse maps.
void Sampler_SetShadow();

#endif //SAMPLER_H