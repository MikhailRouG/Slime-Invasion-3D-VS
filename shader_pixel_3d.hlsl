/*==============================================================================

   3D描画用ピクセルシェーダー [shader_pixel_3d.hlsl]
														 Author : Harada Ren
														 Date   : 2025/09/10
--------------------------------------------------------------------------------

==============================================================================*/

struct PS_IN{
    float4 posH : SV_POSITION;
    float4 color: COLOR0;
    float2 texcoord : TEXCOORD0;
};

Texture2D tex; //テクスチャ
SamplerState samp; //テクスチャサンプラ

float4 main(PS_IN pi) : SV_TARGET{
    return tex.Sample(samp, pi.texcoord) * pi.color; //uvの座標のサンプラーのテクスチャの色を返す

}
