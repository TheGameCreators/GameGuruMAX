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

#ifdef WICKEDENGINE

// whole new lighting system for wicked

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
		#ifdef WICKEDENGINE
		if (uiFlashLight)
		{
			// flashlight off
			iUpdateFlashLightMode = 2;
		}
		#endif // WICKEDENGINE
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

	#ifdef WICKEDENGINE
	{
		lighting_weaponFlash_loop(); //Handles weapon gun flash
	}
	#endif // WICKEDENGINE

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

#else

//PE: Only actually used lights is sent to shaders, so it dont matter if this is quite high.
//#define MAXDYNLIGHTS 80 // PE: Make sure to match this with the shaders.
int nearestlightId[MAXDYNLIGHTS];
bool bLightDataChanged = false;
void dynamic_sun_position(int effectid);

void lighting_init ( void )
{
	//  Create all lights
	lighting_refresh ( );
}

void start_mesh_light(void);
void setlayer_mesh_light(int layer);
void end_mesh_light(void);
bool update_mesh_light(sMesh* pMesh, sObject* pObject, sFrame* pFrame);
void get_nearby_light(float f_meshX, float f_meshY, float f_meshZ, float f_objSize , sObject* pObject);

sObject* debugobject=NULL;
int debugcount = 0;


//PE: Create a render list.
//#define MAXRENDERLIST 8000 //PE: Generate a mesh render list with info.
#ifdef MAXRENDERLIST
int rl_entries = 0;
DWORD rl_object[MAXRENDERLIST];
int rl_object_rendered[MAXRENDERLIST];
sMesh* rl_mesh[MAXRENDERLIST];
int rl_mesh_rendered[MAXRENDERLIST];
int rl_layer[MAXRENDERLIST];
float rl_posx[MAXRENDERLIST];
float rl_posy[MAXRENDERLIST];
float rl_posz[MAXRENDERLIST];
int rl_lightsused[MAXRENDERLIST];
float rl_objsize[MAXRENDERLIST];
char rl_colors[MAXRENDERLIST][1024];
#endif

void start_mesh_light()
{
	#ifdef MAXRENDERLIST
	rl_entries = 0;
	for (int i = 0; i < MAXRENDERLIST; i++) 
	{
		rl_mesh_rendered[i] = 0;
		rl_object_rendered[i] = 0;
		rl_layer[i] = 0;
		strcpy(rl_colors[i], "");
	}
	#endif
}

int rl_current_layer = 0;
void setlayer_mesh_light(int clayer) 
{
	rl_current_layer = clayer;
}

DWORD startrecording = 0;
void end_mesh_light()
{
	#ifdef MAXRENDERLIST
	//PE:
	if (startrecording++ > 500 ) {
		startrecording = 0;
		char debugmsg[1024];
		OutputDebugString("--RenderList START--\n");
		for (int i = 0; i < rl_entries; i++) {
			sprintf(debugmsg, "%ld (%d) - %ld (%d) - (%d) - (%f,%f,%f) - L(%d) S(%f)\n", rl_object[i], rl_object_rendered[i], rl_mesh[i], rl_mesh_rendered[i], rl_layer[i], rl_posx[i], rl_posy[i], rl_posz[i], rl_lightsused[i], rl_objsize[i]);
			OutputDebugString(debugmsg);
		}
		OutputDebugString("--RenderList END--\n");
	}
	#endif
	//Only reset when end of sync.
	bLightDataChanged = false;
}

