/*==============================================================================

   フィールド描画用ピクセルシェーダー [shader_pixel_field.hlsl]
														 Author : Harada Ren
														 Date   : 2025/09/26
--------------------------------------------------------------------------------

==============================================================================*/

struct PS_IN{
    float4 posH : SV_POSITION;
    float4 color: COLOR0;
    float2 texcoord : TEXCOORD0;
};

Texture2D tex0 : register(t0); //テクスチャ
Texture2D tex1 : register(t1); //テクスチャ

SamplerState samp; //テクスチャサンプラ

float4 main(PS_IN pi) : SV_TARGET{
    //float2 uv = pi.texcoord * 0.5f;
    float2 uv;
    float angle = 3.14159f / 180 * 45;
    uv.x = cos(angle) * pi.texcoord.x + sin(angle) * pi.texcoord.y;
    uv.y = cos(angle) * pi.texcoord.y - sin(angle) * pi.texcoord.x;
    
    return tex0.Sample(samp, pi.texcoord) * pi.color.g + tex1.Sample(samp, pi.texcoord) * pi.color.r;
    //大きなテクスチャと小さなテクスチャを重ねることで繰り返し感をなくす
    //return tex0.Sample(samp, pi.texcoord) * 0.5f + tex1.Sample(samp, uv) * 0.5f; //* pi.color; //uvの座標のサンプラーのテクスチャの色を返す

}
