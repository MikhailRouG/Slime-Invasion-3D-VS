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

cbuffer VS_CONSTANT_BUFFER : register(b3)
{
    float4x4 light_view_proj;
};

cbuffer BONE_CONSTANT_BUFFER : register(b4)
{
    float4x4 bones[128];
};

struct VS_IN
{
    float4 posL : POSITION0;
    float4 normalL : NORMAL0;
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;

    int4 boneIDs : BONEIDS0;
    float4 weights : WEIGHTS0;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float4 posW : POSITION0;
    float4 posLightWVP : POSITION1;
    float4 normalW : NORMAL0;
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;
};

VS_OUT main(VS_IN vi)
{
    VS_OUT vo;

    float4x4 skin =
        bones[vi.boneIDs.x] * vi.weights.x +
        bones[vi.boneIDs.y] * vi.weights.y +
        bones[vi.boneIDs.z] * vi.weights.z +
        bones[vi.boneIDs.w] * vi.weights.w;

    float4 skinnedPos = mul(vi.posL, skin);
    float4 skinnedNormal = mul(float4(vi.normalL.xyz, 0.0f), skin);

    float4x4 mtxWV = mul(world, view);
    float4x4 mtxWVP = mul(mtxWV, proj);

    vo.posH = mul(skinnedPos, mtxWVP);
    vo.posW = mul(skinnedPos, world);
    vo.posLightWVP = mul(skinnedPos, mul(world, light_view_proj));

    float4 normalW = mul(float4(skinnedNormal.xyz, 0.0f), world);
    vo.normalW = normalize(normalW);

    vo.color = vi.color;
    vo.texcoord = vi.texcoord;

    return vo;
}