bool update_mesh_light(sMesh* pMesh, sObject* pObject, sFrame* pFrame)
{
	// Ignore shadow camera
	if (g_pGlob->dwRenderCameraID >= 31) return false;
	
	// New light per mesh
	if (pMesh->dl_lights == NULL) 
	{
		if ( pMesh->pVertexShaderEffect )
		{
			if ( pMesh->pVertexShaderEffect->m_pEffect )
			{
				pMesh->dl_lights = pMesh->pVertexShaderEffect->m_pEffect->GetVariableByName("dl_lights");
				if (pMesh->dl_lights) 
				{
					if (pMesh->dl_lightsVS == NULL) 
					{
						pMesh->dl_lightsVS = pMesh->pVertexShaderEffect->m_pEffect->GetVariableByName("dl_lightsVS");
					}
					if (pMesh->dl_pos[0] == NULL) 
					{
						pMesh->dl_pos[0] = pMesh->pVertexShaderEffect->m_pEffect->GetVariableByName("dl_pos");
					}
					if (pMesh->dl_diffuse[0] == NULL) 
					{
						pMesh->dl_diffuse[0] = pMesh->pVertexShaderEffect->m_pEffect->GetVariableByName("dl_diffuse");
					}
					if (pMesh->dl_angle[0] == NULL) 
					{
						pMesh->dl_angle[0] = pMesh->pVertexShaderEffect->m_pEffect->GetVariableByName("dl_angle");
					}
				}
				else 
					return false;
			}
			else 
				return false;
		}
		else 
			return false;
	}

	// Make sure when last light is deleted , that we dont display any
	if (g.infinilightmax == 0 && t.playerlight.spotflash <= 0 ) 
	{
		if (pMesh->dl_lights) 
		{
			float f_tmp = 0;
			pMesh->dl_lights->AsScalar()->SetFloat(f_tmp);
		}
		if (pMesh->dl_lightsVS) 
		{
			float f_tmp = 0;
			pMesh->dl_lightsVS->AsScalar()->SetFloat(f_tmp);
		}
		return true;
	}

	float fposData[ 4 * MAXDYNLIGHTS ];
	float fdiffuseData[4 * MAXDYNLIGHTS];
	float fangleData[4 * MAXDYNLIGHTS];
	int frealLightsUsed = 0;
	int frealVSLightsUsed = 0;
	bool b_angle_used = false;

	if(pObject) 
	{
		//PE: find closest to object.
		//float f_oPosX = pObject->position.vecPosition.x; // 300718 - excludes weapons and locked screen objects (HUD objects)
		//float f_oPosY = pObject->position.vecPosition.y;
		//float f_oPosZ = pObject->position.vecPosition.z;
		float f_oPosX = pObject->position.matWorld._41;
		float f_oPosY = pObject->position.matWorld._42;
		float f_oPosZ = pObject->position.matWorld._43;
		float fXSize = fabs(pObject->collision.vecMax.x - pObject->collision.vecMin.x) * pObject->position.vecScale.x;
		float fYSize = fabs(pObject->collision.vecMax.y - pObject->collision.vecMin.y) * pObject->position.vecScale.y;
		float fZSize = fabs(pObject->collision.vecMax.z - pObject->collision.vecMin.z) * pObject->position.vecScale.z;

		float f_objsize = fXSize;
		if (f_objsize < fZSize) f_objsize = fZSize;
		
		//PE: Need to cache nearestlightId[i] , get_nearby_light is just to slow with many lights.
		//PE: Positon , color .. are still updated so only if .islit is changed we need to refresh.
		//PE: Sure when we move a light into a new mesh it should be updated., but we already use a objsize*1.25
		//PE: so updating evry 20 sync should be fine.

		if ((pObject->iDelayedUpdate++) % 20 == 0 || !pObject->bLightDateUpdated || bLightDataChanged)
		{
			get_nearby_light(f_oPosX, f_oPosY, f_oPosZ, f_objsize, pObject);
		}

		if (t.playerlight.spotflash > 0 && ((frealLightsUsed + frealVSLightsUsed) < g.maxtotalmeshlights))
		{
			fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.playerlight.spotflashx_f;
			fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.playerlight.spotflashy_f;
			fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.playerlight.spotflashz_f;
			fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 30.0; // t.playerlight.spotfadeout_f;

			fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.playerlight.spotlightr_f * (t.playerlight.spotfadeout_f * 0.12);
			fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.playerlight.spotlightg_f * (t.playerlight.spotfadeout_f * 0.12);
			fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.playerlight.spotlightb_f * (t.playerlight.spotfadeout_f * 0.12);
			fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 0; //PE: type point;

			fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = 0.0;
			fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = 0.0;
			fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = 0.0;
			fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 0.0;

			frealLightsUsed++;
		}

		for (int i = 0; i < (g.maxtotalmeshlights); i++)
		{
			int l_index = pObject->CachedNearestLightId[i];
			if (l_index>0 && l_index < t.infinilight.size() )
			{
				t.tlx_f = t.infinilight[l_index].x;
				t.tly_f = t.infinilight[l_index].y;
				t.tlz_f = t.infinilight[l_index].z;

				t.tcolr_f = (t.infinilight[l_index].colrgb.r + 0.0) / 255.0;
				t.tcolg_f = (t.infinilight[l_index].colrgb.g + 0.0) / 255.0;
				t.tcolb_f = (t.infinilight[l_index].colrgb.b + 0.0) / 255.0;

				fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.tlx_f;
				fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.tly_f;
				fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.tlz_f;
				fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = t.infinilight[l_index].range; //PE: Put range in pos.w

				if (t.infinilight[l_index].type == 1) 
				{
					//PE: static light has less intensity then realtime , so lower it to match lightmapper.
					fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = t.infinilight[l_index].range*0.70; //PE: Put range in pos.w
					fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.tcolr_f*0.70;
					fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.tcolg_f*0.70;
					fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.tcolb_f*0.70;
					fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = t.infinilight[l_index].type; //PE: Put type in diffuse.w;
				}
				else 
				{
					fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.tcolr_f;
					fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.tcolg_f;
					fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.tcolb_f;
					fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = t.infinilight[l_index].type; //PE: Put type in diffuse.w;
				}

				fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.infinilight[l_index].f_angle_x;
				fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.infinilight[l_index].f_angle_y;
				fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.infinilight[l_index].f_angle_z;
				fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 0.0;

				if (t.infinilight[l_index].is_spot_light) 
				{
					fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 3; //PE: spot = type 3. also set angle.
					b_angle_used = true;
				}

				#ifdef MAXRENDERLIST
				//char ctmp[256];
				//sprintf(ctmp, " RGB%d(%f,%f,%f)", l_index, t.tcolr_f, t.tcolg_f, t.tcolb_f);
				//strcat(rl_colors[rl_entries], ctmp);
				#endif

				if (frealLightsUsed >= g.maxpixelmeshlights ) 
				{
					//PE: Add the rest as fast Vertex lights.
					frealVSLightsUsed++;
				}
				else 
				{
					frealLightsUsed++;
				}
			}
		}

		#ifdef MAXRENDERLIST
		if (pObject) {
			rl_object[rl_entries] = pObject->dwObjectNumber;
			rl_mesh[rl_entries] = pMesh;

			int iObjCount = 0;
			int iMeshCount = 0;
			for (int i = 0; i <= rl_entries; i++) {
				if (rl_object[i] == pObject->dwObjectNumber) iObjCount++;
				if (rl_mesh[i] == pMesh) iMeshCount++;
			}
			rl_object_rendered[rl_entries] = iObjCount;
			rl_mesh_rendered[rl_entries] = iMeshCount;
			rl_layer[rl_entries] = rl_current_layer;

			rl_posx[rl_entries] = f_oPosX;
			rl_posy[rl_entries] = f_oPosY;
			rl_posz[rl_entries] = f_oPosZ;
			rl_lightsused[rl_entries] = frealLightsUsed;
			rl_objsize[rl_entries] = f_objsize;
			rl_entries++;
		}
		#endif
	}
	else 
	{
		if (t.playerlight.spotflash > 0 && ((frealLightsUsed + frealVSLightsUsed) < g.maxtotalmeshlights))
		{
			fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.playerlight.spotflashx_f;
			fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.playerlight.spotflashy_f;
			fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.playerlight.spotflashz_f;
			fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 30.0;

			fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.playerlight.spotlightr_f * (t.playerlight.spotfadeout_f * 0.12);
			fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.playerlight.spotlightg_f * (t.playerlight.spotfadeout_f * 0.12);
			fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.playerlight.spotlightb_f * (t.playerlight.spotfadeout_f * 0.12);
			fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 0; //PE: type point;

			fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = 0.0;
			fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = 0.0;
			fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = 0.0;
			fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 0.0;

			//PE: Always use per pixel on terrain  when moving lights.
			frealLightsUsed++;
		}

		//PE: Terrain use the old fading light system max 3 dyn lights.

		if (g.terrainoldlight == 1) 
		{
			//PE: Terrain use old way for now.
			for (t.best = 0; t.best <= 3; t.best++)
			{
				int l_index = t.nearestlightindex[t.best];
				if (l_index > 0)
				{
					t.tlx_f = t.infinilight[l_index].x;
					t.tly_f = t.infinilight[l_index].y;
					t.tlz_f = t.infinilight[l_index].z;

					t.tcolr_f = (t.infinilight[l_index].colrgb.r + 0.0)*t.infinilight[l_index].intensity_f; t.tcolr_f = t.tcolr_f / 255.0;
					t.tcolg_f = (t.infinilight[l_index].colrgb.g + 0.0)*t.infinilight[l_index].intensity_f; t.tcolg_f = t.tcolg_f / 255.0;
					t.tcolb_f = (t.infinilight[l_index].colrgb.b + 0.0)*t.infinilight[l_index].intensity_f; t.tcolb_f = t.tcolb_f / 255.0;

					fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.tlx_f;
					fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.tly_f;
					fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.tlz_f;
					fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = t.infinilight[l_index].range;

					if (t.infinilight[l_index].type == 1) 
					{
						//PE: static light has less intensity then realtime , so lower it to match lightmapper.
						fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = t.infinilight[l_index].range*0.70;
						fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.tcolr_f*0.70;
						fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.tcolg_f*0.70;
						fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.tcolb_f*0.70;
						fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 0;
					}
					else 
					{
						fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.tcolr_f;
						fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.tcolg_f;
						fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.tcolb_f;
						fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 0;
					}

					fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.infinilight[l_index].f_angle_x;
					fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.infinilight[l_index].f_angle_y;
					fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.infinilight[l_index].f_angle_z;
					fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 0.0;

					if (t.infinilight[l_index].is_spot_light) 
					{
						fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 3; //PE: spot = type 3. also set angle.
						b_angle_used = true;
					}

					frealLightsUsed++;
				}
			}
		}
		else 
		{
			//PE: Terrain use lights nearby camera.
			//PE: for now add terrain as VS light.
			if (g.maxterrainlights > g.infinilightmax) //PE: Only use fade if needed.
				get_nearby_light(CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0), 70000 , NULL);
			else
				get_nearby_light(CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0), g.terrainlightfadedistance , NULL);

			for (int i = 0; i < g.maxterrainlights; i++)
			{
				int l_index = nearestlightId[i];
				if (l_index > 0)
				{
					t.tlx_f = t.infinilight[l_index].x;
					t.tly_f = t.infinilight[l_index].y;
					t.tlz_f = t.infinilight[l_index].z;

					float fade = 1.0 / ( (g.terrainlightfadedistance-500.0) / t.infinilight[l_index].distfromobj_f);
					if (fade > 1.0) fade = 1.0;
					if (fade < 0.0) fade = 0.0;
					fade = 1.0 - fade;

					if (g.maxterrainlights > g.infinilightmax) //PE: Only use fade if needed.
						fade = 1.0;

					//PE: Ignore lights that is nearly out.
					if (fade > 0.01) 
					{
						t.tcolr_f = ((t.infinilight[l_index].colrgb.r + 0.0)*fade) / 255.0;
						t.tcolg_f = ((t.infinilight[l_index].colrgb.g + 0.0)*fade) / 255.0;
						t.tcolb_f = ((t.infinilight[l_index].colrgb.b + 0.0)*fade) / 255.0;

						fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.tlx_f;
						fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.tly_f;
						fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.tlz_f;
						fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = t.infinilight[l_index].range;

						if (t.infinilight[l_index].type == 1) 
						{
							//PE: static light has less intensity then realtime , so lower it to match lightmapper.
							fposData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = t.infinilight[l_index].range*0.70;
							fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.tcolr_f*0.70;
							fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.tcolg_f*0.70;
							fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.tcolb_f*0.70;
							fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 0;
						}
						else 
						{
							fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.tcolr_f;
							fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.tcolg_f;
							fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.tcolb_f;
							fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 0;
						}

						fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 0] = t.infinilight[l_index].f_angle_x;
						fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 1] = t.infinilight[l_index].f_angle_y;
						fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 2] = t.infinilight[l_index].f_angle_z;
						fangleData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 0.0;

						if (t.infinilight[l_index].is_spot_light) 
						{
							fdiffuseData[((frealLightsUsed + frealVSLightsUsed) * 4) + 3] = 3; //PE: spot = type 3. also set angle.
							b_angle_used = true;
						}

						if( g.terrainusevertexlights == 1 || frealLightsUsed >= g.maxpixelmeshlights ) 
						{
							//PE: Add the rest as fast Vertex lights.
							frealVSLightsUsed++;
						}
						else 
						{
							frealLightsUsed++;
						}
					}
				}
			}
		}
	}

	// Optimizing: 114fps: FPS without sending light data: 114fps , takes 0 fps
	if (pMesh->dl_lights) 
	{
		float f_tmp = frealLightsUsed;
		pMesh->dl_lights->AsScalar()->SetFloat(f_tmp);
	}
	if (pMesh->dl_lightsVS) 
	{
		float f_tmp = frealVSLightsUsed;
		pMesh->dl_lightsVS->AsScalar()->SetFloat(f_tmp);
	}
	if ((frealLightsUsed > 0 || frealVSLightsUsed > 0) && pMesh->dl_pos[0]) 
	{
		pMesh->dl_pos[0]->AsVector()->SetFloatVectorArray(fposData, 0, frealLightsUsed+frealVSLightsUsed);
	}
	if ((frealLightsUsed > 0 || frealVSLightsUsed > 0) && pMesh->dl_diffuse[0]) 
	{
		pMesh->dl_diffuse[0]->AsVector()->SetFloatVectorArray(fdiffuseData, 0, frealLightsUsed+frealVSLightsUsed);
	}
	if ( b_angle_used && pMesh->dl_angle[0]) 
	{
		pMesh->dl_angle[0]->AsVector()->SetFloatVectorArray(fangleData, 0, frealLightsUsed + frealVSLightsUsed);
	}

	return true;
}

