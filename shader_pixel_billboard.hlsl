cbuffer PS_CONSTANT_BUFFER : register(b0)
{
    float4 diffuse_color;
};

struct PS_IN
{
    float4 posL : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

Texture2D tex;
SamplerState samp; 

float4 main(PS_IN pi) : SV_TARGET
{
    float4 color = tex.Sample(samp, pi.uv) * pi.color * diffuse_color;

    if (color.a < 0.1f)
    {
        color.a = saturate(color.a);
    }
    return color;
}