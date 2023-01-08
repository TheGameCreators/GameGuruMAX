Texture2D<uint> texReadBack : register( t0 ); // read back
Texture2DArray<float> texPageTableArray : register( t1 );
Texture2D<float> texPageTableFinal : register( t2 );
Texture2D texPageCache : register( t3 );
Texture2D texNormals : register( t4 );
Texture2D texMask : register( t5 );
Texture2DArray<float4> texLODNormals : register( t6 );

Texture2D texture_skyluminancelut : register( t13 );

SamplerState sampler0 : register( s0 );

#include "GGTerrainConstants.hlsli"

struct PixelIn
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

static const float4 mipColors[16] = { 
	float4( 1.0, 0.0, 0.0, 1.0 ),
	float4( 0.0, 1.0, 0.0, 1.0 ),
	float4( 0.0, 0.0, 1.0, 1.0 ),
	float4( 1.0, 1.0, 0.0, 1.0 ),
	float4( 0.0, 1.0, 1.0, 1.0 ),
	float4( 1.0, 0.0, 1.0, 1.0 ),
	float4( 1.0, 1.0, 1.0, 1.0 ),
	float4( 0.5, 0.0, 0.0, 1.0 ),
	float4( 0.0, 0.5, 0.0, 1.0 ),
	float4( 0.0, 0.0, 0.5, 1.0 ),
	float4( 0.5, 0.5, 0.0, 1.0 ),
	float4( 0.0, 0.5, 0.5, 1.0 ),
	float4( 0.5, 0.0, 0.5, 1.0 ),
	float4( 0.5, 0.5, 0.5, 1.0 ),
	float4( 0.25, 0.25, 0.25, 1.0 ),
	float4( 0.0, 0.0, 0.0, 1.0 ),
};
 
float4 main( PixelIn IN ) : SV_TARGET
{
	uint maxLayer = terrain_numLODLevels - 1;
	uint maxMip = 9; // assumes 256 x256 pixel page tables
	uint maxTotal = maxLayer + maxMip - 1; 

	
	// read back debug
	uint width, height;
	texReadBack.GetDimensions( width, height );
    uint value = texReadBack.Load( int3(IN.uv*float2(width,height), 0) );

	uint value2 = texReadBack.Load( int3(IN.uv*float2(width,height)-float2(1,0), 0) );
	uint value3 = texReadBack.Load( int3(IN.uv*float2(width,height)-float2(0,1), 0) );
	uint value4 = texReadBack.Load( int3(IN.uv*float2(width,height)-float2(1,1), 0) );

	value = min( value, value2 );
	value = min( value, value3 );
	value = min( value, value4 );

	uint mipLevel = value >> 16; // 1 is highest detail level, 0 is no page
	if ( mipLevel == 0 ) mipLevel = 15;
	else mipLevel = mipLevel - 1;
	float3 color = mipColors[ mipLevel ].rgb;

	uint x = value & 0x01;
	uint y = (value >> 8) & 0x01;
	uint combined = (y << 1) | x;
	combined = combined * 16 + 191;
	float intensity = combined / 255.0f;
	
	return float4( color * intensity, 1.0 );
	


	/*
	// page table array debug
	uint2 dimensions;
	uint numElements;
	texPageTableArray.GetDimensions( dimensions.x, dimensions.y, numElements );
	uint level = 0;
	float pageEntry = texPageTableArray.Load( uint4(IN.uv*dimensions, level, 0) ).r;
	*/
	/*
	// final page table debug
	uint2 dimensions;
	texPageTableFinal.GetDimensions( dimensions.x, dimensions.y );
	uint mipLevel = 2;
	dimensions >>= mipLevel;
	float pageEntry = texPageTableFinal.Load( uint3(IN.uv*dimensions, mipLevel) ).r;
	*/
	/*
	float offset = 0;
	uint iPageEntry = (uint) (pageEntry * 65535);
	if ( iPageEntry != 0 ) offset = 0.5;
	uint iOffsetX = iPageEntry & 0xFF;
	uint iOffsetY = iPageEntry >> 8;
	
	return float4( iOffsetX / 32.0 + offset, iOffsetY / 32.0 + offset, 0, 1 );
	*/

	// page cache debug
	//return texPageCache.SampleLevel( sampler0, IN.uv, 0 );

	//return texMask.SampleLevel( sampler0, IN.uv, 0 );

	//return texLODNormals.SampleLevel( sampler0, float3(IN.uv,0), 0 );
}