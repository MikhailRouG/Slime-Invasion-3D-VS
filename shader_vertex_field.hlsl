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
   

struct VS_IN
{
    //:~ セマンティクス
    float4 posL : POSITION0;
    float4 normalL : NORMAL0;
    float4 blend : COLOR0;
    float2 texcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float4 posW : POSITION0;
    float4 normalW : NORMAL0;
    float4 blend : COLOR0;
    float2 texcoord : TEXCOORD0;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUT main(VS_IN vi)
{
    VS_OUT vo;
    
    //座標変換
    float4x4 mtxWV = mul(world, view); //ビュー変換
    float4x4 mtxWVP = mul(mtxWV, proj); // プロジェクション変換
    vo.posH = mul(vi.posL, mtxWVP);
 
    
    //普通のワールド変換行列はだめ(拡大縮小の影響を受けるため)
    //ワールド変換行列の転置逆行列を使う
    float4 normalW = mul(float4(vi.normalL.xyz, 0.0f), world); //αは0
    vo.normalW = normalW; //単位ベクトル化    
    vo.posW = mul(vi.posL, world);

    //地面のテクスチャのブレンド値はそのままパススルー  
    vo.blend = vi.blend;
    
    vo.texcoord = vi.texcoord;
    
    return vo;
}
