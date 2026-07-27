#ifndef MAP_CAMERA_H
#define MAP_CAMERA_H

#include <DirectXMath.h>

void MapCamera_Initialize();
void MapCamera_Finalize();

void MapCamera_SetPosition(const DirectX::XMFLOAT3& position);
void MapCamera_SetFront(const  DirectX::XMFLOAT3& front);

// Returned by value: both matrices are rebuilt per call, so a reference
// would either dangle or alias shared mutable state.
DirectX::XMFLOAT4X4 MapCamera_GetViewMatrix();
DirectX::XMFLOAT4X4 MapCamera_GetPerspectiveMatrix();

#endif // MAP_CAMERA_H
