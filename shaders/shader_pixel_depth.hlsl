
struct PS_IN
{
    float4 posH : SV_POSITION0;
};

float4 main(PS_IN pi) : SV_TARGET
{
    return float4(pi.posH.z, pi.posH.z, pi.posH.z, 1.0f);

}