#include "Basic.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    //return float4(tex.Sample(smp, input.uv));
    //return float4(input.normal, 1);

    float4 texcolor = float4(tex.Sample(smp, input.uv));

    float3 light = normalize(float3(1, -1, 1));
    float diffuse = saturate(dot(-(light), input.normal));
    float brightness = diffuse + 0.3f;

    return float4(texcolor.rgb * brightness, texcolor.a)/* * color*/;
}