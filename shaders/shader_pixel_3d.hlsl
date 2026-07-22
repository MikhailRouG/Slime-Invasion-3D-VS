cbuffer PS_CONSTANT_BUFFER : register(b0)
{
    float4 diffuse_color;
};

cbuffer PS_CONSTANT_BUFFER : register(b1)
{
    float4 ambient_color;
};

cbuffer PS_CONSTANT_BUFFER : register(b2)
{
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
    float3 point_light_dummy;
};


struct PS_IN
{
    float4 posH : SV_POSITION;
    float4 posW : POSITION0;
    float4 posLightWVP : POSITION1;
    float4 normalW : NORMAL0;
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;
};
Texture2D tex0 : register(t0);
Texture2D tex2 : register(t2);
SamplerState samp : register(s0);
//SamplerState samp1: register(s1);

//float4 main(PS_IN pi) : SV_TARGET
//{
//    float3 material_color = tex0.Sample(samp, pi.texcoord).rgb * pi.color.rgb * diffuse_color.rgb;
    
//    float4 normalW = normalize(pi.normalW);
//    //float dl = max(0.0f, dot(-directional_world_vector, normalW)); //“ађПЃA0~1
//  //  float dl = (dot(-directional_world_vector, normalW) + 1.0f) * 0.5f;
//    float3 lightDir = normalize(-directional_world_vector.xyz);
//    float dl = max(dot(lightDir, normalW.xyz), 0.0f);
//    float3 diffuse = material_color * directional_color.rgb * dl;
   
//    float3 ambient = material_color * ambient_color.rgb;
    
//    float3 toEye = normalize(eye_posW - pi.posW.xyz);
//    float3 r = reflect(normalize(directional_world_vector), normalW).xyz;
//    float t = pow(max(dot(r, toEye), 0.0f), specular_power);
//    //float3 specular = specular_color.rgb * t;
//    float NdotV = saturate(dot(normalW.xyz, toEye));
//    float giSpecular = pow(NdotV, specular_power * 0.15f);
//    float3 specular = specular_color.rgb * giSpecular * 0.25f;

//    float alpha = tex0.Sample(samp, pi.texcoord).a * diffuse_color.a;
   
//    float3 color = ambient + diffuse + specular;
//    float lim = 1.0f - max(dot(normalW.xyz, toEye), 0.0f);
//    lim = pow(lim, 5.0f);
    
//    for (int i = 0; i < point_light_count; i++)
//    {
       
//        float3 lightToPixel = pi.posW.xyz - point_light[i].posW;
//        float D = length(lightToPixel);
//        float A = pow(max(1.0f - 1.0f / point_light[i].range * D, 0.0f), 2.0f);
//        float dl = max(0.0f, dot(-normalize(lightToPixel), normalW.xyz));
//        color += material_color * point_light[i].color.rgb * A * dl;
        
//      //  float3 r = reflect(normalize(lightToPixel), normalW.xyz).xyz;
//        float3 r = reflect(-lightDir, normalW.xyz);
//        float t = pow(max(dot(r, toEye), 0.0f), specular_power);
//        float3 point_light_specular = point_light[i].color.rgb * t;
//        color += point_light_specular;
//    }
    
//    float2 shadowmap_uv = pi.posLightWVP.xy / pi.posLightWVP.w;
//    shadowmap_uv = shadowmap_uv * float2(0.5f, -0.5f) + 0.5f;

//    float depthmap_z = tex2.Sample(samp, shadowmap_uv).r;

//    float shadowmap_z = pi.posLightWVP.z / pi.posLightWVP.w;

//    if (shadowmap_uv.x < 0 || shadowmap_uv.x > 1 ||
//    shadowmap_uv.y < 0 || shadowmap_uv.y > 1)
//    {
//    // вне shadowmap → без тени
//    }
//    else
//    {
//        float depthmap_z = tex2.Sample(samp, shadowmap_uv).r;
//        if (shadowmap_z > depthmap_z + 0.001f)
//            color *= 0.5f;
//    }
//    //color *= diffuse_color.rgb;
//    return float4(color, alpha);
//}
float4 main(PS_IN pi) : SV_TARGET
{
    // --- Base data ---
    float3 N = normalize(pi.normalW);
    float3 V = normalize(eye_posW - pi.posW);

    float4 tex = tex0.Sample(samp, pi.texcoord);
    float3 material = tex.rgb * pi.color.rgb * diffuse_color.rgb;

    // --- Directional light ---
    float3 Ld = normalize(-directional_world_vector);
    float NdotL = saturate(dot(N, Ld));

    float3 diffuse = material * directional_color.rgb * NdotL;
    float3 ambient = material * ambient_color.rgb;

    // cheap specular (Blinn-like)
    float NdotV = saturate(dot(N, V));
    float spec = pow(NdotV, specular_power * 0.15f);
    float3 specular = specular_color.rgb * spec * 0.25f;

    float3 color = ambient + diffuse + specular;

    // --- Point lights ---
    for (int i = 0; i < point_light_count; i++)
    {

        float3 L = point_light[i].posW - pi.posW;
        float dist2 = dot(L, L);

        float invRange = 1.0f / point_light[i].range;
        float att = saturate(1.0f - sqrt(dist2) * invRange);
        att *= att;

        L *= rsqrt(dist2); // normalize

        float NdotLp = saturate(dot(N, L));
        color += material * point_light[i].color.rgb * att * NdotLp;

        // Blinn-Phong specular
        float3 H = normalize(L + V);
        float specPL = pow(saturate(dot(N, H)), specular_power);
        color += point_light[i].color.rgb * specPL * 0.2f;
    }
    
    float3 proj = pi.posLightWVP.xyz / pi.posLightWVP.w;
    float2 uv = proj.xy * float2(0.5f, -0.5f) + 0.5f;

    float shadowDepth = tex2.Sample(samp, uv).r;
    float shadow = step(proj.z, shadowDepth + 0.001f);

    float inBounds =
        step(0.0f, uv.x) * step(uv.x, 1.0f) *
        step(0.0f, uv.y) * step(uv.y, 1.0f);

    color *= lerp(0.5f, 1.0f, shadow * inBounds);

    return float4(color, tex.a * diffuse_color.a);
}

