#include "GGTerrainConstants.hlsli"

cbuffer CameraCB : register( b1 )
{
	float4x4	g_xCamera_VP;			// View*Projection
	float4		g_xCamera_ClipPlane;
};

struct VertexIn
{
    float3 position : POSITION;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD0;
	float3 origPos : TEXCOORD1;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;
 
	float4 pos;
	pos.x = IN.position.x * terrain_mapEditSize;
	pos.y = IN.position.y * 30000;
	pos.z = IN.position.z * terrain_mapEditSize;
	pos.w = 1;

	OUT.origPos = IN.position;
	OUT.worldPos = pos.xyz;
	OUT.position = mul( g_xCamera_VP, pos );

	return OUT;
}

