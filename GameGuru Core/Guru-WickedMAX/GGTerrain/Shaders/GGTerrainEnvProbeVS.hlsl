#include "GGTerrainConstants.hlsli"
#include "../GGTerrainPageSettings.h"

#include "PBR/ShaderInterop_Renderer.h"

struct VertexIn
{
	float3 position : POSITION;
	float4 inormal : INORMAL; // normalized float from 8 bit integers, like vertex colors
	uint id : ID; // [0-7]=segX, [8-15]=segZ, [16-23]=lodLevel
	uint instanceData : INSTANCEDATA;
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD1;
	float lodLevel: TEXCOORD2;
	float3 normal : TEXCOORD3;
	uint RenderTargetIndex : SV_RenderTargetArrayIndex;
	float2 uv : TEXCOORD4;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;

	uint cubeFaceID = IN.instanceData & 0x7;
 
	float4 pos = float4( IN.position.xyz, 1.0 );
	OUT.position = mul( xCubemapRenderCams[cubeFaceID].VP, pos );
	OUT.worldPos = IN.position.xyz;
	OUT.normal = IN.inormal.rgb * 2 - 1;
	
	float fX = (float) (IN.id & 0xFF);
	float fZ = (float) ((IN.id >> 8) & 0xFF);
	OUT.lodLevel = max( IN.id >> 16, terrain_detailLimit );

	//OUT.uv = IN.position.xz / float2( 131072, -131072 ) + 0.5;
	//OUT.uv2 = IN.position.xz / float2( 256, -256 ) + 0.5;

	// use most detailed LOD for uv level of detail check
	OUT.uv = IN.position.xz - float2( terrain_LOD[ 0 ].x, terrain_LOD[ 0 ].z );
	OUT.uv *= terrain_LOD[ 0 ].size;
	OUT.uv.y = 1 - OUT.uv.y;
	OUT.uv *= 128; // page size in pixels
	OUT.uv *= terrain_detailScale;

	OUT.RenderTargetIndex = cubeFaceID;
		
    return OUT;
}

