#ifndef MAP_CAMERA_H
#define MAP_CAMERA_H

#include <DirectXMath.h>

void MapCamera_Initialize();
void MapCamera_Finalize();

void MapCamera_SetPosition(const DirectX::XMFLOAT3& position);
void MapCamera_SetFront(const  DirectX::XMFLOAT3& front);

const DirectX::XMFLOAT4X4& MapCamera_GetViewMatrix();
const DirectX::XMFLOAT4X4& MapCamera_GetPerspectiveMatrix();

#endif // MAP_CAMERA_H
