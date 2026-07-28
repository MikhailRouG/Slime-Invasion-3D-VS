#ifndef SHADOW_HLSLI
#define SHADOW_HLSLI

//======================================================================
//  Shared shadow + lighting helpers
//  Both the lit 3D shader and the terrain shader sample the same shadow
//  map, so the lookup lives here instead of being copy-pasted. (The two
//  shaders previously carried separate, subtly different, and equally
//  broken copies of it.)
//  Shadow map: 2048x2048 R32_FLOAT, written by shader_pixel_depth.hlsl,
//  bound to t2 by Direct3D_SetDepthTexture(2).
//  Sampler   : point + BORDER(white) at s1 (see Sampler_SetShadow).
//              Point filtering is required - averaging depth values and
//              then comparing is wrong; PCF must compare per texel and
//              average the *results*. The white border means anything
//              outside the light frustum reads as "fully lit".
//======================================================================

Texture2D    g_ShadowMap     : register(t2);
SamplerState g_ShadowSampler : register(s1);

static const float SHADOW_MAP_SIZE = 2048.0f;

// Depth bias, in normalised [0,1] light-space depth. The projection is
// orthographic, so depth is linear: with the light camera's ~250 unit
// range these work out to roughly 0.2 - 0.9 world units, which clears the
// per-texel depth slope (~0.0002) with margin while staying small enough
// that shadows do not visibly detach from their casters.
static const float SHADOW_BIAS_MIN = 0.0008f;
static const float SHADOW_BIAS_MAX = 0.0035f;

// How dark a fully shadowed surface gets. Direct light is scaled by this,
// ambient is left untouched, so shadows stay coloured instead of grey.
static const float SHADOW_MIN_LIGHT = 0.0f;

//----------------------------------------------------------------------
//  Percentage-Closer Filtering, 3x3 taps.
//  Returns 1.0 = fully lit, 0.0 = fully shadowed.
//  `NdotL` drives a slope-scaled bias: surfaces facing the light edge-on
//  need a larger offset to avoid self-shadowing acne, while surfaces
//  facing it head-on need almost none (which avoids peter-panning).
//----------------------------------------------------------------------
float ShadowFactor(float4 posLightWVP, float NdotL)
{
    float3 proj = posLightWVP.xyz / posLightWVP.w;

    // Clip space -> shadow map UV. Y is flipped: clip space is +Y up,
    // texture space is +V down.
    float2 uv = proj.xy * float2(0.5f, -0.5f) + 0.5f;
    float  z  = proj.z;

    // Past the light's far plane, or behind its near plane: treat as lit.
    if (z > 1.0f || z < 0.0f) return 1.0f;

    float bias  = max(SHADOW_BIAS_MAX * (1.0f - NdotL), SHADOW_BIAS_MIN);
    float texel = 1.0f / SHADOW_MAP_SIZE;

    float lit = 0.0f;
    [unroll]
    for (int y = -1; y <= 1; ++y)
    {
        [unroll]
        for (int x = -1; x <= 1; ++x)
        {
            // SampleLevel, not Sample: sampling inside a loop with implicit
            // gradients is undefined, and we want mip 0 regardless.
            float mapZ = g_ShadowMap.SampleLevel(
                g_ShadowSampler, uv + float2(x, y) * texel, 0).r;

            lit += (z - bias <= mapZ) ? 1.0f : 0.0f;
        }
    }

    return lerp(SHADOW_MIN_LIGHT, 1.0f, lit / 9.0f);
}

//----------------------------------------------------------------------
//  Hemisphere ambient: sky colour from above, bounced ground colour from
//  below. Far more natural than a single flat ambient term, and it keeps
//  shadowed areas readable instead of flat grey.
//----------------------------------------------------------------------
float3 HemisphereAmbient(float3 N, float3 skyColor, float3 groundColor)
{
    return lerp(groundColor, skyColor, saturate(N.y * 0.5f + 0.5f));
}

//----------------------------------------------------------------------
//  Blinn-Phong specular, masked by NdotL so unlit surfaces cannot glint.
//----------------------------------------------------------------------
float BlinnPhong(float3 N, float3 L, float3 V, float power)
{
    float3 H = normalize(L + V);
    return pow(saturate(dot(N, H)), max(power, 1.0f)) * saturate(dot(N, L));
}

#endif // SHADOW_HLSLI
