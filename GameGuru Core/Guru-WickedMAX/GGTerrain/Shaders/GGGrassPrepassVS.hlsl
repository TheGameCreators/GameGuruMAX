/*cbuffer CameraCB : register( b1 )
{
	float4x4	g_xCamera_VP;			// View*Projection
	float4		g_xCamera_ClipPlane;
	float3		g_xCamera_CamPos;
};*/

#include "PBR/globals.hlsli"

#include "GGGrassConstants.hlsli"

struct VertexIn
{
	float2 position : POSITION;
	float3 offset: OFFSET;
	uint data : DATA;
	uint instanceID : SV_InstanceID;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD1;
	float2 uvNoise : TEXCOORD4;
	float3 worldPos : TEXCOORD3;
	uint data : TEXCOORD2;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;

	uint grassType = GetGrassType( IN.data );
	uint index = GetGrassVariation( IN.data );
 
	float3 posOrig = float3(IN.position, 0) * GGGRASS_SCALE;
	float scaleFactor = grass_type[ grassType ].scaleFactor;
	posOrig.x *= scaleFactor;

	float randScale = (IN.instanceID & 0x0F) * 0.02 + 0.8;
	posOrig.y *= randScale;
	
	float2x2 rotMat = { grass_rotMat[ index ].x, grass_rotMat[ index ].y, grass_rotMat[ index ].z, grass_rotMat[ index ].w };

	float4 pos;
	pos.xz = posOrig.xz;

	float wave = grass_type[ index ].cosTime;
	pos.x += wave * IN.position.y * 2;

	pos.xz = mul( rotMat, pos.xz );
	pos.y = posOrig.y;
	pos.w = 1;
	
	pos.xyz += IN.offset;
	/*
	float offset = (IN.instanceID & 0x7F) * 16;
	float dist = length( g_xCamera_CamPos - pos.xyz );
	dist = 1 - saturate( (dist - grass_lodDist + offset) / GGGRASS_LOD_TRANSITION );
	pos.y = (pos.y - IN.offset.y) * dist + IN.offset.y;
	*/
	OUT.worldPos = pos.xyz;
	OUT.position = mul( g_xCamera_VP, pos );
	OUT.uv.x = IN.position.x + 0.5;
	OUT.uv.y = 1 - IN.position.y;
	OUT.uvNoise = OUT.uv + (IN.instanceID & 0xFF) / 256.0;
	OUT.data = IN.data;
    
	return OUT;
}

