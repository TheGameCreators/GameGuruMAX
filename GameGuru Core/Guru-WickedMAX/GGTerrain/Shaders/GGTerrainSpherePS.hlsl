
#include "GGTerrainConstants.hlsli"

#include "../GGTerrainPageSettings.h"

#include "PBR/brdf.hlsli"
#include "PBR/lightingHF.hlsli"

#include "GGCommonFunctions.hlsli"

Texture2DArray<float4> texColor  : register( t50 );
Texture2DArray<float2> texNormal : register( t51 );
Texture2DArray<float4> texSurface : register( t52 ); // R: occlusion, G: roughnes, B: metalness

SamplerState samplerTriWrap : register( s0 );

STRUCTUREDBUFFER(EntityTiles, uint, TEXSLOT_RENDERPATH_ENTITYTILES);

struct PixelIn
{
    float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD1;
	float3 normal : TEXCOORD3;
	uint instanceID : TEXCOORD2;
	float2 uv : TEXCOORD4;
};

struct GBuffer
{
	float4 g0 : SV_TARGET0;	/*FORMAT_R11G11B10_FLOAT*/
	float4 g1 : SV_TARGET1;	/*FORMAT_R8G8B8A8_FLOAT*/
};

inline void TiledLighting(inout Surface surface, inout Lighting lighting, float dist)
{
	const uint2 tileIndex = uint2(floor(surface.pixel / TILED_CULLING_BLOCKSIZE));
	const uint flatTileIndex = flatten2D(tileIndex, g_xFrame_EntityCullingTileCount.xy) * SHADER_ENTITY_TILE_BUCKET_COUNT;

#ifndef DISABLE_ENVMAPS
	// Apply environment maps:
	float4 envmapAccumulation = 0;

#ifndef DISABLE_LOCALENVPMAPS
	[branch]
	if (g_xFrame_EnvProbeArrayCount > 0)
	{
		// Loop through envmap buckets in the tile:
		const uint first_item = g_xFrame_EnvProbeArrayOffset;
		const uint last_item = first_item + g_xFrame_EnvProbeArrayCount - 1;
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

				[branch]
				if (entity_index >= first_item && entity_index <= last_item && envmapAccumulation.a < 1)
				{
					ShaderEntity probe = EntityArray[entity_index];

					const float4x4 probeProjection = MatrixArray[probe.GetMatrixIndex()];
					const float3 clipSpacePos = mul(probeProjection, float4(surface.P, 1)).xyz;
					const float3 uvw = clipSpacePos.xyz * float3(0.5, -0.5, 0.5) + 0.5;
					[branch]
					if (is_saturated(uvw))
					{
						const float4 envmapColor = EnvironmentReflection_Local(surface, probe, probeProjection, clipSpacePos);
						// perform manual blending of probes:
						//  NOTE: they are sorted top-to-bottom, but blending is performed bottom-to-top
						envmapAccumulation.rgb = (1 - envmapAccumulation.a) * (envmapColor.a * envmapColor.rgb) + envmapAccumulation.rgb;
						envmapAccumulation.a = envmapColor.a + (1 - envmapColor.a) * envmapAccumulation.a;
						[branch]
						if (envmapAccumulation.a >= 1.0)
						{
							// force exit:
							bucket = SHADER_ENTITY_TILE_BUCKET_COUNT;
							break;
						}
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
#endif // DISABLE_LOCALENVPMAPS
	
	// Apply global envmap where there is no local envmap information:
	[branch]
	if (envmapAccumulation.a < 0.99)
	{
		envmapAccumulation.rgb = lerp(EnvironmentReflection_Global(surface), envmapAccumulation.rgb, envmapAccumulation.a);
	}
	lighting.indirect.specular += max(0, envmapAccumulation.rgb);
#endif // DISABLE_ENVMAPS

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
						DirectionalLight(light, surface, lighting, 1);
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
	color.rgb = lerp(surface.albedo * combined_lighting.diffuse, surface.refraction.rgb, surface.refraction.a) + combined_lighting.specular;
}

GBuffer main( PixelIn IN )
{
	GBuffer output;

	uint indexX = IN.instanceID % 11;
	uint indexZ = IN.instanceID / 11;

	//float3 baseColor = float3( 0.6, 0.6, 0.6 );
	float3 baseColor = float3( 0.97, 0.74, 0.62 );  // copper
	//float3 baseColor = float3( 1.0, 0.85, 0.57 );  // gold
	float3 normal = float3( 0, 0, 1 );
	float3 surfaceTex = float3( 1, indexX * 0.1, indexZ ); // ao, roughness, metalness

	if ( IN.instanceID == 22 )
	{
		float matIndex = terrain_baseLayerMaterial & 0xFF;
		baseColor = texColor.Sample( samplerTriWrap, float3(IN.uv, matIndex) ).rgb;
		normal.rg = texNormal.Sample( samplerTriWrap, float3(IN.uv, matIndex) ).rg;
		surfaceTex = texSurface.Sample( samplerTriWrap, float3(IN.uv, matIndex) ).rgb;

		normal.rg = normal.rg * 2 - 1;
		normal.b = 1 - (normal.r*normal.r + normal.g*normal.g);
		if ( normal.b > 0 ) normal.b = sqrt( normal.b );
	}

	if ( indexZ == 0 ) baseColor *= 0.65;

	// calculate TBN matrix
	float3 vNormal = normalize( IN.normal );
	float3 tangent = cross( vNormal, float3(0,1,0) );
	if ( dot(tangent,tangent) > 0 ) tangent = normalize(tangent);
	else tangent = float3(1,0,0);
	float3 binormal = normalize( cross( vNormal, tangent ) );
	float3x3 TBN = float3x3( tangent, binormal, vNormal );

	// transform normal
	normal = mul( normal, TBN );
	normal = lerp( IN.normal, normal, terrain_bumpiness );
	normal = normalize( normal );

	//normal = vNormal;

	// WickedEngine PBR
	Surface surface;

	surface.N = normal;
	surface.P = IN.worldPos;
	surface.V = g_xCamera_CamPos - surface.P;
	float dist = length( surface.V );
	surface.V /= dist;

	// de-gamma is now done automatically by hardware due to sRGB texture
	//baseColor = pow( baseColor, terrain_textureGamma );
		
	float metalness = surfaceTex.b;
	float roughness = surfaceTex.g;
	float occlusion = surfaceTex.r;

	surface.createMetalness( metalness, roughness, occlusion, terrain_reflectance, baseColor, false );
	
	const float2 pixel = IN.position.xy;
	const float2 ScreenCoord = pixel * g_xFrame_InternalResolution_rcp;
	surface.pixel = pixel;
	surface.screenUV = ScreenCoord;
	
	surface.update();

	float3 ambient = GetAmbient(surface.N);
	//ambient = lerp(ambient, ambient * surface.sss.rgb, saturate(surface.sss.a));
	

	Lighting lighting;
	lighting.create(0, 0, ambient, 0);
	
	//ForwardLighting(surface, lighting);
	TiledLighting(surface, lighting, dist);

	float4 color = float4(0,0,0,0);
	ApplyLighting(surface, lighting, color);

	if ( (terrain_flags & GGTERRAIN_SHADER_FLAG2_USE_FOG) ) 
	{
		color.rgb = ApplyFogCustom( IN.worldPos, dist, color.rgb, surface.V );
	}
			
	color = max( 0, color );
	output.g0 = float4( color.rgb, 1 );
	output.g1 = float4( surface.N * 0.5f + 0.5f, surface.roughness ); // RGB=normal, A=roughness
	return output;
}