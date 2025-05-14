#ifndef WI_BRDF_HF
#define WI_BRDF_HF
#include "globals.hlsli"

// BRDF functions source: https://github.com/google/filament/blob/main/shaders/src/brdf.fs

#define MEDIUMP_FLT_MAX    65504.0
#define saturateMediump(x) min(x, MEDIUMP_FLT_MAX)
#define highp
#define pow5(x) pow(x, 5)
#define max3(v) max(max(v.x, v.y), v.z)

float D_GGX(float roughness, float NoH)
{
	// Walter et al. 2007, "Microfacet Models for Refraction through Rough Surfaces"
	float oneMinusNoHSquared = 1.0 - NoH * NoH;

	float a = NoH * roughness;
	float k = roughness / (oneMinusNoHSquared + a * a);
	float d = k * k * (1.0 / PI);
	return d;//saturateMediump(d);
}

float D_GGX_Cloth(float roughness, float NoH)
{
	float a2 = roughness * roughness;
	float cos2h = NoH * NoH;
	float sin2h = max(1.0 - cos2h, 0.0078125); // 2^(-14/2), so sin2h^2 > 0 in fp16
	float sin4h = sin2h * sin2h;
	float cot2 = -cos2h / (a2 * sin2h);
	return 1.0 / (PI * (4.0 * a2 + 1.0) * sin4h) * (4.0 * exp(cot2) + sin4h);
}

float D_GGX_Anisotropic(float at, float ab, float ToH, float BoH, float NoH)
{
	// Burley 2012, "Physically-Based Shading at Disney"

	// The values at and ab are perceptualRoughness^2, a2 is therefore perceptualRoughness^4
	// The dot product below computes perceptualRoughness^8. We cannot fit in fp16 without clamping
	// the roughness to too high values so we perform the dot product and the division in fp32
	float a2 = at * ab;
	highp float3 d = float3(ab * ToH, at * BoH, a2 * NoH);
	highp float d2 = dot(d, d);
	float b2 = a2 / d2;
	return a2 * b2 * b2 * (1.0 / PI);
}

float D_Charlie(float roughness, float NoH)
{
	// Estevez and Kulla 2017, "Production Friendly Microfacet Sheen BRDF"
	float invAlpha = 1.0 / roughness;
	float cos2h = NoH * NoH;
	float sin2h = max(1.0 - cos2h, 0.0078125); // 2^(-14/2), so sin2h^2 > 0 in fp16
	return (2.0 + invAlpha) * pow(sin2h, invAlpha * 0.5) / (2.0 * PI);
}

/*
float V_SmithGGXCorrelated(float roughness, float NoV, float NoL)
{
	// Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
	float a2 = roughness * roughness;
	// TODO: lambdaV can be pre-computed for all the lights, it should be moved out of this function
	float lambdaV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);
	float lambdaL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
	float v = 0.5 / (lambdaV + lambdaL);
	// a2=0 => v = 1 / 4*NoL*NoV   => min=1/4, max=+inf
	// a2=1 => v = 1 / 2*(NoL+NoV) => min=1/4, max=+inf
	// clamp to the maximum value representable in mediump
	return v;//saturateMediump(v);
}
*/

float V_SmithGGXCorrelated(float roughness, float NoV, float NoL)
{
	float a2 = roughness;
	float lambdaV = NoL * (NoV*(1 - a2) + a2);
	float lambdaL = NoV * (NoL*(1 - a2) + a2);
	float v = 0.5 / (lambdaV + lambdaL);
	return v;//saturateMediump(v);
}

float V_SmithGGXCorrelated_Anisotropic(float at, float ab, float ToV, float BoV,
	float ToL, float BoL, float NoV, float NoL)
{
	// Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
	// TODO: lambdaV can be pre-computed for all the lights, it should be moved out of this function
	float lambdaV = NoL * length(float3(at * ToV, ab * BoV, NoV));
	float lambdaL = NoV * length(float3(at * ToL, ab * BoL, NoL));
	float v = 0.5 / (lambdaV + lambdaL);
	return saturateMediump(v);
}

float V_Modified(float roughness, float NdotV, float NdotL)
{
	float k = roughness / 2.0;

    float SmithL = NdotL + (NdotL * (1-k) + k);
    float SmithV = NdotV + (NdotV * (1-k) + k);

	float Gs = 0.5 / (SmithL + SmithV);
	return Gs;
}

