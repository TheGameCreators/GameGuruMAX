#include "GGTerrainConstants.hlsli"

struct PixelIn
{
	float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD0;
	float3 origPos : TEXCOORD1;
};
 
float4 main( PixelIn IN ) : SV_TARGET
{
	float3 editColor = float3( 244.0/255.0, 239.0/255.0, 38.0/255.0 );

	float color = 1 - (IN.worldPos.y / 30000);
	color = clamp( color*0.4, 0, 1 );
	if ( color < 0.005 ) color = 1;
	if ( abs(IN.origPos.x) > 0.995 && abs(IN.origPos.z) > 0.995 ) color = 1;
	if ( color < 1 ) editColor = float3( 0.7, 0.7, 0.7 );
	return float4( editColor, color );
}