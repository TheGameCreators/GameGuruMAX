Texture2D texColor             : register( t40 );
Texture2D texNormal            : register( t41 );

Texture2D texRockColor         : register( t42 );
Texture2D texRockNormal        : register( t43 );

Texture2D texSnowColor         : register( t44 );
Texture2D texSnowNormal        : register( t45 );

Texture2D texSandColor         : register( t46 );
Texture2D texSandNormal        : register( t47 );

SamplerState sampler1 : register( s1 );

Texture2DArray<float> texture_shadowarray_2d : register( t14 );
SamplerState sampler_point_clamp: register( s7 );

#include "GGTerrainConstants.hlsli"

struct ShaderEntity
{
	float3 position;
	uint type8_flags8_range16;

	uint2 direction16_coneAngleCos16;
	uint energy16_X16; // 16 bits free
	uint color;

	uint layerMask;
	uint indices;
	uint cubeRemap;
	uint userdata;

	inline uint GetType()
	{
		return type8_flags8_range16 & 0xFF;
	}
	inline uint GetFlags()
	{
		return (type8_flags8_range16 >> 8) & 0xFF;
	}
	inline float GetRange()
	{
		return f16tof32((type8_flags8_range16 >> 16) & 0xFFFF);
	}
	inline float3 GetDirection()
	{
		return float3(
			f16tof32(direction16_coneAngleCos16.x & 0xFFFF),
			f16tof32((direction16_coneAngleCos16.x >> 16) & 0xFFFF),
			f16tof32(direction16_coneAngleCos16.y & 0xFFFF)
		);
	}
	inline float GetConeAngleCos()
	{
		return f16tof32((direction16_coneAngleCos16.y >> 16) & 0xFFFF);
	}
	inline float GetEnergy()
	{
		return f16tof32(energy16_X16 & 0xFFFF);
	}
	inline float GetCubemapDepthRemapNear()
	{
		return f16tof32(cubeRemap & 0xFFFF);
	}
	inline float GetCubemapDepthRemapFar()
	{
		return f16tof32((cubeRemap >> 16) & 0xFFFF);
	}
	inline float4 GetColor()
	{
		float4 fColor;

		fColor.x = (float)((color >> 0) & 0xFF) / 255.0f;
		fColor.y = (float)((color >> 8) & 0xFF) / 255.0f;
		fColor.z = (float)((color >> 16) & 0xFF) / 255.0f;
		fColor.w = (float)((color >> 24) & 0xFF) / 255.0f;

		return fColor;
	}
	inline uint GetMatrixIndex()
	{
		return indices & 0xFFFF;
	}
	inline uint GetTextureIndex()
	{
		return (indices >> 16) & 0xFFFF;
	}
	inline bool IsCastingShadow()
	{
		return indices != ~0;
	}

	// Load decal props:
	inline float GetEmissive() { return GetEnergy(); }
};

StructuredBuffer<ShaderEntity> EntityArray : register( t20 );
StructuredBuffer<float4x4> MatrixArray : register( t21 );

struct PixelIn
{
    float4 position : SV_POSITION;
	float3 normal : TEXCOORD0;
	float lodLevel : TEXCOORD1;
	float2 uv : TEXCOORD2;
#ifndef USE_VIRTUAL_TEXTURING
	float2 uv2 : TEXCOORD4;
#endif
	float3 worldPos : TEXCOORD3;
#ifndef USE_VIRTUAL_TEXTURING
	float height : TEXCOORD5;
#endif
};

struct GBuffer
{
	float4 g0 : SV_TARGET0;	/*FORMAT_R11G11B10_FLOAT*/
	float4 g1 : SV_TARGET1;	/*FORMAT_R8G8B8A8_FLOAT*/
};

float3 ApplyFog( float3 color, float3 lightDir, float3 pointPos )
{
    float3 viewDir = terrain_cameraPos - pointPos;
    float invDist = rsqrt(dot(viewDir,viewDir));
    float sunPoint = dot(viewDir*invDist,lightDir)*0.5 + 0.5;
    invDist = max( 0.0, 1.0/invDist - terrain_fogColor1.w );
    invDist = exp( invDist*terrain_fogColor2.w );
    float3 fogColor = lerp(terrain_fogColor1.rgb, terrain_fogColor2.rgb, pow(sunPoint,24.0));
    color = lerp( fogColor, color, invDist );
    return color;
}

