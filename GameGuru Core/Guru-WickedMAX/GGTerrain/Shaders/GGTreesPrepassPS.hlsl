Texture2DArray texTree : register( t50 );
Texture2D<float> texNoise : register( t51 );

SamplerState samplerBilinearWrap : register( s0 );
SamplerState samplerTrilinearClamp : register( s1 );

#include "PBR/ShaderInterop_Renderer.h"
#include "GGTreesConstants.hlsli"

struct PixelIn
{
    float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD0;
	float  clip : SV_ClipDistance0;
	float2 uv : TEXCOORD1;
	uint data : TEXCOORD2;
};

struct Output
{
	float4 velocity : SV_TARGET0;
	uint   readback : SV_TARGET1;  // virtual texture read back
};

Output main( PixelIn IN )
{
	uint treeType = GetTreeType( IN.data );
	uint index = GetTreeVariation( IN.data );

	float alpha = texTree.Sample( samplerTrilinearClamp, float3(IN.uv, treeType) ).a;
	if ( alpha < 0.3 ) discard;

	alpha = (alpha - 0.3) / max(fwidth(alpha),0.0001) + 0.5;

	if ( !any(g_xCamera_ClipPlane) )
	{
		float3 viewDir = g_xCamera_CamPos - IN.worldPos;
		float sqrDist = dot( viewDir, viewDir );

		float noise = texNoise.Sample( samplerBilinearWrap, IN.uv*3 );
		float limit = (noise * GGTREES_LOD_TRANSITION) + tree_lodDist;
		if ( sqrDist < limit*limit ) discard;
	}

	Output output;
	output.velocity = float4( 0, 0, 0, alpha );
	output.readback = 0; 	
	return output;
}