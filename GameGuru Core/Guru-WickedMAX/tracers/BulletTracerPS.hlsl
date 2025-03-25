
//desc.vs = wiResourceManager::LoadShader(wiResourceManager::VS, "BulletTracerVS.cso");
//desc.ps = wiResourceManager::LoadShader(wiResourceManager::PS, "BulletTracerPS.cso");

Texture2D tracerTexture : register(t0);
SamplerState samplerLinear : register(s0);

cbuffer TracerCB : register(b2)
{
    matrix g_mWorldViewProj;
    float4 g_TintColor;
    float g_GlowIntensity;
    float g_ScrollSpeed;
    float g_Time;
    float g_ScaleV;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    // Sample the tracer texture
    float4 texColor = tracerTexture.Sample(samplerLinear, input.uv);

    // Multiply texture by color and alpha tint
    float4 finalColor = texColor * g_TintColor;

    // Apply glow as HDR intensity boost (optional)
    finalColor.rgb *= g_GlowIntensity;

    // Premultiply alpha for correct blending
    finalColor.rgb *= finalColor.a;

    //finalColor.rgb = texColor.rgb;
    
    //finalColor = float4(1, 0, 0, 1);

    return finalColor;
}

