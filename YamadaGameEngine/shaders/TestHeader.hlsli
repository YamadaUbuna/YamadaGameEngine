
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

cbuffer ViewBuffer : register(b0) //これはカメラの位置で決まる
{
    float4x4 View; //ビュー変換行列
}
cbuffer ProjectionBuffer : register(b1) //これはカメラの位置で決まる
{
    float4x4 Projection; //透視射影変換行列
}
cbuffer WorldBuffer : register(b2) // メッシュが持つ自身のマトリクス
{
    float4x4 World; // ワールド変換行列
}