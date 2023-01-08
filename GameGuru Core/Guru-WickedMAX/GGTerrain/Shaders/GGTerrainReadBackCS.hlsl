
Texture2D<uint>   input  : register( t50 );
RWTexture2D<uint> output : register( u0 );

#include "GGTerrainConstants.hlsli"

[numthreads(8, 8, 1)]
void main( uint3 dispatchThreadId : SV_DispatchThreadID )
{
	uint2 dim;
	output.GetDimensions( dim.x, dim.y );
	if ( dispatchThreadId.x > dim.x || dispatchThreadId.y > dim.y ) return;

	uint2 uv = dispatchThreadId.xy * terrain_readBackReduction + 1;

	output[ dispatchThreadId.xy ] = input.Load( uint3(uv.xy, 0) ).r;
}
