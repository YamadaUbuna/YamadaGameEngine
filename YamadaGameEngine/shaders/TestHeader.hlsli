
struct VS_INPUT
{
    float3 Pos : POSITION;
    float4 Col : COLOR0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Col : COLOR0;
};

cbuffer ViewBuffer : register(b0) //����̓J�����̈ʒu�Ō��܂�
{
    float4x4 View; //�r���[�ϊ��s��
}
cbuffer ProjectionBuffer : register(b1) //����̓J�����̈ʒu�Ō��܂�
{
    float4x4 Projection; //�����ˉe�ϊ��s��
}
cbuffer WorldBuffer : register(b2) // ���b�V���������g�̃}�g���N�X
{
    float4x4 World; // ���[���h�ϊ��s��
}