
cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    float4x4 world;
};

cbuffer VS_CONSTANT_BUFFER : register(b1)
{
    float4x4 view;
};

cbuffer VS_CONSTANT_BUFFER : register(b2)
{
    float4x4 proj;
};
   
cbuffer VS_CONSTANT_BUFFER : register(b3)
{
    float4x4 light_view_proj;
};
struct VS_IN
{
    //:~ �Z�}���e�B�N�X
    float4 posL : POSITION0;
    float4 normalL : NORMAL0;
    float4 blend : COLOR0;
    float2 texcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float4 posW : POSITION0;
    float4 posLightWVP : POSITION1;
    float4 normalW : NORMAL0;
    float4 blend : COLOR0;
    float2 texcoord : TEXCOORD0;
};

//=============================================================================
// ���_�V�F�[�_
//=============================================================================
VS_OUT main(VS_IN vi)
{
    VS_OUT vo;
    
    //���W�ϊ�
    float4x4 mtxWV = mul(world, view); //�r���[�ϊ�
    float4x4 mtxWVP = mul(mtxWV, proj); // �v���W�F�N�V�����ϊ�
    vo.posH = mul(vi.posL, mtxWVP);
 
    vo.posLightWVP = mul(vi.posL, mul(world, light_view_proj));
    //���ʂ̃��[���h�ϊ��s��͂���(�g��k���̉e�����󂯂邽��)
    //���[���h�ϊ��s��̓]�u�t�s����g��
    float4 normalW = mul(float4(vi.normalL.xyz, 0.0f), world); //����0
    vo.normalW = normalize(normalW); //�P�ʃx�N�g����
    vo.posW = mul(vi.posL, world);

    //�n�ʂ̃e�N�X�`���̃u�����h�l�͂��̂܂܃p�X�X���[  
    vo.blend = vi.blend;
    
    vo.texcoord = vi.texcoord;
    
    return vo;
}