float V_3DSMax(float roughness, float LdotH, float NdotL)
{
	float k = roughness / 2.0;
	float k2 = k*k;
	float invK2 = 1.0 - k2;
	float vis = NdotL / ( LdotH * LdotH * invK2 + k2 );

	return vis;
}

float V_Kelemen(float LoH)
{
	// Kelemen 2001, "A Microfacet Based Coupled Specular-Matte BRDF Model with Importance Sampling"
	return saturateMediump(0.25 / (LoH * LoH));
}

float V_Neubelt(float NoV, float NoL)
{
	// Neubelt and Pettineo 2013, "Crafting a Next-gen Material Pipeline for The Order: 1886"
	return saturateMediump(1.0 / (4.0 * (NoL + NoV - NoL * NoV)));
}

float3 F_Schlick(const float3 f0, float f90, float VoH)
{
	// Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"
	return f0 + (f90 - f0) * pow5(1.0 - VoH);
}

float iorToF0(float transmittedIor, float incidentIor)
{
	return sqr((transmittedIor - incidentIor) / (transmittedIor + incidentIor));
}

float f0ToIor(float f0)
{
	float r = sqrt(f0);
	return (1.0 + r) / (1.0 - r);
}


// Surface descriptors:

struct SheenSurface
{
	float3 color;
	float roughness;

	// computed values:
	float roughnessBRDF;
	float DFG;
	float albedoScaling;
};

struct ClearcoatSurface
{
	float factor;
	float roughness;
	float3 N;

	// computed values:
	float roughnessBRDF;
	float3 R;
	float3 F;
};

struct Surface
{
	// Fill these yourself:
	float3 P;				// world space position
	float3 N;				// world space normal
	float3 V;				// world space view vector

	float3 albedo;			// diffuse light absorbtion value (rgb)
	float3 f0;				// fresnel value (rgb) (reflectance at incidence angle, also known as specular color)
	float3 F;
	float fView;
	float metalness;
	float roughness;		// roughness: [0:smooth -> 1:rough] (perceptual)
	float occlusion;		// occlusion [0 -> 1]
	float opacity;			// opacity for blending operation [0 -> 1]
	float4 emissiveColor;	// light emission [0 -> 1]
	float4 refraction;		// refraction color (rgb), refraction amount (a)
	float transmission;		// transmission factor
	float2 pixel;			// pixel coordinate (used for randomization effects)
	float2 screenUV;		// pixel coordinate in UV space [0 -> 1] (used for randomization effects)
	float3 T;				// tangent
	float3 B;				// bitangent
	float anisotropy;		// anisotropy factor [0 -> 1]
	float4 sss;				// subsurface scattering color * amount
	float4 sss_inv;			// 1 / (1 + sss)
	uint layerMask;			// the engine-side layer mask
	bool receiveshadow;
	float3 facenormal;		// surface normal without normal map

	// These will be computed when calling Update():
	float roughnessBRDF;	// roughness input for BRDF functions
	float NdotV;			// cos(angle between normal and view vector)
	float3 R;				// reflection vector
	float TdotV;
	float BdotV;
	float at;
	float ab;

	SheenSurface sheen;
	ClearcoatSurface clearcoat;

	inline void init()
	{
		albedo = 1;
		f0 = 0;
		F = 0;
		fView = 0;
		metalness = 0;
		roughness = 1;
		occlusion = 1;
		opacity = 1;
		emissiveColor = 0;
		refraction = 0;
		transmission = 0;
		pixel = 0;
		screenUV = 0;
		T = 0;
		B = 0;
		anisotropy = 0;
		sss = 0;
		sss_inv = 1;
		layerMask = ~0;
		receiveshadow = true;
		facenormal = 0;

		sheen.color = 0;
		sheen.roughness = 0;
		sheen.roughnessBRDF = 0;
		sheen.DFG = 0;
		sheen.albedoScaling = 0;

		clearcoat.factor = 0;
		clearcoat.roughness = 0;
		clearcoat.roughnessBRDF = 0;
		clearcoat.N = 0;
		clearcoat.F = 0;
		clearcoat.R = 0;
	}

