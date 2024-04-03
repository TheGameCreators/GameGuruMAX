//----------------------------------------------------
//--- GAMEGURU - G-Lighting
//----------------------------------------------------

#include "stdafx.h"
#include "gameguru.h"
#include "CObjectsC.h"

#ifdef WICKEDENGINE
#include "GGTerrain/GGTerrain.h"
#include "GGTerrain/GGGrass.h"
#endif

#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

// Globals
float g_fGlobalGammaFadeIn = 0;
float g_fGlobalGammaFadeInDest = 0;

// 
//  LIGHTING
// 

void lighting_init ( void )
{
	//  Create all lights
	lighting_refresh ( );

	// and reset to defaults before level starts
	t.playerlight.flashlightcontrol_f = 0;
	t.playerlight.flashlightcontrol_range_f = 700;
	t.playerlight.flashlightcontrol_radius_f = 45;
	t.playerlight.flashlightcontrol_colorR_f = 64;
	t.playerlight.flashlightcontrol_colorG_f = 64;
	t.playerlight.flashlightcontrol_colorB_f = 64;
	t.playerlight.flashlightcontrol_cashshadow = true;
}

void lighting_free ( void )
{
	// clear infinilights array
	if ( ArrayCount(t.infinilight) >= 0 ) 
	{
		for (int l = 1; l <= g.infinilightmax; l++)
		{
			infinilighttype* pLightPtr = &t.infinilight[l];
			if (pLightPtr->wickedlightindex > 0)
			{
				WickedCall_DeleteLight(pLightPtr->wickedlightindex);
				pLightPtr->wickedlightindex = 0;
			}
		}
		UnDim ( t.infinilight );
	}
	g.infinilightmax=0;

	//  switch flash light off
	t.playerlight.flashlightcontrol_f=0;
}

void lighting_override ( void )
{
	// does nothing in wicked implementation
}

bool bFlashLightCreated = false;
bool bFlashLightIsOn = false;
uint64_t uiFlashLight;

