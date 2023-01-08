Texture2DArray texTree : register( t50 );
Texture2D<float> texNoise : register( t51 );
Texture2DArray texTreeNormal : register( t53 );

SamplerState samplerBilinearWrap : register( s0 );
SamplerState samplerTrilinearClamp : register( s1 );

#include "PBR/brdf.hlsli"
#include "PBR/lightingHF.hlsli"

#include "GGTreesConstants.hlsli"

#include "GGCommonFunctions.hlsli"

STRUCTUREDBUFFER(EntityTiles, uint, TEXSLOT_RENDERPATH_ENTITYTILES);

struct PixelIn
{
    float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD0;
	float  clip : SV_ClipDistance0;
	float2 uv : TEXCOORD1;
	float2 dir : TEXCOORD4;
	uint data : TEXCOORD2;
	bool isFront : SV_IsFrontFace;
};

struct GBuffer
{
	float4 g0 : SV_TARGET0;	/*FORMAT_R11G11B10_FLOAT*/
	float4 g1 : SV_TARGET1;	/*FORMAT_R8G8B8A8_FLOAT*/
};

inline void TiledLighting(inout Surface surface, inout Lighting lighting)
{
	const uint2 tileIndex = uint2(floor(surface.pixel / TILED_CULLING_BLOCKSIZE));
	const uint flatTileIndex = flatten2D(tileIndex, g_xFrame_EntityCullingTileCount.xy) * SHADER_ENTITY_TILE_BUCKET_COUNT;

	[branch]
	if (g_xFrame_LightArrayCount > 0)
	{
		// Loop through light buckets in the tile:
		const uint first_item = g_xFrame_LightArrayOffset;
		const uint last_item = first_item + g_xFrame_LightArrayCount - 1;
		const uint first_bucket = first_item / 32;
		const uint last_bucket = min(last_item / 32, max(0, SHADER_ENTITY_TILE_BUCKET_COUNT - 1));
		[loop]
		for (uint bucket = first_bucket; bucket <= last_bucket; ++bucket)
		{
			uint bucket_bits = EntityTiles[flatTileIndex + bucket];

			// Bucket scalarizer - Siggraph 2017 - Improved Culling [Michal Drobot]:
			bucket_bits = WaveReadLaneFirst(WaveActiveBitOr(bucket_bits));

			[loop]
			while (bucket_bits != 0)
			{
				// Retrieve global entity index from local bucket, then remove bit from local bucket:
				const uint bucket_bit_index = firstbitlow(bucket_bits);
				const uint entity_index = bucket * 32 + bucket_bit_index;
				bucket_bits ^= 1 << bucket_bit_index;

				// Check if it is a light and process:
				[branch]
				if (entity_index >= first_item && entity_index <= last_item)
				{
					ShaderEntity light = EntityArray[entity_index];

					if (light.GetFlags() & ENTITY_FLAG_LIGHT_STATIC)
					{
						continue; // static lights will be skipped (they are used in lightmap baking)
					}

					switch (light.GetType())
					{
					case ENTITY_TYPE_DIRECTIONALLIGHT:
					{
						DirectionalLight(light, surface, lighting, 1, true);
					}
					break;
					case ENTITY_TYPE_POINTLIGHT:
					{
						PointLight(light, surface, lighting);
					}
					break;
					case ENTITY_TYPE_SPOTLIGHT:
					{
						SpotLight(light, surface, lighting);
					}
					break;
					}
				}
				else if (entity_index > last_item)
				{
					// force exit:
					bucket = SHADER_ENTITY_TILE_BUCKET_COUNT;
					break;
				}

			}
		}
	}
}

inline void ApplyLighting(in Surface surface, in Lighting lighting, inout float4 color)
{
	LightingPart combined_lighting = CombineLighting(surface, lighting);
	//combined_lighting.diffuse = pow( combined_lighting.diffuse, 0.8 );
	//combined_lighting.diffuse *= 0.7;
	//color.rgb = lerp(surface.albedo * combined_lighting.diffuse, surface.refraction.rgb, surface.refraction.a) + combined_lighting.specular;
	color.rgb = surface.albedo * combined_lighting.diffuse + combined_lighting.specular;
}

