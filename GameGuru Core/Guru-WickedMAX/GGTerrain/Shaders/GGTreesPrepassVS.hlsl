/*
cbuffer CameraCB : register( b1 )
{
	float4x4	g_xCamera_VP;			// View*Projection
	float4		g_xCamera_ClipPlane;
	float3		g_xCamera_CamPos;
};
*/

#include "PBR/globals.hlsli"
#include "GGTreesConstants.hlsli"

struct VertexIn
{
	float2 position : POSITION;
	float3 offset: OFFSET;
	uint data : DATA;
	//uint instanceID : SV_InstanceID;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD0;
	float  clip : SV_ClipDistance0;
	float2 uv : TEXCOORD1;
	uint data : TEXCOORD2;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;

	uint treeType = GetTreeType( IN.data );
	uint index = GetTreeVariation( IN.data );
 
	float2 posOrig = IN.position.xy * GetTreeScale( IN.data );
	posOrig.x *= tree_type[ treeType ].scaleX;
	posOrig.y *= tree_type[ treeType ].scaleY;

	float4 pos = float4( posOrig, 0, 1 );
	
	float2 diff = IN.offset.xz - g_xCamera_CamPos.xz;
	float invV = rsqrt( diff.x*diff.x + diff.y*diff.y ); // approximation
	diff *= invV;
	float posX = pos.x * diff.y + pos.z * diff.x;
	float posZ = pos.z * diff.y - pos.x * diff.x;

	pos.x = posX;
	pos.z = posZ;
	 
	pos.xyz += IN.offset;

	OUT.position = mul( g_xCamera_VP, pos );
	OUT.worldPos = pos.xyz;
	OUT.clip = dot( pos, g_xCamera_ClipPlane );
	OUT.uv.x = IN.position.x + 0.5;
	OUT.uv.y = 1 - IN.position.y;
	OUT.data = IN.data;
    
	return OUT;
}