void lighting_loop(void)
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	int iUpdateFlashLightMode = 0;

	// Handle any spot flashes
	lighting_spotflash_control();

	if (t.playerlight.flashlightcontrol_f > 0.0f)
	{
		if (!bFlashLightIsOn) 
		{
			if (bFlashLightCreated == false)
			{
				uiFlashLight = WickedCall_AddLight(2);
				bFlashLightCreated = true;
			}
			bFlashLightIsOn = true;
		}
		if (uiFlashLight) 
		{
			// flashlight on
			iUpdateFlashLightMode = 1;
		}
	}
	else 
	{
		if (uiFlashLight)
		{
			// flashlight off
			iUpdateFlashLightMode = 2;
		}
		if (bFlashLightIsOn) 
		{
			//LB: fix issue of stutter (env probe recalc hit), keep the light, just make it dark :)
			iUpdateFlashLightMode = 3;
			bFlashLightIsOn = false;
		}
	}
	
	// Handle Wicked spotlight controlling flashlight
	if (iUpdateFlashLightMode > 0)
	{
		if (iUpdateFlashLightMode != 3 )
		{
			//PE: Position light in the right hand.
			MoveCamera(5.0);
			MoveCameraRight(0, 8.0);
			float fCamX = CameraPositionX(0);
			float fCamY = CameraPositionY(0) - 7.0f;
			float fCamZ = CameraPositionZ(0);
			float fCamAX = CameraAngleX(0);
			float fCamAY = CameraAngleY(0);
			float fCamAZ = CameraAngleZ(0);
			MoveCameraLeft(0, 8.0);
			MoveCamera(-5.0);
			float fFlashLightRange = t.playerlight.flashlightcontrol_range_f;
			float fFlashLightRadius = t.playerlight.flashlightcontrol_radius_f;
			float fFlashLightColR = t.playerlight.flashlightcontrol_colorR_f;
			float fFlashLightColG = t.playerlight.flashlightcontrol_colorG_f;
			float fFlashLightColB = t.playerlight.flashlightcontrol_colorB_f;
			bool bFlashLightCastShadow = t.playerlight.flashlightcontrol_cashshadow;
			if (iUpdateFlashLightMode == 2)
			{
				fFlashLightColR = 0;
				fFlashLightColG = 0;
				fFlashLightColB = 0;
			}
			WickedCall_UpdateLight(uiFlashLight, fCamX, fCamY, fCamZ, fCamAX, fCamAY, fCamAZ, fFlashLightRange, fFlashLightRadius, fFlashLightColR, fFlashLightColG, fFlashLightColB, bFlashLightCastShadow);
		}
		else
		{
			WickedCall_UpdateLight(uiFlashLight, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, 64, 64, 64, false);
		}
	}

	lighting_weaponFlash_loop(); //Handles weapon gun flash

	// Wicked can host ALL the lights in the scene, so pass them to wicked
	if ( g.infinilightmax > 0 && g.infinilightmax <= ArrayCount(t.infinilight) )
	{
		for ( int l = 1; l <= g.infinilightmax; l++ )
		{
			infinilighttype* pLightPtr = &t.infinilight[l];
			if (pLightPtr->wickedlightindex > 0)
			{
				if (pLightPtr->islit)
				{
					WickedCall_UpdateLight(pLightPtr->wickedlightindex, pLightPtr->x, pLightPtr->y, pLightPtr->z, pLightPtr->f_angle_x, pLightPtr->f_angle_y, pLightPtr->f_angle_z, pLightPtr->range, pLightPtr->spotlightradius, pLightPtr->colrgb.r, pLightPtr->colrgb.g, pLightPtr->colrgb.b, pLightPtr->bCanShadow);
				}
				else
				{
					//PE: For now just move it way out there.
					WickedCall_UpdateLight(pLightPtr->wickedlightindex, -999999.0, -999999.0, -999999.0, pLightPtr->f_angle_x, pLightPtr->f_angle_y, pLightPtr->f_angle_z, pLightPtr->range, pLightPtr->spotlightradius, pLightPtr->colrgb.r, pLightPtr->colrgb.g, pLightPtr->colrgb.b, false);
				}
			}
		}
	}

	//PE: Move any entity cursor light
	if (t.gridentitywickedlightindex > 0)
	{
		float lightx = t.gridentityposx_f;
		float lighty = t.gridentityposy_f;
		float lightz = t.gridentityposz_f;
		float lightax = t.gridentityrotatex_f;
		float lightay = t.gridentityrotatey_f;
		float lightaz = t.gridentityrotatez_f;
		float lightrange = t.grideleprof.light.range;
		float lightspotradius = t.grideleprof.light.offsetup;
		int colr = ((t.grideleprof.light.color & 0x00ff0000) >> 16);
		int colg = ((t.grideleprof.light.color & 0x0000ff00) >> 8);
		int colb = (t.grideleprof.light.color & 0x000000ff);
		bool bCastShadow = true;
		if (t.grideleprof.castshadow == 1) bCastShadow = false;
		WickedCall_UpdateLight(t.gridentitywickedlightindex, lightx, lighty, lightz, lightax, lightay, lightaz, lightrange, lightspotradius, colr, colg, colb, bCastShadow);
	}

	// if particle, update it
	if (t.gridentityobj > 0)
	{
		if (t.grideleprof.newparticle.emitterid != -1)
		{
			entity_updateparticleemitterbyID (&t.grideleprof, t.gridentityobj, t.gridentityscalex_f - 100.0f, t.gridentityposx_f, t.gridentityposy_f, t.gridentityposz_f, ObjectAngleX(t.gridentityobj), ObjectAngleY(t.gridentityobj), ObjectAngleZ(t.gridentityobj));
		}
	}

	// and detect if light probe scale changes
	extern bool g_bLightProbeScaleChanged;
	if (g_bLightProbeScaleChanged == true)
	{
		// we handle env probes in the terrain system, just maintain a list of all needed probes
		// and that system will take care of assigning the best actual probe for the job
		GGTerrain::GGTerrain_ClearEnvProbeList();
		for (int ee = 1; ee <= g.entityelementlist; ee++)
		{
			int entid = t.entityelement[ee].bankindex;
			if (entid > 0)
			{
				if (t.entityprofile[entid].ismarker == 2)
				{
					float fLightProbeRange = t.entityelement[ee].eleprof.light.fLightHasProbe;
					float fLightProbeRangeX = t.entityelement[ee].eleprof.light.fLightHasProbeX;
					float fLightProbeRangeY = t.entityelement[ee].eleprof.light.fLightHasProbeY;
					float fLightProbeRangeZ = t.entityelement[ee].eleprof.light.fLightHasProbeZ;
					if (fLightProbeRange >= 50)
					{
						float fSX = fLightProbeRangeX;// t.entityelement[ee].scalex;
						float fSY = fLightProbeRangeY;// t.entityelement[ee].scaley;
						float fSZ = fLightProbeRangeZ;// t.entityelement[ee].scalez;
						GGTerrain::GGTerrain_AddEnvProbeList(t.entityelement[ee].x, t.entityelement[ee].y, t.entityelement[ee].z, fLightProbeRange, t.entityelement[ee].quatx, t.entityelement[ee].quaty, t.entityelement[ee].quatz, t.entityelement[ee].quatw, fSX, fSY, fSZ);
					}
				}
			}
		}
		g_bLightProbeScaleChanged = false;
	}
	extern bool g_bLightProbeInstantChange;
	if (g_bLightProbeInstantChange == true)
	{
		// rather than a rebuild of the probe list, trigger an instant refresh (used when lights go on and off)
		extern int g_iLightProbeInstantChangeCoolDown;
		if (g_iLightProbeInstantChangeCoolDown == 0)
		{
			g_iLightProbeInstantChangeCoolDown = 1;
		}
		else
		{
			GGTerrain::GGTerrain_InstantEnvProbeRefresh(g_iLightProbeInstantChangeCoolDown);
			g_iLightProbeInstantChangeCoolDown++;
			if (g_iLightProbeInstantChangeCoolDown > 16)
			{
				g_iLightProbeInstantChangeCoolDown = 0;
				g_bLightProbeInstantChange = false;
			}
		}
	}
	extern bool bImGuiInTestGame;
	if (t.widget.pickedEntityIndex > 0 && t.entityprofile[t.entityelement[t.widget.pickedEntityIndex].bankindex].ismarker == 2 && bImGuiInTestGame == false)
	{
		// and only if the light object is a probe
		if (t.entityprofile[t.entityelement[t.widget.pickedEntityIndex].bankindex].light.fLightHasProbe >= 50)
		{
			wiRenderer::SetToDrawDebugEnvProbes(true);
		}
	}
	else
	{
		wiRenderer::SetToDrawDebugEnvProbes(false);
	}

	// separated out gamma control so can fade in (from level load and other future places)
	if (g_fGlobalGammaFadeIn < g_fGlobalGammaFadeInDest)
	{
		g_fGlobalGammaFadeIn += 0.05f;
		if (g_fGlobalGammaFadeIn > g_fGlobalGammaFadeIn)
		{
			g_fGlobalGammaFadeIn = g_fGlobalGammaFadeInDest;
		}
		if (g_fGlobalGammaFadeIn >= 0)
			wiRenderer::SetGamma(g_fGlobalGammaFadeIn);
		else
			wiRenderer::SetGamma(0);
	}
}

