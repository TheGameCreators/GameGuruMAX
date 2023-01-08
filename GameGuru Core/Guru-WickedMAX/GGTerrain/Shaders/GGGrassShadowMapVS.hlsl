cbuffer CameraCB : register( b1 )
{
	float4x4	g_xCamera_VP;			// View*Projection
};

#include "GGGrassConstants.hlsli"

struct VertexIn
{
	float3 position : POSITION;
	float2 uv : UV;
	float3 offset: OFFSET;
	uint data : DATA;
	//uint instanceID : ID;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD0;
	uint data : TEXCOORD2;
	float2 uv : TEXCOORD1;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;

	uint grassType = GetGrassType( IN.data );
	uint index = GetGrassVariation( IN.data );

	float3 posOrig = IN.position * GGGRASS_SCALE;
	float scaleFactor = grass_type[ grassType ].scaleFactor;
	posOrig.x *= scaleFactor;
 
	float4 pos = float4( posOrig, 1.0 );
	pos.xyz = pos.xyz + IN.offset;
	OUT.worldPos = pos.xyz;
	OUT.position = mul( g_xCamera_VP, pos );
	OUT.uv = IN.uv;
	OUT.data = IN.data;
    
	return OUT;
}

