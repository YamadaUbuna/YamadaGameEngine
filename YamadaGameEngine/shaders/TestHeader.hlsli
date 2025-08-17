struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float4 Col : COLOR0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float4 Col : COLOR0;
};

// �萔�o�b�t�@
cbuffer ViewBuffer : register(b0)
{
    float4x4 View;
}
cbuffer ProjectionBuffer : register(b1)
{
    float4x4 Projection;
}
cbuffer WorldBuffer : register(b2)
{
    float4x4 World;
}
cbuffer MaterialBuffer : register(b3)
{
    float4 BaseColor; // �Q�[�����W�b�N�ŏ���������F
}

// �e�N�X�`���ƃT���v���[
Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);