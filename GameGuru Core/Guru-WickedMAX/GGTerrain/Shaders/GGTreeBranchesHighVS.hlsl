
cbuffer CameraCB : register( b1 )
{
	float4x4	g_xCamera_VP;			// View*Projection
	float4		g_xCamera_ClipPlane;
};

#include "GGTreesConstants.hlsli"

struct VertexIn
{
	float3 position : POSITION;
	float4 normal : INORMAL; // packed into a UINT like a color
	float2 uv : UV;
	float3 offset: OFFSET;
	uint data : DATA;
	//uint instanceID : SV_InstanceID;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float  clip : SV_ClipDistance0;
	float2 uv : TEXCOORD2;
	uint data : TEXCORRD4;
	float3 origPos : TEXCOORD3;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;

	uint treeType = GetTreeType( IN.data );
	uint index = GetTreeVariation( IN.data );

	OUT.data = IN.data;

	float2x2 rotMat = { tree_rotMat[ index ].x, tree_rotMat[ index ].y, tree_rotMat[ index ].z, tree_rotMat[ index ].w };

	float4 pos;
	pos.xz = mul( rotMat, IN.position.xz );
	pos.y = IN.position.y;
	pos.w = 1;
	 
	pos.xyz *= GetTreeScale( IN.data );
	pos.xyz += IN.offset;

	float3 normal = IN.normal.xyz * 2 - 1;

	OUT.worldPos = pos.xyz;
	OUT.normal.xz = mul( rotMat, normal.xz );
	OUT.normal.y = normal.y;
	OUT.position = mul( g_xCamera_VP, pos );
	OUT.clip = dot( pos, g_xCamera_ClipPlane );
	OUT.origPos = IN.position.xyz;
	OUT.uv = IN.uv;
		
    return OUT;
}