void lighting_weaponFlash_loop(void)
{
	//Player Weapon Flash
	static bool bWeaponFlashCreated = false;
	static bool bWeaponFlashIsOn = false;
	static uint64_t uiWeaponFlash = 0;

	if (t.playerWeaponFlash.flashlightcontrol_f > 0.0f)
	{
		if (!bWeaponFlashIsOn)
		{
			if (!bWeaponFlashCreated)
			{
				uiWeaponFlash = WickedCall_AddLight(1);
				//PE: BUG - Each time you fire the gun a new light is created, 
				if (uiWeaponFlash > 0) bWeaponFlashCreated = true;
			}

			bWeaponFlashIsOn = true;
		}

		if (uiWeaponFlash)
		{
			WickedCall_UpdateLight(uiWeaponFlash,					 //Light index
								   t.playerWeaponFlash.spotflashx_f, //Pos X
								   t.playerWeaponFlash.spotflashy_f, //Pos Y
								   t.playerWeaponFlash.spotflashz_f, //Pos Z
								   CameraAngleX(0),					 //Angle X
								   CameraAngleY(0),					 //Angle Y
								   CameraAngleZ(0),					 //Angle Z
								   400.0f,							 //Range
								   2.0f,							 //Radius
								   t.playerWeaponFlash.spotlightr_f, //Color R
								   t.playerWeaponFlash.spotlightg_f, //Color G
								   t.playerWeaponFlash.spotlightb_f, //Color B
								   true);							 //Cast shadows
		}
	}
	else
	{
		if (bWeaponFlashIsOn)
		{
			WickedCall_UpdateLight(uiWeaponFlash, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, 64, 64, 64, false); //Resets light
			bWeaponFlashIsOn = false;
		}
	}
}

void lighting_spotflash ( void )
{
	t.playerlight.mode=0;
	t.playerlight.spotflash=100;
	lighting_spotflash_control ( );
}

void lighting_spotflash_forenemies ( void )
{
	// 301115 - added support for enemy spot flashes
	t.playerlight.mode=0;
	t.playerlight.spotflash=100;
	lighting_spotflash_control ( );
}