	inline void create(
		in ShaderMaterial material,
		in float4 baseColor,
		in float4 surfaceMap,
		in float4 specularMap = 1
	)
	{
		init();

		opacity = baseColor.a;
		roughness = material.roughness;
		f0 = material.specularColor.rgb * specularMap.rgb * specularMap.a * material.specularColor.a;

		[branch]
		if (material.IsUsingSpecularGlossinessWorkflow())
		{
			// Specular-glossiness workflow:
			roughness *= saturate(1 - surfaceMap.a);
			f0 *= DEGAMMA(surfaceMap.rgb);
			albedo = baseColor.rgb;
		}
		else
		{
			// Metallic-roughness workflow:
			if (material.IsOcclusionEnabled_Primary())
			{
				occlusion = surfaceMap.r;
			}
			roughness *= surfaceMap.g;
			metalness = material.metalness * surfaceMap.b;
			float reflectance = material.reflectance * surfaceMap.a;
			albedo = lerp( baseColor.rgb, float3(0, 0, 0), metalness );
			f0 *= lerp( float3(reflectance, reflectance, reflectance), baseColor.rgb, metalness );
		}

		receiveshadow = material.IsReceiveShadow();
	}

	inline void createMetalness(
		float matMetalness,
		float matRoughness,
		float matOcclusion,
		float reflectance,
		float3 baseColor,
		uint matReceiveShadow
	)
	{
		init();

		roughness = matRoughness;
		metalness = matMetalness;
		occlusion = matOcclusion;
		receiveshadow = matReceiveShadow;
			
		albedo = lerp( baseColor.rgb, float3(0, 0, 0), metalness );
		f0 = lerp( float3(reflectance, reflectance, reflectance), baseColor.rgb, metalness );
	}

	inline void update()
	{
		roughness = clamp(roughness, 0.045, 1);

		roughnessBRDF = pow(roughness, 1.5);// * roughness;

		NdotV = saturate(abs(dot(N, V)) + 1e-5);

		//float f90 = saturate(50.0 * dot(f0, 0.33));
		float f90 = max(1 - roughnessBRDF, f0.r);
		F = F_Schlick(f0, f90, NdotV);

		R = -reflect(V, N);

#ifdef BRDF_CLEARCOAT
		clearcoat.roughness = clamp(clearcoat.roughness, 0.045, 1);
		clearcoat.roughnessBRDF = clearcoat.roughness * clearcoat.roughness;
		clearcoat.F = F_Schlick(f0, f90, saturate(abs(dot(clearcoat.N, V)) + 1e-5));
		clearcoat.F *= clearcoat.factor;
		clearcoat.R = -reflect(V, clearcoat.N);
#endif

#ifdef BRDF_SHEEN
		sheen.roughness = clamp(sheen.roughness, 0.045, 1);
		sheen.roughnessBRDF = sheen.roughness * sheen.roughness;

		// Sheen energy compensation: https://dassaultsystemes-technology.github.io/EnterprisePBRShadingModel/spec-2021x.md.html#figure_energy-compensation-sheen-e
		sheen.DFG = texture_sheenlut.SampleLevel(sampler_linear_clamp, float2(NdotV, sheen.roughness), 0).r;
		sheen.albedoScaling = 1.0 - max3(sheen.color) * sheen.DFG;
#endif

		TdotV = dot(T, V);
		BdotV = dot(B, V);
		at = max(0, roughnessBRDF * (1 + anisotropy));
		ab = max(0, roughnessBRDF * (1 - anisotropy));

#ifdef BRDF_CARTOON
		F = smoothstep(0.05, 0.1, F);
#endif // BRDF_CARTOON
	}

	inline bool IsReceiveShadow() { return receiveshadow; }
};

struct SurfaceToLight
{
	float3 L;		// surface to light vector (normalized)
	float3 H;		// half-vector between view vector and light vector
	float NdotL;	// cos angle between normal and light direction
	float3 NdotL_sss;	// NdotL with subsurface parameters applied
	float NdotH;	// cos angle between normal and half vector
	float LdotH;	// cos angle between light direction and half vector
	float VdotH;	// cos angle between view direction and half vector
	float3 Fresnel;	// fresnel term

	// Aniso params:
	float TdotL;
	float BdotL;
	float TdotH;
	float BdotH;

