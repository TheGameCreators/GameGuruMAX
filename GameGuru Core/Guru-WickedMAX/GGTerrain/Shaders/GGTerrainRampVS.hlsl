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
	float3 origPos : TEXCOORD1;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;
 
	OUT.origPos = IN.position;
	float4 pos = float4( IN.position, 1 );
	pos = mul( terrain_rampWorldMat, pos );
	OUT.position = mul( g_xCamera_VP, pos );

	return OUT;
}

