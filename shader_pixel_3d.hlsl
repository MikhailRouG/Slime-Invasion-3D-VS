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
    float4 directional_color;
};

cbuffer PS_CONSTANT_BUFFER : register(b3)
{
    float3 eye_posW;
    float specular_power;
    float4 specular_color;
};

struct PointLight
{
    float3 posW;
    float range;
    float4 color;
};
cbuffer PS_CONSTANT_BUFFER : register(b4)
{
    PointLight point_light[4];
    int point_light_count;
    float3 point_light_dummy; //float4つ分ずつ送るためのdummy
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
    //材質
    float3 material_color = tex.Sample(samp, pi.texcoord).rgb * pi.color.rgb * diffuse_color.rgb;
    
    //並行光源(ディフューズライト)
    float4 normalW = normalize(pi.normalW);
    //float dl = max(0.0f, dot(-directional_world_vector, normalW)); //内積、0~1
    float dl = (dot(-directional_world_vector, normalW) + 1.0f) * 0.5f;
    float3 diffuse = material_color * directional_color.rgb * dl;
    
    //環境光(アンビエントカラー(ライト))
    float3 ambient = material_color * ambient_color.rgb;
    
    //スペキュラライト
    float3 toEye = normalize(eye_posW - pi.posW.xyz);
    float3 r = reflect(normalize(directional_world_vector), normalW).xyz;
    float t = pow(max(dot(r, toEye), 0.0f), specular_power);
    float3 specular = specular_color.rgb * t;
    
    float alpha = tex.Sample(samp, pi.texcoord).a * diffuse_color.a;
    float3 color = ambient + diffuse + specular; //最終的に我々の目に届く色
    
    //リムライト
    float lim = 1.0f-max(dot(normalW.xyz, toEye), 0.0f);
    lim = pow(lim, 5.0f);

    //点光源(ポイントライト)
    for (int i = 0; i < point_light_count; i++){
       
        //点光源から面(ピクセル)へのベクトル算出
        float3 lightToPixel = pi.posW.xyz - point_light[i].posW;
        //面(ピクセル)とライトとの距離を測る
        float D = length(lightToPixel);
    
        //影響力の計算
        float A = pow(max(1.0f - 1.0f / point_light[i].range * D, 0.0f), 2.0f);
        // range = 400 length=0,    A*A=1;
        //                   =100,  A*A=0.75
        //                   =200,  A*A=0.5
        //                   =300,  A*A=0.25
        //                   =400,  A*A=0;
        
        //点光源と面(ピクセル)との向きを考慮に入れる
        float dl = max(0.0f, dot(-normalize(lightToPixel), normalW.xyz)); //内積、0~1
        //float dl = (dot(-directional_world_vector, normalW) + 1.0f) * 0.5f;
        
        //点光源の影響を加算
        color += material_color * point_light[i].color.rgb * A * dl;
        
        //点光源のスペキュラ
        float3 r = reflect(normalize(lightToPixel), normalW.xyz).xyz;
        float t = pow(max(dot(r, toEye), 0.0f), specular_power);
        float3 point_light_specular = point_light[i].color.rgb * t;

        //点光源のスペキュラを加算
        color += point_light_specular;
    }
    
    return float4(color, alpha); //uvの座標のサンプラーのテクスチャの色を返す
}
