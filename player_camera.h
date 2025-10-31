/*==============================================================================

   プレイヤー用のカメラ制御 [player_camera.h]
														 Author : Harada Ren
														 Date   : 2025/10/31
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef PLAYER_CAMERA_H
#define PLAYER_CAMERA_H

#include <DirectXMath.h>

void PlayerCamera_Initialize();
void PlayerCamera_Finalize();
void PlayerCamera_Update(double elapsed_time);

const DirectX::XMFLOAT3& PlayerCamera_GetFront();
const DirectX::XMFLOAT3& PlayerCamera_GetPosition();


#endif //PLAYER_CAMERA_H