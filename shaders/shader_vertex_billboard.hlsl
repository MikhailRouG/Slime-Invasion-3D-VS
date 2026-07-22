cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float2 scale;
    float2 translation;
    float2 padding;
    float2 dummy; 
};


struct VS_IN
{
    float3 posL : POSITION0;
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;
};

VS_OUT main(VS_IN vi)
{
    VS_OUT vo;
  
    
    float4x4 mtxWV = mul(world, view);
    float4x4 mtxWVP = mul(mtxWV, proj);
    vo.posH = mul(float4(vi.posL, 1.0f), mtxWVP);

    
    vo.color = vi.color; // 地面のテクスチャのブレンド値はそのままパススルー
    vo.texcoord = vi.texcoord * scale + translation;
    
    return vo;
}
