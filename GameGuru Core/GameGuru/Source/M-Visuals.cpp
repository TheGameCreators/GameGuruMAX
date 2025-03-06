//----------------------------------------------------
//--- GAMEGURU - M-Visuals
//----------------------------------------------------

// Includes
#include "stdafx.h"
#include "gameguru.h"

#ifdef WICKEDENGINE
#include "GGTerrain/GGTerrain.h"
#include "GGTerrain/GGGrass.h"
#endif

#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

// 
//  VISUALS MODULE
// 

namespace GGGrass
{
	extern struct GGGrassParams gggrass_global_params;
}

void visuals_init ( void )
{
	//  Default visual settings
	//if ( GetMaxPixelShaderValue() >= 3.0 ) // Yikes - that was old :)
	//{
	t.visuals.shaderlevels.terrain=1;
	t.visuals.shaderlevels.entities=1;
	t.visuals.shaderlevels.vegetation=1;
	t.visuals.shaderlevels.lighting=2;
	//}
	//else
	//{
	//	t.visuals.shaderlevels.terrain=3;
	//	t.visuals.shaderlevels.entities=2;
	//	t.visuals.shaderlevels.vegetation=3;
	//	t.visuals.shaderlevels.lighting=2;
	//}

	//  Global settings
	t.visuals.mode=1;

	//  Reset to default values
	visuals_resetvalues ( );

	//  Graphic Options
	t.visuals.reflectionmode=99;
	t.visuals.shadowmode=100;
	t.visuals.lightraymode=0;
	t.visuals.vegetationmode=50;
	t.visuals.occlusionvalue=100;
	t.visuals.debugvisualsmode=0;

	//  Post Effects
	t.visuals.bloommode=50;
	t.visuals.VignetteRadius_f=1.0;
	t.visuals.VignetteIntensity_f=1.0;
	t.visuals.MotionDistance_f=0.5;
	t.visuals.MotionIntensity_f=0;
	t.visuals.DepthOfFieldDistance_f=0.5;
	t.visuals.DepthOfFieldIntensity_f=0;
	t.visuals.LightrayLength_f=0.55f;
	t.visuals.LightrayQuality_f=20.0f;
	t.visuals.LightrayDecay_f=0.8f;
	t.visuals.SAORadius_f=0.5f;
	t.visuals.SAOIntensity_f=0.5f;
	t.visuals.SAOQuality_f=1.0f;
	t.visuals.LensFlare_f=0.5f;

	//  Trigger all visuals to update
	t.visuals.refreshvegetation=0;
	t.visuals.refreshshaders=1;
	t.visuals.refreshskysettings=0;
	t.visuals.refreshterraintexture=0;
	t.visuals.refreshvegtexture=0;

	//  Load any previous global settings for in-game
	visuals_load ( );
	t.gamevisuals=t.visuals;

	//  And set editor defaults over the loaded data
	t.editorvisuals=t.visuals ; visuals_editordefaults ( );
	if (t.visuals.reflectionmode == 0)
		t.visuals.reflectionmode = 1;
}

void visuals_calcsunanglefromtimeofday(int iTimeOfday, float* pfSunAngleX, float* pfSunAngleY, float* pfSunAngleZ)
{
	const float time_value[] = { 6.3, 9, 12,15, 18, 20,0 };
	float fCurrentTimeAngleX = 45.0;
	float time = time_value[iTimeOfday];
	if (time >= 12)
	{
		if (time > 15)
		{
			//PE: Sun goes down a little faster.
			fCurrentTimeAngleX = (time - 14) * (180.0 / 12.0);
		}
		else
			fCurrentTimeAngleX = (time - 12) * (180.0 / 12.0);
	}
	else 
	{
		fCurrentTimeAngleX = 180 + (time * (180.0 / 12.0));
	}
	*pfSunAngleX = fCurrentTimeAngleX;
	*pfSunAngleY = 270.0;
	*pfSunAngleZ = 0.0;
}

