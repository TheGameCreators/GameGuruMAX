Texture2DArray<float> texLODHeight : register( t0 );
Texture2DArray<float4> texLODNormals : register( t1 );
Texture2DArray<float4> texColor  : register( t2 );

SamplerState sampler0 : register( s0 );

#include "PBR/globals.hlsli"
#include "GGTerrainConstants.hlsli"

struct PixelIn
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

float4 main( PixelIn IN ) : SV_TARGET
{
	uint layer = terrain_numLODLevels - 2;

	float height = texLODHeight.Sample( sampler0, float3(IN.uv, layer) );
	float3 normal = texLODNormals.Sample( sampler0, float3(IN.uv, layer) ).rgb;

	//float3 color = float3( 0, 0.5, 0 );
	//if ( height > 13000 ) color = float3( 0.9, 0.9, 0.9 );
	//if ( height < 100 ) color = float3( 0.5, 0.5, 0 );

	uint material = terrain_baseLayerMaterial & 0xFF;
	float3 colorUV = float3( 0.5, 0.5, material );
	float3 color = texColor.SampleLevel( sampler0, colorUV, 11 ).rgb;
	
	int i;
	for( i = 0; i < 5; i++ )
	{
		colorUV.z = terrain_layers[i].material & 0xFF;
		float t = clamp( (height-terrain_layers[i].start)*terrain_layers[i].transition, 0.0, 1.0 );
		if ( t >= 1 ) color = texColor.SampleLevel( sampler0, colorUV, 11 ).rgb;
		else if ( t > 0 )
		{
			float3 layerColor = texColor.SampleLevel( sampler0, colorUV, 11 ).rgb;
			color = lerp( color, layerColor, t );
		}
	}

	float t = clamp( (g_xFrame_WaterHeight - height)*0.1, 0.0, 1.0 );
	color = lerp( color, g_xFrame_WaterColor, t );

	// calculate light shading
	float3 lightDir = float3( 0.70356, 0.1, 0.70356 );
	float light = dot( lightDir, normal );
	light = clamp( light, 0, 1 );
	color *= light;
	color = pow( color, 0.3 );
	/*
	// draw edit box
	float3 editColor = float3( 244.0/255.0, 239.0/255.0, 38.0/255.0 );
	float2 worldPos = IN.uv;
	worldPos.y = 1 - worldPos.y;
	worldPos /= terrain_LOD[ layer ].size;
	worldPos += float2( terrain_LOD[ layer ].x, terrain_LOD[ layer ].z );
	float editX = (worldPos.x / terrain_mapEditSize) * 0.5 + 0.5;
	float editZ = (worldPos.y / terrain_mapEditSize) * 0.5 + 0.5;
		
	if ( editX >= 0 && editX <= 1 && editZ >= 0 && editZ <= 1 )
	{
		float border = 8;
		float pixelSize = g_xFrame_InternalResolution_rcp.x * border;
		//pixelSize /= 0.15;
		pixelSize /= terrain_LOD[ layer ].size;
		pixelSize /= terrain_mapEditSize;
		

		float fade = 1;
		if ( editX > pixelSize && editX < 1-pixelSize && editZ > pixelSize && editZ < 1-pixelSize ) fade = 0;
		color.rgb = lerp( color.rgb, editColor, fade );
	}
	*/
	return float4( color, 1 );
}