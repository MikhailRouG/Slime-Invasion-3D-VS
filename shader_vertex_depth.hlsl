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
    float4 posL : POSITION0;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
};

VS_OUT main(VS_IN vi)
{
    VS_OUT vo;

    vo.posH = mul(vi.posL, mul(world, mul(view, proj)));

    return vo;
}