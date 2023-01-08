Texture2D texColorAndMetalness   : register( t50 );
Texture2D texNormalRoughnessAO   : register( t51 );
							     
Texture2DArray<float> texPageTableArray : register( t53 );
Texture2D<float> texPageTableFinal      : register( t54 );

SamplerState sampler1            : register( s1 );

#include "GGTerrainConstants.hlsli"

#include "../GGTerrainPageSettings.h"

#include "PBR/brdf.hlsli"
#include "PBR/lightingHF.hlsli"

#include "GGCommonFunctions.hlsli"

STRUCTUREDBUFFER(EntityTiles, uint, TEXSLOT_RENDERPATH_ENTITYTILES);

static const float4 mipColors[16] = { 
	float4( 1.0, 0.0, 0.0, 1.0 ),
	float4( 0.0, 1.0, 0.0, 1.0 ),
	float4( 0.0, 0.0, 1.0, 1.0 ),
	float4( 1.0, 1.0, 0.0, 1.0 ),
	float4( 0.0, 1.0, 1.0, 1.0 ),
	float4( 1.0, 0.0, 1.0, 1.0 ),
	float4( 1.0, 1.0, 1.0, 1.0 ),
	float4( 0.5, 0.0, 0.0, 1.0 ),
	float4( 0.0, 0.5, 0.0, 1.0 ),
	float4( 0.0, 0.0, 0.5, 1.0 ),
	float4( 0.5, 0.5, 0.0, 1.0 ),
	float4( 0.0, 0.5, 0.5, 1.0 ),
	float4( 0.5, 0.0, 0.5, 1.0 ),
	float4( 0.5, 0.5, 0.5, 1.0 ),
	float4( 0.25, 0.25, 0.25, 1.0 ),
	float4( 0.0, 0.0, 0.0, 1.0 ),
};

struct PixelIn
{
    float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD1;
	float lodLevel: TEXCOORD2;
	float3 normal : TEXCOORD3;
	float clip : SV_ClipDistance0;
	float2 uv : TEXCOORD4;
};

struct GBuffer
{
	float4 g0 : SV_TARGET0;	/*FORMAT_R11G11B10_FLOAT*/
	float4 g1 : SV_TARGET1;	/*FORMAT_R8G8B8A8_FLOAT*/
};

inline void TiledLighting(inout Surface surface, inout Lighting lighting, float dist, out float3 envmapAmbient)
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
						float3 ambient = texture_envmaparray.SampleLevel(sampler_linear_clamp, float4(surface.N, probe.GetTextureIndex()), g_xFrame_EnvProbeMipCount-1).rgb;
						const float4 envmapColor = EnvironmentReflection_Local(surface, probe, probeProjection, clipSpacePos);
						// perform manual blending of probes:
						//  NOTE: they are sorted top-to-bottom, but blending is performed bottom-to-top
						envmapAccumulation.rgb = (1 - envmapAccumulation.a) * (envmapColor.a * envmapColor.rgb) + envmapAccumulation.rgb;
						envmapAmbient.rgb = (1 - envmapAccumulation.a) * (envmapColor.a * ambient.rgb) + envmapAmbient.rgb;
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
		float3 ambient = texture_envmaparray.SampleLevel(sampler_linear_clamp, float4(surface.N, g_xFrame_GlobalEnvProbeIndex), g_xFrame_EnvProbeMipCount-1).rgb;
		envmapAmbient = lerp( ambient, envmapAmbient, envmapAccumulation.a );
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
	color.rgb = surface.albedo * combined_lighting.diffuse + combined_lighting.specular;
}

// virtual texture variables
static const float2 virtToPageSize = float2( pageSize / physTexSizeX, pageSize / physTexSizeY );
static const float2 texelOffset = float2( pagePaddingLeft / physTexSizeX, pagePaddingLeft / physTexSizeY );

