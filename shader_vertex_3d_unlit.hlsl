cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    float4x4 world;
};

cbuffer VS_CONSTANT_BUFFER : register(b1)
{
    float4x4 view;
};

cbuffer VS_CONSTANT_BUFFER : register(b2)
{
    float4x4 proj;
};


struct VS_IN
{
    float3 posL : POSITION0;
    float2 texcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

VS_OUT main(VS_IN vi)
{
    VS_OUT vo;
    float4x4 mtxWVP = mul(mul(world, view), proj);
    vo.posH = mul(float4(vi.posL, 1.0f), mtxWVP);
    vo.texcoord = vi.texcoord;

    return vo;
}