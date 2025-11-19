/*==============================================================================

   ビルボード描画用頂点シェーダー [shader_pixel_billboard.hlsl]


														 Author : Harada Ren
														 Date   : 2025/11/14
--------------------------------------------------------------------------------

==============================================================================*/
struct PS_IN
{
    float4 posL : POSITION0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

Texture2D tex; // テクスチャ
SamplerState samp; //テクスチャサンプラー

float4 main(PS_IN pi) : SV_TARGET
{
    return tex.Sample(samp, pi.uv) * pi.color;
}