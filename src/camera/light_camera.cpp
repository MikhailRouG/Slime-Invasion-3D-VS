#include "light_camera.h"

using namespace DirectX;

namespace {

// Must match SHADOW_MAP_SIZE in shaders/shadow.hlsli and the texture
// created by configureDepthBuffer() in direct3d.cpp.
constexpr float kShadowMapSize = 2048.0f;

// Direction the light travels, normalised on assignment.
XMFLOAT3 g_Direction{ -1.0f, -1.0f, 1.0f };
XMFLOAT3 g_Target{ 0.0f, 0.0f, 0.0f };

// Half-width of the orthographic box. Covers the play area around the
// player; the smaller this is, the sharper the shadows. Sized for the
// angled third-person camera, which sees noticeably further than a
// straight-down view. At 2048 texels this is ~0.06 world units per texel.
float g_OrthoSize = 60.0f;

// How far back along the light direction the camera sits. Must be large
// enough that everything casting into the box is in front of the near
// plane - the sky/terrain is well inside 120 units.
// Keep near/far as tight as the scene allows: the depth bias in
// shadow.hlsli is expressed in normalised depth, so a needlessly long
// range turns a small bias into a large world-space offset and makes
// shadows visibly detach from their casters.
float g_Distance = 120.0f;
float g_NearZ = 1.0f;
float g_FarZ  = 250.0f;

XMVECTOR DirectionVector()
{
    XMVECTOR d = XMVector3Normalize(XMLoadFloat3(&g_Direction));
    // Guard against a zero/degenerate direction.
    if (XMVector3Equal(d, XMVectorZero())) d = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
    return d;
}

// Any vector not parallel to the light direction works as "up"; picking a
// different axis when the light is near-vertical avoids a degenerate basis.
XMVECTOR UpVector(XMVECTOR direction)
{
    const float verticality = fabsf(XMVectorGetY(direction));
    return (verticality > 0.99f) ? XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)
                                 : XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
}

} // namespace

void LightCamera_Initialize(const XMFLOAT3& world_directional, const XMFLOAT3& target)
{
    g_Direction = world_directional;
    g_Target = target;
}

void LightCamera_Finalize()
{
}

void LightCamera_SetTarget(const XMFLOAT3& target)
{
    g_Target = target;
}

void LightCamera_SetDirection(const XMFLOAT3& world_directional)
{
    g_Direction = world_directional;
}

void LightCamera_SetOrthoSize(float half_extent)
{
    if (half_extent > 0.0f) g_OrthoSize = half_extent;
}

void LightCamera_SetPosition(const XMFLOAT3& position)
{
    g_Target = position;
}

void LightCamera_SetFront(const XMFLOAT3& front)
{
    g_Direction = front;
}

DirectX::XMMATRIX LightCamera_GetViewMatrix()
{
    const XMVECTOR dir = DirectionVector();
    const XMVECTOR up  = UpVector(dir);

    // Snap the centre to whole shadow-map texels, otherwise sub-texel
    // movement makes every shadow edge crawl. Do it in light space: build
    // the rotation first, quantise the target there, then rotate back.
    const XMMATRIX lightRotation = XMMatrixLookToLH(XMVectorZero(), dir, up);

    const float texelSize = (g_OrthoSize * 2.0f) / kShadowMapSize;

    XMVECTOR targetLS = XMVector3TransformCoord(XMLoadFloat3(&g_Target), lightRotation);
    targetLS = XMVectorSetX(targetLS, floorf(XMVectorGetX(targetLS) / texelSize) * texelSize);
    targetLS = XMVectorSetY(targetLS, floorf(XMVectorGetY(targetLS) / texelSize) * texelSize);

    const XMVECTOR snappedTarget =
        XMVector3TransformCoord(targetLS, XMMatrixInverse(nullptr, lightRotation));

    // Pull the eye back along the light direction so the whole box is in
    // front of the near plane.
    const XMVECTOR eye = XMVectorSubtract(snappedTarget, XMVectorScale(dir, g_Distance));

    // XMMatrixLookToLH(EyePosition, EyeDirection, UpDirection) - the
    // previous code passed the direction and up in the wrong order, so the
    // shadow camera never actually looked along the light.
    return XMMatrixLookToLH(eye, dir, up);
}

DirectX::XMMATRIX LightCamera_GetProjectionMatrix()
{
    // Orthographic: a directional light's rays are parallel. The previous
    // perspective projection made shadows shrink with distance and wasted
    // most of the shadow map.
    return XMMatrixOrthographicLH(g_OrthoSize * 2.0f, g_OrthoSize * 2.0f, g_NearZ, g_FarZ);
}
