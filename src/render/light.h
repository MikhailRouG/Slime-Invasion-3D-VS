#ifndef LIGHT_H
#define LIGHT_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

//======================================================================
//  Lighting
//  LightRenderer owns the pixel-shader constant buffers that describe the
//  scene lighting (ambient, directional, specular and up to four point
//  lights) and uploads them to the GPU.
//  Design notes (OOP / SOLID):
//    * Single Responsibility - the class only manages light constant
//      buffers; it does not create devices, load assets or draw geometry.
//    * Dependency Inversion   - the D3D device/context are injected through
//      the constructor instead of being pulled from a global.
//    * RAII                   - constant buffers are held in ComPtr and
//      released automatically; there is no manual "finalize" step and no
//      chance of leaking on an early return.
//  The free Light_* functions below are a thin compatibility facade kept so
//  that existing C-style call sites (main.cpp, game.cpp) continue to work
//  unchanged while the implementation is fully object oriented.
//======================================================================
class LightRenderer
{
public:
    LightRenderer(ID3D11Device* device, ID3D11DeviceContext* context);
    ~LightRenderer() = default;

    // Resource-owning type: copying would double-free the GPU buffers.
    LightRenderer(const LightRenderer&) = delete;
    LightRenderer& operator=(const LightRenderer&) = delete;

    static constexpr int kMaxPointLights = 4;

    void SetAmbient(const DirectX::XMFLOAT4& color);
    void SetDirectional(const DirectX::XMFLOAT4& worldDirection, const DirectX::XMFLOAT4& color);
    void SetSpecular(const DirectX::XMFLOAT3& cameraPosition, float power, const DirectX::XMFLOAT4& color);
    void SetPointLightCount(int count);
    void SetPointLight(int index, const DirectX::XMFLOAT3& position, float range, const DirectX::XMFLOAT3& color);

private:
    template <class T>
    void UpdateAndBind(UINT slot, ID3D11Buffer* buffer, const T& data);

    // Layout must stay in sync with the b4 cbuffer in shader_pixel_3d.hlsl.
    struct PointLight {
        DirectX::XMFLOAT3 position;
        float             range;
        DirectX::XMFLOAT4 color;
    };
    struct PointLightList {
        PointLight        lights[kMaxPointLights];
        int               count;
        DirectX::XMFLOAT3 _pad;
    };

    ID3D11DeviceContext*                        m_context = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Buffer>        m_ambientBuffer;      // b1
    Microsoft::WRL::ComPtr<ID3D11Buffer>        m_directionalBuffer;  // b2
    Microsoft::WRL::ComPtr<ID3D11Buffer>        m_specularBuffer;     // b3
    Microsoft::WRL::ComPtr<ID3D11Buffer>        m_pointLightBuffer;   // b4
    PointLightList                              m_pointLights{};
};

//--- Compatibility facade (delegates to a single LightRenderer instance) ---
void Light_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Light_Finalize(void);
void Light_SetAmbient(const DirectX::XMFLOAT4& color);
void Light_SetDirectionalWorld(const DirectX::XMFLOAT4& world_directional, const DirectX::XMFLOAT4& color);
void Light_SetSpecularWorld(const DirectX::XMFLOAT3& camera_position, float power, const DirectX::XMFLOAT4& color);
void Light_SetPointLightcount(int count);
void Light_SetPointLight(int n, const DirectX::XMFLOAT3& position, float range, const DirectX::XMFLOAT3& color);

#endif // LIGHT_H