void lighting_spotflashtracking ( void )
{
	if (  t.playerlight.mode == 0 ) 
	{
		t.playerlight.modeindex=t.tmodeindex;
		t.playerlight.mode=1;
	}
	if (  t.playerlight.mode == 1 && t.playerlight.modeindex == t.tmodeindex ) 
	{
		t.playerlight.spotflash=99;
		t.playerlight.spotflashx_f=t.tx_f ; t.playerlight.spotflashy_f=t.ty_f ; t.playerlight.spotflashz_f=t.tz_f;
		t.playerlight.spotlightr_f=255;
		t.playerlight.spotlightg_f=128;
		t.playerlight.spotlightb_f=0;
	}
	lighting_spotflash_control ( );
}

void lighting_spotflashexplosion ( void )
{
	if (  t.playerlight.mode == 0 ) 
	{
		t.playerlight.mode=2;
		t.playerlight.modeindex=t.tmodeindex;
		t.playerlight.spotflashx_f=t.tx_f ; t.playerlight.spotflashy_f=t.ty_f ; t.playerlight.spotflashz_f=t.tz_f;
		t.tdx_f=CameraPositionX(t.terrain.gameplaycamera)-t.playerlight.spotflashx_f;
		t.tdy_f=CameraPositionY(t.terrain.gameplaycamera)-t.playerlight.spotflashy_f;
		t.tdz_f=CameraPositionZ(t.terrain.gameplaycamera)-t.playerlight.spotflashz_f;
		t.tdd_f=Sqrt(abs(t.tdx_f*t.tdx_f)+abs(t.tdy_f*t.tdy_f)+abs(t.tdz_f*t.tdz_f));
		t.tdx_f=t.tdx_f/t.tdd_f ; t.tdy_f=t.tdy_f/t.tdd_f ; t.tdz_f=t.tdz_f/t.tdd_f;
		t.playerlight.spotflashx_f=t.playerlight.spotflashx_f+(t.tdx_f*25);
		t.playerlight.spotflashy_f=t.playerlight.spotflashy_f+(t.tdy_f*25);
		t.playerlight.spotflashz_f=t.playerlight.spotflashz_f+(t.tdz_f*25);
		t.playerlight.spotflash=199;
		t.playerlight.spotlightr_f=255;
		t.playerlight.spotlightg_f=128;
		t.playerlight.spotlightb_f=0;
	}
	lighting_spotflash_control ( );
}

void lighting_spotflash_control ( void )
{
	//  Control spot flash modes
	if (  t.playerlight.mode == 0 ) 
	{
		//  gun flash system
		if (  t.playerlight.spotflash == 100 ) 
		{
			t.playerlight.spotflash=99 ; t.playerlight.spotflashx_f=t.tx_f ; t.playerlight.spotflashy_f=t.ty_f ; t.playerlight.spotflashz_f=t.tz_f;
			t.playerlight.spotlightr_f=t.tcolr ; t.playerlight.spotlightg_f=t.tcolg ; t.playerlight.spotlightb_f=t.tcolb;
		}
		if (  t.playerlight.spotflash>0 && t.playerlight.spotflash<100 ) 
		{
			t.playerlight.spotflash=t.playerlight.spotflash-(20.0*g.timeelapsed_f);
			if (  t.playerlight.spotflash>0 ) 
			{
				t.playerlight.spotfadeout_f=t.playerlight.spotflash/200.0;
			}
			else
			{
				t.playerlight.spotfadeout_f=0;
				t.playerlight.spotflash=0;
			}
		}
	}
	if (  t.playerlight.mode == 1 ) 
	{
		//  fireball dynamic light - updated by _lighting_spotflashtracking
		t.playerlight.spotflash=t.playerlight.spotflash-(2.0*g.timeelapsed_f);
		if (  t.playerlight.spotflash<0 ) 
		{
			t.playerlight.spotfadeout_f=0;
			t.playerlight.spotflash=0;
			t.playerlight.mode=0;
		}
		t.playerlight.spotfadeout_f=t.playerlight.spotflash/400.0;
	}
	if (  t.playerlight.mode == 2 ) 
	{
		//  explosion dynamic light - updated by _lighting_spotflashexplosion
		t.playerlight.spotflash=t.playerlight.spotflash-(20.0*g.timeelapsed_f);
		if (  t.playerlight.spotflash<0 ) 
		{
			t.playerlight.spotfadeout_f=0;
			t.playerlight.spotflash=0;
			t.playerlight.mode=0;
		}
		t.playerlight.spotfadeout_f=t.playerlight.spotflash/200.0;
	}
}
