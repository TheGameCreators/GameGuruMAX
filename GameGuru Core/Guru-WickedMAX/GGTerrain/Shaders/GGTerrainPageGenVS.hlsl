#include "GGTerrainConstants.hlsli"

struct VertexIn
{
    float2 position : POSITION;
	float2 uv : UV;
	float2 uv2 : HEIGHTUV;
	float2 worldPos : WORLDPOS;
	uint chunkID : CHUNKID; 
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float2 uv2 : TEXCOORD1;
	float2 uv3 : TEXCOORD2;
	float2 uvMat : TEXCOORD3;
	uint lodLevel : TEXCOORD4;
};

VertexOut main( VertexIn IN )
{
    VertexOut OUT;
 
	float2 pos;
	pos.x = (IN.position.x * 2) - 1;
	pos.y = 1 - (2 * IN.position.y);

	OUT.position = float4( pos, 0.5, 1.0 );
	OUT.uv = IN.uv;
	OUT.uv2 = IN.uv2;
	//OUT.uv3 = IN.worldPos * terrain_maskScale + 0.5;
	OUT.uv3 = IN.uv * 390 * terrain_maskScale;
	OUT.lodLevel = IN.chunkID;

	OUT.uvMat = IN.worldPos / terrain_mapEditSize;
	OUT.uvMat = OUT.uvMat * 0.5 + 0.5;
	
    return OUT;
}

