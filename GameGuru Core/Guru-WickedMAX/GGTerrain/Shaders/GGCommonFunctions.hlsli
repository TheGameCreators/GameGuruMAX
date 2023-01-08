float3 ApplyFogCustom( float3 pos, float dist, float3 color, float3 viewDir )
{
	float fogMin = g_xFrame_Fog.x; //terrain_fogRange.x;
	float fogMax = g_xFrame_Fog.y; //terrain_fogRange.y;
	float3 fogColor = g_xFrame_WaterColor.rgb;
	float fogMinAmount = 1;
	
	if ( (g_xFrame_Options & OPTION_BIT_WATER_ENABLED) && pos.y < g_xFrame_WaterHeight+10 && g_xCamera_CamPos.y < g_xFrame_WaterHeight+15 ) 
	{
		fogMin = g_xFrame_WaterFogMin;
		fogMax = g_xFrame_WaterFogMax;
		fogMinAmount = saturate( 1 - g_xFrame_WaterFogMinAmount );

		float fogFade = viewDir.y * 0.5 + 0.5;
		fogFade = 1.0 - (fogFade * fogFade);
		fogColor *= fogFade;

		float dist2 = max( 0, g_xFrame_WaterHeight - pos.y );
		dist += dist2;
		/*
		float colorFade = max( 0, g_xFrame_WaterHeight - pos.y );
		colorFade = saturate( colorFade * 0.003 );
		colorFade = 1.0 - colorFade;
		color *= colorFade;
		*/
	}
	else
	{
		//LB: Restore fog control for interior scenes
		const float3 PassedInFogColor = GetFogColor();
		const float PassedInFogOpacity = clamp(GetFogOpacity(), 0.0, 1.0);

		if (g_xFrame_Options & OPTION_BIT_REALISTIC_SKY)
		{
			float3 horizonDir = -viewDir;
			float invLen = rsqrt( horizonDir.x*horizonDir.x + horizonDir.z*horizonDir.z );
			invLen *= 0.995;
			horizonDir.x *= invLen;
			horizonDir.z *= invLen;
			fogColor = GetDynamicSkyColor( float3(horizonDir.x, 0.1, horizonDir.z), false, false, false, true );
		}
		else
		{
			viewDir = -viewDir;
			viewDir.y = abs( viewDir.y );
			//PE: interior scenes , skybox looks strange, so disable for now.
			//fogColor = texture_globalenvmap.SampleLevel(sampler_linear_clamp, viewDir, 8).rgb;
			fogColor = color.rgb;
			//PE: Remove env map by opacity.
			//fogColor = lerp(color.rgb, fogColor, PassedInFogOpacity);
		}

		fogColor = lerp(fogColor, PassedInFogColor, PassedInFogOpacity);
	}

	if ( dist <= fogMin ) 
	{
		return color;
	}
	else
	{
		dist = max( 0.0, dist - fogMin );
		dist = exp( dist * 4.0 / (fogMin - fogMax) );
		dist = min( fogMinAmount, dist );
		color = lerp( fogColor, color, dist );
		return color;
	}
}