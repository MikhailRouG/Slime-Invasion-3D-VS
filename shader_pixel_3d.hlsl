/*==============================================================================

   3D描画用ピクセルシェーダー [shader_pixel_3d.hlsl]
														 Author : Harada Ren
														 Date   : 2025/09/10
--------------------------------------------------------------------------------

==============================================================================*/

//定数バッファ
cbuffer PS_CONSTANT_BUFFER : register(b0){
    float4 color;
};

cbuffer PS_CONSTANT_BUFFER : register(b1){
    float4 ambient_color;
};

cbuffer PS_CONSTANT_BUFFER : register(b2){
    float4 directional_world_vector;
    float4 directional_color;
    //float3 gColorSpecularLight;
    float3 eye_posW;
    //float specular_power;
};

struct PS_IN{
    float4 posH : SV_POSITION;
    float4 posW : POSITION0;
    float4 normalW : NORMAL0;
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;
};

Texture2D tex; //テクスチャ
SamplerState samp; //テクスチャサンプラ

float4 main(PS_IN pi) : SV_TARGET{
    //平行光源
    float4 normalW = normalize(pi.normalW);
    float dl = max(0.0f, dot(-directional_world_vector, normalW)); //内積
    //スペキュラ
    float3 toEye = normalize(eye_posW - pi.posW.xyz);
    float3 r = reflect(normalize(directional_world_vector), normalW).xyz;
    float t = pow(max(dot(r, toEye), 0.0f), 5.0f);
    
    float3 lcolor = pi.color.rgb * directional_color.rgb * dl + ambient_color.rgb * pi.color.rgb;
    lcolor += float3(1.0f, 1.0f, 1.0f) * t;
    
    return tex.Sample(samp, pi.texcoord) * float4(lcolor,1.0f) * color; //uvの座標のサンプラーのテクスチャの色を返す
}
