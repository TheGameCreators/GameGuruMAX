cbuffer CameraCB : register( b1 )
{
	float4x4	g_xCamera_VP;			// View*Projection
	float4		g_xCamera_ClipPlane;
	float3		g_xCamera_CamPos;
};

#include "GGTreesConstants.hlsli"

struct VertexIn
{
	float2 position : POSITION;
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

	uint treeType = GetTreeType( IN.data );
	uint index = GetTreeVariation( IN.data );

	float2 posOrig = IN.position.xy * GetTreeScale( IN.data );
	posOrig.x *= tree_type[ treeType ].scaleX;
	posOrig.y *= tree_type[ treeType ].scaleY;

	float4 pos = float4( posOrig, 0, 1 );

	float2x2 rotMat = { tree_rotMatShadow.x, tree_rotMatShadow.y, tree_rotMatShadow.z, tree_rotMatShadow.w };

	pos.xz = mul( rotMat, pos.xz );

	pos.xyz = pos.xyz + IN.offset;
	OUT.worldPos = pos.xyz;
	OUT.position = mul( g_xCamera_VP, pos );
	OUT.uv.x = IN.position.x + 0.5;
	OUT.uv.y = 1 - IN.position.y;
	OUT.data = IN.data;
    
	return OUT;
}