inline float shadowCmpLookup( Texture2DArray<float> shadowMap, float3 coords, float dist, float4 interp, float scaleFactor )
{
	float4 depthSamples = shadowMap.Gather( sampler_point_clamp, coords );
    float4 shadows = depthSamples > dist;
    //shadows *= saturate( (depthSamples-dist)*scaleFactor );
    return dot( shadows.wzxy, interp );
}

inline float shadowCascade(in float texIndex, in float shadowPosZ, in float2 shadowUV, in uint cascade)
{
	const float slice = texIndex + cascade;
	const float realDistance = shadowPosZ;// + (0.0001*(cascade+1)); // bias was already applied when shadow map was rendered
	float shadow = 0;
	
	float4 interp;
    interp.xy = frac( shadowUV.xy * g_xFrame_ShadowRes2D - 0.5 );
    interp.zw = 1 - interp.xy;
    interp = interp.zxzx * interp.wwyy;

	float scaleFactor = 65536.0 / (cascade + 1);

	// soft shadow
	shadow += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2(-1, -1) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
	shadow += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2(-1,  0) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
	shadow += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2(-1,  1) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
	shadow += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2( 0, -1) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
	shadow += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV, slice), realDistance, interp, scaleFactor );
	shadow += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2( 0,  1) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
	shadow += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2( 1, -1) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
	shadow += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2( 1,  0) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
	shadow += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2( 1,  1) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
	shadow = 1 - (shadow.x / 9.0);
	
	// hard shadow with smoothed depth buffer
	//shadow = texture_shadowarray_2d.SampleLevel(sampler_linear_clamp, float3(shadowUV, slice), 0);

	return shadow;
}

inline bool is_saturated(float a) { return a == saturate(a); }
inline bool is_saturated(float3 a) { return is_saturated(a.x) && is_saturated(a.y) && is_saturated(a.z); }

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
 
