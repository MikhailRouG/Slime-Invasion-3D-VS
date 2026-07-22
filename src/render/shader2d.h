
#ifndef SHADER2d_H
#define	SHADER2d_H

#include <d3d11.h>
#include <DirectXMath.h>

bool Shader2d_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Shader2d_Finalize();

void Shader2d_SetWorldMatrix(const DirectX::XMMATRIX& matrix);

void Shader2d_SetProjectionMatrix(const DirectX::XMMATRIX& matrix);

void Shader2d_Begin();

#endif // SHADER2d_H
