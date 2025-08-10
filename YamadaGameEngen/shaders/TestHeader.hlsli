
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

cbuffer ConstantBuffer : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};
