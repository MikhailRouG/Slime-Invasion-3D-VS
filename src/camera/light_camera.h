#ifndef LIGHT_CAMERA_H
#define LIGHT_CAMERA_H

#include <DirectXMath.h>

//======================================================================
//  Shadow-map camera for the directional (sun) light.
//
//  A directional light has no position - only a direction - so the
//  shadow camera is an *orthographic* box that slides along with the
//  action. Two details matter for quality:
//
//    * Orthographic, not perspective. A perspective shadow frustum for a
//      sun light distorts the projection and wastes most of the map.
//    * Texel snapping. If the box slides by sub-texel amounts, every
//      shadow edge crawls and shimmers as the camera moves. Snapping the
//      centre to whole shadow-map texels removes that completely.
//======================================================================

// `world_directional` is the direction the light travels (e.g. {-1,-1,1}
// for a sun in the upper-right shining down); `target` is the point the
// shadow box initially centres on.
void LightCamera_Initialize(const DirectX::XMFLOAT3& world_directional,
                            const DirectX::XMFLOAT3& target);
void LightCamera_Finalize();

// The point the shadow box centres on - normally the player, so the
// shadow map always covers what the player can see.
void LightCamera_SetTarget(const DirectX::XMFLOAT3& target);
void LightCamera_SetDirection(const DirectX::XMFLOAT3& world_directional);

// Half-width of the orthographic box, in world units. Smaller = sharper
// shadows over a smaller area.
void LightCamera_SetOrthoSize(float half_extent);

// Returned by value: these matrices are built on the fly, so handing back a
// reference would dangle (previously `const XMMATRIX&` to a temporary - UB).
DirectX::XMMATRIX LightCamera_GetViewMatrix();
DirectX::XMMATRIX LightCamera_GetProjectionMatrix();

// Kept for source compatibility with the previous API.
void LightCamera_SetPosition(const DirectX::XMFLOAT3& position);
void LightCamera_SetFront(const DirectX::XMFLOAT3& front);

#endif // LIGHT_CAMERA_H