void visuals_resetvalues (bool bNewLevel)
{
	// Visual Settings
	t.visuals.FogR_f=140;
	t.visuals.FogG_f=158;
	t.visuals.FogB_f=173;
	t.visuals.FogA_f=128;
	t.visuals.AmbienceIntensity_f=128.0f;

	t.visuals.FogNearest_f = 40000;
	t.visuals.FogDistance_f = 700000;
	t.visuals.AmbienceRed_f = 70;
	t.visuals.AmbienceGreen_f = 70;
	t.visuals.AmbienceBlue_f = 70;

	t.visuals.iPhysicsDebugDraw = 0;

	t.visuals.SurfaceIntensity_f=1.333;
	t.visuals.SurfaceRed_f=255;
	t.visuals.SurfaceGreen_f=255;
	t.visuals.SurfaceBlue_f=255;
	t.visuals.SurfaceSunFactor_f=1.0;
	t.visuals.Specular_f=1.0;
	t.visuals.DistanceTransitionStart_f=4000;
	t.visuals.DistanceTransitionRange_f=400;
	t.visuals.DistanceTransitionMultiplier_f=1;
	t.visuals.PostBrightness_f=0.0f;
	t.visuals.PostContrast_f=1.1666f;
	if (t.visuals.reflectionmode == 0)
		t.visuals.reflectionmode = 1;

	// defaults for occlusion, lightrays and water mode
	t.visuals.occlusionvalue = 100;
	t.visuals.LightrayLength_f=0.55f;
	t.visuals.LightrayQuality_f=20.0f;
	t.visuals.LightrayDecay_f=0.8f;
	t.visuals.SAORadius_f=0.5f;
	t.visuals.SAOIntensity_f=0.5f;
	t.visuals.SAOQuality_f=1.0f;
	t.visuals.LensFlare_f=0.5f;

	//  Camera settings
	t.visuals.CameraNEAR_f=DEFAULT_NEAR_PLANE;
	t.visuals.CameraFAR_f=DEFAULT_FAR_PLANE;
	t.visuals.CameraASPECT_f = 1920.0 / 1080.0; //PE: Fixed in wicked, or fov will be wrong on widescreen.
	t.visuals.CameraFOV_f = 60; //LB:Default now 90 degree (XM_PI/(fov/15.0f) was 45.0; //PE: Fixed in wicked.
	t.visuals.WeaponFOV_f = 60; //LB:Default now 90 degree (XM_PI/(fov/15.0f) was 45.0;
	t.visuals.CameraFOVZoomed_f=1.0;

	//  Quality Settings
	t.visuals.TerrainLOD1_f=4000.0;
	t.visuals.TerrainLOD2_f=5000.0;
	t.visuals.TerrainLOD3_f=5500.0;
	t.visuals.TerrainSize_f=100.0;
	t.visuals.VegQuantity_f=100.0;
	t.visuals.VegWidth_f=50.0;
	t.visuals.VegHeight_f=50.0;

	//  World Settings (reset to default selections)
	terrain_initstyles_reset ( );
	grass_initstyles_reset();
	t.visuals.sky_s="";
	t.visuals.terrain_s=g.terrainstyle_s;
	t.visuals.vegetation_s=g.vegstyle_s;
	visuals_updateskyterrainvegindex ( );
	if (t.visuals.skyindex >= t.skybank_s.size()) t.visuals.skyindex = t.skybank_s.size() - 1;
	if (t.visuals.terrainindex >= t.terrainstylebank_s.size()) t.visuals.terrainindex = t.terrainstylebank_s.size() - 1;
	if (t.visuals.vegetationindex >= t.vegstylebank_s.size()) t.visuals.vegetationindex = t.vegstylebank_s.size() - 1;
	t.visuals.sky_s=t.skybank_s[t.visuals.skyindex];
	t.visuals.terrain_s=t.terrainstylebank_s[t.visuals.terrainindex];
	t.visuals.vegetation_s=t.vegstylebank_s[t.visuals.vegetationindex];
	t.strwork = ""; t.strwork = t.strwork +"visuals.sky$="+t.visuals.sky_s;
	timestampactivity(0,t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork +"visuals.terrain$="+t.visuals.terrain_s;
	timestampactivity(0,t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork +"visuals.vegetation$="+t.visuals.vegetation_s;
	timestampactivity(0,t.strwork.Get());

	float fWaterLine = t.terrain.waterliney_f;

	visuals_water_reset();

	//PE: Dont change water line as each biome is different.
	if (!bNewLevel)
		g.gdefaultwaterheight = t.terrain.waterliney_f = fWaterLine;

	t.visuals.CameraFOV_f = 60; //LB:Default now 90 degree (XM_PI/(fov/15.0f) was 45.0;
	t.visuals.WeaponFOV_f = 60; //LB:Default now 90 degree (XM_PI/(fov/15.0f) was 45.0;

	t.visuals.FogA_f = 0.0f; //PE: We use FogA as height in wicked.
	t.visuals.SkyIntensity_f = 1.0f;
	t.visuals.SunIntensity_f = 9.0f;
	t.visuals.SunRed_f = 255.0f;
	t.visuals.SunGreen_f = 255.0f;
	t.visuals.SunBlue_f = 255.0f;
	t.visuals.ZenithRed_f = 104.0f;
	t.visuals.ZenithGreen_f = 104.0f;
	t.visuals.ZenithBlue_f = 104.0f;
	t.visuals.fSunShadowBias = 0.00005f;
	t.visuals.bColorGrading = false;
	t.visuals.ColorGradingLUT = "";
	t.visuals.bBloomEnabled = true;
	t.visuals.bLevelVSyncEnabled = true;
	t.visuals.bOcclusionCulling = false;

	t.visuals.bEnableTerrainChunkCulling = false;
	t.visuals.bEnablePointShadowCulling = false;
	t.visuals.bEnableSpotShadowCulling = false;
	t.visuals.bEnableObjectCulling = false;
	t.visuals.bEnableAnimationCulling = false;
	t.visuals.fLODMultiplier = 1.0f;

	t.visuals.bEnable30FpsAnimations = false;

	t.visuals.bShadowsLowestLOD = false;
	t.visuals.bProbesLowestLOD = false;
	t.visuals.bRaycastLowestLOD = false;
	t.visuals.bPhysicsLowestLOD = false;
	t.visuals.bThreadedPhysics = false;
	t.visuals.bReflectionsLowestLOD = false;

	t.visuals.g_bDelayedShadows = false;
	t.visuals.g_bDelayedShadowsLaptop = false;

	t.visuals.fsetBloomThreshold = 2.0f;
	t.visuals.ApparentSize = 0.000008f;
	t.visuals.fsetBloomStrength = 1.0f;
	t.visuals.bSSREnabled = false;
	t.visuals.bReflectionsEnabled = true;
	t.visuals.bFXAAEnabled = false;

	t.visuals.bDOF = false;
	t.visuals.fDOFStrength = 10.0f;
	t.visuals.fDOFApertureSize = 0.2f;
	t.visuals.fDOFFocalLength = 300.0f;

	t.visuals.bTessellation = true;
	t.visuals.bLightShafts = true;
	t.visuals.lightShaftDensity = 0.8f;
	t.visuals.lightShaftWeight = 0.3f;
	t.visuals.lightShaftDecay = 0.945f;
	t.visuals.lightShaftExposure = 0.7f;
	t.visuals.bLensFlare = true;
	t.visuals.bAutoExposure = false;
	t.visuals.fAutoExposureRate = 0.5f;
	t.visuals.fAutoExposureKey = 0.15f;
	t.visuals.fExposure = 1.0f;
	t.visuals.fGamma = 2.2f;
	t.visuals.iMSAASampleCount = 1;
	t.visuals.iFSRMode = 0;
	t.visuals.fFSRSharpness = 0.2f;

	t.visuals.iMSAO = 1;
	t.visuals.fMSAOPower = 1.0;

	t.visuals.iShadowSpotCascadeResolution = 2048;
	t.visuals.iShadowSpotResolution = 512;
	t.visuals.iShadowPointResolution = 512;
	t.visuals.iShadowPointMax = 16;
	t.visuals.iShadowSpotMax = 8;
	t.visuals.bTransparentShadows = false;

	t.visuals.fShadowFarPlane = DEFAULT_FAR_PLANE;

	t.visuals.SkyCloudiness = 0.68f;
	t.visuals.SkyCloudCoverage = 1.38f;
	t.visuals.SkyCloudHeight = 60000.0f;
	t.visuals.SkyCloudThickness = 98000.0f;
	t.visuals.SkyCloudSpeed = 3.0f;

	t.visuals.bWaterEnable = true;
	t.showeditorwater = true;

	t.visuals.fWaterWaveAmplitude = 20.0f;
	t.visuals.fWaterPatchLength = 40.0f;
	t.visuals.fWaterChoppyScale = 0.0f;
	t.visuals.fWaterWindDependency = 0.0f;
	t.visuals.WaterSpeed1 = 0.06f;
	t.visuals.WaterFogMaxDist = 11500.0f;
	t.visuals.WaterFogMinDist = 0.0f;
	t.visuals.WaterFogMinAmount = 0.25f;

	t.visuals.iTimeOfday = 1;
	visuals_calcsunanglefromtimeofday(t.visuals.iTimeOfday,&t.visuals.SunAngleX,&t.visuals.SunAngleY,&t.visuals.SunAngleZ);

	t.visuals.bSimulate24Hours = false;
	t.visuals.fTimeSpeed = 1.0;

	t.visuals.fWeatherIntensity = 50.0;
	t.visuals.fWeatherLighting = 0.0;
	t.visuals.fWeatherThunder = 0.0;
	t.visuals.fWeatherWind = 50.0;

	t.visuals.bPPSnow = false;
	t.visuals.voxel_steps = 16.0f;
	t.visuals.pp_size = 1.0f;
	t.visuals.pp_alpha = 1.0f;
	t.visuals.wind_direction_x = 1.0f;
	t.visuals.wind_direction_y = -7.0f;
	t.visuals.wind_direction_z = 1.0f;
	t.visuals.wind_speed = 1.0f;
	t.visuals.wind_randomness = 1.0f;
	t.visuals.bpp_disable_indoor = true;
	t.visuals.tree_wind = 0.0f;
	
	t.visuals.fLevelDifficulty = 50.0;

	for (int iL = 0; iL < 32; iL++) 
	{
		t.visuals.sTerrainTextures[iL] = "";
		t.visuals.sTerrainTexturesName[iL] = "";
	}
	for (int iL = 0; iL < 128; iL++) 
	{
		t.visuals.sGrassTextures[iL] = "";
		t.visuals.sGrassTexturesName[iL] = "";
		extern bool iDeleteAllGrassTextures;
		iDeleteAllGrassTextures = true;
		t.visuals.sFactionName[iL] = "";
	}

	t.visuals.bEndableAmbientMusicTrack = false;
	t.visuals.sAmbientMusicTrack = "";
	t.visuals.iAmbientMusicTrackVolume = 100;
	t.visuals.bEnableCombatMusicTrack = false;
	t.visuals.sCombatMusicTrack = "";
	t.visuals.iCombatMusicTrackVolume = 100;

	if (bNewLevel)
	{
		t.visuals.bEndableTreeDrawing = false;
		t.visuals.bEndableGrassDrawing = false;
		t.visuals.bEndableTerrainDrawing = true;

		t.showeditortrees = false; //Editor also follow this on new level.
		t.showeditorveg = false;
		t.showeditorterrain = true;
		t.visuals.iHeightmapWidth = 0;
		t.visuals.iHeightmapHeight = 0;
	}

	t.visuals.bDisableSkybox = false;

	t.visuals.bRainEnabled = false;
	t.visuals.fRainSpeedX = -1.5;
	t.visuals.fRainSpeedY = -18.0;
	t.visuals.fRainOpacity = 0.0;
	t.visuals.fRainScaleX = 2.0;
	t.visuals.fRainScaleY = 2.0;
	t.visuals.fRainRefreactionScale = 0.005;

	t.visuals.bSnowEnabled = false;
	t.visuals.fSnowLayers = 15.0;
	t.visuals.fSnowDepth = 0.5;
	t.visuals.fSnowWind = 0.5;
	t.visuals.fSnowSpeed = 2.0;
	t.visuals.fSnowOpacity = 1.0;
	t.visuals.fSnowOffset = 0.0;

	t.visuals.iEnvProbeResolution = 128;

	t.visuals.newperformancepresets = 0;

	if (t.visuals.skyindex == 0)
	{
		extern wiECS::Entity g_weatherEntityID;
		wiScene::WeatherComponent* weather = wiScene::GetScene().weathers.GetComponent(g_weatherEntityID);
		if (weather)
		{
			//PE: Must delete old skymap.
			if (weather->skyMap != nullptr && weather->skyMapName.length() > 0)
			{
				//PE: Make sure to free any old resources.
				WickedCall_DeleteImage(weather->skyMapName);
			}
			weather->skyMap = nullptr;
			weather->skyMapName = "";
		}
	}

	//PE: MEM - 1472 : Wicked_Update_Visuals((void *)&t.visuals);               S:0MB   V : (3620, 0)
	void Wicked_Update_Visuals(void *voidvisual);
	Wicked_Update_Visuals((void *)&t.visuals);
	//PE: MEM - 1480 : Done Wicked_Update_Visuals((void *)&t.visuals);          S:889MB V : (4510, 0)
}

void visuals_editordefaults ( void )
{
	//  Editor requires some different values
	t.visuals=t.editorvisuals;
	t.visuals.shaderlevels.terrain=3;
	t.visuals.shaderlevels.entities=2;
	t.visuals.shaderlevels.vegetation=3;
	t.visuals.shaderlevels.lighting=2;

	t.visuals.TerrainLOD1_f=8000.0;
	t.visuals.TerrainLOD2_f=15000.0;
	t.visuals.TerrainLOD3_f=20000.0;
	t.visuals.TerrainSize_f=100.0f;
	t.visuals.AmbienceIntensity_f=89.25f;
	//PE: We should always have light in editor, addition to issue: https://github.com/TheGameCreators/GameGuruRepo/issues/352#event-2404317522
	//PE: fix for commit: https://github.com/TheGameCreators/GameGuruRepo/commit/4aff257545b19a969e4676c7b057a42d8e93fdf1
	t.visuals.SurfaceSunFactor_f = 1.0;
	t.visuals.SurfaceIntensity_f=0.8f;
	t.visuals.SurfaceRed_f=255;
	t.visuals.SurfaceGreen_f=255;
	t.visuals.SurfaceBlue_f=255;
	t.visuals.PostBrightness_f=0.0;
	t.visuals.PostContrast_f=1.1666f;

	t.visuals.CameraASPECT_f = 1920.0 / 1080.0; //PE: Fixed in wicked, or fov will be wrong on widescreen.
	t.visuals.CameraFOV_f = 60; //LB:Default now 90 degree (XM_PI/(fov/15.0f) was 45.0; //PE: Fixed in wicked.
	t.visuals.WeaponFOV_f = 60; //LB:Default now 90 degree (XM_PI/(fov/15.0f) was 45.0;

	//PE: Only have camera settings in test game.
	t.visuals.CameraNEAR_f = DEFAULT_NEAR_PLANE;
	t.visuals.CameraFAR_f = DEFAULT_FAR_PLANE;
	t.visuals.CameraFOVZoomed_f=1.0f;
	t.visuals.shadowmode=100;

	if (t.visuals.FogA_f > 2.0)
		t.visuals.FogA_f = 0;
	
	t.editorvisuals=t.visuals;
}

void visuals_water_reset(void)
{
	//Water Shader Settings
	g.gdefaultwaterheight = -500.0f;// GGORIGIN_Y; properly out the picture (stops it showing on grey grid level)
	t.terrain.waterliney_f = g.gdefaultwaterheight;
	t.visuals.WaterRed_f = 9;
	t.visuals.WaterGreen_f = 21;
	t.visuals.WaterBlue_f = 43;
	t.visuals.WaterAlpha_f = 0.0f;
	t.visuals.WaterWaveIntensity_f = 155.0f;
	t.visuals.WaterTransparancy_f = 0.75f;
	t.visuals.WaterReflection_f = 0.50f;
	t.visuals.WaterReflectionSparkleIntensity = 1.90f;
	t.visuals.WaterFlowDirectionX = 1;
	t.visuals.WaterFlowDirectionY = 1;
	t.visuals.WaterDistortionWaves = 0.030f;
	t.visuals.WaterSpeed1 = 0.06f;
	t.visuals.WaterFogMaxDist = 11500.0f;
	t.visuals.WaterFogMinDist = 0.0f;
	t.visuals.WaterFogMinAmount = 0.25f;
	t.visuals.WaterFlowSpeed = 1.0f;
}

void visuals_newlevel ( void )
{
	// 310117 - Game gets its visuals from the visuals.ini file
	visuals_load ( );
	// 090517 - leaves leftover things like old FOG setting on a !!NEW LEVEL!!
	//t.gamevisuals = t.visuals;

	//  New level resets these values
	visuals_resetvalues ( );

	//  Trigger all visuals to update
	t.visuals.refreshvegetation=0;
	t.visuals.refreshshaders=1;
	t.visuals.refreshskysettings=0;
	t.visuals.refreshterraintexture=0;
	t.visuals.refreshvegtexture=0;
	t.editorvisuals=t.visuals;

	// 310117 - do not wipe out game visuals (see above)
	//t.gamevisuals=t.visuals;

	// 090517 - actually we WANT to reset some of the game visual settings (reset fog/terrain/etc) for NEW LEVEL!
	t.gamevisuals=t.visuals;

	//  Apply editor defaults
	visuals_editordefaults ( );

	// also ensure new terrain starts with no PBR terrain
	//t.terrain.iTerrainPBRMode = 0;
	//t.terrain.iForceTerrainVegShaderUpdate = 1;

	// force a fade in of the gamma (hides construction artefacts and looks nice)
	extern float g_fGlobalGammaFadeIn;
	g_fGlobalGammaFadeIn = -2;
}

void visuals_free ( void )
{
	//  clear values when end session
	t.nearestlightindex[1]=0;
	t.nearestlightindex[2]=0;
	t.nearestlightindex[3]=0;
	t.nearestlightindex[4]=0;

	//  reset underwater effects when session ends
	physics_player_reset_underwaterstate ( );

return;

}

void visuals_updateskyterrainvegindex ( void )
{
	//  Find sky (saved EXEs have smaller sky banks)
	g.skyindex=1;
	for ( t.s = 1 ; t.s<=  g.skymax; t.s++ )
	{
		if (  t.skybank_s[t.s] == "clear"  )  g.skyindex = t.s;
	}
	#ifdef WICKEDENGINE
	//PE: Support "None"
	g.skyindex = 0;// for MAX, Non is the default (Dynamic Sky!)
	for (t.s = 0; t.s <= g.skymax; t.s++)
	#else
	for ( t.s = 1 ; t.s<=  g.skymax; t.s++ )
	#endif
	{
		if (  t.skybank_s[t.s] == t.visuals.sky_s ) 
		{
			g.skyindex=t.s;
		}
	}
	t.visuals.skyindex=g.skyindex;

	//  Find terrain (saved EXEs have smaller sky banks)
	g.terrainstyleindex=1;
	for ( t.t = 1 ; t.t<=  g.terrainstylemax; t.t++ )
	{
		if (  t.terrainstylebank_s[t.t] == t.visuals.terrain_s ) 
		{
			g.terrainstyleindex=t.t;
		}
	}
	t.visuals.terrainindex=g.terrainstyleindex;

	//  Find veg (saved EXEs have smaller sky banks)
	g.vegstyleindex=1;
	for ( t.v = 1 ; t.v<=  g.vegstylemax; t.v++ )
	{
		if (  t.vegstylebank_s[t.v] == t.visuals.vegetation_s ) 
		{
			g.vegstyleindex=t.v;
		}
	}
	t.visuals.vegetationindex=g.vegstyleindex;
}

void visuals_save ( void )
{
	// save all visual settings
	t.visfile_s=g.fpscrootdir_s+"\\visuals.ini";
	#ifdef WICKEDENGINE
	char pRealVisFile[MAX_PATH] = "";
	strcpy(pRealVisFile, t.visfile_s.Get());
	GG_GetRealPath(pRealVisFile, 1);
	t.visfile_s = pRealVisFile;
	#endif
	if ( FileExist(t.visfile_s.Get()) == 1 ) DeleteAFile ( t.visfile_s.Get() );
	OpenToWrite (  1,t.visfile_s.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.shaderlevels.terrain="+Str(t.visuals.shaderlevels.terrain);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.shaderlevels.entities="+Str(t.visuals.shaderlevels.entities);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.shaderlevels.vegetation="+Str(t.visuals.shaderlevels.vegetation);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.shaderlevels.lighting="+Str(t.visuals.shaderlevels.lighting);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.mode="+Str(t.visuals.mode);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.PostContrast#="+Str(t.visuals.PostContrast_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.PostBrightness#="+Str(t.visuals.PostBrightness_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.FogNearest#="+Str(t.visuals.FogNearest_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.FogDistance#="+Str(t.visuals.FogDistance_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.FogR#="+Str(t.visuals.FogR_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.FogG#="+Str(t.visuals.FogG_f) ;
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.FogB#="+Str(t.visuals.FogB_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.FogA#="+Str(t.visuals.FogA_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.AmbienceIntensity#="+Str(t.visuals.AmbienceIntensity_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.AmbienceRed#="+Str(t.visuals.AmbienceRed_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.AmbienceGeen#="+Str(t.visuals.AmbienceGreen_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.AmbienceBlue#="+Str(t.visuals.AmbienceBlue_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.SurfaceIntensity#="+Str(t.visuals.SurfaceIntensity_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.SurfaceRed#="+Str(t.visuals.SurfaceRed_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.SurfaceGreen#="+Str(t.visuals.SurfaceGreen_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.SurfaceBlue#="+Str(t.visuals.SurfaceBlue_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.Specular#="+Str(t.visuals.Specular_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.SurfaceSunFactor#="+Str(t.visuals.SurfaceSunFactor_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.DistanceTransitionStart#="+Str(t.visuals.DistanceTransitionStart_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.DistanceTransitionRange#="+Str(t.visuals.DistanceTransitionRange_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.CameraNEAR#="+Str(t.visuals.CameraNEAR_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.CameraFAR#="+Str(t.visuals.CameraFAR_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.CameraFOV#="+Str(t.visuals.CameraFOV_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.WeaponFOV#="+Str(t.visuals.WeaponFOV_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.CameraFOVZoomed#="+Str(t.visuals.CameraFOVZoomed_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.TerrainLOD1#="+Str(t.visuals.TerrainLOD1_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.TerrainLOD2#="+Str(t.visuals.TerrainLOD2_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.TerrainLOD3#="+Str(t.visuals.TerrainLOD3_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.TerrainSize#="+Str(t.visuals.TerrainSize_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.VegQuantity#="+Str(t.visuals.VegQuantity_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.VegWidth#="+Str(t.visuals.VegWidth_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.VegHeight#="+Str(t.visuals.VegHeight_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.skyindex="+Str(t.visuals.skyindex);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.sky$="+t.visuals.sky_s;
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.terrainindex="+Str(t.visuals.terrainindex);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.terrain$="+t.visuals.terrain_s;
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.vegetationindex="+Str(t.visuals.vegetationindex);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.vegetation$="+t.visuals.vegetation_s;
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.reflectionmode="+Str(t.visuals.reflectionmode);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.shadowmode="+Str(t.visuals.shadowmode);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.bloommode="+Str(t.visuals.bloommode);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.lightraymode="+Str(t.visuals.lightraymode);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.vegetationmode="+Str(t.visuals.vegetationmode);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.occlusionvalue="+Str(t.visuals.occlusionvalue);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.debugvisualsmode="+Str(t.visuals.debugvisualsmode);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.VignetteRadius#="+Str(t.visuals.VignetteRadius_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.VignetteIntensity#="+Str(t.visuals.VignetteIntensity_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.MotionDistance#="+Str(t.visuals.MotionDistance_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.MotionIntensity#="+Str(t.visuals.MotionIntensity_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.DepthOfFieldDistance#="+Str(t.visuals.DepthOfFieldDistance_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.DepthOfFieldIntensity#="+Str(t.visuals.DepthOfFieldIntensity_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.LightrayLength="+Str(t.visuals.LightrayLength_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.LightrayQuality="+Str(t.visuals.LightrayQuality_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.LightrayDecay="+Str(t.visuals.LightrayDecay_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.SAORadius="+Str(t.visuals.SAORadius_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.SAOIntensity="+Str(t.visuals.SAOIntensity_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork +"visuals.SAOQuality="+Str(t.visuals.SAOQuality_f);
	WriteString (  1, t.strwork.Get() );
	t.strwork = ""; t.strwork = t.strwork + "visuals.LensFlare=" + Str(t.visuals.LensFlare_f);
	WriteString (1, t.strwork.Get());
	#ifdef WICKEDENGINE
	t.strwork = ""; t.strwork = t.strwork + "visuals.Exposure=" + Str(t.visuals.fExposure);
	WriteString (1, t.strwork.Get());
	#endif
	//New Water Settings
	t.strwork = ""; t.strwork = t.strwork + "visuals.Waterheight=" + Str(g.gdefaultwaterheight);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.Waterred=" + Str(t.visuals.WaterRed_f);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.Waterblue=" + Str(t.visuals.WaterBlue_f);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.Watergreen=" + Str(t.visuals.WaterGreen_f);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterWaveIntensity=" + Str(t.visuals.WaterWaveIntensity_f);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterTransparancy=" + Str(t.visuals.WaterTransparancy_f);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterSparkleCol=" + Str(t.visuals.WaterReflectionSparkleIntensity);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterReflection=" + Str(t.visuals.WaterReflection_f);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterFlowDirectionX=" + Str(t.visuals.WaterFlowDirectionX);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterFlowDirectionY=" + Str(t.visuals.WaterFlowDirectionY);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterDistortionWaves=" + Str(t.visuals.WaterDistortionWaves);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterSpeed1=" + Str(t.visuals.WaterSpeed1);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterFlowSpeed=" + Str(t.visuals.WaterFlowSpeed);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.Weather=" + Str(t.visuals.iEnvironmentWeather);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.SunIntensity=" + Str(t.visuals.SunIntensity_f);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.SunRed=" + Str(t.visuals.SunRed_f);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SunGreen=" + Str(t.visuals.SunGreen_f);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SunBlue=" + Str(t.visuals.SunBlue_f);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.ZenithRed=" + Str(t.visuals.ZenithRed_f);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.ZenithGreen=" + Str(t.visuals.ZenithGreen_f);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.ZenithBlue=" + Str(t.visuals.ZenithBlue_f);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.SunShadowBias=" + Str(t.visuals.fSunShadowBias);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.ColorGrading=" + Str(t.visuals.bColorGrading);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.ColorGradingLUT=" + t.visuals.ColorGradingLUT;
	WriteString(1, t.strwork.Get());
	
	t.strwork = ""; t.strwork = t.strwork + "visuals.LevelVSyncEnabled=" + Str(t.visuals.bLevelVSyncEnabled);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.OcclusionCulling=" + Str(t.visuals.bOcclusionCulling);
	WriteString(1, t.strwork.Get());
	
	t.strwork = ""; t.strwork = t.strwork + "visuals.EnableTerrainChunkCulling=" + Str(t.visuals.bEnableTerrainChunkCulling);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.EnablePointShadowCulling=" + Str(t.visuals.bEnablePointShadowCulling);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.EnableSpotShadowCulling=" + Str(t.visuals.bEnableSpotShadowCulling);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.EnableObjectCulling=" + Str(t.visuals.bEnableObjectCulling);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.EnableAnimationCulling=" + Str(t.visuals.bEnableAnimationCulling);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.LODMultiplier=" + Str(t.visuals.fLODMultiplier);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.ThreadedPhysics=" + Str(t.visuals.bThreadedPhysics);
	WriteString(1, t.strwork.Get());
	

	t.strwork = ""; t.strwork = t.strwork + "visuals.Enable30FpsAnimations=" + Str(t.visuals.bEnable30FpsAnimations);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.ShadowsLowestLOD=" + Str(t.visuals.bShadowsLowestLOD);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.ProbesLowestLOD=" + Str(t.visuals.bProbesLowestLOD);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.RaycastLowestLOD=" + Str(t.visuals.bRaycastLowestLOD);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.PhysicsLowestLOD=" + Str(t.visuals.bPhysicsLowestLOD);
	WriteString(1, t.strwork.Get());
	
	t.strwork = ""; t.strwork = t.strwork + "visuals.ReflectionsLowestLOD=" + Str(t.visuals.bReflectionsLowestLOD);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.DelayedShadows=" + Str(t.visuals.g_bDelayedShadows);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.DelayedShadowsLaptop=" + Str(t.visuals.g_bDelayedShadowsLaptop);
	WriteString(1, t.strwork.Get());


	t.strwork = ""; t.strwork = t.strwork + "visuals.BloomEnabled=" + Str(t.visuals.bBloomEnabled);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.BloomThreshold=" + Str(t.visuals.fsetBloomThreshold);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.ApparentSize=" + Str(t.visuals.ApparentSize);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.BloomStrength=" + Str(t.visuals.fsetBloomStrength);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SSREnabled=" + Str(t.visuals.bSSREnabled);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.ReflectionsEnabled=" + Str(t.visuals.bReflectionsEnabled);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.FXAAEnabled=" + Str(t.visuals.bFXAAEnabled);
	WriteString(1, t.strwork.Get());


	t.strwork = ""; t.strwork = t.strwork + "visuals.DOF=" + Str(t.visuals.bDOF);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.DOFStrength=" + Str(t.visuals.fDOFStrength);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.DOFApertureSize=" + Str(t.visuals.fDOFApertureSize);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.DOFFocalLength=" + Str(t.visuals.fDOFFocalLength);
	WriteString(1, t.strwork.Get());


	t.strwork = ""; t.strwork = t.strwork + "visuals.Tessellation=" + Str(t.visuals.bTessellation);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.LightShafts=" + Str(t.visuals.bLightShafts);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.LensFlare=" + Str(t.visuals.bLensFlare);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.Exposure=" + Str(t.visuals.fExposure);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.AutoExposure=" + Str(t.visuals.bAutoExposure);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.fAutoExposureRate=" + Str(t.visuals.fAutoExposureRate);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.fAutoExposureKey=" + Str(t.visuals.fAutoExposureKey);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.Gamma=" + Str(t.visuals.fGamma);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.MSAASampleCount=" + Str(t.visuals.iMSAASampleCount);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.FSRMode=" + Str(t.visuals.iFSRMode);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.FSRSharpness=" + Str(t.visuals.fFSRSharpness);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.MSAO=" + Str(t.visuals.iMSAO);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.MSAOPower=" + Str(t.visuals.fMSAOPower);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.SkyCloudiness=" + Str(t.visuals.SkyCloudiness);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SkyCloudScale=" + Str(t.visuals.SkyCloudHeight);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SkyCloudSpeed=" + Str(t.visuals.SkyCloudSpeed);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SkyCloudCoverage=" + Str(t.visuals.SkyCloudCoverage);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SkyCloudHeight=" + Str(t.visuals.SkyCloudHeight);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SkyCloudThickness=" + Str(t.visuals.SkyCloudThickness);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.ShadowSpotCascadeResolution=" + Str(t.visuals.iShadowSpotCascadeResolution);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.ShadowSpotResolution=" + Str(t.visuals.iShadowSpotResolution);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.ShadowPointResolution=" + Str(t.visuals.iShadowPointResolution);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.iShadowPointMax=" + Str(t.visuals.iShadowPointMax);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.iShadowSpotMax=" + Str(t.visuals.iShadowSpotMax);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.bTransparentShadows=" + Str(t.visuals.bTransparentShadows);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.SunAngleX=" + Str(t.visuals.SunAngleX);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SunAngleY=" + Str(t.visuals.SunAngleY);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SunAngleZ=" + Str(t.visuals.SunAngleZ);
	WriteString(1, t.strwork.Get());


	t.strwork = ""; t.strwork = t.strwork + "visuals.ShadowFarPlane=" + Str(t.visuals.fShadowFarPlane);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterEnable=" + Str(t.visuals.bWaterEnable);
	WriteString(1, t.strwork.Get());


	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterWaveAmplitude=" + Str(t.visuals.fWaterWaveAmplitude);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterPatchLength=" + Str(t.visuals.fWaterPatchLength);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterChoppyScale=" + Str(t.visuals.fWaterChoppyScale);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterWindDependency=" + Str(t.visuals.fWaterWindDependency);
	WriteString(1, t.strwork.Get());


	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterFogMinDist=" + Str(t.visuals.WaterFogMinDist);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterFogMaxDist=" + Str(t.visuals.WaterFogMaxDist);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WaterFogMinAmount=" + Str(t.visuals.WaterFogMinAmount);
	WriteString(1, t.strwork.Get());


	t.strwork = ""; t.strwork = t.strwork + "visuals.TimeOfday=" + Str(t.visuals.iTimeOfday);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.Simulate24Hours=" + Str(t.visuals.bSimulate24Hours);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.TimeSpeed=" + Str(t.visuals.fTimeSpeed);
	WriteString(1, t.strwork.Get());


	t.strwork = ""; t.strwork = t.strwork + "visuals.WeatherIntensity=" + Str(t.visuals.fWeatherIntensity);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WeatherLighting=" + Str(t.visuals.fWeatherLighting);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WeatherThunder=" + Str(t.visuals.fWeatherThunder);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.WeatherWind=" + Str(t.visuals.fWeatherWind);
	WriteString(1, t.strwork.Get());


	t.strwork = ""; t.strwork = t.strwork + "visuals.bPPSnow=" + Str(t.visuals.bPPSnow);
	WriteString(1, t.strwork.Get());


	t.strwork = ""; t.strwork = t.strwork + "visuals.bPPSnow=" + Str(t.visuals.bPPSnow);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.voxel_steps=" + Str(t.visuals.voxel_steps);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.pp_size=" + Str(t.visuals.pp_size);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.pp_alpha=" + Str(t.visuals.pp_alpha);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.wind_direction_x=" + Str(t.visuals.wind_direction_x);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.wind_direction_x=" + Str(t.visuals.wind_direction_x);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.wind_direction_y=" + Str(t.visuals.wind_direction_y);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.wind_direction_z=" + Str(t.visuals.wind_direction_z);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.wind_speed=" + Str(t.visuals.wind_speed);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.wind_randomness=" + Str(t.visuals.wind_randomness);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.bpp_disable_indoor=" + Str(t.visuals.bpp_disable_indoor);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.tree_wind=" + Str(t.visuals.tree_wind);
	WriteString(1, t.strwork.Get());
	

	t.strwork = ""; t.strwork = t.strwork + "visuals.LevelDifficulty=" + Str(t.visuals.fLevelDifficulty);
	WriteString(1, t.strwork.Get());
	
	for (int iL = 0; iL < 32; iL++) 
	{
		if (t.visuals.sTerrainTextures[iL] != "")
		{
			t.strwork = ""; t.strwork = t.strwork + "visuals.TerrainTextures" + cStr(iL) + "=" + cStr(t.visuals.sTerrainTextures[iL]);
			WriteString(1, t.strwork.Get());
			t.strwork = ""; t.strwork = t.strwork + "visuals.TerrainTexturesName" + cStr(iL) + "=" + cStr(t.visuals.sTerrainTexturesName[iL]);
			WriteString(1, t.strwork.Get());
		}
	}
	for (int iL = 0; iL < 128; iL++) 
	{
		if (t.visuals.sGrassTextures[iL] != "")
		{
			t.strwork = ""; t.strwork = t.strwork + "visuals.GrassTextures" + cStr(iL) + "=" + cStr(t.visuals.sGrassTextures[iL]);
			WriteString(1, t.strwork.Get());
			t.strwork = ""; t.strwork = t.strwork + "visuals.GrassTexturesName" + cStr(iL) + "=" + cStr(t.visuals.sGrassTexturesName[iL]);
			WriteString(1, t.strwork.Get());
		}
		if (t.visuals.sFactionName[iL] != "")
		{
			t.strwork = ""; t.strwork = t.strwork + "visuals.FactionName" + cStr(iL) + "=" + cStr(t.visuals.sFactionName[iL]);
			WriteString(1, t.strwork.Get());
		}
		
	}

	t.strwork = ""; t.strwork = t.strwork + "visuals.EndableAmbientMusicTrack=" + Str(t.visuals.bEndableAmbientMusicTrack);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.AmbientMusicTrack=" + cStr(t.visuals.sAmbientMusicTrack);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.AmbientMusicTrackVolume=" + Str(t.visuals.iAmbientMusicTrackVolume);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.EnableCombatMusicTrack=" + Str(t.visuals.bEnableCombatMusicTrack);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.CombatMusicTrack=" + cStr(t.visuals.sCombatMusicTrack);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.CombatMusicTrackVolume=" + Str(t.visuals.iCombatMusicTrackVolume);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.RainEnabled=" + Str(t.visuals.bRainEnabled);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.RainSpeedX=" + Str(t.visuals.fRainSpeedX);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.RainSpeedY=" + Str(t.visuals.fRainSpeedY);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.RainOpacity=" + Str(t.visuals.fRainOpacity);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.RainScaleX=" + Str(t.visuals.fRainScaleX);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.RainScaleY=" + Str(t.visuals.fRainScaleY);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.RainRefreactionScale=" + Str(t.visuals.fRainRefreactionScale);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.SnowEnabled=" + Str(t.visuals.bSnowEnabled);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SnowLayers=" + Str(t.visuals.fSnowLayers);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SnowDepth=" + Str(t.visuals.fSnowDepth);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SnowWind=" + Str(t.visuals.fSnowWind);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SnowSpeed=" + Str(t.visuals.fSnowSpeed);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.SnowOpacity=" + Str(t.visuals.fSnowOpacity);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.EndableTreeDrawing=" + Str(t.visuals.bEndableTreeDrawing);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.EndableGrassDrawing=" + Str(t.visuals.bEndableGrassDrawing);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.EndableTerrainDrawing=" + Str(t.visuals.bEndableTerrainDrawing);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.HeightmapWidth=" + Str(t.visuals.iHeightmapWidth);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.HeightmapHeight=" + Str(t.visuals.iHeightmapHeight);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.DisableSkybox=" + Str(t.visuals.bDisableSkybox);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.GrassMinHeight=" + Str(GGGrass::gggrass_global_params.min_height);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.GrassMaxHeight=" + Str(GGGrass::gggrass_global_params.max_height);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.GrassStartHeightUnderwater=" + Str(GGGrass::gggrass_global_params.min_height_underwater);
	WriteString(1, t.strwork.Get());
	t.strwork = ""; t.strwork = t.strwork + "visuals.GrassEndHeightUnderwater=" + Str(GGGrass::gggrass_global_params.max_height_underwater);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.EnvProbeResolution=" + Str(t.visuals.iEnvProbeResolution);
	WriteString(1, t.strwork.Get());

	t.strwork = ""; t.strwork = t.strwork + "visuals.NewPerformancePresets=" + Str(t.visuals.newperformancepresets);
	WriteString(1, t.strwork.Get());

	WriteString (  1, "" );

	CloseFile (  1 );
	return;
}

void visuals_load ( void )
{
	//  Record previous visuals settings (see below)
	t.defaultvisuals = t.visuals;

	//  Reset newer visual values that may not exist in older visuals.ini files
	t.visuals.VignetteRadius_f=0.5;
	t.visuals.VignetteIntensity_f=0;
	t.visuals.MotionDistance_f=0.5;
	t.visuals.MotionIntensity_f=0;
	t.visuals.DepthOfFieldDistance_f=0.5;
	t.visuals.DepthOfFieldIntensity_f=0;
	t.visuals.LightrayLength_f=0.55f;
	t.visuals.LightrayQuality_f=20.0f;
	t.visuals.LightrayDecay_f=0.8f;
	t.visuals.SAORadius_f=0.5f;
	t.visuals.SAOIntensity_f=0.5f;
	t.visuals.SAOQuality_f=1.0f;
	t.visuals.LensFlare_f=0.5f;
	//water settings
	visuals_water_reset();

	//Wicked defaults.
	t.visuals.SkyIntensity_f = 1.0f;
	t.visuals.SunIntensity_f = 9.0f;
	t.visuals.SunRed_f = 255.0f;
	t.visuals.SunGreen_f = 255.0f;
	t.visuals.SunBlue_f = 255.0f;
	t.visuals.ZenithRed_f = 104.0f;
	t.visuals.ZenithGreen_f = 104.0f;
	t.visuals.ZenithBlue_f = 104.0f;
	t.visuals.fSunShadowBias = 0.00005f;
	t.visuals.bColorGrading = false;
	t.visuals.ColorGradingLUT = "";
	t.visuals.bBloomEnabled = true;
	t.visuals.bLevelVSyncEnabled = true;
	t.visuals.bOcclusionCulling = false;
	t.visuals.bEnableTerrainChunkCulling = false;
	t.visuals.bEnablePointShadowCulling = false;
	t.visuals.bEnableSpotShadowCulling = false;
	t.visuals.bEnableObjectCulling = false;
	t.visuals.bEnableAnimationCulling = false;
	t.visuals.fLODMultiplier = 1.0f;

	t.visuals.bEnable30FpsAnimations = false;

	t.visuals.bPhysicsLowestLOD = false;
	t.visuals.bShadowsLowestLOD = false;
	t.visuals.bProbesLowestLOD = false;
	t.visuals.bRaycastLowestLOD = false;
	t.visuals.bThreadedPhysics = false;
	t.visuals.bReflectionsLowestLOD = false;

	t.visuals.g_bDelayedShadows = false;
	t.visuals.g_bDelayedShadowsLaptop = false;

	t.visuals.fsetBloomThreshold = 2.0f;
	t.visuals.ApparentSize = 0.000008f;
	t.visuals.fsetBloomStrength = 1.0f;
	t.visuals.bSSREnabled = false;
	t.visuals.bReflectionsEnabled = true;
	t.visuals.bFXAAEnabled = false;

	t.visuals.bDOF = false;
	t.visuals.fDOFStrength = 10.0f;
	t.visuals.fDOFApertureSize = 0.2f;
	t.visuals.fDOFFocalLength = 300.0f;

	t.visuals.bTessellation = true;
	t.visuals.bLightShafts = true;
	t.visuals.bLensFlare = true;
	t.visuals.bAutoExposure = true;
	t.visuals.fAutoExposureRate = 0.5f;
	t.visuals.fAutoExposureKey = 0.15f;
	t.visuals.fExposure = 1.0f;
	t.visuals.fGamma = 2.2f;
	t.visuals.iMSAASampleCount = 1;
	t.visuals.iFSRMode = 0;
	t.visuals.fFSRSharpness = 0.2f;

	t.visuals.iMSAO = 1;
	t.visuals.fMSAOPower = 1.0;

	t.visuals.SkyCloudiness = 0.68f;
	t.visuals.SkyCloudCoverage = 1.38f;
	t.visuals.SkyCloudHeight = 60000.0f;
	t.visuals.SkyCloudThickness = 98000.0f;
	t.visuals.SkyCloudSpeed = 3.0f;

	t.visuals.iShadowSpotCascadeResolution = 2048;
	t.visuals.iShadowSpotResolution = 512;
	t.visuals.iShadowPointResolution = 512;
	t.visuals.iShadowPointMax = 16;
	t.visuals.iShadowSpotMax = 8;
	t.visuals.bTransparentShadows = false;

	t.visuals.fShadowFarPlane = DEFAULT_FAR_PLANE;
	t.visuals.bWaterEnable = true;
	t.visuals.fWaterWaveAmplitude = 20.0f;
	t.visuals.fWaterPatchLength = 40.0f;
	t.visuals.fWaterChoppyScale = 0.0f;
	t.visuals.fWaterWindDependency = 0.0f;
	t.visuals.WaterSpeed1 = 0.06f;

	t.visuals.WaterFogMaxDist = 11500.0f;
	t.visuals.WaterFogMinDist = 0.0f;
	t.visuals.WaterFogMinAmount = 0.25f;

	t.visuals.iTimeOfday = 1;
	visuals_calcsunanglefromtimeofday(t.visuals.iTimeOfday, &t.visuals.SunAngleX, &t.visuals.SunAngleY, &t.visuals.SunAngleZ);

	t.visuals.bSimulate24Hours = false;
	t.visuals.fTimeSpeed = 1.0;

	t.visuals.fWeatherIntensity = 50.0;
	t.visuals.fWeatherLighting = 0.0;
	t.visuals.fWeatherThunder = 0.0;
	t.visuals.fWeatherWind = 50.0;

	t.visuals.bPPSnow = false;
	t.visuals.voxel_steps = 16.0f;
	t.visuals.pp_size = 1.0f;
	t.visuals.pp_alpha = 1.0f;
	t.visuals.wind_direction_x = 1.0f;
	t.visuals.wind_direction_y = -7.0f;
	t.visuals.wind_direction_z = 1.0f;
	t.visuals.wind_speed = 1.0f;
	t.visuals.wind_randomness = 1.0f;
	t.visuals.bpp_disable_indoor = true;
	t.visuals.tree_wind = 0.0f;
	

	t.visuals.fLevelDifficulty = 50.0;

	for (int iL = 0; iL < 128; iL++) 
	{
		t.visuals.sGrassTextures[iL] = "";
		t.visuals.sGrassTexturesName[iL] = "";
		extern bool iDeleteAllGrassTextures;
		iDeleteAllGrassTextures = true;
		t.visuals.sFactionName[iL] = "";
	}

	t.visuals.bEndableAmbientMusicTrack = false;
	t.visuals.sAmbientMusicTrack = "";
	t.visuals.iAmbientMusicTrackVolume = 100;
	t.visuals.bEnableCombatMusicTrack = false;
	t.visuals.sCombatMusicTrack = "";
	t.visuals.iCombatMusicTrackVolume = 100;

	t.visuals.bEndableTreeDrawing = false;
	t.visuals.bEndableGrassDrawing = false;
	t.visuals.bEndableTerrainDrawing = true;

	t.visuals.iHeightmapWidth = 0;
	t.visuals.iHeightmapHeight = 0;

	t.visuals.bDisableSkybox = false;

	t.visuals.bRainEnabled = false;
	t.visuals.fRainSpeedX = -1.5;
	t.visuals.fRainSpeedY = -18.0;
	t.visuals.fRainOpacity = 0.0;
	t.visuals.fRainScaleX = 2.0;
	t.visuals.fRainScaleY = 2.0;
	t.visuals.fRainRefreactionScale = 0.005;

	t.visuals.bSnowEnabled = false;
	t.visuals.fSnowLayers = 15.0;
	t.visuals.fSnowDepth = 0.5;
	t.visuals.fSnowWind = 0.5;
	t.visuals.fSnowSpeed = 2.0;
	t.visuals.fSnowOpacity = 1.0;
	t.visuals.fSnowOffset = 0.0;

	// special flag to know when new performance defaults applied
	t.visuals.newperformancepresets = 0;

	// load all visual settings
	bool bVisualFileFromLevel = false;
	if ( g.fpscrootdir_s == "" ) 
	{
		t.visfile_s="visuals.ini";
		bVisualFileFromLevel = true;
	}
	else
	{
		#ifdef WICKEDENGINE
		char cFullWritePath[MAX_PATH] = "";
		GG_GetRealPath(cFullWritePath, 0);
		t.visfile_s = cstr(cFullWritePath) + "visuals.ini";
		if (FileExist(t.visfile_s.Get()) == 0)
		{
			//PE: No file yet in write folder , take the original.
			t.visfile_s = g.fpscrootdir_s + "\\visuals.ini";
		}
		#else
		t.visfile_s=g.fpscrootdir_s+"\\visuals.ini";
		#endif
	}
	if ( FileExist(t.visfile_s.Get()) == 1 ) 
	{
		OpenToRead (  1,t.visfile_s.Get() );
		do
		{
			t.tline_s = ReadString ( 1 );
			for ( t.n = 1 ; t.n<=  Len(t.tline_s.Get()); t.n++ )
			{
				if (  cstr(Mid(t.tline_s.Get(),t.n)) == "=" ) 
				{
					t.tfield_s=Left(t.tline_s.Get(),t.n-1);
					t.tvalue_s=Right(t.tline_s.Get(),Len(t.tline_s.Get())-Len(t.tfield_s.Get())-1);
				}
			}
			t.try_s = "visuals.shaderlevels.terrain" ; if (  t.tfield_s == t.try_s  )  t.visuals.shaderlevels.terrain = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.shaderlevels.entities" ;
			if (  t.tfield_s == t.try_s  )
				t.visuals.shaderlevels.entities = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.shaderlevels.vegetation" ; if (  t.tfield_s == t.try_s  )  t.visuals.shaderlevels.vegetation = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.shaderlevels.lighting" ; if (  t.tfield_s == t.try_s  )  t.visuals.shaderlevels.lighting = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.mode" ; if (  t.tfield_s == t.try_s  )  t.visuals.mode = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.PostContrast#" ; if (  t.tfield_s == t.try_s  )  t.visuals.PostContrast_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.PostBrightness#" ; if (  t.tfield_s == t.try_s  )  t.visuals.PostBrightness_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.FogNearest#" ;
			if (t.tfield_s == t.try_s)
			{
				//PE: @Paul we need this so users can change the settings.
				//PE: You can change the defaults in "visual.ini" this is what the editor use :)
				//PE: Removed t.visuals.FogNearest_f = 360000 , t.visuals.FogDistance_f = 1000000.0f.

				t.visuals.FogNearest_f = ValF(t.tvalue_s.Get());
			}
			t.try_s = "visuals.FogDistance#" ; if (  t.tfield_s == t.try_s  )  t.visuals.FogDistance_f = ValF(t.tvalue_s.Get()); //PE: Needed ?
			t.try_s = "visuals.FogR#" ; if (  t.tfield_s == t.try_s  )  t.visuals.FogR_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.FogG#" ; if (  t.tfield_s == t.try_s  )  t.visuals.FogG_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.FogB#" ; if (  t.tfield_s == t.try_s  )  t.visuals.FogB_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.FogA#" ;
			if (t.tfield_s == t.try_s)
			{
				t.visuals.FogA_f = ValF(t.tvalue_s.Get());
				#ifdef WICKEDENGINE
				if (t.visuals.FogA_f > 2.0) //PE: Wicked used for height.
					t.visuals.FogA_f = 0;
				#endif
			}
			t.try_s = "visuals.AmbienceIntensity#" ; if (  t.tfield_s == t.try_s  )  t.visuals.AmbienceIntensity_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.AmbienceRed#" ; if (  t.tfield_s == t.try_s  )
				t.visuals.AmbienceRed_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.AmbienceGeen#" ; if (  t.tfield_s == t.try_s  )
				t.visuals.AmbienceGreen_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.AmbienceBlue#" ; if (  t.tfield_s == t.try_s  )
				t.visuals.AmbienceBlue_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SurfaceIntensity#" ; if (  t.tfield_s == t.try_s  )  t.visuals.SurfaceIntensity_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SurfaceRed#" ; if (  t.tfield_s == t.try_s  )  t.visuals.SurfaceRed_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SurfaceGreen#" ; if (  t.tfield_s == t.try_s  )  t.visuals.SurfaceGreen_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SurfaceBlue#" ; if (  t.tfield_s == t.try_s  )  t.visuals.SurfaceBlue_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SurfaceSunFactor#" ; if (  t.tfield_s == t.try_s  )  t.visuals.SurfaceSunFactor_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.Specular#" ; if (  t.tfield_s == t.try_s  )  t.visuals.Specular_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.DistanceTransitionStart#" ; if (  t.tfield_s == t.try_s  )  t.visuals.DistanceTransitionStart_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.DistanceTransitionRange#" ; if (  t.tfield_s == t.try_s  )  t.visuals.DistanceTransitionRange_f = ValF(t.tvalue_s.Get());
			//t.try_s = "visuals.CameraNEAR#" ; if (  t.tfield_s == t.try_s  )  t.visuals.CameraNEAR_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.CameraFAR#" ; if (  t.tfield_s == t.try_s  )  t.visuals.CameraFAR_f = ValF(t.tvalue_s.Get());
			//  20032015 = 021 - Don't use FOV for multiplayer
			if (  t.game.runasmultiplayer  ==  0 ) 
			{
				t.try_s = "visuals.CameraFOV#" ; if (  t.tfield_s == t.try_s  )
				t.visuals.CameraFOV_f = ValF(t.tvalue_s.Get());
				t.try_s = "visuals.WeaponFOV#" ; if (  t.tfield_s == t.try_s  )  t.visuals.WeaponFOV_f = ValF(t.tvalue_s.Get());
				t.try_s = "visuals.CameraFOVZoomed#" ; if (  t.tfield_s == t.try_s  )  t.visuals.CameraFOVZoomed_f = ValF(t.tvalue_s.Get());
			}
			t.try_s = "visuals.TerrainLOD1#" ; if (  t.tfield_s == t.try_s  )  t.visuals.TerrainLOD1_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.TerrainLOD2#" ; if (  t.tfield_s == t.try_s  )  t.visuals.TerrainLOD2_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.TerrainLOD3#" ; if (  t.tfield_s == t.try_s  )  t.visuals.TerrainLOD3_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.TerrainSize#" ; if (  t.tfield_s == t.try_s  )  t.visuals.TerrainSize_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.VegQuantity#" ; if (  t.tfield_s == t.try_s  )  t.visuals.VegQuantity_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.VegWidth#" ; if (  t.tfield_s == t.try_s  )  t.visuals.VegWidth_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.VegHeight#" ; if (  t.tfield_s == t.try_s  )  t.visuals.VegHeight_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.skyindex" ; if (  t.tfield_s == t.try_s  )  t.visuals.skyindex = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.sky$" ; if (  t.tfield_s == t.try_s  )  t.visuals.sky_s = t.tvalue_s;
			t.try_s = "visuals.terrainindex" ; if (  t.tfield_s == t.try_s  )  t.visuals.terrainindex = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.terrain$" ; if (  t.tfield_s == t.try_s  )  t.visuals.terrain_s = t.tvalue_s;
			t.try_s = "visuals.vegetationindex" ; if (  t.tfield_s == t.try_s  )  t.visuals.vegetationindex = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.vegetation$" ; if (  t.tfield_s == t.try_s  )  t.visuals.vegetation_s = t.tvalue_s;
			t.try_s = "visuals.reflectionmode" ; if (  t.tfield_s == t.try_s  )  t.visuals.reflectionmode = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.shadowmode" ; if (  t.tfield_s == t.try_s  )  t.visuals.shadowmode = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.bloommode" ; if (  t.tfield_s == t.try_s  )  t.visuals.bloommode = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.lightraymode" ; if (  t.tfield_s == t.try_s  )  t.visuals.lightraymode = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.vegetationmode" ; if (  t.tfield_s == t.try_s  )
				t.visuals.vegetationmode = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.occlusionvalue" ; if (  t.tfield_s == t.try_s  )  t.visuals.occlusionvalue = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.debugvisualsmode" ; if (  t.tfield_s == t.try_s  )  t.visuals.debugvisualsmode = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.VignetteRadius#" ; if (  t.tfield_s == t.try_s  )  t.visuals.VignetteRadius_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.VignetteIntensity#" ; if (  t.tfield_s == t.try_s  )  t.visuals.VignetteIntensity_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.MotionDistance#" ; if (  t.tfield_s == t.try_s  )  t.visuals.MotionDistance_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.MotionIntensity#" ; if (  t.tfield_s == t.try_s  )  t.visuals.MotionIntensity_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.DepthOfFieldDistance#" ; if (  t.tfield_s == t.try_s  )  t.visuals.DepthOfFieldDistance_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.DepthOfFieldIntensity#" ; if (  t.tfield_s == t.try_s  )  t.visuals.DepthOfFieldIntensity_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.LightrayLength" ; if (  t.tfield_s == t.try_s  )  t.visuals.LightrayLength_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.LightrayQuality" ; if (  t.tfield_s == t.try_s  )  t.visuals.LightrayQuality_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.LightrayDecay" ; if (  t.tfield_s == t.try_s  )  t.visuals.LightrayDecay_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SAORadius" ; if (  t.tfield_s == t.try_s  )  t.visuals.SAORadius_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SAOIntensity" ; if (  t.tfield_s == t.try_s  )  t.visuals.SAOIntensity_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SAOQuality" ; if (  t.tfield_s == t.try_s  )  t.visuals.SAOQuality_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.LensFlare"; if (t.tfield_s == t.try_s)  t.visuals.LensFlare_f = ValF(t.tvalue_s.Get());
			#ifdef WICKEDENGINE
			t.try_s = "visuals.Exposure"; if (t.tfield_s == t.try_s)  t.visuals.fExposure = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.fAutoExposureRate"; if (t.tfield_s == t.try_s)  t.visuals.fAutoExposureRate = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.fAutoExposureKey"; if (t.tfield_s == t.try_s)  t.visuals.fAutoExposureKey = ValF(t.tvalue_s.Get());
			#endif
			//new water settings
			t.try_s = "visuals.Waterheight"; if (t.tfield_s == t.try_s)  g.gdefaultwaterheight = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.Waterred"; if (t.tfield_s == t.try_s)  t.visuals.WaterRed_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.Waterblue"; if (t.tfield_s == t.try_s)  t.visuals.WaterBlue_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.Watergreen"; if (t.tfield_s == t.try_s)  t.visuals.WaterGreen_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterWaveIntensity"; if (t.tfield_s == t.try_s)  t.visuals.WaterWaveIntensity_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterTransparancy"; if (t.tfield_s == t.try_s)  t.visuals.WaterTransparancy_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterSparkleCol"; if (t.tfield_s == t.try_s)  t.visuals.WaterReflectionSparkleIntensity = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterReflection"; if (t.tfield_s == t.try_s)  t.visuals.WaterReflection_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterFlowDirectionX"; if (t.tfield_s == t.try_s)  t.visuals.WaterFlowDirectionX = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterFlowDirectionY"; if (t.tfield_s == t.try_s)  t.visuals.WaterFlowDirectionY = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterDistortionWaves"; if (t.tfield_s == t.try_s)  t.visuals.WaterDistortionWaves = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterSpeed1"; if (t.tfield_s == t.try_s)  t.visuals.WaterSpeed1 = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterFlowSpeed"; if (t.tfield_s == t.try_s)  t.visuals.WaterFlowSpeed = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.Weather"; if (t.tfield_s == t.try_s)  t.visuals.iEnvironmentWeather = ValF(t.tvalue_s.Get());

			//PE: Why was SunIntensity disabled (commented out) ? , re enabled it so it can be saved/loaded with levels.
			t.try_s = "visuals.SunIntensity"; if (t.tfield_s == t.try_s)  t.visuals.SunIntensity_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SunRed"; if (t.tfield_s == t.try_s)  t.visuals.SunRed_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SunGreen"; if (t.tfield_s == t.try_s)  t.visuals.SunGreen_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SunBlue"; if (t.tfield_s == t.try_s)  t.visuals.SunBlue_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.ZenithRed"; if (t.tfield_s == t.try_s)  t.visuals.ZenithRed_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.ZenithGreen"; if (t.tfield_s == t.try_s)  t.visuals.ZenithGreen_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.ZenithBlue"; if (t.tfield_s == t.try_s)  t.visuals.ZenithBlue_f = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SunShadowBias"; if (t.tfield_s == t.try_s)  t.visuals.fSunShadowBias = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.ColorGrading"; if (t.tfield_s == t.try_s)  t.visuals.bColorGrading = ValF(t.tvalue_s.Get());
		
			t.try_s = "visuals.ColorGradingLUT"; if (t.tfield_s == t.try_s)
			{
				//PE: Change saved lut locations.
				std::string sString = t.tvalue_s.Get();
				replaceAll(sString, "imagebank\\", "editors\\");
				t.visuals.ColorGradingLUT = sString.c_str();
			}

			t.try_s = "visuals.LevelVSyncEnabled"; if (t.tfield_s == t.try_s)  t.visuals.bLevelVSyncEnabled = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.OcclusionCulling"; if (t.tfield_s == t.try_s)  t.visuals.bOcclusionCulling = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.EnableTerrainChunkCulling"; if (t.tfield_s == t.try_s)  t.visuals.bEnableTerrainChunkCulling = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.EnablePointShadowCulling"; if (t.tfield_s == t.try_s)  t.visuals.bEnablePointShadowCulling = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.EnableSpotShadowCulling"; if (t.tfield_s == t.try_s)  t.visuals.bEnableSpotShadowCulling = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.EnableObjectCulling";
			if (t.tfield_s == t.try_s)
				t.visuals.bEnableObjectCulling = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.EnableAnimationCulling"; if (t.tfield_s == t.try_s)  t.visuals.bEnableAnimationCulling = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.LODMultiplier"; if (t.tfield_s == t.try_s)  t.visuals.fLODMultiplier = ValF(t.tvalue_s.Get());
			
			t.try_s = "visuals.ThreadedPhysics"; if (t.tfield_s == t.try_s)  t.visuals.bThreadedPhysics = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.Enable30FpsAnimations"; if (t.tfield_s == t.try_s)  t.visuals.bEnable30FpsAnimations = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.PhysicsLowestLOD"; if (t.tfield_s == t.try_s)  t.visuals.bPhysicsLowestLOD = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.ShadowsLowestLOD"; if (t.tfield_s == t.try_s)  t.visuals.bShadowsLowestLOD = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.ProbesLowestLOD"; if (t.tfield_s == t.try_s)  t.visuals.bProbesLowestLOD = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.RaycastLowestLOD"; if (t.tfield_s == t.try_s)  t.visuals.bRaycastLowestLOD = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.ReflectionsLowestLOD"; if (t.tfield_s == t.try_s)  t.visuals.bReflectionsLowestLOD = ValF(t.tvalue_s.Get());


			t.try_s = "visuals.DelayedShadows"; if (t.tfield_s == t.try_s)  t.visuals.g_bDelayedShadows = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.DelayedShadowsLaptop"; if (t.tfield_s == t.try_s)  t.visuals.g_bDelayedShadowsLaptop = ValF(t.tvalue_s.Get());


			t.try_s = "visuals.BloomEnabled"; if (t.tfield_s == t.try_s)  t.visuals.bBloomEnabled = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.BloomThreshold"; if (t.tfield_s == t.try_s)  t.visuals.fsetBloomThreshold = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.ApparentSize"; if (t.tfield_s == t.try_s)  t.visuals.ApparentSize = ValF(t.tvalue_s.Get());
			
			t.try_s = "visuals.BloomStrength"; if (t.tfield_s == t.try_s)  t.visuals.fsetBloomStrength = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.SSREnabled"; if (t.tfield_s == t.try_s)	t.visuals.bSSREnabled = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.ReflectionsEnabled"; if (t.tfield_s == t.try_s)  t.visuals.bReflectionsEnabled = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.FXAAEnabled"; if (t.tfield_s == t.try_s)  t.visuals.bFXAAEnabled = ValF(t.tvalue_s.Get());


			t.try_s = "visuals.DOF"; if (t.tfield_s == t.try_s)  t.visuals.bDOF = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.DOFStrength"; if (t.tfield_s == t.try_s)  t.visuals.fDOFStrength = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.DOFApertureSize"; if (t.tfield_s == t.try_s)  t.visuals.fDOFApertureSize = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.DOFFocalLength"; if (t.tfield_s == t.try_s)  t.visuals.fDOFFocalLength = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.Tessellation"; if (t.tfield_s == t.try_s)  t.visuals.bTessellation = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.LightShafts"; if (t.tfield_s == t.try_s)  t.visuals.bLightShafts = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.LensFlare"; if (t.tfield_s == t.try_s)  t.visuals.bLensFlare = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.Exposure"; if (t.tfield_s == t.try_s)  t.visuals.fExposure = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.AutoExposure"; if (t.tfield_s == t.try_s)  t.visuals.bAutoExposure = ValF(t.tvalue_s.Get());
			
			//PE: Who removed gamme ? we need it when restoring saved levels ? it was commented out ?
			t.try_s = "visuals.Gamma"; if (t.tfield_s == t.try_s)  t.visuals.fGamma = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.MSAASampleCount"; if (t.tfield_s == t.try_s)  t.visuals.iMSAASampleCount = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.FSRMode"; if (t.tfield_s == t.try_s)  t.visuals.iFSRMode = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.FSRSharpness"; if (t.tfield_s == t.try_s)  t.visuals.fFSRSharpness = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.MSAO"; if (t.tfield_s == t.try_s)  t.visuals.iMSAO = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.MSAOPower"; if (t.tfield_s == t.try_s)  t.visuals.fMSAOPower = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.SkyCloudiness"; if (t.tfield_s == t.try_s)  t.visuals.SkyCloudiness = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SkyCloudScale"; 
			if (t.tfield_s == t.try_s)
			{
				// LB: older levels had incredible small scale value
				float fTheValue = ValF(t.tvalue_s.Get());
				if (fTheValue >= 1)
				{
					// only accept the newer much larger cloud height values, ignore the 0.0003 stuff from earlier!
					t.visuals.SkyCloudHeight = fTheValue;
				}
			}
			t.try_s = "visuals.SkyCloudSpeed"; if (t.tfield_s == t.try_s)  t.visuals.SkyCloudSpeed = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SkyCloudCoverage"; if (t.tfield_s == t.try_s)  t.visuals.SkyCloudCoverage = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SkyCloudThickness"; if (t.tfield_s == t.try_s)  t.visuals.SkyCloudThickness = ValF(t.tvalue_s.Get());
		
			t.try_s = "visuals.ShadowSpotCascadeResolution"; if (t.tfield_s == t.try_s)  t.visuals.iShadowSpotCascadeResolution = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.ShadowSpotResolution"; if (t.tfield_s == t.try_s)  t.visuals.iShadowSpotResolution = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.ShadowPointResolution"; if (t.tfield_s == t.try_s)  t.visuals.iShadowPointResolution = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.iShadowPointMax"; if (t.tfield_s == t.try_s)  t.visuals.iShadowPointMax = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.iShadowSpotMax"; if (t.tfield_s == t.try_s)  t.visuals.iShadowSpotMax = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.bTransparentShadows"; if (t.tfield_s == t.try_s)  t.visuals.bTransparentShadows = ValF(t.tvalue_s.Get());
			
			t.try_s = "visuals.SunAngleX"; if (t.tfield_s == t.try_s)  t.visuals.SunAngleX = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SunAngleY"; if (t.tfield_s == t.try_s)  t.visuals.SunAngleY = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SunAngleZ"; if (t.tfield_s == t.try_s)  t.visuals.SunAngleZ = ValF(t.tvalue_s.Get());

			//t.try_s = "visuals.ShadowFarPlane"; if (t.tfield_s == t.try_s)  t.visuals.fShadowFarPlane = ValF(t.tvalue_s.Get());


			t.try_s = "visuals.WaterEnable"; if (t.tfield_s == t.try_s)
				t.visuals.bWaterEnable = ValF(t.tvalue_s.Get());
			
			t.try_s = "visuals.WaterWaveAmplitude"; if (t.tfield_s == t.try_s)  t.visuals.fWaterWaveAmplitude = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterPatchLength"; if (t.tfield_s == t.try_s)  t.visuals.fWaterPatchLength = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterChoppyScale"; if (t.tfield_s == t.try_s)  t.visuals.fWaterChoppyScale = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterWindDependency"; if (t.tfield_s == t.try_s)  t.visuals.fWaterWindDependency = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.WaterFogMinDist"; if (t.tfield_s == t.try_s)  t.visuals.WaterFogMinDist = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterFogMaxDist"; if (t.tfield_s == t.try_s)  t.visuals.WaterFogMaxDist = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WaterFogMinAmount"; if (t.tfield_s == t.try_s)  t.visuals.WaterFogMinAmount = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.TimeOfday"; if (t.tfield_s == t.try_s)  t.visuals.iTimeOfday = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.Simulate24Hours"; if (t.tfield_s == t.try_s)  t.visuals.bSimulate24Hours = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.TimeSpeed"; if (t.tfield_s == t.try_s)  t.visuals.fTimeSpeed = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.WeatherIntensity"; if (t.tfield_s == t.try_s)  t.visuals.fWeatherIntensity = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WeatherLighting"; if (t.tfield_s == t.try_s)  t.visuals.fWeatherLighting = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WeatherThunder"; if (t.tfield_s == t.try_s)  t.visuals.fWeatherThunder = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.WeatherWind"; if (t.tfield_s == t.try_s)  t.visuals.fWeatherWind = ValF(t.tvalue_s.Get());


			t.try_s = "visuals.bPPSnow"; if (t.tfield_s == t.try_s)  t.visuals.bPPSnow = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.voxel_steps"; if (t.tfield_s == t.try_s)  t.visuals.voxel_steps = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.pp_size"; if (t.tfield_s == t.try_s)  t.visuals.pp_size = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.pp_alpha"; if (t.tfield_s == t.try_s)  t.visuals.pp_alpha = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.wind_direction_x"; if (t.tfield_s == t.try_s)  t.visuals.wind_direction_x = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.wind_direction_y"; if (t.tfield_s == t.try_s)  t.visuals.wind_direction_y = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.wind_direction_z"; if (t.tfield_s == t.try_s)  t.visuals.wind_direction_z = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.wind_speed"; if (t.tfield_s == t.try_s)  t.visuals.wind_speed = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.wind_randomness"; if (t.tfield_s == t.try_s)  t.visuals.wind_randomness = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.bpp_disable_indoor"; if (t.tfield_s == t.try_s)  t.visuals.bpp_disable_indoor = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.tree_wind"; if (t.tfield_s == t.try_s)  t.visuals.tree_wind = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.LevelDifficulty"; if (t.tfield_s == t.try_s)  t.visuals.fLevelDifficulty = ValF(t.tvalue_s.Get());

			for (int iL = 0; iL < 128; iL++) 
			{
				t.try_s = cStr("visuals.TerrainTextures") + cStr(iL);
				if (t.tfield_s == t.try_s)
					t.visuals.sTerrainTextures[iL] = t.tvalue_s;
				t.try_s = cStr("visuals.TerrainTexturesName") + cStr(iL);
				if (t.tfield_s == t.try_s)
					t.visuals.sTerrainTexturesName[iL] = t.tvalue_s;

				t.try_s = cStr("visuals.GrassTextures") + cStr(iL);
				if (t.tfield_s == t.try_s)
					t.visuals.sGrassTextures[iL] = t.tvalue_s;
				t.try_s = cStr("visuals.GrassTexturesName") + cStr(iL);
				if (t.tfield_s == t.try_s)
					t.visuals.sGrassTexturesName[iL] = t.tvalue_s;

				t.try_s = cStr("visuals.FactionName") + cStr(iL);
				if (t.tfield_s == t.try_s)
					t.visuals.sFactionName[iL] = t.tvalue_s;
			}

			t.try_s = "visuals.EndableAmbientMusicTrack"; if (t.tfield_s == t.try_s)  t.visuals.bEndableAmbientMusicTrack = ValF(t.tvalue_s.Get());
			t.try_s = cStr("visuals.AmbientMusicTrack"); if (t.tfield_s == t.try_s) t.visuals.sAmbientMusicTrack = t.tvalue_s;
			t.try_s = "visuals.AmbientMusicTrackVolume"; if (t.tfield_s == t.try_s)  t.visuals.iAmbientMusicTrackVolume = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.EnableCombatMusicTrack"; if (t.tfield_s == t.try_s)  t.visuals.bEnableCombatMusicTrack = ValF(t.tvalue_s.Get());
			t.try_s = cStr("visuals.CombatMusicTrack"); if (t.tfield_s == t.try_s) t.visuals.sCombatMusicTrack = t.tvalue_s;
			t.try_s = "visuals.CombatMusicTrackVolume"; if (t.tfield_s == t.try_s)  t.visuals.iCombatMusicTrackVolume = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.RainEnabled"; if (t.tfield_s == t.try_s)  t.visuals.bRainEnabled = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.RainSpeedX"; if (t.tfield_s == t.try_s)  t.visuals.fRainSpeedX = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.RainSpeedY"; if (t.tfield_s == t.try_s)  t.visuals.fRainSpeedY = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.RainOpacity"; if (t.tfield_s == t.try_s)  t.visuals.fRainOpacity = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.RainScaleX"; if (t.tfield_s == t.try_s)  t.visuals.fRainScaleX = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.RainScaleY"; if (t.tfield_s == t.try_s)  t.visuals.fRainScaleY = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.RainRefreactionScale"; if (t.tfield_s == t.try_s)  t.visuals.fRainRefreactionScale = ValF(t.tvalue_s.Get());


			t.try_s = "visuals.SnowEnabled"; if (t.tfield_s == t.try_s)  t.visuals.bSnowEnabled = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SnowLayers"; if (t.tfield_s == t.try_s)  t.visuals.fSnowLayers = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SnowDepth"; if (t.tfield_s == t.try_s)  t.visuals.fSnowDepth = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SnowWind"; if (t.tfield_s == t.try_s)  t.visuals.fSnowWind = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SnowSpeed"; if (t.tfield_s == t.try_s)  t.visuals.fSnowSpeed = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.SnowOpacity"; if (t.tfield_s == t.try_s)  t.visuals.fSnowOpacity = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.EndableTreeDrawing"; if (t.tfield_s == t.try_s)  t.visuals.bEndableTreeDrawing = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.EndableGrassDrawing"; if (t.tfield_s == t.try_s)  t.visuals.bEndableGrassDrawing = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.EndableTerrainDrawing"; if (t.tfield_s == t.try_s)  t.visuals.bEndableTerrainDrawing = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.HeightmapWidth"; if (t.tfield_s == t.try_s)  t.visuals.iHeightmapWidth = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.HeightmapHeight"; if (t.tfield_s == t.try_s)  t.visuals.iHeightmapHeight = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.DisableSkybox"; if (t.tfield_s == t.try_s)  t.visuals.bDisableSkybox = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.GrassMinHeight"; if (t.tfield_s == t.try_s)  GGGrass::gggrass_global_params.min_height = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.GrassMaxHeight"; if (t.tfield_s == t.try_s)  GGGrass::gggrass_global_params.max_height = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.GrassStartHeightUnderwater"; if (t.tfield_s == t.try_s)  GGGrass::gggrass_global_params.min_height_underwater = ValF(t.tvalue_s.Get());
			t.try_s = "visuals.GrassEndHeightUnderwater"; if (t.tfield_s == t.try_s)  GGGrass::gggrass_global_params.max_height_underwater = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.EnvProbeResolution"; if (t.tfield_s == t.try_s)  t.visuals.iEnvProbeResolution = ValF(t.tvalue_s.Get());

			t.try_s = "visuals.NewPerformancePresets"; if (t.tfield_s == t.try_s)  t.visuals.newperformancepresets = ValF(t.tvalue_s.Get());

		} while ( !(  Len(t.tline_s.Get())<2 ) );

		CloseFile (  1 );

		// LB: Detect old MAX level values, and correct here
		if (t.visuals.WaterSpeed1 > 1.0f)
		{
			// a recent change reduced this value from the likes oif 35 down to 0.00xx, which coincided with reducing water level to zero
			t.visuals.WaterSpeed1 = 0.06f;
			g.gdefaultwaterheight = -500.0f;
			t.visuals.fWaterPatchLength = 10.0f;
			t.visuals.bWaterEnable = false;
		}
		t.terrain.waterliney_f = g.gdefaultwaterheight;

		if (!t.visuals.bWaterEnable)
		{
			t.terrain.waterliney_f = g.gdefaultwaterheight = -10000.0f;
		}

		// reset grass type choices to paint with
		grass_resetchoices();

		// if older visuals file and loaded into new performance aware engine, add best defaults for PERFORMANCE! No-one remarks on QUALITY in Steam Reviews, so favor PERFORMANCE.
		if (bVisualFileFromLevel == true)
		{
			if (t.visuals.newperformancepresets == 0)
			{
				bool bUpdateEngine = false;
				visuals_shaderlevels_setlevel(4, bUpdateEngine); // LOW - best for performance :)
				t.visuals.newperformancepresets = 1;
			}
		}
	}

	//  Right away we cap 'VERTICAL' CameraFOV# for legacy levels which could set it VERY high
	if (  t.visuals.CameraFOV_f>62.14f  )  t.visuals.CameraFOV_f = 62.14f;
	if (  t.visuals.WeaponFOV_f>62.14f  )  t.visuals.WeaponFOV_f = 62.14f;

	//  Restore any settings we want when IDE first starts (that might have been changed by load/save code)
	if (  t.tresetforstartofeditor == 1 ) 
	{
		//  settings that do not change!
		t.visuals.AmbienceIntensity_f=t.defaultvisuals.AmbienceIntensity_f;
		t.visuals.AmbienceRed_f=t.defaultvisuals.AmbienceRed_f;
		t.visuals.AmbienceGreen_f=t.defaultvisuals.AmbienceGreen_f;
		t.visuals.AmbienceBlue_f=t.defaultvisuals.AmbienceBlue_f;
		t.visuals.SurfaceRed_f=t.defaultvisuals.SurfaceRed_f;
		t.visuals.SurfaceGreen_f=t.defaultvisuals.SurfaceGreen_f;
		t.visuals.SurfaceBlue_f=t.defaultvisuals.SurfaceBlue_f;
		t.visuals.skyindex=t.defaultvisuals.skyindex;
		t.visuals.sky_s=t.defaultvisuals.sky_s;
		t.visuals.terrainindex=t.defaultvisuals.terrainindex;
		t.visuals.terrain_s=t.defaultvisuals.terrain_s;
		t.visuals.vegetationindex=t.defaultvisuals.vegetationindex;
		t.visuals.vegetation_s=t.defaultvisuals.vegetation_s;
		t.visuals.TerrainSize_f=100.0;
		t.visuals.iEnvironmentWeather = 0;
		t.tresetforstartofeditor=0;
		//  save out to establish these defaults (if later used to save a level)
		visuals_save ( );
	}

	// Set initial states for the Editor View Options - based on the saved visuals for the level.
	t.showeditorwater = t.visuals.bWaterEnable;
	t.showeditortrees = t.visuals.bEndableTreeDrawing;
	t.showeditorveg = t.visuals.bEndableGrassDrawing;
	t.showeditorterrain = t.visuals.bEndableTerrainDrawing;
}

void visuals_justshaderupdate ( void )
{
	#ifdef WICKEDENGINE
	// Wicked has its own shaders
	#else
	//  Post Process Shaders
	if ( GetEffectExist(g.postprocesseffectoffset+0) == 1 ) 
	{
		// Regular BLOOM or SAO shader (ie send values to SAO shader now in charge of post processing)
		int iPPS = 0; if ( t.visuals.SAOIntensity_f > 0.0f ) iPPS = 4;

		//  Bloom constants
		if (  t.visuals.bloommode>0 ) 
		{
			if (  g.gfinalrendercameraid>0 ) 
			{
				SetEffectConstantF (  g.postprocesseffectoffset+iPPS,"BloomThreshold",1.0-((t.visuals.bloommode+0.0)/100.0) );
				SetEffectConstantF (  g.postprocesseffectoffset+iPPS,"PostContrast",t.visuals.PostContrast_f );
				SetEffectConstantF (  g.postprocesseffectoffset+iPPS,"PostBrightness",t.visuals.PostBrightness_f );
				SetVector4 (  g.generalvectorindex+1,t.visuals.VignetteRadius_f,t.visuals.VignetteIntensity_f,0,0 );
				SetEffectConstantV (  g.postprocesseffectoffset+iPPS,"Vignette",g.generalvectorindex+1 );
				SetVector4 (  g.generalvectorindex+1,t.visuals.MotionDistance_f,t.visuals.MotionIntensity_f,0,0 );
				SetEffectConstantV (  g.postprocesseffectoffset+iPPS,"Motion",g.generalvectorindex+1 );
				SetVector4 (  g.generalvectorindex+1,t.visuals.DepthOfFieldDistance_f,t.visuals.DepthOfFieldIntensity_f,0,0 );
				SetEffectConstantV (  g.postprocesseffectoffset+iPPS,"DepthOfField",g.generalvectorindex+1 );
			}
			if (  g.globals.riftmode == 0 ) 
			{
				if ( g.postprocessobjectoffset0laststate != g.postprocesseffectoffset+iPPS )
				{
					SetVector4 ( g.terrainvectorindex,0,0,0,0 );
					SetEffectConstantV ( g.postprocesseffectoffset+iPPS,"[hook-depth-data]", g.terrainvectorindex );
					SetObjectEffect (  g.postprocessobjectoffset+0,g.postprocesseffectoffset+iPPS );
					g.postprocessobjectoffset0laststate = g.postprocesseffectoffset+iPPS;
				}
			}
			else
			{
				if ( g.postprocessobjectoffset0laststate != g.postprocesseffectoffset+3 )
				{
					SetVector4 ( g.terrainvectorindex,0,0,0,0 );
					SetEffectConstantV ( g.postprocesseffectoffset+3,"[hook-depth-data]", g.terrainvectorindex );
					SetObjectEffect ( g.postprocessobjectoffset+0,g.postprocesseffectoffset+3 );
					g.postprocessobjectoffset0laststate = g.postprocesseffectoffset+3;
				}
			}
		}
		else
		{
			if (  g.gfinalrendercameraid>0 ) 
			{
				SetEffectConstantF (  g.postprocesseffectoffset+2,"PostContrast",t.visuals.PostContrast_f );
				SetEffectConstantF (  g.postprocesseffectoffset+2,"PostBrightness",t.visuals.PostBrightness_f );
			}
			if (  g.globals.riftmode == 0 ) 
			{
				if ( g.postprocessobjectoffset0laststate != g.postprocesseffectoffset+2 )
				{
					SetVector4 ( g.terrainvectorindex,0,0,0,0 );
					SetEffectConstantV ( g.postprocesseffectoffset+2,"[hook-depth-data]", g.terrainvectorindex );
					SetObjectEffect (  g.postprocessobjectoffset+0,g.postprocesseffectoffset+2 );
					g.postprocessobjectoffset0laststate = g.postprocesseffectoffset+2;
				}
			}
			else
			{
				if ( g.postprocessobjectoffset0laststate != g.postprocesseffectoffset+3 )
				{
					SetVector4 ( g.terrainvectorindex,0,0,0,0 );
					SetEffectConstantV ( g.postprocesseffectoffset+3,"[hook-depth-data]", g.terrainvectorindex );
					SetObjectEffect (  g.postprocessobjectoffset+0,g.postprocesseffectoffset+3 );
					g.postprocessobjectoffset0laststate = g.postprocesseffectoffset+3;
				}
			}
		}

		//  Rift constants
		if (  g.globals.riftmode>0 ) 
		{
			//  Lees tweaked values
			SetVector4 (  g.generalvectorindex+1, 2.0f, 1.66f, 0, 0    );// ScaleIn;
			SetVector4 (  g.generalvectorindex+2, 0.32f, 0.45f, 0, 0   );// Scale;
			SetVector4 (  g.generalvectorindex+3, 1, 0.22f, 0.24f, 0   );// HmdWarpParam;
			SetVector4 (  g.generalvectorindex+4, 0.5f, 0.5f, 0, 0     );// ScreenCenter;
			SetVector4 (  g.generalvectorindex+5, 0.5f, 0.5f, 0, 0     );// Len ( sCenter );

			SetEffectConstantV (  g.postprocesseffectoffset+3, "ScaleIn", g.generalvectorindex+1 );
			SetEffectConstantV (  g.postprocesseffectoffset+3, "Scale", g.generalvectorindex+2 );
			SetEffectConstantV (  g.postprocesseffectoffset+3, "HmdWarpParam", g.generalvectorindex+3 );
			SetEffectConstantV (  g.postprocesseffectoffset+3, "ScreenCenter", g.generalvectorindex+4 );
			SetEffectConstantV (  g.postprocesseffectoffset+3, "LensCenter", g.generalvectorindex+5 );
		}
	}

	if (  GetEffectExist(g.postprocesseffectoffset+1) == 1 ) 
	{
		if (  t.glightraycameraid>0 ) 
		{
			if (  t.visuals.lightraymode>0 ) 
			{
				LPSTR pScatterTechnique = "ScatterLOW";
				if ( t.visuals.LightrayQuality_f > 25 )
				{
					pScatterTechnique = "ScatterMEDIUM";
					if ( t.visuals.LightrayQuality_f > 50 )
					{
						pScatterTechnique = "ScatterHIGH";
						if ( t.visuals.LightrayQuality_f > 75 )
						{
							pScatterTechnique = "ScatterHIGHEST";
						}
					}
				}
				SetEffectTechnique (  g.postprocesseffectoffset+1, pScatterTechnique );
				SetEffectConstantF (  g.postprocesseffectoffset+1,"LightRayFactor",((t.visuals.lightraymode+0.0)/100.0) );
			}
			else
			{
				SetEffectTechnique (  g.postprocesseffectoffset+1,"NoScatter" );
			}
		}
	}

	//  In-Game Shaders
	if (  t.terrain.terrainshaderindex>0 ) 
	{
		if (  GetEffectExist(t.terrain.terrainshaderindex) == 1 ) 
		{
			SetEffectConstantF (  t.terrain.terrainshaderindex,"ShadowStrength",t.visuals.shadowmode/100.0 );
			SetVector4 (  g.terrainvectorindex,t.visuals.FogNearest_f,t.visuals.FogDistance_f,0,0 );
			SetEffectConstantV (  t.terrain.terrainshaderindex,"HudFogDist",g.terrainvectorindex );
			SetVector4 (  g.terrainvectorindex,t.visuals.FogR_f/255.0,t.visuals.FogG_f/255.0,t.visuals.FogB_f/255.0,t.visuals.FogA_f/255.0 );
			SetEffectConstantV (  t.terrain.terrainshaderindex,"HudFogColor",g.terrainvectorindex );
			SetVector4 (  g.terrainvectorindex,t.visuals.AmbienceIntensity_f/255.0,t.visuals.AmbienceIntensity_f/255.0,t.visuals.AmbienceIntensity_f/255.0,t.visuals.AmbienceIntensity_f/255.0 );
			SetEffectConstantV (  t.terrain.terrainshaderindex,"AmbiColorOverride",g.terrainvectorindex );
			SetVector4 (  g.terrainvectorindex,t.visuals.AmbienceRed_f/255.0,t.visuals.AmbienceGreen_f/255.0,t.visuals.AmbienceBlue_f/255.0,0 );
			SetEffectConstantV (  t.terrain.terrainshaderindex,"AmbiColor",g.terrainvectorindex );
			SetVector4 (  g.terrainvectorindex,t.visuals.SurfaceRed_f/255.0,t.visuals.SurfaceGreen_f/255.0,t.visuals.SurfaceBlue_f/255.0,0 );
			SetEffectConstantV (  t.terrain.terrainshaderindex,"SurfColor",g.terrainvectorindex );
			SetVector4 (  g.terrainvectorindex,t.terrain.suncolorr_f/255.0,t.terrain.suncolorg_f/255.0,t.terrain.suncolorb_f/255.0,0 );
			SetEffectConstantV (  t.terrain.terrainshaderindex,"SkyColor",g.terrainvectorindex );
			SetVector4 (  g.terrainvectorindex,t.terrain.floorcolorr_f/255.0,t.terrain.floorcolorg_f/255.0,t.terrain.floorcolorb_f/255.0,0 );
			SetEffectConstantV (  t.terrain.terrainshaderindex,"FloorColor",g.terrainvectorindex );
			SetVector4 (  g.terrainvectorindex,t.terrain.sundirectionx_f,t.terrain.sundirectiony_f,t.terrain.sundirectionz_f,0.0 );
			SetEffectConstantV (  t.terrain.terrainshaderindex,"LightSource",g.terrainvectorindex );
			if (  t.game.set.ismapeditormode == 1 ) 
			{
				SetVector4 (  g.terrainvectorindex,9999999.0,9999999.0,0.0,0.0 );
			}
			else
			{
				t.tactualstart_f=t.visuals.DistanceTransitionStart_f*t.visuals.DistanceTransitionMultiplier_f;
				SetVector4 (  g.terrainvectorindex,t.tactualstart_f,t.tactualstart_f+t.visuals.DistanceTransitionRange_f,t.visuals.DistanceTransitionRange_f,0.0 );
			}
			SetEffectConstantV (  t.terrain.terrainshaderindex,"DistanceTransition",g.terrainvectorindex );
			SetEffectConstantF (  t.terrain.terrainshaderindex,"SurfaceSunFactor",t.visuals.SurfaceSunFactor_f );
			SetEffectConstantF (  t.terrain.terrainshaderindex,"GlobalSpecular",t.visuals.Specular_f );
			SetEffectConstantF (  t.terrain.terrainshaderindex,"GlobalSurfaceIntensity",t.visuals.SurfaceIntensity_f );
		}
	}
	if (  t.terrain.vegetationshaderindex>0 ) 
	{
		if (  GetEffectExist(t.terrain.vegetationshaderindex) == 1 ) 
		{
			SetEffectConstantF (  t.terrain.vegetationshaderindex,"ShadowStrength",t.visuals.shadowmode/100.0 );
			SetVector4 (  g.vegetationvectorindex,t.visuals.FogNearest_f,t.visuals.FogDistance_f,0,0 );
			SetEffectConstantV (  t.terrain.vegetationshaderindex,"HudFogDist",g.vegetationvectorindex );
			SetVector4 (  g.vegetationvectorindex,t.visuals.FogR_f/255.0,t.visuals.FogG_f/255.0,t.visuals.FogB_f/255.0,t.visuals.FogA_f/255.0 );
			SetEffectConstantV (  t.terrain.vegetationshaderindex,"HudFogColor",g.vegetationvectorindex );
			SetVector4 (  g.vegetationvectorindex,t.visuals.AmbienceIntensity_f/255.0,t.visuals.AmbienceIntensity_f/255.0,t.visuals.AmbienceIntensity_f/255.0,t.visuals.AmbienceIntensity_f/255.0 );
			SetEffectConstantV (  t.terrain.vegetationshaderindex,"AmbiColorOverride",g.vegetationvectorindex );
			SetVector4 (  g.vegetationvectorindex,t.visuals.AmbienceRed_f/255.0,t.visuals.AmbienceGreen_f/255.0,t.visuals.AmbienceBlue_f/255.0,0 );
			SetEffectConstantV (  t.terrain.vegetationshaderindex,"AmbiColor",g.vegetationvectorindex );
			SetVector4 (  g.vegetationvectorindex,t.visuals.SurfaceRed_f/255.0,t.visuals.SurfaceGreen_f/255.0,t.visuals.SurfaceBlue_f/255.0,0 );
			SetEffectConstantV (  t.terrain.vegetationshaderindex,"SurfColor",g.vegetationvectorindex );
			SetVector4 (  g.vegetationvectorindex,t.terrain.suncolorr_f/255.0,t.terrain.suncolorg_f/255.0,t.terrain.suncolorb_f/255.0,0 );
			SetEffectConstantV (  t.terrain.vegetationshaderindex,"SkyColor",g.terrainvectorindex );
			SetVector4 (  g.vegetationvectorindex,t.terrain.floorcolorr_f/255.0,t.terrain.floorcolorg_f/255.0,t.terrain.floorcolorb_f/255.0,0 );
			SetEffectConstantV (  t.terrain.vegetationshaderindex,"FloorColor",g.terrainvectorindex );
			SetVector4 (  g.vegetationvectorindex,t.terrain.sundirectionx_f,t.terrain.sundirectiony_f,t.terrain.sundirectionz_f,0.0 );
			SetEffectConstantV (  t.terrain.vegetationshaderindex,"LightSource",g.vegetationvectorindex );
			grass_setgrassgridandfade ( );
			SetEffectConstantF (  t.terrain.vegetationshaderindex,"GrassFadeDistance", (float)t.tGrassFadeDistance );
			SetEffectConstantF (  t.terrain.vegetationshaderindex,"SurfaceSunFactor",t.visuals.SurfaceSunFactor_f );
			SetEffectConstantF (  t.terrain.vegetationshaderindex,"GlobalSpecular",t.visuals.Specular_f );
			SetEffectConstantF (  t.terrain.vegetationshaderindex,"GlobalSurfaceIntensity",t.visuals.SurfaceIntensity_f );
		}
	}
	//update water shader
	if (GetEffectExist(t.terrain.effectstartindex + 1))
	{
		SetVector4(g.terrainvectorindex, t.visuals.WaterRed_f / 256, t.visuals.WaterGreen_f / 256, t.visuals.WaterBlue_f / 256, 0);
		SetEffectConstantV(t.terrain.effectstartindex + 1, "WaterCol", g.terrainvectorindex);
		//nWaterscale 0 let strange lightning artifacts appear
		SetVector4(g.terrainvectorindex, t.visuals.WaterWaveIntensity_f+1, t.visuals.WaterWaveIntensity_f+1, 0, 0);
		SetEffectConstantV(t.terrain.effectstartindex + 1, "nWaterScale", g.terrainvectorindex);
		SetEffectConstantF(t.terrain.effectstartindex + 1, "WaterTransparancy", t.visuals.WaterTransparancy_f);
		SetEffectConstantF(t.terrain.effectstartindex + 1, "WaterReflection", t.visuals.WaterReflection_f);
		SetEffectConstantF(t.terrain.effectstartindex + 1, "reflectionSparkleIntensity", t.visuals.WaterReflectionSparkleIntensity);
		SetVector4(g.terrainvectorindex, t.visuals.WaterFlowDirectionX * t.visuals.WaterFlowSpeed, t.visuals.WaterFlowDirectionY * t.visuals.WaterFlowSpeed, 0, 0);
		SetEffectConstantV(t.terrain.effectstartindex + 1, "flowdirection", g.terrainvectorindex);
		SetEffectConstantF(t.terrain.effectstartindex + 1, "WaterSpeed1", t.visuals.WaterSpeed1);
		SetEffectConstantF(t.terrain.effectstartindex + 1, "distortion2", t.visuals.WaterDistortionWaves);
	}

	//  update fog shader
	t.tFogNear_f=t.visuals.FogNearest_f ; t.tFogFar_f=t.visuals.FogDistance_f;
	t.tFogR_f=t.visuals.FogR_f ; t.tFogG_f=t.visuals.FogG_f ; t.tFogB_f=t.visuals.FogB_f ; ; t.tFogA_f=t.visuals.FogA_f;
	terrain_water_setfog ( );
	for ( t.t = -6 ; t.t<=  g.effectbankmax; t.t++ )
	{
		if (  t.t == -6  )  t.effectid = g.lightmappbreffectillum;
		#ifdef VRTECH
		if (  t.t == -5  )  t.effectid = g.controllerpbreffect;
		#else
		if (t.t == -5)  continue;
		#endif
		if (  t.t == -4  )  t.effectid = g.lightmappbreffect;
		if (  t.t == -3  )  t.effectid = g.thirdpersonentityeffect;
		if (  t.t == -2  )  t.effectid = g.thirdpersoncharactereffect;
		if (  t.t == -1  )  t.effectid = g.staticlightmapeffectoffset;
		if (  t.t == 0  )  t.effectid = g.staticshadowlightmapeffectoffset;
		if (  t.t>0  )  t.effectid = g.effectbankoffset+t.t;
		if (  GetEffectExist(t.effectid) == 1 ) 
		{
			SetEffectConstantF (  t.effectid,"ShadowStrength",t.visuals.shadowmode/100.0 );
			SetVector4 (  g.terrainvectorindex,t.visuals.FogNearest_f,t.visuals.FogDistance_f,0,0 );
			SetEffectConstantV (  t.effectid,"HudFogDist",g.terrainvectorindex );
			SetVector4 (  g.terrainvectorindex,t.visuals.FogR_f/255.0,t.visuals.FogG_f/255.0,t.visuals.FogB_f/255.0,t.visuals.FogA_f/255.0 );
			SetEffectConstantV (  t.effectid,"HudFogColor",g.terrainvectorindex );

			SetVector4 (  g.terrainvectorindex,t.visuals.AmbienceIntensity_f/255.0,t.visuals.AmbienceIntensity_f/255.0,t.visuals.AmbienceIntensity_f/255.0,t.visuals.AmbienceIntensity_f/255.0 );
			SetEffectConstantV (  t.effectid,"AmbiColorOverride",g.terrainvectorindex );
			SetVector4 (  g.terrainvectorindex,t.visuals.AmbienceRed_f/255.0,t.visuals.AmbienceGreen_f/255.0,t.visuals.AmbienceBlue_f/255.0,0 );
			SetEffectConstantV (  t.effectid,"AmbiColor",g.terrainvectorindex );

			SetVector4 (  g.terrainvectorindex,t.visuals.SurfaceRed_f/255.0,t.visuals.SurfaceGreen_f/255.0,t.visuals.SurfaceBlue_f/255.0, 0.0f );
			SetEffectConstantV (  t.effectid,"SurfColor",g.terrainvectorindex );
			SetVector4 (  g.terrainvectorindex,t.terrain.suncolorr_f/255.0,t.terrain.suncolorg_f/255.0,t.terrain.suncolorb_f/255.0,0 );
			SetEffectConstantV (  t.effectid,"SkyColor",g.terrainvectorindex );
			SetVector4 (  g.terrainvectorindex,t.terrain.floorcolorr_f/255.0,t.terrain.floorcolorg_f/255.0,t.terrain.floorcolorb_f/255.0,0 );
			SetEffectConstantV (  t.effectid,"FloorColor",g.terrainvectorindex );
			SetVector4 (  g.terrainvectorindex,t.terrain.sundirectionx_f,t.terrain.sundirectiony_f,t.terrain.sundirectionz_f,0.0 );
			SetEffectConstantV (  t.effectid,"LightSource",g.terrainvectorindex );
			SetVector4 ( g.terrainvectorindex, 500000, 1, 0, 0 );
			SetEffectConstantV (  t.effectid,"EntityEffectControl",g.terrainvectorindex );
			SetEffectConstantF (  t.effectid,"SurfaceSunFactor",t.visuals.SurfaceSunFactor_f );
			SetEffectConstantF (  t.effectid,"GlobalSpecular",t.visuals.Specular_f );
			SetEffectConstantF (  t.effectid,"GlobalSurfaceIntensity",t.visuals.SurfaceIntensity_f );
		}
	}

	//PE: HUD. did not get any visuals, shader variables.
	if(GetEffectExist(g.jetpackeffectoffset) )  {
		t.effectid = g.jetpackeffectoffset;
		SetEffectConstantF(t.effectid, "ShadowStrength", t.visuals.shadowmode / 100.0);
		SetVector4(g.terrainvectorindex, t.visuals.FogNearest_f, t.visuals.FogDistance_f, 0, 0);
		SetEffectConstantV(t.effectid, "HudFogDist", g.terrainvectorindex);
		SetVector4(g.terrainvectorindex, t.visuals.FogR_f / 255.0, t.visuals.FogG_f / 255.0, t.visuals.FogB_f / 255.0, t.visuals.FogA_f / 255.0);
		SetEffectConstantV(t.effectid, "HudFogColor", g.terrainvectorindex);

		SetVector4(g.terrainvectorindex, t.visuals.AmbienceIntensity_f / 255.0, t.visuals.AmbienceIntensity_f / 255.0, t.visuals.AmbienceIntensity_f / 255.0, t.visuals.AmbienceIntensity_f / 255.0);
		SetEffectConstantV(t.effectid, "AmbiColorOverride", g.terrainvectorindex);
		SetVector4(g.terrainvectorindex, t.visuals.AmbienceRed_f / 255.0, t.visuals.AmbienceGreen_f / 255.0, t.visuals.AmbienceBlue_f / 255.0, 0);
		SetEffectConstantV(t.effectid, "AmbiColor", g.terrainvectorindex);

		SetVector4(g.terrainvectorindex, t.visuals.SurfaceRed_f / 255.0, t.visuals.SurfaceGreen_f / 255.0, t.visuals.SurfaceBlue_f / 255.0, 0.0f);
		SetEffectConstantV(t.effectid, "SurfColor", g.terrainvectorindex);
		SetVector4(g.terrainvectorindex, t.terrain.suncolorr_f / 255.0, t.terrain.suncolorg_f / 255.0, t.terrain.suncolorb_f / 255.0, 0);
		SetEffectConstantV(t.effectid, "SkyColor", g.terrainvectorindex);
		SetVector4(g.terrainvectorindex, t.terrain.floorcolorr_f / 255.0, t.terrain.floorcolorg_f / 255.0, t.terrain.floorcolorb_f / 255.0, 0);
		SetEffectConstantV(t.effectid, "FloorColor", g.terrainvectorindex);
		SetVector4(g.terrainvectorindex, t.terrain.sundirectionx_f, t.terrain.sundirectiony_f, t.terrain.sundirectionz_f, 0.0);
		SetEffectConstantV(t.effectid, "LightSource", g.terrainvectorindex);
		SetVector4(g.terrainvectorindex, 500000, 1, 0, 0);
		SetEffectConstantV(t.effectid, "EntityEffectControl", g.terrainvectorindex);
		SetEffectConstantF(t.effectid, "SurfaceSunFactor", t.visuals.SurfaceSunFactor_f);
		SetEffectConstantF(t.effectid, "GlobalSpecular", t.visuals.Specular_f);
		SetEffectConstantF(t.effectid, "GlobalSurfaceIntensity", t.visuals.SurfaceIntensity_f);
	}


	//  update lightray shader
	if (  GetEffectExist(g.postprocesseffectoffset+1) == 1 ) 
	{
		SetVector4 (  g.terrainvectorindex,t.terrain.suncolorr_f/255.0,t.terrain.suncolorg_f/255.0,t.terrain.suncolorb_f/255.0,t.terrain.sunstrength_f/100.0 );
		SetEffectConstantV (  g.postprocesseffectoffset+1,"SkyColor",g.terrainvectorindex );
	}
	
	#endif
}

void visuals_restoreterrainshaderforeditor ( void )
{
	//  must set this distance value to FAR distance so overhead view in editor mode DOES NOT USE super texture (not exist)
	if (  t.terrain.terrainshaderindex>0 ) 
	{
		if (  GetEffectExist(t.terrain.terrainshaderindex) == 1 ) 
		{
			SetVector4 (  g.terrainvectorindex,9999999.0,9999999.0,0.0,0.0 );
			SetEffectConstantV (  t.terrain.terrainshaderindex,"DistanceTransition",g.terrainvectorindex );
		}
	}
}

void visuals_updatespecificobjectmasks ( int e, int obj )
{
	if ( obj>0 ) 
	{
		if ( ObjectExist(obj) == 1 ) 
		{
			int entid = t.entityelement[e].bankindex;
			if ( t.entityprofile[entid].ismarker != 0 ) 
			{
				SetObjectMask ( obj,1,0,0,0 );
			}
			else
			{
				if ( t.entityelement[e].hasbeenbatched == 1 || t.entityprofile[entid].castshadow == -1 ) 
				{
					if (  t.entityprofile[entid].castshadow == -1 ) 
					{
						SetObjectMask ( obj,t.tmaskforcamerasraw,0,0,0 );
#ifdef WICKEDENGINE
						//PE: This removes per mesh cast shadow in test game.
						sObject* pObject = g_ObjectList[obj];
						if (pObject)
							WickedCall_SetObjectCastShadows(pObject, false);

						bool bUseWEMaterial = false;
						if (t.entityprofile[entid].WEMaterial.MaterialActive)
						{
							WickedSetEntityId(entid);
							if (e > 0)
								WickedSetElementId(e);
							else
								WickedSetElementId(0);

							if (pObject)
							{
								bUseWEMaterial = true;
								for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
								{
									sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
									if (pMesh)
									{
										// set properties of mesh
										WickedSetMeshNumber(iMeshIndex);
										bool bCastShadows = WickedGetCastShadows();

										// get wicked meshID
										wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
										if (mesh)
										{
											// get material from mesh
											uint64_t materialEntity = mesh->subsets[0].materialID;
											wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
											if (pObjectMaterial)
											{
												if (bCastShadows)
													pObjectMaterial->SetCastShadow(true);
												else
													pObjectMaterial->SetCastShadow(false);
											}
										}
									}
								}
							}
							WickedSetEntityId(-1);
							WickedSetElementId(0);
						}

#endif
					}
					else
					{
						SetObjectMask ( obj,0,0,0,0 );
					}
				}
				else
				{
					t.tsize=ObjectSize(obj,1);
					t.tsmallmask=t.tmaskforcamerasnoreflectionlightrayflag;
					if ( t.tsize>10 )  // 131215 - 25 ) 
					{
						// 131215 - if larger than 10, allow entity to be reflected (before it was only over 100!)
						t.tsmallmask = t.tmaskforcamerasnoreflectionlightrayflag + 4;
						if ( t.visuals.lightraymode>0 )
						{
							t.tsmallmask=t.tsmallmask+16;
						}
					}
					if ( t.entityelement[e].donotreflect == 1 ) 
					{
						if ( t.visuals.lightraymode>0 ) 
						{
							t.tlargemask=t.tmaskforcamerasnoreflectionlightrayflag+16;
						}
						else
						{
							t.tlargemask=t.tmaskforcamerasnoreflectionlightrayflag;
						}
					}
					else
					{
						t.tlargemask=t.tmaskforcameras;
					}
					if ( t.tsize<100 ) 
					{
						SetObjectMask ( obj,t.tsmallmask,0,0,0 );
					}
					else
					{
						SetObjectMask ( obj,t.tlargemask,0,0,0 );
					}
				}
			}
		}
	}
}

void visuals_updateobjectmasks ( void )
{
	// actually used relfection value
	int iUseThisReflectionValue = t.visuals.reflectionmode;
	#ifdef VRTECH
	if ( g.gvrmode != 0 && iUseThisReflectionValue > 10 ) 
		iUseThisReflectionValue = 10;
	#endif

	// can be called from _loop and also from terrain (reflection update when not looking at water pixels)
	if ( 1 ) 
	{
		#ifdef VRTECH
		if (g.globals.riftmode > 0 || (g.vrglobals.GGVREnabled > 0 && g.vrglobals.GGVRUsingVRSystem == 1))
		{
			if (g.vrglobals.GGVREnabled > 0)
			{
				// VIVE = left and right eye cameras, and camera zero for editor
				t.tmaskforcamerasraw = 00001 + (1 << t.glefteyecameraid) + (1 << t.grighteyecameraid);
			}
			else
			{
				// RIFT = left and right eye cameras too (but not camera zero)
				// left and right eye cameras too (but not camera zero)
				t.tmaskforcamerasraw = 00000 + (1 << t.glefteyecameraid) + (1 << t.grighteyecameraid);
			}
		}
		#else
		if ( g.globals.riftmode>0  ) 
		{
			// left and right eye cameras too (but not camera zero)
			t.tmaskforcamerasraw=00000+(1<<t.glefteyecameraid)+(1<<t.grighteyecameraid);
		}
		#endif
		else
		{
			t.tmaskforcamerasraw=00001;
		}
		t.tmaskforcamerasnoreflectionlightrayshadowsflag=t.tmaskforcamerasraw;
		t.tmaskforcamerasnoshad=t.tmaskforcamerasraw;
		if (  t.visuals.shadowmode>0 || t.game.onceonlyshadow == 1 ) 
		{
			t.tmaskforcameras=t.tmaskforcamerasraw+(1<<31);
		}
		else
		{
			t.tmaskforcameras=t.tmaskforcamerasraw;
		}
		t.game.onceonlyshadow=0;
		t.tmaskforcamerasnoreflectionlightrayflag=t.tmaskforcameras;
		if (  t.visuals.lightraymode>0 ) 
		{
			t.tmaskforcameras=t.tmaskforcameras+16;
			t.tmaskforcamerasnoshad=t.tmaskforcamerasnoshad+16;
		}
		if ( iUseThisReflectionValue > 0 ) // t.visuals.reflectionmodemodified>0 ) 
		{
			t.tmaskforcameras=t.tmaskforcameras+4;
			t.tmaskforcamerasnoshad=t.tmaskforcamerasnoshad+4;
		}

		//  All game objects
		for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
		{
			visuals_updatespecificobjectmasks ( t.e, t.entityelement[t.e].obj );
		}

		//  grass can be excluded with specific reflection mode
		if ( iUseThisReflectionValue <= 99 ) 
		{
			//  ensure grass does not render in reflection render
			t.tmask=t.tmaskforcamerasnoreflectionlightrayshadowsflag;
			if ( iUseThisReflectionValue == 0 ) 
			{
				//  no water at all
				if( g.globals.forcenowaterreflection == 0 )
					t.hardwareinfoglobals.nowater=2;
				//  set all entities to never reflect
				for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
				{
					t.entityelement[t.e].donotreflect=1;
				}
			}
			else
			{
				//  restore water feature
				if (  t.hardwareinfoglobals.nowater == 2  )  t.hardwareinfoglobals.nowater = 0;
				//  work out which entities should reflect in reflection render
				t.thideatthissize=(100-iUseThisReflectionValue)*20;
				for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
				{
					t.entid=t.entityelement[t.e].bankindex;
					if (  t.entityprofile[t.entid].ismarker == 0 ) 
					{
						t.entityelement[t.e].donotreflect=1;
						t.obj=t.entityelement[t.e].obj;
						if (  t.obj>0 ) 
						{
							if (  ObjectExist(t.obj) == 1 ) 
							{
								t.tsizex=ObjectSizeX(t.obj,1);
								t.tsizey=ObjectSizeY(t.obj,1);
								t.tsizez=ObjectSizeZ(t.obj,1);
								if (  t.tsizex>t.thideatthissize || t.tsizey>t.thideatthissize || t.tsizez>t.thideatthissize ) 
								{
									t.entityelement[t.e].donotreflect=0;
								}
							}
						}
					}
				}
				//  and also go through any LM objects with same check as above
				if (  t.visuals.shaderlevels.lighting == 1 && t.game.set.ismapeditormode == 0 ) 
				{
					for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
					{
						if (  t.e <= ArrayCount(t.lmsceneobj) ) 
						{
							if (  t.lmsceneobj[t.e].startobj>0 ) 
							{
								for ( t.tobj = t.lmsceneobj[t.e].startobj; t.tobj<= t.lmsceneobj[t.e].finishobj; t.tobj++ )
								{
									if (  ObjectExist(t.tobj) == 1 ) 
									{
										t.tsizex=ObjectSizeX(t.tobj,1);
										t.tsizey=ObjectSizeY(t.tobj,1);
										t.tsizez=ObjectSizeZ(t.tobj,1);
										if (  t.tsizex>t.thideatthissize || t.tsizey>t.thideatthissize || t.tsizez>t.thideatthissize ) 
										{
											t.entityelement[t.e].donotreflect=0;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			//  restore water feature
			if (  t.hardwareinfoglobals.nowater == 2  )  t.hardwareinfoglobals.nowater = 0;
			//  grass in reflection render
			t.tmask=t.tmaskforcamerasnoreflectionlightrayshadowsflag+4;
			//  set all entities to allow full reflection
			for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
			{
				t.entityelement[t.e].donotreflect=0;
			}
		}
		SetTerrainMask (  t.tmask );

		//  hide LM objects from shadow render if pre-bake(lighting=1)
		if (  t.visuals.shaderlevels.lighting == 1 && t.game.set.ismapeditormode == 0 ) 
		{
			for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
			{
				if (  t.e <= ArrayCount(t.lmsceneobj) ) 
				{
					if (  t.lmsceneobj[t.e].startobj>0 ) 
					{
						for ( t.tobj = t.lmsceneobj[t.e].startobj; t.tobj <= t.lmsceneobj[t.e].finishobj; t.tobj++ )
						{
							if (  ObjectExist(t.tobj) == 1 ) 
							{
								t.tsize=ObjectSize(t.tobj,1);
								t.tsmallmask=t.tmaskforcamerasnoreflectionlightrayshadowsflag;
								if (  t.tsize>25 ) 
								{
									if (  t.visuals.lightraymode>0 ) 
									{
										t.tsmallmask=t.tsmallmask+16;
									}
								}
								if (  t.entityelement[t.e].donotreflect == 1 ) 
								{
									if (  t.visuals.lightraymode>0 ) 
									{
										t.tlargemask=t.tmaskforcamerasnoreflectionlightrayshadowsflag+16;
									}
									else
									{
										t.tlargemask=t.tmaskforcamerasnoreflectionlightrayshadowsflag;
									}
								}
								else
								{
									t.tlargemask=t.tmaskforcamerasnoshad;
								}
								if (  t.tsize<100 ) 
								{
									SetObjectMask (  t.tobj,t.tsmallmask,0,0,0 );
								}
								else
								{
									SetObjectMask (  t.tobj,t.tlargemask,0,0,0 );
								}
							}
						}
					}
				}
			}
			if (  g.lightmappedterrainoffset != -1 ) 
			{
				for ( t.tlmobj2 = g.lightmappedterrainoffset ; t.tlmobj2<=  g.lightmappedterrainoffsetfinish; t.tlmobj2++ )
				{
					if (  ObjectExist(t.tlmobj2) == 1 ) 
					{
						if ( g.iLightmappingExcludeTerrain == 0 )
						{
							if (  ObjectSize(t.tlmobj2,1)<100 ) 
							{
								SetObjectMask (  t.tlmobj2,t.tmaskforcamerasnoreflectionlightrayshadowsflag,0,0,0 );
							}
							else
							{
								SetObjectMask (  t.tlmobj2,t.tmaskforcamerasnoshad,0,0,0 );
							}
						}
						else
						{
							SetObjectMask ( t.tlmobj2,0,0,0,0 );
						}
					}
				}
			}
		}
	}
}

// Moved this code into its own function so lua can call it too rather than refreshing all visuals
void visuals_CheckSetGlobalDepthSkipSystem ( void )
{
	if ( t.visuals.DepthOfFieldIntensity_f==0.0f && t.visuals.MotionIntensity_f==0.0f && t.visuals.SAOIntensity_f==0.0f )
		SetGlobalDepthSkipSystem ( true );
	else
		SetGlobalDepthSkipSystem ( false );
}

void visuals_loop ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	// Shortcut-keys to adjust visual settings
	if ( t.conkit.editmodeactive == 0 ) 
	{
		if ( ScanCode() == 0  )  t.visuals.pressed = 0;
		if ( t.visuals.pressed == 0 ) 
		{
			if ( t.visuals.pressed == 1 ) 
			{
				if ( t.visuals.promptstatetimer == 0 ) 
				{
					t.visuals.showpromptssavestate=t.aisystem.showprompts;
				}
				t.visuals.promptstatetimer=Timer()+3000;
			}
			if ( KeyState(g.keymap[87]) == 1 ) 
			{ 
				// when F11 pressed, scroll through FPS show modes (and hardware info panel)
				t.visuals.pressed = 1; 
				g.tabmodeshowfps = g.tabmodeshowfps + 1;
				if ( g.tabmodeshowfps > 2 ) g.tabmodeshowfps = 0;
			}
		}
	}
	if ( t.visuals.promptstatetimer > 0 ) 
	{
		t.tvisualprompt_s="";
		t.tvisualprompt_s=t.tvisualprompt_s+" : "+StrEx(t.visuals.value_f,2);
		if ( Timer()>t.visuals.promptstatetimer ) 
		{
			t.visuals.promptstatetimer=0;
			t.aisystem.showprompts=t.visuals.showpromptssavestate;
		}
		t.tvisualalpha=255;
		if ( Timer()>t.visuals.promptstatetimer-255  )  t.tvisualalpha = (t.visuals.promptstatetimer-Timer());
		if ( t.tvisualalpha<0  )  t.tvisualalpha = 0;
		if ( t.tvisualalpha>255  )  t.tvisualalpha = 255;
		pastebitmapfont(t.tvisualprompt_s.Get(),20,(GetDisplayHeight()-50),1,t.tvisualalpha);
	}

	// General prompt
	if ( t.visuals.generalpromptstatetimer>0 ) 
	{
		if ( t.postprocessings.fadeinvalue_f == 1.0 ) 
		{
			t.tvisualprompt_s=t.visuals.generalprompt_s;
			if ( Timer()>t.visuals.generalpromptstatetimer ) 
			{
				t.visuals.generalpromptstatetimer=0;
				t.visuals.generalpromptalignment=0;
			}
			t.tvisualalpha=255;
			if ( Timer()>t.visuals.generalpromptstatetimer-255  )  t.tvisualalpha = (t.visuals.generalpromptstatetimer-Timer());
			if ( t.tvisualalpha<0  )  t.tvisualalpha = 0;
			if ( t.tvisualalpha>255  )  t.tvisualalpha = 255;
			t.txwidth=getbitmapfontwidth(t.tvisualprompt_s.Get(),1);
			if ( t.visuals.generalpromptalignment == 1  )  t.tprmpty = GetDisplayHeight()-50; else t.tprmpty = 50;
			pastebitmapfont(t.tvisualprompt_s.Get(),(GetDisplayWidth()-t.txwidth)/2,t.tprmpty,1,t.tvisualalpha);
		}
		else
		{
			t.visuals.generalpromptstatetimer=Timer()+3000;
		}
	}

	//  Apply visual settings when change detected
	if ( t.visuals.pressed == 1 || t.visuals.refreshshaders == 1 ) 
	{
		// One refresh per request
		t.visuals.refreshshaders=0;

		// If change in debug objects flag should update relevant modules
		darkai_updatedebugobjects ( );

		// 091115 - new system to SKIP the depth render of all qualifying shader (using a pass called "RenderDepthPixelsPass")
		visuals_CheckSetGlobalDepthSkipSystem();

		// Code to update ALL OBJECT MASKS to decide which cameras will get them
		// Done on a refresh as this process is expensive (20% of empty level)
		if ( 1 ) 
		{
			// Set the object masks based on graphic options (uses reflectionmodemodified)
			visuals_updateobjectmasks ( );

			// reflection settings can hide/show plane of water
			terrain_updatewaterphysics ( );
			if ( t.hardwareinfoglobals.nowater != 0 ) 
			{
				if ( ObjectExist(t.terrain.objectstartindex+5) == 1  )  HideObject (  t.terrain.objectstartindex+5 );
			}
			else
			{
				if ( ObjectExist(t.terrain.objectstartindex+5) == 1  )  ShowObject (  t.terrain.objectstartindex+5 );
			}

			// However, ensure reflection sky remains (even if game objects culled away)
			t.tskymaskforcamerasnoshadow = t.tmaskforcameras & 1073741823;
			if ( ObjectExist(t.terrain.objectstartindex+4) == 1 ) 
			{
				SetObjectMask (  t.terrain.objectstartindex+4,(t.tskymaskforcamerasnoshadow),0,0,0 );
			}
			if ( ObjectExist(t.terrain.objectstartindex+8) == 1 ) 
			{
				SetObjectMask (  t.terrain.objectstartindex+8,(t.tskymaskforcamerasnoshadow),0,0,0 );
			}
			if ( ObjectExist(t.terrain.objectstartindex+9) == 1 ) 
			{
				SetObjectMask (  t.terrain.objectstartindex+9,1);//(t.tskymaskforcamerasnoshadow),0,0,0 ); no relfection of scroll
			}

			// And, ensure terrain physics objects (used for occlusion) are never rendered
			if ( t.terrain.TerrainLODOBJStart>0 ) 
				for ( t.o = t.terrain.TerrainLODOBJStart ; t.o <= t.terrain.TerrainLODOBJFinish; t.o++ )
					if ( ObjectExist(t.o) == 1 ) 
						SetObjectMask ( t.o,0,0,0,0 );

			// Update in-game objects that only appear in main camera
			#ifdef VRTECH
			if ( g.globals.riftmode>0 || (g.vrglobals.GGVREnabled > 0 && g.vrglobals.GGVRUsingVRSystem == 1) )  
			{
				if ( g.vrglobals.GGVREnabled > 0 )
					t.tmaskmaincamera=1+(1<<t.glefteyecameraid)+(1<<t.grighteyecameraid);
				else
					t.tmaskmaincamera=0+(1<<t.glefteyecameraid)+(1<<t.grighteyecameraid);
			}
			#else
			if ( g.globals.riftmode>0 )  
			{
				t.tmaskmaincamera=0+(1<<t.glefteyecameraid)+(1<<t.grighteyecameraid);
			}
			#endif
			else
			{
				t.tmaskmaincamera=1;
			}

			// guns only for main camera (HUD objects for camera zero only)
			for ( t.o = g.gunbankoffset ; t.o <= g.editorwaypointoffset-1; t.o++ )
			{
				if ( ObjectExist(t.o) == 1 ) 
				{
					//SetObjectMask ( t.o, t.tmaskmaincamera, 0, 0, 0 ); //until have a VR friendly way to show HUD objects
					SetObjectMask ( t.o, 1, 0, 0, 0 );
				}
			}

			// Water plane only for main camera
			if ( ObjectExist(t.terrain.objectstartindex+5)>0 ) 
			{
				SetObjectMask ( t.terrain.objectstartindex+5, t.tmaskmaincamera );//1 ); need to see water in VR renders
			}

			#ifdef VRTECH
			// go through all entities to ensure they render to VR scenes
			for ( t.e = 1; t.e <= g.entityelementlist; t.e++ )
			{
				t.entid = t.entityelement[t.e].bankindex;
				if ( t.entid > 0 ) 
				{
					if ( t.entityprofile[t.entid].ischaractercreator == 1 ) 
					{
						for ( int iParts = 0; iParts <= 2; iParts++ )
						{
							t.tccobj = (g.charactercreatorrmodelsoffset+((t.e*3)-t.characterkitcontrol.offset))+iParts;
							if ( ObjectExist(t.tccobj) == 1 ) 
							{
								SetObjectMask ( t.tccobj, t.tmaskmaincamera+(1<<31) );
							}
						}
					}
				}
			}
			#endif
		}

		// Shader updates
		visuals_justshaderupdate ( );

		// Calculate 'reasonable' camera FOV (void zero and 100)
		g.greasonableWeaponFOV_f=t.visuals.WeaponFOV_f;

		// Set selected object FOV's
		for ( t.tgunid = 1 ; t.tgunid<=  g.gunmax; t.tgunid++ )
		{
			t.tgunobj=t.gun[t.tgunid].obj;
			if ( t.tgunobj>0 ) 
			{
				if ( ObjectExist(t.tgunobj) == 1 ) 
				{
					SetObjectFOV ( t.tgunobj,g.greasonableWeaponFOV_f );
				}
			}
		}

		// Muzzle Flash(es)
		for ( t.t = 0 ; t.t<=  1; t.t++ )
		{
			if ( t.t == 0  )  t.tobj = g.hudbankoffset+5;
			if ( t.t == 1  )  t.tobj = g.hudbankoffset+32;
			if ( ObjectExist(t.tobj) == 1  )  SetObjectFOV (  t.tobj,g.greasonableWeaponFOV_f );
		}

		// Brass
		for ( t.t = 6 ; t.t<=  20; t.t++ )
		{
			t.tobj=g.hudbankoffset+t.t;
			if ( ObjectExist(t.tobj) == 1  )  SetObjectFOV (  t.tobj,g.greasonableWeaponFOV_f );
		}

		// Smoke
		for ( t.t = 21 ; t.t<=  30; t.t++ )
		{
			t.tobj=g.hudbankoffset+t.t;
			if ( ObjectExist(t.tobj) == 1  )  SetObjectFOV (  t.tobj,g.greasonableWeaponFOV_f );
		}

		// Adjust resolution of reflection based on slider value
		if ( t.visuals.reflectionmode>0 ) 
		{
			if ( ImageExist(t.terrain.imagestartindex+6) == 1 ) 
			{
				if ( t.terrain.reflsizer != g.greflectionrendersize ) 
				{
					t.terrain.reflsizer=g.greflectionrendersize;
					SetCameraToImage ( 2,-1,0,0 );
					SetCameraToImage ( 2,t.terrain.imagestartindex+6,t.terrain.reflsizer,t.terrain.reflsizer );
					TextureObject ( t.terrain.objectstartindex+5,2,t.terrain.imagestartindex+6 );
				}
			}
		}

		// Set terrain LOD distances (700 is slightly larger than width of terrain segment piece)
		if ( t.terrain.TerrainID>0 ) 
		{
			BT_SetTerrainLODDistance ( t.terrain.TerrainID,1,1401.0+t.visuals.TerrainLOD1_f );
			BT_SetTerrainLODDistance ( t.terrain.TerrainID,2,1401.0+t.visuals.TerrainLOD2_f );
			SetTerrainRenderLevel ( t.visuals.TerrainSize_f );
		}

		// Ensures LOW FPS detector not fooled by setting changes
		g.lowfpstarttimer=Timer();

		// and trigger camera refresh (but flag can be triggered elsewhere, like LUA command to change camera)
		t.visuals.refreshmaincameras = 1;
		void Wicked_Update_Visuals(void *voidvisual);
		Wicked_Update_Visuals( (void *) &t.visuals );
	}

	// 070918 - have separate control of camera refresh
	if ( t.visuals.refreshmaincameras == 1 )
	{
		// Set camera settings
		g.greasonableCameraFOV_f=t.visuals.CameraFOV_f;
		for ( t.tcamid = 0 ; t.tcamid <= 4; t.tcamid++ )
		{
			if ( CameraExist(t.tcamid) == 1 && t.tcamid != 3 ) 
			{
				// 311017 - solve Z clash issues by adjusting near depth based on far depth
				//PE: removes flickering on "old bridge" in TBE.
				//PE: 8+ seams wo work without near geo disappering.
				//PE: 14 seams to be the largest possible when directly up to a flat wall.
				//PE: Default range 8-14 . use setup.ini lowestnearcamera to go lower then 8.
				float fFinalNearDepth = g.lowestnearcamera + t.visuals.CameraNEAR_f + ((t.visuals.CameraFAR_f/DEFAULT_FAR_PLANE)*6.0f); // PE: range 8-14
				SetCameraRange ( t.tcamid, fFinalNearDepth, t.visuals.CameraFAR_f );
				SetCameraAspect ( t.tcamid,t.visuals.CameraASPECT_f );
				SetCameraFOV ( t.tcamid,g.greasonableCameraFOV_f );
			}
		}
		t.visuals.refreshmaincameras = 0;
	}

	// Update vegetation when required. Wait for mouse release because it takes a long time when updating during a drag
	if ( t.visuals.refreshvegetation == 1 ) 
	{
		if (  MouseClick() == 0 ) 
		{
			// Set grass grid and fade
			grass_setgrassgridandfade ( );
			// Only update the vegetation grid if the view distance is far enough, otherwise just clear it
			if ( t.terrain.vegetationgridsize>1 ) 
			{
				SetEffectConstantF (  t.terrain.terrainshaderindex,"GrassFadeDistance",t.tGrassFadeDistance );
				if ( t.terrain.vegetationshaderindex>0 ) 
				{
					if ( GetEffectExist(t.terrain.vegetationshaderindex) == 1 ) 
					{
						SetEffectConstantF (  t.terrain.vegetationshaderindex,"GrassFadeDistance",t.tGrassFadeDistance );
						grass_init ( );
					}
				}
			}
			else
			{
				DeleteVegetationGrid (  );
			}
			t.visuals.refreshvegetation=0;
			g.lowfpstarttimer=Timer();
		}
		else
		{
			t.s_s="generating new grass geometry" ; lua_prompt ( );
		}
	}

	// refreshterrain super texture (when switch terrain bank)
	if ( t.visuals.refreshterrainsupertexture>0 ) 
	{
		if ( t.visuals.refreshterrainsupertexture == 2 ) 
		{
			//  generate super texture from above textures
			terrain_generatesupertexture ( false );
			t.visuals.refreshterrainsupertexture=0;
		}
		else
		{
			t.s_s="generating new terrain super texture" ; lua_prompt ( );
			t.visuals.refreshterrainsupertexture=2;
		}
	}

	// Update world settings (sky, terrain, veg texture)
	if ( t.visuals.refreshskysettings == 1 ) 
	{
		// change day sky
		g.skyindex = t.visuals.skyindex ; if (  g.skyindex>g.skymax  )  g.skyindex = g.skymax;
		t.visuals.sky_s=t.skybank_s[g.skyindex];
		t.terrainskyspecinitmode=0 ; sky_skyspec_init ( );
		t.sky.currenthour_f=8.0;
		t.sky.daynightprogress=0;
		t.sky.changingsky=1;
		t.visuals.refreshskysettings=0;
		g.lowfpstarttimer=Timer();

		// if change sky, regenerate env map
		cubemap_generateglobalenvmap();
	}

	// Update terrain textures
	if ( t.visuals.refreshterraintexture > 0 ) 
	{
		// 080320 - this prevented terrain from being changed in test game
		//if ( t.game.set.ismapeditormode == 1 ) t.visuals.refreshterraintexture = 2;
		t.visuals.refreshterraintexture = 2;
		if ( t.visuals.refreshterraintexture == 2 ) 
		{
			// first check if CUSTOM available (texture_d.dds present)
			#ifdef VRTECH
			bool bCustomTextureExists = false;
			if ( FileExist ( cstr(g.mysystem.levelBankTestMapAbs_s+"Texture_D.dds").Get() ) == 1 ) bCustomTextureExists = true;
			if ( FileExist ( cstr(g.mysystem.levelBankTestMapAbs_s+"Texture_D.jpg").Get() ) == 1 ) bCustomTextureExists = true;
			#ifdef ENABLECUSTOMTERRAIN
			if ( t.visuals.terrainindex > 1 || (t.visuals.terrainindex==1 && bCustomTextureExists == true) )
			#else
			if ( t.visuals.terrainindex >= 1 )
			#endif
			#else
			if ( t.visuals.terrainindex > 1 || (t.visuals.terrainindex==1 && FileExist ( cstr(g.mysystem.levelBankTestMapAbs_s+"Texture_D.dds").Get() ) == 1) )
			#endif
			{
				// change terrain textures
				g.terrainstyleindex=t.visuals.terrainindex;
				if ( g.terrainstyleindex>g.terrainstylemax  )  g.terrainstyleindex = g.terrainstylemax;
				t.visuals.terrain_s=t.terrainstylebank_s[g.terrainstyleindex];
				terrain_changestyle ( );
			}
			else
			{
				// revert to last one, custom texture not present
				if ( t.visuals.terrainindex == 1 )
				{
					t.visuals.terrainindex = g.terrainstyleindex;
					t.slidersmenuvalue[t.slidersmenunames.worldpanel][2].value_s = t.terrainstylebank_s[g.terrainstyleindex];
					t.slidersmenuvalue[t.slidersmenunames.worldpanel][2].value = g.terrainstyleindex;
				}
			}
			t.visuals.refreshterraintexture=0;
			g.lowfpstarttimer=Timer();
			t.visuals.refreshshaders=1;

			// if change terrain, regenerate env map
			cubemap_generateglobalenvmap();
		}
	}

	// Update veg texture
	if ( t.visuals.refreshvegtexture == 1 ) 
	{
		// change vegetation textures
		g.vegstyleindex=t.visuals.vegetationindex;
		if (  g.vegstyleindex>g.vegstylemax  )  g.vegstyleindex = g.vegstylemax;
		t.visuals.vegetation_s=t.vegstylebank_s[g.vegstyleindex];
		grass_changevegstyle ( );
		t.visuals.refreshvegtexture=0;
		g.lowfpstarttimer=Timer();
	}

	// Depth Of Field can be overridden any time by weapon
	#ifdef WICKEDENGINE
	#else
	if ( GetEffectExist(g.postprocesseffectoffset+0) == 1 ) 
	{
		t.tMotionIntensity_f=t.visuals.MotionIntensity_f;
		t.tDepthOfFieldDistance_f=t.visuals.DepthOfFieldDistance_f;
		t.tDepthOfFieldIntensity_f=t.visuals.DepthOfFieldIntensity_f;
		if ( t.gunid>0 && g.firemodes[t.gunid][g.firemode].settings.dofintensity>0 ) 
		{
			if ( t.gunzoommode>1 ) 
			{
				t.tMotionIntensity_f=0.0;
				t.tDepthOfFieldDistance_f=(g.firemodes[t.gunid][g.firemode].settings.dofdistance+0.0)/100.0;
				t.tpercfadein_f=(t.gunzoommode+0.0)/10.0;
				if ( t.tpercfadein_f<0.0  )  t.tpercfadein_f = 0.0;
				if ( t.tpercfadein_f>1.0  )  t.tpercfadein_f = 1.0;
				t.tnewDOFIntensity_f=(g.firemodes[t.gunid][g.firemode].settings.dofintensity+0.0)/100.0;
				t.tDepthOfFieldIntensity_f=(t.tnewDOFIntensity_f*t.tpercfadein_f)+(t.tDepthOfFieldIntensity_f*(1.0-t.tpercfadein_f));
			}
		}
		// Regular BLOOM or SAO shader (ie send values to SAO shader now in charge of post processing)
		int iPPS = 0; if ( t.visuals.SAOIntensity_f > 0.0f ) iPPS = 4;
		// More values to the post processing shader
		SetVector4 ( g.generalvectorindex+1,t.visuals.MotionDistance_f,t.tMotionIntensity_f,0,0 );
		SetEffectConstantV (  g.postprocesseffectoffset+iPPS,"Motion",g.generalvectorindex+1 );
		SetVector4 ( g.generalvectorindex+1,t.tDepthOfFieldDistance_f,t.tDepthOfFieldIntensity_f,0,0 );
		SetEffectConstantV (  g.postprocesseffectoffset+iPPS,"DepthOfField",g.generalvectorindex+1 );
		SetVector4 ( g.generalvectorindex+1,t.tFogNear_f,t.tFogFar_f,0,t.tFogA_f/255.0 );
		SetEffectConstantV ( g.postprocesseffectoffset+iPPS,"HudFogDistAndAlpha",g.generalvectorindex+1 );
		SetVector4 ( g.generalvectorindex+1, t.visuals.SAORadius_f, t.visuals.SAOIntensity_f, t.visuals.SAOQuality_f, t.visuals.LensFlare_f );
		SetEffectConstantV (  g.postprocesseffectoffset+iPPS,"SAOSettings",g.generalvectorindex+1 );
	}
	#endif
}

void visuals_shaderlevels_update_core (bool bUpdateEngine)
{
	// HIGHEST
	// t.visuals.shaderlevels.entities = 1;
	// t.visuals.shaderlevels.terrain = 1;
	// t.visuals.shaderlevels.vegetation = 1;
	// MEDIUM
	// t.visuals.shaderlevels.entities = 2;
	// t.visuals.shaderlevels.terrain = 3;
	// t.visuals.shaderlevels.vegetation = 3;
	// LOWEST
	// t.visuals.shaderlevels.entities = 3;
	// t.visuals.shaderlevels.terrain = 4;
	// t.visuals.shaderlevels.vegetation = 4;

	// Settings for Editor and Game
	extern bool bEnable30FpsAnimations;
	extern bool bShadowsLowestLOD;
	extern bool bProbesLowestLOD;
	extern bool bRaycastLowestLOD;
	extern bool bPhysicsLowestLOD;
	extern bool bReflectionsLowestLOD;

	extern bool g_bDelayedShadows;
	extern bool g_bDelayedShadowsLaptop;
	extern bool bEnableTerrainChunkCulling;
	extern bool bEnablePointShadowCulling;
	extern bool bEnableSpotShadowCulling;
	extern bool bEnableObjectCulling;
	extern bool bEnableAnimationCulling;
	extern float maxApparentSize;
	extern float fLODMultiplier;
	extern bool bThreadedPhysics;
	bool bPerformAnUpdate = false;
	int iChangeSkyType = -1;
	if (t.visuals.shaderlevels.entities == 2) // CUSTOM (MEDIUM)
	{
		// Custom settings - do not override
	}
	else
	{
		if (t.visuals.shaderlevels.entities == 1) // HIGHEST
		{
			bShadowsLowestLOD = false;
			bProbesLowestLOD = false;
			bRaycastLowestLOD = true;
			bReflectionsLowestLOD = false;

			bEnable30FpsAnimations = false;
			bPhysicsLowestLOD = false;
			g_bDelayedShadows = false;
			g_bDelayedShadowsLaptop = false;
			float fASize = 0.08f;
			maxApparentSize = fASize / 10000.0f;
			t.visuals.bLevelVSyncEnabled = true;
			t.visuals.bReflectionsEnabled = true;
			//PE: Only change if user has not selected a custom skybox and use bDisableSkybox
			if (t.visuals.skyindex == 0)
			{
				t.visuals.bDisableSkybox = false;
				iChangeSkyType = 0;
			}
			fLODMultiplier = 3.0f;
		}
		if (t.visuals.shaderlevels.entities == 3) // LOW
		{
			bShadowsLowestLOD = true;
			bProbesLowestLOD = true;
			bRaycastLowestLOD = true;
			bPhysicsLowestLOD = true;
			bReflectionsLowestLOD = true;

			bEnable30FpsAnimations = true;

			g_bDelayedShadows = true;
			g_bDelayedShadowsLaptop = true;
			float fASize = 1.0f;
			maxApparentSize = fASize / 10000.0f;
			t.visuals.bLevelVSyncEnabled = false;
			t.visuals.bReflectionsEnabled = false;
			//PE: Only change if user has not selected a custom skybox and use bDisableSkybox
			if (t.visuals.skyindex == 0)
			{
				t.visuals.bDisableSkybox = true;
				iChangeSkyType = 1;
			}
			fLODMultiplier = 1.0f;

		}
		t.visuals.bOcclusionCulling = true;
		bEnableObjectCulling = true;
		bEnableTerrainChunkCulling = true;
		bEnablePointShadowCulling = true;
		bEnableSpotShadowCulling = true;
		bEnableAnimationCulling = true;
		t.gamevisuals.bDisableSkybox = t.visuals.bDisableSkybox;
		bPerformAnUpdate = true;
	}
	if ( bPerformAnUpdate == true )
	{
		t.gamevisuals.bEnable30FpsAnimations = t.visuals.bEnable30FpsAnimations = bEnable30FpsAnimations;

		t.gamevisuals.bPhysicsLowestLOD = t.visuals.bPhysicsLowestLOD = bPhysicsLowestLOD;
		t.gamevisuals.bShadowsLowestLOD = t.visuals.bShadowsLowestLOD = bShadowsLowestLOD;
		t.gamevisuals.bProbesLowestLOD = t.visuals.bProbesLowestLOD = bProbesLowestLOD;
		t.gamevisuals.bRaycastLowestLOD = t.visuals.bRaycastLowestLOD = bRaycastLowestLOD;
		t.gamevisuals.bReflectionsLowestLOD = t.visuals.bReflectionsLowestLOD = bReflectionsLowestLOD;

		t.gamevisuals.g_bDelayedShadows = t.visuals.g_bDelayedShadows = g_bDelayedShadows;
		t.gamevisuals.g_bDelayedShadowsLaptop = t.visuals.g_bDelayedShadowsLaptop = g_bDelayedShadowsLaptop;
		t.gamevisuals.bEnableObjectCulling = t.visuals.bEnableObjectCulling = bEnableObjectCulling;
		t.gamevisuals.bEnableTerrainChunkCulling = t.visuals.bEnableTerrainChunkCulling = bEnableTerrainChunkCulling;
		t.gamevisuals.bEnablePointShadowCulling = t.visuals.bEnablePointShadowCulling = bEnablePointShadowCulling;
		t.gamevisuals.bEnableSpotShadowCulling = t.visuals.bEnableSpotShadowCulling = bEnableSpotShadowCulling;
		t.gamevisuals.bEnableObjectCulling = t.visuals.bEnableObjectCulling = bEnableObjectCulling;
		t.gamevisuals.bEnableAnimationCulling = t.visuals.bEnableAnimationCulling = bEnableAnimationCulling;
		t.gamevisuals.fLODMultiplier = t.visuals.fLODMultiplier = fLODMultiplier;

		t.gamevisuals.bThreadedPhysics = t.visuals.bThreadedPhysics = bThreadedPhysics;
		
		t.gamevisuals.ApparentSize = t.visuals.ApparentSize = maxApparentSize;
		t.gamevisuals.bReflectionsEnabled = t.visuals.bReflectionsEnabled;
		if (bEnableTerrainChunkCulling && !t.visuals.bOcclusionCulling)
		{
			t.gamevisuals.bOcclusionCulling = t.visuals.bOcclusionCulling = true;
		}
		if (bEnablePointShadowCulling && !t.visuals.bOcclusionCulling)
		{
			t.gamevisuals.bOcclusionCulling = t.visuals.bOcclusionCulling = true;
		}
		if (bEnableSpotShadowCulling && !t.visuals.bOcclusionCulling)
		{
			t.gamevisuals.bOcclusionCulling = t.visuals.bOcclusionCulling = true;
		}
		if (bEnableObjectCulling && !t.visuals.bOcclusionCulling)
		{
			t.gamevisuals.bOcclusionCulling = t.visuals.bOcclusionCulling = true;
		}
		if (bEnableAnimationCulling && !t.visuals.bOcclusionCulling)
		{
			t.gamevisuals.bOcclusionCulling = t.visuals.bOcclusionCulling = true;
		}
		t.gamevisuals.bOcclusionCulling = t.visuals.bOcclusionCulling;
		t.gamevisuals.bLevelVSyncEnabled = t.visuals.bLevelVSyncEnabled;
		if (bUpdateEngine == true)
		{
			wiRenderer::SetOcclusionCullingEnabled(t.visuals.bOcclusionCulling);
			extern void gridedit_setvsync (bool);
			gridedit_setvsync(t.visuals.bLevelVSyncEnabled);
			extern void gridedit_setreflection (bool);
			gridedit_setreflection(t.visuals.bReflectionsEnabled);
			extern void gridedit_setsky (int);
			if(iChangeSkyType >= 0)
				gridedit_setsky(iChangeSkyType);
			g.projectmodified = 1;
		}
	}

	// Only do this in standalone as it will otherwise mess up editor settings!
	if (bUpdateEngine == true && t.game.gameisexe == 1)
	{
		// Wicked controls some early performance levers:
		static bool bInitGraphicsSettingsValues = true;
		static float fInitialShadowPointResolution = 0;
		static float fInitialShadowSpotResolution = 0;
		static float fInitialGrassDrawDistanceValue = 0;
		static float fInitialCameraFar = t.visuals.CameraFAR_f;
		if (bInitGraphicsSettingsValues == true)
		{
			if (t.visuals.shaderlevels.entities == 1)
			{
				fInitialShadowPointResolution = t.visuals.iShadowPointResolution;
				fInitialShadowSpotResolution = t.visuals.iShadowSpotResolution;
			}
			if (t.visuals.shaderlevels.entities == 2)
			{
				fInitialShadowPointResolution = t.visuals.iShadowPointResolution * 2;
				fInitialShadowSpotResolution = t.visuals.iShadowSpotResolution * 2;
			}
			if (t.visuals.shaderlevels.entities == 3)
			{
				fInitialShadowPointResolution = t.visuals.iShadowPointResolution * 4;
				fInitialShadowSpotResolution = t.visuals.iShadowSpotResolution * 4;
			}
			if (t.visuals.shaderlevels.vegetation == 1) fInitialGrassDrawDistanceValue = GGGrass::gggrass_global_params.lod_dist;
			if (t.visuals.shaderlevels.vegetation == 2 || t.visuals.shaderlevels.vegetation == 3) fInitialGrassDrawDistanceValue = GGGrass::gggrass_global_params.lod_dist / 2;
			if (t.visuals.shaderlevels.vegetation == 4) fInitialGrassDrawDistanceValue = GGGrass::gggrass_global_params.lod_dist / 3;

			// all initial values set for possible changes below
			bInitGraphicsSettingsValues = false;
		}

		// "entities" controls shadow work
		if (fInitialShadowPointResolution > 0)
		{
			t.visuals.iShadowPointResolution = fInitialShadowPointResolution;
			if (t.visuals.shaderlevels.entities == 2) t.visuals.iShadowPointResolution = fInitialShadowPointResolution / 2;
			if (t.visuals.shaderlevels.entities == 3) t.visuals.iShadowPointResolution = fInitialShadowPointResolution / 4;
		}
		if (fInitialShadowSpotResolution > 0)
		{
			t.visuals.iShadowSpotResolution = fInitialShadowSpotResolution;
			if (t.visuals.shaderlevels.entities == 2) t.visuals.iShadowSpotResolution = fInitialShadowSpotResolution / 2;
			if (t.visuals.shaderlevels.entities == 3) t.visuals.iShadowSpotResolution = fInitialShadowSpotResolution / 4;
		}
		void Wicked_Update_Visuals(void* voidvisual);
		Wicked_Update_Visuals((void*)&t.visuals);

		// "vegetation" controls draw distance
		extern GGGrass::GGGrassParams gggrass_global_params;
		GGGrass::gggrass_global_params.lod_dist = fInitialGrassDrawDistanceValue;
		if (t.visuals.shaderlevels.vegetation >= 2) GGGrass::gggrass_global_params.lod_dist = fInitialGrassDrawDistanceValue * 2;
		if (t.visuals.shaderlevels.vegetation == 4) GGGrass::gggrass_global_params.lod_dist = fInitialGrassDrawDistanceValue * 3;

		// "terrain" controls camera distance (does not alter t.visuals.CameraFAR_f)
		float fUseCameraFar = fInitialCameraFar;
		if (t.visuals.shaderlevels.terrain >= 2) fUseCameraFar = fUseCameraFar / 2;
		if (t.visuals.shaderlevels.terrain == 4) fUseCameraFar = fUseCameraFar / 4;
		wiScene::GetCamera().zFarP = fUseCameraFar;
		extern CCameraManager m_CameraManager;
		tagCameraData* m_ptr = m_CameraManager.GetData(0);
		WickedCall_SetCameraFOV(m_ptr->fFOV);
	}
}

void visuals_shaderlevels_setlevel (int iActionTypeInternalByName, bool bUpdateEngine)
{
	if (iActionTypeInternalByName == 1)
	{
		t.visuals.shaderlevels.entities = 1;
		t.visuals.shaderlevels.terrain = 1;
		t.visuals.shaderlevels.vegetation = 1;
	}
	if (iActionTypeInternalByName == 3)
	{
		t.visuals.shaderlevels.entities = 2;
		t.visuals.shaderlevels.terrain = 3;
		t.visuals.shaderlevels.vegetation = 3;
	}
	if (iActionTypeInternalByName == 4)
	{
		t.visuals.shaderlevels.entities = 3;
		t.visuals.shaderlevels.terrain = 4;
		t.visuals.shaderlevels.vegetation = 4;
	}
	visuals_shaderlevels_update_core(bUpdateEngine);
}

void visuals_shaderlevels_update (void)
{
	bool bUpdateEngineToo = true;
	visuals_shaderlevels_update_core (bUpdateEngineToo);
}

void visuals_underwater_on ( void )
{
	// save all our shader settings then set the underwater fog action
	if ( t.visuals.underwatermode  ==  0 ) 
	{
		t.tDrowning_OldReflectionMode = t.visuals.reflectionmode;
		t.tDrowning_OldFogFar_f = t.visuals.FogDistance_f;
		t.tDrowning_OldFogNear_f = t.visuals.FogNearest_f;
		t.tDrowning_OldFogR_f = t.visuals.FogR_f;
		t.tDrowning_OldFogG_f = t.visuals.FogG_f;
		t.tDrowning_OldFogB_f = t.visuals.FogB_f;
		t.tDrowning_OldFogA_f = t.visuals.FogA_f;
		t.tDrowning_OldWobbleHeight = t.playercontrol.wobbleheight_f;
		//PE: Terrain reflection from underwater looks strange , so render reflections without terrain.
		t.visuals.reflectionmode = 1;
		// don't change fog underwater, handled by the shader
		//PE: SSAO looks wrong underwater so perhaps disable it. or increase fog distance.
		//PE: remove head bobbing
		t.playercontrol.wobbleheight_f = 0.0;

		if (g.underwatermode == 1) 
		{
			//PE: You can control how the post process make waves to make it look like we are underwater.
			SetVector4(g.terrainvectorindex1, 1.0, 40.0, 0.0135, 0.17);  //PE: Active=1,Speed,Distortion,Scale
			SetEffectConstantV(g.postprocesseffectoffset + 0, "UnderWaterSettings", g.terrainvectorindex1); //PE: post bloom.
			SetEffectConstantV(g.postprocesseffectoffset + 4, "UnderWaterSettings", g.terrainvectorindex1); //PE: also post sao.
		}

		// water gravity control
		if (t.playercontrol.gravityactive == 1)
			ODESetWorldGravity(0, -0.5f, 0, 150.0);
		else
			ODESetWorldGravity(0, 0, 0);

		t.tFogR_f = t.visuals.FogR_f; t.tFogG_f = t.visuals.FogG_f; t.tFogB_f = t.visuals.FogB_f ; t.tFogA_f = t.visuals.FogA_f;
		t.tFogNear_f = t.visuals.FogNearest_f; t.tFogFar_f = t.visuals.FogDistance_f;
		terrain_setfog ( );
		terrain_water_setfog ( );
		t.visuals.underwatermode = 1;

		//PE: objects underwater also need fog.
		visuals_justshaderupdate(); 
	}
}

void visuals_underwater_off ( void )
{
	// reset all our shaders back
	if ( t.visuals.underwatermode  ==  1 ) 
	{
		t.visuals.reflectionmode = t.tDrowning_OldReflectionMode;
		t.visuals.FogDistance_f = t.tDrowning_OldFogFar_f;
		t.visuals.FogNearest_f = t.tDrowning_OldFogNear_f;
		t.visuals.FogR_f = t.tDrowning_OldFogR_f;
		t.visuals.FogG_f = t.tDrowning_OldFogG_f;
		t.visuals.FogB_f = t.tDrowning_OldFogB_f;
		t.visuals.FogA_f = t.tDrowning_OldFogA_f;
		t.playercontrol.wobbleheight_f = t.tDrowning_OldWobbleHeight; //PE: restore head bobbing

		t.tFogR_f = t.visuals.FogR_f; t.tFogG_f = t.visuals.FogG_f ; t.tFogB_f = t.visuals.FogB_f ; t.tFogA_f = t.visuals.FogA_f;
		t.tFogNear_f = t.visuals.FogNearest_f; t.tFogFar_f = t.visuals.FogDistance_f;
		terrain_setfog ( );
		terrain_water_setfog ( );
		t.visuals.underwatermode = 0;

		// restore to ground gravity
		if (t.playercontrol.gravityactive == 1)
			ODESetWorldGravity(0, -20, 0 , 0);
		else
			ODESetWorldGravity(0, 0, 0);

		//PE: Restore normal fog and disable screen wave effect.
		visuals_justshaderupdate();
		if (g.underwatermode == 1) 
		{
			SetVector4(g.terrainvectorindex, 0.0, 155.0, 0.0095, 0.0);
			SetEffectConstantV(g.postprocesseffectoffset + 0, "UnderWaterSettings", g.terrainvectorindex); //PE: post bloom.
			SetEffectConstantV(g.postprocesseffectoffset + 4, "UnderWaterSettings", g.terrainvectorindex); //PE: also post sao.
		}
	}
}
