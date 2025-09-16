/*==============================================================================

   ÉJÉÅÉâêßå‰ [camera.h]
														 Author : Harada Ren
														 Date   : 2025/09/11
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>

void Camera_Initialize();
void Camera_Finalize();
void Camera_Update(double elapsed_time);

const DirectX::XMFLOAT4X4& Camera_GetMatrix();
const DirectX::XMFLOAT4X4& Camera_GetPerspectiveMatrix();


#endif //GRID_H