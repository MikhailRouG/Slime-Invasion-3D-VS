/*==============================================================================

   3D描画用頂点シェーダー [shader_vertex_3d.hlsl]
														 Author : Harada Ren
														 Date   : 2025/09/10
--------------------------------------------------------------------------------

==============================================================================*/

// 定数バッファ
cbuffer VS_CONSTANT_BUFFER : register(b0){
    float4x4 world;
};

cbuffer VS_CONSTANT_BUFFER : register(b1){
    float4x4 view;
};

cbuffer VS_CONSTANT_BUFFER : register(b2){
    float4x4 proj;
};

cbuffer VS_CONSTANT_BUFFER : register(b3){
    float4 ambient_color ;
};

cbuffer VS_CONSTANT_BUFFER : register(b4){
    float4 directional_world_vector;
    float4 directional_color;
    //float3 gColorSpecularLight;
    float3 eye_posW;
    //float specular_power;
};
   

struct VS_IN{
    //:~ セマンティクス
    float4 posL : POSITION0;
    float4 normalL : NORMAL0; //ローカル
    float4 color: COLOR0; 
    float2 texcoord : TEXCOORD0;
};

struct VS_OUT{
    float4 posH : SV_POSITION;
    float4 color: COLOR0;
    float2 texcoord : TEXCOORD0;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUT main(VS_IN vi){
    VS_OUT vo;
    
    
    //座標変換
    //float4 posW = mul(vi.posL, world); 
    //float4 posWV = mul(posW, view); 
    //vo.posH = mul(posWV, proj); 
    float4x4 mtxWV = mul(world, view); //ビュー変換
    float4x4 mtxWVP = mul(mtxWV, proj); // プロジェクション変換
    vo.posH = mul(vi.posL, mtxWVP);
    
    //ライト計算
    //普通のワールド変換行列はだめ(拡大縮小の影響を受けるため)
    //ワールド変換行列の転置逆行列を使う
    float4 normalW = mul(float4(vi.normalL.xyz, 0.0f), world); //αは0
    normalW = normalize(normalW); //単位ベクトル化
    float dl = max(0.0f, dot(-directional_world_vector, normalW)); //内積
    
    //スペキュラ
    float4 posW = mul(vi.posL, world);
    float3 toEye = normalize(eye_posW - posW.xyz);
    float3 r = reflect(normalize(directional_world_vector),normalW);
    
    float t = pow(max(dot(r, toEye), 0.0f), 10.0f);
    
    
    float3 color = vi.color.rgb * directional_color.rgb * dl + ambient_color.rgb * vi.color.rgb;
    color += float3(1.0f, 1.0f, 1.0f) * t;
    vo.color = float4(color, vi.color.a);
    vo.texcoord = vi.texcoord;
    
	return vo;
}
