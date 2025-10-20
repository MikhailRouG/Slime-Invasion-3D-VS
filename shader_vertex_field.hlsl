/*==============================================================================

   メッシュフィールド描画用頂点シェーダー [shader_vertex_field.hlsl]
														 Author : Harada Ren
														 Date   : 2025/10/20
--------------------------------------------------------------------------------

==============================================================================*/

// 定数バッファ
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
    float4 ambient_color;
};

cbuffer VS_CONSTANT_BUFFER : register(b4)
{
    float4 directional_world_vector;
    float4 directional_color;
};
   

struct VS_IN
{
    //:~ セマンティクス
    float4 posL : POSITION0;
    float4 normalL : NORMAL0; //ローカル
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float4 color : COLOR0;
    float4 directional : COLOR1;
    float4 ambient : COLOR2;
    float2 texcoord : TEXCOORD0;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUT main(VS_IN vi)
{
    VS_OUT vo;
    
    //座標変換
    float4 posW = mul(vi.posL, world); //posLをワールド変換
    float4 posWV = mul(posW, view); // posWをビュー変換
    vo.posH = mul(posWV, proj); //posWVをプロジェクション変換
    
    //ライト計算
    //普通のワールド変換行列はだめ(拡大縮小の影響を受けるため)
    //ワールド変換行列の転置逆行列を使う
    float4 normalW = mul(float4(vi.normalL.xyz, 0.0f), world); //αは0
    normalW = normalize(normalW); //単位ベクトル化
    float dl = max(0.0f, dot(-directional_world_vector, normalW)); //内積
    
    vo.color = vi.color; //地面のテクスチャのブレンド値はそのままパススルー
    
    //ライトの計算
    float3 color =ambient_color.rgb;
    vo.directional = float4(directional_color.rgb * dl, 1.0f);
    vo.ambient = float4(ambient_color.rgb, 1.0f);
    vo.texcoord = vi.texcoord;
    
    return vo;
}