	inline void create(in Surface surface, in float3 Lnormalized)
	{
		L = Lnormalized;
		H = normalize(L + surface.V);

		NdotL = dot(L, surface.N);

#ifdef BRDF_NDOTL_BIAS
		NdotL += BRDF_NDOTL_BIAS;
#endif // BRDF_NDOTL_BIAS

		NdotL_sss = (NdotL + surface.sss.rgb) * surface.sss_inv.rgb;

		NdotH = saturate(dot(surface.N, H));
		LdotH = saturate(dot(L, H));
		VdotH = saturate(dot(surface.V, H));

		//float f90 = 0.5 + 2.0 * LdotH*LdotH * (1-surface.roughnessBRDF);
		float f90 = max(1 - surface.roughnessBRDF, surface.f0.r);
		Fresnel = F_Schlick( surface.f0, f90, LdotH );

		TdotL = dot(surface.T, L);
		BdotL = dot(surface.B, L);
		TdotH = dot(surface.T, H);
		BdotH = dot(surface.B, H);

#ifdef BRDF_CARTOON
		// SSS is handled differently in cartoon shader:
		//	1) The diffuse wraparound is monochrome at first to avoid banding with smoothstep()
		NdotL_sss = (NdotL + surface.sss.a) * surface.sss_inv.a;

		NdotL = smoothstep(0.005, 0.05, NdotL);
		NdotL_sss = smoothstep(0.005, 0.05, NdotL_sss);
		NdotH = smoothstep(0.98, 0.99, NdotH);

		// SSS is handled differently in cartoon shader:
		//	2) The diffuse wraparound is tinted after smoothstep
		NdotL_sss = (NdotL_sss + surface.sss.rgb) * surface.sss_inv.rgb;
#endif // BRDF_CARTOON

		NdotL = saturate(NdotL);
		NdotL_sss = saturate(NdotL_sss);
	}
};


// These are the functions that will be used by shaders:

float3 BRDF_GetSpecular(in Surface surface, in SurfaceToLight surfaceToLight)
{
#ifdef BRDF_ANISOTROPIC
	float D = D_GGX_Anisotropic(surface.at, surface.ab, surfaceToLight.TdotH, surfaceToLight.BdotH, surfaceToLight.NdotH);
	float Vis = V_SmithGGXCorrelated_Anisotropic(surface.at, surface.ab, surface.TdotV, surface.BdotV,
		surfaceToLight.TdotL, surfaceToLight.BdotL, surface.NdotV, surfaceToLight.NdotL);
#else
	float D = D_GGX(surface.roughnessBRDF, surfaceToLight.NdotH);
	//float Vis = V_SmithGGXCorrelated(surface.roughnessBRDF, surface.NdotV, surfaceToLight.NdotL);
	float Vis = V_Modified(surface.roughnessBRDF, surface.NdotV, surfaceToLight.NdotL);
#endif

	float3 specular = min( 1, D * Vis * surfaceToLight.Fresnel );

#ifdef BRDF_SHEEN
	specular *= surface.sheen.albedoScaling;
	D = D_Charlie(surface.sheen.roughnessBRDF, surfaceToLight.NdotH);
	Vis = V_Neubelt(surface.NdotV, surfaceToLight.NdotL);
	specular += D * Vis * surface.sheen.color;
#endif // BRDF_SHEEN

#ifdef BRDF_CLEARCOAT
	specular *= 1 - surface.clearcoat.F;
	float NdotH = saturate(dot(surface.clearcoat.N, surfaceToLight.H));
	D = D_GGX(surface.clearcoat.roughnessBRDF, NdotH);
	Vis = V_Kelemen(surfaceToLight.LdotH);
	specular += D * Vis * surface.clearcoat.F;
#endif // BRDF_CLEARCOAT

	return specular;
}
float3 BRDF_GetDiffuse(in Surface surface, in SurfaceToLight surfaceToLight)
{
	//return float3(1,1,1);
	return float3(1/PI, 1/PI, 1/PI);

	// Note: subsurface scattering will remove Fresnel (F), because otherwise
	//	there would be artifact on backside where diffuse wraps
	float3 diffuse = (1 - lerp(surfaceToLight.Fresnel, 0, saturate(surface.sss.a))) / PI;

#ifdef BRDF_SHEEN
	diffuse *= surface.sheen.albedoScaling;
#endif // BRDF_SHEEN

#ifdef BRDF_CLEARCOAT
	diffuse *= 1 - surface.clearcoat.F;
#endif // BRDF_CLEARCOAT

	return diffuse;
}

#endif // WI_BRDF_HF
