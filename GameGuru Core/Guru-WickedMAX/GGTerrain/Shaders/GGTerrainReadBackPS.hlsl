Texture2D<float> texPageTable : register( t3 );

SamplerState sampler0 : register( s0 );

struct PixelIn
{
    float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float2 uv2 : TEXCOORD1;
};

static const float mipSizeLUT[13] = { 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1 };

#include "../GGTerrainPageSettings.h"

#if (GGTERRAIN_TEXTURE_FILTERING == GGTERRAIN_TEXTURE_FILTERING_TRILINEAR)
	static const float offset = 0;
#elif (GGTERRAIN_TEXTURE_FILTERING == GGTERRAIN_TEXTURE_FILTERING_ANISO_X4)
	static const float offset = 0.115;
#else
	static const float offset = 0.15;
#endif

uint main( PixelIn IN ) : SV_TARGET
{
	float LOD = texPageTable.CalculateLevelOfDetail( sampler0, IN.uv ); // LOD 0 is the highest level of detail
	uint mipLevel = (uint) (LOD + offset); // this offset appears to be necessary for ansiotropic filtering, possibly because we render at 1/4th the resolution?
	uint mipSize = mipSizeLUT[ mipLevel ]; 

	uint2 pageUV = (uint2) (IN.uv2 * mipSize);
	pageUV = min( pageUV, uint2(mipSize-1,mipSize-1) );

	// texture is cleared with 0, so invert mipLevel so highest detail level is 12 and lowest is 0
	uint output = (12 - mipLevel) << 28; 
	output |= (pageUV.y << 14);
	output |= pageUV.x;
		
	return output;
}