void get_nearby_light(float f_meshX , float f_meshY , float f_meshZ, float f_objSize, sObject* pObject)
{
	//PE: Get the nearest lights.

	if (pObject) {
		if (!pObject->bLightDateUpdated) {
			//Clear cache.
			for (int i = 0; i < g.maxtotalmeshlights; i++)
				if (pObject) pObject->CachedNearestLightId[i] = 0;
		}
	}

	if (g.infinilightmax>0 && g.infinilightmax <= ArrayCount(t.infinilight))
	{
		bool bUseAllLights = false;
		if (g.showstaticlightinrealtime == 1 && t.visuals.shaderlevels.lighting != 1) //PE: Also display static lights if not in prebake.
			bUseAllLights = true;

		t.infinilight[0].distfromobj_f = 9999999;
		t.infinilight[0].drop_entry = false;
		int i_ctype = 0;
		for (int loop = 1; loop <= g.infinilightmax; loop++)
		{
			t.infinilight[loop].drop_entry = false;

			if (t.infinilight[loop].used == 1 && (bUseAllLights || t.infinilight[loop].type == 0))
			{
				//get distance from object to light.
				t.tdx_f = t.infinilight[loop].x - f_meshX;
				t.tdz_f = t.infinilight[loop].z - f_meshZ;
				t.tdd_f = Sqrt( (t.tdx_f*t.tdx_f) + (t.tdz_f*t.tdz_f) );
				t.infinilight[loop].distfromobj_f = fabs(t.tdd_f);

				//PE: We should have some light culling here.
				bool culllight = false;

				if (t.infinilight[loop].distfromobj_f > (f_objSize + (t.infinilight[loop].range *1.25))) culllight = true;
				if( !(t.infinilight[loop].used == 1 && t.infinilight[loop].islit == 1 && (bUseAllLights || t.infinilight[loop].type == 0)) ) culllight = true;

				if(culllight) t.infinilight[loop].drop_entry = true; //out of range cull here.
			}
		}

		for (int i = 0; i < g.maxtotalmeshlights; i++) 
		{
			nearestlightId[i] = 0;
			if(pObject) pObject->CachedNearestLightId[i] = 0;

			float fsearch = 9999999;

			for (int loop = 1; loop <= g.infinilightmax; loop++) //g.infinilightmax*0.5 fps from 6 to 16.
			{
				if (t.infinilight[loop].drop_entry == false) 
				{
					// PE: Add * 1.25 to allow fading out.
					if (t.infinilight[loop].distfromobj_f < fsearch) 
					{
						nearestlightId[i] = loop;
							if (pObject) pObject->CachedNearestLightId[i] = loop;
						fsearch = t.infinilight[loop].distfromobj_f;
					}
				}
			}
			t.infinilight[nearestlightId[i]].drop_entry = true;
		}
	}
	if (pObject) {
		pObject->bLightDateUpdated = true;
	}
}

