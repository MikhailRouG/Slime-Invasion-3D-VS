
struct PS_IN
{
    float4 posH : SV_POSITION; //System Value SV_Position
    float4 color: COLOR0;
    float2 uv   : TEXCOORD0;
};

Texture2D tex;
SamplerState samp;

float4 main(PS_IN pi) : SV_TARGET
{
    float4 color = tex.Sample(samp, pi.uv) * pi.color;

   if(color.a < 0.01f)
    {
        discard;
    }
    return color;

}
