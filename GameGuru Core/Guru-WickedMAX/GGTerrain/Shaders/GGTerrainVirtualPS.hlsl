Texture2D texColorAndMetalness   : register( t50 );
Texture2D texNormalRoughnessAO   : register( t51 );
							     
Texture2DArray<float> texPageTableArray : register( t53 );
Texture2D<float> texPageTableFinal      : register( t54 );

SamplerState sampler1            : register( s1 );

#include "GGTerrainConstants.hlsli"

#include "../GGTerrainPageSettings.h"

#include "PBR/brdf.hlsli"
#include "PBR/lightingHF.hlsli"

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
	float lodLevel : TEXCOORD2;
	float2 uv : TEXCOORD3;
};

struct GBuffer
{
	float4 g0 : SV_TARGET0;	/*FORMAT_R11G11B10_FLOAT*/
	float4 g1 : SV_TARGET1;	/*FORMAT_R8G8B8A8_FLOAT*/
};

float3 ApplyFog( float3 color, float3 lightDir, float3 pointPos )
{
    float3 viewDir = g_xCamera_CamPos - pointPos;
    float invDist = rsqrt(dot(viewDir,viewDir));
    float sunPoint = dot(viewDir*invDist,lightDir)*0.5 + 0.5;
    invDist = max( 0.0, 1.0/invDist - terrain_fogColor1.w );
    invDist = exp( invDist*terrain_fogColor2.w );
    float3 fogColor = lerp(terrain_fogColor1.rgb, terrain_fogColor2.rgb, pow(max(sunPoint,0),24.0));
    color = lerp( fogColor, color, invDist );
    return color;
}

// Shadow functions
float GetShadow( uint entityIndex, float3 worldPos )
{
	uint matrixIndex = EntityArray[entityIndex].GetMatrixIndex();
	uint textureIndex = EntityArray[entityIndex].GetTextureIndex();
	float shadow = 1;

	[loop]
	for (uint cascade = 0; cascade < g_xFrame_ShadowCascadeCount; ++cascade)
	{
		// Project into shadow map space (no need to divide by .w because ortho projection!):
		float3 ShPos = mul(MatrixArray[matrixIndex + cascade], float4(worldPos, 1)).xyz;
		float3 ShTex = ShPos * float3(0.5, -0.5, 0.5) + 0.5;

		// Determine if pixel is inside current cascade bounds and compute shadow if it is:
		[branch]
		if (is_saturated(ShTex))
		{
			const float shadow_main = shadowCascade(textureIndex, ShPos.z, ShTex.xy, cascade);
			const float3 cascade_edgefactor = saturate(saturate(abs(ShPos)) - 0.8) * 5.0; // fade will be on edge and inwards 20%
			const float cascade_fade = max(cascade_edgefactor.x, max(cascade_edgefactor.y, cascade_edgefactor.z));

			// If we are on cascade edge threshold and not the last cascade, then fallback to a larger cascade:
			[branch]
			if (cascade_fade > 0 && cascade < g_xFrame_ShadowCascadeCount - 1)
			{
				// Project into next shadow cascade (no need to divide by .w because ortho projection!):
				cascade += 1;
				ShPos = mul(MatrixArray[matrixIndex + cascade], float4(worldPos, 1)).xyz;
				ShTex = ShPos * float3(0.5, -0.5, 0.5) + 0.5;
				const float shadow_fallback = shadowCascade(textureIndex, ShPos.z, ShTex.xy, cascade);

				shadow *= lerp(shadow_main, shadow_fallback, cascade_fade);
			}
			else
			{
				shadow *= shadow_main;
			}
			break;
		}
	}

	return shadow;
}

// virtual texture variables
static const float2 virtToPageSize = float2( pageSize / physTexSizeX, pageSize / physTexSizeY );
static const float2 texelOffset = float2( pagePaddingLeft / physTexSizeX, pagePaddingLeft / physTexSizeY );

GBuffer main( PixelIn IN )
{
	GBuffer output;
	
	float3 lightDir = terrain_sunDir;

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
		detailLevel = 15;
		mipSize = 1;
	}
	else
	{
		detailLevel--;
		mipSize = 256 >> mipLevel;
	}
	
	uint iOffsetX = iPageEntry & 0xFF;
	uint iOffsetY = iPageEntry >> 8;
	float2 pageOffset = float2(iOffsetX / physPagesX, iOffsetY / physPagesY);

	float2 pageUV = frac( levelUV * mipSize ) * virtToPageSize + texelOffset;
	
	pageUV += pageOffset;

	float uvScale = 256.0 / (1 << detailLevel);
	float2 dx = ddx( IN.uv ) * (uvScale / physTexSizeX);
	float2 dy = ddy( IN.uv ) * (uvScale / physTexSizeY);

	float3 color = texColorAndMetalness.SampleGrad( sampler1, pageUV, dx, dy ).rgb;
	//float3 color = mipColors[ min(detailLevel, 15) ].rgb;
	//float3 color = mipColors[ min(origLOD,15) ].rgb;
	//float3 color = float3( IN.uv, 0 );
	//float3 color = float3( pageUV, 0 );
	//float3 color = float3( ddx( IN.uv )*10, 0 );

	float2 normalRG = texNormalRoughnessAO.SampleGrad( sampler1, pageUV, dx, dy ).rg;
	float3 normal;
	normal.rb = normalRG * 2 - 1;
	normal.g = 1 - (normal.r*normal.r + normal.b*normal.b);
	if ( normal.b > 0 ) normal.b = sqrt( normal.b );

	//float light = max( dot( lightDir, normal ), 0.0 ) + 0.2;
	float light = dot( lightDir, normal );
	if ( light > 0 ) light = light * terrain_sunColor.w * 0.2;
	light = light * 0.5 + 0.5;

	float shadow = 1.0;
	if ( g_xFrame_DirectionalLightIndex >= 0 && g_xFrame_ShadowRes2D > 0 ) shadow = GetShadow( g_xFrame_DirectionalLightIndex, IN.worldPos );
	light *= (shadow * 0.8 + 0.2);

	color = color * light * terrain_sunColor.rgb;	

	color = ApplyFog( color, -lightDir, IN.worldPos );

	normal = normal * 0.5 + 0.5;
	output.g0 = float4( color, 1 );
	output.g1 = float4( normal, 1 ); // RGB=normal, A=roughness
	return output;
}