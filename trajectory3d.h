#ifndef TRAJECTORY3D_H
#define TRAJECTORY3D_H

#include <DirectXMath.h>

void Trajectory3d_Initialize();
void Trajectory3d_Finalize();
void Trajectory3d_Update(double elapsed_time);
void Trajectory3d_Draw();

void Trajectory3d_Create(const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT4& color,
    float size,
    double lifeTime);

#endif // TRAJECTORY3D_H
#pragma once
