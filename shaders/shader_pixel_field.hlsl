//======================================================================
//  Terrain pixel shader
//
//  Two detail textures blended by the vertex colour, lit with the same
//  model as shader_pixel_3d.hlsl and receiving the same soft PCF shadows.
//  The ground is the scene's main shadow receiver, so the shadow lookup
//  here has to match the one used by casters exactly - hence the shared
//  shadow.hlsli include.
//======================================================================

#include "shadow.hlsli"

cbuffer PS_CONSTANT_BUFFER : register(b0)
{
    float4 diffuse_color;
};

cbuffer PS_CONSTANT_BUFFER : register(b1)
{
    float4 ambient_color;
};

cbuffer PS_CONSTANT_BUFFER : register(b2)
{
    float4 directional_world_vector;
    float4 directional_color;
};

cbuffer PS_CONSTANT_BUFFER : register(b3)
{
    float3 eye_posW;
    float  specular_power;
    float4 specular_color;
};

struct PointLight
{
    float3 posW;
    float  range;
    float4 color;
};

cbuffer PS_CONSTANT_BUFFER : register(b4)
{
    PointLight point_light[4];
    int        point_light_count;
    float3     point_light_dummy;
};

struct PS_IN
{
    float4 posH        : SV_POSITION;
    float4 posW        : POSITION0;
    float4 posLightWVP : POSITION1;
    float4 normalW     : NORMAL0;
    float4 blend       : COLOR0;
    float2 texcoord    : TEXCOORD0;
};

Texture2D    tex0 : register(t0);
Texture2D    tex1 : register(t1);
SamplerState samp : register(s0);

static const float3 GROUND_BOUNCE = float3(0.55f, 0.50f, 0.42f);

float4 main(PS_IN pi) : SV_TARGET
{
    // Two tiling detail textures blended by the vertex colour; overlapping
    // a large and a small texture hides the repetition.
    float4 tex_color = tex0.Sample(samp, pi.texcoord) * pi.blend.g
                     + tex1.Sample(samp, pi.texcoord) * pi.blend.r;

    float3 albedo = tex_color.rgb * diffuse_color.rgb;

    float3 N = normalize(pi.normalW.xyz);
    float3 V = normalize(eye_posW - pi.posW.xyz);

    // --- Directional key light -----------------------------------------
    float3 L     = normalize(-directional_world_vector.xyz);
    float  NdotL = saturate(dot(N, L));

    float shadow = ShadowFactor(pi.posLightWVP, NdotL);

    float3 diffuse  = albedo * directional_color.rgb * NdotL;
    float3 specular = specular_color.rgb * directional_color.rgb *
                      BlinnPhong(N, L, V, specular_power);

    // --- Ambient (never shadowed) ---------------------------------------
    float3 ambient = albedo * HemisphereAmbient(
        N, ambient_color.rgb, ambient_color.rgb * GROUND_BOUNCE);

    float3 color = ambient + (diffuse + specular) * shadow;

    // --- Point lights ----------------------------------------------------
    for (int i = 0; i < point_light_count; ++i)
    {
        float3 toLight = point_light[i].posW - pi.posW.xyz;
        float  dist2   = dot(toLight, toLight);

        float att = saturate(1.0f - sqrt(dist2) / max(point_light[i].range, 0.001f));
        att *= att;

        toLight *= rsqrt(max(dist2, 1e-6f));

        color += albedo * point_light[i].color.rgb * att * saturate(dot(N, toLight));
        color += point_light[i].color.rgb * specular_color.rgb *
                 BlinnPhong(N, toLight, V, specular_power) * att;
    }

    return float4(color, 1.0f);
}
