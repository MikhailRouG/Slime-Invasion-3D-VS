#include "light.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace {

// Constant-buffer payloads. Their layout must match the b2/b3 cbuffers in
// shader_pixel_3d.hlsl (16-byte aligned).
struct DirectionalLight {
    XMFLOAT4 directional;
    XMFLOAT4 color;
};

struct SpecularLight {
    XMFLOAT3 cameraPosition;
    float    power;
    XMFLOAT4 color;
};

// Create a dynamic-less (default usage) constant buffer of the given size.
ComPtr<ID3D11Buffer> CreateConstantBuffer(ID3D11Device* device, UINT byteWidth)
{
    D3D11_BUFFER_DESC desc{};
    desc.ByteWidth = byteWidth;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    ComPtr<ID3D11Buffer> buffer;
    device->CreateBuffer(&desc, nullptr, buffer.GetAddressOf());
    return buffer;
}

} // namespace

//----------------------------------------------------------------------
//  LightRenderer
//----------------------------------------------------------------------
LightRenderer::LightRenderer(ID3D11Device* device, ID3D11DeviceContext* context)
    : m_context(context)
{
    m_ambientBuffer     = CreateConstantBuffer(device, sizeof(XMFLOAT4));
    m_directionalBuffer = CreateConstantBuffer(device, sizeof(DirectionalLight));
    m_specularBuffer    = CreateConstantBuffer(device, sizeof(SpecularLight));
    m_pointLightBuffer  = CreateConstantBuffer(device, sizeof(PointLightList));
}

template <class T>
void LightRenderer::UpdateAndBind(UINT slot, ID3D11Buffer* buffer, const T& data)
{
    m_context->UpdateSubresource(buffer, 0, nullptr, &data, 0, 0);
    m_context->PSSetConstantBuffers(slot, 1, &buffer);
}

void LightRenderer::SetAmbient(const XMFLOAT4& color)
{
    UpdateAndBind(1, m_ambientBuffer.Get(), color);
}

void LightRenderer::SetDirectional(const XMFLOAT4& worldDirection, const XMFLOAT4& color)
{
    UpdateAndBind(2, m_directionalBuffer.Get(), DirectionalLight{ worldDirection, color });
}

void LightRenderer::SetSpecular(const XMFLOAT3& cameraPosition, float power, const XMFLOAT4& color)
{
    UpdateAndBind(3, m_specularBuffer.Get(), SpecularLight{ cameraPosition, power, color });
}

void LightRenderer::SetPointLightCount(int count)
{
    m_pointLights.count = count;
    UpdateAndBind(4, m_pointLightBuffer.Get(), m_pointLights);
}

void LightRenderer::SetPointLight(int index, const XMFLOAT3& position, float range, const XMFLOAT3& color)
{
    if (index < 0 || index >= kMaxPointLights) return; // guard against out-of-range writes

    m_pointLights.lights[index].position = position;
    m_pointLights.lights[index].range    = range;
    m_pointLights.lights[index].color    = { color.x, color.y, color.z, 1.0f };
    UpdateAndBind(4, m_pointLightBuffer.Get(), m_pointLights);
}

//----------------------------------------------------------------------
//  Compatibility facade
//
//  A single process-wide LightRenderer instance backs the legacy Light_*
//  free functions so existing C-style call sites keep working unchanged.
//----------------------------------------------------------------------
namespace {
LightRenderer* g_light = nullptr;
}

void Light_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    delete g_light;
    g_light = new LightRenderer(pDevice, pContext);
}

void Light_Finalize(void)
{
    delete g_light;
    g_light = nullptr;
}

void Light_SetAmbient(const XMFLOAT4& color)
{
    if (g_light) g_light->SetAmbient(color);
}

void Light_SetDirectionalWorld(const XMFLOAT4& world_directional, const XMFLOAT4& color)
{
    if (g_light) g_light->SetDirectional(world_directional, color);
}

void Light_SetSpecularWorld(const XMFLOAT3& camera_position, float power, const XMFLOAT4& color)
{
    if (g_light) g_light->SetSpecular(camera_position, power, color);
}

void Light_SetPointLightcount(int count)
{
    if (g_light) g_light->SetPointLightCount(count);
}

void Light_SetPointLight(int n, const XMFLOAT3& position, float range, const XMFLOAT3& color)
{
    if (g_light) g_light->SetPointLight(n, position, range, color);
}
