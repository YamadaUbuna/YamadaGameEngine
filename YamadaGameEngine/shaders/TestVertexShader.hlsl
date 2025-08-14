#include "TestHeader.hlsli"

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    float4 worldPos = mul(float4(input.Pos, 1.0f), World);
    worldPos = mul(worldPos, View);
    worldPos = mul(worldPos, Projection);
    output.Pos = worldPos;

    output.Normal = input.Normal;
    output.TexCoord = input.TexCoord;
    output.Col = input.Col;
    return output;
}
