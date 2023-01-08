Texture2DArray texGrass : register( t50 );
Texture2D<float> texNoise : register( t51 );

SamplerState samplerPointWrap : register( s0 );
SamplerState samplerTrilinearClamp : register( s1 );
SamplerState samplerTrilinearWrap : register( s2 );

#include "PBR/ShaderInterop_Renderer.h"
#include "GGGrassConstants.hlsli"

struct PixelIn
{
    float4 position : SV_POSITION;
	float2 uv : TEXCOORD1;
	float2 uvNoise : TEXCOORD4;
	float3 worldPos : TEXCOORD3;
	uint data : TEXCOORD2;
};

struct Output
{
	float4 velocity : SV_TARGET0;
	uint   readback : SV_TARGET1;  // virtual texture read back
};

Output main( PixelIn IN )
{
	Output output;
	
	uint grassType = GetGrassType( IN.data );
	uint index = GetGrassVariation( IN.data );

	float alpha = texGrass.Sample( samplerTrilinearClamp, float3(IN.uv, grassType) ).a;
	if ( alpha < 0.5 ) discard;

	float3 view = g_xCamera_CamPos - IN.worldPos;
	float sqrDist = dot( view, view );

	float noise = texNoise.Sample( samplerTrilinearWrap, IN.uvNoise );
	float limit = noise * GGGRASS_LOD_TRANSITION + grass_lodDist;
	if( sqrDist > limit*limit ) discard;

	alpha = (alpha - 0.5) / max(fwidth(alpha),0.0001) + 0.5;
	//float alpha = 1;

	output.velocity = float4( 0, 0, 0, alpha );
	output.readback = 0; 	
	return output;
}