
cbuffer VS_CONSTANT_BUFFER : register(b0){
    float4x4 world;
};

cbuffer VS_CONSTANT_BUFFER : register(b1){
    float4x4 view;
};

cbuffer VS_CONSTANT_BUFFER : register(b2){
    float4x4 proj;
};
cbuffer VS_CONSTANT_BUFFER : register(b3)
{
    float4x4 light_view_proj;
};

struct VS_IN{
    float4 posL : POSITION0;
    float4 normalL : NORMAL0; 
    float4 color: COLOR0; 
    float2 texcoord : TEXCOORD0;
};

struct VS_OUT{
    float4 posH : SV_POSITION;
    float4 posW : POSITION0;
    float4 posLightWVP : POSITION1;
    float4 normalW : NORMAL0;
    float4 color: COLOR0;
    float2 texcoord : TEXCOORD0;
};
VS_OUT main(VS_IN vi){
    VS_OUT vo;
    
    float4x4 mtxWV = mul(world, view); //ビュー変換
    float4x4 mtxWVP = mul(mtxWV, proj); // プロジェクション変換
    vo.posH = mul(vi.posL, mtxWVP);
    
    vo.posLightWVP = mul(vi.posL, mul(world, light_view_proj));
    float4 normalW = mul(float4(vi.normalL.xyz, 0.0f), world); //αは0
    vo.normalW = normalize(normalW); //単位ベクトル化    
    vo.posW = mul(vi.posL, world);
    
    vo.color = vi.color;
    vo.texcoord = vi.texcoord;
    
	return vo;
}
