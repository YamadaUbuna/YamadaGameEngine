#include "TestHeader.hlsli"


float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float4 texColor = gTexture.Sample(gSampler, input.TexCoord);
    return texColor;
}
