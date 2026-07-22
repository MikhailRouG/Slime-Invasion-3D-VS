cbuffer PS_CONSTANT_BUFFER : register(b0)
{
    float4 diffuse_color;
};

struct PS_IN
{
    float4 posL : SV_POSITION;
    float2 uv : TEXCOORD0;
};
Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(PS_IN pi) : SV_TARGET
{
    return tex.Sample(samp, pi.uv) * diffuse_color;
}