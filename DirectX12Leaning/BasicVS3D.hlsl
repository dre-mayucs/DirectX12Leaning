#include "Basic.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    VSOutput output;
    output.normal = normal;
    output.svpos = mul(mat, pos);
    output.uv = uv;
    return output;
}