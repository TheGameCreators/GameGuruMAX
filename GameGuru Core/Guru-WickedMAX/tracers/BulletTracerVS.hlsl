
#include "../GGterrain/Shaders/PBR/globals.hlsli"

cbuffer TracerCB : register(b2)
{
    matrix g_mWorldViewProj;
    float4 g_TintColor;
    float g_GlowIntensity;
    float g_ScrollSpeed;
    float g_Time;
    float g_ScaleV;
};

struct VSInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PSInput main(VSInput input)
{
    PSInput output;

    // Transform quad vertex to screen space
    output.pos = mul(float4(input.pos, 1.0f), g_mWorldViewProj);
    // Apply scrolling UV for texture movement along the tracer
    
    output.uv = float2(input.uv.x, ( input.uv.y * g_ScaleV) + (g_Time * g_ScrollSpeed));
    
    return output;
}