void lighting_blanklights ( void )
{
	//  wipe out lights fed into the shader
	if ( t.terrain.terrainshaderindex>0 ) 
	{
		if ( GetEffectExist(t.terrain.terrainshaderindex) == 1 ) 
		{
			SetVector4 (  g.terrainvectorindex,0,0,0,0 );
			t.bestpos_s = "g_lights_data";
			SetEffectConstantV ( t.terrain.terrainshaderindex, t.bestpos_s.Get(), g.terrainvectorindex );
			for ( t.ttt = 1 ; t.ttt<=  3; t.ttt++ )
			{
				if (  t.ttt == 1 ) { t.bestpos_s = "g_lights_pos0"  ; t.bestatt_s = "g_lights_atten0" ; t.bestdif_s = "g_lights_diffuse0"; }
				if (  t.ttt == 2 ) { t.bestpos_s = "g_lights_pos1" ; t.bestatt_s = "g_lights_atten1" ; t.bestdif_s = "g_lights_diffuse1"; }
				if (  t.ttt == 3 ) { t.bestpos_s = "g_lights_pos2"  ; t.bestatt_s = "g_lights_atten2" ; t.bestdif_s = "g_lights_diffuse2"; }
				SetEffectConstantV (  t.terrain.terrainshaderindex,t.bestpos_s.Get(),g.terrainvectorindex );
				SetEffectConstantV (  t.terrain.terrainshaderindex,t.bestatt_s.Get(),g.terrainvectorindex );
				SetEffectConstantV (  t.terrain.terrainshaderindex,t.bestdif_s.Get(),g.terrainvectorindex );
			}
		}
	}
}

void lighting_free ( void )
{
	//  clear infinilights array
	if (  ArrayCount(t.infinilight) >= 0 ) 
	{
		UnDim ( t.infinilight );
	}
	g.infinilightmax=0;

	//  clear old infini light history
	t.nearestlightindex[0]=0;
	t.nearestlightindex[1]=0;
	t.nearestlightindex[2]=0;
	t.nearestlightindex[3]=0;
	t.nearestlightindex[4]=0;

	//  and remove from shader influence
	lighting_blanklights ( );

	//  switch flash light off
	t.playerlight.flashlightcontrol_f=0;
}

void lighting_override ( void )
{
	//  called from grideditor (entity cursor update)
	t.lighting.override=0;
	if ( t.gridentity > 0 ) 
	{
		if (  t.entityprofile[t.gridentity].ismarker == 2 ) 
		{
			t.lighting.override=1;
		}
	}
	else
	{
		if ( g.entityrubberbandlist.size()==0 )
		{
			if ( t.widget.pickedObject != 0 && t.widget.pickedEntityIndex>0 ) 
			{
				t.tentid=t.entityelement[t.widget.pickedEntityIndex].bankindex;
				if (  t.entityprofile[t.tentid].ismarker == 2 ) 
				{
					t.lighting.override=2;
				}
			}
		}
	}
}

