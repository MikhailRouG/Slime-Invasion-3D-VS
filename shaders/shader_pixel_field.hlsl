
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
    float4 directional_color = { 1.0f, 1.0f, 1.0f, 1.0f };
};

cbuffer PS_CONSTANT_BUFFER : register(b3)
{
    float3 eye_posW;
    float specular_power = 30.0f;
    float4 specular_color = { 0.1f, 0.1f, 0.1f, 1.0f };
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
    float3 point_light_dummy; //float4‚В•Є‚ё‚В‘—‚й‚Ѕ‚Я‚Мdummy
};


struct PS_IN{
    float4 posH : SV_POSITION;
    float4 posW : POSITION0;
    float4 posLightWVP : POSITION1;
    float4 normalW : NORMAL0;
    float4 blend : COLOR0;
    float2 texcoord : TEXCOORD0;
};

//”z—с‚Е‚а‚ў‚ў
Texture2D tex0 : register(t0); //ѓeѓNѓXѓ`ѓѓ
Texture2D tex1 : register(t1); //ѓeѓNѓXѓ`ѓѓ
Texture2D tex2 : register(t2);
SamplerState samp; //ѓeѓNѓXѓ`ѓѓѓTѓ“ѓvѓ‰

float4 main(PS_IN pi) : SV_TARGET{
    //uv‚р‰БЌH‚µ‚ДѓeѓNѓXѓ`ѓѓ‚р‰с“]‚·‚й
    //float2 uv = pi.texcoord * 0.5f;
    float2 uv;
    float angle = 3.14159f / 180 * 45;
    uv.x = cos(angle) * pi.texcoord.x + sin(angle) * pi.texcoord.y;
    uv.y = cos(angle) * pi.texcoord.y - sin(angle) * pi.texcoord.x;
    
    //2–‡‚МѓeѓNѓXѓ`ѓѓ‚рѓuѓЊѓ“ѓhѓpѓ‰ѓЃЃ[ѓ^‚рЉо‚ЙѓuѓЊѓ“ѓh‚·‚й
    //‘е‚«‚ИѓeѓNѓXѓ`ѓѓ‚ЖЏ¬‚і‚ИѓeѓNѓXѓ`ѓѓ‚рЏd‚Л‚й‚±‚Ж‚ЕЊJ‚и•Ф‚µЉґ‚р‚И‚­‚·
    float4 tex_color = tex0.Sample(samp, pi.texcoord) * pi.blend.g + tex1.Sample(samp, pi.texcoord) * pi.blend.r;
 
    //ЌЮЋї‚МђF
    float3 material_color = tex_color.rgb * diffuse_color.rgb;
    
    //•АЌsЊхЊ№(ѓfѓBѓtѓ…Ѓ[ѓYѓ‰ѓCѓg)
    float4 normalW = normalize(pi.normalW);
    //float dl = max(0.0f, dot(-directional_world_vector, normalW)); //“ађПЃA0~1
    float dl = (dot(-directional_world_vector, normalW) + 1.0f) * 0.5f;
    float3 diffuse = material_color * directional_color.rgb * dl;
    
    //ЉВ‹«Њх(ѓAѓ“ѓrѓGѓ“ѓgѓJѓ‰Ѓ[(ѓ‰ѓCѓg))
    float3 ambient = material_color * ambient_color.rgb;
    
    //ѓXѓyѓLѓ…ѓ‰
    float3 toEye = normalize(eye_posW - pi.posW.xyz);
    float3 r = reflect(normalize(directional_world_vector), normalW).xyz;
    float t = pow(max(dot(r, toEye), 0.0f), specular_power);
    float3 specular = specular_color * t;

    //ЌЕЏIѓJѓ‰Ѓ[
    //float alpha = tex.Sample(samp, pi.texcoord).a * diffuse_color.a; //“§–ѕ‚И’n–К‚рЋg‚¤‚И‚з
    float3 color = ambient + diffuse + specular; //ЌЕЏI“I‚Й‰дЃX‚М–Ъ‚Й“Н‚­ђF
    
     //“_ЊхЊ№(ѓ|ѓCѓ“ѓgѓ‰ѓCѓg)
    for (int i = 0; i < point_light_count; i++)
    {
       
        //“_ЊхЊ№‚©‚з–К(ѓsѓNѓZѓ‹)‚Ц‚МѓxѓNѓgѓ‹ЋZЏo
        float3 lightToPixel = pi.posW.xyz - point_light[i].posW;
        //–К(ѓsѓNѓZѓ‹)‚Жѓ‰ѓCѓg‚Ж‚М‹——Ј‚р‘Є‚й
        float D = length(lightToPixel);
    
        //‰e‹ї—Н‚МЊvЋZ
        float A = pow(max(1.0f - 1.0f / point_light[i].range * D, 0.0f), 2.0f);
        // range = 400 length=0,    A*A=1;
        //                   =100,  A*A=0.75
        //                   =200,  A*A=0.5
        //                   =300,  A*A=0.25
        //                   =400,  A*A=0;
        
        //“_ЊхЊ№‚Ж–К(ѓsѓNѓZѓ‹)‚Ж‚МЊь‚«‚рЌl—¶‚Й“ь‚к‚й
        float dl = max(0.0f, dot(-normalize(lightToPixel), normalW.xyz)); //“ађПЃA0~1
        //float dl = (dot(-directional_world_vector, normalW) + 1.0f) * 0.5f;
        
        //“_ЊхЊ№‚М‰e‹ї‚р‰БЋZ
        color += material_color * point_light[i].color.rgb * A * dl;
        
        //“_ЊхЊ№‚МѓXѓyѓLѓ…ѓ‰
        float3 r = reflect(normalize(lightToPixel), normalW.xyz).xyz;
        float t = pow(max(dot(r, toEye), 0.0f), specular_power);
        float3 point_light_specular = point_light[i].color.rgb * t;

        //“_ЊхЊ№‚МѓXѓyѓLѓ…ѓ‰‚р‰БЋZ
        color += point_light_specular;
    }
    float2 shadowmap_uv = pi.posLightWVP.xy / pi.posLightWVP.w;
    shadowmap_uv = shadowmap_uv * float2(0.5f, -0.5f) + 0.5f;

    float shadowmap_z = pi.posLightWVP.z / pi.posLightWVP.w;

    bool inBounds = (shadowmap_uv.x >= 0.0f && shadowmap_uv.x <= 1.0f &&
                     shadowmap_uv.y >= 0.0f && shadowmap_uv.y <= 1.0f);
    if (inBounds)
    {
        float depthmap_z = tex2.Sample(samp, shadowmap_uv).r;
        if (shadowmap_z > depthmap_z + 0.001f)
            color *= 0.5f;
    }

    return float4(color, 1.0f);

    //return tex0.Sample(samp, pi.texcoord) * 0.5f + tex1.Sample(samp, uv) * 0.5f; //* pi.color; //uv‚МЌА•W‚МѓTѓ“ѓvѓ‰Ѓ[‚МѓeѓNѓXѓ`ѓѓ‚МђF‚р•Ф‚·

}