GBuffer main( PixelIn IN )
{
	uint treeType = GetTreeType( IN.data );
	uint index = GetTreeVariation( IN.data );

	float4 baseColor = texTree.Sample( samplerTrilinearClamp, float3(IN.uv, treeType) );
	float alpha = baseColor.a;
	if ( alpha < 0.3 ) discard;
	
	//baseColor.rgb /= baseColor.a;
	alpha = (alpha - 0.3) / max(fwidth(alpha),0.0001) + 0.5;
	
	GBuffer output;
	
	/*
	output.g0 = float4( 0, 0, 0, 1 );
	if ( IN.isFront ) output.g0.r = 1;
	else output.g0.g = 1;
	output.g1 = float4( 0, 1, 0, 1 ); // RGB=normal, A=roughness
	return output;
	*/

	Surface surface;
	surface.P = IN.worldPos;
	surface.V = g_xCamera_CamPos - surface.P;
	float sqrDist = dot( surface.V, surface.V );

	if ( !any(g_xCamera_ClipPlane) )
	{
		float noise = texNoise.Sample( samplerBilinearWrap, IN.uv*3 );
		float limit = noise * GGTREES_LOD_TRANSITION + tree_lodDist;
		if( sqrDist < limit*limit ) discard;
	}

	float dist = sqrt( sqrDist );
	surface.V /= dist;

	float height = 1 - IN.uv.y;

	float sinAng = IN.dir.x;
	float cosAng = IN.dir.y;

	//float3 normal = float3( 0, 0, 1 );
	//normal.x = -sinAng * 0.7071068;
	//normal.y = 0.7071068;
	//normal.z = cosAng * 0.7071068;

	float3 normal = texTreeNormal.Sample( samplerTrilinearClamp, float3(IN.uv, treeType) ).rgb;
	normal = normal * 2 - 1;
	normal.y = abs(normal.y);

	float normX = normal.x * cosAng - normal.z * sinAng;
	float normZ = -normal.x * sinAng - normal.z * cosAng;
	normal.x = normX;
	normal.z = normZ;

	float3 dir = float3( -sinAng, 0, -cosAng );
	normal = lerp( dir, normal, 2 );
	
	/*
	output.g0 = float4( normal*0.5 + 0.5, 1 );
	output.g1 = float4( 0, 1, 0, 1 ); // RGB=normal, A=roughness
	return output;
	*/

	// WickedEngine PBR

	surface.N = normal;

	// de-gamma is now done automatically by hardware due to sRGB texture
	//baseColor.rgb = pow( baseColor.rgb, 2.2 ); // de-gamma

	surface.createMetalness( 0, 1, 1, GGTREES_REFLECTANCE, baseColor.rgb, true );
	
	const float2 pixel = IN.position.xy;
	const float2 ScreenCoord = pixel * g_xFrame_InternalResolution_rcp;
	surface.pixel = pixel;
	surface.screenUV = ScreenCoord;

	surface.update();
	
	float3 ambient = GetAmbient(surface.N);
	
	Lighting lighting;
	lighting.create(0, 0, ambient, 0);
	
	//ForwardLighting(surface, lighting);
	TiledLighting(surface, lighting);

	float4 color = float4(0,0,0,0);
	ApplyLighting(surface, lighting, color);

	//color.rgb *= height * 0.5 + 0.3;

	//ApplyFog(dist, color);
	color.rgb = ApplyFogCustom( IN.worldPos, dist, color.rgb, surface.V );

	color = max( 0, color );

	if ( GetTreeHighlighted(IN.data) ) color.rgb = lerp( color.rgb, float3(178.0/255.0, 148.0/255.0, 45.0/255.0), 0.3 );
	
	output.g0 = float4( color.rgb, alpha );
	output.g1 = float4( surface.N * 0.5f + 0.5f, surface.roughness ); // RGB=normal, A=roughness
	return output;
}