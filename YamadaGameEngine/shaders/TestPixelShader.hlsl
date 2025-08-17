#include "TestHeader.hlsli"


float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float4 texColor = gTexture.Sample(gSampler, input.TexCoord);

    // 頂点カラーは固定(白)にしておき、マテリアル色を掛ける
    return texColor * BaseColor;
    
}
