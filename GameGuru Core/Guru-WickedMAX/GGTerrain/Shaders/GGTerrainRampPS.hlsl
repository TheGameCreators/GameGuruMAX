#include "GGTerrainConstants.hlsli"

struct PixelIn
{
	float4 position : SV_POSITION;
	float3 origPos : TEXCOORD1;
};
 
float4 main( PixelIn IN ) : SV_TARGET
{
	float3 edgeColor = float3( 244.0/255.0, 239.0/255.0, 38.0/255.0 );

	float color = abs(IN.origPos.x) + abs(IN.origPos.z) - 1;
	color = clamp( color*0.4 + 0.2, 0, 1 );
	if ( abs(IN.origPos.x) > 0.95 && abs(IN.origPos.z) > 0.95 ) color = 1;
	if ( color < 1 ) edgeColor = float3( 0.7, 0.7, 0.7 );
	return float4( edgeColor, color );
}