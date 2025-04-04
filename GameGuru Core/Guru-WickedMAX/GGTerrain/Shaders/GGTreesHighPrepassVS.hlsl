/*
cbuffer CameraCB : register( b1 )
{
	float4x4	g_xCamera_VP;			// View*Projection
	float4		g_xCamera_ClipPlane;
};
*/

#include "PBR/globals.hlsli"
#include "GGTreesConstants.hlsli"

struct VertexIn
{
	float3 position : POSITION;
	float4 normal : INORMAL;
	float2 uv : UV;
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

    //uint treeType = GetTreeType(IN.data);
	const uint index = GetTreeVariation( IN.data );

	float2x2 rotMat = { tree_rotMat[ index ].x, tree_rotMat[ index ].y, tree_rotMat[ index ].z, tree_rotMat[ index ].w };

	float4 pos;
	pos.xz = mul( rotMat, IN.position.xz );
	pos.y = IN.position.y;
	pos.w = 1;
	
	pos.xyz *= GetTreeScale( IN.data );
	pos.xyz += IN.offset;

    pos.x += TreeWaveX(IN.position.y, IN.offset.x + IN.offset.z);
    pos.z += TreeWaveZ(IN.position.y, IN.offset.z);
	
	OUT.worldPos = pos.xyz;
	OUT.position = mul( g_xCamera_VP, pos );
	OUT.clip = dot( pos, g_xCamera_ClipPlane );
	OUT.uv = IN.uv;
	OUT.data = IN.data;
    
	return OUT;
}
