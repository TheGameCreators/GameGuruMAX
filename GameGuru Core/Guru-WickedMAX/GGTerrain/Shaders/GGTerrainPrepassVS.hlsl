#include "GGTerrainConstants.hlsli"

cbuffer CameraCB : register( b1 )
{
	float4x4	g_xCamera_VP;			// View*Projection
};

struct VertexIn
{
	float3 position : POSITION;
	float4 inormal : INORMAL; // normalized float from 8 bit integers, like vertex colors
	uint id : ID; 
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float2 uvPos : TEXCOORD1;
	uint lodLevel : TEXCOORD2;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;
 
	float4 pos = float4( IN.position.xyz, 1.0 );
	OUT.position = mul( g_xCamera_VP, pos );
	OUT.uvPos = IN.position.xz;
	OUT.lodLevel = max( IN.id >> 16, terrain_detailLimit );

	//OUT.uv = IN.position.xz / float2( 256, -256 ) + 0.5;
	//OUT.uv2 = IN.position.xz / float2( 131072, -131072 ) + 0.5;

	// use most detailed LOD for uv level of detail check
	OUT.uv = IN.position.xz - float2( terrain_LOD[ 0 ].x, terrain_LOD[ 0 ].z );
	OUT.uv *= terrain_LOD[ 0 ].size;
	OUT.uv.y = 1 - OUT.uv.y;
	OUT.uv *= 128; // page size in pixels
	OUT.uv *= terrain_detailScale;
    
	return OUT;
}
