#include "PBR/ShaderInterop_Renderer.h"
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
	float3 normal : TEXCOORD1;
	uint RenderTargetIndex : SV_RenderTargetArrayIndex;
	float2 uv : TEXCOORD2;
    uint data : TEXCOORD4;
	float3 origPos : TEXCOORD3;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;

	//uint treeType = GetTreeType( IN.data );
	const uint index = GetTreeVariation( IN.data );
	const uint cubeFaceID = IN.data & 0xFF;

	OUT.data = IN.data;

	float2x2 rotMat = { tree_rotMat[ index ].x, tree_rotMat[ index ].y, tree_rotMat[ index ].z, tree_rotMat[ index ].w };

	float4 pos;
	pos.xz = mul( rotMat, IN.position.xz );
	pos.y = IN.position.y;
	pos.w = 1;
	 
	pos.xyz *= GetTreeScale( IN.data );
	pos.xyz += IN.offset;

    pos.x += TreeWaveX(IN.position.y, IN.offset.x + IN.offset.z);
    pos.z += TreeWaveZ(IN.position.y, IN.offset.z);
	
	float3 normal = IN.normal.xyz * 2 - 1;

	OUT.worldPos = pos.xyz;
	OUT.normal.xz = mul( rotMat, normal.xz );
	OUT.normal.y = normal.y;
	OUT.position = mul( xCubemapRenderCams[cubeFaceID].VP, pos );
	OUT.origPos = IN.position.xyz;
	OUT.uv = IN.uv;
	OUT.RenderTargetIndex = cubeFaceID;
	
    return OUT;
}
