#include "TestHeader.hlsli"

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    float4 pos = mul(float4(input.Pos, 1.0f), World);
    pos = mul(pos, View);
    pos = mul(pos, Projection);
    output.Pos = pos;
    output.Col = input.Col;
    return output;
}