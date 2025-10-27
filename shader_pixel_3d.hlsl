/*==============================================================================

   3D描画用ピクセルシェーダー [shader_pixel_3d.hlsl]
														 Author : Harada Ren
														 Date   : 2025/09/10
--------------------------------------------------------------------------------

==============================================================================*/

//定数バッファ
cbuffer PS_CONSTANT_BUFFER : register(b0){
    float4 diffuse_color;
};

cbuffer PS_CONSTANT_BUFFER : register(b1){
    float4 ambient_color;
};

cbuffer PS_CONSTANT_BUFFER : register(b2){
    float4 directional_world_vector;
    float4 directional_color = { 1.0f, 1.0f, 1.0f, 1.0f };
};

cbuffer PS_CONSTANT_BUFFER : register(b3){
    float3 eye_posW;
    float specular_power = 30.0f;
    float4 specular_color = { 0.1f, 0.1f, 0.1f, 1.0f };
}


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
    //材質
    float3 material_color = tex.Sample(samp, pi.texcoord).rgb * pi.color.rgb * diffuse_color.rgb;
    
    //並行光源(ディフューズライト)
    float4 normalW = normalize(pi.normalW);
    //float dl = max(0.0f, dot(-directional_world_vector, normalW)); //内積、0~1
    float dl = (dot(-directional_world_vector, normalW) + 1.0f) * 0.5f;
    float3 diffuse = material_color * directional_color.rgb * dl;
    
    //環境光(アンビエントカラー(ライト))
    float3 ambient = material_color * ambient_color.rgb;
    
    //スペキュラ
    float3 toEye = normalize(eye_posW - pi.posW.xyz);
    float3 r = reflect(normalize(directional_world_vector), normalW).xyz;
    float t = pow(max(dot(r, toEye), 0.0f), specular_power);
    //float3 specular = diffuse_color.rgb * t;
    float3 specular = specular_color * t;
    
    float alpha = tex.Sample(samp, pi.texcoord).a * diffuse_color.a;
    float3 color = ambient + diffuse + specular; //最終的に我々の目に届く色
    
    return float4(color, alpha); //uvの座標のサンプラーのテクスチャの色を返す
}