//float4 main( PixelIn IN ) : SV_TARGET
GBuffer main( PixelIn IN )
{
	GBuffer output;
	
	float3 lightDir = terrain_sunDir;

	// TBN matrix
	float3 normal = normalize( IN.normal );
	float3 tangent = normalize( cross( normal, float3(0,0,1) ) );
	float3 binormal = normalize( cross( normal, tangent ) );
	float3x3 TBN = float3x3( tangent, binormal, normal );

	float2 viewDir = terrain_cameraPos.xz - IN.worldPos.xz;
	float lenSqr = dot(viewDir,viewDir) + 0.1;
	float uvScale = log2( lenSqr ) * 0.5 - 7;
	uvScale = max( uvScale, 1.0 );
	/*
	float dist = sqrt( dot( viewDir, viewDir ) );
	dist = max( dist/128.0, 2.0 );
	float uvScale = log2( dist );
	*/
	float uvLerp = frac( uvScale );
	uvScale = uvScale - uvLerp;
	uvScale = pow( 2, uvScale );
	
	float2 scaledUV = IN.uv / uvScale;
	float2 scaledUV2 = scaledUV * 2;

	float2 scaledDx = ddx(IN.uv) / uvScale;
	float2 scaledDy = ddy(IN.uv) / uvScale;

	// colors
	float3 grassColor = texColor.SampleGrad( sampler1, scaledUV, scaledDx, scaledDy ).rgb;
	float3 grassColor2 = texColor.SampleGrad( sampler1, scaledUV2, scaledDx*2, scaledDy*2 ).rgb;
	grassColor = lerp( grassColor2, grassColor, uvLerp );
		
	float3 rockColor = texRockColor.SampleGrad( sampler1, scaledUV, scaledDx, scaledDy ).rgb;
	float3 rockColor2 = texRockColor.SampleGrad( sampler1, scaledUV2, scaledDx*2, scaledDy*2 ).rgb;
	rockColor = lerp( rockColor2, rockColor, uvLerp );
	
	float3 snowColor = texSnowColor.SampleGrad( sampler1, scaledUV, scaledDx, scaledDy ).rgb;
	float3 snowColor2 = texSnowColor.SampleGrad( sampler1, scaledUV2, scaledDx*2, scaledDy*2 ).rgb;
	snowColor = lerp( snowColor2, snowColor, uvLerp );

	float3 sandColor = texSandColor.SampleGrad( sampler1, scaledUV, scaledDx, scaledDy ).rgb;
	float3 sandColor2 = texSandColor.SampleGrad( sampler1, scaledUV2, scaledDx*2, scaledDy*2 ).rgb;
	sandColor = lerp( sandColor2, sandColor, uvLerp );

	// normals
	float3 grassNormal = texNormal.SampleGrad( sampler1, scaledUV, scaledDx, scaledDy ).rgb;
	float3 grassNormal2 = texNormal.SampleGrad( sampler1, scaledUV2, scaledDx*2, scaledDy*2 ).rgb;
	grassNormal = lerp( grassNormal2, grassNormal, uvLerp );
	grassNormal = grassNormal * 2 - 1;
		
	float3 rockNormal = texRockNormal.SampleGrad( sampler1, scaledUV, scaledDx, scaledDy ).rgb;
	float3 rockNormal2 = texRockNormal.SampleGrad( sampler1, scaledUV2, scaledDx*2, scaledDy*2 ).rgb;
	rockNormal = lerp( rockNormal2, rockNormal, uvLerp );
	rockNormal = rockNormal * 2 - 1;
	
	float3 snowNormal = texSnowNormal.SampleGrad( sampler1, scaledUV, scaledDx, scaledDy ).rgb;
	float3 snowNormal2 = texSnowNormal.SampleGrad( sampler1, scaledUV2, scaledDx*2, scaledDy*2 ).rgb;
	snowNormal = lerp( snowNormal2, snowNormal, uvLerp );
	snowNormal = snowNormal * 2 - 1;

	float3 sandNormal = texSandNormal.SampleGrad( sampler1, scaledUV, scaledDx, scaledDy ).rgb;
	float3 sandNormal2 = texSandNormal.SampleGrad( sampler1, scaledUV2, scaledDx*2, scaledDy*2 ).rgb;
	sandNormal = lerp( sandNormal2, sandNormal, uvLerp );
	sandNormal = sandNormal * 2 - 1;

	// blend colors/normals using slope and height 
	float t = clamp( (IN.height-terrain_snowHeightStart)*terrain_snowTransition, 0.0, 1.0 );
	float3 color = lerp( grassColor, snowColor, t );
	float3 normal2 = lerp( grassNormal, snowNormal, t );

	t = clamp( (IN.height-terrain_sandHeightStart)*terrain_sandTransition, 0.0, 1.0 );
	color = lerp( sandColor, color, t );
	normal2 = lerp( sandNormal, normal2, t );
	
    // blend from that color to rock color based on the angle of the normal
    t = clamp((IN.normal.y-terrain_rockEnd)*terrain_rockTransition, 0.0, 1.0); 
    color = lerp( rockColor, color, t );
	normal2 = lerp( rockNormal, normal2, t );
	normal2 = normalize( normal2 );

	normal = mul( normal2, TBN );

	//float light = max( dot( lightDir, normal ), 0.0 ) + 0.2;
	float light = dot( lightDir, normal );
	if ( light > 0 ) light = light * terrain_sunColor.w * 0.2;
	light = light * 0.5 + 0.5;

	float shadow = 1.0;
	if ( g_xFrame_DirectionalLightIndex >= 0 ) shadow = GetShadow( g_xFrame_DirectionalLightIndex, IN.worldPos );
	light *= shadow;

	color = color * light * terrain_sunColor.rgb;	

	color = ApplyFog( color, -lightDir, IN.worldPos );
	
	normal = normal * 0.5 + 0.5;
	output.g0 = float4( color, 1 );
	output.g1 = float4( normal, 1 ); // RGB=normal, A=roughness
	return output;
	
    //return IN.color;
	//return tex0.Sample( samplerLinear, IN.uv );
}