void lighting_loop ( void )
{
	//  Handle any spot flashes
	lighting_spotflash_control ( );

	//  Calculate four nearest lights (three are visible)
	if (  g.infinilightmax>0 && g.infinilightmax <= ArrayCount(t.infinilight) ) 
	{
		t.infinilight[0].distfromcam_f=9999999;
		t.ttcx_f=CameraPositionX(0);
		t.ttcz_f=CameraPositionZ(0);
		for ( t.l = 1 ; t.l<=  g.infinilightmax; t.l++ )
		{
			if (  t.infinilight[t.l].used == 1 && t.infinilight[t.l].type == 0 && t.infinilight[t.l].islit == 1 ) 
			{
				//  work out distance to this light
				t.tdx_f=t.infinilight[t.l].x-t.ttcx_f;
				t.tdz_f=t.infinilight[t.l].z-t.ttcz_f;
				t.tdd_f=Sqrt(abs(t.tdx_f*t.tdx_f)+abs(t.tdz_f*t.tdz_f));
				//  lights in front of player are given more importance (reduced in distance proportionally)
				t.tda_f=atan2deg(t.tdx_f,t.tdz_f)-CameraAngleY(0);
				if (  t.tda_f<-180  )  t.tda_f = t.tda_f+360.0;
				if (  t.tda_f >= 180  )  t.tda_f = t.tda_f-360.0;
				t.tfrontfacingstr_f=abs(t.tda_f)/45.0;
				if (  t.tfrontfacingstr_f<1.0  )  t.tfrontfacingstr_f = 1.0;
				if (  t.tfrontfacingstr_f>2.0  )  t.tfrontfacingstr_f = 2.0;
				t.infinilight[t.l].distfromcam_f=t.tdd_f*t.tfrontfacingstr_f;
				//  assign intensity based on distance (from last cycle)
				t.ld2_f=t.infinilight[t.nearestlightindex[2]].distfromcam_f;
				t.ld3_f=t.infinilight[t.nearestlightindex[3]].distfromcam_f;
				t.ld4_f=t.infinilight[t.nearestlightindex[4]].distfromcam_f;
				t.tperc_f=(1.0/(t.ld4_f-t.ld2_f))*(t.ld3_f-t.ld2_f);
				t.tintensity_f=1.0;
				if (  t.nearestlightindex[1] == t.l  )  t.tintensity_f = 0.0;
				if (  t.nearestlightindex[2] == t.l  )  t.tintensity_f = 0.0;
				if (  t.nearestlightindex[3] == t.l  )  t.tintensity_f = t.tperc_f;
				if (  t.tintensity_f<0.0  )  t.tintensity_f = 0.0;
				if (  t.tintensity_f>1.0  )  t.tintensity_f = 1.0;
				t.infinilight[t.l].intensity_f=1.0-t.tintensity_f;
			}
		}
		t.nearestlightindex[1]=0;
		t.nearestlightindex[2]=0;
		t.nearestlightindex[3]=0;
		t.nearestlightindex[4]=0;
		for ( t.l = 1 ; t.l<=  g.infinilightmax; t.l++ )
		{
			if (  t.infinilight[t.l].used == 1 && t.infinilight[t.l].type == 0 && t.infinilight[t.l].islit == 1 ) 
			{
				if (  t.infinilight[t.l].distfromcam_f<t.infinilight[t.nearestlightindex[1]].distfromcam_f  )  t.nearestlightindex[1] = t.l;
			}
		}
		for ( t.l = 1 ; t.l<=  g.infinilightmax; t.l++ )
		{
			if (  t.infinilight[t.l].used == 1 && t.infinilight[t.l].type == 0 && t.infinilight[t.l].islit == 1 ) 
			{
				if (  t.l != t.nearestlightindex[1] ) 
				{
					if (  t.infinilight[t.l].distfromcam_f<t.infinilight[t.nearestlightindex[2]].distfromcam_f  )  t.nearestlightindex[2] = t.l;
				}
			}
		}
		for ( t.l = 1 ; t.l<=  g.infinilightmax; t.l++ )
		{
			if (  t.infinilight[t.l].used == 1 && t.infinilight[t.l].type == 0 && t.infinilight[t.l].islit == 1 ) 
			{
				if (  t.l != t.nearestlightindex[1] && t.l != t.nearestlightindex[2] ) 
				{
					if (  t.infinilight[t.l].distfromcam_f<t.infinilight[t.nearestlightindex[3]].distfromcam_f  )  t.nearestlightindex[3] = t.l;
				}
			}
		}
		for ( t.l = 1 ; t.l<=  g.infinilightmax; t.l++ )
		{
			if (  t.infinilight[t.l].used == 1 && t.infinilight[t.l].type == 0 && t.infinilight[t.l].islit == 1 ) 
			{
				if (  t.l != t.nearestlightindex[1] && t.l != t.nearestlightindex[2] && t.l != t.nearestlightindex[3] ) 
				{
					if (  t.infinilight[t.l].distfromcam_f<t.infinilight[t.nearestlightindex[4]].distfromcam_f  )  t.nearestlightindex[4] = t.l;
				}
			}
		}
	}

	//  update dynamic lights whether have them or not
	if (  1 ) 
	{
		//  For all shaders, affect using dynamic light values arrived at
		if (  t.terrain.terrainshaderindex>0 ) 
		{
			// Count lights in this sequence (for g_lights_data)
			int iCountDynamicBestLights = 0;
			for ( t.best = 1 ; t.best<= 3; t.best++ )
			{
				t.l=t.nearestlightindex[t.best];
				if (  t.l>0 ) iCountDynamicBestLights++;
			}
			
			// Apply intensity to light constants for ALL shaders
			for ( t.best = 0 ; t.best <= 3; t.best++ )
			{
				if ( t.best == 0 )
				{
					SetVector4 ( g.terrainvectorindex1,iCountDynamicBestLights,0,0,0 );
				}
				else
				{
					t.l=t.nearestlightindex[t.best];
					if (  t.l>0 ) 
					{
						t.tlx_f=t.infinilight[t.l].x;
						t.tly_f=t.infinilight[t.l].y;
						t.tlz_f=t.infinilight[t.l].z;
						t.trange_f=t.infinilight[t.l].range;
						t.tatten0_f=1.0;
						t.tatten1_f=0.0;
						t.tatten2_f=t.trange_f/2.0 ; t.tatten2_f=1.0/(t.tatten2_f*t.tatten2_f);
						t.tcolr_f=(t.infinilight[t.l].colrgb.r+0.0)*t.infinilight[t.l].intensity_f ; t.tcolr_f=t.tcolr_f/255.0;
						t.tcolg_f=(t.infinilight[t.l].colrgb.g+0.0)*t.infinilight[t.l].intensity_f ; t.tcolg_f=t.tcolg_f/255.0;
						t.tcolb_f=(t.infinilight[t.l].colrgb.b+0.0)*t.infinilight[t.l].intensity_f ; t.tcolb_f=t.tcolb_f/255.0;
						SetVector4 (  g.terrainvectorindex1,t.tlx_f,t.tly_f,t.tlz_f, t.trange_f ); // 300116 - added range to skip atten calc (corrupted by latest NVIDIA driver 361)
						SetVector4 (  g.terrainvectorindex2,t.tatten0_f,t.tatten1_f,t.tatten2_f,0 );
						SetVector4 (  g.terrainvectorindex3,t.tcolr_f,t.tcolg_f,t.tcolb_f,0 );
					}
					else
					{
						SetVector4 (  g.terrainvectorindex1,0,0,0,0 );
						SetVector4 (  g.terrainvectorindex2,0,0,0,0 );
						SetVector4 (  g.terrainvectorindex3,0,0,0,0 );
					}
				}

				if (  GetEffectExist(t.terrain.terrainshaderindex) == 1 ) 
				{
					dynamic_sun_position(t.terrain.terrainshaderindex);
					if ( t.best == 0 )
					{
						t.bestpos = t.effectparam.terrain.g_lights_data;
						SetEffectConstantVEx(  t.terrain.terrainshaderindex,t.bestpos,g.terrainvectorindex1 );
					}
					else
					{
						if (  t.best == 1 ) { t.bestpos = t.effectparam.terrain.g_lights_pos0  ; t.bestatt = t.effectparam.terrain.g_lights_atten0 ; t.bestdif = t.effectparam.terrain.g_lights_diffuse0; }
						if (  t.best == 2 ) { t.bestpos = t.effectparam.terrain.g_lights_pos1  ; t.bestatt = t.effectparam.terrain.g_lights_atten1 ; t.bestdif = t.effectparam.terrain.g_lights_diffuse1; }
						if (  t.best == 3 ) { t.bestpos = t.effectparam.terrain.g_lights_pos2  ; t.bestatt = t.effectparam.terrain.g_lights_atten2 ; t.bestdif = t.effectparam.terrain.g_lights_diffuse2; }
						SetEffectConstantVEx(  t.terrain.terrainshaderindex,t.bestpos,g.terrainvectorindex1 );
						SetEffectConstantVEx (  t.terrain.terrainshaderindex,t.bestatt,g.terrainvectorindex2 );
						SetEffectConstantVEx (  t.terrain.terrainshaderindex,t.bestdif,g.terrainvectorindex3 );
					}
				}
				if ( t.terrain.vegetationshaderindex>0 ) 
				{
					if ( GetEffectExist(t.terrain.vegetationshaderindex) == 1 ) 
					{
						dynamic_sun_position(t.terrain.vegetationshaderindex);

						if ( t.best == 0 )
						{
							t.bestpos = t.effectparam.vegetation.g_lights_data;
							SetEffectConstantVEx (  t.terrain.vegetationshaderindex,t.bestpos,g.terrainvectorindex1 );
						}
						else
						{
							if (  t.best == 1 ) { t.bestpos = t.effectparam.vegetation.g_lights_pos0  ; t.bestatt = t.effectparam.vegetation.g_lights_atten0 ; t.bestdif = t.effectparam.vegetation.g_lights_diffuse0; }
							if (  t.best == 2 ) { t.bestpos = t.effectparam.vegetation.g_lights_pos1  ; t.bestatt = t.effectparam.vegetation.g_lights_atten1 ; t.bestdif = t.effectparam.vegetation.g_lights_diffuse1; }
							if (  t.best == 3 ) { t.bestpos = t.effectparam.vegetation.g_lights_pos2  ; t.bestatt = t.effectparam.vegetation.g_lights_atten2 ; t.bestdif = t.effectparam.vegetation.g_lights_diffuse2; }
							SetEffectConstantVEx (  t.terrain.vegetationshaderindex,t.bestpos,g.terrainvectorindex1 );
							SetEffectConstantVEx (  t.terrain.vegetationshaderindex,t.bestatt,g.terrainvectorindex2 );
							SetEffectConstantVEx (  t.terrain.vegetationshaderindex,t.bestdif,g.terrainvectorindex3 );
						}
					}
				}
				for ( t.tgunid = 1 ; t.tgunid<=  g.gunmax; t.tgunid++ )
				{
					if (  t.gun[t.tgunid].effectidused>0 ) 
					{
						t.tteffectid=t.gun[t.tgunid].effectidused;
						if (  GetEffectExist(t.tteffectid) == 1 ) 
						{
							dynamic_sun_position(t.tteffectid);

							if ( t.best == 0 )
							{
								t.bestpos = t.effectparamarray[t.tteffectid].g_lights_data;
								SetEffectConstantVEx (  t.tteffectid,t.bestpos,g.terrainvectorindex1 );
							}
							else
							{
								if (  t.best == 1 ) { t.bestpos = t.effectparamarray[t.tteffectid].g_lights_pos0  ; t.bestatt = t.effectparamarray[t.tteffectid].g_lights_atten0 ; t.bestdif = t.effectparamarray[t.tteffectid].g_lights_diffuse0; }
								if (  t.best == 2 ) { t.bestpos = t.effectparamarray[t.tteffectid].g_lights_pos1  ; t.bestatt = t.effectparamarray[t.tteffectid].g_lights_atten1 ; t.bestdif = t.effectparamarray[t.tteffectid].g_lights_diffuse1; }
								if (  t.best == 3 ) { t.bestpos = t.effectparamarray[t.tteffectid].g_lights_pos2  ; t.bestatt = t.effectparamarray[t.tteffectid].g_lights_atten2 ; t.bestdif = t.effectparamarray[t.tteffectid].g_lights_diffuse2; }
								SetEffectConstantVEx (  t.tteffectid,t.bestpos,g.terrainvectorindex1 );
								SetEffectConstantVEx (  t.tteffectid,t.bestatt,g.terrainvectorindex2 );
								SetEffectConstantVEx (  t.tteffectid,t.bestdif,g.terrainvectorindex3 );
							}
						}
					}
				}
				for ( t.t = -6 ; t.t<=  g.effectbankmax; t.t++ )
				{
					if (  t.t == -6  )  t.tteffectid = g.lightmappbreffectillum;
					#ifdef VRTECH
					if (  t.t == -5  )  t.tteffectid = g.controllerpbreffect;
					#else
					if (  t.t == -5  )  continue;
					#endif
					if (  t.t == -4  )  t.tteffectid = g.lightmappbreffect;
					if (  t.t == -3  )  t.tteffectid = g.thirdpersonentityeffect;
					if (  t.t == -2  )  t.tteffectid = g.thirdpersoncharactereffect;
					if (  t.t == -1  )  t.tteffectid = g.staticlightmapeffectoffset;
					if (  t.t == 0  )  t.tteffectid = g.staticshadowlightmapeffectoffset;
					if (  t.t>0  )  t.tteffectid = g.effectbankoffset+t.t;
					if (  GetEffectExist(t.tteffectid) == 1 ) 
					{
						dynamic_sun_position(t.tteffectid);

						if ( t.best == 0 )
						{
							t.bestpos = t.effectparamarray[t.tteffectid].g_lights_data;
							SetEffectConstantVEx (  t.tteffectid,t.bestpos,g.terrainvectorindex1 );
						}
						else
						{
							if (  t.best == 1 ) { t.bestpos = t.effectparamarray[t.tteffectid].g_lights_pos0  ; t.bestatt = t.effectparamarray[t.tteffectid].g_lights_atten0 ; t.bestdif = t.effectparamarray[t.tteffectid].g_lights_diffuse0; }
							if (  t.best == 2 ) { t.bestpos = t.effectparamarray[t.tteffectid].g_lights_pos1  ; t.bestatt = t.effectparamarray[t.tteffectid].g_lights_atten1 ; t.bestdif = t.effectparamarray[t.tteffectid].g_lights_diffuse1; }
							if (  t.best == 3 ) { t.bestpos = t.effectparamarray[t.tteffectid].g_lights_pos2  ; t.bestatt = t.effectparamarray[t.tteffectid].g_lights_atten2 ; t.bestdif = t.effectparamarray[t.tteffectid].g_lights_diffuse2; }
							SetEffectConstantVEx (  t.tteffectid,t.bestpos,g.terrainvectorindex1 );
							SetEffectConstantVEx (  t.tteffectid,t.bestatt,g.terrainvectorindex2 );
							SetEffectConstantVEx (  t.tteffectid,t.bestdif,g.terrainvectorindex3 );
						}
					}
				}
			}
		}
	}

	//  can override light zero using override (usually entity cursor modification)
	if (  t.lighting.override>0 ) 
	{
		if (  t.terrain.terrainshaderindex>0 ) 
		{
			if ( GetEffectExist(t.terrain.terrainshaderindex) == 1 ) 
			{
				//  light zero shows edited light
				if (  t.lighting.override == 0 ) 
				{
					if (  t.game.set.ismapeditormode == 1 ) 
					{
						SetVector4 (  g.terrainvectorindex,-999999,-999999,-99999,0 );
					}
				}
				if (  t.lighting.override == 1 ) 
				{
					SetVector4 (  g.terrainvectorindex,t.gridentityposx_f,t.gridentityposy_f,t.gridentityposz_f, t.grideleprof.light.range ); //300116 - added range to pos to skip atten constant (NVIDIA 361)
				}
				if (  t.lighting.override == 2 ) 
				{
					if (  t.widget.pickedEntityIndex>0 ) 
					{
						SetVector4 (  g.terrainvectorindex,t.entityelement[t.widget.pickedEntityIndex].x,t.entityelement[t.widget.pickedEntityIndex].y,t.entityelement[t.widget.pickedEntityIndex].z, t.grideleprof.light.range ); //300116 - added range to pos to skip atten constant (NVIDIA 361)
						t.grideleprof.light = t.entityelement[t.widget.pickedEntityIndex].eleprof.light;
					}
				}
				SetEffectConstantVEx (  t.terrain.terrainshaderindex,t.effectparam.terrain.g_lights_pos0,g.terrainvectorindex );
				SetVector4 (  g.terrainvectorindex,1,1.0/t.grideleprof.light.range,1.0/(t.grideleprof.light.range*t.grideleprof.light.range),0 );
				SetEffectConstantVEx (  t.terrain.terrainshaderindex,t.effectparam.terrain.g_lights_atten0,g.terrainvectorindex );
				SetVector4 (  g.terrainvectorindex,RgbR(t.grideleprof.light.color)/255.0,RgbG(t.grideleprof.light.color)/255.0,RgbB(t.grideleprof.light.color)/255.0,0 );
				SetEffectConstantVEx (  t.terrain.terrainshaderindex,t.effectparam.terrain.g_lights_diffuse0,g.terrainvectorindex );
				int iLightCount = 3; SetVector4 (  g.terrainvectorindex,iLightCount,0,0,0);
				SetEffectConstantVEx (  t.terrain.terrainshaderindex,t.effectparam.terrain.g_lights_data,g.terrainvectorindex );

				//  wipe out rest
				SetVector4 (  g.terrainvectorindex,0,0,0,0 );
				for ( t.ttt = 2 ; t.ttt<=  3; t.ttt++ )
				{
					if (  t.ttt == 2 ) { t.bestpos = t.effectparam.terrain.g_lights_pos1  ; t.bestatt = t.effectparam.terrain.g_lights_atten1 ; t.bestdif = t.effectparam.terrain.g_lights_diffuse1; }
					if (  t.ttt == 3 ) { t.bestpos = t.effectparam.terrain.g_lights_pos2  ; t.bestatt = t.effectparam.terrain.g_lights_atten2 ; t.bestdif = t.effectparam.terrain.g_lights_diffuse2; }
					SetEffectConstantVEx (  t.terrain.terrainshaderindex,t.bestpos,g.terrainvectorindex );
					SetEffectConstantVEx (  t.terrain.terrainshaderindex,t.bestatt,g.terrainvectorindex );
					SetEffectConstantVEx (  t.terrain.terrainshaderindex,t.bestdif,g.terrainvectorindex );
					if (  t.terrain.vegetationshaderindex>0 ) 
					{
						if ( GetEffectExist(t.terrain.vegetationshaderindex) == 1 ) 
						{
							if (  t.ttt == 2 ) { t.bestpos = t.effectparam.vegetation.g_lights_pos1  ; t.bestatt = t.effectparam.vegetation.g_lights_atten1 ; t.bestdif = t.effectparam.vegetation.g_lights_diffuse1; }
							if (  t.ttt == 3 ) { t.bestpos = t.effectparam.vegetation.g_lights_pos2  ; t.bestatt = t.effectparam.vegetation.g_lights_atten2 ; t.bestdif = t.effectparam.vegetation.g_lights_diffuse2; }
							SetEffectConstantVEx (  t.terrain.vegetationshaderindex,t.bestpos,g.terrainvectorindex );
							SetEffectConstantVEx (  t.terrain.vegetationshaderindex,t.bestatt,g.terrainvectorindex );
							SetEffectConstantVEx (  t.terrain.vegetationshaderindex,t.bestdif,g.terrainvectorindex );
						}
					}
				}

			}
		}
	}

	//  For all shaders, feed in latest spot light activity (and flash light control)
	if (  t.terrain.terrainshaderindex>0 && t.terrain.vegetationshaderindex>0 ) 
	{
		if ( GetEffectExist(t.terrain.vegetationshaderindex) == 1 ) 
		{
			//  Spot light shader updatesusespotlighting
			if (  t.playerlight.spotflash>0 ) 
			{
				SetVector4 (  g.terrainvectorindex1,t.playerlight.spotflashx_f,t.playerlight.spotflashy_f,t.playerlight.spotflashz_f,t.playerlight.spotfadeout_f );
				SetVector4 (  g.terrainvectorindex2,t.playerlight.spotlightr_f,t.playerlight.spotlightg_f,t.playerlight.spotlightb_f,t.playerlight.flashlightcontrol_f );
				SetEffectConstantVEx (  t.terrain.terrainshaderindex,t.effectparam.terrain.SpotFlashPos,g.terrainvectorindex1 );
				SetEffectConstantVEx (  t.terrain.terrainshaderindex,t.effectparam.terrain.SpotFlashColor,g.terrainvectorindex2 );
				SetEffectConstantVEx (  t.terrain.vegetationshaderindex,t.effectparam.vegetation.SpotFlashPos,g.terrainvectorindex1 );
				SetEffectConstantVEx (  t.terrain.vegetationshaderindex,t.effectparam.vegetation.SpotFlashColor,g.terrainvectorindex2 );
				for ( t.t = -6 ; t.t<=  g.effectbankmax; t.t++ )
				{
					if (  t.t == -6  )  t.effectid = g.lightmappbreffectillum;
					#ifdef VRTECH
					if (  t.t == -5  )  t.effectid = g.controllerpbreffect;
					#else
					if (  t.t == -5  )  continue;
					#endif
					if (  t.t == -4  )  t.effectid = g.lightmappbreffect;
					if (  t.t == -3  )  t.effectid = g.thirdpersonentityeffect;
					if (  t.t == -2  )  t.effectid = g.thirdpersoncharactereffect;
					if (  t.t == -1  )  t.effectid = g.staticlightmapeffectoffset;
					if (  t.t == 0  )  t.effectid = g.staticshadowlightmapeffectoffset;
					if (  t.t>0  )  t.effectid = g.effectbankoffset+t.t;
					if (  GetEffectExist(t.effectid) == 1 ) 
					{
						SetEffectConstantVEx (  t.effectid,t.effectparamarray[t.effectid].SpotFlashPos,g.terrainvectorindex1 );
						SetEffectConstantVEx (  t.effectid,t.effectparamarray[t.effectid].SpotFlashColor,g.terrainvectorindex2 );
					}
				}
			}
			else
			{
				SetVector4 (  g.terrainvectorindex1,0,0,0,0 );
				SetVector4 (  g.terrainvectorindex2,0,0,0,t.playerlight.flashlightcontrol_f );
				SetEffectConstantVEx (  t.terrain.terrainshaderindex,t.effectparam.terrain.SpotFlashPos,g.terrainvectorindex1 );
				SetEffectConstantVEx (  t.terrain.terrainshaderindex,t.effectparam.terrain.SpotFlashColor,g.terrainvectorindex2 );
				SetEffectConstantVEx (  t.terrain.vegetationshaderindex,t.effectparam.vegetation.SpotFlashPos,g.terrainvectorindex1 );
				SetEffectConstantVEx (  t.terrain.vegetationshaderindex,t.effectparam.vegetation.SpotFlashColor,g.terrainvectorindex2 );
				for ( t.t = -6 ; t.t<=  g.effectbankmax; t.t++ )
				{
					if (  t.t == -6  )  t.effectid = g.lightmappbreffectillum;
					#ifdef VRTECH
					if (  t.t == -5  )  t.effectid = g.controllerpbreffect;
					#else
					if (  t.t == -5  )  continue;
					#endif
					if (  t.t == -4  )  t.effectid = g.lightmappbreffect;
					if (  t.t == -3  )  t.effectid = g.thirdpersonentityeffect;
					if (  t.t == -2  )  t.effectid = g.thirdpersoncharactereffect;
					if (  t.t == -1  )  t.effectid = g.staticlightmapeffectoffset;
					if (  t.t == 0  )  t.effectid = g.staticshadowlightmapeffectoffset;
					if (  t.t>0  )  t.effectid = g.effectbankoffset+t.t;
					if (  GetEffectExist(t.effectid) == 1 ) 
					{
						SetEffectConstantVEx (  t.effectid,t.effectparamarray[t.effectid].SpotFlashPos,g.terrainvectorindex1 );
						SetEffectConstantVEx (  t.effectid,t.effectparamarray[t.effectid].SpotFlashColor,g.terrainvectorindex2 );
					}
				}
			}
		}
	}
}

#endif

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
