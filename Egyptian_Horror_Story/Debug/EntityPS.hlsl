texture2D tex : register(t0);
texture2D shadowMap : register(t1);
SamplerState sSampler;

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 wPos : WORLDPOS;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

cbuffer lightBuffer : register(b0)
{
    float4 lightPos;
    float4 lightDir;
};

cbuffer cameraPos : register(b1)
{
    float4 camPos;
}

cbuffer lightVP : register(b2)
{
    //OPTIMERING
    matrix lView;
    matrix lProjection;
}

float4 main(VS_OUT input) : SV_TARGET
{
    return tex.Sample(sSampler, input.uv);
}