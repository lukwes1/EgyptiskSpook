texture2D tex : register(t0);
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

cbuffer effectBuffer : register(b3)
{
    float4 planeNormal;
    float4 planePoint;
}

float4 main(VS_OUT input) : SV_TARGET
{
    float effectScalar = saturate(2.25 - abs(dot(planeNormal.xyz, input.wPos.xyz - planePoint.xyz)));
    return float4(tex.Sample(sSampler, input.uv).rgb * (effectScalar + 0.5), 0.25f);
}