GBuffer main( PixelIn IN )
{
	GBuffer output;
	
	// page table look up
	int maxLevel = terrain_numLODLevels - 1; 
	int detailLevel = texPageTableFinal.CalculateLevelOfDetailUnclamped( sampler1, IN.uv );
	detailLevel = max( detailLevel, IN.lodLevel );
	int origLOD = detailLevel;
	detailLevel = min( detailLevel, maxLevel + GGTERRAIN_MAX_PAGE_TABLE_MIP - 1 );
	float pageEntry = 0;
	float2 levelUV = float2(0,0);
	
	// go through the page table array until we find a suitable page
	while( pageEntry == 0 && detailLevel < maxLevel )
	{
		levelUV = IN.worldPos.xz - float2( terrain_LOD[ detailLevel ].x, terrain_LOD[ detailLevel ].z );
		levelUV *= terrain_LOD[ detailLevel ].size;
		levelUV.y = 1 - levelUV.y;
		
		int2 loadUV = levelUV * 256.0; // assumes texPageTableArray is 256 x 256 pixels
		pageEntry = texPageTableArray.Load( int4(loadUV, detailLevel, 0) ); // uv, arrayLevel, mipLevel
		
		detailLevel++;
	}

	uint mipLevel = max( detailLevel - maxLevel, 0 );

	// if still no page then go through the final page table mipmaps
	if ( pageEntry == 0 )
	{
		levelUV = IN.worldPos.xz - float2( terrain_LOD[ maxLevel ].x, terrain_LOD[ maxLevel ].z );
		levelUV *= terrain_LOD[ maxLevel ].size;
		levelUV.y = 1 - levelUV.y;

		uint mipSize = 256 >> mipLevel; // assumes texPageTableFinal is 256 x 256 pixels
		int2 loadUV = levelUV * mipSize; 
		
		while( pageEntry == 0 && mipLevel < GGTERRAIN_MAX_PAGE_TABLE_MIP ) // not a full mip stack
		{
			pageEntry = texPageTableFinal.Load( int3(loadUV, mipLevel) ); // uv, mipLevel
			loadUV >>= 1;
			mipLevel++;
			detailLevel++;
		}
		mipLevel--;
	}
	
	uint iPageEntry = (uint) (pageEntry * 65535);

	uint mipSize;
	if ( iPageEntry == 0 )
	{
		detailLevel = maxLevel + GGTERRAIN_MAX_PAGE_TABLE_MIP - 1;
		mipSize = 1;
	}
	else
	{
		detailLevel--;
		mipSize = 256 >> mipLevel;
	}
	
	// calculate UV for physical (page cache) texture
	uint iOffsetX = iPageEntry & 0xFF;
	uint iOffsetY = iPageEntry >> 8;
	float2 pageOffset = float2(iOffsetX / physPagesX, iOffsetY / physPagesY);

	float2 pageUV = frac( levelUV * mipSize ) * virtToPageSize + texelOffset;
	
	pageUV += pageOffset;

	float uvScale = 256.0 / (1 << detailLevel);
	float2 dx = ddx( IN.uv ) * (uvScale / physTexSizeX);
	float2 dy = ddy( IN.uv ) * (uvScale / physTexSizeY);

	// physical texture sample
	float4 colorMetalness = texColorAndMetalness.SampleGrad( sampler1, pageUV, dx, dy );
	float4 normalRoughnessAO = texNormalRoughnessAO.SampleGrad( sampler1, pageUV, dx, dy );
	
	/*
	output.g0 = float4( colorMetalness.rgb, 1 );
	output.g1 = float4( 0, 1, 0, 1 ); // RGB=normal, A=roughness
	return output;
	*/

	// expand normal from 2 channels to 3 channels
	float3 normal;
	normal.rg = normalRoughnessAO.rg * 2 - 1;
	normal.b = 1 - (normal.r*normal.r + normal.g*normal.g);
	if ( normal.b > 0 ) normal.b = sqrt( normal.b );

	// calculate TBN matrix
	float3 tangent = normalize( cross( IN.normal, float3(0,0,1) ) );
	float3 binormal = normalize( cross( IN.normal, tangent ) );
	float3x3 TBN = float3x3( tangent, binormal, IN.normal );

	// transform normal
	normal = mul( normal, TBN );
	normal = lerp( IN.normal, normal, terrain_bumpiness );
	normal = normalize( normal );

	// WickedEngine PBR
	Surface surface;

	surface.N = normal;
	surface.P = IN.worldPos;
	surface.V = g_xCamera_CamPos - surface.P;
	float dist = length( surface.V );
	surface.V /= dist;

	// de-gamma is now done automatically by hardware due to sRGB texture
	float3 baseColor = colorMetalness.rgb;
		
	float metalness = colorMetalness.a;
	float roughness = normalRoughnessAO.b;
	float occlusion = normalRoughnessAO.a;

	surface.createMetalness( metalness, roughness, occlusion, terrain_reflectance, baseColor, true );
	
	const float2 pixel = IN.position.xy;
	const float2 ScreenCoord = pixel * g_xFrame_InternalResolution_rcp;
	surface.pixel = pixel;
	surface.screenUV = ScreenCoord;
	
	surface.update();

	float3 ambient = GetAmbient(surface.N);
	//ambient = lerp(ambient, ambient * surface.sss.rgb, saturate(surface.sss.a));

	/*
	float3 ambient = g_xFrame_Ambient.rgb;
	if (g_xFrame_Options & OPTION_BIT_REALISTIC_SKY)
	{
		float3 sampleDir;
		sampleDir.x = 0;
		sampleDir.y = saturate( 1 - IN.normal.y ) * 0.7 + 0.15;
		sampleDir.z = sqrt( 1 - sampleDir.y*sampleDir.y );
		ambient += GetDynamicSkyColor( sampleDir, false, false, false, true )*3;
		
		//ambient += lerp(
		//	GetDynamicSkyColor(-surface.V, false, false, false, true),
		//	GetDynamicSkyColor(float3(0, 1, 0), false, false, false, true),
		//	max(surface.N.y * 0.5 + 0.5, 0) );
	}
	else
	{
		float3 sampleDir = -surface.V;
		sampleDir.y = abs(sampleDir.y);
		ambient += texture_globalenvmap.SampleLevel(sampler_linear_clamp, IN.normal, 8).rgb * 2;
	}
	*/

	

	Lighting lighting;
	lighting.create(0, 0, ambient, 0);
	
	//ForwardLighting(surface, lighting);
	float3 envAmbient = 0;
	TiledLighting(surface, lighting, dist, envAmbient);

	lighting.indirect.diffuse += envAmbient;

	float4 color = float4(0,0,0,0);
	ApplyLighting(surface, lighting, color);

	//ApplyFog(dist, color);
	if ( (terrain_flags & GGTERRAIN_SHADER_FLAG2_USE_FOG) ) 
	{
		color.rgb = ApplyFogCustom( IN.worldPos, dist, color.rgb, surface.V );
	}
	
	if ( terrain_flags & GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE )
	{
		float3 editColor = float3( 244.0/255.0, 239.0/255.0, 38.0/255.0 );
		float editX = (IN.worldPos.x / terrain_mapEditSize) * 0.5 + 0.5;
		float editZ = (IN.worldPos.z / terrain_mapEditSize) * 0.5 + 0.5;
		
		if ( editX >= 0 && editX <= 1 && editZ >= 0 && editZ <= 1 )
		{
			float fade = editX + editZ;
			fade = sin( fade * 150 + g_xFrame_Time*2 ) * 0.5 + 0.5;
			fade = step( fade, 0.5 );
			if ( editX > 0.004 && editX < 0.996 && editZ > 0.004 && editZ < 0.996 ) fade = 0;
			color.rgb = lerp( color.rgb, editColor, fade );
		}
	}

	if ( (terrain_flags & GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE) && !any(g_xCamera_ClipPlane) )
	{
		float3 brushColor = float3( 37.0/255.0, 245.0/255.0, 43.0/255.0 );
		float2 brushPos = IN.worldPos.xz - terrain_mouseHit.xy;
		float sqrDist = dot( brushPos, brushPos );
		float border = terrain_brushSize * 0.02;
		if ( border > 20 ) border = 20;
		if ( border < 1 ) border = 1;
		float minDist = terrain_brushSize - border;
		float maxDist = terrain_brushSize + border;

		if ( sqrDist < maxDist*maxDist && sqrDist > minDist*minDist )
		{
			color.rgb = brushColor;
		}
	}
		
	color = max( 0, color );
	output.g0 = float4( color.rgb, 1 );
	output.g1 = float4( surface.N * 0.5f + 0.5f, surface.roughness ); // RGB=normal, A=roughness
	return output;
}