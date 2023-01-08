Texture2DArray texGrass : register( t50 );
Texture2D<float> texNoise : register( t51 );

SamplerState samplerPointWrap : register( s0 );
SamplerState samplerTrilinearClamp : register( s1 );

#include "GGGrassConstants.hlsli"

struct PixelIn
{
    float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD0;
	uint data : TEXCOORD2;
	float2 uv : TEXCOORD1;
};

float4 main( PixelIn IN ) : SV_TARGET
{
	uint grassType = GetGrassType( IN.data );
	uint index = GetGrassVariation( IN.data );

	//float alpha = texGrass.Sample( samplerTrilinearClamp, float3(IN.uv, grassType) ).a;
	//if ( alpha < 0.5 ) discard;
	
	return float4( 1, 1, 1, 1 );
}