#include "TestHeader.hlsli"


float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float4 texColor = gTexture.Sample(gSampler, input.TexCoord);

    // ���_�J���[�͌Œ�(��)�ɂ��Ă����A�}�e���A���F���|����
    return texColor * BaseColor;
    
}
