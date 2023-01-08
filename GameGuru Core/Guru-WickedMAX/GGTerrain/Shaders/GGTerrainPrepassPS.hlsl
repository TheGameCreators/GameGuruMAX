Texture2DArray<float> texPageTableArray : register( t53 );
Texture2D<float> texPageTableFinal : register( t54 );
SamplerState sampler0 : register( s0 );

#include "GGTerrainConstants.hlsli"
#include "../GGTerrainPageSettings.h"

struct PixelIn
{
    float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float2 uvPos : TEXCOORD1;
	uint lodLevel : TEXCOORD2;
};

struct Output
{
	float4 velocity : SV_TARGET0;
	uint   readback : SV_TARGET1;  // virtual texture read back
};

Output main( PixelIn IN )
{
	Output output;
	output.velocity = float4( 0, 0, 0, 0 );

	uint maxLayer = terrain_numLODLevels - 1;
	uint maxMip = GGTERRAIN_MAX_PAGE_TABLE_MIP; // not a full mip stack
	uint maxTotal = maxLayer + maxMip - 1;

	uint LOD = texPageTableFinal.CalculateLevelOfDetailUnclamped( sampler0, IN.uv ); // LOD 0 is the highest level of detail
	uint origLOD = LOD;
	uint2 pixelPos = IN.position.xy;
	if ( origLOD <= 5 )
	{
		if ( pixelPos.x & 0x4 ) LOD += 1;
		if ( pixelPos.y & 0x4 ) LOD += 2;
	}
	else 
	{
		if ( pixelPos.x & 0x4 ) LOD += 1;
	}
	LOD = clamp( LOD, IN.lodLevel, maxTotal );
	
	uint2 pageUV;
	if ( LOD <= maxLayer )
	{
		float2 levelUV = IN.uvPos - float2( terrain_LOD[ LOD ].x, terrain_LOD[ LOD ].z );
		levelUV *= terrain_LOD[ LOD ].size;
		levelUV.y = 1 - levelUV.y;

		pageUV = (uint2) (levelUV * 256);
		pageUV = clamp( pageUV, uint2(0,0), uint2(255,255) );
	}
	else
	{
		float2 levelUV = IN.uvPos - float2( terrain_LOD[ maxLayer ].x, terrain_LOD[ maxLayer ].z );
		levelUV *= terrain_LOD[ maxLayer ].size;
		levelUV.y = 1 - levelUV.y;

		uint mipLevel = LOD - maxLayer;
		uint mipSize = 256 >> mipLevel; 
		pageUV = (uint2) (levelUV * mipSize);
		pageUV = clamp( pageUV, uint2(0,0), uint2(mipSize-1,mipSize-1) );
	}
	
	// texture is cleared with 0, so increment mip level by 1
	output.readback = (LOD + 1) << 16; 
	output.readback |= (pageUV.y << 8);
	output.readback |= pageUV.x;
	
	return output;
}