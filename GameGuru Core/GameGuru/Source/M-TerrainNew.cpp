

//
// Utterly Nre Terrain System for GameGuru MAX
//

#include "stdafx.h"
#include "gameguru.h"
//#include "wincodec.h"

#ifdef ENABLEIMGUI
#include "..\Imgui\imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "..\Imgui\imgui_internal.h"
#include "..\Imgui\imgui_impl_win32.h"
#include "..\Imgui\imgui_gg_dx11.h"
#endif

//#include "Terrain.h"
#include "Utility/stb_image.h"

#include "GGTerrain\GGTerrainFile.h"
#include "GGTerrain\GGTerrain.h"
using namespace GGTerrain;

#include "GGTerrain\GGTrees.h"
using namespace GGTrees;

#include "GGTerrain\GGGrass.h"
using namespace GGGrass;

#define DYNAMICSUNPOSITION

#ifdef BUSHUI
void imgui_Customize_Bush_v3(int mode);
#endif

//PE: DISPLAY4x4 for new design , will get removed when ready.
#define DISPLAY4x4

void SmallTutorialVideo(char *tutorial, char* combo_items[] = NULL, int combo_entries = 0, int iVideoSection = 0, bool bAutoStart = false);
void visuals_calcsunanglefromtimeofday(int iTimeOfday, float* pfSunAngleX, float* pfSunAngleY, float* pfSunAngleZ);

// shadow mapping
//extern CascadedShadowsManager g_CascadedShadow;
extern int g_iTerrainIDForShadowMap;

#ifdef ENABLEIMGUI
extern bool bHelp_Window;
extern bool bHelpVideo_Window;
extern char cForceTutorialName[1024];
#endif

#ifndef PRODUCTCLASSIC
extern bool bBoostIconColors;
#endif

#ifdef PROCEDURALTERRAINWINDOW
extern bool bProceduralLevel;
extern int iQuitProceduralLevel;
extern bool BackBufferSnapShotMode;
extern bool bSnapShotModeUseCamera;
extern float fSnapShotModeCameraX, fSnapShotModeCameraY, fSnapShotModeCameraZ;
extern float fSnapShotModeCameraAngX, fSnapShotModeCameraAngY, fSnapShotModeCameraAngZ;
extern int BackBufferEntityID;
extern int BackBufferObjectID;
extern int BackBufferImageID;
extern int BackBufferSizeX;
extern int BackBufferSizeY;
extern float BackBufferRotateY;
extern float BackBufferRotateX;
extern float BackBufferRotateZ;
extern float BackBufferZoom;
extern float BackBufferCamMove;
extern float BackBufferCamLeft;
extern float BackBufferCamUp;
extern bool bBackBufferAnimated;
extern bool bBackBufferRestoreCamera;
extern bool bLoopBackBuffer;
extern bool bLoopFullFPS;
extern bool bRotateBackBuffer;
extern cstr BackBufferSaveCacheName;
extern int iLaunchAfterSync;
extern int iSkibFramesBeforeLaunch;
extern float fLastRubberBandX1, fLastRubberBandX2, fLastRubberBandY1, fLastRubberBandY2;
extern ImVec4 drawCol_back;
extern ImVec4 drawCol_normal;
extern ImVec4 drawCol_hover;
extern ImVec4 drawCol_Down;
bool bPopModalOpenProcedural = false;
bool bPopModalOpenProceduralCameraMode = false;
bool bPopModalTakeMapSnapshot = false;
#endif

#ifdef CUSTOMTEXTURES
int g_iCustomTerrainMatSounds[32] = { 10 };
#endif

int g_iDeferTextureUpdateToNow = 0;
cstr g_DeferTextureUpdateMAXRootFolder_s = "";
cstr g_DeferTextureUpdateCurrentFolder_s = "";
std::vector<std::string> g_DeferTextureUpdate;
std::vector<int> g_DeferTextureUpdateIncompatibleTextures;

#ifdef WICKEDENGINE
void Wicked_Update_Visuals(void *voidvisual);
ImVec4 vLastTerrainPickPosition = ImVec4(0, 0,0,0);
ImVec4 vLastRampTerrainPickPosition = ImVec4(0, 0, 0, 0);
extern bool bProceduralLevelFromStoryboard;

#include "..\..\Guru-WickedMAX\master.h"
extern wiECS::Entity g_weatherEntityID;
extern MasterRenderer * master_renderer;

#endif

// Terrain Build Globals
#define TERRAINTEXPANELSPRMAX 6

int sTerrainTexturesID[32];
int sTerrainSelectionID[32];
bool bTextureNameWindow[32];
bool iDeleteAllTerrainTextures = false;
int iDeleteSingleTerrainTextures = 0;
int iCurrentTextureForPaint = 0;
bool bUpdateTerrainMaterials = false;
int iOldMaterial = -1;
int sGrassTexturesID[128];
bool iDeleteAllGrassTextures = false;
int iDeleteSingleGrassTextures = 0;
bool bUpdateGrassMaterials = false;
int iOldGrassMaterial = -1;
bool bCurrentGrassTextureForPaint[128];
cStr sGrassChangedTextures[128];

bool bGrassNameWindow[128];

float g_fvegRandomMin = 0.0f;
float g_fvegRandomMax = 100.0f;

float g_fvegDensityMin = 0.0f;
float g_fvegDensityMax = 100.0f;

int iBrushStrength = 128;
int iBrushShape = 0;

int g_iDelayActualObjectAdjustment = 0;
int g_iDelayActualObjectAdjustmentSculptCount = 0;

extern int iLastOpenHeader;
extern bool bRenderTabTab;

// Tried switching between texture_D.dds and texture_D.jpg but JPG just as large, lower quality and loses a channel!
#define TEXTURE_D_NAME "texture_D.dds"
#define TEXTURE_N_NAME "texture_N.dds"

/* g_pTerrain no longer used
MaxTerrain::cTerrain* g_pTerrain = NULL;
*/

VOID DXUtil_ConvertWideStringToAnsi ( CHAR* strDestination, const WCHAR* wstrSource,int cchDestChar );

// Terrain Build Local Structure
struct terrainbuildtype
{
	int initialised;
	int active;
	bool bReleaseMouseFirst;
	bool bUsingTerrainTextureSystemPaintSelector;
	bool bCustomiseTexture;
	cstr terrainstyle;
	int iCurrentTexture;
	int iTexturePanelSprite[6];
	int iTexturePanelImg[6];
	int iTexturePanelX;
	int iTexturePanelY;
	int iTexturePanelWidth;
	int iTexturePanelHeight;
	int iTexturePanelHighSprite;
	int iTexturePanelHighImg;
	int iHelpSpr;
	int iHelpImg;
	int iTexHelpSpr;
	int iTexHelpImg;
	int iTexPlateImage;
};
terrainbuildtype terrainbuild;

#ifdef VRTECH
#ifdef ENABLEIMGUI

int delay_terrain_execute = 0;
int skib_terrain_frames_execute = 0;

extern bool bTerrain_Tools_Window;
extern int grideleprof_uniqui_id;
extern preferences pref;
extern bool bForceKey;
extern int iForceScancode;
extern cstr csForceKey;
extern bool bForceKey2;
extern cstr csForceKey2;
extern bool imgui_is_running;
bool bDisableAllTerrainSprites = true;

bool bEnableVeg = true; //Display veg on by default.
bool bUpdateVeg = true; //Update veg on by default.
int iLastUpdateVeg = 0;
int iForceUpdateVegetation = 0;
bool bEnableWeather = false;
extern bool bTutorialCheckAction;
bool TutorialNextAction(void);
bool CheckTutorialPlaceit(void);
bool CheckTutorialAction(const char * action, float x_adder = 0.0f);

int iTerrainPaintMode = 1;
int iTerrainGrassPaintMode = 1;
int iTerrainRaiseMode = 1;
int iLastRegionUpdateX, iLastRegionUpdateY;
int iTerrainVegLoopUpdate = 0;
extern bool bImGuiGotFocus;
#else
bool bDisableAllTerrainSprites = false;
#endif
bool bVegHasChanged = false;
#endif

#ifdef WICKEDENGINE
struct NewLevelCamera
{
	float x;
	float y;
	float z;
	int set = 0;
};
NewLevelCamera newLevelCamera;
void PositionCameraForNewLevel();
#endif

/* g_bTerrainGeneratorChooseRealTerrain no longer used
// terrain generator grey grid vs real terrain
bool g_bTerrainGeneratorChooseRealTerrain = false;
*/

// Prototypes
void set_inputsys_mclick(int value);

#if defined(ENABLEIMGUI)
int current_mode = 0;
ImVec4 tool_selected_col;

void imgui_terrain_loop_v2(void)
{
	if (!imgui_is_running)
		return;

	// terrain editing causes grass to fully update (hills raise grass)
	if (iForceUpdateVegetation == 2)
	{
		iForceUpdateVegetation = 0;
		extern bool bFullVegUpdate;
		bFullVegUpdate = true;
		bUpdateVeg = true;
	}

	//if (bUpdateVeg && Timer() - iLastUpdateVeg > 500 && !object_preload_files_in_progress() ) 
	if (bUpdateVeg)
	{
		if (bEnableVeg)
		{
			t.visuals.VegQuantity_f = t.gamevisuals.VegQuantity_f;
			t.visuals.VegWidth_f = t.gamevisuals.VegWidth_f;
			t.visuals.VegHeight_f = t.gamevisuals.VegHeight_f;
			grass_setgrassgridandfade();

			extern bool bResourcesSet, bGridMade;
			if (!(ObjectExist(t.tGrassObj) == 1 && GetMeshExist(t.tGrassObj) == 1))
				grass_init();
			else if (!bGridMade)
			{
				//grass_init();
				//grass_setgrassimage();
				//UpdateGrassTexture(g.gpbroverride);
			}

			bool bOldGridMade = bGridMade;
			int iTrimUsingGrassMemblock = 0;
			if (t.game.gameisexe == 1) iTrimUsingGrassMemblock = t.terrain.grassmemblock;
			if (g.usegrassbelowwater > 0)
				MakeVegetationGridQuick(4.0f*t.visuals.VegQuantity_f, t.visuals.VegWidth_f, t.visuals.VegHeight_f, terrain_veg_areawidth, t.terrain.vegetationgridsize, t.tTerrainID, iTrimUsingGrassMemblock, true);
			else
				MakeVegetationGridQuick(4.0f*t.visuals.VegQuantity_f, t.visuals.VegWidth_f, t.visuals.VegHeight_f, terrain_veg_areawidth, t.terrain.vegetationgridsize, t.tTerrainID, iTrimUsingGrassMemblock, false);

			if (bGridMade && !bOldGridMade)
			{
				//Refresh textures.
				//grass_setgrassimage();
			}

			// small lookup for memblock painting circles
			static bool bCurveDataSet = false;
			if (!bCurveDataSet) {
				Dim(t.curve_f, 100);
				for (t.r = 0; t.r <= 180; t.r++)
				{
					t.trx_f = Cos(t.r - 90)*100.0;
					t.trz_f = Sin(t.r - 90)*100.0;
					t.curve_f[int((100 + t.trz_f) / 2)] = t.trx_f / 100.0;
				}
				bCurveDataSet = true;
			}
			t.terrain.grassregionupdate = 0; //PE: Make sure we update.
			t.terrain.grassupdateafterterrain = 1;
			t.terrain.lastgrassupdatex1 = -1; //PE: Force update.
			grass_loop();
			t.terrain.grassupdateafterterrain = 0;
			ShowVegetationGrid();
			visuals_justshaderupdate();
			iLastUpdateVeg = Timer();
		}
		else
		{
			HideVegetationGrid();
			iLastUpdateVeg = Timer();
		}
		bUpdateVeg = false;
	}
	else
	{
		bool bReadyToUpdateVeg = false;
		if (bVegHasChanged)
			bReadyToUpdateVeg = true;

		if (bEnableVeg && iTerrainVegLoopUpdate++ > 10)
		{
			grass_loop();
			iTerrainVegLoopUpdate = 0;
		}

		//Continue cheking if we need to update terrain.
		//if (t.inputsys.mclick == 0 && bReadyToUpdateVeg && bEnableVeg && Timer() - iLastUpdateVeg > 250 && !object_preload_files_in_progress() ) 
		if (bReadyToUpdateVeg && bEnableVeg)
		{
			t.visuals.VegQuantity_f = t.gamevisuals.VegQuantity_f;
			t.visuals.VegWidth_f = t.gamevisuals.VegWidth_f;
			t.visuals.VegHeight_f = t.gamevisuals.VegHeight_f;
			grass_setgrassgridandfade();

			if (!(ObjectExist(t.tGrassObj) == 1 && GetMeshExist(t.tGrassObj) == 1))
				grass_init();

			t.terrain.grassupdateafterterrain = 1;
			grass_loop();
			t.terrain.grassupdateafterterrain = 0;
			ShowVegetationGrid();

			bReadyToUpdateVeg = false;
			iLastUpdateVeg = Timer();
			bVegHasChanged = false;
		}
	}

	if (t.grideditselect == 0 && t.terrain.terrainpaintermode >= 0 && t.terrain.terrainpaintermode <= 10)
	{
#ifdef ENABLECUSTOMTERRAIN
		switch (delay_terrain_execute)
		{
		case 1: //Change texture in plate.
		{
			delay_terrain_execute = 0;
			terrainbuild.bCustomiseTexture = true;
			skib_terrain_frames_execute = 5;
			break;
		}
		default:
			break;
		}
#endif
		if (skib_terrain_frames_execute > 0)
			skib_terrain_frames_execute--;

		if (bTerrain_Tools_Window) 
		{
			float media_icon_size = 40.0f;
			float plate_width = (media_icon_size + 6.0) * 4.0f;
			grideleprof_uniqui_id = 16000;
			int icon_size = 60;
			ImVec2 iToolbarIconSize = { (float)icon_size, (float)icon_size };
			ImVec2 tool_selected_padding = { 1.0, 1.0 };
			tool_selected_col = ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram];
			if (pref.current_style == 3)
				tool_selected_col = ImGui::GetStyle().Colors[ImGuiCol_Button];

			current_mode = -1;
			if (t.terrain.terrainpaintermode >= 6) 
			{
				if (t.terrain.terrainpaintermode == 10) 
				{
					current_mode = TOOL_PAINTGRASS;
				}
				else 
				{
					current_mode = TOOL_PAINTTEXTURE;
				}
			}
			else 
			{
				if (t.terrain.terrainpaintermode == 1)
					current_mode = TOOL_SHAPE;
				if (t.terrain.terrainpaintermode == 2)
					current_mode = TOOL_LEVELMODE;
				if (t.terrain.terrainpaintermode == 3)
					current_mode = TOOL_STOREDLEVEL;
				if (t.terrain.terrainpaintermode == 4)
					current_mode = TOOL_BLENDMODE;
				if (t.terrain.terrainpaintermode == 5)
					current_mode = TOOL_RAMPMODE;
			}

			cstr sWindowLabel = "Sculpt Terrain##TerrainToolsWindow";
			if (current_mode == TOOL_PAINTGRASS)
				sWindowLabel = "Add Vegetation##TerrainToolsWindow";
			if (current_mode == TOOL_PAINTTEXTURE)
				sWindowLabel = "Paint Terrain##TerrainToolsWindow";

			extern int iGenralWindowsFlags;
			ImGui::Begin(sWindowLabel.Get(), &bTerrain_Tools_Window, iGenralWindowsFlags);

			float w = ImGui::GetWindowContentRegionWidth();
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			if (pref.bAutoClosePropertySections && iLastOpenHeader != 30)
				ImGui::SetNextItemOpen(false, ImGuiCond_Always);

			if (ImGui::StyleCollapsingHeader("Edit Mode", ImGuiTreeNodeFlags_DefaultOpen))
			{
				iLastOpenHeader = 30;
#ifdef GGTERRAIN_USE_NEW_TERRAIN

				extern wiECS::Entity g_weatherEntityID;
				wiScene::WeatherComponent* weather = wiScene::GetScene().weathers.GetComponent(g_weatherEntityID);

				// do this to unfocus text input when right clicking on main window
				if ( ImGui::IsMouseClicked(1) ) 
				{
					//PE: This will close down any popups.
					extern bool bPopModalOpenEntity;
					if (!bPopModalOpenEntity && !bPopModalOpenProcedural)
					{
						ImGui::FocusWindow(NULL);
						ImGui::ClearActiveID();
					}
				}

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

				ImGui::PushItemWidth(-10);

				float w = ImGui::GetWindowContentRegionWidth();
				int icon_size = (w - 20.0) / 3.0;
				icon_size -= 7; //Padding
				
				ImVec2 oldstyle = ImGui::GetStyle().FramePadding;
				ImGui::GetStyle().FramePadding = { 2,2 };

				static int terrainMode = -1; // -1 = inital value, 0 = generate, 1 = edit, 2 = paint
				
				if ( terrainMode == 0 )
				{
					ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					ImVec2 padding = { 2.0, 2.0 };
					const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(icon_size, icon_size));
					ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
				}

				if ( ImGui::ImgBtn(KEY_G, ImVec2(icon_size, icon_size), ImColor(255,255,255,0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255) ,-1,0,0,0,false,false,false,false,false, bBoostIconColors)
					 || terrainMode == -1 )
				{
					terrainMode = 0;
					weather->SetVolumetricClouds( false );
					ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_USE_FOG;
					GGTerrain_SetEditSizeVisible( 1 );
					GGTerrain_SetMiniMapVisible( 0 );
				}
				if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("Generate New Terrain");
				ImGui::SameLine();

				if ( terrainMode == 1 )
				{
					ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					ImVec2 padding = { 2.0, 2.0 };
					const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(icon_size, icon_size));
					ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
				}
				if (ImGui::ImgBtn(TOOL_LEVELMODE, ImVec2(icon_size, icon_size), ImColor(255, 255, 255, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
				{
					terrainMode = 1;
					weather->SetVolumetricClouds( true );
					ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_USE_FOG;
					GGTerrain_SetEditSizeVisible( 1 );
					GGTerrain_SetMiniMapVisible( 0 );
				}
				if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("Sculpt Terrain");
				ImGui::SameLine();

				if ( terrainMode == 2 )
				{
					ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					ImVec2 padding = { 2.0, 2.0 };
					const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(icon_size, icon_size));
					ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
				}
				if (ImGui::ImgBtn(EBE_CONTROL2, ImVec2(icon_size, icon_size), ImColor(255, 255, 255, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
				{
					terrainMode = 2;
					weather->SetVolumetricClouds( true );
					ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_USE_FOG;
					GGTerrain_SetEditSizeVisible( 0 );
					GGTerrain_SetMiniMapVisible( 0 );
				}
				if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("Paint Terrain");

				ImGui::GetStyle().FramePadding = oldstyle;

				ImGui::Separator();
				
				//ggterrain_extra_params.edit_mode = GGTERRAIN_EDIT_NONE;

				ImGui::Text("Enable Water"); ImGui::SameLine();
				if ( ImGui::Checkbox("##EnableWater", &t.visuals.bWaterEnable)) 
				{
					t.showeditorwater = t.visuals.bWaterEnable ? 1 : 0;
					t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
					Wicked_Update_Visuals((void *)&t.visuals);
				}

				float meterValue;

				switch( terrainMode )
				{
					case 0: // generate
					{
						float fButSizeX = ImGui::GetContentRegionAvailWidth() / 4.0;
						fButSizeX -= 6.0f; //Padding.
						float fButSizeY = fButSizeX * 0.60f;

						if (ImGui::StyleButton("Desert", ImVec2(fButSizeX, fButSizeY)))
						{
							ggterrain_global_params.offset_y = GGTerrain_MetersToUnits(45);
							ggterrain_global_params.height = GGTerrain_MetersToUnits(48);
							ggterrain_global_params.minHeight = GGTerrain_MetersToUnits(48);
							ggterrain_global_params.noise_power = 1.258f;
							ggterrain_global_params.noise_fallof_power = 1.539f;
							ggterrain_global_params.fractal_levels = 10;
							ggterrain_global_params.fractal_initial_freq = 0.282f;
							ggterrain_global_params.fractal_freq_increase = 2.5f;
							ggterrain_global_params.fractal_freq_weight = 0.4;
							ggterrain_global_render_params.baseLayerMaterial = 0x100 | 7;
							ggterrain_global_render_params.layerMatIndex[0] = 0x100 | 7;
							ggterrain_global_render_params.layerMatIndex[1] = 0x100 | 7;
							ggterrain_global_render_params.layerMatIndex[2] = 0x100 | 7;
							ggterrain_global_render_params.layerStartHeight[0] = GGTerrain_MetersToUnits(0.0f);
							ggterrain_global_render_params.layerStartHeight[1] = GGTerrain_MetersToUnits(4.572f);
							ggterrain_global_render_params.layerStartHeight[2] = GGTerrain_MetersToUnits(145.7f);
							ggterrain_global_render_params.layerStartHeight[3] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerStartHeight[4] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerEndHeight[0] = GGTerrain_MetersToUnits(1.524f);
							ggterrain_global_render_params.layerEndHeight[1] = GGTerrain_MetersToUnits(9.144f);
							ggterrain_global_render_params.layerEndHeight[2] = GGTerrain_MetersToUnits(205.7f);
							ggterrain_global_render_params.layerEndHeight[3] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerEndHeight[4] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.slopeMatIndex[0] = 0x100 | 6;
							ggterrain_global_render_params.slopeMatIndex[1] = 0x100 | 6;
							ggterrain_global_render_params.slopeStart[0] = 0.2f;
							ggterrain_global_render_params.slopeStart[1] = 1.0f;
							ggterrain_global_render_params.slopeEnd[0] = 0.4f;
							ggterrain_global_render_params.slopeEnd[1] = 1.0f;

							ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS0) | GGTERRAIN_FRACTAL_RIDGES0;
							ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS1) | GGTERRAIN_FRACTAL_RIDGES1;
							ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS2) | GGTERRAIN_FRACTAL_RIDGES2;
							ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS3) | GGTERRAIN_FRACTAL_RIDGES3;

							// ggtrees_global_params.draw_enabled = 0; //PE: Trees is now only controlled by visual.ini t.visuals.bEndableTreeDrawing
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Desert Theme");
						ImGui::SameLine();

						if (ImGui::StyleButton("Snow", ImVec2(fButSizeX, fButSizeY))) //Arctic
						{
							ggterrain_global_params.offset_y = GGTerrain_MetersToUnits(0);
							ggterrain_global_params.height = GGTerrain_MetersToUnits(73);
							ggterrain_global_params.minHeight = GGTerrain_MetersToUnits(73);
							ggterrain_global_params.noise_power = 0.487f;
							ggterrain_global_params.noise_fallof_power = 0.144f;
							ggterrain_global_params.fractal_levels = 7;
							ggterrain_global_params.fractal_initial_freq = 0.282f;
							ggterrain_global_params.fractal_freq_increase = 2.5f;
							ggterrain_global_params.fractal_freq_weight = 0.4;
							ggterrain_global_render_params.baseLayerMaterial = 0x100 | 12;
							ggterrain_global_render_params.layerMatIndex[0] = 0x100 | 12;
							ggterrain_global_render_params.layerMatIndex[1] = 0x100 | 12;
							ggterrain_global_render_params.layerMatIndex[2] = 0x100 | 12;
							ggterrain_global_render_params.layerStartHeight[0] = GGTerrain_MetersToUnits(0.0f);
							ggterrain_global_render_params.layerStartHeight[1] = GGTerrain_MetersToUnits(4.572f);
							ggterrain_global_render_params.layerStartHeight[2] = GGTerrain_MetersToUnits(145.7f);
							ggterrain_global_render_params.layerStartHeight[3] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerStartHeight[4] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerEndHeight[0] = GGTerrain_MetersToUnits(1.524f);
							ggterrain_global_render_params.layerEndHeight[1] = GGTerrain_MetersToUnits(9.144f);
							ggterrain_global_render_params.layerEndHeight[2] = GGTerrain_MetersToUnits(205.7f);
							ggterrain_global_render_params.layerEndHeight[3] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerEndHeight[4] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.slopeMatIndex[0] = 0x100 | 12;
							ggterrain_global_render_params.slopeMatIndex[1] = 0x100 | 12;
							ggterrain_global_render_params.slopeStart[0] = 0.2f;
							ggterrain_global_render_params.slopeStart[1] = 1.0f;
							ggterrain_global_render_params.slopeEnd[0] = 0.4f;
							ggterrain_global_render_params.slopeEnd[1] = 1.0f;

							ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS0) | GGTERRAIN_FRACTAL_RIDGES0;
							ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS1) | GGTERRAIN_FRACTAL_RIDGES1;
							ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS2) | GGTERRAIN_FRACTAL_RIDGES2;
							ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS3) | GGTERRAIN_FRACTAL_RIDGES3;

							//ggtrees_global_params.draw_enabled = 0; //PE: Trees is now only controlled by visual.ini t.visuals.bEndableTreeDrawing
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Arctic Theme");
						ImGui::SameLine();

						if (ImGui::StyleButton("Temperate", ImVec2(fButSizeX, fButSizeY)))
						{
							ggterrain_global_params.offset_y = GGTerrain_MetersToUnits(8.9f);
							ggterrain_global_params.height = GGTerrain_MetersToUnits(131.5f);
							ggterrain_global_params.minHeight = GGTerrain_MetersToUnits(131.5f);
							ggterrain_global_params.noise_power = 1.3f;
							ggterrain_global_params.noise_fallof_power = 0.323f;
							ggterrain_global_params.fractal_levels = 6;
							ggterrain_global_params.fractal_initial_freq = 0.3f;
							ggterrain_global_params.fractal_freq_increase = 2.5f;
							ggterrain_global_params.fractal_freq_weight = 0.4;
							ggterrain_global_render_params.baseLayerMaterial = 0x100 | 17;
							ggterrain_global_render_params.layerMatIndex[0] = 0x100 | 28;
							ggterrain_global_render_params.layerMatIndex[1] = 0x100 | 29;
							ggterrain_global_render_params.layerMatIndex[2] = 0x100 | 0;
							ggterrain_global_render_params.layerStartHeight[0] = GGTerrain_MetersToUnits(0.0f);
							ggterrain_global_render_params.layerStartHeight[1] = GGTerrain_MetersToUnits(2.657f);
							ggterrain_global_render_params.layerStartHeight[2] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerStartHeight[3] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerStartHeight[4] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerEndHeight[0] = GGTerrain_MetersToUnits(1.524f);
							ggterrain_global_render_params.layerEndHeight[1] = GGTerrain_MetersToUnits(5.027f);
							ggterrain_global_render_params.layerEndHeight[2] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerEndHeight[3] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerEndHeight[4] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.slopeMatIndex[0] = 0x100 | 30;
							ggterrain_global_render_params.slopeMatIndex[1] = 0x100 | 30;
							ggterrain_global_render_params.slopeStart[0] = 0.07f;
							ggterrain_global_render_params.slopeStart[1] = 1.0f;
							ggterrain_global_render_params.slopeEnd[0] = 0.2f;
							ggterrain_global_render_params.slopeEnd[1] = 1.0f;

							ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_RIDGES0) | GGTERRAIN_FRACTAL_VALLEYS0;
							ggterrain_global_params.fractal_flags = ggterrain_global_params.fractal_flags & ~(GGTERRAIN_FRACTAL_VALLEYS1 | GGTERRAIN_FRACTAL_RIDGES1);
							ggterrain_global_params.fractal_flags = ggterrain_global_params.fractal_flags & ~(GGTERRAIN_FRACTAL_VALLEYS2 | GGTERRAIN_FRACTAL_RIDGES2);
							ggterrain_global_params.fractal_flags = ggterrain_global_params.fractal_flags & ~(GGTERRAIN_FRACTAL_VALLEYS3 | GGTERRAIN_FRACTAL_RIDGES3);

							// gtrees_global_params.draw_enabled = 1; //PE: Trees is now only controlled by visual.ini t.visuals.bEndableTreeDrawing
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Temperate Theme");
						ImGui::SameLine();

						if (ImGui::StyleButton("Mountains", ImVec2(fButSizeX, fButSizeY)))
						{
							ggterrain_global_params.offset_y = GGTerrain_MetersToUnits(87);
							ggterrain_global_params.height = GGTerrain_MetersToUnits(300);
							ggterrain_global_params.minHeight = GGTerrain_MetersToUnits(300);
							ggterrain_global_params.noise_power = 0.612f;
							ggterrain_global_params.noise_fallof_power = 0.299f;
							ggterrain_global_params.fractal_levels = 10;
							ggterrain_global_params.fractal_initial_freq = 0.371f;
							ggterrain_global_params.fractal_freq_increase = 2.5f;
							ggterrain_global_params.fractal_freq_weight = 0.4;
							ggterrain_global_render_params.baseLayerMaterial = 0x100 | 17;
							ggterrain_global_render_params.layerMatIndex[0] = 0x100 | 20;
							ggterrain_global_render_params.layerMatIndex[1] = 0x100 | 19;
							ggterrain_global_render_params.layerMatIndex[2] = 0x100 | 22;
							ggterrain_global_render_params.layerStartHeight[0] = GGTerrain_MetersToUnits(0.0f);
							ggterrain_global_render_params.layerStartHeight[1] = GGTerrain_MetersToUnits(4.572f);
							ggterrain_global_render_params.layerStartHeight[2] = GGTerrain_MetersToUnits(145.7f);
							ggterrain_global_render_params.layerStartHeight[3] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerStartHeight[4] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerEndHeight[0] = GGTerrain_MetersToUnits(1.524f);
							ggterrain_global_render_params.layerEndHeight[1] = GGTerrain_MetersToUnits(9.144f);
							ggterrain_global_render_params.layerEndHeight[2] = GGTerrain_MetersToUnits(205.7f);
							ggterrain_global_render_params.layerEndHeight[3] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.layerEndHeight[4] = GGTerrain_MetersToUnits(1500.0f);
							ggterrain_global_render_params.slopeMatIndex[0] = 0x100 | 18;
							ggterrain_global_render_params.slopeMatIndex[1] = 0x100 | 18;
							ggterrain_global_render_params.slopeStart[0] = 0.2f;
							ggterrain_global_render_params.slopeStart[1] = 1.0f;
							ggterrain_global_render_params.slopeEnd[0] = 0.4f;
							ggterrain_global_render_params.slopeEnd[1] = 1.0f;

							ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_RIDGES0) | GGTERRAIN_FRACTAL_VALLEYS0;
							ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS1) | GGTERRAIN_FRACTAL_RIDGES1;
							ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS2) | GGTERRAIN_FRACTAL_RIDGES2;
							ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS3) | GGTERRAIN_FRACTAL_RIDGES3;

							// ggtrees_global_params.draw_enabled = 1; //PE: Trees is now only controlled by visual.ini t.visuals.bEndableTreeDrawing
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Mountain Theme");

						fButSizeX = ImGui::GetContentRegionAvailWidth();
						if (ImGui::StyleButton("Randomize", ImVec2(fButSizeX, 30.0f)))
						{
							ggterrain_global_params.seed = Random2();
							float height = pow( RandomFloat(), 0.7 );
							height = height * 590 + 10;
							ggterrain_global_params.height = GGTerrain_MetersToUnits( height );
							ggterrain_global_params.minHeight = GGTerrain_MetersToUnits( height );
							float offsety = pow( RandomFloat(), 1.7 );
							if ( Random2() & 0x01 ) offsety = -offsety;
							offsety *= 50;
							if ( offsety + height < 5 ) offsety = 5 - height;
							ggterrain_global_params.offset_y = GGTerrain_MetersToUnits( offsety );
							ggterrain_global_params.noise_power = RandomFloat() * 1.5f + 0.5f;
							ggterrain_global_params.noise_fallof_power = RandomFloat() * 1.2f;
						}

						ImGui::Text("Seed"); ImGui::SameLine();
						int step = 1; int step_fast = 100;
						ImGui::InputScalar( "##TerrainSeed", ImGuiDataType_U32, &ggterrain_global_params.seed, &step, &step_fast, "%u", ImGuiInputTextFlags_AutoSelectAll );

						ImGui::TextCenter("Performance Setting");
						static int terrain_performance = 2;
						if ( ImGui::SliderInt("##TerrainPerformance", &terrain_performance, 0, 3) )
						{
							GGTerrain_SetPerformanceMode( terrain_performance );
						}

						//ImGui::Text("Resolution Reduction");
						//ImGui::SliderInt("##TerrainResReduction", (int*)&ggterrain_global_render_params2.readBackTextureReduction, 4, 16);
						
						ImGui::TextCenter("Editable Size (meters)");
						// editable_size is from center to edge whereas meterValue is edge to edge so multiply by 2
						meterValue = GGTerrain_UnitsToMeters( ggterrain_global_render_params2.editable_size*2 );
						if ( ImGui::SliderFloat("##EditableSize", &meterValue, 500.0f, 5000.0f) ) 
						{
							ggterrain_global_render_params2.editable_size = GGTerrain_MetersToUnits( meterValue/2 );
						}

						if (pref.iTerrainAdvanced)
						{
							ImGui::TextCenter("Show Editable Area");
							ImGui::Text("2D");
							ImGui::SameLine();
							bool showMapSize = (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE) != 0;
							if (ImGui::Checkbox("##EditBoxVisible", &showMapSize))
							{
								if (showMapSize) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
								else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
							}
							ImGui::SameLine();
							ImGui::Text(" 3D");
							ImGui::SameLine();
							showMapSize = (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D) != 0;
							if (ImGui::Checkbox("##EditBoxVisible3D", &showMapSize))
							{
								if (showMapSize) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
								else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
							}
							ImGui::SameLine();

							ImGui::Text(" MiniMap");
							ImGui::SameLine();
							showMapSize = (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP) != 0;
							if (ImGui::Checkbox("##EditBoxVisibleMiniMap", &showMapSize))
							{
								if (showMapSize) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP;
								else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP;
							}
						}
						ImGui::TextCenter("Offset X");
						meterValue = GGTerrain_OffsetToMeters( ggterrain_global_params.offset_x );
						if ( ImGui::SliderFloat("##Offset X", &meterValue, -10000.0f, 10000.0f) )
						{
							ggterrain_global_params.offset_x = GGTerrain_MetersToOffset( meterValue );
						}

						ImGui::TextCenter("Offset Z");
						meterValue = GGTerrain_OffsetToMeters( ggterrain_global_params.offset_z );
						if ( ImGui::SliderFloat("##Offset Z", &meterValue, -10000.0f, 10000.0f) )
						{
							ggterrain_global_params.offset_z = GGTerrain_MetersToOffset( meterValue );
						}

						ImGui::TextCenter("Offset Y (meters)");
						meterValue = GGTerrain_UnitsToMeters( ggterrain_global_params.offset_y );
						if ( ImGui::SliderFloat("##Offset Y", &meterValue, -1000.0f, 1000.0f, "%.1f", 2.0f) )
						{
							ggterrain_global_params.offset_y = GGTerrain_MetersToUnits( meterValue );
						}

						// maximum height of the terrain
						ImGui::TextCenter("Max Height (meters)");
						meterValue = GGTerrain_UnitsToMeters( ggterrain_global_params.height );
						if ( ImGui::SliderFloat("##HeightRange", &meterValue, 0.0f, 1000.0f) )
						{
							ggterrain_global_params.height = GGTerrain_MetersToUnits( meterValue );
						}

						ImGui::TextCenter("Min Height Underwater (meters)");
						meterValue = GGTerrain_UnitsToMeters( ggterrain_global_params.minHeight );
						if ( ImGui::SliderFloat("##MinHeightRange", &meterValue, 0.0f, 1000.0f) )
						{
							ggterrain_global_params.minHeight = GGTerrain_MetersToUnits( meterValue );
						}

						ImGui::TextCenter("Height Map Scale");
						ImGui::SliderFloat("##HeightMapScale", &ggterrain_global_params.heightmap_scale, 0.01f, 10.0f, "%.3f", 2.0f);

						meterValue = GGTerrain_UnitsToMeters( ggterrain_global_params.height_outside_heightmap );
						ImGui::TextCenter("Height Map Outside Height");
						if ( ImGui::SliderFloat("##HeightMapOutHeight", &meterValue, -1000, 4000.0f, "%.1f", 1.5f) )
						{
							ggterrain_global_params.height_outside_heightmap = GGTerrain_MetersToUnits( meterValue );
						}

						ImGui::TextCenter("Height Map Outside Slope");
						ImGui::SliderFloat("##HeightMapOutFade", &ggterrain_global_params.fade_outside_heightmap, 0, 500.0f, "%.1f", 1.0f);

						int outsideType = GGTerrain_GetGenerateTerrainOutsideHeightMap();
						ImGui::TextCenter("Height Map Outside Type");
						if ( ImGui::SliderInt("##HeightMapOutType", &outsideType, 0, 1) )
						{
							GGTerrain_SetGenerateTerrainOutsideHeightMap( outsideType );
						}
				
						// how the noise value is modified after being generated
						ImGui::TextCenter("Noise Curve");
						ImGui::SliderFloat("##NoisePower", &ggterrain_global_params.noise_power, 0.01f, 6.0f, "%.3f", 2.0f);

						// how the noise value is modified by the height of the previous detail levels
						ImGui::TextCenter("Noise Falloff");
						ImGui::SliderFloat("##NoiseFalloffPower", &ggterrain_global_params.noise_fallof_power, 0.0f, 6.0f, "%.3f", 2.0f);

						// number of iterations of noise to create the fractal noise
						ImGui::TextCenter("Noise Iterations");
						ImGui::SliderInt("##FractalIterations", (int*)&ggterrain_global_params.fractal_levels, 1, 14);

						ImGui::TextCenter("Rivers/Valleys");
						int selection = 0;
						if ( ggterrain_global_params.fractal_flags & GGTERRAIN_FRACTAL_VALLEYS0 ) selection = 1;
						else if ( ggterrain_global_params.fractal_flags & GGTERRAIN_FRACTAL_RIDGES0 ) selection = 2;
						if ( ImGui::SliderInt("##FractalRiversValleys0", &selection, 0, 2) )
						{
							switch( selection )
							{
								case 0: ggterrain_global_params.fractal_flags &= ~(GGTERRAIN_FRACTAL_VALLEYS0 | GGTERRAIN_FRACTAL_RIDGES0); break;
								case 1: ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_RIDGES0) | GGTERRAIN_FRACTAL_VALLEYS0; break;
								case 2: ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS0) | GGTERRAIN_FRACTAL_RIDGES0; break;
							}
						}

						selection = 0;
						if ( ggterrain_global_params.fractal_flags & GGTERRAIN_FRACTAL_VALLEYS1 ) selection = 1;
						else if ( ggterrain_global_params.fractal_flags & GGTERRAIN_FRACTAL_RIDGES1 ) selection = 2;
						if ( ImGui::SliderInt("##FractalRiversValleys1", &selection, 0, 2) )
						{
							switch( selection )
							{
								case 0: ggterrain_global_params.fractal_flags &= ~(GGTERRAIN_FRACTAL_VALLEYS1 | GGTERRAIN_FRACTAL_RIDGES1); break;
								case 1: ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_RIDGES1) | GGTERRAIN_FRACTAL_VALLEYS1; break;
								case 2: ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS1) | GGTERRAIN_FRACTAL_RIDGES1; break;
							}
						}

						selection = 0;
						if ( ggterrain_global_params.fractal_flags & GGTERRAIN_FRACTAL_VALLEYS2 ) selection = 1;
						else if ( ggterrain_global_params.fractal_flags & GGTERRAIN_FRACTAL_RIDGES2 ) selection = 2;
						if ( ImGui::SliderInt("##FractalRiversValleys2", &selection, 0, 2) )
						{
							switch( selection )
							{
								case 0: ggterrain_global_params.fractal_flags &= ~(GGTERRAIN_FRACTAL_VALLEYS2 | GGTERRAIN_FRACTAL_RIDGES2); break;
								case 1: ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_RIDGES2) | GGTERRAIN_FRACTAL_VALLEYS2; break;
								case 2: ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS2) | GGTERRAIN_FRACTAL_RIDGES2; break;
							}
						}

						selection = 0;
						if ( ggterrain_global_params.fractal_flags & GGTERRAIN_FRACTAL_VALLEYS3 ) selection = 1;
						else if ( ggterrain_global_params.fractal_flags & GGTERRAIN_FRACTAL_RIDGES3 ) selection = 2;
						if ( ImGui::SliderInt("##FractalRiversValleys3", &selection, 0, 2) )
						{
							switch( selection )
							{
								case 0: ggterrain_global_params.fractal_flags &= ~(GGTERRAIN_FRACTAL_VALLEYS3 | GGTERRAIN_FRACTAL_RIDGES3); break;
								case 1: ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_RIDGES3) | GGTERRAIN_FRACTAL_VALLEYS3; break;
								case 2: ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_VALLEYS3) | GGTERRAIN_FRACTAL_RIDGES3; break;
							}
						}

						// scale of the initial noise frequency
						ImGui::TextCenter("Noise Initial Frequency");
						ImGui::SliderFloat("##FractalInitFreq", &ggterrain_global_params.fractal_initial_freq, 0.01f, 32.0f, "%.3f", 2.0f);

						ImGui::TextCenter("Noise Initial Amplitude");
						ImGui::SliderFloat("##FractalInitAmplitude", &ggterrain_global_params.fractal_initial_amplitude, 0.0f, 2.0f, "%.6f", 2.0f);

						// how the noise frequency changes with increasing detail level
						ImGui::TextCenter("Noise Frequency Change");
						ImGui::SliderFloat("##FractalFreqInc", &ggterrain_global_params.fractal_freq_increase, 0.01f, 8.0f, "%.3f", 2.0f);

						// how much the noise weight changes with increasing detail level
						ImGui::TextCenter("Noise Amplitude Change");
						ImGui::SliderFloat("##FractalFreqWeight", &ggterrain_global_params.fractal_freq_weight, 0.01f, 2.0f, "%.3f", 2.0f);

						ImGui::TextCenter("Heightmap Roughness");
						ImGui::SliderFloat("##HeightmapRougness", &ggterrain_global_params.heightmap_roughness, 0.0f, 2.0f, "%.6f", 2.0f);
					}; break;

					case 1: // sculpt
					{
						ggterrain_extra_params.edit_mode = GGTERRAIN_EDIT_SCULPT;

						if (pref.iTerrainAdvanced)
						{
							ImGui::Text("Show Editable Area");
							ImGui::SameLine();
							bool showMapSize = (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE) != 0;
							if (ImGui::Checkbox("##MaskVisible", &showMapSize))
							{
								if (showMapSize) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
								else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
							}
							ImGui::SameLine();
							ImGui::Text(" 3D");
							ImGui::SameLine();
							showMapSize = (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D) != 0;
							if (ImGui::Checkbox("##MaskVisible3D", &showMapSize))
							{
								if (showMapSize) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
								else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
							}
						}
						ImGui::Text("Brush Visible");
						ImGui::SameLine();
						bool showBrush = (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE) != 0;
						if ( ImGui::Checkbox( "##BrushVisible", &showBrush ) )
						{
							if ( showBrush ) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;
							else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;
						}

						ImGui::TextCenter("Brush Size");
						ImGui::SliderFloat("##Brush Size", &ggterrain_global_render_params2.brushSize, 25.0f, 7000.0f, "%.1f", 2.0f);

						ImGui::Text("Pick Using Plane");
						ImGui::SameLine();
						bool pickPlane = ggterrain_extra_params.edit_pick_mode != 0;
						if ( ImGui::Checkbox( "##PickPlane", &pickPlane ) )
						{
							ggterrain_extra_params.edit_pick_mode = pickPlane ? 1 : 0;
						}

						ImGui::TextCenter("Sculpt Speed");
						ImGui::SliderFloat("##Sculpt Speed", &ggterrain_extra_params.sculpt_speed, 1.0f, 200.0f, "%.1f", 2.0f);

						ImGui::TextCenter("Sculpt Mode");
						ImGui::SliderInt("##Sculpt Mode", &ggterrain_extra_params.sculpt_mode, 0, 9);

						ImGui::TextCenter("Chosen Sculpt Height");
						meterValue = GGTerrain_UnitsToMeters( ggterrain_extra_params.sculpt_chosen_height );
						if ( ImGui::SliderFloat("##ChosenSculptHeight", &meterValue, -2000.0f, 5000.0f) ) 
						{
							ggterrain_extra_params.sculpt_chosen_height = GGTerrain_MetersToUnits( meterValue );
						}
					}; break;

					case 2: // paint
					{
						if (pref.iTerrainAdvanced)
						{
							ImGui::Text("Show Editable Area");
							ImGui::SameLine();
							bool showMapSize = (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE) != 0;
							if (ImGui::Checkbox("##EditBoxVisible", &showMapSize))
							{
								if (showMapSize) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
								else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
							}
							ImGui::SameLine();
							ImGui::Text(" 3D");
							ImGui::SameLine();
							showMapSize = (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D) != 0;
							if (ImGui::Checkbox("##EditBoxVisible3D", &showMapSize))
							{
								if (showMapSize) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
								else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
							}
						}
						ImGui::Text("Material Map Visible");
						ImGui::SameLine();
						bool showMask = (ggterrain_global_render_params.flags & GGTERRAIN_SHADER_FLAG_SHOW_MAT_MAP) != 0;
						if ( ImGui::Checkbox( "##MatMapVisible", &showMask ) )
						{
							if ( showMask ) ggterrain_global_render_params.flags |= GGTERRAIN_SHADER_FLAG_SHOW_MAT_MAP;
							else ggterrain_global_render_params.flags &= ~GGTERRAIN_SHADER_FLAG_SHOW_MAT_MAP;
						}

						ImGui::Text("Mask Visible");
						ImGui::SameLine();
						bool showMatMap = (ggterrain_global_render_params.flags & GGTERRAIN_SHADER_FLAG_SHOW_MASK) != 0;
						if ( ImGui::Checkbox( "##MaskVisible", &showMatMap ) )
						{
							if ( showMatMap ) ggterrain_global_render_params.flags |= GGTERRAIN_SHADER_FLAG_SHOW_MASK;
							else ggterrain_global_render_params.flags &= ~GGTERRAIN_SHADER_FLAG_SHOW_MASK;
						}

						ImGui::TextCenter("Mask Scale");
						ImGui::SliderFloat("##MaskScale", &ggterrain_global_render_params.maskScale, 0.01f, 20.0f, "%.3f", 2.0f);

						ImGui::TextCenter("Detail Scale");
						ImGui::SliderFloat("##DetailScale", &ggterrain_global_render_params2.detailScale, 0.1f, 1.0f);

						ImGui::TextCenter("Texture Tiling Fallof");
						ImGui::SliderFloat("##TextureTiling", &ggterrain_global_render_params.tilingPower, 0.15f, 1.0f);

						ImGui::Text("Brush Visible");
						ImGui::SameLine();
						bool showBrush = (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE) != 0;
						if ( ImGui::Checkbox( "##BrushVisible", &showBrush ) )
						{
							if ( showBrush ) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;
							else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;
						}

						ImGui::TextCenter("Brush Size");
						ImGui::SliderFloat("##Brush Size", &ggterrain_global_render_params2.brushSize, 25.0f, 7000.0f, "%.1f", 2.0f);

						ImGui::Text("Pick Using Plane");
						ImGui::SameLine();
						bool pickPlane = ggterrain_extra_params.edit_pick_mode != 0;
						if ( ImGui::Checkbox( "##PickPlane", &pickPlane ) )
						{
							ggterrain_extra_params.edit_pick_mode = pickPlane ? 1 : 0;
						}

						ImGui::TextCenter("Edit Mode");
						ImGui::SliderInt("##EditMode", &ggterrain_extra_params.edit_mode, 0, 6);

						if ( ggterrain_extra_params.edit_mode == GGTERRAIN_EDIT_PAINT )
						{
							ImGui::TextCenter("Paint Material");
							ImGui::SliderInt("##PaintMaterial", &ggterrain_extra_params.paint_material, 0, 32);
						}

						if ( ggterrain_extra_params.edit_mode == GGTERRAIN_EDIT_TREES )
						{
							ImGui::TextCenter("Tree LOD Distance");
							ImGui::SliderFloat("##TreeLODDist", &ggtrees_global_params.lod_dist, 750, 7000);

							ImGui::TextCenter("Tree Shadow LOD Distance");
							ImGui::SliderFloat("##TreeShadowLODDist", &ggtrees_global_params.lod_dist_shadow, 750, 7000);

							ImGui::TextCenter("Tree Shadow Range");
							ImGui::SliderInt("##TreeShadowRange", &ggtrees_global_params.tree_shadow_range, 0, 5);

							ImGui::TextCenter("Tree Paint Mode");
							ImGui::SliderInt("##TreePaintMode", &ggtrees_global_params.paint_mode, 0, 5);

							//ImGui::TextCenter("Paint Tree Type");
							//ImGui::SliderInt("##PaintTreeType", (int*)&ggtrees_global_params.paint_tree_type, 0, GGTrees_GetNumTypes()-1 );

							ImGui::TextCenter("Tree Selection");
							uint64_t values = ggtrees_global_params.paint_tree_bitfield;
							for( uint32_t i = 0; i < GGTrees_GetNumTypes(); i++ )
							{
								uint64_t mask = 1ULL << i;
								bool selected = ((values & mask) != 0);

								char str[ 32 ];
								sprintf( str, "##TreeSelection%d", i );
								if ( ImGui::Checkbox(str, &selected ) )
								{
									if ( selected ) values |= mask;
									else values &= ~mask;
								}
								if ( (i+1) % 8 != 0 && i != GGTrees_GetNumTypes()-1 ) ImGui::SameLine();
							}
							ggtrees_global_params.paint_tree_bitfield = values;
							
							ImGui::TextCenter("Tree Density");
							ImGui::SliderInt("##TreeDensity", &ggtrees_global_params.paint_density, 0, 100);

							ImGui::TextCenter("Tree Water Distance");
							if ( ImGui::SliderFloat("##TreeWaterDist", &ggtrees_global_params.water_dist, -500.0f, 2000.0f, "%.1f", 2.0f) )
							{
								ggterrain_extra_params.iUpdateTrees = 1;
							}
						}

						if ( ggterrain_extra_params.edit_mode == GGTERRAIN_EDIT_GRASS )
						{
							ImGui::TextCenter("Grass Distance");
							ImGui::SliderFloat("##GrassDist", &gggrass_global_params.lod_dist, 750, 6000);

							ImGui::TextCenter("Grass Max Height");
							if ( ImGui::SliderFloat("##GrassMaxHeight", &gggrass_global_params.max_height, -1000, 40000, "%.3f", 2.0f) )
							{
								ggterrain_extra_params.iUpdateGrass = 2;
							}

							ImGui::TextCenter("Grass Min Height");
							if ( ImGui::SliderFloat("##GrassMinHeight", &gggrass_global_params.min_height, -1000, 40000, "%.3f", 2.0f) )
							{
								ggterrain_extra_params.iUpdateGrass = 2;
							}

							ImGui::TextCenter("Grass Max Height (Underwater)");
							if ( ImGui::SliderFloat("##GrassMaxHeightWater", &gggrass_global_params.max_height_underwater, -10000, 1000, "%.3f", 2.0f) )
							{
								ggterrain_extra_params.iUpdateGrass = 2;
							}

							ImGui::TextCenter("Grass Min Height (Underwater)");
							if ( ImGui::SliderFloat("##GrassMinHeightWater", &gggrass_global_params.min_height_underwater, -10000, 1000, "%.3f", 2.0f) )
							{
								ggterrain_extra_params.iUpdateGrass = 2;
							}

							ImGui::TextCenter("Grass Paint Material (0=auto)");
							ImGui::SliderInt("##GrassPaintMat", &gggrass_global_params.paint_material, 0, 32);

							ImGui::TextCenter("Grass Selection");
							uint64_t values = gggrass_global_params.paint_type;
							for( uint32_t i = 0; i < GGGRASS_NUM_SELECTABLE_TYPES; i++ )
							{
								uint64_t mask = 1ULL << i;
								bool selected = ((values & mask) != 0);

								char str[ 32 ];
								sprintf( str, "##GrassSelection1%d", i );
								if ( ImGui::Checkbox(str, &selected ) )
								{
									if ( selected ) values |= mask;
									else values &= ~mask;
								}
								if ( (i+1) % 8 != 0 && i != GGGRASS_NUM_SELECTABLE_TYPES-1 ) ImGui::SameLine();
							}

							gggrass_global_params.paint_type = values;
							
							ImGui::TextCenter("Grass Paint Mode");
							ImGui::SliderInt("##GrassPaintMode", (int*)&gggrass_global_params.paint_mode, 0, 2 );
							
							ImGui::TextCenter("Grass Density");
							ImGui::SliderInt("##GrassDensity", &gggrass_global_params.paint_density, 0, 100);
						}

						if ( ggterrain_extra_params.edit_mode == 5 || ggterrain_extra_params.edit_mode == 6 )
						{
							ImGui::TextCenter("Flat Area Angle");
							ImGui::SliderFloat("##FlatAreaAngle", &ggterrain_extra_params.flat_area_angle, 0, 360.0f, "%.1f", 1.0f);

							if ( ImGui::Button( "Reset Flat Areas" ) )
							{
								timestampactivity(0, "GGTerrain_RemoveAllFlatAreas");
								GGTerrain_RemoveAllFlatAreas();
							}
						}

						ImGui::TextCenter("Base Matrial");

						float fSizeX = ImGui::GetContentRegionAvailWidth() - 35.0f;
						ImGui::PushItemWidth( fSizeX );

						int index = ggterrain_global_render_params.baseLayerMaterial & 0xFF;
						bool rotate = (ggterrain_global_render_params.baseLayerMaterial >> 8) != 0;
						ImGui::SliderInt( "##LayerBaseIndex", &index, 0, GGTERRAIN_MAX_SOURCE_TEXTURES-1 ); 
						ImGui::SameLine();
						ImGui::Checkbox( "##LayerBaseRotate", &rotate );
						ggterrain_global_render_params.baseLayerMaterial = (rotate ? 0x100 : 0) | index;

						char layerName[ 64 ];
						ImGui::TextCenter("Layer Matrials");
						for( int i = 0; i < 5; i++ )
						{
							int index = ggterrain_global_render_params.layerMatIndex[i] & 0xFF;
							bool rotate = (ggterrain_global_render_params.layerMatIndex[i] >> 8) != 0;
							
							sprintf_s( layerName, "##LayerIndex%d", i );
							ImGui::SliderInt( layerName, &index, 0, GGTERRAIN_MAX_SOURCE_TEXTURES-1 ); 
							ImGui::SameLine();
							sprintf_s( layerName, "##LayerRotate%d", i );
							ImGui::Checkbox( layerName, &rotate );
														
							ggterrain_global_render_params.layerMatIndex[i] = (rotate ? 0x100 : 0) | index;
						}

						ImGui::PopItemWidth();

						fSizeX = ImGui::GetContentRegionAvailWidth() / 2.0;
						ImGui::PushItemWidth( fSizeX );

						ImGui::TextCenter("Layer Height Ranges");
						for( int i = 0; i < 5; i++ )
						{
							sprintf_s( layerName, "##LayerHeightStart%d", i );
							meterValue = GGTerrain_UnitsToMeters( ggterrain_global_render_params.layerStartHeight[i] );
							if ( ImGui::SliderFloat(layerName, &meterValue, -100, 1000, "%.3f", 2.0f ) )
							{
								ggterrain_global_render_params.layerStartHeight[i] = GGTerrain_MetersToUnits( meterValue );
							}
							ImGui::SameLine();
							sprintf_s( layerName, "##LayerHeightEnd%d", i );
							meterValue = GGTerrain_UnitsToMeters( ggterrain_global_render_params.layerEndHeight[i] );
							if ( ImGui::SliderFloat(layerName, &meterValue, -100, 1000, "%.3f", 2.0f ) )
							{
								ggterrain_global_render_params.layerEndHeight[i] = GGTerrain_MetersToUnits( meterValue );
							}
							if ( ggterrain_global_render_params.layerEndHeight[i] < ggterrain_global_render_params.layerStartHeight[i] )
							{
								ggterrain_global_render_params.layerEndHeight[i] = ggterrain_global_render_params.layerStartHeight[i];
							}
						}

						ImGui::PopItemWidth();

						fSizeX = ImGui::GetContentRegionAvailWidth() - 35.0f;
						ImGui::PushItemWidth( fSizeX );
						
						ImGui::TextCenter("Slope Materials");
						for( int i = 0; i < 2; i++ )
						{
							int index = ggterrain_global_render_params.slopeMatIndex[i] & 0xFF;
							bool rotate = (ggterrain_global_render_params.slopeMatIndex[i] >> 8) != 0;

							sprintf_s( layerName, "##SlopeIndex%d", i );
							ImGui::SliderInt( layerName, &index, 0, GGTERRAIN_MAX_SOURCE_TEXTURES-1 ); 
							ImGui::SameLine();
							sprintf_s( layerName, "##SlopeRotate%d", i );
							ImGui::Checkbox( layerName, &rotate );
														
							ggterrain_global_render_params.slopeMatIndex[i] = (rotate ? 0x100 : 0) | index;
						}

						ImGui::PopItemWidth();

						ImGui::TextCenter("Slope Steepness");
						ImGui::RangeSlider("##SlopeRange0", ggterrain_global_render_params.slopeStart[0], ggterrain_global_render_params.slopeEnd[0], 1.0f);
						ImGui::RangeSlider("##SlopeRange1", ggterrain_global_render_params.slopeStart[1], ggterrain_global_render_params.slopeEnd[1], 1.0f);

						ImGui::TextCenter("Bumpiness");
						ImGui::SliderFloat("##Bumpiness", &ggterrain_global_render_params.bumpiness, 0.0f, 2.0f );

						ImGui::TextCenter("Non-Metal Reflectance");
						ImGui::SliderFloat("##Reflectance", &ggterrain_global_render_params2.reflectance, 0.0f, 0.16f );

						ImGui::TextCenter("Texture Gamma");
						ImGui::SliderFloat("##TextureGamma", &ggterrain_global_render_params2.textureGamma, 0.2f, 5.0f );
					}; break;
				}
				

				ImGui::Separator();
				ImGui::TextCenter( "Pages Active: %d", GGTerrain_GetPagesActive() );
				ImGui::TextCenter( "Pages Needed: %d", GGTerrain_GetPagesNeeded() );
				ImGui::TextCenter( "Pages Refresh Needed: %d", GGTerrain_GetPagesRefreshNeeded() );
				ImGui::TextCenter( "Camera: %.1f, %.1f, %.1f", CameraPositionX(), CameraPositionY(), CameraPositionZ() );
				//ImGui::TextCenter( "Debug Value: %d", GGTerrain_GetDebugValue() );

				ImGui::PopItemWidth();

#else

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
				// center icons
				float center_icons_numbers = 3.0f; //5
				float icon_spacer = 10.0f;
				int max_icon_size = 56;
				int control_image_size = 26; //PE: 34 - This is now the lowest possible icon size.
				float control_width = (control_image_size + 3.0) * center_icons_numbers + 6.0;
				control_width += (icon_spacer * center_icons_numbers);
				int indent = 10;
				if (w > control_width) 
				{
					//PE: fit perfectly with window width.
					control_image_size = (w - 20.0) / center_icons_numbers;
					control_image_size -= 4.0; //Padding.
					if (control_image_size > max_icon_size) control_image_size = max_icon_size;
					control_width = (control_image_size + 3.0) * center_icons_numbers + 6.0;
					control_width += (icon_spacer * (center_icons_numbers-1.0f));
					if (control_image_size == max_icon_size)
					{
						indent = (w*0.5) - (control_width*0.5);
						if (indent < 10)
							indent = 10;
					}
				}
				else
				{
					indent = (w*0.5) - (control_width*0.5);
					if (indent < 10)
						indent = 10;
				}
				int control_image_size_2 = control_image_size;
				ImGui::Indent(indent);

				//bool bHoldShift = t.inputsys.keyshift == 1;
				// LB: Use TAB instead of shift or ctrl, and trigger the toggle as though clicking the button
				bool bSwitchModeToOne = false;
				bool bSwitchModeToZero = false;
				static bool bReadyToChange = true;
				bool bPressTAB = t.inputsys.keytab == 1;
				if (!bPressTAB) bReadyToChange = true;
				if (bReadyToChange && bPressTAB)
				{
					if (current_mode != TOOL_PAINTTEXTURE && current_mode != TOOL_PAINTGRASS)
					{
						if (iTerrainRaiseMode == 0) bSwitchModeToOne = true;
						if (iTerrainRaiseMode == 1) bSwitchModeToZero = true;
					}
					else
					{
						if (iTerrainPaintMode == 0) bSwitchModeToOne = true;
						if (iTerrainPaintMode == 1) bSwitchModeToZero = true;
					}
					bReadyToChange = false; //toggle, wait until tab is released again.
				}

				// add title above edit mode
				LPSTR pEditTitle = "Sculpting Mode";
				if (current_mode == TOOL_PAINTTEXTURE) pEditTitle = "Painting Mode";
				if (current_mode == TOOL_PAINTGRASS) pEditTitle = "Paint and Erase";
				ImGui::TextCenter(pEditTitle);

				if (current_mode != TOOL_PAINTTEXTURE && current_mode != TOOL_PAINTGRASS)
				{
					//###############################
					//## PE: Display Sculpt tools. ##
					//###############################
					
					ImVec2 padding = { 3.0, 3.0 };

					ImVec4 toggle_color = ImColor(255, 255, 255, 0);
					if (current_mode == TOOL_SHAPE && iTerrainRaiseMode==1 )//(iTerrainRaiseMode == 1 && !bHoldShift) )
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (bSwitchModeToOne==true || ImGui::ImgBtn(TOOL_SHAPE_UP, ImVec2(control_image_size, control_image_size), toggle_color, ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors))
					{
						//Paint mode.
						iTerrainRaiseMode = 1;
						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "1";
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Raise Terrain");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

					toggle_color = ImColor(255, 255, 255, 0);
					if (current_mode == TOOL_SHAPE && iTerrainRaiseMode != 1 )//(iTerrainRaiseMode != 1 || bHoldShift ) )
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (bSwitchModeToZero==true || ImGui::ImgBtn(TOOL_SHAPE_DOWN, ImVec2(control_image_size, control_image_size), toggle_color, ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors))
					{
						//Remove mode.
						iTerrainRaiseMode = 0;
						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "1";
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Lower Terrain");

					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));


					CheckTutorialAction("TOOL_LEVELMODE", -10.0f); //Tutorial: check if we are waiting for this action
					if (current_mode == TOOL_LEVELMODE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_LEVELMODE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {
						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "2";
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Level Mode (This feature is not yet available)");
					
					//New line.

					CheckTutorialAction("TOOL_STOREDLEVEL", -10.0f); //Tutorial: check if we are waiting for this action
					if (current_mode == TOOL_STOREDLEVEL)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_STOREDLEVEL, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {
						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "3";
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Copy Mode (This feature is not yet available)");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

					CheckTutorialAction("TOOL_BLENDMODE", -10.0f); //Tutorial: check if we are waiting for this action
					if (current_mode == TOOL_BLENDMODE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_BLENDMODE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {

						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "4";
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Blend Mode (This feature is not yet available)");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

					CheckTutorialAction("TOOL_RAMPMODE", -10.0f); //Tutorial: check if we are waiting for this action
					if (current_mode == TOOL_RAMPMODE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_RAMPMODE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {

						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "5";
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Ramp Mode (This feature is not yet available)");

					//#########################
				}
				ImGui::Indent(-indent);

				//Setup indent for 2 icons.
				control_width = (control_image_size_2 + 3.0f) * 2.0f + 6.0f;
				indent = (w*0.5f) - (control_width*0.5f);
				if (indent < 10)
					indent = 10;

				ImGui::Indent(indent);
				if (t.terrain.terrainpaintermode >= 6)
				{
					if (iTerrainPaintMode == 1)// && !bHoldShift)
					{
						ImVec2 padding = { 3.0, 3.0 };
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size_2, control_image_size_2));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}

					if (bSwitchModeToOne == true || ImGui::ImgBtn(EBE_CONTROL1, ImVec2(control_image_size_2, control_image_size_2), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {
						//Paint mode.
						iTerrainPaintMode = 1;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Paint Mode");
					ImGui::SameLine();

					if (iTerrainPaintMode != 1)// || bHoldShift )
					{
						ImVec2 padding = { 3.0, 3.0 };
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size_2, control_image_size_2));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (bSwitchModeToZero == true || ImGui::ImgBtn(EBE_CONTROL2, ImVec2(control_image_size_2, control_image_size_2), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {
						//Remove mode.
						iTerrainPaintMode = 0;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Delete Mode");

				}
				ImGui::Indent(-indent);


				ImGui::Indent(10);
				
				//Brush Size.
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
				ImGui::PushItemWidth(-10);
				ImGui::TextCenter("Brush Size");
				if (ImGui::MaxSliderInputFloat("##Brushsize", &t.terrain.RADIUS_f, 70.0f, 500.0f, "Draw Radius"))
				{
					if (t.terrain.RADIUS_f < t.tmin) t.terrain.RADIUS_f = t.tmin;
					if (t.terrain.RADIUS_f > g.fTerrainBrushSizeMax) t.terrain.RADIUS_f = g.fTerrainBrushSizeMax;
				}
				ImGui::PopItemWidth();

				//PE: Process brush size keyboard-shortcuts.
				if (ImGui::IsKeyDown(17) && (ImGui::IsKeyDown(187) || ImGui::IsKeyDown(107) ) ) //[CTRL] + [+]
				{
					t.terrain.RADIUS_f += g.timeelapsed_f*3.0;
					if (t.terrain.RADIUS_f < t.tmin) t.terrain.RADIUS_f = t.tmin;
					if (t.terrain.RADIUS_f > g.fTerrainBrushSizeMax) t.terrain.RADIUS_f = g.fTerrainBrushSizeMax;
				}
				if (ImGui::IsKeyDown(17) && (ImGui::IsKeyDown(189) || ImGui::IsKeyDown(109))) //[CTRL] + [-]
				{
					t.terrain.RADIUS_f -= g.timeelapsed_f*3.0;
					if (t.terrain.RADIUS_f < t.tmin) t.terrain.RADIUS_f = t.tmin;
					if (t.terrain.RADIUS_f > g.fTerrainBrushSizeMax) t.terrain.RADIUS_f = g.fTerrainBrushSizeMax;
				}

				if (current_mode != TOOL_PAINTTEXTURE && current_mode != TOOL_PAINTGRASS)
				{
					control_width = (control_image_size_2 + 3.0f) * 2.0f + 6.0f;
					control_width += 10.0f; //icon spacer.
					int indent3 = (w*0.5f) - (control_width*0.5f);
					if (indent3 < 10)
						indent3 = 10;

					ImGui::TextCenter("Brush Shape");

					ImGui::Indent(-10);
					ImGui::Indent(indent3);
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 4.0f));

					if (iBrushShape == 0)
					{
						ImVec2 padding = { 3.0, 3.0 };
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(SHAPE_CIRCLE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {
						iBrushShape = 0;
						g_pTerrain->SetBrush(0);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Shape Circle");

					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

					if (iBrushShape != 0)
					{
						ImVec2 padding = { 3.0, 3.0 };
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(SHAPE_SQUARE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors))
					{
						iBrushShape = 1;
						//PE: @Lee Square brush is not working, it only have a circle function.
						g_pTerrain->SetBrush(1);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Shape Square (This feature is not yet available)");

					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

					ImGui::Indent(-indent3);
					ImGui::Indent(10);

				}
				else
				{
					//PE: Make sure we always use circle if not in "Sculpt" mode.
					g_pTerrain->SetBrush(0);
					iBrushShape = 0;
				}

				if (current_mode == TOOL_PAINTTEXTURE)
				{
					if (g_pTerrain)
					{
						ImGui::TextCenter("Texture Opacity");
						ImGui::PushItemWidth(-10);
						float fTmp = iBrushStrength;
						if (ImGui::MaxSliderInputFloat("##TerrainBrushStrength", &fTmp, 1, 255, "Goes From Transparent to Opaque",1)) {
							iBrushStrength = fTmp;
							g_pTerrain->SetPaintHardness(iBrushStrength);
						}
						ImGui::PopItemWidth();

						ImGui::TextCenter("Texture Size");
						//Range 0.0 - 10.0 1:1 = 1.0
						static int iSourceScale = 1;
						ImGui::PushItemWidth(-10);
						if (ImGui::MaxSliderInputInt("##TerrainScaleSourcesize", &iSourceScale, 1, 10, "Set Texture Size")) {
							g_pTerrain->SetScaleSource(iSourceScale);
						}
						ImGui::PopItemWidth();

					}
				}

				if (current_mode == TOOL_PAINTGRASS)
				{
					int iTextRightPos = 116; //136

					ImGui::TextCenter("Spray Strength");
					ImGui::PushItemWidth(-10);
					//PE: @Lee iBrushStrength shared with terrain paint texture strength.
					float fTmp = iBrushStrength;
					if (ImGui::MaxSliderInputFloat("##TerrainGrassBrushStrength", &fTmp, 1, 255, "Set Spray Strength (This feature is not yet available)", 1)) {
						iBrushStrength = fTmp;
						//No function yet.
					}
					ImGui::PopItemWidth();

					ImGui::TextCenter("Vegetation Height");
					ImGui::PushItemWidth(-10);
					if (ImGui::MaxSliderInputRangeFloat("##GrassVegHeight", &g_fvegRandomMin, &g_fvegRandomMax, 0.0, 100.0, "Set Vegetation Height"))
					{
						//No function needed.
					}
					ImGui::PopItemWidth();

					ImGui::TextCenter("Vegetation Density");
					ImGui::PushItemWidth(-10);
					//PE: @Lee use g_fvegDensityMin , g_fvegDensityMax.
					if (ImGui::MaxSliderInputRangeFloat("##GrassDensity", &g_fvegDensityMin, &g_fvegDensityMax, 0.0, 100.0, "Set Vegetation Density (This feature is not yet available)"))
					{
						//No function needed.
					}
					ImGui::PopItemWidth();


					//PE: Overall removed in new design.
					/*
					//PE: These is still Overall and not when spraying, so keep them as is for now.
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
					ImGui::Text("Overall Quantity:");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
					ImGui::SetCursorPos(ImVec2(iTextRightPos, ImGui::GetCursorPosY()));
					ImGui::PushItemWidth(-10);
					if (ImGui::SliderFloat("##VegOverallQuantity", &t.gamevisuals.VegQuantity_f, 0.0, 100.0, "%.0f"))
					{
						iLastUpdateVeg = Timer();
						bUpdateVeg = true;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Overall Vegetation Quantity");
					ImGui::PopItemWidth();

					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
					ImGui::Text("Overall Width:");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
					ImGui::SetCursorPos(ImVec2(iTextRightPos, ImGui::GetCursorPosY()));
					ImGui::PushItemWidth(-10);
					if (ImGui::SliderFloat("##VegOverallWidth", &t.gamevisuals.VegWidth_f, 0.0, 100.0, "%.0f"))
					{
						iLastUpdateVeg = Timer();
						bUpdateVeg = true;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Overall Vegetation Width");
					ImGui::PopItemWidth();
					*/
				}
				
				ImGui::Indent(-10);
#endif // GGTERRAIN_USE_NEW_TERRAIN
			}

			if (current_mode == TOOL_PAINTTEXTURE)
				imgui_Customize_Terrain(0);
			if (current_mode == TOOL_PAINTGRASS)
				imgui_Customize_Vegetation(0);

			if (!pref.bHideTutorials)
			{
#ifndef REMOVED_EARLYACCESS
				if (ImGui::StyleCollapsingHeader("Tutorial (this feature is incomplete)", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Indent(10);
					char* my_combo_itemsp[] = { NULL,NULL,NULL };
					int my_combo_items = 0;
					int iVideoSection = 0;
					cstr cShowTutorial = "02 - Creating terrain";
					if (current_mode == TOOL_PAINTTEXTURE) {
						my_combo_itemsp[0] = "50 - Painting terrain";
						my_combo_itemsp[1] = "02 - Creating terrain";
						my_combo_itemsp[2] = "03 - Add character and set a path";
						my_combo_items = 3;
						cShowTutorial = "50 - Painting terrain";
						iVideoSection = SECTION_PAINT_TERRAIN;
					}
					else if (current_mode == TOOL_PAINTGRASS) {
						my_combo_itemsp[0] = "01 - Getting started";
						my_combo_itemsp[1] = "02 - Creating terrain";
						my_combo_itemsp[2] = "03 - Add character and set a path";
						my_combo_items = 3;
						cShowTutorial = "02 - Creating terrain";
						iVideoSection = SECTION_ADD_VEGETATION;
					}
					else // TOOL_SHAPE,TOOL_LEVELMODE ...
					{
						my_combo_itemsp[0] = "02 - Creating terrain";
						my_combo_itemsp[1] = "01 - Getting started";
						my_combo_itemsp[2] = "03 - Add character and set a path";
						my_combo_items = 3;
						cShowTutorial = "02 - Creating terrain";
						iVideoSection = SECTION_SCULPT_TERRAIN;
					}

					SmallTutorialVideo(cShowTutorial.Get(), my_combo_itemsp, my_combo_items, iVideoSection);
					float but_gadget_size = ImGui::GetFontSize()*12.0;
					float w = ImGui::GetWindowContentRegionWidth() - 10.0;
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
					#ifdef INCLUDESTEPBYSTEP
					if (ImGui::StyleButton("View Step by Step Tutorial", ImVec2(but_gadget_size, 0)))
					{
						bHelp_Window = true;
						bHelpVideo_Window = true;
						extern bool bSetTutorialSectionLeft;
						bSetTutorialSectionLeft = false;
						strcpy(cForceTutorialName, cShowTutorial.Get());
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Start Step by Step Tutorial");
					#endif

					ImGui::Indent(-10);
				}
#endif
			}

			// insert a keyboard shortcut component into panel
			eKeyboardShortcutType KST = eKST_Sculpt;
			if (current_mode == TOOL_PAINTTEXTURE)
				KST = eKST_Paint;
			else if (current_mode == TOOL_PAINTGRASS)
				KST = eKST_AddVeg;
			UniversalKeyboardShortcut(KST);

			ImRect bbwin(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize());
			if (ImGui::IsMouseHoveringRect(bbwin.Min, bbwin.Max))
			{
				bImGuiGotFocus = true;
			}

			void CheckMinimumDockSpaceSize(float minsize);
			CheckMinimumDockSpaceSize(250.0f);

			if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) 
			{
				//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
				ImGui::Text("");
				ImGui::Text("");
			}

			ImGui::End();
		}
	}
}

void reset_terrain_paint_date( void )
{
	uint32_t size = GGTerrain_GetPaintDataSize();

	uint8_t* pNewMap = new uint8_t[size];
	if (pNewMap)
	{
		memset(pNewMap, 0, size);
		GGTerrain_SetPaintData(size, pNewMap);
		delete [] pNewMap;
		GGTerrain::GGTerrain_InvalidateRegion(-1000000.0, -1000000.0, 1000000.0, 1000000.0, GGTERRAIN_INVALIDATE_ALL);
		extern int iTriggerInvalidateAfterFrames;
		iTriggerInvalidateAfterFrames = 20;
	}
}

void clear_highlighted_tree(void)
{
	GGTrees_DeselectHighlightedTree();
}

void set_terrain_edit_mode(int mode)
{
	ggterrain_extra_params.edit_mode = mode;
	if (mode == 0) ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;
}

void set_terrain_sculpt_mode(int mode)
{
	ggterrain_extra_params.sculpt_mode = mode;
	if(mode == 0) ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;
}

int get_terrain_sculpt_mode( void )
{
	return ggterrain_extra_params.sculpt_mode;
}

int iLastTerrainSculptMode = -1;
bool bTreeGlobalInit = false;
float fTreeRandomMin = (ggtrees_global_params.paint_scale_random_low); 
float fTreeRandomMax = (ggtrees_global_params.paint_scale_random_high);

void imgui_terrain_loop_v3(void)
{
	if (!imgui_is_running)
		return;

	// terrain editing causes grass to fully update (hills raise grass)
	if (iForceUpdateVegetation == 2)
	{
		iForceUpdateVegetation = 0;
		extern bool bFullVegUpdate;
		bFullVegUpdate = true;
		bUpdateVeg = true;
	}
	if (!bTreeGlobalInit)
	{
		fTreeRandomMin = (ggtrees_global_params.paint_scale_random_low); 
		fTreeRandomMax = (ggtrees_global_params.paint_scale_random_high); 
	
		uint64_t values = gggrass_global_params.paint_type;
		for (int iL = 0; iL < GGGRASS_NUM_SELECTABLE_TYPES; iL++)
		{
			uint64_t mask = 1ULL << iL;
			if (values & mask)
				bCurrentGrassTextureForPaint[iL] = true;
			else
				bCurrentGrassTextureForPaint[iL] = false;
		}
		bTreeGlobalInit = true;
	}
	if (t.grideditselect == 0 && t.terrain.terrainpaintermode >= 0 && t.terrain.terrainpaintermode <= 12)
	{
		if (skib_terrain_frames_execute > 0)
			skib_terrain_frames_execute--;

		if (bTerrain_Tools_Window)
		{
			if (iLastTerrainSculptMode >= 0)
			{
				ggterrain_extra_params.sculpt_mode = iLastTerrainSculptMode;
				iLastTerrainSculptMode = -1;
			}
			ggterrain_extra_params.edit_mode = GGTERRAIN_EDIT_NONE;

			float media_icon_size = 40.0f;
			float plate_width = (media_icon_size + 6.0) * 4.0f;
			grideleprof_uniqui_id = 16000;
			int icon_size = 60;
			ImVec2 iToolbarIconSize = { (float)icon_size, (float)icon_size };
			ImVec2 tool_selected_padding = { 1.0, 1.0 };
			tool_selected_col = ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram];
			if (pref.current_style == 3)
				tool_selected_col = ImGui::GetStyle().Colors[ImGuiCol_Button];

			current_mode = -1;
			int top_current_mode = TOOL_SHAPE;
			if (t.terrain.terrainpaintermode >= 6)
			{
				if (t.terrain.terrainpaintermode == 11)
				{
					current_mode = TOOL_PAINTTREE;
					top_current_mode = TOOL_PAINTTREE;
					ggterrain_extra_params.edit_mode = GGTERRAIN_EDIT_TREES;
					ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;

					if (ggtrees_global_params.paint_mode == GGTREES_PAINT_ADD)
						current_mode = TOOL_TREE_ADD;
					if (ggtrees_global_params.paint_mode == GGTREES_PAINT_MOVE)
						current_mode = TOOL_TREE_MOVE;
					if (ggtrees_global_params.paint_mode == GGTREES_PAINT_REMOVE)
						current_mode = TOOL_TREE_DELETE;
					if (ggtrees_global_params.paint_mode == GGTREES_PAINT_SPRAY)
						current_mode = TOOL_TREES_ADD;
					if (ggtrees_global_params.paint_mode == GGTREES_PAINT_SPRAY_REMOVE)
						current_mode = TOOL_TREES_DELETE;
					if (ggtrees_global_params.paint_mode == GGTREES_PAINT_SCALE)
						current_mode = TOOL_TREE_SCALE;
				}
				else if (t.terrain.terrainpaintermode == 10)
				{
					current_mode = TOOL_PAINTGRASS;
					top_current_mode = TOOL_PAINTGRASS;
					ggterrain_extra_params.edit_mode = GGTERRAIN_EDIT_GRASS;
					ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;
				}
				else
				{
					current_mode = TOOL_PAINTTEXTURE;
					top_current_mode = TOOL_PAINTTEXTURE;
					ggterrain_extra_params.edit_mode = GGTERRAIN_EDIT_PAINT;
					ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;
				}
			}
			else
			{
				ggterrain_extra_params.edit_mode = GGTERRAIN_EDIT_SCULPT;
				ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;

				if(ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_RAISE || ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_LOWER)
					current_mode = TOOL_SHAPE;
				if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_LEVEL)
					current_mode = TOOL_LEVELMODE;
				if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_BLEND)
					current_mode = TOOL_BLENDMODE;
				if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_RAMP)
					current_mode = TOOL_RAMPMODE;
				if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_PICK)
					current_mode = TOOL_PICK;
				if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_WRITE)
					current_mode = TOOL_WRITE;
				if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_RANDOM)
					current_mode = TOOL_RANDOM;
				if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_RESTORE)
					current_mode = TOOL_RESTORE;
			}

			if (ggterrain_extra_params.sculpt_mode != GGTERRAIN_SCULPT_NONE)
			{
				//PE: We need to delay raise/lower object until terrain is done working on it.
				static std::vector<int> adjustedObjects;
				
				if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_RAMP && !ImGui::IsMouseDown(0) && (vLastRampTerrainPickPosition.z != 0 || vLastRampTerrainPickPosition.w != 0) )
				{
					//React on mouse release.
					float fx = vLastRampTerrainPickPosition.x;
					float fz = vLastRampTerrainPickPosition.y;
					float fx2 = vLastRampTerrainPickPosition.z;
					float fz2 = vLastRampTerrainPickPosition.w;

					//PE: Move to center of line.
					float radiusx = fx2 - fx;
					float radiusz = fz2 - fz;
					fx += radiusx * 0.5;
					fz += radiusz * 0.5;

					radiusx = abs(radiusx);
					radiusz = abs(radiusz);
					float radius = radiusx;
					if (radiusz > radius) radius = radiusz;
					radius *= 1.5;

					for (t.e = 1; t.e <= g.entityelementlist; t.e++)
					{
						t.obj = t.entityelement[t.e].obj;
						if (t.obj > 0)
						{
							t.ttdx_f = t.entityelement[t.e].x - fx;
							t.ttdz_f = t.entityelement[t.e].z - fz;
							t.ttdd_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdz_f*t.ttdz_f));
							if (t.ttdd_f <= radius)
							{
								//PE: Delay adjusting so just save for now.
								//LB: remember the distance from the old floor so can keep items on tables
								if (t.entityelement[t.e].delay_floorposy == -90000.0f)
								{
									t.entityelement[t.e].delay_floorposy = BT_GetGroundHeight (t.terrain.TerrainID, t.entityelement[t.e].x, t.entityelement[t.e].z);
									adjustedObjects.push_back(t.e);
								}
								g_iDelayActualObjectAdjustment = 40;
							}
						}
					}

					vLastRampTerrainPickPosition.z = 0;
					vLastRampTerrainPickPosition.w = 0;
					
				}
				else if (ImGui::IsMouseDown(0) && ggterrain_extra_params.sculpt_mode != GGTERRAIN_SCULPT_RAMP)
				{
					float fRadius = ggterrain_global_render_params2.brushSize;
					for (t.e = 1; t.e <= g.entityelementlist; t.e++)
					{
						t.obj = t.entityelement[t.e].obj;
						if (t.obj > 0)
						{
							t.ttdx_f = t.entityelement[t.e].x - vLastTerrainPickPosition.x;
							t.ttdz_f = t.entityelement[t.e].z - vLastTerrainPickPosition.z;
							t.ttdd_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdz_f*t.ttdz_f));
							if (t.ttdd_f <= fRadius)
							{
								//PE: Delay adjusting so just save for now.
								//LB: remember the distance from the old floor so can keep items on tables
								if (t.entityelement[t.e].delay_floorposy == -90000.0f)
								{
									t.entityelement[t.e].delay_floorposy = BT_GetGroundHeight (t.terrain.TerrainID, t.entityelement[t.e].x, t.entityelement[t.e].z);
									adjustedObjects.push_back(t.e);
								}
								g_iDelayActualObjectAdjustment = 40;
							}
						}
					}
				}
				else
				{
					if (g_iDelayActualObjectAdjustment > 0)
					{
						if (g_iDelayActualObjectAdjustment == 1)
						{
							// Execute actual object adjustments.
							undosys_multiplevents_start();
							for (t.e = 1; t.e <= g.entityelementlist; t.e++)
							{
								t.obj = t.entityelement[t.e].obj;
								if (t.obj > 0)
								{
									for (int i = 0; i < adjustedObjects.size(); i++)
									{
										if (adjustedObjects[i] == t.e)
										{
											// Can't mix undo master stack items, so move the objects using the terrain undo system.
											entity_createundoaction(eUndoSys_Object_ChangePosRotScl, t.e);
											break;
										}
									}
									t.entityelement[t.e].floorposy = t.entityelement[t.e].delay_floorposy;
									t.entityelement[t.e].delay_floorposy = -90000.0f;
								}
							}
							undosys_multiplevents_finish();
							if (g_iDelayActualObjectAdjustmentSculptCount > 0)
							{
								// connects to earlier terrain sculpts
								undosys_glue(eUndoSys_UndoList, g_iDelayActualObjectAdjustmentSculptCount); 
							}
							adjustedObjects.clear();
							g_iDelayActualObjectAdjustmentSculptCount = 0;
						}
						g_iDelayActualObjectAdjustment--;
					}
				}
			}
			cstr sWindowLabel = "Terrain Tools##Sculpt Terrain##TerrainToolsWindow";
			if (current_mode == TOOL_PAINTGRASS)
				sWindowLabel = "Terrain Tools##Add Vegetation##TerrainToolsWindow";
			if (current_mode == TOOL_PAINTTEXTURE)
				sWindowLabel = "Terrain Tools##Paint Terrain##TerrainToolsWindow";
			if (top_current_mode == TOOL_PAINTTREE)
				sWindowLabel = "Terrain Tools##Add Trees##TerrainToolsWindow";

			extern int iGenralWindowsFlags;
			ImGui::Begin(sWindowLabel.Get(), &bTerrain_Tools_Window, iGenralWindowsFlags);

			float w = ImGui::GetWindowContentRegionWidth();
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			if (ImGui::StyleCollapsingHeader("Edit Mode", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
				// center icons
				float center_icons_numbers = 4.3; //4.0f; //5
				float icon_spacer = 6.0; //10.0f;
				int max_icon_size = 56;
				int control_image_size = 26; //PE: 34 - This is now the lowest possible icon size.
				float control_width = (control_image_size + 3.0) * center_icons_numbers + 6.0;
				control_width += (icon_spacer * center_icons_numbers);
				int indent = 10;
				if (w > control_width)
				{
					//PE: fit perfectly with window width.
					control_image_size = (w - 20.0) / center_icons_numbers;
					control_image_size -= 4.0; //Padding.
					if (control_image_size > max_icon_size) control_image_size = max_icon_size;
					control_width = (control_image_size + 3.0) * center_icons_numbers + 6.0;
					control_width += (icon_spacer * (center_icons_numbers - 1.0f));
					if (control_image_size == max_icon_size)
					{
						indent = (w*0.5) - (control_width*0.5);
						if (indent < 10)
							indent = 10;
					}
				}
				else
				{
					indent = (w*0.5) - (control_width*0.5);
					if (indent < 10)
						indent = 10;
				}
				ImGui::Indent(indent);

				//PE: This is the same as current toolbar background.
				ImVec2 padding = { 3.0, 3.0 };
				extern ImVec4 drawCol_toogle; 
				ImVec4 vIconBackground = ImVec4(0,0,0,0);

				//## Terrain Tool Selection Icons Start ##	
				ImGuiWindow* window = ImGui::GetCurrentWindow();

				ImVec2 cursorRestore = ImGui::GetCursorPos() + ImVec2(0.0f, 1.0f);
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 1.0f - window->Scroll.y));

				if (window)
				{
					ImVec4 background = drawCol_toogle;
					if (pref.current_style == 25)
					{
						background = ImVec4(0.12f, 0.26f, 0.35f, 1.00f);
					}
					ImVec2 cpos = ImGui::GetCursorPos();
					window->DrawList->AddRectFilled(window->Pos+ImVec2(3.0f, cpos.y - padding.y - 3.0) , window->Pos + ImVec2(0, cpos.y+ padding.y) + ImVec2(window->Size.x, control_image_size + padding.x), ImGui::GetColorU32(background),2.0, ImDrawCornerFlags_All);
				}

				ImGui::SetCursorPos(cursorRestore);

				if (top_current_mode == TOOL_SHAPE)	window->DrawList->AddRect((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size), ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);

				if (ImGui::ImgBtn(TOOL_TERRAIN_TOOLBAR, ImVec2(control_image_size, control_image_size), vIconBackground, ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
				{
					bForceKey = true;
					csForceKey = "t";
					bForceKey2 = true;
					csForceKey2 = "1";
					ggterrain_extra_params.edit_mode = GGTERRAIN_EDIT_SCULPT;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Sculpt the editable area of the terrain");
				ImGui::SameLine();
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

				if (current_mode == TOOL_PAINTTEXTURE) window->DrawList->AddRect((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size), ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
				if (ImGui::ImgBtn(TOOL_PAINTTEXTURE, ImVec2(control_image_size, control_image_size), vIconBackground, ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
				{
					bForceKey = true;
					csForceKey = "t";
					bForceKey2 = true;
					csForceKey2 = "6";
					bTerrain_Tools_Window = true;
					ggterrain_extra_params.edit_mode = GGTERRAIN_EDIT_PAINT;
				}
				if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Paint into the editable area of the terrain");
				ImGui::SameLine();
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

				if (top_current_mode == TOOL_PAINTTREE) window->DrawList->AddRect((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size), ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
				if (ImGui::ImgBtn(TOOL_PAINTTREE, ImVec2(control_image_size, control_image_size), vIconBackground, ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
				{
					t.terrain.terrainpaintermode = 11;
					bTerrain_Tools_Window = true;
					ggterrain_extra_params.edit_mode = GGTERRAIN_EDIT_TREES;
					csForceKey = "t";
					csForceKey2 = "11";
					bForceKey = true;
					bForceKey2 = true;
				}
				if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Paint into the editable area of the terrain");
				ImGui::SameLine();
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

				if (current_mode == TOOL_PAINTGRASS) window->DrawList->AddRect((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size), ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
				if (ImGui::ImgBtn(TOOL_PAINTGRASS, ImVec2(control_image_size, control_image_size), vIconBackground, ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
				{
					bForceKey = true;
					csForceKey = "t";
					bForceKey2 = true;
					csForceKey2 = "0";
					bTerrain_Tools_Window = true;
				}
				if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Add vegetation into the editable area of the terrain");
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 3.0f));
				
				//## Terrain Tool Selection Icons End ##

				//PE: Now fit to 3 icons.
				ImGui::Indent(-indent);

				//PE: Size to fit trees and bushes.
				center_icons_numbers = 5.0f; //Added SCALE

				icon_spacer = 10.0f;
				max_icon_size = 56;
				control_image_size = 26; //PE: 34 - This is now the lowest possible icon size.
				control_width = (control_image_size + 3.0) * center_icons_numbers + 6.0;
				control_width += (icon_spacer * center_icons_numbers);
				indent = 10;
				if (w > control_width)
				{
					//PE: fit perfectly with window width.
					control_image_size = (w - 20.0) / center_icons_numbers;
					control_image_size -= 4.0; //Padding.
					if (control_image_size > max_icon_size) control_image_size = max_icon_size;
					control_width = (control_image_size + 3.0) * center_icons_numbers + 6.0;
					control_width += (icon_spacer * (center_icons_numbers - 1.0f));
					if (control_image_size == max_icon_size)
					{
						indent = (w*0.5) - (control_width*0.5);
						if (indent < 10)
							indent = 10;
					}
				}
				else
				{
					indent = (w*0.5) - (control_width*0.5);
					if (indent < 10)
						indent = 10;
				}
				int control_image_size_2 = control_image_size;
				ImGui::Indent(indent);

				// LB: Use TAB instead of shift or ctrl, and trigger the toggle as though clicking the button
				//bool bHoldShift = t.inputsys.keyshift == 1;
				bool bSwitchModeToOne = false;
				bool bSwitchModeToZero = false;
				static bool bReadyToChange = true;
				bool bPressTAB = t.inputsys.keytab == 1;
				if (!bPressTAB) bReadyToChange = true;
				if (bReadyToChange && bPressTAB)
				{
					if (current_mode != TOOL_PAINTTEXTURE && current_mode != TOOL_PAINTGRASS)
					{
						if (iTerrainRaiseMode == 0) bSwitchModeToOne = true;
						if (iTerrainRaiseMode == 1) bSwitchModeToZero = true;
					}
					else if (current_mode == TOOL_PAINTGRASS)
					{
						if (iTerrainGrassPaintMode == 0) bSwitchModeToOne = true;
						if (iTerrainGrassPaintMode == 1) bSwitchModeToZero = true;
					}
					else
					{
						if (iTerrainPaintMode == 0) bSwitchModeToOne = true;
						if (iTerrainPaintMode == 1) bSwitchModeToZero = true;
					}
					bReadyToChange = false; //toggle, wait until tab is released again.
				}

				LPSTR pEditTitle = "Sculpting Terrain";
				if (current_mode == TOOL_PAINTTEXTURE) pEditTitle = "Painting Terrain";
				if (current_mode == TOOL_PAINTGRASS) pEditTitle = "Adding Vegetation";
				if (top_current_mode == TOOL_PAINTTREE) pEditTitle = "Painting Trees";

				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 3.0f)); //Give header a little space.
				ImGui::TextCenter(pEditTitle);
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 3.0f));

				if (top_current_mode == TOOL_PAINTTREE && ggtrees_global_params.draw_enabled )
				{
					//#############################
					//## PE: Display Tree tools. ##
					//#############################

					ImVec4 back_color = ImColor(255, 255, 255, 0);
					if (current_mode == TOOL_TREE_ADD)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_TREE_ADD, ImVec2(control_image_size, control_image_size), back_color, ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
					{
						ggtrees_global_params.paint_mode = GGTREES_PAINT_ADD;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Add Tree");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

					if (current_mode == TOOL_TREE_MOVE)//(iTerrainRaiseMode != 1 || bHoldShift ) )
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_TREE_MOVE, ImVec2(control_image_size, control_image_size), back_color, ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
					{
						ggtrees_global_params.paint_mode = GGTREES_PAINT_MOVE;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Move Tree");

					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

					if (current_mode == TOOL_TREE_DELETE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_TREE_DELETE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) {
						ggtrees_global_params.paint_mode = GGTREES_PAINT_REMOVE;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Remove Tree");

					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

					if (current_mode == TOOL_TREE_SCALE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_TREE_SCALE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) {
						ggtrees_global_params.paint_mode = GGTREES_PAINT_SCALE;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Scale Tree");

					//New line.

					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((control_image_size + icon_spacer + icon_spacer), 0.0f));
					if (current_mode == TOOL_TREES_ADD)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_TREES_ADD, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) {
						ggtrees_global_params.paint_mode = GGTREES_PAINT_SPRAY;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Spray Trees");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));
					//ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((control_image_size + icon_spacer)*0.5, 0.0f));

					if (current_mode == TOOL_TREES_DELETE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_TREES_DELETE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) {

						ggtrees_global_params.paint_mode = GGTREES_PAINT_SPRAY_REMOVE;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Clear Trees");

				}

				ImGui::Indent(-indent);

				//PE: Size to fit rest.
				center_icons_numbers = 3.0f; //Added SCALE
				icon_spacer = 10.0f;
				max_icon_size = 56;
				control_image_size = 26; //PE: 34 - This is now the lowest possible icon size.
				control_width = (control_image_size + 3.0) * center_icons_numbers + 6.0;
				control_width += (icon_spacer * center_icons_numbers);
				indent = 10;
				if (w > control_width)
				{
					//PE: fit perfectly with window width.
					control_image_size = (w - 20.0) / center_icons_numbers;
					control_image_size -= 4.0; //Padding.
					if (control_image_size > max_icon_size) control_image_size = max_icon_size;
					control_width = (control_image_size + 3.0) * center_icons_numbers + 6.0;
					control_width += (icon_spacer * (center_icons_numbers - 1.0f));
					if (control_image_size == max_icon_size)
					{
						indent = (w*0.5) - (control_width*0.5);
						if (indent < 10)
							indent = 10;
					}
				}
				else
				{
					indent = (w*0.5) - (control_width*0.5);
					if (indent < 10)
						indent = 10;
				}
				control_image_size_2 = control_image_size;
				ImGui::Indent(indent);

				if (current_mode != TOOL_PAINTTEXTURE && current_mode != TOOL_PAINTGRASS && top_current_mode != TOOL_PAINTTREE && top_current_mode != TOOL_PAINTBUSH)
				{
					//###############################
					//## PE: Display Sculpt tools. ##
					//###############################

					ImVec4 toggle_color = ImColor(255, 255, 255, 0);
					if (current_mode == TOOL_SHAPE && iTerrainRaiseMode == 1)//(iTerrainRaiseMode == 1 && !bHoldShift) )
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (bSwitchModeToOne == true || ImGui::ImgBtn(TOOL_SHAPE_UP, ImVec2(control_image_size, control_image_size), toggle_color, ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
					{
						//Paint mode.
						iTerrainRaiseMode = 1;
						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "1";
						ggterrain_extra_params.sculpt_mode = GGTERRAIN_SCULPT_RAISE;
						GGTerrain::GGTerrain_CancelRamp();
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Raise Terrain");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

					toggle_color = ImColor(255, 255, 255, 0);
					if (current_mode == TOOL_SHAPE && iTerrainRaiseMode != 1)//(iTerrainRaiseMode != 1 || bHoldShift ) )
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (bSwitchModeToZero == true || ImGui::ImgBtn(TOOL_SHAPE_DOWN, ImVec2(control_image_size, control_image_size), toggle_color, ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
					{
						//Remove mode.
						iTerrainRaiseMode = 0;
						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "1";
						ggterrain_extra_params.sculpt_mode = GGTERRAIN_SCULPT_LOWER;
						GGTerrain::GGTerrain_CancelRamp();
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Lower Terrain");

					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

					CheckTutorialAction("TOOL_LEVELMODE", -10.0f); //Tutorial: check if we are waiting for this action
					if (current_mode == TOOL_LEVELMODE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_LEVELMODE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) {
						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "2";
						ggterrain_extra_params.sculpt_mode = GGTERRAIN_SCULPT_LEVEL;
						GGTerrain::GGTerrain_CancelRamp();
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Level Mode");

					//New line.
					
					CheckTutorialAction("TOOL_BLENDMODE", -10.0f); //Tutorial: check if we are waiting for this action
					if (current_mode == TOOL_BLENDMODE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_BLENDMODE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) {

						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "4";
						ggterrain_extra_params.sculpt_mode = GGTERRAIN_SCULPT_BLEND;
						GGTerrain::GGTerrain_CancelRamp();
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Blend Mode");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

					CheckTutorialAction("TOOL_RAMPMODE", -10.0f); //Tutorial: check if we are waiting for this action
					if (current_mode == TOOL_RAMPMODE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_RAMPMODE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) 
					{
						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "5";
						ggterrain_extra_params.sculpt_mode = GGTERRAIN_SCULPT_RAMP;

						// Ensure that the ramp brush size is set only once, so that after the user changes it, they don't need to keep changing it
						static bool bOnlyOnce = true;
						if (bOnlyOnce)
						{
							// If people don't like this way, we could store a brush size for ramp mode only and use that instead
							ggterrain_global_render_params2.brushSize = 75;
							bOnlyOnce = false;
						}
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Ramp Mode");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

					CheckTutorialAction("TOOL_RANDOM", -10.0f);
					if (current_mode == TOOL_RANDOM)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_RANDOM, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) 
					{
						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "5";
						ggterrain_extra_params.sculpt_mode = GGTERRAIN_SCULPT_RANDOM;
						GGTerrain::GGTerrain_CancelRamp();
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Random Mode");

					//New line.
					
					CheckTutorialAction("TOOL_PICK", -10.0f);
					if (current_mode == TOOL_PICK)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_PICK, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) {

						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "4";
						ggterrain_extra_params.sculpt_mode = GGTERRAIN_SCULPT_PICK;
						GGTerrain::GGTerrain_CancelRamp();
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Pick A Height");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

					CheckTutorialAction("TOOL_WRITE", -10.0f);
					if (current_mode == TOOL_WRITE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_WRITE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) {

						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "5";
						ggterrain_extra_params.sculpt_mode = GGTERRAIN_SCULPT_WRITE;
						GGTerrain::GGTerrain_CancelRamp();
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Paint With Chosen Height");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

					CheckTutorialAction("TOOL_RESTORE", -10.0f);
					if (current_mode == TOOL_RESTORE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_RESTORE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) {

						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "5";
						ggterrain_extra_params.sculpt_mode = GGTERRAIN_SCULPT_RESTORE;
						GGTerrain::GGTerrain_CancelRamp();
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Restore To Original Height");

					//#########################
				}
				ImGui::Indent(-indent);

				#define GRASSRESTOREWASREMOVED

				if (t.terrain.terrainpaintermode == 10)
				{
					control_width = (control_image_size_2 + 3.0f) * 2.0f + 6.0f;
					indent = (w*0.5f) - (control_width*0.5f);
					if (indent < 10)
						indent = 10;
					ImGui::Indent(indent);

					//Grass
					if (!(top_current_mode == TOOL_PAINTGRASS && !gggrass_global_params.draw_enabled))
					{
						if (iTerrainGrassPaintMode == 1)// && !bHoldShift)
						{
							ImVec2 padding = { 3.0, 3.0 };
							const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size_2, control_image_size_2));
							window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
						}

						if (bSwitchModeToOne == true || ImGui::ImgBtn(EBE_CONTROL1, ImVec2(control_image_size_2, control_image_size_2), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) 
						{
							//Paint mode.
							iTerrainGrassPaintMode = 1;
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Paint Mode");
						ImGui::SameLine();

						if (iTerrainGrassPaintMode == 0)
						{
							ImVec2 padding = { 3.0, 3.0 };
							const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size_2, control_image_size_2));
							window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
						}
						if (bSwitchModeToZero == true || ImGui::ImgBtn(EBE_CONTROL2, ImVec2(control_image_size_2, control_image_size_2), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) 
						{
							//Remove mode.
							iTerrainGrassPaintMode = 0;
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Delete Mode");
					}
					ImGui::Indent(-indent);
				}

				//Setup indent for 2 icons.
				control_width = (control_image_size_2 + 3.0f) * 2.0f + 6.0f;
				indent = (w*0.5f) - (control_width*0.5f);
				if (indent < 10)
					indent = 10;

				ImGui::Indent(indent);
				if (t.terrain.terrainpaintermode >= 6 && t.terrain.terrainpaintermode < 10)
				{
					if (iTerrainPaintMode == 1)// && !bHoldShift)
					{
						ImVec2 padding = { 3.0, 3.0 };
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size_2, control_image_size_2));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}

					if (bSwitchModeToOne == true || ImGui::ImgBtn(EBE_CONTROL1, ImVec2(control_image_size_2, control_image_size_2), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) 
					{
						//Paint mode.
						iTerrainPaintMode = 1;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Paint Mode");
					ImGui::SameLine();

					if (iTerrainPaintMode != 1)// || bHoldShift )
					{
						ImVec2 padding = { 3.0, 3.0 };
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size_2, control_image_size_2));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (bSwitchModeToZero == true || ImGui::ImgBtn(EBE_CONTROL2, ImVec2(control_image_size_2, control_image_size_2), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) 
					{
						//Remove mode.
						iTerrainPaintMode = 0;
						ggterrain_extra_params.paint_material = 0;
					}
					if (iTerrainPaintMode == 1)
					{
						ggterrain_extra_params.paint_material = iCurrentTextureForPaint + 1;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Restore Mode");
				}

				if (current_mode == TOOL_PAINTGRASS)
				{
					if(iTerrainGrassPaintMode == 0)
						gggrass_global_params.paint_mode = 1; //PE: Delete
					if (iTerrainGrassPaintMode == 1)
						gggrass_global_params.paint_mode = 0; //PE: Paint.
				}

				ImGui::Indent(-indent);

				ImGui::Indent(10);

				//Brush Size.
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
				ImGui::PushItemWidth(-10);

				if (current_mode != TOOL_PAINTTEXTURE && top_current_mode != TOOL_PAINTTREE && top_current_mode != TOOL_PAINTBUSH)
				{
					//PE: Hidden in new design but pickplane always on.
					ggterrain_extra_params.edit_pick_mode = 0; //(1) PE: Now default to pick mode 0.
				}

				//ggtrees_global_params.draw_enabled
				bool bBrushSizeEnable = true;
				if (top_current_mode == TOOL_PAINTTREE)
				{
					if (!ggtrees_global_params.draw_enabled) bBrushSizeEnable = false;
				}
				else if (top_current_mode == TOOL_PAINTBUSH)
				{
					if (!ggtrees_global_params.draw_enabled) bBrushSizeEnable = false;
				}
				else if (top_current_mode == TOOL_PAINTGRASS)
				{
					if (!gggrass_global_params.draw_enabled) bBrushSizeEnable = false;
				}

				if (bBrushSizeEnable)
				{
					ImGui::TextCenter("Brush Size");
					ImGui::MaxSliderInputFloatPower("##BrushSize", &ggterrain_global_render_params2.brushSize, 15.0f, 7000.0f, 0, 15.0f, 7500, 30, 2.0f);

					// Alter brush size with input.
					if (t.inputsys.k_s == "-" && ggterrain_global_render_params2.brushSize > 15.0f)
					{
						ggterrain_global_render_params2.brushSize -= 500 * ImGui::GetIO().DeltaTime;
					}
					if (t.inputsys.k_s == "=" && ggterrain_global_render_params2.brushSize < 7000.0f)
					{
						ggterrain_global_render_params2.brushSize += 500 * ImGui::GetIO().DeltaTime;
					}

					ImGuiIO& io = ImGui::GetIO();
					if (io.KeyCtrl && ImGui::GetIO().MouseWheel != 0)
					{
						float speed = 50.0;
						ggterrain_global_render_params2.brushSize += ImGui::GetIO().MouseWheel*speed;
						if (ggterrain_global_render_params2.brushSize > 7000.0f) ggterrain_global_render_params2.brushSize = 7000.0f;
						if (ggterrain_global_render_params2.brushSize < 15.0) ggterrain_global_render_params2.brushSize = 15.0f;
					}
				}

				static bool bGrassMatchTerrain = 0;
				if (current_mode == TOOL_PAINTTEXTURE)
				{
					bool bTmp = 1 - bGrassMatchTerrain;
					if (ImGui::Checkbox("Match Painted Grass", &bTmp))
					{
						bGrassMatchTerrain = 1 - bTmp;
						gggrass_global_params.paint_material = bGrassMatchTerrain;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("When painting, any grass will change to ensure it blends best. This option is synced with the grass tick tickbox");
				}

				if (current_mode != TOOL_PAINTTEXTURE && top_current_mode != TOOL_PAINTTREE && top_current_mode != TOOL_PAINTBUSH && top_current_mode != TOOL_PAINTGRASS)
				{

					ImGui::TextCenter("Scuplt Speed");
					ImGui::MaxSliderInputFloatPower("##Sculpt Speed", &ggterrain_extra_params.sculpt_speed, 1.0f, 200.0f, 0, 1.0f, 200.0f, 30, 2.0f);

					if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_RANDOM)
					{
						ImGui::TextCenter("Randomness Frequency");
						ImGui::SliderFloat("##RandomnessFreq", &ggterrain_extra_params.sculpt_randomness_frequency, 3.0f, 50.0f, "%.1f", 2.0f);
					}

					if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_PICK || ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_WRITE)
					{
						ImGui::TextCenter("Chosen Sculpt Height");
						float meterValue = GGTerrain_UnitsToMeters(ggterrain_extra_params.sculpt_chosen_height);
						if (ImGui::SliderFloat("##ChosenSculptHeight", &meterValue, -2000.0f, 5000.0f))
						{
							ggterrain_extra_params.sculpt_chosen_height = GGTerrain_MetersToUnits(meterValue);
						}
					}
				}

				if (top_current_mode == TOOL_PAINTGRASS && gggrass_global_params.draw_enabled)
				{
					ImGui::TextCenter("Grass Density");
					ImGui::SliderInt("##GrassDensity", &gggrass_global_params.paint_density, 0, 100);

					ImGui::TextCenter("Grass Draw Distance");
					int fTmp = gggrass_global_params.lod_dist;
					if (ImGui::SliderInt("##GrassDrawDistance", &fTmp, 750, 7000)) //PE: min distance changed to 750 so there are a range to fade, or chunks will pop in.
					{
						g.projectmodified = 1;
						gggrass_global_params.lod_dist = fTmp;
					}

					if (pref.iEnableAdvancedGrass)
					{
						int iTmp;

						if (gggrass_global_params.min_height < g.gdefaultwaterheight) gggrass_global_params.min_height = g.gdefaultwaterheight;
						if (gggrass_global_params.max_height < g.gdefaultwaterheight) gggrass_global_params.max_height = g.gdefaultwaterheight;
						if (gggrass_global_params.min_height_underwater > g.gdefaultwaterheight) gggrass_global_params.min_height_underwater = g.gdefaultwaterheight;
						if (gggrass_global_params.max_height_underwater > g.gdefaultwaterheight) gggrass_global_params.max_height_underwater = g.gdefaultwaterheight;
						if (gggrass_global_params.min_height_underwater < g.gdefaultwaterheight - 2000.0) gggrass_global_params.min_height_underwater = g.gdefaultwaterheight - 2000.0;
						if (gggrass_global_params.max_height_underwater < g.gdefaultwaterheight - 2000.0) gggrass_global_params.max_height_underwater = g.gdefaultwaterheight - 2000.0;

						ImGui::TextCenter("Grass Start/End Height");

						ImGui::TextCenter("Grass Min Height");
						if (ImGui::MaxSliderInputFloatPower("##GrassMinHeight", &gggrass_global_params.min_height, g.gdefaultwaterheight, 30000.0, "Grass Start Height", 0, 100, 30, 3.0f))
						{
							ggterrain_extra_params.iUpdateGrass = 2;
							g.projectmodified = 1;
						}

						if (ImGui::MaxSliderInputFloatPower("##GrassMaxHeight", &gggrass_global_params.max_height, g.gdefaultwaterheight, 30000.0, "Grass End Height", 0, 100, 30, 3.0f))
						{
							ggterrain_extra_params.iUpdateGrass = 2;
							g.projectmodified = 1;
						}
						if (gggrass_global_params.max_height < gggrass_global_params.min_height)
						{
							float fTmp = gggrass_global_params.max_height;
							gggrass_global_params.max_height = gggrass_global_params.min_height;
							gggrass_global_params.min_height = fTmp;
						}

						ImGui::TextCenter("Grass Start/End Height Underwater");
						if (ImGui::MaxSliderInputFloatPower("##GrassMinHeightWater", &gggrass_global_params.min_height_underwater, g.gdefaultwaterheight-2000.0, g.gdefaultwaterheight, "Grass Start Height Underwater", 0, 100, 30, 1.0f))
						{
							ggterrain_extra_params.iUpdateGrass = 2;
							g.projectmodified = 1;
						}

						if (ImGui::MaxSliderInputFloatPower("##GrassMaxHeightWater", &gggrass_global_params.max_height_underwater, g.gdefaultwaterheight-2000.0, g.gdefaultwaterheight, "Grass End Height Underwater", 0, 100, 30, 1.0f))
						{
							ggterrain_extra_params.iUpdateGrass = 2;
							g.projectmodified = 1;
						}
						if (gggrass_global_params.max_height_underwater < gggrass_global_params.min_height_underwater)
						{
							float fTmp = gggrass_global_params.max_height_underwater;
							gggrass_global_params.max_height_underwater = gggrass_global_params.min_height_underwater;
							gggrass_global_params.min_height_underwater = fTmp;
						}
					}

					bool bTmp = 1 - bGrassMatchTerrain;
					if (ImGui::Checkbox("Match Terrain Color", &bTmp))
					{
						bGrassMatchTerrain = 1 - bTmp;
						gggrass_global_params.paint_material = bGrassMatchTerrain;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("When painting grass, the style will be automatically chosen to blend with the terrain. This option is synced with the paint texture tickbox");

					extern void ControlAdvancedSetting(int&, const char*, bool* = nullptr);
					ControlAdvancedSetting(pref.iEnableAdvancedGrass, "Advanced Grass Settings");
				}

				if (top_current_mode == TOOL_PAINTTREE && ggtrees_global_params.draw_enabled)
				{

					ImGui::TextCenter("Tree Density");
					ImGui::SliderInt("##TreeDensity", &ggtrees_global_params.paint_density, 0, 100);
					ImGui::TextCenter("Tree Height");
					ImGui::PushItemWidth(-10);
					if (ImGui::MaxSliderInputRangeFloatDirect("##TreeRandomHeight", &fTreeRandomMin, &fTreeRandomMax, 0.0, 255.0, "Set Tree Random Height Interval"))
					{
						fTreeRandomMin = (int)fTreeRandomMin;
						fTreeRandomMax = (int)fTreeRandomMax;
						if (fTreeRandomMin > fTreeRandomMax)
						{
							int iTmp = fTreeRandomMax;
							fTreeRandomMax = fTreeRandomMin;
							fTreeRandomMin = iTmp;
						}
						if (fTreeRandomMin < 0) fTreeRandomMin = 0;
						if (fTreeRandomMax > 255) fTreeRandomMax = 255;
						ggtrees_global_params.paint_scale_random_low = (fTreeRandomMin); //*2.55) + 1;
						ggtrees_global_params.paint_scale_random_high = (fTreeRandomMax); //*2.55) + 1;
						if (ggtrees_global_params.paint_scale_random_high > 255) ggtrees_global_params.paint_scale_random_high = 255;
					}
					ImGui::PopItemWidth();

					ImGui::TextCenter("Tree Water Distance");
					if (ImGui::SliderFloat("##TreeWaterDist", &ggtrees_global_params.water_dist, -1000.0f, 5000.0f, "%.0f", 2.0f))
					{
						ggterrain_extra_params.iUpdateTrees = 1;
					}

					float but_gadget_size = ImGui::GetFontSize()*10.0;
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));

					if (ImGui::StyleButton("Lock Tree Visibility##TerrainTrees", ImVec2(but_gadget_size, 0)))
					{
						int iAction = askBoxCancel("This will lock the current visibility of trees so that terrain changes will no longer show or hide them, this can only be undone by clicking Randomize All Trees.\n\nAre you sure?", "Confirmation"); //1==Yes 2=Cancel 0=No
						if (iAction == 1)
						{
							GGTrees::GGTrees_LockVisibility();
						}
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Locks the current visibility of trees so that terrain changes no longer affect them, this can only be undone by clicking Randomize All Trees");
				}

				ImGui::PopItemWidth();

				//PE: Process brush size keyboard-shortcuts.
				if (ImGui::IsKeyDown(17) && (ImGui::IsKeyDown(187) || ImGui::IsKeyDown(107))) //[CTRL] + [+]
				{
					t.terrain.RADIUS_f += g.timeelapsed_f*3.0;
					if (t.terrain.RADIUS_f < t.tmin) t.terrain.RADIUS_f = t.tmin;
					if (t.terrain.RADIUS_f > g.fTerrainBrushSizeMax) t.terrain.RADIUS_f = g.fTerrainBrushSizeMax;
				}
				if (ImGui::IsKeyDown(17) && (ImGui::IsKeyDown(189) || ImGui::IsKeyDown(109))) //[CTRL] + [-]
				{
					t.terrain.RADIUS_f -= g.timeelapsed_f*3.0;
					if (t.terrain.RADIUS_f < t.tmin) t.terrain.RADIUS_f = t.tmin;
					if (t.terrain.RADIUS_f > g.fTerrainBrushSizeMax) t.terrain.RADIUS_f = g.fTerrainBrushSizeMax;
				}

				ImGui::Indent(-10);
			}

			if (current_mode == TOOL_PAINTTEXTURE)
				imgui_Customize_Terrain_v3(0);
			if (current_mode == TOOL_PAINTGRASS && gggrass_global_params.draw_enabled)
				imgui_Customize_Vegetation_v3(0);
			if (top_current_mode == TOOL_PAINTTREE && ggtrees_global_params.draw_enabled)
				imgui_Customize_Tree_v3(0);

			if (top_current_mode == TOOL_PAINTTREE && t.showeditortrees == 0)
			{
				bool bShow = t.showeditortrees;
				ImGui::Indent(10.0f);
				if (ImGui::Checkbox("Enable Trees##terraintooltrees", &bShow))
				{
					t.showeditortrees = bShow;
					ggtrees_global_params.draw_enabled = bShow;
					t.gamevisuals.bEndableTreeDrawing = t.visuals.bEndableTreeDrawing = t.showeditortrees; //PE: Also set test level.
				}
				ImGui::Indent(-10.0f);
			}
			if (top_current_mode == TOOL_PAINTGRASS && t.showeditorveg == 0)
			{
				bool bShow = t.showeditorveg;
				ImGui::Indent(10.0f);
				if (ImGui::Checkbox("Enable Grass##terraintoolgrass", &bShow))
				{
					t.showeditorveg = bShow;
					gggrass_global_params.draw_enabled = bShow;
					t.gamevisuals.bEndableGrassDrawing = t.visuals.bEndableGrassDrawing = t.showeditorveg; //PE: Need to also update test level.
				}
				ImGui::Indent(-10.0f);
			}

			if (current_mode != TOOL_PAINTTEXTURE && current_mode != TOOL_PAINTGRASS && top_current_mode != TOOL_PAINTTREE && top_current_mode != TOOL_PAINTBUSH)
			{
				imgui_Customize_Water_V2(4);
			}

			if (top_current_mode == TOOL_PAINTGRASS && gggrass_global_params.draw_enabled)
			{
				if (ImGui::StyleCollapsingHeader("Fill Whole Terrain", ImGuiTreeNodeFlags_DefaultOpen))
				{
					float but_gadget_size = ImGui::GetFontSize()*14.0;
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
					if (ImGui::StyleButton("Populate Vegetation Everywhere", ImVec2(but_gadget_size, 0)))
					{
						//PE: Crash if empty, div by zero.
						const char* msg = "This will populate vegetation everywhere with your chosen settings, are you sure?\n\n (To populate grass that matches the terrain instead, check the Match Terrain Color checkbox)";
						if( gggrass_global_params.paint_material == 0 ) msg = "This will reset grass everywhere to match the terrain, are you sure?\n\n (To populate with your chosen grass instead, uncheck the Match Terrain Color checkbox)";
						int iAction = askBoxCancel(msg, "Confirmation"); //1==Yes 2=Cancel 0=No
						if (iAction == 1)
						{
							if (gggrass_global_params.paint_type == 0)
								gggrass_global_params.paint_type = 1;

							GGGrass::GGGrass_AddAll();
						}
					}
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
					if (ImGui::StyleButton("Clear All Vegetation", ImVec2(but_gadget_size, 0)))
					{
						int iAction = askBoxCancel("This will clear all vegetation, are you sure?", "Confirmation"); //1==Yes 2=Cancel 0=No
						if (iAction == 1)
						{
							GGGrass::GGGrass_RemoveAll();
						}
					}
				}
			}

			if (!pref.bHideTutorials)
			{
				if (ImGui::StyleCollapsingHeader("Tutorial", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Indent(10);
					char* my_combo_itemsp[] = { NULL,NULL,NULL };
					int my_combo_items = 0;
					int iVideoSection = 0;
					cstr cShowTutorial = "0601 - Terrain Editing";
					my_combo_itemsp[0] = "0601 - Terrain Editing";
					my_combo_items = 1;
					cShowTutorial = "0601 - Terrain Editing";
					iVideoSection = SECTION_SCULPT_TERRAIN;

					SmallTutorialVideo(cShowTutorial.Get(), my_combo_itemsp, my_combo_items, iVideoSection);
					float but_gadget_size = ImGui::GetFontSize()*12.0;
					float w = ImGui::GetWindowContentRegionWidth() - 10.0;
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
					#ifdef INCLUDESTEPBYSTEP
					if (ImGui::StyleButton("View Step by Step Tutorial", ImVec2(but_gadget_size, 0)))
					{
						bHelp_Window = true;
						bHelpVideo_Window = true;
						extern bool bSetTutorialSectionLeft;
						bSetTutorialSectionLeft = false;
						strcpy(cForceTutorialName, cShowTutorial.Get());
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Start Step by Step Tutorial");
					#endif
					ImGui::Indent(-10);
				}
			}

			// insert a keyboard shortcut component into panel
			eKeyboardShortcutType KST = eKST_Sculpt;
			if (current_mode == TOOL_PAINTTEXTURE)
				KST = eKST_Paint;
			else if (current_mode == TOOL_PAINTGRASS)
				KST = eKST_AddVeg;
			UniversalKeyboardShortcut(KST);

			ImRect bbwin(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize());
			if (ImGui::IsMouseHoveringRect(bbwin.Min, bbwin.Max))
			{
				bImGuiGotFocus = true;
			}

			void CheckMinimumDockSpaceSize(float minsize);
			CheckMinimumDockSpaceSize(250.0f);

			if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) 
			{
				//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
				ImGui::Text("");
				ImGui::Text("");
			}

			ImGui::End();
		}
	}
}

void imgui_terrain_loop(void)
{
	if (!imgui_is_running)
		return;

	// terrain editing causes grass to fully update (hills raise grass)
	if (iForceUpdateVegetation == 2)
	{
		iForceUpdateVegetation = 0;
		extern bool bFullVegUpdate;
		bFullVegUpdate = true;
		bUpdateVeg = true;
	}
	
	if (bUpdateVeg) 
	{
		if (bEnableVeg) 
		{
			t.visuals.VegQuantity_f = t.gamevisuals.VegQuantity_f;
			t.visuals.VegWidth_f = t.gamevisuals.VegWidth_f;
			t.visuals.VegHeight_f = t.gamevisuals.VegHeight_f;
			grass_setgrassgridandfade();

			extern bool bResourcesSet, bGridMade;
			if (!(ObjectExist(t.tGrassObj) == 1 && GetMeshExist(t.tGrassObj) == 1) )
				grass_init();

			bool bOldGridMade = bGridMade;
			int iTrimUsingGrassMemblock = 0;
			if (t.game.gameisexe == 1) iTrimUsingGrassMemblock = t.terrain.grassmemblock;
			if (g.usegrassbelowwater > 0)
				MakeVegetationGridQuick(4.0f*t.visuals.VegQuantity_f, t.visuals.VegWidth_f, t.visuals.VegHeight_f, terrain_veg_areawidth, t.terrain.vegetationgridsize, t.tTerrainID, iTrimUsingGrassMemblock, true);
			else
				MakeVegetationGridQuick(4.0f*t.visuals.VegQuantity_f, t.visuals.VegWidth_f, t.visuals.VegHeight_f, terrain_veg_areawidth, t.terrain.vegetationgridsize, t.tTerrainID, iTrimUsingGrassMemblock, false);

			// small lookup for memblock painting circles
			static bool bCurveDataSet = false;
			if (!bCurveDataSet) {
				Dim(t.curve_f, 100);
				for (t.r = 0; t.r <= 180; t.r++)
				{
					t.trx_f = Cos(t.r - 90)*100.0;
					t.trz_f = Sin(t.r - 90)*100.0;
					t.curve_f[int((100 + t.trz_f) / 2)] = t.trx_f / 100.0;
				}
				bCurveDataSet = true;
			}
			t.terrain.grassregionupdate = 0; //PE: Make sure we update.
			t.terrain.grassupdateafterterrain = 1;
			t.terrain.lastgrassupdatex1 = -1; //PE: Force update.
			grass_loop();
			t.terrain.grassupdateafterterrain = 0;
			ShowVegetationGrid();
			visuals_justshaderupdate();
			iLastUpdateVeg = Timer();
		}
		else 
		{
			HideVegetationGrid();
			iLastUpdateVeg = Timer();
		}
		bUpdateVeg = false;
	}
	else 
	{
		bool bReadyToUpdateVeg = false;
		if (bVegHasChanged)
			bReadyToUpdateVeg = true;

		if (bEnableVeg && iTerrainVegLoopUpdate++ > 10) 
		{
			grass_loop();
			iTerrainVegLoopUpdate = 0;
		}

		//Continue cheking if we need to update terrain.
		if (bReadyToUpdateVeg && bEnableVeg ) 
		{
			t.visuals.VegQuantity_f = t.gamevisuals.VegQuantity_f;
			t.visuals.VegWidth_f = t.gamevisuals.VegWidth_f;
			t.visuals.VegHeight_f = t.gamevisuals.VegHeight_f;
			grass_setgrassgridandfade();

			if (!(ObjectExist(t.tGrassObj) == 1 && GetMeshExist(t.tGrassObj) == 1))
				grass_init();

			t.terrain.grassupdateafterterrain = 1;
			grass_loop();
			t.terrain.grassupdateafterterrain = 0;
			ShowVegetationGrid();

			bReadyToUpdateVeg = false;
			iLastUpdateVeg = Timer();
			bVegHasChanged = false;
		}
	}

	if (t.grideditselect == 0 && t.terrain.terrainpaintermode >= 0 && t.terrain.terrainpaintermode <= 10)
	{
		if (skib_terrain_frames_execute > 0)
			skib_terrain_frames_execute--;

		if (bTerrain_Tools_Window) {

			
			float media_icon_size = 40.0f;
			float plate_width = (media_icon_size + 6.0) * 4.0f;
			grideleprof_uniqui_id = 16000;
			int icon_size = 60;
			ImVec2 iToolbarIconSize = { (float)icon_size, (float)icon_size };
			ImVec2 tool_selected_padding = { 1.0, 1.0 };
			tool_selected_col = ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram];
			if (pref.current_style == 3)
				tool_selected_col = ImGui::GetStyle().Colors[ImGuiCol_Button];

			current_mode = -1;
			if (t.terrain.terrainpaintermode >= 6) 
			{
				if (t.terrain.terrainpaintermode == 10) 
				{
					current_mode = TOOL_PAINTGRASS;
				}
				else 
				{
					current_mode = TOOL_PAINTTEXTURE;
				}
			}
			else 
			{
				if (t.terrain.terrainpaintermode == 1)
					current_mode = TOOL_SHAPE;
				if (t.terrain.terrainpaintermode == 2)
					current_mode = TOOL_LEVELMODE;
				if (t.terrain.terrainpaintermode == 3)
					current_mode = TOOL_STOREDLEVEL;
				if (t.terrain.terrainpaintermode == 4)
					current_mode = TOOL_BLENDMODE;
				if (t.terrain.terrainpaintermode == 5)
					current_mode = TOOL_RAMPMODE;
			}

			cstr sWindowLabel = "Sculpt Terrain##TerrainToolsWindow";
			if(current_mode == TOOL_PAINTGRASS)
				sWindowLabel = "Add Vegetation##TerrainToolsWindow";
			if (current_mode == TOOL_PAINTTEXTURE)
				sWindowLabel = "Paint Terrain##TerrainToolsWindow";

			extern int iGenralWindowsFlags;
			ImGui::Begin(sWindowLabel.Get(), &bTerrain_Tools_Window, iGenralWindowsFlags);

			float w = ImGui::GetWindowContentRegionWidth();
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			if (ImGui::StyleCollapsingHeader("Edit Mode", ImGuiTreeNodeFlags_DefaultOpen)) 
			{
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

				int control_image_size = 26; //PE: 34 - This is now the lowest possible icon size.
				float control_width = (control_image_size + 3.0) * 5.0f + 6.0;
				int indent = 10;

				if (w > control_width) 
				{
					//PE: fit perfectly with window width.
					control_image_size = (w - 20.0) / 5.0;
					control_image_size -= 4.0; //Padding.
					if (control_image_size > 64) control_image_size = 64;
					control_width = (control_image_size + 3.0) * 5.0f + 6.0;
					if (control_image_size == 64)
					{
						indent = (w*0.5) - (control_width*0.5);
						if (indent < 10)
							indent = 10;
					}
					//iSliderAdjustY = control_image_size - 34
				}
				else {
					indent = (w*0.5) - (control_width*0.5);
					if (indent < 10)
						indent = 10;
				}

				ImGui::Indent(indent);

				if (current_mode != TOOL_PAINTTEXTURE && current_mode != TOOL_PAINTGRASS)
				{
					//#########################
					//PE: Display Sculpt tools.
					//#########################

					ImVec2 padding = { 3.0, 3.0 };

					//CheckTutorialAction("TOOL_SHAPE", -10.0f); //This goes to the main toolbar not here.
					if (current_mode == TOOL_SHAPE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_SHAPE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,true, bBoostIconColors)) {
						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "1";
					}
					if (ImGui::IsItemHovered() ) ImGui::SetTooltip("%s", "Shape Mode");
					ImGui::SameLine();

					CheckTutorialAction("TOOL_LEVELMODE", -10.0f); //Tutorial: check if we are waiting for this action
					if (current_mode == TOOL_LEVELMODE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_LEVELMODE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {
						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "2";
					}
					if (ImGui::IsItemHovered() ) ImGui::SetTooltip("%s", "Level Mode");
					ImGui::SameLine();

					CheckTutorialAction("TOOL_STOREDLEVEL", -10.0f); //Tutorial: check if we are waiting for this action
					if (current_mode == TOOL_STOREDLEVEL)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_STOREDLEVEL, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {
						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "3";
					}
					if (ImGui::IsItemHovered() ) ImGui::SetTooltip("%s", "Stored Level Mode");
					ImGui::SameLine();

					CheckTutorialAction("TOOL_BLENDMODE", -10.0f); //Tutorial: check if we are waiting for this action
					if (current_mode == TOOL_BLENDMODE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_BLENDMODE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {

						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "4";
					}
					if (ImGui::IsItemHovered() ) ImGui::SetTooltip("%s", "Blend Mode");
					ImGui::SameLine();

					CheckTutorialAction("TOOL_RAMPMODE", -10.0f); //Tutorial: check if we are waiting for this action
					if (current_mode == TOOL_RAMPMODE)
					{
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(TOOL_RAMPMODE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {

						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "5";
					}
					if (ImGui::IsItemHovered() ) ImGui::SetTooltip("%s", "Ramp Mode");

					//#########################
				}

				if (t.terrain.terrainpaintermode >= 6) 
				{
					if (iTerrainPaintMode == 1)
					{
						ImVec2 padding = { 3.0, 3.0 };
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}

					if (ImGui::ImgBtn(EBE_CONTROL1, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {
						//Paint mode.
						iTerrainPaintMode = 1;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Paint Mode");
					ImGui::SameLine();

					if (iTerrainPaintMode != 1)
					{
						ImVec2 padding = { 3.0, 3.0 };
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(EBE_CONTROL2, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {
						//Remove mode.
						iTerrainPaintMode = 0;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Delete Mode");

					ImGui::SameLine();
				}

				if (current_mode != TOOL_PAINTTEXTURE && current_mode != TOOL_PAINTGRASS) { //Was: current_mode == TOOL_SHAPE

					ImVec4 toggle_color = ImColor(255, 255, 255, 0);
					if (iTerrainRaiseMode == 1)
					{
						//PE: Toggle looks better when  we have the tool icons above.
						toggle_color = ImColor(128, 128, 128, 128);
					}

					if (ImGui::ImgBtn(TOOL_SHAPE_UP, ImVec2(control_image_size, control_image_size), toggle_color, ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {
						//Paint mode.
						iTerrainRaiseMode = 1;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Raise Terrain");
					ImGui::SameLine();

					toggle_color = ImColor(255, 255, 255, 0);
					if (iTerrainRaiseMode != 1)
					{
						toggle_color = ImColor(128, 128, 128, 128);
					}
					if (ImGui::ImgBtn(TOOL_SHAPE_DOWN, ImVec2(control_image_size, control_image_size), toggle_color, ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) 
					{
						//Remove mode.
						iTerrainRaiseMode = 0;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Lower Terrain");

					ImGui::SameLine();
				}

				ImVec2 cp = ImGui::GetCursorPos();
				ImGui::SetItemAllowOverlap();
				ImGui::SameLine();
				float fAdjY = control_image_size - 34 * 0.5;
				fAdjY -= 20;

				ImGui::SetCursorPos(ImVec2(cp.x, cp.y + (ImGui::GetFontSize() * 1.5) + fAdjY ));
				//				ImGui::PushItemWidth(-10);
				ImGui::PushItemWidth((control_image_size + 6.0) * 3.0);
				ImGui::SetWindowFontScale(0.5);

				//ImGuiCol_FrameBg
				ImVec4 oldFrameBg = ImGui::GetStyle().Colors[ImGuiCol_FrameBg];
				ImVec4 oldBorder = ImGui::GetStyle().Colors[ImGuiCol_Border];

				ImGui::GetStyle().Colors[ImGuiCol_FrameBg].w *= 0.25;
				ImGui::GetStyle().Colors[ImGuiCol_Border].w *= 0.25;

				if (ImGui::SliderFloat("##Brushsize", &t.terrain.RADIUS_f, 70.0f, 500.0f, "")) { //g.fTerrainBrushSizeMax
					if (t.terrain.RADIUS_f < t.tmin) t.terrain.RADIUS_f = t.tmin;
					if (t.terrain.RADIUS_f > g.fTerrainBrushSizeMax) t.terrain.RADIUS_f = g.fTerrainBrushSizeMax;
				}
				ImGui::GetStyle().Colors[ImGuiCol_FrameBg].w = oldFrameBg.w;
				ImGui::GetStyle().Colors[ImGuiCol_Border].w = oldBorder.w;
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Draw Radius %.2f", t.terrain.RADIUS_f);
				ImGui::PopItemWidth();
				ImGui::SetWindowFontScale(1.0);

				ImGui::SetCursorPos(cp);

				if (0 && t.terrain.RADIUS_f == 110.0f)
				{
					ImVec2 padding = { 3.0, 3.0 };
					const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
					window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
				}
				ImGui::SetItemAllowOverlap();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 5));
				if (ImGui::ImgBtn(TOOL_DOTCIRCLE_S, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {
					t.terrain.RADIUS_f = 110.0f;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Draw Radius Small");

				ImGui::SameLine();

				if (0 && t.terrain.RADIUS_f == 280.0f)
				{
					ImVec2 padding = { 3.0, 3.0 };
					const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
					window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
				}
				ImGui::SetItemAllowOverlap();
				if (ImGui::ImgBtn(TOOL_DOTCIRCLE_M, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {
					t.terrain.RADIUS_f = 280.0f;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Draw Radius Medium");

				ImGui::SameLine();

				if (0 && t.terrain.RADIUS_f == 450.0f)
				{
					ImVec2 padding = { 3.0, 3.0 };
					const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
					window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
				}
				ImGui::SetItemAllowOverlap();
				if (ImGui::ImgBtn(TOOL_DOTCIRCLE, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors)) {
					t.terrain.RADIUS_f = 450.0f;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Draw Radius Large");
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5));
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 8));
				ImGui::Indent(-indent);
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

				if (current_mode == TOOL_PAINTGRASS)
				{
					int iTextRightPos = 116; //136

					ImGui::PushItemWidth(-10);
					static float fval1 = 20.0, fval2 = 80.0;
					ImVec2 vOldPos = ImGui::GetCursorPos();
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
					ImGui::Text("Height:");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
					ImGui::SetCursorPos(ImVec2(iTextRightPos, ImGui::GetCursorPosY()));
					ImGui::RangeSlider("##VegHeight", g_fvegRandomMin, g_fvegRandomMax, 100.0f);
					ImGui::SetCursorPos(ImVec2(vOldPos.x, ImGui::GetCursorPosY() + 9));
					ImGui::PopItemWidth();
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Vegetation Draw Height Range");

					//PE: These is still Overall and not when spraying, so keep them as is for now.
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
					ImGui::Text("Overall Quantity:");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
					ImGui::SetCursorPos(ImVec2(iTextRightPos, ImGui::GetCursorPosY()));
					ImGui::PushItemWidth(-10);
					if (ImGui::SliderFloat("##VegOverallQuantity", &t.gamevisuals.VegQuantity_f, 0.0, 100.0,"%.0f"))
					{
						iLastUpdateVeg = Timer();
						bUpdateVeg = true;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Overall Vegetation Quantity");
					ImGui::PopItemWidth();

					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
					ImGui::Text("Overall Width:");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
					ImGui::SetCursorPos(ImVec2(iTextRightPos, ImGui::GetCursorPosY()));
					ImGui::PushItemWidth(-10);
					if (ImGui::SliderFloat("##VegOverallWidth", &t.gamevisuals.VegWidth_f, 0.0, 100.0, "%.0f"))
					{
						iLastUpdateVeg = Timer();
						bUpdateVeg = true;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Overall Vegetation Width");
					ImGui::PopItemWidth();
				}
			}

			if (current_mode == TOOL_PAINTTEXTURE)
				imgui_Customize_Terrain(0);
			if (current_mode == TOOL_PAINTGRASS)
				imgui_Customize_Vegetation(0);

			if (!pref.bHideTutorials)
			{
				#ifndef REMOVED_EARLYACCESS
				if (ImGui::StyleCollapsingHeader("Tutorial (this feature is incomplete)", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Indent(10);
					char* my_combo_itemsp[] = { NULL,NULL,NULL };
					int my_combo_items = 0;
					int iVideoSection = 0;
					cstr cShowTutorial = "02 - Creating terrain";
					if (current_mode == TOOL_PAINTTEXTURE) {
						my_combo_itemsp[0] = "50 - Painting terrain";
						my_combo_itemsp[1] = "02 - Creating terrain";
						my_combo_itemsp[2] = "03 - Add character and set a path";
						my_combo_items = 3;
						cShowTutorial = "50 - Painting terrain";
						iVideoSection = SECTION_PAINT_TERRAIN;
					}
					else if (current_mode == TOOL_PAINTGRASS) {
						my_combo_itemsp[0] = "01 - Getting started";
						my_combo_itemsp[1] = "02 - Creating terrain";
						my_combo_itemsp[2] = "03 - Add character and set a path";
						my_combo_items = 3;
						cShowTutorial = "02 - Creating terrain";
						iVideoSection = SECTION_ADD_VEGETATION;
					}
					else // TOOL_SHAPE,TOOL_LEVELMODE ...
					{
						my_combo_itemsp[0] = "02 - Creating terrain";
						my_combo_itemsp[1] = "01 - Getting started";
						my_combo_itemsp[2] = "03 - Add character and set a path";
						my_combo_items = 3;
						cShowTutorial = "02 - Creating terrain";
						iVideoSection = SECTION_SCULPT_TERRAIN;
					}

					SmallTutorialVideo(cShowTutorial.Get(), my_combo_itemsp, my_combo_items, iVideoSection);
					float but_gadget_size = ImGui::GetFontSize()*12.0;
					float w = ImGui::GetWindowContentRegionWidth()-10.0;
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
					#ifdef INCLUDESTEPBYSTEP
					if (ImGui::StyleButton("View Step by Step Tutorial", ImVec2(but_gadget_size, 0)))
					{
						bHelp_Window = true;
						bHelpVideo_Window = true;
						extern bool bSetTutorialSectionLeft;
						bSetTutorialSectionLeft = false;
						strcpy(cForceTutorialName, cShowTutorial.Get());
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Start Step by Step Tutorial");
					#endif

					ImGui::Indent(-10);
				}
				#endif
			}

			if (ImGui::StyleCollapsingHeader("Keyboard Shortcuts ???", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Indent(10);
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 4.0f));

				// context help button
				float button_gadget_size = ImGui::GetFontSize()*10.0;
				float w = ImGui::GetWindowContentRegionWidth();
				ImGui::Text("Left Mouse Button to Paint.");
				ImGui::Text("Shift + Left Mouse Button to Remove.");
				ImGui::Text("+ Increase Draw Radius.");
				ImGui::Text("- Decrease Draw Radius.");
				ImGui::Indent(-10);
			}

			ImRect bbwin(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize());
			if (ImGui::IsMouseHoveringRect(bbwin.Min, bbwin.Max))
			{
				bImGuiGotFocus = true;
			}

			void CheckMinimumDockSpaceSize(float minsize);
			CheckMinimumDockSpaceSize(250.0f);

			if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
				//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
				ImGui::Text("");
				ImGui::Text("");
			}

			ImGui::End();
		}
	}
}

#endif

void terrain_createactualterrain(void)
{
	#ifdef WICKEDENGINE
	// strangely, object can be zero when called from standalone game init
	t.terrain.terrainobjectindex = t.terrain.objectstartindex + 3;
	#endif

	/* g_pTerrain no longer used
	if ( g_pTerrain )
	{
		delete g_pTerrain;
		g_pTerrain = NULL;
	}
	*/

	#ifdef GGTERRAIN_USE_NEW_TERRAIN
	return;
	#endif

	/* g_pTerrain no longer used
	// create our temporary write location for data
	char	szPath				[ MAX_PATH ] = "levelbank\\testmap\\";
	WCHAR	szDirectory			[ MAX_PATH ] = L"";
	char	szWorkingDirectory	[ MAX_PATH ] = "";

	// get our working directory
	GetCurrentDirectory ( 256, szDirectory );

	DXUtil_ConvertWideStringToAnsi ( szWorkingDirectory, szDirectory, -1 );

	strcat_s ( szWorkingDirectory, "\\" );
	
	GG_GetRealPath ( &szPath [ 0 ], 1 );
	
	g_pTerrain = new MaxTerrain::cTerrain;

	// we will be using our terrain node naming convention!
	char pTerrainNodeFile[MAX_PATH];
	strcpy(pTerrainNodeFile, szPath);
	strcat(pTerrainNodeFile, "TTR0XR0.dat"); // any file name will do, its the path we are using!
	g_pTerrain->Load ( pTerrainNodeFile );
	g_pTerrain->SetPath ( szWorkingDirectory, szWorkingDirectory );

	// changing DDS to PNG gains almost 5000ms as no need to decompress DDS to get raw pixel data!!
	g_pTerrain->ClearMaterials(true);
	if (g_bTerrainGeneratorChooseRealTerrain == true)
		g_pTerrain->AddMaterial("base", "texturebank\\terrain grass_color (uncompressed).dds");
	else
		g_pTerrain->AddMaterial("base", "texturebank\\terrain grey grid_color (uncompressed).dds");

	g_pTerrain->SetDefaultMaterial ( "base" );

	g_pTerrain->SetLOD ( false );
	g_pTerrain->SetMaximumLOD ( 1 );
	g_pTerrain->SetNodeDivision ( 10 );
	g_pTerrain->SetMultithreading ( true );
	g_pTerrain->SetEditing ( true );

	#ifdef DEBUG
		// add any debug calls here
		g_pTerrain->SetDebugMode ( false );

		// we want a tiny world for fast testing
		g_pTerrain->SetDebugTiny ( true, 4 );
	#endif

	g_pTerrain->Build ( );
	*/
}

//PE: We need these functions for skyspec ...
void terrain_parsed_getstring(void)
{
	for (t.n = 1; t.n <= Len(t.line_s.Get()); t.n++)
	{
		if (cstr(Mid(t.line_s.Get(), t.n)) == "=")
		{
			if (cstr(Mid(t.line_s.Get(), t.n + 1)) == " ")
				t.rest_s = Right(t.line_s.Get(), (Len(t.line_s.Get()) - t.n) - 1);
			else
				t.rest_s = Right(t.line_s.Get(), Len(t.line_s.Get()) - t.n);

			t.n = Len(t.line_s.Get());
		}
	}
}

void terrain_parsed_getvalues(void)
{
	for (t.n = 1; t.n <= Len(t.line_s.Get()); t.n++)
	{
		if (cstr(Mid(t.line_s.Get(), t.n)) == "=")
		{
			t.rest_s = Right(t.line_s.Get(), Len(t.line_s.Get()) - t.n);
			t.n = Len(t.line_s.Get());
		}
	}
	t.valuei = 0;
	for (t.n = 1; t.n <= Len(t.rest_s.Get()); t.n++)
	{
		if (cstr(Mid(t.rest_s.Get(), t.n)) == "," || t.n == Len(t.rest_s.Get()))
		{
			if (t.n == Len(t.rest_s.Get()))
			{
				t.value_s = Left(t.rest_s.Get(), t.n);
			}
			else
			{
				t.value_s = Left(t.rest_s.Get(), t.n - 1);
			}
			t.value_f[t.valuei] = ValF(t.value_s.Get()); ++t.valuei;
			t.rest_s = Right(t.rest_s.Get(), Len(t.rest_s.Get()) - t.n);
			t.n = 0;
		}
	}
}


std::vector<cstr> terrain_selections;
std::vector<cstr> terrain_selections_text;
void init_terrain_selections()
{
	terrain_selections.clear();

	terrain_selections.push_back("texturebank\\terrain grassy gravel_color.dds");
	terrain_selections.push_back("texturebank\\terrain rock_color.dds");
	terrain_selections.push_back("texturebank\\terrain stony grass_color.dds");
	terrain_selections.push_back("texturebank\\terrain grass_color.dds");
	terrain_selections.push_back("texturebank\\terrain cracked mud_color.dds");
	terrain_selections.push_back("texturebank\\terrain dirt_color.dds");

	for (int i = 0; i < terrain_selections.size(); i++)
	{
		char pFileOnly[MAX_PATH];
		strcpy(pFileOnly, terrain_selections[i].Get());
		for (int n = strlen(pFileOnly) - 1; n > 0; n--)
		{
			if (pFileOnly[n] == '\\' || pFileOnly[n] == '/')
			{
				strcpy(pFileOnly, pFileOnly + n + 1);
				break;
			}
		}
		char *remove_ext = (char *)pestrcasestr(pFileOnly, "_color.dds");
		if (remove_ext)
			*remove_ext = 0;
		terrain_selections_text.push_back(pFileOnly);
	}
}

int imgui_get_selections(std::vector<cstr> selections, std::vector<cstr> selection_text, int uniqid, int imagestart, bool *winopen)
{
	if (!*winopen)
		return(-1);

	int retval = -1;
	int iPreviewIconSize = 64;

	ImGui::SetNextWindowSize(ImVec2(35 * ImGui::GetFontSize(), 33 * ImGui::GetFontSize()), ImGuiCond_Once); //ImGuiCond_FirstUseEver
	ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
	//ImGui::SetNextWindowPos( ImGui::GetCursorPos() - ImVec2(25 * ImGui::GetFontSize(), 25 * ImGui::GetFontSize() ) ,  ImGuiCond_Once);

	ImGui::Begin("Select Texture##imgui_get_selections", winopen, 0);

	ImGui::Indent(10);
	cstr label = cstr("imgui_my_selections") + cstr(uniqid);
	ImGui::Columns(4, label.Get(), false);  //false no border
	for (int i = 0; i < selections.size(); i++)
	{
		//	LoadImage(t.visuals.sGrassTextures[iL].Get(), t.terrain.imagestartindex + 180 + iL, 0, g.gdividetexturesize);

		if (!ImageExist(t.terrain.imagestartindex + imagestart + i)) {
			image_setlegacyimageloading(true);
			LoadImage(selections[i].Get(), t.terrain.imagestartindex + imagestart + i, 0, g.gdividetexturesize);
			image_setlegacyimageloading(false);
		}

		if (ImageExist(t.terrain.imagestartindex + imagestart + i)) {

			//PE: 100% transparent dont look so good.
			//if (ImGui::ImgBtn(t.terrain.imagestartindex + imagestart + i, ImVec2(iPreviewIconSize, iPreviewIconSize), ImColor(255, 255, 255, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), 0, 0, 0, 0, false, false))

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((ImGui::GetContentRegionAvail().x*0.5) - (iPreviewIconSize*0.5), 0.0f));
			if (ImGui::ImgBtn(t.terrain.imagestartindex + imagestart + i, ImVec2(iPreviewIconSize, iPreviewIconSize), ImColor(0, 0, 0, 196), ImColor(255, 255, 255, 255), ImColor(220, 220, 220, 220), ImColor(180, 180, 160, 255), 0, 0, 0, 0, false, false, true))
				//if (ImGui::ImgBtn(t.terrain.imagestartindex + imagestart + i, ImVec2(iPreviewIconSize, iPreviewIconSize), ImColor(0, 0, 0, 255)))
			{
				winopen = false;
				retval = i;
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::ImgBtn(t.terrain.imagestartindex + imagestart + i, ImVec2(300, 300), ImColor(0, 0, 0, 255));
				ImGui::EndTooltip();
			}
			ImGui::TextCenter(selection_text[i].Get());
			ImGui::NextColumn();
		}
	}

	if (ImageExist(TOOL_LOADLEVEL)) {

		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((ImGui::GetContentRegionAvail().x*0.5) - (iPreviewIconSize*0.5), 0.0f));
		if (ImGui::ImgBtn(TOOL_LOADLEVEL, ImVec2(iPreviewIconSize, iPreviewIconSize), ImColor(0, 0, 0, 196), ImColor(255, 255, 255, 255), ImColor(220, 220, 220, 220), ImColor(180, 180, 160, 255), 0, 0, 0, 0, false, false, true,false,false, bBoostIconColors))
		{
			winopen = false;
			retval = 999;
		}

		ImGui::TextCenter("Custom Texture");
		ImGui::NextColumn();
	}

	ImGui::Indent(-10);

	ImGui::Columns(1);
	ImGui::End();

	return(retval);
}

void imgui_Customize_Terrain(int mode)
{
	int wflags = ImGuiTreeNodeFlags_DefaultOpen;
	if (mode == 1) wflags = ImGuiTreeNodeFlags_None;
	if(pref.bAutoClosePropertySections && mode == 1 && iLastOpenHeader != 2)
		ImGui::SetNextItemOpen(false, ImGuiCond_Always);

	float media_icon_size = 40.0f;
	float w = ImGui::GetWindowContentRegionWidth();
	float plate_width = (media_icon_size + 6.0) * 4.0f;
#ifdef WICKEDENGINE
	if (ImGui::StyleCollapsingHeader("Palette", wflags))
#else
	if (ImGui::StyleCollapsingHeader("Customize Terrain", wflags))
#endif
	{
		if (mode == 1) iLastOpenHeader = 2;
		//Drpo down.
		ImGui::Indent(10);
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));


#ifdef WICKEDENGINE

		//Use "texturebank" as default folder.

		if (iDeleteSingleTerrainTextures > 0) {
			if (ImageExist(iDeleteSingleTerrainTextures))
				DeleteImage(iDeleteSingleTerrainTextures);
			iDeleteSingleTerrainTextures = 0;
		}
		if (iDeleteAllTerrainTextures) {
			iDeleteAllTerrainTextures = false;
			for (int iL = 0; iL < 32; iL++) {
				sTerrainTexturesID[iL] = t.terrain.imagestartindex + 80 + iL;
				sTerrainSelectionID[iL] = iL;
				bTextureNameWindow[iL] = false;
				if (ImageExist(t.terrain.imagestartindex + 80 + iL))
					DeleteImage(t.terrain.imagestartindex + 80 + iL);
			}
			bUpdateTerrainMaterials = true;
			iOldMaterial = -1; //PE: Reset selection.
		}
		if (t.visuals.sTerrainTextures[0] == "")
		{
			//Add a few default textures.
			//PE: We always need a default texture in [0].

			for( int i = 0; i < 32; i++ )
			{
				char temp[ 1024 ];
				sprintf( temp, "terraintextures\\mat%d\\Color.dds", i+1 );
				t.visuals.sTerrainTextures[i] = temp;
				sprintf( temp, "Material %d", i+1 );
				t.visuals.sTerrainTexturesName[i] = temp;
				t.gamevisuals.sTerrainTextures[i] = t.visuals.sTerrainTextures[i];
				t.gamevisuals.sTerrainTexturesName[i] = t.visuals.sTerrainTexturesName[i];
			}

			for (int iL = 0; iL < 32; iL++) {
				sTerrainSelectionID[iL] = iL;
				sTerrainTexturesID[iL] = t.terrain.imagestartindex + 80 + iL;
				bTextureNameWindow[iL] = false;
			}

			init_terrain_selections();
			//bUpdateTerrainMaterials = true;
			//PE: We now activate if not exists, when selecting, so no need to reset all textures on a fresh palette.
			bUpdateTerrainMaterials = false;
		}
		int iUsedImages = 0;
		cStr sTextureFolder = g.rootdir_s + "texturebank\\";
		float col_start = 100.0f;
		int iSelectTexture = -1;

#ifdef DISPLAY4x4
		ImGui::Indent(-10);
		ImGui::Indent(4);
		ImGui::Columns(4, "##terrain4x4columns", false);  //false no border
#endif

		for (int iL = 0; iL < 32; iL++)
		{

			if (t.visuals.sTerrainTextures[iL] != "")
			{
				if (!ImageExist(sTerrainTexturesID[iL]))
				{
					//Load in image.
					image_setlegacyimageloading(true);
					SetMipmapNum(1); //PE: mipmaps not needed.
					//t.terrain.imagestartindex = 63600
					if (ImageExist(sTerrainTexturesID[iL]) == 1) DeleteImage(sTerrainTexturesID[iL]);

					// sTerrainTextures stores as (uncompressed) to speed up material loading as raw data, but 
					// here they can be compressed for quicker loading, so..
					char pCompressedVersionIfAny[MAX_PATH];
					strcpy(pCompressedVersionIfAny, t.visuals.sTerrainTextures[iL].Get());
					int iLen = strlen(pCompressedVersionIfAny) - strlen(" (uncompressed).dds");
					//PE: Make sure we dont crash if using small filenames like "a.dds".
					if (iLen > 0)
					{
						pCompressedVersionIfAny[iLen] = 0;
						strcat(pCompressedVersionIfAny, ".dds");
						if (FileExist(pCompressedVersionIfAny) == 0)
						{
							strcpy(pCompressedVersionIfAny, t.visuals.sTerrainTextures[iL].Get());
						}
					}
					LoadImage(pCompressedVersionIfAny, sTerrainTexturesID[iL], 0, g.gdividetexturesize);
					//LoadImageSize(pCompressedVersionIfAny, sTerrainTexturesID[iL], 512,512); //Takes to long.
					if (ImageExist(sTerrainTexturesID[iL]) == 1) 
					{
						//sTerrainTexturesID[iL] = t.terrain.imagestartindex + 80 + iL;
					}
					else {
						//Load failed, clear texture slot.
						t.visuals.sTerrainTextures[iL] = "";
						t.gamevisuals.sTerrainTextures[iL] = t.visuals.sTerrainTextures[iL];
						g.projectmodified = 1;
						bUpdateTerrainMaterials = true;
					}
					SetMipmapNum(-1);
					image_setlegacyimageloading(false);
				}

				if (ImageExist(sTerrainTexturesID[iL]))
				{

					if (bTextureNameWindow[iL]) {
						//Ask for a proper name of texture.

						ImGui::SetNextWindowSize(ImVec2(26 * ImGui::GetFontSize(), 32 * ImGui::GetFontSize()), ImGuiCond_Once);
						ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
						cstr sUniqueWinName = cstr("Terrain Texture Name##ttn") + cstr(iL);
						ImGui::Begin(sUniqueWinName.Get(), &bTextureNameWindow[iL], 0);
						ImGui::Indent(10);
						static char NewTextureName[256];
						cstr sUniqueInputName = cstr("##InoutTerrainName") + cstr(iL);
						float content_width = ImGui::GetContentRegionAvailWidth() - 10.0;
						ImGui::ImgBtn(sTerrainTexturesID[iL], ImVec2(content_width, content_width), ImColor(0, 0, 0, 255));
						ImGui::PushItemWidth(-10);
						ImGui::Text("Enter a name for your terrain texture:");
						if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
							ImGui::SetKeyboardFocusHere(0);
						if (ImGui::InputText(sUniqueInputName.Get(), t.visuals.sTerrainTexturesName[iL].Get(), 250, ImGuiInputTextFlags_EnterReturnsTrue)) {
							t.gamevisuals.sTerrainTexturesName[iL] = t.visuals.sTerrainTexturesName[iL];
							bTextureNameWindow[iL] = false;
						}
						#ifdef WICKEDENGINE
						if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;
						#endif

						ImGui::PopItemWidth();
						ImGui::Indent(-10);
						if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
							//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
							ImGui::Text("");
							ImGui::Text("");
						}
						bImGuiGotFocus = true;
						ImGui::End();
					}



					iUsedImages++;

#ifndef DISPLAY4x4
					if (iUsedImages < 32 && iL == 0)
					{
						float but_gadget_size = ImGui::GetFontSize()*10.0;
						ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));

						if (ImGui::StyleButton("Add Texture", ImVec2(but_gadget_size, 0))) {
							iSelectTexture = 99;
						}
						ImGui::Separator();
					}

					char materialname[256];
					strcpy(materialname, t.visuals.sTerrainTextures[iL].Get());
#endif
					float path_gadget_size = ImGui::GetFontSize()*3.0;
					int preview_icon_size = ImGui::GetFontSize();

					bool bDeleteImage = true;
					if (iL == 0)
						bDeleteImage = false;

					//TEXTURE.

					cStr sLabel = cStr("Texture##InputTerrainTexture") + cStr(iL);
					if (!bDeleteImage)
						sLabel = cStr("Default##InputTerrainTexture") + cStr(iL);

					//PE: New do 4x4 coloums here.
#ifdef DISPLAY4x4
					int iLargerPreviewIconSize = 28;//PE: 54 , now lowest possible icon
					float control_width = (iLargerPreviewIconSize + 3.0) * 4.0f + 6.0;

					if (w > control_width) {
						//PE: fit perfectly with window width.
						iLargerPreviewIconSize = (w - 20.0) / 4.0;
						iLargerPreviewIconSize -= 6.0; //Padding.
						if (iLargerPreviewIconSize > 70) iLargerPreviewIconSize = 70;
					}

					ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					if (sTerrainTexturesID[iL] > 0)
					{

						if (iL == 0) {
							CheckTutorialAction("TOOL_TERRAIN_SAND", -20.0f); //Tutorial: check if we are waiting for this action
						}
						if (iL == 2) {
							CheckTutorialAction("TOOL_TERRAIN_ROCK", -20.0f); //Tutorial: check if we are waiting for this action
						}

						cStr sLabelChild = cStr("##terrain4x4") + cStr(iL);

						ImVec2 content_avail = { iLargerPreviewIconSize + 1.0f ,iLargerPreviewIconSize + 1.0f };

						//style.WindowPadding
						ImVec2 oldstyle = ImGui::GetStyle().FramePadding;
						ImGui::GetStyle().FramePadding = { 0,0 };
						ImGui::BeginChild(sLabelChild.Get(), content_avail, false, ImGuiWindowFlags_NoScrollbar);

						iLargerPreviewIconSize &= 0xfffe;

						//iLargerPreviewIconSize = (iLargerPreviewIconSize / 8) * 8;
						//if (iLargerPreviewIconSize < 8) iLargerPreviewIconSize = 8;

						ImGui::SetBlurMode(true);
						if (ImGui::ImgBtn(sTerrainTexturesID[iL], ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize), ImColor(0, 0, 0, 255), ImColor(220,220,220,220), ImColor(255,255,255,255), ImColor(180,180,160,255), -1,
										   0, 0, 0, false, false, false, false, false, true))
						{
							//iSelectTexture = iL; //PE: Just select image.
							if (iL == 0 && bTutorialCheckAction) TutorialNextAction();
							if (iL == 2 && bTutorialCheckAction) TutorialNextAction();
							iCurrentTextureForPaint = sTerrainSelectionID[iL];

						}
						ImGui::SetBlurMode(false);

						// Our buttons are both drag sources and drag targets here!
						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							ImGui::SetDragDropPayload("DND_TERRAIN_TEXTURES", &iL, sizeof(int));
							ImGui::Text("Swap Texture");
							ImGui::EndDragDropSource();
						}
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TERRAIN_TEXTURES"))
							{
								IM_ASSERT(payload->DataSize == sizeof(int));
								int payload_n = *(const int*)payload->Data;

								if (payload_n >= 0 && payload_n < 32)
								{
									//PE: Swap iL and payload_n
									cStr sTmp = t.visuals.sTerrainTextures[iL];
									cStr sTmp2 = t.visuals.sTerrainTexturesName[iL];
									int iTmp = sTerrainTexturesID[iL];
									int iTmp2 = sTerrainSelectionID[iL];

									t.visuals.sTerrainTextures[iL] = t.visuals.sTerrainTextures[payload_n];
									t.visuals.sTerrainTexturesName[iL] = t.visuals.sTerrainTexturesName[payload_n];
									sTerrainTexturesID[iL] = sTerrainTexturesID[payload_n];
									sTerrainSelectionID[iL] = sTerrainSelectionID[payload_n];

									t.visuals.sTerrainTextures[payload_n] = sTmp;
									t.visuals.sTerrainTexturesName[payload_n] = sTmp2;
									sTerrainTexturesID[payload_n] = iTmp;
									sTerrainSelectionID[payload_n] = iTmp2;

									t.gamevisuals.sTerrainTextures[iL] = t.visuals.sTerrainTextures[iL];
									t.gamevisuals.sTerrainTexturesName[iL] = t.visuals.sTerrainTexturesName[iL];
									t.gamevisuals.sTerrainTextures[payload_n] = t.visuals.sTerrainTextures[payload_n];
									t.gamevisuals.sTerrainTexturesName[payload_n] = t.visuals.sTerrainTexturesName[payload_n];
								}
							}
							ImGui::EndDragDropTarget();
						}

						bool bInContext = false;
						static int iCurrentTerrainContext = -1;
						/*
						if (iCurrentTerrainContext == -1 || iCurrentTerrainContext == iL)
						{
							if (ImGui::BeginPopupContextWindow())
							{
								iCurrentTerrainContext = iL;
								bInContext = true;
								if (ImGui::MenuItem("Change Texture"))
								{
									iSelectTexture = iL;
									iCurrentTerrainContext = -1;
								}
								if (ImGui::MenuItem("Change Texture Name"))
								{
									bTextureNameWindow[iL] = true;
									iCurrentTerrainContext = -1;
								}
								if (ImGui::MenuItem("Select Texture"))
								{
									if (iL == 0 && bTutorialCheckAction) TutorialNextAction();
									if (iL == 2 && bTutorialCheckAction) TutorialNextAction();

									iCurrentTextureForPaint = sTerrainSelectionID[iL]; //iL;
								}
								
								if (iL != 0)
								{
									if (ImGui::MenuItem("Delete Texture"))
									{
										t.visuals.sTerrainTextures[iL] = ""; //delete image in next run.
										t.gamevisuals.sTerrainTextures[iL] = t.visuals.sTerrainTextures[iL];
										g.projectmodified = 1;
										bUpdateTerrainMaterials = true;
										if (iCurrentTextureForPaint == iL)
											iCurrentTextureForPaint--;

										iCurrentTerrainContext = -1;
									}
								}
								ImGui::EndPopup();
							}
							else {
								iCurrentTerrainContext = -1;
							}
						}
						*/
						if (!bInContext && ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::ImgBtn(sTerrainTexturesID[iL], ImVec2(350, 350), ImColor(0, 0, 0, 255));
							ImGui::TextCenter(t.visuals.sTerrainTexturesName[iL].Get());
							ImGui::Separator();
							ImGui::EndTooltip();
						}

						ImGui::EndChild();
						ImGui::GetStyle().FramePadding = oldstyle;
					}

					if (iCurrentTextureForPaint == sTerrainSelectionID[iL]) //sTerrainTexturesID[iL] - t.terrain.imagestartindex - 80)
					{
						ImVec2 padding = { 2.0, 2.0 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}

//					sLabel = cStr("##tradio") + cStr(iL);
//					float checkwidth = ImGui::GetFontSize()*1.5;
//					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((ImGui::GetContentRegionAvail().x*0.5) - (checkwidth*0.5), 0.0f));

					if (iOldMaterial != iCurrentTextureForPaint)
					{
						/* g_pTerrain no longer used
						if (g_pTerrain)
						{
							cstr sTextureName = cStr(iCurrentTextureForPaint);
							//Only add material when user select it, save mem and faster loading.
							if (g_pTerrain->FindMaterial(sTextureName.Get()) == nullptr)
							{
								//Not found try to load it.
								g_pTerrain->AddMaterial(sTextureName.Get(), t.visuals.sTerrainTextures[sTerrainSelectionID[iCurrentTextureForPaint]].Get());

							}
							if (g_pTerrain->FindMaterial(sTextureName.Get()) != nullptr)
							{
								g_pTerrain->SetDefaultMaterial("base");

								g_pTerrain->SetMaterial(sTextureName.Get());
							}
							iOldMaterial = iCurrentTextureForPaint;
						}
						*/
					}
//					if (ImGui::RadioButton(sLabel.Get(), &iCurrentTextureForPaint, iL)) {
//						if (iL == 0 && bTutorialCheckAction) TutorialNextAction();
//					}

					ImGui::NextColumn();

#else
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

					if (iL == 0) {
						CheckTutorialAction("TOOL_TERRAIN_SAND", -20.0f); //Tutorial: check if we are waiting for this action
					}

					if (ImGui::RadioButton(sLabel.Get(), &iCurrentTextureForPaint, iL)) {
						if (iL == 0 && bTutorialCheckAction) TutorialNextAction();
					}
					if (iOldMaterial != iCurrentTextureForPaint)
					{
						//@Michael + 1 should be removed when your ready, this is just to follow your current test code :)
						if (g_pTerrain) 
						{
							if (g_pTerrain->FindMaterial(cStr(iCurrentTextureForPaint).Get()) != nullptr)
							{
								g_pTerrain->SetMaterial(cStr(iCurrentTextureForPaint).Get());
							}

							iOldMaterial = iCurrentTextureForPaint;
						}
					}

					ImGui::SameLine();

					int iSmallPreviewYMargin = preview_icon_size / 2;
					int iLargerPreviewIconSize = preview_icon_size * 2;
					ImGui::SetCursorPos(ImVec2(col_start-10.0f , ImGui::GetCursorPosY() - iSmallPreviewYMargin));
//					ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY() - 3));

					if (sTerrainTexturesID[iL] > 0)
					{
						if (ImGui::ImgBtn(sTerrainTexturesID[iL], ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize), ImColor(0, 0, 0, 255)))
						{
							iSelectTexture = iL;
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::ImgBtn(sTerrainTexturesID[iL], ImVec2(180, 180), ImColor(0, 0, 0, 255));
							ImGui::EndTooltip();
						}
						ImGui::SameLine();
					}

					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + iSmallPreviewYMargin));

					int iInputFlags = ImGuiInputTextFlags_EnterReturnsTrue;
					if (!bDeleteImage)
						iInputFlags = ImGuiInputTextFlags_ReadOnly;

					sLabel = cStr("##InputTerrainTexture") + cStr(iL);
					ImGui::PushItemWidth(-10 - path_gadget_size);
					if (ImGui::InputText(sLabel.Get(), &materialname[0], 250, iInputFlags))
					{
						if (strlen(materialname) == 0)
							t.visuals.sTerrainTextures[iL] = ""; //delete image in next run.
						t.gamevisuals.sTerrainTextures[iL] = t.visuals.sTerrainTextures[iL];
						g.projectmodified = 1;
						bUpdateTerrainMaterials = true;
						if (iCurrentTextureForPaint == iL)
							iCurrentTextureForPaint--;

					}
					ImGui::PopItemWidth();

					ImGui::SameLine();
					ImGui::PushItemWidth(path_gadget_size);
					sLabel = cStr("...##InputTerrainTexture") + cStr(iL);

					if (ImGui::StyleButton(sLabel.Get()))
						iSelectTexture = iL;

					if (bDeleteImage) {
						ImGui::SameLine();
						sLabel = cStr("X##InputTerrainTexture") + cStr(iL);
						if (ImGui::StyleButton(sLabel.Get())) {
							t.visuals.sTerrainTextures[iL] = ""; //delete image in next run.
							t.gamevisuals.sTerrainTextures[iL] = t.visuals.sTerrainTextures[iL];
							g.projectmodified = 1;
							bUpdateTerrainMaterials = true;
							if (iCurrentTextureForPaint == iL)
								iCurrentTextureForPaint--;
						}
					}

					ImGui::PopItemWidth();

					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - (iSmallPreviewYMargin*2.0)));
#endif

				}
			}
			else {
				//Delete old image.
				if (ImageExist(sTerrainTexturesID[iL]) == 1) DeleteImage(sTerrainTexturesID[iL]);
				//sTerrainTexturesID[iL] = 0;
			}

		}

		// Removed for early access
		/*
#ifdef DISPLAY4x4
		ImGui::Columns(1);
		ImGui::Indent(-4);
		ImGui::Indent(10);
		if (iUsedImages < 32 )
		{
			//ImGui::Text(""); //ImGui::Separator();
			float but_gadget_size = ImGui::GetFontSize()*10.0;
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));

			if (ImGui::StyleButton("Add New Texture", ImVec2(but_gadget_size, 0))) {
				iSelectTexture = 99;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Add New Texture");

			//ImGui::Separator();
		}
#endif
		*/


		#define REMOVETERRAINTEXTUREDIALOG
		static bool terrain_selection_window = false;
		static int iSelectedTerrainTexture = -1;
		if (iSelectTexture >= 0)
		{
			iSelectedTerrainTexture = iSelectTexture;
			#ifndef REMOVETERRAINTEXTUREDIALOG
			terrain_selection_window = true;
			iSelectTexture = 0;
			#else
			iSelectTexture = 999;
			#endif
		}

		#ifndef REMOVETERRAINTEXTUREDIALOG
		iSelectTexture = imgui_get_selections(terrain_selections, terrain_selections_text, 2, 380, &terrain_selection_window);
		#endif
		if (iSelectTexture >= 0 && iSelectedTerrainTexture >= 0)
		{
			bool bNeedFileSelector = false;
			if (iSelectTexture == 999)
				bNeedFileSelector = true;

			terrain_selection_window = false;

			int iNewTerrainIndex = iSelectTexture;
			iSelectTexture = iSelectedTerrainTexture;

			if (iSelectTexture == 99) {
				//Find free slot.
				for (int iL2 = 0; iL2 < 32; iL2++) {
					if (t.visuals.sTerrainTextures[iL2] == "") {
						iSelectTexture = iL2;
						break;
					}
				}
			}

			if (!bNeedFileSelector) {
				t.visuals.sTerrainTextures[iSelectTexture] = terrain_selections[iNewTerrainIndex];
				if (ImageExist(sTerrainTexturesID[iSelectTexture]))
					iDeleteSingleTerrainTextures = sTerrainTexturesID[iSelectTexture];
				iCurrentTextureForPaint = iSelectTexture;
				bUpdateTerrainMaterials = true;
			}

			if (bNeedFileSelector)
			{

				//iSelectTexture
				cStr tOldDir = GetDir();
				char * cFileSelected;
				cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "All\0*.*\0DDS\0*.dds\0PNG\0*.png\0JPEG\0*.jpg\0TGA\0*.tga\0BMP\0*.bmp\0\0\0", sTextureFolder.Get(), NULL);
				SetDir(tOldDir.Get());

				if (cFileSelected && strlen(cFileSelected) > 0)
				{
					char *relonly = (char *)pestrcasestr(cFileSelected, g.rootdir_s.Get());

					t.visuals.sTerrainTextures[iSelectTexture] = cFileSelected;
					t.visuals.sTerrainTexturesName[iSelectTexture] = ""; //PE: User must enter a proper name.
					bTextureNameWindow[iSelectTexture] = true;
					if (relonly) {
						t.visuals.sTerrainTextures[iSelectTexture] = cFileSelected + g.rootdir_s.Len();
					}
					g.projectmodified = 1;
					t.gamevisuals.sTerrainTextures[iSelectTexture] = t.visuals.sTerrainTextures[iSelectTexture];
					t.gamevisuals.sTerrainTexturesName[iSelectTexture] = t.visuals.sTerrainTexturesName[iSelectTexture];
					bUpdateTerrainMaterials = true;
					iCurrentTextureForPaint = iSelectTexture;

					//PE: Reload image.
					if (ImageExist(sTerrainTexturesID[iSelectTexture]))
						iDeleteSingleTerrainTextures = sTerrainTexturesID[iSelectTexture];
				}
			}
			iSelectTexture = -1;
		}

		if (bUpdateTerrainMaterials)
		{
			/* g_pTerrain no longer used
			if (g_pTerrain)
			{
				// THIS TAKES 4 SECONDS DURING THE LAUNCH (OUCH)
				// I would move this to a thread and have it done in the background
				// though Paul may redo the whole terrain code with the updated design so not need at this stage to optimize this

				// this is going to clear all current materials in the list, ideally it should not do this
				g_pTerrain->ClearMaterials ( false );
				for (int iL = 0; iL < 32; iL++)
				{
					if (t.visuals.sTerrainTextures[iL] != "") 
					{
						//PE: When reload , selection id must fit.
						sTerrainSelectionID[iL] = iL;

						// add a material, given this is already in use for the palette we should share
						// the pixel data as this function is going to make its own copy and it will slow
						// things down, not good
						g_pTerrain->AddMaterial( cStr(iL).Get(), t.visuals.sTerrainTextures[iL].Get ( ));
					}
				}

				// due to materials being cleared aside from the base we must reset it
				g_pTerrain->SetDefaultMaterial ( "base" );

				// set the new material
				g_pTerrain->SetMaterial ( cStr ( iCurrentTextureForPaint ).Get ( ) );
			}
			*/
			bUpdateTerrainMaterials = false;
		}

#else
		ImGui::PushItemWidth(-10);
		char * current_terrain = NULL;
		if (t.visuals.terrainindex < g.terrainstylemax) current_terrain = t.terrainstylebank_s[t.visuals.terrainindex].Get();
		if (!current_terrain) current_terrain = "NA";

		if (ImGui::BeginCombo("##SelectTerrainCombo", current_terrain)) // The second parameter is the label previewed before opening the combo.
		{
			for (int terrainindex = 1; terrainindex <= g.terrainstylemax; terrainindex++)
			{
				if (t.terrainstylebank_s[terrainindex].Len() > 0 && t.terrainstylebank_s[terrainindex] != "custom")
				{
					bool is_selected = false;
					if (t.terrainstylebank_s[terrainindex].Get() == current_terrain)
						is_selected = true;
					if (ImGui::Selectable(t.terrainstylebank_s[terrainindex].Get(), is_selected))
					{
						//Change Terrain.
						bool bNewTextureValid = true;

						#ifdef ENABLECUSTOMTERRAIN
						if (t.terrainstylebank_s[terrainindex] == "CUSTOM")
						{
							if (FileExist(cstr(g.mysystem.levelBankTestMap_s + TEXTURE_D_NAME).Get()) == 0)
							{
								extern bool bTriggerMessage;
								extern char cTriggerMessage[MAX_PATH];
								bNewTextureValid = false;
								strcpy(cTriggerMessage, "Custom Terrain Not Found.");
								bTriggerMessage = true;
							}
						}
						#endif
						if (bNewTextureValid) 
						{
							g.projectmodified = 1;
							current_terrain = t.terrainstylebank_s[terrainindex].Get();

							//Get old so we can delete later. (they take up some mem).
							cstr sOldTerrainTextureLocation = terrain_getterrainfolder();

							g.terrainstyleindex = terrainindex;
							t.visuals.terrainindex = g.terrainstyleindex;
							if (g.terrainstyleindex > g.terrainstylemax)  g.terrainstyleindex = g.terrainstylemax;
							t.visuals.terrain_s = t.terrainstylebank_s[g.terrainstyleindex];
							t.gamevisuals.terrain_s = t.visuals.terrain_s;
							t.gamevisuals.terrainindex = t.visuals.terrainindex; //for save fpm
							terrain_changestyle();

							//Load new terrain textures
							int iTex = 5; //Terrain plate.

							//PE: First Delete old internal image.
							deleteinternaltexture(cstr(sOldTerrainTextureLocation + "\\" + TEXTURE_D_NAME).Get());

							cstr sTerrainTextureLocation = terrain_getterrainfolder();
							//PE: Delete current if we had been switched to custom.
							//deleteinternaltexture(cstr(sTerrainTextureLocation + "\\" + TEXTURE_D_NAME).Get());
							removeinternalimage(terrainbuild.iTexturePanelImg[iTex]);

							terrainbuild.iTexturePanelImg[iTex] = loadinternalimage(cstr(sTerrainTextureLocation + "\\" + TEXTURE_D_NAME).Get());
							if (terrainbuild.iTexturePanelImg[iTex] == 0)
							{
								terrainbuild.iTexturePanelImg[iTex] = loadinternalimage(cstr(sTerrainTextureLocation + "\\texture_D.dds").Get());
								if (terrainbuild.iTexturePanelImg[iTex] == 0)
								{
									// means the terrain texture is missing, report this and switch to default to avoid crash
									terrain_initstyles_reset();
									grass_initstyles_reset();
									sTerrainTextureLocation = terrain_getterrainfolder();
									terrainbuild.iTexturePanelImg[iTex] = loadinternalimage(cstr(sTerrainTextureLocation + "\\" + TEXTURE_D_NAME).Get());
								}
							}
							terrainbuild.terrainstyle = g.terrainstyle_s;
							terrainbuild.iTexPlateImage = terrainbuild.iTexturePanelImg[iTex];

							//if (t.terrainstylebank_s[terrainindex] == "CUSTOM") {
							terrain_generatesupertexture(false);
							//}

							visuals_justshaderupdate();
							// if change sky, regenerate env map
							t.visuals.refreshskysettingsfromlua = true;
							cubemap_generateglobalenvmap();
							t.visuals.refreshskysettingsfromlua = false;

						}
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::Indent(-10);


		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (plate_width*0.5), 0.0f));
		int indent = (w*0.5) - (plate_width*0.5);
		if (indent < 10)
			indent = 10;
		ImGui::Indent(indent);


		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 4.0f));

		if (terrainbuild.iTexPlateImage <= 0) {
			ImGui::Text("");
		}
		int n = 0;
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{

				if (terrainbuild.iTexPlateImage > 0) {

					ImGuiWindow* window = ImGui::GetCurrentWindow();

					ImVec2 padding = { 2.0, 2.0 };
					ImVec4 bg_col = { 0.0, 0.0, 0.0, 1.0 };
					const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(media_icon_size, media_icon_size));
					window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, ImGui::GetColorU32(bg_col), 0.0f, 15);

					if (current_mode == TOOL_PAINTTEXTURE && terrainbuild.iCurrentTexture == n) {
						ImVec2 padding = { 4.0, 4.0 };
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(media_icon_size, media_icon_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}

					if (n == 0) {
						CheckTutorialAction("TOOL_TERRAIN_SAND", -20.0f); //Tutorial: check if we are waiting for this action
					}
					ImGui::PushID(terrainbuild.iTexPlateImage + n + 200000);
					if (ImGui::ImgBtn(terrainbuild.iTexPlateImage, ImVec2(media_icon_size, media_icon_size), ImVec4(0.0, 0.0, 0.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, n + 1, 4, 4, false, false, false, true)) {

						if (n == 0 && bTutorialCheckAction) TutorialNextAction();

						terrainbuild.iCurrentTexture = n;
						//Change to selected terrainpaintermode
						if (current_mode != TOOL_PAINTTEXTURE) {

							if (terrainbuild.iCurrentTexture < 8)
								t.terrain.terrainpaintermode = 6;
							else if (terrainbuild.iCurrentTexture < 12)
								t.terrain.terrainpaintermode = 7;
							else if (terrainbuild.iCurrentTexture < 15)
								t.terrain.terrainpaintermode = 8;
							else
								t.terrain.terrainpaintermode = 9;

						}
					}
					if (skib_terrain_frames_execute == 0 && ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
						terrainbuild.iCurrentTexture = n;
						if (current_mode != TOOL_PAINTTEXTURE) {
							if (terrainbuild.iCurrentTexture < 8)
								t.terrain.terrainpaintermode = 6;
							else if (terrainbuild.iCurrentTexture < 12)
								t.terrain.terrainpaintermode = 7;
							else if (terrainbuild.iCurrentTexture < 15)
								t.terrain.terrainpaintermode = 8;
							else
								t.terrain.terrainpaintermode = 9;
						}
						delay_terrain_execute = 1; //@Lee remove this line if you dont want to support changing textures.
					}

					ImGui::PopID();
					if (x != 3)
						ImGui::SameLine();

					if (delay_terrain_execute == 0 && ImGui::IsItemHovered()) {
						ImVec2 cursor_pos = ImGui::GetIO().MousePos;
						ImVec2 tooltip_offset(10.0f, ImGui::GetFontSize()*1.5);
						ImVec2 tooltip_position = cursor_pos;
						if (tooltip_position.x + 210 > GetDesktopWidth())
							tooltip_position.x -= 210;
						tooltip_position.x += tooltip_offset.x;
						tooltip_position.y += tooltip_offset.y;
						ImGui::SetNextWindowPos(tooltip_position);
						ImGui::SetNextWindowContentWidth(204.0f);
						ImGui::BeginTooltip();
						float icon_ratio;
						int icon_size = 204;
						ImGui::ImgBtn(terrainbuild.iTexPlateImage, ImVec2(icon_size, icon_size), ImVec4(0.0, 0.0, 0.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, n + 1, 4, 4, false, false, false, true);
						//char hchar[MAX_PATH];
						//ImGui::Text("%s", hchar);
						ImGui::EndTooltip();

					}

				}
				n++;
			}
		}

		ImGui::Indent(-indent);
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

#endif
#ifdef WICKEDENGINE
		ImGui::Indent(-10);
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
#endif

	}
}


void imgui_Customize_Terrain_v3(int mode)
{
	int wflags = ImGuiTreeNodeFlags_DefaultOpen;
	if (mode == 1) wflags = ImGuiTreeNodeFlags_None;
	//if (pref.bAutoClosePropertySections && mode == 1 && iLastOpenHeader != 2)
		//ImGui::SetNextItemOpen(false, ImGuiCond_Always);

	float media_icon_size = 40.0f;
	float w = ImGui::GetWindowContentRegionWidth();
	float plate_width = (media_icon_size + 6.0) * 4.0f;
	if (ImGui::StyleCollapsingHeader("Palette", wflags))
	{
		//if (mode == 1) iLastOpenHeader = 2;
		//Drpo down.
		ImGui::Indent(10);
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));


		//Use "texturebank" as default folder.

		if (iDeleteSingleTerrainTextures > 0) {
			if (ImageExist(iDeleteSingleTerrainTextures))
				DeleteImage(iDeleteSingleTerrainTextures);
			iDeleteSingleTerrainTextures = 0;
		}
		if (iDeleteAllTerrainTextures) {
			iDeleteAllTerrainTextures = false;
			for (int iL = 0; iL < 32; iL++) {
				sTerrainTexturesID[iL] = t.terrain.imagestartindex + 80 + iL;
				sTerrainSelectionID[iL] = iL;
				bTextureNameWindow[iL] = false;
				if (ImageExist(t.terrain.imagestartindex + 80 + iL))
					DeleteImage(t.terrain.imagestartindex + 80 + iL);
			}
			bUpdateTerrainMaterials = true;
			iOldMaterial = -1; //PE: Reset selection.
		}

		//PE: Saved textures in visual.ini is not really possible at the monent, so reset every time. t.visuals.sTerrainTextures[0] == ""
		//terraintextures\mat1\Color.dds
		if (t.visuals.customTexturesFolder.Len() == 0 && (t.visuals.sTerrainTextures[0] != "terraintextures\\mat1\\Color.dds" || t.visuals.sTerrainTexturesName[0] != "Rainforest Overgrowth" ))
		{
			//Add a few default textures.
			//PE: We always need a default texture in [0].

			for( int i = 0; i < 32; i++ )
			{
				char temp[ 1024 ];
				sprintf( temp, "terraintextures\\mat%d\\Color.dds", i+1 );
				t.visuals.sTerrainTextures[i] = temp;
				sprintf( temp, "Material %d", i+1 );
				t.visuals.sTerrainTexturesName[i] = temp;
				t.gamevisuals.sTerrainTextures[i] = t.visuals.sTerrainTextures[i];
				t.gamevisuals.sTerrainTexturesName[i] = t.visuals.sTerrainTexturesName[i];
				
				//PE: Texture name List from Mark.
				if (i + 1 == 1) t.visuals.sTerrainTexturesName[i] = "Rainforest Overgrowth";
				if (i + 1 == 2) t.visuals.sTerrainTexturesName[i] = "Wet Shoreline Sand";
				if (i + 1 == 3) t.visuals.sTerrainTexturesName[i] = "Grassy Clover";
				if (i + 1 == 4) t.visuals.sTerrainTexturesName[i] = "Dry Beach Sand";
				if (i + 1 == 5) t.visuals.sTerrainTexturesName[i] = "Cliff Rock with Weeds";
				if (i + 1 == 6) t.visuals.sTerrainTexturesName[i] = "Desert Gravel Path";
				if (i + 1 == 7) t.visuals.sTerrainTexturesName[i] = "Desert Jagged Rock";
				if (i + 1 == 8) t.visuals.sTerrainTexturesName[i] = "Desert Sand Flat";
				if (i + 1 == 9) t.visuals.sTerrainTexturesName[i] = "Melting Snow";
				if (i + 1 == 10) t.visuals.sTerrainTexturesName[i] = "Shoreline Mud";
				if (i + 1 == 11) t.visuals.sTerrainTexturesName[i] = "Snowy Rock";
				if (i + 1 == 12) t.visuals.sTerrainTexturesName[i] = "Trampled Snow";
				if (i + 1 == 13) t.visuals.sTerrainTexturesName[i] = "Virgin Snow";
				if (i + 1 == 14) t.visuals.sTerrainTexturesName[i] = "Cracked Ground";
				if (i + 1 == 15) t.visuals.sTerrainTexturesName[i] = "Canyon Gravel Path";
				if (i + 1 == 16) t.visuals.sTerrainTexturesName[i] = "Patchy Grass";
				if (i + 1 == 17) t.visuals.sTerrainTexturesName[i] = "Canyon Cliff Wall";
				if (i + 1 == 18) t.visuals.sTerrainTexturesName[i] = "Chunky Rock";
				if (i + 1 == 19) t.visuals.sTerrainTexturesName[i] = "Cliff Rock";
				if (i + 1 == 20) t.visuals.sTerrainTexturesName[i] = "Moss";
				if (i + 1 == 21) t.visuals.sTerrainTexturesName[i] = "Mossy Rock";
				if (i + 1 == 22) t.visuals.sTerrainTexturesName[i] = "Riverbed Stones";
				if (i + 1 == 23) t.visuals.sTerrainTexturesName[i] = "Rocky Mountain Ground";
				if (i + 1 == 24) t.visuals.sTerrainTexturesName[i] = "Ferns";
				if (i + 1 == 25) t.visuals.sTerrainTexturesName[i] = "Grass Dense";
				if (i + 1 == 26) t.visuals.sTerrainTexturesName[i] = "Rocky Soil";
				if (i + 1 == 27) t.visuals.sTerrainTexturesName[i] = "Woodland Cliff Rock";
				if (i + 1 == 28) t.visuals.sTerrainTexturesName[i] = "Dirt Path";
				if (i + 1 == 29) t.visuals.sTerrainTexturesName[i] = "Field Grass";
				if (i + 1 == 30) t.visuals.sTerrainTexturesName[i] = "Forest Ground";
				if (i + 1 == 31) t.visuals.sTerrainTexturesName[i] = "Wet Riverbed Gravel";
				if (i + 1 == 32) t.visuals.sTerrainTexturesName[i] = "Square Pattern";
			}

			for (int iL = 0; iL < 32; iL++) {
				sTerrainSelectionID[iL] = iL;
				sTerrainTexturesID[iL] = t.terrain.imagestartindex + 80 + iL;
				bTextureNameWindow[iL] = false;
			}

			init_terrain_selections();
			//bUpdateTerrainMaterials = true;
			//PE: We now activate if not exists, when selecting, so no need to reset all textures on a fresh palette.
			bUpdateTerrainMaterials = false;
		}
		int iUsedImages = 0;
		cStr sTextureFolder = g.rootdir_s + "texturebank\\";
		float col_start = 100.0f;
		int iSelectTexture = -1;

#ifdef DISPLAY4x4
		ImGui::Indent(-10);
		//ImGui::Indent(4);

		static float fLast4x4Height = 228.0;
		static float fLastMaxY = 0.0;
		if (fLastMaxY > 100.0 && fLastMaxY < 400.0)
		{
			fLast4x4Height = fLastMaxY;
		}

		ImVec2 oldstylemain = ImGui::GetStyle().FramePadding;
		ImVec2 oldwinstylemain = ImGui::GetStyle().WindowPadding;
		ImGui::GetStyle().WindowPadding = { 0,0 };
		ImGui::GetStyle().FramePadding = { 0,0 };
		ImVec2 vWindowPos = ImGui::GetWindowPos();
		ImVec2 vWindowSize = ImGui::GetWindowSize();

		ImGui::BeginChild("##terrain4x4scrollbar", ImVec2(0, fLast4x4Height), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

		w = ImGui::GetWindowContentRegionWidth(); //PE: Minus scrollbar.
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0, 2.0));

		ImGui::Columns(4, "##terrain4x4columns", false);  //false no border
		//ImGui::Indent(4);

#endif

		for (int iL = 0; iL < 32; iL++)
		{
			if (t.visuals.sTerrainTextures[iL] != "")
			{
				if (!ImageExist(sTerrainTexturesID[iL]))
				{
					//PE: This loop leaks, somewhere. even if all images has been deleted.
					//PE: We dont really need this , so now we dont reload on new level, load level. (iDeleteAllTerrainTextures=false).

					//Load in image.
					image_setlegacyimageloading(true);
					SetMipmapNum(1); //PE: mipmaps not needed.
					//t.terrain.imagestartindex = 63600
					if (ImageExist(sTerrainTexturesID[iL]) == 1) DeleteImage(sTerrainTexturesID[iL]);

					// sTerrainTextures stores as (uncompressed) to speed up material loading as raw data, but 
					// here they can be compressed for quicker loading, so..
					char pCompressedVersionIfAny[MAX_PATH];
					strcpy(pCompressedVersionIfAny, t.visuals.sTerrainTextures[iL].Get());
					int iLen = strlen(pCompressedVersionIfAny) - strlen(" (uncompressed).dds");
					//PE: Make sure we dont crash if using small filenames like "a.dds".
					if (iLen > 0)
					{
						pCompressedVersionIfAny[iLen] = 0;
						strcat(pCompressedVersionIfAny, ".dds");
						if (FileExist(pCompressedVersionIfAny) == 0)
						{
							strcpy(pCompressedVersionIfAny, t.visuals.sTerrainTextures[iL].Get());
						}
					}
					LoadImage(pCompressedVersionIfAny, sTerrainTexturesID[iL], 0, g.gdividetexturesize);
					//LoadImageSize(pCompressedVersionIfAny, sTerrainTexturesID[iL], 512,512); //Takes to long.
					if (ImageExist(sTerrainTexturesID[iL]) == 1)
					{
						//sTerrainTexturesID[iL] = t.terrain.imagestartindex + 80 + iL;
					}
					else {
						//Load failed, clear texture slot.
						t.visuals.sTerrainTextures[iL] = "";
						t.gamevisuals.sTerrainTextures[iL] = t.visuals.sTerrainTextures[iL];
						g.projectmodified = 1;
						bUpdateTerrainMaterials = true;
					}
					SetMipmapNum(-1);
					image_setlegacyimageloading(false);
				}

				if (ImageExist(sTerrainTexturesID[iL]))
				{

					if (bTextureNameWindow[iL]) {
						//Ask for a proper name of texture.

						ImGui::SetNextWindowSize(ImVec2(26 * ImGui::GetFontSize(), 32 * ImGui::GetFontSize()), ImGuiCond_Once);
						ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
						cstr sUniqueWinName = cstr("Terrain Texture Name##ttn") + cstr(iL);
						ImGui::Begin(sUniqueWinName.Get(), &bTextureNameWindow[iL], 0);
						ImGui::Indent(10);
						static char NewTextureName[256];
						cstr sUniqueInputName = cstr("##InoutTerrainName") + cstr(iL);
						float content_width = ImGui::GetContentRegionAvailWidth() - 10.0;
						ImGui::ImgBtn(sTerrainTexturesID[iL], ImVec2(content_width, content_width), ImColor(0, 0, 0, 255));
						ImGui::PushItemWidth(-10);
						ImGui::Text("Enter a name for your terrain texture:");
						if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
							ImGui::SetKeyboardFocusHere(0);
						if (ImGui::InputText(sUniqueInputName.Get(), t.visuals.sTerrainTexturesName[iL].Get(), 250, ImGuiInputTextFlags_EnterReturnsTrue)) {
							t.gamevisuals.sTerrainTexturesName[iL] = t.visuals.sTerrainTexturesName[iL];
							bTextureNameWindow[iL] = false;
						}

						if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;

						ImGui::PopItemWidth();
						ImGui::Indent(-10);
						if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
							//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
							ImGui::Text("");
							ImGui::Text("");
						}
						bImGuiGotFocus = true;
						ImGui::End();
					}



					iUsedImages++;

#ifndef DISPLAY4x4
					if (iUsedImages < 32 && iL == 0)
					{
						float but_gadget_size = ImGui::GetFontSize()*10.0;
						ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));

						if (ImGui::StyleButton("Add Texture", ImVec2(but_gadget_size, 0))) {
							iSelectTexture = 99;
						}
						ImGui::Separator();
					}

					char materialname[256];
					strcpy(materialname, t.visuals.sTerrainTextures[iL].Get());
#endif
					float path_gadget_size = ImGui::GetFontSize()*3.0;
					int preview_icon_size = ImGui::GetFontSize();

					bool bDeleteImage = true;
					if (iL == 0)
						bDeleteImage = false;

					//TEXTURE.

					cStr sLabel = cStr("Texture##InputTerrainTexture") + cStr(iL);
					if (!bDeleteImage)
						sLabel = cStr("Default##InputTerrainTexture") + cStr(iL);

					//PE: New do 4x4 coloums here.
#ifdef DISPLAY4x4
					int iLargerPreviewIconSize = 28;//PE: 54 , now lowest possible icon
					float control_width = (iLargerPreviewIconSize + 3.0) * 4.0f + 6.0;

					if (w > control_width) {
						//PE: fit perfectly with window width.
						iLargerPreviewIconSize = (w - 20.0) / 4.0;
						iLargerPreviewIconSize -= 6.0; //Padding.
						if (iLargerPreviewIconSize > 70) iLargerPreviewIconSize = 70;
					}

					ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					if (sTerrainTexturesID[iL] > 0)
					{

						if (iL == 0) {
							CheckTutorialAction("TOOL_TERRAIN_SAND", -20.0f); //Tutorial: check if we are waiting for this action
						}
						if (iL == 2) {
							CheckTutorialAction("TOOL_TERRAIN_ROCK", -20.0f); //Tutorial: check if we are waiting for this action
						}

						cStr sLabelChild = cStr("##terrain4x4") + cStr(iL);

						ImVec2 content_avail = { iLargerPreviewIconSize + 1.0f ,iLargerPreviewIconSize + 1.0f };

						//style.WindowPadding
						ImVec2 oldstyle = ImGui::GetStyle().FramePadding;
						ImGui::GetStyle().FramePadding = { 1,1 };
						ImGui::BeginChild(sLabelChild.Get(), content_avail, false, ImGuiWindowFlags_NoScrollbar);

						iLargerPreviewIconSize &= 0xfffe;

						//iLargerPreviewIconSize = (iLargerPreviewIconSize / 8) * 8;
						//if (iLargerPreviewIconSize < 8) iLargerPreviewIconSize = 8;

						ImGui::SetBlurMode(true);
						if (ImGui::ImgBtn(sTerrainTexturesID[iL], ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize), ImColor(0, 0, 0, 255)))
						{
							//iSelectTexture = iL; //PE: Just select image.
							if (iL == 0 && bTutorialCheckAction) TutorialNextAction();
							if (iL == 2 && bTutorialCheckAction) TutorialNextAction();
							iCurrentTextureForPaint = sTerrainSelectionID[iL];
							ggterrain_extra_params.paint_material = iCurrentTextureForPaint + 1; //iL + 1;
							iTerrainPaintMode = 1;
						}
						ImGui::SetBlurMode(false);

						// Our buttons are both drag sources and drag targets here!
						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							ImGui::SetDragDropPayload("DND_TERRAIN_TEXTURES", &iL, sizeof(int));
							ImGui::Text("Swap Texture");
							ImGui::EndDragDropSource();
						}
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TERRAIN_TEXTURES"))
							{
								IM_ASSERT(payload->DataSize == sizeof(int));
								int payload_n = *(const int*)payload->Data;

								if (payload_n >= 0 && payload_n < 32)
								{
									//PE: Swap iL and payload_n
									cStr sTmp = t.visuals.sTerrainTextures[iL];
									cStr sTmp2 = t.visuals.sTerrainTexturesName[iL];
									int iTmp = sTerrainTexturesID[iL];
									int iTmp2 = sTerrainSelectionID[iL];

									t.visuals.sTerrainTextures[iL] = t.visuals.sTerrainTextures[payload_n];
									t.visuals.sTerrainTexturesName[iL] = t.visuals.sTerrainTexturesName[payload_n];
									sTerrainTexturesID[iL] = sTerrainTexturesID[payload_n];
									sTerrainSelectionID[iL] = sTerrainSelectionID[payload_n];

									t.visuals.sTerrainTextures[payload_n] = sTmp;
									t.visuals.sTerrainTexturesName[payload_n] = sTmp2;
									sTerrainTexturesID[payload_n] = iTmp;
									sTerrainSelectionID[payload_n] = iTmp2;

									t.gamevisuals.sTerrainTextures[iL] = t.visuals.sTerrainTextures[iL];
									t.gamevisuals.sTerrainTexturesName[iL] = t.visuals.sTerrainTexturesName[iL];
									t.gamevisuals.sTerrainTextures[payload_n] = t.visuals.sTerrainTextures[payload_n];
									t.gamevisuals.sTerrainTexturesName[payload_n] = t.visuals.sTerrainTexturesName[payload_n];
								}
							}
							ImGui::EndDragDropTarget();
						}


						bool bInContext = false;
						static int iCurrentTerrainContext = -1;
						/*
						if (iCurrentTerrainContext == -1 || iCurrentTerrainContext == iL)
						{
							if (ImGui::BeginPopupContextWindow())
							{
								iCurrentTerrainContext = iL;
								bInContext = true;
								if (ImGui::MenuItem("Change Texture"))
								{
									iSelectTexture = iL;
									iCurrentTerrainContext = -1;
								}
								if (ImGui::MenuItem("Change Texture Name"))
								{
									bTextureNameWindow[iL] = true;
									iCurrentTerrainContext = -1;
								}
								if (ImGui::MenuItem("Select Texture"))
								{
									if (iL == 0 && bTutorialCheckAction) TutorialNextAction();
									if (iL == 2 && bTutorialCheckAction) TutorialNextAction();

									iCurrentTextureForPaint = sTerrainSelectionID[iL]; //iL;
								}

								if (iL != 0)
								{
									if (ImGui::MenuItem("Delete Texture"))
									{
										t.visuals.sTerrainTextures[iL] = ""; //delete image in next run.
										t.gamevisuals.sTerrainTextures[iL] = t.visuals.sTerrainTextures[iL];
										g.projectmodified = 1;
										bUpdateTerrainMaterials = true;
										if (iCurrentTextureForPaint == iL)
											iCurrentTextureForPaint--;

										iCurrentTerrainContext = -1;
									}
								}
								ImGui::EndPopup();
							}
							else {
								iCurrentTerrainContext = -1;
							}
						}
						*/

						if (!bInContext && ImGui::IsItemHovered())
						{
							int tooltip_height = 350;
							int tooltip_width = 350;
							ImVec2 tooltip_position = ImVec2(ImGui::GetWindowPos());
							tooltip_position.x = vWindowPos.x - (tooltip_width + 10.0);
							if ((tooltip_position.y + tooltip_height) > (vWindowPos.y + vWindowSize.y))
								tooltip_position.y = (vWindowPos.y + vWindowSize.y) - tooltip_height - 10.0;
							ImGui::SetNextWindowPos(tooltip_position);

							ImGui::BeginTooltip();
							ImGui::ImgBtn(sTerrainTexturesID[iL], ImVec2(350, 350), ImColor(0, 0, 0, 255));
							ImGui::TextCenter(t.visuals.sTerrainTexturesName[iL].Get());
							ImGui::Separator();
							ImGui::EndTooltip();
						}

						ImGui::EndChild();
						ImGui::GetStyle().FramePadding = oldstyle;
					}

					if (iCurrentTextureForPaint == sTerrainSelectionID[iL]) //sTerrainTexturesID[iL] - t.terrain.imagestartindex - 80)
					{
						ImVec2 padding = { 2.0, 2.0 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}

					//					sLabel = cStr("##tradio") + cStr(iL);
					//					float checkwidth = ImGui::GetFontSize()*1.5;
					//					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((ImGui::GetContentRegionAvail().x*0.5) - (checkwidth*0.5), 0.0f));

					if (iOldMaterial != iCurrentTextureForPaint)
					{
						/* g_pTerrain no longer used
						if (g_pTerrain)
						{
							cstr sTextureName = cStr(iCurrentTextureForPaint);
							//Only add material when user select it, save mem and faster loading.
							if (g_pTerrain->FindMaterial(sTextureName.Get()) == nullptr)
							{
								//Not found try to load it.
								g_pTerrain->AddMaterial(sTextureName.Get(), t.visuals.sTerrainTextures[sTerrainSelectionID[iCurrentTextureForPaint]].Get());

							}
							if (g_pTerrain->FindMaterial(sTextureName.Get()) != nullptr)
							{
								g_pTerrain->SetDefaultMaterial("base");

								g_pTerrain->SetMaterial(sTextureName.Get());
							}
							iOldMaterial = iCurrentTextureForPaint;
						}
						*/
					}

					ImGui::NextColumn();

					if (iUsedImages == 1)
					{
						fLastMaxY = ImGui::GetCursorPosY();
					}
					if (iUsedImages == 17)
					{
						fLastMaxY = ImGui::GetCursorPosY() - fLastMaxY;
					}

#else
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

					if (iL == 0) {
						CheckTutorialAction("TOOL_TERRAIN_SAND", -20.0f); //Tutorial: check if we are waiting for this action
					}

					if (ImGui::RadioButton(sLabel.Get(), &iCurrentTextureForPaint, iL)) {
						if (iL == 0 && bTutorialCheckAction) TutorialNextAction();
					}
					if (iOldMaterial != iCurrentTextureForPaint)
					{
						//@Michael + 1 should be removed when your ready, this is just to follow your current test code :)
						if (g_pTerrain)
						{
							if (g_pTerrain->FindMaterial(cStr(iCurrentTextureForPaint).Get()) != nullptr)
							{
								g_pTerrain->SetMaterial(cStr(iCurrentTextureForPaint).Get());
							}

							iOldMaterial = iCurrentTextureForPaint;
						}
					}

					ImGui::SameLine();

					int iSmallPreviewYMargin = preview_icon_size / 2;
					int iLargerPreviewIconSize = preview_icon_size * 2;
					ImGui::SetCursorPos(ImVec2(col_start - 10.0f, ImGui::GetCursorPosY() - iSmallPreviewYMargin));
					//					ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY() - 3));

					if (sTerrainTexturesID[iL] > 0)
					{
						if (ImGui::ImgBtn(sTerrainTexturesID[iL], ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize), ImColor(0, 0, 0, 255)))
						{
							iSelectTexture = iL;
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::ImgBtn(sTerrainTexturesID[iL], ImVec2(180, 180), ImColor(0, 0, 0, 255));
							ImGui::EndTooltip();
						}
						ImGui::SameLine();
					}

					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + iSmallPreviewYMargin));

					int iInputFlags = ImGuiInputTextFlags_EnterReturnsTrue;
					if (!bDeleteImage)
						iInputFlags = ImGuiInputTextFlags_ReadOnly;

					sLabel = cStr("##InputTerrainTexture") + cStr(iL);
					ImGui::PushItemWidth(-10 - path_gadget_size);
					if (ImGui::InputText(sLabel.Get(), &materialname[0], 250, iInputFlags))
					{
						if (strlen(materialname) == 0)
							t.visuals.sTerrainTextures[iL] = ""; //delete image in next run.
						t.gamevisuals.sTerrainTextures[iL] = t.visuals.sTerrainTextures[iL];
						g.projectmodified = 1;
						bUpdateTerrainMaterials = true;
						if (iCurrentTextureForPaint == iL)
							iCurrentTextureForPaint--;

					}
					ImGui::PopItemWidth();

					ImGui::SameLine();
					ImGui::PushItemWidth(path_gadget_size);
					sLabel = cStr("...##InputTerrainTexture") + cStr(iL);

					if (ImGui::StyleButton(sLabel.Get()))
						iSelectTexture = iL;

					if (bDeleteImage) {
						ImGui::SameLine();
						sLabel = cStr("X##InputTerrainTexture") + cStr(iL);
						if (ImGui::StyleButton(sLabel.Get())) {
							t.visuals.sTerrainTextures[iL] = ""; //delete image in next run.
							t.gamevisuals.sTerrainTextures[iL] = t.visuals.sTerrainTextures[iL];
							g.projectmodified = 1;
							bUpdateTerrainMaterials = true;
							if (iCurrentTextureForPaint == iL)
								iCurrentTextureForPaint--;
						}
					}

					ImGui::PopItemWidth();

					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - (iSmallPreviewYMargin*2.0)));
#endif

				}
			}
			else {
				//Delete old image.
				if (ImageExist(sTerrainTexturesID[iL]) == 1) DeleteImage(sTerrainTexturesID[iL]);
				//sTerrainTexturesID[iL] = 0;
			}

		}

#ifdef DISPLAY4x4

		//ImGui::Indent(-4);
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0, 3.0));

		ImGui::EndChild();

		ImGui::GetStyle().WindowPadding = oldwinstylemain;
		ImGui::GetStyle().FramePadding = oldstylemain;

		ImGui::Columns(1);
		ImGui::Indent(10);

		#ifdef CUSTOMTEXTURES
		float avail = ImGui::GetContentRegionAvailWidth();
		ImVec2 buttonSize = ImVec2(195, 23);
		float width = 0.0f;
		width += buttonSize.x;
		width += ImGui::GetStyle().ItemSpacing.x;
		float off = (avail - width) * 0.5f;
		if (off > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
		if (ImGui::Button("Change texture folder", buttonSize))
		{
			ChooseTerrainTextureFolder();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Use textures from a folder elsewhere on your system");
		}

		//LB: moved out of GGTerrain so can access global structures
		if (g_iDeferTextureUpdateToNow == 3)
		{
			// ensure invalid terrain textures are not allowed to be used
			for (auto& id : g_DeferTextureUpdateIncompatibleTextures)
			{
				char* name = t.visuals.sTerrainTextures[id].Get();
				int nameLength = t.visuals.sTerrainTextures[id].Len();
				if (strcmp(name + nameLength - strlen("mat32\\Color.dds"), "mat32\\Color.dds") != 0)
				{
					t.visuals.sTerrainTextures[id] = "";
				}
			}
			if (g_DeferTextureUpdateIncompatibleTextures.size() == g_DeferTextureUpdate.size())
			{
				// No textures in this folder are useable
				extern bool bTriggerMessage;
				extern char cTriggerMessage[MAX_PATH];
				bTriggerMessage = true;
				strcpy(cTriggerMessage, "No compatible textures");// , try PNG format");
				ResetTextureSettings();
			}

			// textures updated, we can finish this state machine :)
			g_iDeferTextureUpdateToNow = 0;
		}

		width = 0.0f;
		width += buttonSize.x;
		width += ImGui::GetStyle().ItemSpacing.x;
		off = (avail - width) * 0.5f;
		if (off > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
		if (ImGui::Button("Reset texture settings", buttonSize))
		{
			ResetTextureSettings();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Restore the texture folder and default texture set to their original states");
		}
		#endif
#endif


#define REMOVETERRAINTEXTUREDIALOG
		static bool terrain_selection_window = false;
		static int iSelectedTerrainTexture = -1;
		if (iSelectTexture >= 0)
		{
			iSelectedTerrainTexture = iSelectTexture;
#ifndef REMOVETERRAINTEXTUREDIALOG
			terrain_selection_window = true;
			iSelectTexture = 0;
#else
			iSelectTexture = 999;
#endif
		}

#ifndef REMOVETERRAINTEXTUREDIALOG
		iSelectTexture = imgui_get_selections(terrain_selections, terrain_selections_text, 2, 380, &terrain_selection_window);
#endif
		if (iSelectTexture >= 0 && iSelectedTerrainTexture >= 0)
		{
			bool bNeedFileSelector = false;
			if (iSelectTexture == 999)
				bNeedFileSelector = true;

			terrain_selection_window = false;

			int iNewTerrainIndex = iSelectTexture;
			iSelectTexture = iSelectedTerrainTexture;

			if (iSelectTexture == 99) {
				//Find free slot.
				for (int iL2 = 0; iL2 < 32; iL2++) {
					if (t.visuals.sTerrainTextures[iL2] == "") {
						iSelectTexture = iL2;
						break;
					}
				}
			}

			if (!bNeedFileSelector) {
				t.visuals.sTerrainTextures[iSelectTexture] = terrain_selections[iNewTerrainIndex];
				if (ImageExist(sTerrainTexturesID[iSelectTexture]))
					iDeleteSingleTerrainTextures = sTerrainTexturesID[iSelectTexture];
				iCurrentTextureForPaint = iSelectTexture;
				bUpdateTerrainMaterials = true;
			}

			if (bNeedFileSelector)
			{

				//iSelectTexture
				cStr tOldDir = GetDir();
				char * cFileSelected;
				cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "All\0*.*\0DDS\0*.dds\0PNG\0*.png\0JPEG\0*.jpg\0TGA\0*.tga\0BMP\0*.bmp\0\0\0", sTextureFolder.Get(), NULL);
				SetDir(tOldDir.Get());

				if (cFileSelected && strlen(cFileSelected) > 0)
				{
					char *relonly = (char *)pestrcasestr(cFileSelected, g.rootdir_s.Get());

					t.visuals.sTerrainTextures[iSelectTexture] = cFileSelected;
					t.visuals.sTerrainTexturesName[iSelectTexture] = ""; //PE: User must enter a proper name.
					bTextureNameWindow[iSelectTexture] = true;
					if (relonly) {
						t.visuals.sTerrainTextures[iSelectTexture] = cFileSelected + g.rootdir_s.Len();
					}
					g.projectmodified = 1;
					t.gamevisuals.sTerrainTextures[iSelectTexture] = t.visuals.sTerrainTextures[iSelectTexture];
					t.gamevisuals.sTerrainTexturesName[iSelectTexture] = t.visuals.sTerrainTexturesName[iSelectTexture];
					bUpdateTerrainMaterials = true;
					iCurrentTextureForPaint = iSelectTexture;

					//PE: Reload image.
					if (ImageExist(sTerrainTexturesID[iSelectTexture]))
						iDeleteSingleTerrainTextures = sTerrainTexturesID[iSelectTexture];
				}
			}
			iSelectTexture = -1;
		}

		if (bUpdateTerrainMaterials)
		{
			/* g_pTerrain no longer used
			if (g_pTerrain)
			{
				// THIS TAKES 4 SECONDS DURING THE LAUNCH (OUCH)
				// I would move this to a thread and have it done in the background
				// though Paul may redo the whole terrain code with the updated design so not need at this stage to optimize this

				// this is going to clear all current materials in the list, ideally it should not do this
				g_pTerrain->ClearMaterials(false);
				for (int iL = 0; iL < 32; iL++)
				{
					if (t.visuals.sTerrainTextures[iL] != "")
					{
						//PE: When reload , selection id must fit.
						sTerrainSelectionID[iL] = iL;

						// add a material, given this is already in use for the palette we should share
						// the pixel data as this function is going to make its own copy and it will slow
						// things down, not good
						g_pTerrain->AddMaterial(cStr(iL).Get(), t.visuals.sTerrainTextures[iL].Get());
					}
				}

				// due to materials being cleared aside from the base we must reset it
				g_pTerrain->SetDefaultMaterial("base");

				// set the new material
				g_pTerrain->SetMaterial(cStr(iCurrentTextureForPaint).Get());
			}
			*/
			bUpdateTerrainMaterials = false;
		}

		ImGui::Indent(-10);
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
	}
	if (t.visuals.customTexturesFolder.Len() > 0)
	{
		if (ImGui::StyleCollapsingHeader("Palette Properties", 0))
		{
			bool bOpenMaterialSelector = false;

			ImGui::Indent(10);
			ImGui::TextCenter("Terrain Layers");
			ImGui::Spacing();
			const char* materialNames[5] = {"Lowest Layer", "Low Layer", "Middle Layer", "High Layer", "Highest Layer"};
			int startNameIndexOffset = 0;
			static int layerMatIndex = -1;
			for (int i = 4; i >= 0; i--)
			{
				if (ggterrain_global_render_params.layerStartHeight[i] != ggterrain_global_render_params.layerEndHeight[i])
				{
					int index = ggterrain_global_render_params.layerMatIndex[i] & 0xFF;
					bool rotate = (ggterrain_global_render_params.layerMatIndex[i] >> 8) != 0;
					ImGui::PushID(234355 + i);
					ImGui::Text(materialNames[i + startNameIndexOffset]);
					int texID = sTerrainTexturesID[index];
					if (ImageExist(texID) == 0)
					{
						for (int ID = 31; ID >= 0; ID--)
						{
							if (t.visuals.sTerrainTextures[ID].Len() > 0)
							{
								texID = sTerrainTexturesID[ID];
								index = ID;
								break;
							}
						}
					}
					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() / 2.0f - 25, ImGui::GetCursorPosY()-8));
					if (ImGui::ImgBtn(texID, ImVec2(50, 25)))
					{
						bOpenMaterialSelector = true;
						layerMatIndex = i;
					}
					ImGui::PopID();
				}
				else
				{
					if (startNameIndexOffset == 0)
					{
						// Display "Low Layer", "Middle Layer", "High Layer" instead of the five material names (lowest and highest not used)
						startNameIndexOffset++;
					}
				}
			}
			int index = ggterrain_global_render_params.baseLayerMaterial & 0xFF;
			bool rotate = (ggterrain_global_render_params.baseLayerMaterial >> 8) != 0;
			ImGui::PushID(255555);
			ImGui::Text("Underwater");
			static int underWaterLayer = -1;
			int texID = sTerrainTexturesID[index];
			if (ImageExist(texID) == 0)
			{
				for (int ID = 31; ID >= 0; ID--)
				{
					if (t.visuals.sTerrainTextures[ID].Len() > 0)
					{
						texID = sTerrainTexturesID[ID];
						index = ID;
						break;
					}
				}
			}
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() / 2.0f - 25, ImGui::GetCursorPosY() - 8));
			if (ImGui::ImgBtn(texID, ImVec2(50, 25)))
			{
				bOpenMaterialSelector = true;
				underWaterLayer = 1;
			}
			ImGui::PopID();
			ImGui::TextCenter("Slope Layers");
			ImGui::Spacing();
			const char* slopeNames[2] = { "Main Slope", "Secondary Slope" };
			static int slopeMatIndex = -1;
			for (int i = 0; i < 2; i++)
			{
				if (ggterrain_global_render_params.slopeStart[i] != ggterrain_global_render_params.slopeEnd[i])
				{
					int index = ggterrain_global_render_params.slopeMatIndex[i] & 0xFF;
					bool rotate = (ggterrain_global_render_params.slopeMatIndex[i] >> 8) != 0;
					ImGui::PushID(234000 + i);
					ImGui::Text(slopeNames[i]);
					int texID = sTerrainTexturesID[index];
					if (ImageExist(texID) == 0)
					{
						for (int ID = 31; ID >= 0; ID--)
						{
							if (t.visuals.sTerrainTextures[ID].Len() > 0)
							{
								texID = sTerrainTexturesID[ID];
								index = ID;
								break;
							}
						}
					}
					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() / 2.0f - 25, ImGui::GetCursorPosY() - 8));
					if (ImGui::ImgBtn(texID, ImVec2(50, 25)))
					{
						bOpenMaterialSelector = true;
						slopeMatIndex = i;
					}
					ImGui::PopID();
				}
			}

			ImGui::Spacing();
			ImGui::Spacing();

			if (bOpenMaterialSelector)
			{
				ImGui::OpenPopup("##texturepalette");
			}
			if (ImGui::BeginPopup("##texturepalette"))
			{
				for (int i = 0; i < GGTERRAIN_MAX_SOURCE_TEXTURES; i++)
				{
					ImGui::PushID(88888+ i);
					if (i % 3 != 0)
						ImGui::SameLine();
					if (ImGui::ImgBtn(sTerrainTexturesID[i], ImVec2(50.0f, 50.0f)))
					{
						// Check if we are altering height material or slope material
						int* matIndexToChange = nullptr;
						if (layerMatIndex >= 0)
						{
							matIndexToChange = &ggterrain_global_render_params.layerMatIndex[layerMatIndex];
						}
						else if (slopeMatIndex >= 0)
						{
							matIndexToChange = &ggterrain_global_render_params.slopeMatIndex[slopeMatIndex];
						}
						else if (underWaterLayer >= 0)
						{
							matIndexToChange = &ggterrain_global_render_params.baseLayerMaterial;
						}

						// Update the layer material
						if (matIndexToChange)
						{
							*matIndexToChange = 0x100 | i;
						}

						layerMatIndex = -1;
						slopeMatIndex = -1;
						// Trigger update of material sounds
						extern bool g_bMapMatIDToMatIndexAvailable;
						g_bMapMatIDToMatIndexAvailable = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::PopID();
				}
				ImGui::EndPopup();
			}
			
			ImGui::TextCenter("Texture Material Type");
			ImGui::GetStyle().ItemSpacing.y -= 10;
			std::array<int, 8> matTypes = {
				0, // Grass
				1, // Stone
				2, // Metal
				3, // Wood
				6, // Snow
				10,// Tarmac (generic)
				11,// Dirt
				13 // Sand
			};
			std::array<const char*, 8> matTypeNames = {
				"Grass",
				"Stone",
				"Metal",
				"Wood",
				"Snow",
				"Generic",
				"Dirt",
				"Sand"
			};
			for (int i = 0; i < GGTERRAIN_MAX_SOURCE_TEXTURES; i++)
			{
				if (t.visuals.sTerrainTextures[i].Len() > 0)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::ImgBtn(sTerrainTexturesID[i], ImVec2(50, 25));
					ImGui::PopItemFlag();
					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 4.0f));
					bool b = false;
					char comboName[64];
					sprintf_s(comboName, "##materialtype%d", i);
					char selectedMat[64];
					int val = g_iCustomTerrainMatSounds[i];
					for (int j= 0; j < matTypes.size();j++)
					{
						if (val == matTypes[j])
						{
							strcpy(selectedMat, matTypeNames[j]);
							break;
						}
					}
					if (strlen(selectedMat) == 0)
					{
						strcpy(selectedMat, "Silent");
					}
					ImGui::PushItemWidth(-10);
					if (ImGui::BeginCombo(comboName, selectedMat))
					{
						ImGui::GetStyle().ItemSpacing.y += 10;
						for (int j = 0; j < matTypes.size(); j++)
						{
							if (ImGui::Selectable(matTypeNames[j]))
							{
								g_iCustomTerrainMatSounds[i] = matTypes[j];
								// Trigger update of material sounds
								extern bool g_bMapMatIDToMatIndexAvailable;
								g_bMapMatIDToMatIndexAvailable = false;
							}
						}
						ImGui::GetStyle().ItemSpacing.y -= 10;
						ImGui::EndCombo();
					}
					ImGui::PopItemWidth();
				}
			}
			ImGui::GetStyle().ItemSpacing.y += 10;
			ImGui::Indent(-10);
			ImGui::Spacing();
		}
	}
}

int iDeleteSingleTreeTextures = 0;
int iDeleteAllTreeTextures = 0;
void imgui_Customize_Tree_v3(int mode)
{
	const uint32_t numTreeTypes = 64; // max tree types

	static cstr sTreeTextures[numTreeTypes];
	static cstr sTreeTexturesName[numTreeTypes];
	static int sTreeTexturesID[numTreeTypes];
	static bool bTreeSelected[numTreeTypes];
	static bool bTreeInit = true;


	int wflags = ImGuiTreeNodeFlags_DefaultOpen;
	if (mode == 1) wflags = ImGuiTreeNodeFlags_None;
	if (pref.bAutoClosePropertySections && mode == 1 && iLastOpenHeader != 2)
		ImGui::SetNextItemOpen(false, ImGuiCond_Always);

	float media_icon_size = 40.0f;
	float w = ImGui::GetWindowContentRegionWidth();
	float plate_width = (media_icon_size + 6.0) * 4.0f;
	if (ImGui::StyleCollapsingHeader("Palette", wflags))
	{
		if (mode == 1) iLastOpenHeader = 2;
		//Drpo down.
		ImGui::Indent(10);
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));


		if (iDeleteSingleTreeTextures > 0)
		{
			if (ImageExist(iDeleteSingleTreeTextures))
				DeleteImage(iDeleteSingleTreeTextures);
			iDeleteSingleTreeTextures = 0;
		}
		if (iDeleteAllTreeTextures)
		{
			iDeleteAllTreeTextures = false;
			for (int iL = 0; iL < GGTrees_GetNumTypes(); iL++)
			{
				sTreeTexturesID[iL] = t.terrain.imagestartindex + 280 + iL;
				if (ImageExist(sTreeTexturesID[iL]))
					DeleteImage(sTreeTexturesID[iL]);
			}
		}

		if (bTreeInit)
		{
			bTreeInit = false;

			for (int iL = 0; iL < GGTrees_GetNumTypes(); iL++) {
				sTreeTexturesID[iL] = t.terrain.imagestartindex + 280 + iL;
				sTreeTexturesName[iL] = "";
				sTreeTextures[0] = "";
				bTreeSelected[iL] = false;
			}
			bTreeSelected[0] = true;

			char path[ 1024 ];
			for( uint32_t i = 0; i < GGTrees_GetNumTypes(); i++ )
			{
				const char* textureName = GGTrees_GetTextureName(i);
				if ( textureName && *textureName )
				{
					strcpy_s( path, "treebank/billboards/" );
					strcat_s( path, GGTrees_GetTextureName(i) );
					sTreeTextures[i] = path;
				}
			}

			sTreeTexturesName[0]  = "Birch";
			sTreeTexturesName[1]  = "Cactus Var 1";
			sTreeTexturesName[2]  = "Cactus Var 2";
			sTreeTexturesName[3]  = "Cactus Var 3";
			sTreeTexturesName[4]  = "Cactus Var 4";
			sTreeTexturesName[5]  = "Dead Pine Tree";
			sTreeTexturesName[6]  = "Dry Pine";
			sTreeTexturesName[7]  = "Italian Pine";
			sTreeTexturesName[8]  = "Jungle Tree 1";
			sTreeTexturesName[9]  = "Jungle Tree 2";
			sTreeTexturesName[10] = "Jungle Tree 3a";
			sTreeTexturesName[11] = "Jungle Tree 3b";
			sTreeTexturesName[12] = "Jungle Tree 4a";
			sTreeTexturesName[13] = "Jungle Tree 4b";
			sTreeTexturesName[14] = "Jungle Tree 5a";
			sTreeTexturesName[15] = "Jungle Tree 5b";
			sTreeTexturesName[16] = "Jungle Tree 6a";
			sTreeTexturesName[17] = "Jungle Tree 6b";
			sTreeTexturesName[18] = "Kentia Palm";
			sTreeTexturesName[19] = "Palm";
			sTreeTexturesName[20] = "Pine";
			sTreeTexturesName[21] = "Scots Pine 1";
			sTreeTexturesName[22] = "Scots Pine 2";
			sTreeTexturesName[23] = "Scots Pine Dead";
			sTreeTexturesName[24] = "Snow Fir 2";
			sTreeTexturesName[25] = "Snow Fir 3";
			sTreeTexturesName[26] = "Snow Fir";
			sTreeTexturesName[27] = "Snow Pine";
			sTreeTexturesName[28] = "Snow Pine Tall 2";
			sTreeTexturesName[29] = "Snow Pine Tall";
			sTreeTexturesName[30] = "Sparse Pine";
			sTreeTexturesName[31] = "Vine Tree Large";
			sTreeTexturesName[32] = "Vine Tree Small";
			sTreeTexturesName[33] = "Western Pine";
			sTreeTexturesName[34] = "White Pine";
			sTreeTexturesName[35] = "Autumn Birch 1";
			sTreeTexturesName[36] = "Autumn Birch 2";
			sTreeTexturesName[37] = "Autumn Birch 3";
		}

		int iUsedImages = 0;
		float col_start = 100.0f;
		int iSelectTexture = -1;

		ImGui::Indent(-10);

		static float fLast4x4Height = 228.0;
		static float fLastMaxY = 0.0;
		if (fLastMaxY > 100.0 && fLastMaxY < 400.0)
		{
			fLast4x4Height = fLastMaxY+1.0;
		}

		ImVec2 oldstylemain = ImGui::GetStyle().FramePadding;
		ImVec2 oldwinstylemain = ImGui::GetStyle().WindowPadding;
		ImGui::GetStyle().WindowPadding = { 0,0 };
		ImGui::GetStyle().FramePadding = { 0,0 };
		ImVec2 vWindowPos = ImGui::GetWindowPos();
		ImVec2 vWindowSize = ImGui::GetWindowSize();

		ImGui::BeginChild("##Tree4x4scrollbar", ImVec2(0, fLast4x4Height), false, 0); //ImGuiWindowFlags_AlwaysVerticalScrollbar

		w = ImGui::GetWindowContentRegionWidth(); //PE: Minus scrollbar.
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0, 2.0));

		ImGui::Columns(4, "##Tree4x4columns", false);  //false no border
		bool bGotSelection = false;
		ImRect image_selected_bb;
		uint64_t values = ggtrees_global_params.paint_tree_bitfield;
		if (values > 0x3000000000) //Max 2fffffffff
		{
			values = 1; //PE: Somethis is wrong , select default tree.
			ggtrees_global_params.paint_tree_bitfield = 1;
		}
		for (int iL = 0; iL < GGTrees_GetNumTypes(); iL++)
		{
			float imageScaleX = GGTrees_GetImageScale( iL );

			if (sTreeTextures[iL] != "")
			{
				if (!ImageExist(sTreeTexturesID[iL]))
				{
					//Load in image.
					image_setlegacyimageloading(true);
					SetMipmapNum(1); //PE: mipmaps not needed.
					if (ImageExist(sTreeTexturesID[iL]) == 1) DeleteImage(sTreeTexturesID[iL]);
					LoadImage(sTreeTextures[iL].Get(), sTreeTexturesID[iL], 0, g.gdividetexturesize);
					//LoadImageSize(pCompressedVersionIfAny, sTreeTexturesID[iL], 512,512); //Takes to long.
					if (!ImageExist(sTreeTexturesID[iL]) == 1)
					{
						//Load failed, clear texture slot.
						sTreeTextures[iL] = "";
					}
					SetMipmapNum(-1);
					image_setlegacyimageloading(false);
				}

				if (ImageExist(sTreeTexturesID[iL]))
				{

					iUsedImages++;

					float path_gadget_size = ImGui::GetFontSize()*3.0;
					int preview_icon_size = ImGui::GetFontSize();

					bool bDeleteImage = true;
					if (iL == 0)
						bDeleteImage = false;

					//TEXTURE.

					cStr sLabel = cStr("Texture##InputTreeTexture") + cStr(iL);
					if (!bDeleteImage)
						sLabel = cStr("Default##InputTreeTexture") + cStr(iL);

					//PE: New do 4x4 coloums here.
					int iLargerPreviewIconSize = 28;//PE: 54 , now lowest possible icon
					float control_width = (iLargerPreviewIconSize + 3.0) * 4.0f + 6.0;

					if (w > control_width) {
						//PE: fit perfectly with window width.
						iLargerPreviewIconSize = (w - 20.0) / 4.0;
						//iLargerPreviewIconSize -= 6.0; //Padding.
						iLargerPreviewIconSize -= 10.0; //Padding.
						if (iLargerPreviewIconSize > 70) iLargerPreviewIconSize = 70;
					}

					ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					if (sTreeTexturesID[iL] > 0)
					{

						cStr sLabelChild = cStr("##Tree4x4") + cStr(iL);

						//ImVec2 content_avail = { iLargerPreviewIconSize*imageScaleX + 1.0f ,iLargerPreviewIconSize + 1.0f };
						ImVec2 content_avail = { iLargerPreviewIconSize + 1.0f ,iLargerPreviewIconSize + 1.0f };

						//style.WindowPadding
						ImVec2 oldstyle = ImGui::GetStyle().FramePadding;
						ImGui::GetStyle().FramePadding = { 1,1 };
						ImGui::BeginChild(sLabelChild.Get(), content_avail, false, ImGuiWindowFlags_NoScrollbar);

						iLargerPreviewIconSize &= 0xfffe;

						ImGui::SetBlurMode(true);
						//9999 halv uv x
						ImVec2 fill_rect = ImGui::GetWindowPos() + ImGui::GetCursorPos();
						ImGui::GetCurrentWindow()->DrawList->AddRectFilled(fill_rect, fill_rect + ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), 0.0f, ImDrawCornerFlags_None);
						ImGui::SetCursorPosX(((float)iLargerPreviewIconSize - ((float)iLargerPreviewIconSize*imageScaleX)) * 0.5);

						if (ImGui::ImgBtn(sTreeTexturesID[iL], ImVec2(iLargerPreviewIconSize*imageScaleX, iLargerPreviewIconSize), ImColor(0, 0, 0, 255), ImColor(255, 255, 255, 255), ImColor(220, 220, 220, 220), ImColor(220, 220, 220, 220),-1,0,0,0))
						{
							//ggtrees_global_params.paint_tree_type = iL;
							//ggtrees_global_params.paint_tree_bitfield = 1 << iL; // multiple bits can be set
							//PE: Toggle
							if (ggtrees_global_params.paint_tree_bitfield & (1ULL << iL))
								ggtrees_global_params.paint_tree_bitfield &= ~(1ULL << iL);
							else
								ggtrees_global_params.paint_tree_bitfield |= (1ULL << iL);
						}
						ImGui::SetBlurMode(false);

						if (ImGui::IsItemHovered())
						{
							int tooltip_height = 350;
							int tooltip_width = 350 * imageScaleX;
							ImVec2 tooltip_position = ImVec2(ImGui::GetWindowPos());
							tooltip_position.x = vWindowPos.x - (tooltip_width + 10.0);
							if ((tooltip_position.y + tooltip_height) > (vWindowPos.y + vWindowSize.y))
								tooltip_position.y = (vWindowPos.y + vWindowSize.y) - tooltip_height - 10.0;
							ImGui::SetNextWindowPos(tooltip_position);

							ImGui::BeginTooltip();
							ImGui::ImgBtn(sTreeTexturesID[iL], ImVec2(350*imageScaleX, 350), ImColor(0, 0, 0, 255), ImColor(255, 255, 255, 255), ImColor(220, 220, 220, 220), ImColor(220, 220, 220, 220), -1, 0, 0, 0);
							ImGui::TextCenter(sTreeTexturesName[iL].Get());
							ImGui::Separator();
							ImGui::EndTooltip();
						}

						ImGui::EndChild();
						ImGui::GetStyle().FramePadding = oldstyle;
					}

					if (ggtrees_global_params.paint_tree_bitfield & (1ULL << iL)) //sTerrainTexturesID[iL] - t.terrain.imagestartindex - 80)
					{
						ImVec2 padding = { 2.0, 2.0 };
						image_selected_bb = ImRect((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_selected_bb.Min, image_selected_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
						bGotSelection = true;
						values |= (1ULL << iL);
					}
					else
					{
						values &= ~(1ULL << iL);
					}

					if (iUsedImages == 1 || iUsedImages == 5 || iUsedImages == 9 || iUsedImages == 13 || iUsedImages == 17)
					{
						fLastMaxY = ImGui::GetCursorPosY();
					}

					ImGui::NextColumn();

				}
			}
			else {
				//Delete old image.
				if (ImageExist(sTerrainTexturesID[iL]) == 1) DeleteImage(sTerrainTexturesID[iL]);
			}

		}
		if (values <= 0) values = 1; //Always have one tree selected.
		ggtrees_global_params.paint_tree_bitfield = values;

		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0, 3.0));

		if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
			//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
			ImGui::Text("");
			ImGui::Text("");
			fLastMaxY += 20.0;
		}

		ImGui::EndChild();
//		if (bGotSelection)
//		{
//			//tool_selected_col
//			ImVec4 test = ImVec4(1.0,0.0,0.0,1.0);
//			//ImGui::GetCurrentWindow()->DrawList->AddRect(image_selected_bb.Min, image_selected_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
//			ImGui::PushClipRect(image_selected_bb.Min, image_selected_bb.Max, false);
//			ImGui::GetCurrentWindow()->DrawList->AddRect(image_selected_bb.Min, image_selected_bb.Max, ImGui::GetColorU32(test), 0.0f, 15, 4.0f);
//			ImGui::PopClipRect();
//		}

		ImGui::GetStyle().WindowPadding = oldwinstylemain;
		ImGui::GetStyle().FramePadding = oldstylemain;

		ImGui::Columns(1);


		float but_gadget_size = ImGui::GetFontSize()*10.0;
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));

		if (ImGui::StyleButton("Randomize All Trees##TerrainTrees", ImVec2(but_gadget_size, 0)))
		{
			int iAction = askBoxCancel("This will randomize all your trees, are you sure?", "Confirmation"); //1==Yes 2=Cancel 0=No
			if (iAction == 1)
			{
				//ggtrees_global_params.paint_density //PE: Perhaps make this random ?
				GGTrees::GGTrees_RepopulateInstances(); //PE: Needed to get new random positions.
			}
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("All trees in the level will have their position and rotation randomized");

		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
		if (ImGui::StyleButton("Clear All Trees##TerrainTrees", ImVec2(but_gadget_size, 0)))
		{
			int iAction = askBoxCancel("This will delete all your trees, are you sure?", "Confirmation"); //1==Yes 2=Cancel 0=No
			if (iAction == 1)
			{
				GGTrees::GGTrees_HideAll();
			}
		}

		ImRect bbwin(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize());
		if (ImGui::IsMouseHoveringRect(bbwin.Min, bbwin.Max))
		{
			bImGuiGotFocus = true;
		}

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
	}
}


#ifdef BUSHUI

int iDeleteSingleBushTextures = 0;
int iDeleteAllBushTextures = 0;
void imgui_Customize_Bush_v3(int mode)
{

	static cstr sBushTextures[32];
	static cstr sBushTexturesName[32];
	static int sBushTexturesID[32];
	static bool bBushSelected[32];
	static bool bBushInit = true;


	int wflags = ImGuiTreeNodeFlags_DefaultOpen;
	if (mode == 1) wflags = ImGuiTreeNodeFlags_None;
	if (pref.bAutoClosePropertySections && mode == 1 && iLastOpenHeader != 2)
		ImGui::SetNextItemOpen(false, ImGuiCond_Always);

	float media_icon_size = 40.0f;
	float w = ImGui::GetWindowContentRegionWidth();
	float plate_width = (media_icon_size + 6.0) * 4.0f;
	if (ImGui::StyleCollapsingHeader("Palette", wflags))
	{
		if (mode == 1) iLastOpenHeader = 2;
		//Drpo down.
		ImGui::Indent(10);
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));


		if (iDeleteSingleBushTextures > 0)
		{
			if (ImageExist(iDeleteSingleBushTextures))
				DeleteImage(iDeleteSingleBushTextures);
			iDeleteSingleBushTextures = 0;
		}
		if (iDeleteAllBushTextures)
		{
			iDeleteAllBushTextures = false;
			for (int iL = 0; iL < 32; iL++)
			{
				sBushTexturesID[iL] = t.terrain.imagestartindex + 280 + iL;
				if (ImageExist(sBushTexturesID[iL]))
					DeleteImage(sBushTexturesID[iL]);
			}
		}

		if (bBushInit)
		{
			bBushInit = false;

			for (int iL = 0; iL < 32; iL++) {
				sBushTexturesID[iL] = t.terrain.imagestartindex + 280 + iL;
				sBushTexturesName[iL] = "";
				sBushTextures[0] = "";
				bBushSelected[iL] = false;
			}
			bBushSelected[0] = true;

			sBushTextures[0] = "treebank/billboards/pine.dds";
			sBushTextures[1] = "treebank/billboards/birch.dds";
			sBushTextures[2] = "treebank/billboards/cactus var1.dds";
			sBushTextures[3] = "treebank/billboards/italian pine.dds";
			sBushTextures[4] = "treebank/billboards/kentia palm.dds";
			sBushTextures[5] = "treebank/billboards/palm.dds";
			sBushTextures[6] = "treebank/billboards/vine tree large.dds";
			sBushTextures[7] = "treebank/billboards/vine tree small.dds";
			sBushTextures[8] = "treebank/billboards/white pine.dds";
			sBushTextures[9] = "treebank/billboards/snow pine.dds";
			sBushTextures[10] = "treebank/billboards/snow fir1.dds";
			sBushTextures[11] = "treebank/billboards/snow fir2.dds";
			sBushTextures[12] = "treebank/billboards/snow fir3.dds";

			sBushTexturesName[0] = "pine";
			sBushTexturesName[1] = "birch";
			sBushTexturesName[2] = "cactus var1";
			sBushTexturesName[3] = "italian pine";
			sBushTexturesName[4] = "kentia palm";
			sBushTexturesName[5] = "palm";
			sBushTexturesName[6] = "vine tree large";
			sBushTexturesName[7] = "vine tree small";
			sBushTexturesName[8] = "white pine";
			sBushTexturesName[9] = "Snow pine";
			sBushTexturesName[10] = "Snow fir 1";
			sBushTexturesName[11] = "Snow fir 2";
			sBushTexturesName[12] = "Snow fir 3";

		}

		int iUsedImages = 0;
		float col_start = 100.0f;
		int iSelectTexture = -1;

		ImGui::Indent(-10);

		static float fLast4x4Height = 228.0;
		static float fLastMaxY = 0.0;
		if (fLastMaxY > 100.0 && fLastMaxY < 400.0)
		{
			fLast4x4Height = fLastMaxY + 1.0;
		}

		ImVec2 oldstylemain = ImGui::GetStyle().FramePadding;
		ImVec2 oldwinstylemain = ImGui::GetStyle().WindowPadding;
		ImGui::GetStyle().WindowPadding = { 0,0 };
		ImGui::GetStyle().FramePadding = { 0,0 };

		ImGui::BeginChild("##Bush4x4scrollbar", ImVec2(0, fLast4x4Height), false, 0); //ImGuiWindowFlags_AlwaysVerticalScrollbar

		w = ImGui::GetWindowContentRegionWidth(); //PE: Minus scrollbar.
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0, 2.0));

		ImGui::Columns(4, "##Bush4x4columns", false);  //false no border
		bool bGotSelection = false;
		ImRect image_selected_bb;
		uint32_t values = ggtrees_global_params.paint_tree_bitfield;
		for (int iL = 0; iL < 32; iL++)
		{

			if (sBushTextures[iL] != "")
			{
				if (!ImageExist(sBushTexturesID[iL]))
				{
					//Load in image.
					image_setlegacyimageloading(true);
					SetMipmapNum(1); //PE: mipmaps not needed.
					if (ImageExist(sBushTexturesID[iL]) == 1) DeleteImage(sBushTexturesID[iL]);
					LoadImage(sBushTextures[iL].Get(), sBushTexturesID[iL], 0, g.gdividetexturesize);
					//LoadImageSize(pCompressedVersionIfAny, sBushTexturesID[iL], 512,512); //Takes to long.
					if (!ImageExist(sBushTexturesID[iL]) == 1)
					{
						//Load failed, clear texture slot.
						sBushTextures[iL] = "";
					}
					SetMipmapNum(-1);
					image_setlegacyimageloading(false);
				}

				if (ImageExist(sBushTexturesID[iL]))
				{

					iUsedImages++;

					float path_gadget_size = ImGui::GetFontSize()*3.0;
					int preview_icon_size = ImGui::GetFontSize();

					bool bDeleteImage = true;
					if (iL == 0)
						bDeleteImage = false;

					//TEXTURE.

					cStr sLabel = cStr("Texture##InputBushTexture") + cStr(iL);
					if (!bDeleteImage)
						sLabel = cStr("Default##InputBushTexture") + cStr(iL);

					//PE: New do 4x4 coloums here.
					int iLargerPreviewIconSize = 28;//PE: 54 , now lowest possible icon
					float control_width = (iLargerPreviewIconSize + 3.0) * 4.0f + 6.0;

					if (w > control_width) {
						//PE: fit perfectly with window width.
						iLargerPreviewIconSize = (w - 20.0) / 4.0;
						//iLargerPreviewIconSize -= 6.0; //Padding.
						iLargerPreviewIconSize -= 10.0; //Padding.
						if (iLargerPreviewIconSize > 70) iLargerPreviewIconSize = 70;
					}

					ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					if (sBushTexturesID[iL] > 0)
					{

						cStr sLabelChild = cStr("##Bush4x4") + cStr(iL);

						ImVec2 content_avail = { iLargerPreviewIconSize + 1.0f ,iLargerPreviewIconSize + 1.0f };

						//style.WindowPadding
						ImVec2 oldstyle = ImGui::GetStyle().FramePadding;
						ImGui::GetStyle().FramePadding = { 1,1 };
						ImGui::BeginChild(sLabelChild.Get(), content_avail, false, ImGuiWindowFlags_NoScrollbar);

						iLargerPreviewIconSize &= 0xfffe;

						ImGui::SetBlurMode(true);
						//9999 halv uv x
						if (ImGui::ImgBtn(sBushTexturesID[iL], ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize), ImColor(0, 0, 0, 255), ImColor(255, 255, 255, 255), ImColor(220, 220, 220, 220), ImColor(220, 220, 220, 220), -1, 0, 0, 9999))
						{
							//ggtrees_global_params.paint_tree_type = iL;
							//ggtrees_global_params.paint_tree_bitfield = 1 << iL; // multiple bits can be set
							//PE: Toggle
							if (ggtrees_global_params.paint_tree_bitfield & (1 << iL))
								ggtrees_global_params.paint_tree_bitfield &= ~(1 << iL);
							else
								ggtrees_global_params.paint_tree_bitfield |= (1 << iL);
						}
						ImGui::SetBlurMode(false);

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::ImgBtn(sBushTexturesID[iL], ImVec2(350, 350), ImColor(0, 0, 0, 255), ImColor(255, 255, 255, 255), ImColor(220, 220, 220, 220), ImColor(220, 220, 220, 220), -1, 0, 0, 9999);
							ImGui::TextCenter(sBushTexturesName[iL].Get());
							ImGui::Separator();
							ImGui::EndTooltip();
						}

						ImGui::EndChild();
						ImGui::GetStyle().FramePadding = oldstyle;
					}

					if (ggtrees_global_params.paint_tree_bitfield & (1 << iL)) //sTerrainTexturesID[iL] - t.terrain.imagestartindex - 80)
					{
						ImVec2 padding = { 2.0, 2.0 };
						image_selected_bb = ImRect((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_selected_bb.Min, image_selected_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
						bGotSelection = true;
						values |= (1 << iL);
					}
					else
					{
						values &= ~(1 << iL);
					}

					if (iUsedImages == 1 || iUsedImages == 5 || iUsedImages == 9 || iUsedImages == 13 || iUsedImages == 17)
					{
						fLastMaxY = ImGui::GetCursorPosY();
					}

					ImGui::NextColumn();

				}
			}
			else {
				//Delete old image.
				if (ImageExist(sTerrainTexturesID[iL]) == 1) DeleteImage(sTerrainTexturesID[iL]);
			}

		}
		if (values <= 0) values = 1; //Always have one tree selected.
		ggtrees_global_params.paint_tree_bitfield = values;

		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0, 3.0));

		if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
			//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
			ImGui::Text("");
			ImGui::Text("");
			fLastMaxY += 20.0;
		}

		ImGui::EndChild();
		//		if (bGotSelection)
		//		{
		//			//tool_selected_col
		//			ImVec4 test = ImVec4(1.0,0.0,0.0,1.0);
		//			//ImGui::GetCurrentWindow()->DrawList->AddRect(image_selected_bb.Min, image_selected_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
		//			ImGui::PushClipRect(image_selected_bb.Min, image_selected_bb.Max, false);
		//			ImGui::GetCurrentWindow()->DrawList->AddRect(image_selected_bb.Min, image_selected_bb.Max, ImGui::GetColorU32(test), 0.0f, 15, 4.0f);
		//			ImGui::PopClipRect();
		//		}

		ImGui::GetStyle().WindowPadding = oldwinstylemain;
		ImGui::GetStyle().FramePadding = oldstylemain;

		ImGui::Columns(1);


		float but_gadget_size = ImGui::GetFontSize()*10.0;
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));

		if (ImGui::StyleButton("Randomize All Bushes##TerrainBushes", ImVec2(but_gadget_size, 0)))
		{
			//ggtrees_global_params.paint_density //PE: Perhaps make this random ?
			GGTrees::GGTrees_RepopulateInstances(); //PE: Needed to get new random positions.
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("All bushes in the level will have their position and rotation randomized");

		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
		if (ImGui::StyleButton("Clear All Bushes##TerrainBushes", ImVec2(but_gadget_size, 0)))
		{
			int iAction = askBoxCancel("This will delete all your bushes, are you sure?", "Confirmation"); //1==Yes 2=Cancel 0=No
			if (iAction == 1)
			{
				GGTrees::GGTrees_HideAll();
			}
		}

		ImRect bbwin(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize());
		if (ImGui::IsMouseHoveringRect(bbwin.Min, bbwin.Max))
		{
			bImGuiGotFocus = true;
		}

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
	}
}

#endif

std::vector<cstr> grass_selections;
std::vector<cstr> grass_selections_text;
void init_grass_selections()
{
	grass_selections.clear();
	grass_selections.push_back("grassbank\\grass short dead_color.dds");
	grass_selections.push_back("grassbank\\grass short_color.dds");
	grass_selections.push_back("grassbank\\grass tall dead_color.dds");
	grass_selections.push_back("grassbank\\grass tall_color.dds");
	grass_selections.push_back("grassbank\\short grass 2 dead_color.dds");
	grass_selections.push_back("grassbank\\short grass 2_color.dds");
	grass_selections.push_back("grassbank\\short grass 3 dead_color.dds");
	grass_selections.push_back("grassbank\\short grass 3_color.dds");
	grass_selections.push_back("grassbank\\short grass 4 dead_color.dds");
	grass_selections.push_back("grassbank\\short grass 4_color.dds");
	grass_selections.push_back("grassbank\\short grass 5 dead_color.dds");
	grass_selections.push_back("grassbank\\short grass 5_color.dds");
	grass_selections.push_back("grassbank\\tall grass 2 dead_color.dds");
	grass_selections.push_back("grassbank\\tall grass 2_color.dds");
	grass_selections.push_back("grassbank\\tall grass 3 dead_color.dds");
	grass_selections.push_back("grassbank\\tall grass 3_color.dds");
	for (int i = 0; i < grass_selections.size(); i++)
	{
		char pFileOnly[MAX_PATH];
		strcpy(pFileOnly, grass_selections[i].Get());
		for (int n = strlen(pFileOnly) - 1; n > 0; n--)
		{
			if (pFileOnly[n] == '\\' || pFileOnly[n] == '/')
			{
				strcpy(pFileOnly, pFileOnly + n + 1);
				break;
			}
		}
		char *remove_ext = (char *) pestrcasestr( pFileOnly , "_color.dds");
		if (remove_ext)
			*remove_ext = 0;
		grass_selections_text.push_back(pFileOnly);
	}
}


void imgui_Customize_Vegetation(int mode)
{
	int wflags = ImGuiTreeNodeFlags_DefaultOpen;
	if (mode == 1) wflags = ImGuiTreeNodeFlags_None;
	if (pref.bAutoClosePropertySections && mode == 1 && iLastOpenHeader != 3)
		ImGui::SetNextItemOpen(false, ImGuiCond_Always);

	float w = ImGui::GetWindowContentRegionWidth();

	//Customize Vegetation
	if (ImGui::StyleCollapsingHeader("Palette", wflags)) {

		if (mode == 1) iLastOpenHeader = 3;

		ImGui::Indent(10);
		ImGui::PushItemWidth(-10);

		#ifdef WICKEDENGINE
		if (iDeleteSingleGrassTextures > 0) 
		{
			if (ImageExist(iDeleteSingleGrassTextures))
				DeleteImage(iDeleteSingleGrassTextures);
			iDeleteSingleGrassTextures = 0;
		}
		if (iDeleteAllGrassTextures) 
		{
			iDeleteAllGrassTextures = false;
			for (int iL = 0; iL < 16; iL++) {
				if (ImageExist(t.terrain.imagestartindex + 180 + iL))
					DeleteImage(t.terrain.imagestartindex + 180 + iL);
			}
			bUpdateGrassMaterials = true;
		}
		if (t.visuals.sGrassTextures[0] == "")
		{
			//PE: @Lee-Grass Define default palette here. Also need to update t.gamevisuals :)

			init_grass_selections(); //PE: Set defaults for selection window.

			t.visuals.sGrassTextures[0] = "grassbank\\grass short dead_color.dds";
			t.visuals.sGrassTexturesName[0] = "Grass short dead";
			t.gamevisuals.sGrassTextures[0] = t.visuals.sGrassTextures[0];
			t.gamevisuals.sGrassTexturesName[0] = t.visuals.sGrassTexturesName[0];
			t.visuals.sGrassTextures[1] = "grassbank\\grass short_color.dds";
			t.visuals.sGrassTexturesName[1] = "Grass short";
			t.gamevisuals.sGrassTextures[1] = t.visuals.sGrassTextures[1];
			t.gamevisuals.sGrassTexturesName[1] = t.visuals.sGrassTexturesName[1];
			t.visuals.sGrassTextures[2] = "grassbank\\grass tall dead_color.dds";
			t.visuals.sGrassTexturesName[2] = "Grass tall dead";
			t.gamevisuals.sGrassTextures[2] = t.visuals.sGrassTextures[2];
			t.gamevisuals.sGrassTexturesName[2] = t.visuals.sGrassTexturesName[2];
			t.visuals.sGrassTextures[3] = "grassbank\\grass tall_color.dds";
			t.visuals.sGrassTexturesName[3] = "Grass tall";
			t.gamevisuals.sGrassTextures[3] = t.visuals.sGrassTextures[3];
			t.gamevisuals.sGrassTexturesName[3] = t.visuals.sGrassTexturesName[3];
			t.visuals.sGrassTextures[4] = "grassbank\\short grass 2 dead_color.dds";
			t.visuals.sGrassTexturesName[4] = "Grass 2 dead";
			t.gamevisuals.sGrassTextures[4] = t.visuals.sGrassTextures[4];
			t.gamevisuals.sGrassTexturesName[4] = t.visuals.sGrassTexturesName[4];
			t.visuals.sGrassTextures[5] = "grassbank\\short grass 2_color.dds";
			t.visuals.sGrassTexturesName[5] = "Short grass 2";
			t.gamevisuals.sGrassTextures[5] = t.visuals.sGrassTextures[5];
			t.gamevisuals.sGrassTexturesName[5] = t.visuals.sGrassTexturesName[5];
			t.visuals.sGrassTextures[6] = "grassbank\\short grass 3 dead_color.dds";
			t.visuals.sGrassTexturesName[6] = "Grass 3 dead";
			t.gamevisuals.sGrassTextures[6] = t.visuals.sGrassTextures[6];
			t.gamevisuals.sGrassTexturesName[6] = t.visuals.sGrassTexturesName[6];
			t.visuals.sGrassTextures[7] = "grassbank\\short grass 3_color.dds";
			t.visuals.sGrassTexturesName[7] = "Short grass 3";
			t.gamevisuals.sGrassTextures[7] = t.visuals.sGrassTextures[7];
			t.gamevisuals.sGrassTexturesName[7] = t.visuals.sGrassTexturesName[7];

			// blank rest to allow more room for UI stuff, and user can always add more!
			for (int iBlankRest = 8; iBlankRest < 16; iBlankRest++)
			{
				t.visuals.sGrassTextures[iBlankRest] = "";
				t.visuals.sGrassTexturesName[iBlankRest] = "";
				t.gamevisuals.sGrassTextures[iBlankRest] = t.visuals.sGrassTextures[iBlankRest];
				t.gamevisuals.sGrassTexturesName[iBlankRest] = t.visuals.sGrassTextures[iBlankRest];
			}

			// save out reset grass plate choices
			visuals_save ( );

			// reset grass type choices to paint with
			grass_resetchoices();
		}

		// find last slot used
		int iLastSlotUsed = 0;
		for (int iL = 0; iL < 16; iL++)
			if (t.visuals.sGrassTextures[iL] != "")
				if (ImageExist(t.terrain.imagestartindex + 180 + iL))
					iLastSlotUsed = iL;

		int iUsedImages = 0;
		float col_start = 90.0f;
		int iSelectGrassTexture = -1;
		cStr sGrassTextureFolder = g.rootdir_s + "grassbank\\";


#ifdef DISPLAY4x4
		ImGui::Indent(-10);
		ImGui::Indent(4);
		ImGui::Columns(4, "##veg4x4columns", false);  //false no border
#endif

		for (int iL = 0; iL < 16; iL++) 
		{
			if (t.visuals.sGrassTextures[iL] != "")
			{
				if (!ImageExist(t.terrain.imagestartindex + 180 + iL))
				{
					//Load in image.
					image_setlegacyimageloading(true);
					SetMipmapNum(1); //PE: mipmaps not needed.
					//t.terrain.imagestartindex = 63600
					if (ImageExist(t.terrain.imagestartindex + 180 + iL) == 1) DeleteImage(t.terrain.imagestartindex + 180 + iL);
					LoadImage(t.visuals.sGrassTextures[iL].Get(), t.terrain.imagestartindex + 180 + iL, 0, g.gdividetexturesize);
					if (ImageExist(t.terrain.imagestartindex + 180 + iL) == 1)
					{
						sGrassTexturesID[iL] = t.terrain.imagestartindex + 180 + iL;
					}
					else
					{
						//Load failed, clear texture slot.
						t.visuals.sGrassTextures[iL] = "";
						t.gamevisuals.sGrassTextures[iL] = t.visuals.sGrassTextures[iL];
						g.projectmodified = 1;
						bUpdateGrassMaterials = true;
					}
					SetMipmapNum(-1);
					image_setlegacyimageloading(false);
				}
				else
				{
					sGrassTexturesID[iL] = t.terrain.imagestartindex + 180 + iL;
				}
				if (ImageExist(sGrassTexturesID[iL]))
				{
					iUsedImages++;



					if (bGrassNameWindow[iL]) {
						//Ask for a proper name of veg.
						ImGui::SetNextWindowSize(ImVec2(26 * ImGui::GetFontSize(), 32 * ImGui::GetFontSize()), ImGuiCond_Once);
						ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
						cstr sUniqueWinName = cstr("Vegetation Name##ttn") + cstr(iL);
						ImGui::Begin(sUniqueWinName.Get(), &bGrassNameWindow[iL], 0);
						ImGui::Indent(10);
						static char NewTextureName[256];
						cstr sUniqueInputName = cstr("##InputVegetationName") + cstr(iL);
						float content_width = ImGui::GetContentRegionAvailWidth() - 10.0;
						ImGui::ImgBtn(sGrassTexturesID[iL], ImVec2(content_width, content_width), ImColor(0, 0, 0, 255));
						ImGui::PushItemWidth(-10);
						ImGui::Text("Enter a name for your vegetation:");
						
						if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
							ImGui::SetKeyboardFocusHere(0);

						if (ImGui::InputText(sUniqueInputName.Get(), t.visuals.sGrassTexturesName[iL].Get(), 250, ImGuiInputTextFlags_EnterReturnsTrue)) {
							t.gamevisuals.sGrassTexturesName[iL] = t.visuals.sGrassTexturesName[iL];
							bGrassNameWindow[iL] = false;
						}
						#ifdef WICKEDENGINE
						if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;
						#endif

						ImGui::PopItemWidth();
						ImGui::Indent(-10);
						if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
							//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
							ImGui::Text("");
							ImGui::Text("");
						}
						bImGuiGotFocus = true;
						ImGui::End();
					}


#ifndef DISPLAY4x4
					if (iUsedImages < 16 && iL == 0)
					{
						float but_gadget_size = ImGui::GetFontSize()*10.0;
						ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));

						if (ImGui::StyleButton("Add Texture##Grass", ImVec2(but_gadget_size, 0)))
						{
							iSelectGrassTexture = 99;
						}
						ImGui::Separator();
					}
#endif
					float path_gadget_size = ImGui::GetFontSize()*3.0;
					int preview_icon_size = ImGui::GetFontSize();

					//PE: You can only change image slot 0 , not delete it, we need at least one texture.
					bool bDeleteImage = false;
					if (iL != 0 && iL == iLastSlotUsed) bDeleteImage = true;

					// chop grassbank from material name 
					cstr materialname = t.visuals.sGrassTexturesName[iL];

					cStr sLabel = cStr("##InputGrassTexture") + cStr(iL);

					//PE: New do 4x4 coloums here.
#ifdef DISPLAY4x4
					int iLargerPreviewIconSize = 28;//PE: 54 , now lowest possible icon
					float control_width = (iLargerPreviewIconSize + 3.0) * 4.0f + 6.0;

					if (w > control_width) {
						//PE: fit perfectly with window width.
						iLargerPreviewIconSize = (w - 20.0) / 4.0;
						iLargerPreviewIconSize -= 6.0; //Padding.
						if (iLargerPreviewIconSize > 70) iLargerPreviewIconSize = 70;
					}

					ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;

					if (sGrassTexturesID[iL] > 0)
					{
						cStr sLabelChild = cStr("##grass4x4") + cStr(iL);

						ImVec2 content_avail = { iLargerPreviewIconSize + 1.0f ,iLargerPreviewIconSize + 1.0f };

						//style.WindowPadding
						ImVec2 oldstyle = ImGui::GetStyle().FramePadding;
						ImGui::GetStyle().FramePadding = { 0,0 };
						ImGui::BeginChild(sLabelChild.Get(), content_avail,false, ImGuiWindowFlags_NoScrollbar);

						if (ImGui::ImgBtn(sGrassTexturesID[iL], ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize), ImColor(0, 0, 0, 255)))
						{
							//iSelectGrassTexture = iL;
							//Now do toggle selection.
							if (bCurrentGrassTextureForPaint[iL])
								bCurrentGrassTextureForPaint[iL] = false;
							else
								bCurrentGrassTextureForPaint[iL] = true;

						}

						bool bInContext = false;
						static int iCurrentContext = -1;
						if ( iCurrentContext == -1 || iCurrentContext == iL )
						{
							if (ImGui::BeginPopupContextWindow())
							{
								iCurrentContext = iL;
								bInContext = true;
								if (ImGui::MenuItem("Change Vegetation"))
								{
									iSelectGrassTexture = iL;
									iCurrentContext = -1;
								}
								if (ImGui::MenuItem("Change Vegetation Name"))
								{
									bGrassNameWindow[iL] = true;
									iCurrentContext = -1;
								}
								if (bCurrentGrassTextureForPaint[iL])
								{
									if (ImGui::MenuItem("Unselect Vegetation"))
									{
										bCurrentGrassTextureForPaint[iL] = false;
									}
								}
								else {
									if (ImGui::MenuItem("Select Vegetation"))
									{
										bCurrentGrassTextureForPaint[iL] = true;
									}
								}
								if (iL != 0)
								{
									if (ImGui::MenuItem("Delete Vegetation"))
									{
										t.visuals.sGrassTextures[iL] = ""; //delete image in next run.
										t.gamevisuals.sGrassTextures[iL] = t.visuals.sGrassTextures[iL];
										g.projectmodified = 1;
										bUpdateGrassMaterials = true;
										iCurrentContext = -1;
									}
								}
								//ImGui::TextCenter(materialname.Get());
								ImGui::EndPopup();
							}
							else {
								iCurrentContext = -1;
							}
						}
						if (!bInContext && ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::ImgBtn(sGrassTexturesID[iL], ImVec2(350, 350), ImColor(0, 0, 0, 255), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false,false,true);
							ImGui::TextCenter(materialname.Get());
							ImGui::Separator();
							ImGui::EndTooltip();
						}

						ImGui::EndChild();
						ImGui::GetStyle().FramePadding = oldstyle;

					}
					sLabel = cStr("##veg") +  cStr(iL);

					float checkwidth = ImGui::GetFontSize()*1.5;
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((ImGui::GetContentRegionAvail().x*0.5) - (checkwidth*0.5), 0.0f));
					//ImGui::Checkbox(sLabel.Get(), &bCurrentGrassTextureForPaint[iL]);

					if (bCurrentGrassTextureForPaint[iL]) //sTerrainTexturesID[iL] - t.terrain.imagestartindex - 80)
					{
						ImVec2 padding = { 2.0, 2.0 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}


					ImGui::NextColumn();

#else
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

					//PE: @Lee-Grass bCurrentGrassTextureForPaint[iL] will be true if you need to use it when painting.
					ImGui::Checkbox(sLabel.Get(), &bCurrentGrassTextureForPaint[iL]);

					ImGui::SameLine();
					//ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY() - 3));
					//ImGui::SetCursorPos(ImVec2(col_start-40, ImGui::GetCursorPosY()));
					int iSmallPreviewYMargin = preview_icon_size / 2;
					int iLargerPreviewIconSize = preview_icon_size * 2;
					ImGui::SetCursorPos(ImVec2(col_start-40, ImGui::GetCursorPosY()-iSmallPreviewYMargin));

					if (sGrassTexturesID[iL] > 0)
					{
						if (ImGui::ImgBtn(sGrassTexturesID[iL], ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize), ImColor(0, 0, 0, 255)))
						{
							iSelectGrassTexture = iL;
						}
						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::ImgBtn(sGrassTexturesID[iL], ImVec2(180, 180), ImColor(0, 0, 0, 255));
							ImGui::EndTooltip();
						}
						ImGui::SameLine();
					}

					int iInputFlags = ImGuiInputTextFlags_EnterReturnsTrue;
					if (!bDeleteImage)
						iInputFlags = ImGuiInputTextFlags_ReadOnly;

					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY()+iSmallPreviewYMargin));
					sLabel = cStr(" ##InputGrassTexture") + cStr(iL);
					ImGui::PushItemWidth(-10 - path_gadget_size);
					if (ImGui::InputText(sLabel.Get(), &materialname[0], 250, iInputFlags))
					{
						if (strlen(materialname) == 0) t.visuals.sGrassTextures[iL] = ""; //delete image in next run.
						t.gamevisuals.sGrassTextures[iL] = t.visuals.sGrassTextures[iL];
						g.projectmodified = 1;
						bUpdateGrassMaterials = true;
					}
					ImGui::PopItemWidth();

					ImGui::SameLine();
					ImGui::PushItemWidth(path_gadget_size);
					sLabel = cStr("...##InputGrassTexture") + cStr(iL);

					if (ImGui::StyleButton(sLabel.Get()))
						iSelectGrassTexture = iL;

					ImGui::PopItemWidth();

					if (bDeleteImage)
					{
						ImGui::SameLine();
						sLabel = cStr("X##InputGrassTexture") + cStr(iL);
						if (ImGui::StyleButton(sLabel.Get()))
						{
							t.visuals.sGrassTextures[iL] = ""; //delete image in next run.
							t.gamevisuals.sGrassTextures[iL] = t.visuals.sGrassTextures[iL];
							bCurrentGrassTextureForPaint[iL] = false;
							g.projectmodified = 1;
							bUpdateGrassMaterials = true;
							visuals_save();
						}
					}

					// squish together as it looks better - can see more grass choices
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY()-iSmallPreviewYMargin*2));
#endif
				}
			}
			else 
			{
				//Delete old image.
				if (ImageExist(t.terrain.imagestartindex + 180 + iL) == 1) DeleteImage(t.terrain.imagestartindex + 180 + iL);
				sGrassTexturesID[iL] = 0;
				bCurrentGrassTextureForPaint[iL] = false;
			}
		}

#ifdef DISPLAY4x4
		ImGui::Indent(-4);
		ImGui::Columns(1);
		ImGui::Indent(10);

		if (iUsedImages < 16 )
		{
			//ImGui::Separator();
			float but_gadget_size = ImGui::GetFontSize()*10.0;
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));

			if (ImGui::StyleButton("Add New Vegetation##Grass", ImVec2(but_gadget_size, 0)))
			{
				iSelectGrassTexture = 99;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Add New Vegetation");

			//ImGui::Separator();
		}
#endif

		static bool grass_selection_window = false;
		static int iSelectedGrassTexture = -1;
		if (iSelectGrassTexture >= 0)
		{
			iSelectedGrassTexture = iSelectGrassTexture;
			grass_selection_window = true;
			iSelectGrassTexture = 0;
		}
		iSelectGrassTexture = imgui_get_selections(grass_selections, grass_selections_text, 1, 280, &grass_selection_window);

		if (iSelectGrassTexture >= 0 && iSelectedGrassTexture >= 0)
		{
			bool bNeedFileSelector = false;
			if (iSelectGrassTexture == 999)
				bNeedFileSelector = true;

			grass_selection_window = false;

			int iNewGrassIndex = iSelectGrassTexture;
			iSelectGrassTexture = iSelectedGrassTexture;
			if (iSelectGrassTexture == 99)
			{
				//Find free slot.
				for (int iL2 = 0; iL2 < 32; iL2++) {
					if (t.visuals.sGrassTextures[iL2] == "")
					{
						iSelectGrassTexture = iL2;
						break;
					}
				}
			}

			if (!bNeedFileSelector) {
				t.visuals.sGrassTextures[iSelectGrassTexture] = grass_selections[iNewGrassIndex];
				if (ImageExist(t.terrain.imagestartindex + 180 + iSelectGrassTexture))
					iDeleteSingleGrassTextures = t.terrain.imagestartindex + 180 + iSelectGrassTexture;
				bUpdateGrassMaterials = true;
				t.visuals.sGrassTexturesName[iSelectGrassTexture] = grass_selections_text[iNewGrassIndex];
				bGrassNameWindow[iSelectGrassTexture] = true;
				bCurrentGrassTextureForPaint[iSelectGrassTexture] = true; //PE: Select new added texture.
			}

			if (bNeedFileSelector)
			{
				cStr tOldDir = GetDir();
				char * cFileSelected;
				//cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "All\0*.*\0DDS\0*_color.dds\0PNG\0*_color.png\0JPEG\0*_color.jpg\0TGA\0*_color.tga\0BMP\0*_color.bmp\0\0\0", sGrassTextureFolder.Get(), NULL);
				cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "DDS\0*_color.dds\0\0\0", sGrassTextureFolder.Get(), NULL);
				SetDir(tOldDir.Get());

				if (cFileSelected && strlen(cFileSelected) > 0)
				{
					char *relonly = (char *)pestrcasestr(cFileSelected, g.rootdir_s.Get());
					t.visuals.sGrassTextures[iSelectGrassTexture] = cFileSelected;
					t.visuals.sGrassTexturesName[iSelectGrassTexture] = "";
					if (relonly)
					{
						t.visuals.sGrassTextures[iSelectGrassTexture] = cFileSelected + g.rootdir_s.Len();
					}
					g.projectmodified = 1;
					t.gamevisuals.sGrassTextures[iSelectGrassTexture] = t.visuals.sGrassTextures[iSelectGrassTexture];
					bUpdateGrassMaterials = true;

					bGrassNameWindow[iSelectGrassTexture] = true;

					//PE: Reload image.
					if (ImageExist(t.terrain.imagestartindex + 180 + iSelectGrassTexture))
						iDeleteSingleGrassTextures = t.terrain.imagestartindex + 180 + iSelectGrassTexture;

					bCurrentGrassTextureForPaint[iSelectGrassTexture] = true; //PE: Select new added texture.

				}
			}
			iSelectGrassTexture = -1;
		}

		if (bUpdateGrassMaterials)
		{
			//@Lee-Grass update super palette.
			for (int iL = 0; iL < 16; iL++) 
			{
				if (t.visuals.sGrassTextures[iL] != "") 
				{
					if (sGrassChangedTextures[iL] != t.visuals.sGrassTextures[iL]) 
					{
						//Add t.visuals.sGrassTextures[iL] to palette in slot iL
						sGrassChangedTextures[iL] = t.visuals.sGrassTextures[iL];

						// the chosen grass file
						char pGrassSrcWorkFile[MAX_PATH];
						strcpy(pGrassSrcWorkFile, sGrassChangedTextures[iL].Get());
						pGrassSrcWorkFile[strlen(pGrassSrcWorkFile) - strlen("_color.dds")] = 0;
						char pGrassSrcFile[MAX_PATH];
						strcpy(pGrassSrcFile, pGrassSrcWorkFile + strlen("grassbank\\"));

						char pChosenGrassSrcFile[MAX_PATH];
						strcpy(pChosenGrassSrcFile, pGrassSrcFile);

						// determine grass texture filenames to insert (i.e. take ' dead' from 'grass short dead' and put in pChosenGrassSrcFile as 'grass short')
						char pFullGrassSrcFile[MAX_PATH];
						strcpy(pFullGrassSrcFile, pChosenGrassSrcFile);
						for (int n = strlen(pFullGrassSrcFile) - 1; n > 0; n--)
						{
							if (pFullGrassSrcFile[n] == ' ')
							{
								pChosenGrassSrcFile[n] = 0;
								break;
							}
						}
						/* not needed
						bool bHasNormalAndSurface = false;
						char pTestIfHasNormalAndSurface[MAX_PATH];
						strcpy(pTestIfHasNormalAndSurface, g.fpscrootdir_s.Get());
						strcat(pTestIfHasNormalAndSurface, "\\Files\\grassbank\\");
						strcat(pTestIfHasNormalAndSurface, pChosenGrassSrcFile);
						strcat(pTestIfHasNormalAndSurface, "_normal.dds");
						if (FileExist(pTestIfHasNormalAndSurface) == 1)
						{
							strcpy(pTestIfHasNormalAndSurface, g.fpscrootdir_s.Get());
							strcat(pTestIfHasNormalAndSurface, "\\Files\\grassbank\\");
							strcat(pTestIfHasNormalAndSurface, pChosenGrassSrcFile);
							strcat(pTestIfHasNormalAndSurface, "_surface.dds");
							if (FileExist(pTestIfHasNormalAndSurface) == 1)
							{
								bHasNormalAndSurface = true;
							}
						}
						if (bHasNormalAndSurface == false)
						{
							// turns out the reduced _color texture had no equivilant normal or surface texture
							strcpy(pChosenGrassSrcFile, pFullGrassSrcFile);
						}
						*/

						// replace grass image in grass plate
						bool bGrassPlateChanged = false;
						//for (int iGrassTexSet = 0; iGrassTexSet < 3; iGrassTexSet++) // no more normal or surface
						for (int iGrassTexSet = 0; iGrassTexSet < 1; iGrassTexSet++)
						{
							// determine destination grass plate
							char pDestTerrainTextureFile[MAX_PATH];
							strcpy(pDestTerrainTextureFile, g.fpscrootdir_s.Get());
							strcat(pDestTerrainTextureFile, "\\Files\\levelbank\\testmap\\grass");
							if (iGrassTexSet == 0) strcat(pDestTerrainTextureFile, "_coloronly.dds");
							//if (iGrassTexSet == 1) strcat(pDestTerrainTextureFile, "_normal.dds");
							//if (iGrassTexSet == 2) strcat(pDestTerrainTextureFile, "_surface.dds");

							// construct grass texture filenames to insert
							char pTexFileToLoad[MAX_PATH];
							strcpy(pTexFileToLoad, g.fpscrootdir_s.Get());
							strcat(pTexFileToLoad, "\\Files\\grassbank\\");
							if (iGrassTexSet == 0)
							{
								// allows an additional word before _color so can use same normal and surface texture maps
								strcat(pTexFileToLoad, pFullGrassSrcFile);
							}
							else
							{
								// possibly truncated for normal and surface loading
								strcat(pTexFileToLoad, pChosenGrassSrcFile);
							}
							if (iGrassTexSet == 0) strcat(pTexFileToLoad, "_color.dds");
							//if (iGrassTexSet == 1) strcat(pTexFileToLoad, "_normal.dds");
							//if (iGrassTexSet == 2) strcat(pTexFileToLoad, "_surface.dds");

							// do the insert
							int iGrassType = iL;
							if (ImageCreateTexturePlate(pDestTerrainTextureFile, iGrassType, pTexFileToLoad, 1, 1) == 1)
							{
								// success
								bGrassPlateChanged = true;
							}
						}
						if (bGrassPlateChanged == true)
						{
							// must remove pre-stored reference to any previous grass_color texture set
							WickedCall_DeleteImage("levelbank\\testmap\\grass_coloronly.dds");
							//WickedCall_DeleteImage("levelbank\\testmap\\grass_normal.dds");
							//WickedCall_DeleteImage("levelbank\\testmap\\grass_surface.dds");

							// Reload grass plate textures on all grass objects
							grass_setgrassimage();
						}
					}
				}
			}
			bUpdateGrassMaterials = false;
		}
		#else
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
		char * current_veg = NULL;// t.vegstylebank_s[g.vegstyleindex].Get();
		if (g.vegstyleindex <= g.vegstylemax) current_veg = t.vegstylebank_s[g.vegstyleindex].Get();
		if (!current_veg) current_veg = "NA";
		if (ImGui::BeginCombo("##SelectVegetationCombo", current_veg)) // The second parameter is the label previewed before opening the combo.
		{

			for (int vegindex = 1; vegindex <= g.vegstylemax; vegindex++)
			{

				if (t.vegstylebank_s[vegindex].Len() > 0)
				{
					bool is_selected = false;
					if (t.vegstylebank_s[vegindex].Get() == current_veg)
						is_selected = true;
					if (ImGui::Selectable(t.vegstylebank_s[vegindex].Get(), is_selected)) {


						//Test display veg.
						//grass_setgrassgridandfade();
						//grass_init();
						//t.completelyfillvegarea = 1;
						t.terrain.grassupdateafterterrain = 1;
						grass_loop();
						t.terrain.grassupdateafterterrain = 0;

						g.projectmodified = 1;
						//	current_veg = t.terrainstylebank_s[vegindex].Get(); a bug?
						current_veg = t.vegstylebank_s[vegindex].Get();

						g.vegstyleindex = vegindex;
						g.vegstyle_s = t.vegstylebank_s[g.vegstyleindex];

						t.visuals.vegetationindex = g.vegstyleindex;
						t.visuals.vegetation_s = t.vegstylebank_s[g.vegstyleindex];;
						t.gamevisuals.vegetationindex = t.visuals.vegetationindex;
						t.gamevisuals.vegetation_s = t.visuals.vegetation_s;
						grass_changevegstyle();
						bUpdateVeg = true;
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		#endif

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
		#ifdef WICKEDENGINE
		// no need to toggle veg, we ALWAYS 'may' need it!
		bEnableVeg = true;
		#else
		//ImGui::Text("Display Vegetation:");
		//ImGui::SameLine();
		//ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
		//ImGui::SetCursorPos(ImVec2(136, ImGui::GetCursorPosY()));
		if (ImGui::Checkbox("Display Vegetation##DisplayVeg", &bEnableVeg)) {
			if (bEnableVeg) {
				iLastUpdateVeg = 0;
				bUpdateVeg = true;
			}
		}
		#endif
#ifndef WICKEDENGINE
		if (bEnableVeg) 
		{
			int iTextRightPos = 116; //136
			//PE: Now use new RangeSlider.
			/*
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
			ImGui::Text("Spray Minimum:");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
			ImGui::SetCursorPos(ImVec2(iTextRightPos, ImGui::GetCursorPosY()));
			ImGui::PushItemWidth(-10);
			if (ImGui::SliderFloat("##VegRandomMin", &g_fvegRandomMin, 0.0, 100.0))
			{
				if (g_fvegRandomMin > g_fvegRandomMax) g_fvegRandomMax = g_fvegRandomMin;
			}
			ImGui::PopItemWidth();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
			ImGui::Text("Spray Maximum:");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
			ImGui::SetCursorPos(ImVec2(iTextRightPos, ImGui::GetCursorPosY()));
			ImGui::PushItemWidth(-10);
			if (ImGui::SliderFloat("##VegRandomMax", &g_fvegRandomMax, 0.0, 100.0))
			{
				if (g_fvegRandomMax < g_fvegRandomMin) g_fvegRandomMin = g_fvegRandomMax;
			}
			ImGui::PopItemWidth();
			*/
		
			ImGui::PushItemWidth(-10);
			static float fval1=20.0, fval2 = 80.0;
			ImVec2 vOldPos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
			ImGui::Text("Height:");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
			ImGui::SetCursorPos(ImVec2(iTextRightPos, ImGui::GetCursorPosY()));
			ImGui::RangeSlider("##VegHeight", g_fvegRandomMin, g_fvegRandomMax, 100.0f);
			ImGui::SetCursorPos(ImVec2(vOldPos.x, ImGui::GetCursorPosY()+9));
			ImGui::PopItemWidth();
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Vegetation Draw Height Range");


			//PE: These is still Overall and not when spraying, so keep them as is for now.
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
			ImGui::Text("Overall Quantity:");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
			ImGui::SetCursorPos(ImVec2(iTextRightPos, ImGui::GetCursorPosY()));
			ImGui::PushItemWidth(-10);
			if (ImGui::SliderFloat("##VegOverallQuantity", &t.gamevisuals.VegQuantity_f, 0.0, 100.0))
			{
				iLastUpdateVeg = Timer();
				bUpdateVeg = true;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Overall Vegetation Quantity");
			ImGui::PopItemWidth();

			//PE: Overall Height: is not used anymore in MAX.
#ifndef WICKEDENGINE
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
			ImGui::Text("Overall Height:");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
			ImGui::SetCursorPos(ImVec2(iTextRightPos, ImGui::GetCursorPosY()));
			ImGui::PushItemWidth(-10);
			if (ImGui::SliderFloat("##VegOverallHeight", &t.gamevisuals.VegHeight_f, 0.0, 100.0))
			{
				iLastUpdateVeg = Timer();
				bUpdateVeg = true;
			}
			ImGui::PopItemWidth();
#endif

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
			ImGui::Text("Overall Width:");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
			ImGui::SetCursorPos(ImVec2(iTextRightPos, ImGui::GetCursorPosY()));
			ImGui::PushItemWidth(-10);
			if (ImGui::SliderFloat("##VegOverallWidth", &t.gamevisuals.VegWidth_f, 0.0, 100.0))
			{
				iLastUpdateVeg = Timer();
				bUpdateVeg = true;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Overall Vegetation Width");
			ImGui::PopItemWidth();
		}
#endif

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
		ImGui::Indent(-10);
	}
}


void imgui_Customize_Vegetation_v3(int mode)
{
	int wflags = ImGuiTreeNodeFlags_DefaultOpen;
	if (mode == 1) wflags = ImGuiTreeNodeFlags_None;
	if (pref.bAutoClosePropertySections && mode == 1 && iLastOpenHeader != 3)
		ImGui::SetNextItemOpen(false, ImGuiCond_Always);

	float w = ImGui::GetWindowContentRegionWidth();

	//Customize Vegetation
	if (ImGui::StyleCollapsingHeader("Palette", wflags)) {

		if (mode == 1) iLastOpenHeader = 3;

		ImGui::Indent(10);
		ImGui::PushItemWidth(-10);


		if (iDeleteSingleGrassTextures > 0)
		{
			if (ImageExist(iDeleteSingleGrassTextures))
				DeleteImage(iDeleteSingleGrassTextures);
			iDeleteSingleGrassTextures = 0;
		}
		if (iDeleteAllGrassTextures)
		{
			iDeleteAllGrassTextures = false;
			for (int iL = 0; iL < GGGRASS_NUM_SELECTABLE_TYPES; iL++) {
				if (ImageExist(t.terrain.imagestartindex + 180 + iL))
					DeleteImage(t.terrain.imagestartindex + 180 + iL);
			}
			bUpdateGrassMaterials = true;
		}
		bool bInitNewGrassSystem = true;
		//PE: @Paul We need this check to be able to update grass available in saved .fpm files (loaded .fpm after init) , as we save all information to be able to support "custom" grass from users.
		//PE: So we need someting unique like grassbank/course grass_mat1_SF_1.15.dds to go into the first slot :)
		if (bInitNewGrassSystem || t.visuals.sGrassTextures[0] == "" || t.visuals.sGrassTextures[0] != "grassbank/course grass_mat1_SF_1.15.dds")
		{
			char grassFilename[ 256 ];
			uint32_t currMat = gggrass_global_params.paint_material;
			for (uint32_t i = 0; i < GGGRASS_NUM_SELECTABLE_TYPES; i++)
			{
				strcpy_s( grassFilename, "grassbank/" );
				strcat_s( grassFilename, GGGrass_GetTextureFilename(currMat, i) );

				t.visuals.sGrassTextures[i] = grassFilename;
				t.visuals.sGrassTexturesName[i] = GGGrass_GetTextureShortName(currMat, i);
				t.gamevisuals.sGrassTextures[i] = t.visuals.sGrassTextures[i];
				t.gamevisuals.sGrassTexturesName[i] = t.visuals.sGrassTexturesName[i];
			}
			bInitNewGrassSystem = false;
		}

		// find last slot used
		int iLastSlotUsed = 0;
		for (int iL = 0; iL < GGGRASS_NUM_SELECTABLE_TYPES; iL++)
			if (t.visuals.sGrassTextures[iL] != "")
				if (ImageExist(t.terrain.imagestartindex + 180 + iL))
					iLastSlotUsed = iL;

		int iUsedImages = 0;
		float col_start = 90.0f;
		int iSelectGrassTexture = -1;

		ImGui::Indent(-10);


		struct icon_selections
		{
			bool bActive;
			ImRect image_bb;
		} draw_selections[128];
		for (int iL = 0; iL < GGGRASS_NUM_SELECTABLE_TYPES; iL++) draw_selections[iL].bActive = false;

		//Child
		static float fContentHeight = 0;
		static ImVec2 vLastRunHeight = { 0,0 };
		if (fContentHeight <= 85) {
			fContentHeight = 85; //One line default. and prevent flicker.
		}
		vLastRunHeight = { 0 ,fContentHeight };
		int iActiveGrass = 0;
		ImVec2 oldstylemain = ImGui::GetStyle().FramePadding;
		ImVec2 oldwinstylemain = ImGui::GetStyle().WindowPadding;
		ImGui::GetStyle().WindowPadding = { 0,0 };
		ImGui::GetStyle().FramePadding = { 0,0 };
		ImVec2 child_begin = ImGui::GetCursorPos();
		ImVec2 vWindowPos = ImGui::GetWindowPos();
		ImVec2 vWindowSize = ImGui::GetWindowSize();
		ImGui::BeginChild("##grass4x4forscrollbar", vLastRunHeight, false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 3));

		//ImGui::Indent(4);
		ImGui::Columns(4, "##veg4x4columns", false);  //false no border

		float curposy = ImGui::GetCursorPosY();
		uint64_t values = gggrass_global_params.paint_type;
		for (int iL = 0; iL < GGGRASS_NUM_SELECTABLE_TYPES; iL++)
		{
			uint64_t mask = 1ULL << iL;
			if (t.visuals.sGrassTextures[iL] != "")
			{
				if (iActiveGrass++ <= 20) //record buttom
					fContentHeight = ImGui::GetCursorPosY() - curposy;

				if (!ImageExist(t.terrain.imagestartindex + 180 + iL))
				{
					//Load in image.
					image_setlegacyimageloading(true);
					SetMipmapNum(1); //PE: mipmaps not needed.
					//t.terrain.imagestartindex = 63600
					if (ImageExist(t.terrain.imagestartindex + 180 + iL) == 1) DeleteImage(t.terrain.imagestartindex + 180 + iL);
					LoadImage(t.visuals.sGrassTextures[iL].Get(), t.terrain.imagestartindex + 180 + iL, 0, g.gdividetexturesize);
					if (ImageExist(t.terrain.imagestartindex + 180 + iL) == 1)
					{
						sGrassTexturesID[iL] = t.terrain.imagestartindex + 180 + iL;
					}
					else
					{
						//Load failed, clear texture slot.
						t.visuals.sGrassTextures[iL] = "";
						t.gamevisuals.sGrassTextures[iL] = t.visuals.sGrassTextures[iL];
						g.projectmodified = 1;
						bUpdateGrassMaterials = true;
					}
					SetMipmapNum(-1);
					image_setlegacyimageloading(false);
				}
				else
				{
					sGrassTexturesID[iL] = t.terrain.imagestartindex + 180 + iL;
				}
				if (ImageExist(sGrassTexturesID[iL]))
				{
					iUsedImages++;

					float path_gadget_size = ImGui::GetFontSize()*3.0;
					int preview_icon_size = ImGui::GetFontSize();

					//PE: You can only change image slot 0 , not delete it, we need at least one texture.
					bool bDeleteImage = false;
					if (iL != 0 && iL == iLastSlotUsed) bDeleteImage = true;

					// chop grassbank from material name 
					cstr materialname = t.visuals.sGrassTexturesName[iL];

					cStr sLabel = cStr("##InputGrassTexture") + cStr(iL);

					//PE: New do 4x4 coloums here.

					int iLargerPreviewIconSize = 28;//PE: 54 , now lowest possible icon
					float control_width = (iLargerPreviewIconSize + 3.0) * 4.0f + 6.0;

					if (w > control_width) {
						//PE: fit perfectly with window width.
						iLargerPreviewIconSize = (w - 20.0) / 4.0;
						//iLargerPreviewIconSize -= 6.0; //Padding.
						iLargerPreviewIconSize -= 8.0; //Padding.
						if (iLargerPreviewIconSize > 70) iLargerPreviewIconSize = 70;
					}

					ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;

					if (sGrassTexturesID[iL] > 0)
					{
						cStr sLabelChild = cStr("##grass4x4") + cStr(iL);

						ImVec2 content_avail = { iLargerPreviewIconSize + 1.0f ,iLargerPreviewIconSize + 1.0f };

						//style.WindowPadding
						ImVec2 oldstyle = ImGui::GetStyle().FramePadding;
						ImGui::GetStyle().FramePadding = { 0,0 };
						ImGui::BeginChild(sLabelChild.Get(), content_avail, false, ImGuiWindowFlags_NoScrollbar);

						if (ImGui::ImgBtn(sGrassTexturesID[iL], ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize), ImColor(0, 0, 0, 255)))
						{
							//iSelectGrassTexture = iL;
							//Now do toggle selection.
							if (bCurrentGrassTextureForPaint[iL])
								bCurrentGrassTextureForPaint[iL] = false;
							else
								bCurrentGrassTextureForPaint[iL] = true;

						}

						if ( ImGui::IsItemHovered())
						{
							int tooltip_height = 350;
							int tooltip_width = 350;
							ImVec2 tooltip_position = ImVec2(ImGui::GetWindowPos());
							tooltip_position.x = vWindowPos.x - (tooltip_width + 10.0);
							if ((tooltip_position.y + tooltip_height) > (vWindowPos.y+vWindowSize.y))
								tooltip_position.y = (vWindowPos.y+vWindowSize.y) - tooltip_height - 10.0;
							ImGui::SetNextWindowPos(tooltip_position);
							ImGui::BeginTooltip();
							ImGui::ImgBtn(sGrassTexturesID[iL], ImVec2(tooltip_width, tooltip_height), ImColor(0, 0, 0, 255), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, true);
							ImGui::TextCenter(materialname.Get());
							ImGui::Separator();
							ImGui::EndTooltip();
						}

						ImGui::EndChild();
						ImGui::GetStyle().FramePadding = oldstyle;

					}
					sLabel = cStr("##veg") + cStr(iL);

					float checkwidth = ImGui::GetFontSize()*1.5;
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((ImGui::GetContentRegionAvail().x*0.5) - (checkwidth*0.5), 0.0f));
					//ImGui::Checkbox(sLabel.Get(), &bCurrentGrassTextureForPaint[iL]);

					if (bCurrentGrassTextureForPaint[iL])
					{
						ImVec2 padding = { 2, 2 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize));
						draw_selections[iL].bActive = true;
						draw_selections[iL].image_bb = image_bb;
						//PE: Cant draw here, as Column background will overwrite part of the rect.
						//ImGui::GetCurrentWindow()->DrawList->PushClipRect(image_bb.Min, image_bb.Max, true);
						//ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 3.0f);
						//ImGui::GetCurrentWindow()->DrawList->PopClipRect();
						values |= mask;
					}
					else
					{
						values &= ~mask;
					}

					ImGui::NextColumn();

				}
				else
				{
					values &= ~mask;
				}
			}
			else
			{
				//Delete old image.
				if (ImageExist(t.terrain.imagestartindex + 180 + iL) == 1) DeleteImage(t.terrain.imagestartindex + 180 + iL);
				sGrassTexturesID[iL] = 0;
				bCurrentGrassTextureForPaint[iL] = false;
				values &= ~mask;
			}
		}

		if (iActiveGrass <= 20)
			fContentHeight = 2.0 + (ImGui::GetCursorPosY() - curposy);

		ImGui::GetStyle().WindowPadding = oldwinstylemain;
		ImGui::GetStyle().FramePadding = oldstylemain;

		ImGui::EndChild();
		
		child_begin -= ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
		ImRect cliprect = ImRect(ImGui::GetWindowPos() + child_begin, ImGui::GetWindowPos() + child_begin + ImVec2(ImGui::GetWindowContentRegionWidth(), fContentHeight));
		for (int iL = 0; iL < 32; iL++)
		{
			if (draw_selections[iL].bActive)
			{
				//curposy
				//ImGui::GetCurrentWindow()->DrawList->PushClipRect(draw_selections[iL].image_bb.Min, draw_selections[iL].image_bb.Max, true);
				ImGui::GetCurrentWindow()->DrawList->PushClipRect(cliprect.Min, cliprect.Max, true);
				ImGui::GetCurrentWindow()->DrawList->AddRect(draw_selections[iL].image_bb.Min, draw_selections[iL].image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
				ImGui::GetCurrentWindow()->DrawList->PopClipRect();
			}
		}
		gggrass_global_params.paint_type = values;


		//ImGui::Indent(-4);
		ImGui::Columns(1);
		ImGui::Indent(10);


		if (bUpdateGrassMaterials)
		{
			bUpdateGrassMaterials = false;
		}
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

		// no need to toggle veg, we ALWAYS 'may' need it!
		bEnableVeg = true;

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
		ImGui::Indent(-10);
	}
}
/*
//PE: Just keep a backup of old setup.
void imgui_Customize_Vegetation_v3_old(int mode)
{
	int wflags = ImGuiTreeNodeFlags_DefaultOpen;
	if (mode == 1) wflags = ImGuiTreeNodeFlags_None;
	if (pref.bAutoClosePropertySections && mode == 1 && iLastOpenHeader != 3)
		ImGui::SetNextItemOpen(false, ImGuiCond_Always);

	float w = ImGui::GetWindowContentRegionWidth();

	//Customize Vegetation
	if (ImGui::StyleCollapsingHeader("Palette", wflags)) {

		if (mode == 1) iLastOpenHeader = 3;

		ImGui::Indent(10);
		ImGui::PushItemWidth(-10);

#ifdef WICKEDENGINE
		if (iDeleteSingleGrassTextures > 0)
		{
			if (ImageExist(iDeleteSingleGrassTextures))
				DeleteImage(iDeleteSingleGrassTextures);
			iDeleteSingleGrassTextures = 0;
		}
		if (iDeleteAllGrassTextures)
		{
			iDeleteAllGrassTextures = false;
			for (int iL = 0; iL < 16; iL++) {
				if (ImageExist(t.terrain.imagestartindex + 180 + iL))
					DeleteImage(t.terrain.imagestartindex + 180 + iL);
			}
			bUpdateGrassMaterials = true;
		}
		if (t.visuals.sGrassTextures[0] == "")
		{
			//PE: @Lee-Grass Define default palette here. Also need to update t.gamevisuals :)

			init_grass_selections(); //PE: Set defaults for selection window.

			const char* grass_filenames[16] =
			{
				"grassbank/grass1-rainforest.dds",
				"grassbank/grass2-rainforest.dds",
				"grassbank/grass3-rainforest.dds",
				"grassbank/grass4-rainforest.dds",
				"grassbank/flower red_color.dds",
				"grassbank/flower white_color.dds",
				"grassbank/flower yellow_color.dds",
				"grassbank/short grass 3.dds",
				"grassbank/tall grass 2_color.dds",
				"grassbank/tall grass 2 dead_color.dds",
				"grassbank/tall grass 3_color.dds",
				"grassbank/tall grass 3 dead_color.dds",
				"grassbank/tall grass 4_color.dds",
				"grassbank/tall grass 4 dead_color.dds",
				"grassbank/tall grass 5_color.dds",
				"grassbank/tall grass 5 dead_color.dds",
			};

			const char* grass_shortnames[16] =
			{
				"Rain Forest 1.dds",
				"Rain Forest 2.dds",
				"Rain Forest 3.dds",
				"Rain Forest 4.dds",
				"Red Flower.dds",
				"White Flower.dds",
				"Yellow Flower.dds",
				"Short Grass 3.dds",
				"Tall Grass 2.dds",
				"Tall Grass 2 Dead.dds",
				"Tall Grass 3.dds",
				"Tall Grass 3 Dead.dds",
				"Tall Grass 4.dds",
				"Tall Grass 4 Dead.dds",
				"Tall Grass 5.dds",
				"Tall Grass 5 Dead.dds",
			};

			for (uint32_t i = 0; i < 16; i++)
			{
				t.visuals.sGrassTextures[i] = grass_filenames[i];
				t.visuals.sGrassTexturesName[i] = grass_shortnames[i];
				t.gamevisuals.sGrassTextures[i] = t.visuals.sGrassTextures[i];
				t.gamevisuals.sGrassTexturesName[i] = t.visuals.sGrassTexturesName[i];
			}

			// save out reset grass plate choices
			visuals_save();

			// reset grass type choices to paint with
			grass_resetchoices();
		}

		// find last slot used
		int iLastSlotUsed = 0;
		for (int iL = 0; iL < 16; iL++)
			if (t.visuals.sGrassTextures[iL] != "")
				if (ImageExist(t.terrain.imagestartindex + 180 + iL))
					iLastSlotUsed = iL;

		int iUsedImages = 0;
		float col_start = 90.0f;
		int iSelectGrassTexture = -1;
		cStr sGrassTextureFolder = g.rootdir_s + "grassbank\\";


#ifdef DISPLAY4x4
		ImGui::Indent(-10);
		ImGui::Indent(4);
		ImGui::Columns(4, "##veg4x4columns", false);  //false no border
#endif

		for (int iL = 0; iL < 16; iL++)
		{
			if (t.visuals.sGrassTextures[iL] != "")
			{
				if (!ImageExist(t.terrain.imagestartindex + 180 + iL))
				{
					//Load in image.
					image_setlegacyimageloading(true);
					SetMipmapNum(1); //PE: mipmaps not needed.
					//t.terrain.imagestartindex = 63600
					if (ImageExist(t.terrain.imagestartindex + 180 + iL) == 1) DeleteImage(t.terrain.imagestartindex + 180 + iL);
					LoadImage(t.visuals.sGrassTextures[iL].Get(), t.terrain.imagestartindex + 180 + iL, 0, g.gdividetexturesize);
					if (ImageExist(t.terrain.imagestartindex + 180 + iL) == 1)
					{
						sGrassTexturesID[iL] = t.terrain.imagestartindex + 180 + iL;
					}
					else
					{
						//Load failed, clear texture slot.
						t.visuals.sGrassTextures[iL] = "";
						t.gamevisuals.sGrassTextures[iL] = t.visuals.sGrassTextures[iL];
						g.projectmodified = 1;
						bUpdateGrassMaterials = true;
					}
					SetMipmapNum(-1);
					image_setlegacyimageloading(false);
				}
				else
				{
					sGrassTexturesID[iL] = t.terrain.imagestartindex + 180 + iL;
				}
				if (ImageExist(sGrassTexturesID[iL]))
				{
					iUsedImages++;



					if (bGrassNameWindow[iL]) {
						//Ask for a proper name of veg.
						ImGui::SetNextWindowSize(ImVec2(26 * ImGui::GetFontSize(), 32 * ImGui::GetFontSize()), ImGuiCond_Once);
						ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
						cstr sUniqueWinName = cstr("Vegetation Name##ttn") + cstr(iL);
						ImGui::Begin(sUniqueWinName.Get(), &bGrassNameWindow[iL], 0);
						ImGui::Indent(10);
						static char NewTextureName[256];
						cstr sUniqueInputName = cstr("##InputVegetationName") + cstr(iL);
						float content_width = ImGui::GetContentRegionAvailWidth() - 10.0;
						ImGui::ImgBtn(sGrassTexturesID[iL], ImVec2(content_width, content_width), ImColor(0, 0, 0, 255));
						ImGui::PushItemWidth(-10);
						ImGui::Text("Enter a name for your vegetation:");

						if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
							ImGui::SetKeyboardFocusHere(0);

						if (ImGui::InputText(sUniqueInputName.Get(), t.visuals.sGrassTexturesName[iL].Get(), 250, ImGuiInputTextFlags_EnterReturnsTrue)) {
							t.gamevisuals.sGrassTexturesName[iL] = t.visuals.sGrassTexturesName[iL];
							bGrassNameWindow[iL] = false;
						}
#ifdef WICKEDENGINE
						if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;
#endif

						ImGui::PopItemWidth();
						ImGui::Indent(-10);
						if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
							//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
							ImGui::Text("");
							ImGui::Text("");
						}
						bImGuiGotFocus = true;
						ImGui::End();
					}


#ifndef DISPLAY4x4
					if (iUsedImages < 16 && iL == 0)
					{
						float but_gadget_size = ImGui::GetFontSize()*10.0;
						ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));

						if (ImGui::StyleButton("Add Texture##Grass", ImVec2(but_gadget_size, 0)))
						{
							iSelectGrassTexture = 99;
						}
						ImGui::Separator();
					}
#endif
					float path_gadget_size = ImGui::GetFontSize()*3.0;
					int preview_icon_size = ImGui::GetFontSize();

					//PE: You can only change image slot 0 , not delete it, we need at least one texture.
					bool bDeleteImage = false;
					if (iL != 0 && iL == iLastSlotUsed) bDeleteImage = true;

					// chop grassbank from material name 
					cstr materialname = t.visuals.sGrassTexturesName[iL];

					cStr sLabel = cStr("##InputGrassTexture") + cStr(iL);

					//PE: New do 4x4 coloums here.
#ifdef DISPLAY4x4
					int iLargerPreviewIconSize = 28;//PE: 54 , now lowest possible icon
					float control_width = (iLargerPreviewIconSize + 3.0) * 4.0f + 6.0;

					if (w > control_width) {
						//PE: fit perfectly with window width.
						iLargerPreviewIconSize = (w - 20.0) / 4.0;
						iLargerPreviewIconSize -= 6.0; //Padding.
						if (iLargerPreviewIconSize > 70) iLargerPreviewIconSize = 70;
					}

					ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;

					if (sGrassTexturesID[iL] > 0)
					{
						cStr sLabelChild = cStr("##grass4x4") + cStr(iL);

						ImVec2 content_avail = { iLargerPreviewIconSize + 1.0f ,iLargerPreviewIconSize + 1.0f };

						//style.WindowPadding
						ImVec2 oldstyle = ImGui::GetStyle().FramePadding;
						ImGui::GetStyle().FramePadding = { 0,0 };
						ImGui::BeginChild(sLabelChild.Get(), content_avail, false, ImGuiWindowFlags_NoScrollbar);

						if (ImGui::ImgBtn(sGrassTexturesID[iL], ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize), ImColor(0, 0, 0, 255)))
						{
							//iSelectGrassTexture = iL;
							//Now do toggle selection.
							if (bCurrentGrassTextureForPaint[iL])
								bCurrentGrassTextureForPaint[iL] = false;
							else
								bCurrentGrassTextureForPaint[iL] = true;

						}

						bool bInContext = false;
						static int iCurrentContext = -1;
						if (iCurrentContext == -1 || iCurrentContext == iL)
						{
							if (ImGui::BeginPopupContextWindow())
							{
								iCurrentContext = iL;
								bInContext = true;
								if (ImGui::MenuItem("Change Vegetation"))
								{
									iSelectGrassTexture = iL;
									iCurrentContext = -1;
								}
								if (ImGui::MenuItem("Change Vegetation Name"))
								{
									bGrassNameWindow[iL] = true;
									iCurrentContext = -1;
								}
								if (bCurrentGrassTextureForPaint[iL])
								{
									if (ImGui::MenuItem("Unselect Vegetation"))
									{
										bCurrentGrassTextureForPaint[iL] = false;
									}
								}
								else {
									if (ImGui::MenuItem("Select Vegetation"))
									{
										bCurrentGrassTextureForPaint[iL] = true;
									}
								}
								if (iL != 0)
								{
									if (ImGui::MenuItem("Delete Vegetation"))
									{
										t.visuals.sGrassTextures[iL] = ""; //delete image in next run.
										t.gamevisuals.sGrassTextures[iL] = t.visuals.sGrassTextures[iL];
										g.projectmodified = 1;
										bUpdateGrassMaterials = true;
										iCurrentContext = -1;
									}
								}
								//ImGui::TextCenter(materialname.Get());
								ImGui::EndPopup();
							}
							else {
								iCurrentContext = -1;
							}
						}
						if (!bInContext && ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text("(This feature is not yet available)");
							ImGui::ImgBtn(sGrassTexturesID[iL], ImVec2(350, 350), ImColor(0, 0, 0, 255), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, true);
							ImGui::TextCenter(materialname.Get());
							ImGui::Separator();
							ImGui::EndTooltip();
						}

						ImGui::EndChild();
						ImGui::GetStyle().FramePadding = oldstyle;

					}
					sLabel = cStr("##veg") + cStr(iL);

					float checkwidth = ImGui::GetFontSize()*1.5;
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((ImGui::GetContentRegionAvail().x*0.5) - (checkwidth*0.5), 0.0f));
					//ImGui::Checkbox(sLabel.Get(), &bCurrentGrassTextureForPaint[iL]);

					if (bCurrentGrassTextureForPaint[iL]) //sTerrainTexturesID[iL] - t.terrain.imagestartindex - 80)
					{
						ImVec2 padding = { 2.0, 2.0 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}


					ImGui::NextColumn();

#else
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

					//PE: @Lee-Grass bCurrentGrassTextureForPaint[iL] will be true if you need to use it when painting.
					ImGui::Checkbox(sLabel.Get(), &bCurrentGrassTextureForPaint[iL]);

					ImGui::SameLine();
					//ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY() - 3));
					//ImGui::SetCursorPos(ImVec2(col_start-40, ImGui::GetCursorPosY()));
					int iSmallPreviewYMargin = preview_icon_size / 2;
					int iLargerPreviewIconSize = preview_icon_size * 2;
					ImGui::SetCursorPos(ImVec2(col_start - 40, ImGui::GetCursorPosY() - iSmallPreviewYMargin));

					if (sGrassTexturesID[iL] > 0)
					{
						if (ImGui::ImgBtn(sGrassTexturesID[iL], ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize), ImColor(0, 0, 0, 255)))
						{
							iSelectGrassTexture = iL;
						}
						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::ImgBtn(sGrassTexturesID[iL], ImVec2(180, 180), ImColor(0, 0, 0, 255));
							ImGui::EndTooltip();
						}
						ImGui::SameLine();
					}

					int iInputFlags = ImGuiInputTextFlags_EnterReturnsTrue;
					if (!bDeleteImage)
						iInputFlags = ImGuiInputTextFlags_ReadOnly;

					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + iSmallPreviewYMargin));
					sLabel = cStr(" ##InputGrassTexture") + cStr(iL);
					ImGui::PushItemWidth(-10 - path_gadget_size);
					if (ImGui::InputText(sLabel.Get(), &materialname[0], 250, iInputFlags))
					{
						if (strlen(materialname) == 0) t.visuals.sGrassTextures[iL] = ""; //delete image in next run.
						t.gamevisuals.sGrassTextures[iL] = t.visuals.sGrassTextures[iL];
						g.projectmodified = 1;
						bUpdateGrassMaterials = true;
					}
					ImGui::PopItemWidth();

					ImGui::SameLine();
					ImGui::PushItemWidth(path_gadget_size);
					sLabel = cStr("...##InputGrassTexture") + cStr(iL);

					if (ImGui::StyleButton(sLabel.Get()))
						iSelectGrassTexture = iL;

					ImGui::PopItemWidth();

					if (bDeleteImage)
					{
						ImGui::SameLine();
						sLabel = cStr("X##InputGrassTexture") + cStr(iL);
						if (ImGui::StyleButton(sLabel.Get()))
						{
							t.visuals.sGrassTextures[iL] = ""; //delete image in next run.
							t.gamevisuals.sGrassTextures[iL] = t.visuals.sGrassTextures[iL];
							bCurrentGrassTextureForPaint[iL] = false;
							g.projectmodified = 1;
							bUpdateGrassMaterials = true;
							visuals_save();
						}
					}

					// squish together as it looks better - can see more grass choices
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - iSmallPreviewYMargin * 2));
#endif
				}
			}
			else
			{
				//Delete old image.
				if (ImageExist(t.terrain.imagestartindex + 180 + iL) == 1) DeleteImage(t.terrain.imagestartindex + 180 + iL);
				sGrassTexturesID[iL] = 0;
				bCurrentGrassTextureForPaint[iL] = false;
			}
		}

#ifdef DISPLAY4x4
		ImGui::Indent(-4);
		ImGui::Columns(1);
		ImGui::Indent(10);

		if (iUsedImages < 16)
		{
			//ImGui::Separator();
			float but_gadget_size = ImGui::GetFontSize()*10.0;
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));

			if (ImGui::StyleButton("Add New Vegetation##Grass", ImVec2(but_gadget_size, 0)))
			{
				iSelectGrassTexture = 99;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Add New Vegetation (This feature is not yet available)");

			//ImGui::Separator();
		}
#endif

		static bool grass_selection_window = false;
		static int iSelectedGrassTexture = -1;
		if (iSelectGrassTexture >= 0)
		{
			iSelectedGrassTexture = iSelectGrassTexture;
			grass_selection_window = true;
			iSelectGrassTexture = 0;
		}
		iSelectGrassTexture = imgui_get_selections(grass_selections, grass_selections_text, 1, 280, &grass_selection_window);

		if (iSelectGrassTexture >= 0 && iSelectedGrassTexture >= 0)
		{
			bool bNeedFileSelector = false;
			if (iSelectGrassTexture == 999)
				bNeedFileSelector = true;

			grass_selection_window = false;

			int iNewGrassIndex = iSelectGrassTexture;
			iSelectGrassTexture = iSelectedGrassTexture;
			if (iSelectGrassTexture == 99)
			{
				//Find free slot.
				for (int iL2 = 0; iL2 < 32; iL2++) {
					if (t.visuals.sGrassTextures[iL2] == "")
					{
						iSelectGrassTexture = iL2;
						break;
					}
				}
			}

			if (!bNeedFileSelector) {
				t.visuals.sGrassTextures[iSelectGrassTexture] = grass_selections[iNewGrassIndex];
				if (ImageExist(t.terrain.imagestartindex + 180 + iSelectGrassTexture))
					iDeleteSingleGrassTextures = t.terrain.imagestartindex + 180 + iSelectGrassTexture;
				bUpdateGrassMaterials = true;
				t.visuals.sGrassTexturesName[iSelectGrassTexture] = grass_selections_text[iNewGrassIndex];
				bGrassNameWindow[iSelectGrassTexture] = true;
				bCurrentGrassTextureForPaint[iSelectGrassTexture] = true; //PE: Select new added texture.
			}

			if (bNeedFileSelector)
			{
				cStr tOldDir = GetDir();
				char * cFileSelected;
				//cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "All\0*.*\0DDS\0*_color.dds\0PNG\0*_color.png\0JPEG\0*_color.jpg\0TGA\0*_color.tga\0BMP\0*_color.bmp\0\0\0", sGrassTextureFolder.Get(), NULL);
				cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "DDS\0*_color.dds\0\0\0", sGrassTextureFolder.Get(), NULL);
				SetDir(tOldDir.Get());

				if (cFileSelected && strlen(cFileSelected) > 0)
				{
					char *relonly = (char *)pestrcasestr(cFileSelected, g.rootdir_s.Get());
					t.visuals.sGrassTextures[iSelectGrassTexture] = cFileSelected;
					t.visuals.sGrassTexturesName[iSelectGrassTexture] = "";
					if (relonly)
					{
						t.visuals.sGrassTextures[iSelectGrassTexture] = cFileSelected + g.rootdir_s.Len();
					}
					g.projectmodified = 1;
					t.gamevisuals.sGrassTextures[iSelectGrassTexture] = t.visuals.sGrassTextures[iSelectGrassTexture];
					bUpdateGrassMaterials = true;

					bGrassNameWindow[iSelectGrassTexture] = true;

					//PE: Reload image.
					if (ImageExist(t.terrain.imagestartindex + 180 + iSelectGrassTexture))
						iDeleteSingleGrassTextures = t.terrain.imagestartindex + 180 + iSelectGrassTexture;

					bCurrentGrassTextureForPaint[iSelectGrassTexture] = true; //PE: Select new added texture.

				}
			}
			iSelectGrassTexture = -1;
		}

		if (bUpdateGrassMaterials)
		{
			//@Lee-Grass update super palette.
			for (int iL = 0; iL < 16; iL++)
			{
				if (t.visuals.sGrassTextures[iL] != "")
				{
					if (sGrassChangedTextures[iL] != t.visuals.sGrassTextures[iL])
					{
						//Add t.visuals.sGrassTextures[iL] to palette in slot iL
						sGrassChangedTextures[iL] = t.visuals.sGrassTextures[iL];

						// the chosen grass file
						char pGrassSrcWorkFile[MAX_PATH];
						strcpy(pGrassSrcWorkFile, sGrassChangedTextures[iL].Get());
						pGrassSrcWorkFile[strlen(pGrassSrcWorkFile) - strlen("_color.dds")] = 0;
						char pGrassSrcFile[MAX_PATH];
						strcpy(pGrassSrcFile, pGrassSrcWorkFile + strlen("grassbank\\"));

						char pChosenGrassSrcFile[MAX_PATH];
						strcpy(pChosenGrassSrcFile, pGrassSrcFile);

						// determine grass texture filenames to insert (i.e. take ' dead' from 'grass short dead' and put in pChosenGrassSrcFile as 'grass short')
						char pFullGrassSrcFile[MAX_PATH];
						strcpy(pFullGrassSrcFile, pChosenGrassSrcFile);
						for (int n = strlen(pFullGrassSrcFile) - 1; n > 0; n--)
						{
							if (pFullGrassSrcFile[n] == ' ')
							{
								pChosenGrassSrcFile[n] = 0;
								break;
							}
						}

						// replace grass image in grass plate
						bool bGrassPlateChanged = false;
						//for (int iGrassTexSet = 0; iGrassTexSet < 3; iGrassTexSet++) // no more normal or surface
						for (int iGrassTexSet = 0; iGrassTexSet < 1; iGrassTexSet++)
						{
							// determine destination grass plate
							char pDestTerrainTextureFile[MAX_PATH];
							strcpy(pDestTerrainTextureFile, g.fpscrootdir_s.Get());
							strcat(pDestTerrainTextureFile, "\\Files\\levelbank\\testmap\\grass");
							if (iGrassTexSet == 0) strcat(pDestTerrainTextureFile, "_coloronly.dds");
							//if (iGrassTexSet == 1) strcat(pDestTerrainTextureFile, "_normal.dds");
							//if (iGrassTexSet == 2) strcat(pDestTerrainTextureFile, "_surface.dds");

							// construct grass texture filenames to insert
							char pTexFileToLoad[MAX_PATH];
							strcpy(pTexFileToLoad, g.fpscrootdir_s.Get());
							strcat(pTexFileToLoad, "\\Files\\grassbank\\");
							if (iGrassTexSet == 0)
							{
								// allows an additional word before _color so can use same normal and surface texture maps
								strcat(pTexFileToLoad, pFullGrassSrcFile);
							}
							else
							{
								// possibly truncated for normal and surface loading
								strcat(pTexFileToLoad, pChosenGrassSrcFile);
							}
							if (iGrassTexSet == 0) strcat(pTexFileToLoad, "_color.dds");
							//if (iGrassTexSet == 1) strcat(pTexFileToLoad, "_normal.dds");
							//if (iGrassTexSet == 2) strcat(pTexFileToLoad, "_surface.dds");

							// do the insert
							int iGrassType = iL;
							if (ImageCreateTexturePlate(pDestTerrainTextureFile, iGrassType, pTexFileToLoad, 1, 1) == 1)
							{
								// success
								bGrassPlateChanged = true;
							}
						}
						if (bGrassPlateChanged == true)
						{
							// must remove pre-stored reference to any previous grass_color texture set
							WickedCall_DeleteImage("levelbank\\testmap\\grass_coloronly.dds");
							//WickedCall_DeleteImage("levelbank\\testmap\\grass_normal.dds");
							//WickedCall_DeleteImage("levelbank\\testmap\\grass_surface.dds");

							// Reload grass plate textures on all grass objects
							grass_setgrassimage();
						}
					}
				}
			}
			bUpdateGrassMaterials = false;
		}
#else
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
		char * current_veg = NULL;// t.vegstylebank_s[g.vegstyleindex].Get();
		if (g.vegstyleindex <= g.vegstylemax) current_veg = t.vegstylebank_s[g.vegstyleindex].Get();
		if (!current_veg) current_veg = "NA";
		if (ImGui::BeginCombo("##SelectVegetationCombo", current_veg)) // The second parameter is the label previewed before opening the combo.
		{

			for (int vegindex = 1; vegindex <= g.vegstylemax; vegindex++)
			{

				if (t.vegstylebank_s[vegindex].Len() > 0)
				{
					bool is_selected = false;
					if (t.vegstylebank_s[vegindex].Get() == current_veg)
						is_selected = true;
					if (ImGui::Selectable(t.vegstylebank_s[vegindex].Get(), is_selected)) {


						//Test display veg.
						//grass_setgrassgridandfade();
						//grass_init();
						//t.completelyfillvegarea = 1;
						t.terrain.grassupdateafterterrain = 1;
						grass_loop();
						t.terrain.grassupdateafterterrain = 0;

						g.projectmodified = 1;
						//	current_veg = t.terrainstylebank_s[vegindex].Get(); a bug?
						current_veg = t.vegstylebank_s[vegindex].Get();

						g.vegstyleindex = vegindex;
						g.vegstyle_s = t.vegstylebank_s[g.vegstyleindex];

						t.visuals.vegetationindex = g.vegstyleindex;
						t.visuals.vegetation_s = t.vegstylebank_s[g.vegstyleindex];;
						t.gamevisuals.vegetationindex = t.visuals.vegetationindex;
						t.gamevisuals.vegetation_s = t.visuals.vegetation_s;
						grass_changevegstyle();
						bUpdateVeg = true;
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
#endif

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

		// no need to toggle veg, we ALWAYS 'may' need it!
		bEnableVeg = true;

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
		ImGui::Indent(-10);
	}
}
*/

void imgui_Customize_Weather(int mode)
{
#ifdef ALLOW_WEATHER_IN_EDITOR
	//int wflags = ImGuiTreeNodeFlags_DefaultOpen;
	//if (mode == 1) wflags = ImGuiTreeNodeFlags_None;
	//PE: Default to closed.
	int wflags = ImGuiTreeNodeFlags_None;

	if (pref.bAutoClosePropertySections && iLastOpenHeader != 4)
		ImGui::SetNextItemOpen(false, ImGuiCond_Always);


	if (ImGui::StyleCollapsingHeader("Weather", wflags)) {

		iLastOpenHeader = 4;

		ImGui::Indent(10);

		const char* items_align[] = { "None" , "Light Rain", "Heavy Rain","Light Snow" ,"Heavy Snow" }; //,"Test"
		int item_current_type_selection = 0;
		item_current_type_selection = t.visuals.iEnvironmentWeather;
		ImGui::PushItemWidth(-10);
		if (ImGui::Combo("##WeatherDropwDown", &item_current_type_selection, items_align, IM_ARRAYSIZE(items_align))) {
			t.visuals.iEnvironmentWeather = item_current_type_selection;
			t.gamevisuals.iEnvironmentWeather = t.visuals.iEnvironmentWeather;
		}
		ImGui::PopItemWidth();
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Weather");


		//ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
		//ImGui::Text("Display Weather:");
		//ImGui::SameLine();
		//ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
		//ImGui::SetCursorPos(ImVec2(136, ImGui::GetCursorPosY()));
		if (ImGui::Checkbox("Display Weather##DisplayWeather", &bEnableWeather))
		{
			reset_env_particles();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Display Weather in Editor");

		ImGui::Indent(-10);
	}
#endif
}


void imgui_Customize_Water(int mode)
{
	void tab_tab_Column_text(char *text, float fColumn);

	//int wflags = ImGuiTreeNodeFlags_DefaultOpen;
	//if (mode == 1) wflags = ImGuiTreeNodeFlags_None;
	//PE: Default to closed.
	int wflags = ImGuiTreeNodeFlags_None;
	if (pref.bAutoClosePropertySections && iLastOpenHeader != 32)
		ImGui::SetNextItemOpen(false, ImGuiCond_Always);


	if (ImGui::StyleCollapsingHeader("Water", wflags)) {

		iLastOpenHeader = 32;

		ImGui::Indent(10);
		float fTabColumnWidth = 120.0f;

		//tab_tab_Column_text("Enable Water", fTabColumnWidth);
		ImGui::PushItemWidth(-10);

		
		if (ImGui::Checkbox("Enable Water##v2bEnableWater", &t.visuals.bWaterEnable)) {
			t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
			Wicked_Update_Visuals((void *)&t.visuals);
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Enable Water");
		

		if (t.visuals.bWaterEnable)
		{
			t.terrain.waterliney_f = g.gdefaultwaterheight;

			ImGui::PushItemWidth(-10);
			float waterHeight = GGTerrain_UnitsToMeters( g.gdefaultwaterheight );
			if (ImGui::SliderFloat("##igdefaultwaterheight:", &waterHeight, -500, 1500, "%.1f", 2.0f))
			{
				g.gdefaultwaterheight = waterHeight;
				Wicked_Update_Visuals((void *)&t.visuals);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Water Height");
			ImGui::PopItemWidth();


			ImGui::PushItemWidth(-10);
			if (ImGui::SliderFloat("##fWaterSpeed1:", &t.visuals.WaterSpeed1, -100.0f, 100.0f))
			{
				t.gamevisuals.WaterSpeed1 = t.visuals.WaterSpeed1;
				Wicked_Update_Visuals((void *)&t.visuals);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Water Speed");
			ImGui::PopItemWidth();


			float fWaterColor[4];

			fWaterColor[0] = t.visuals.WaterRed_f / 255.0;
			fWaterColor[1] = t.visuals.WaterGreen_f / 255.0;
			fWaterColor[2] = t.visuals.WaterBlue_f / 255.0;
			fWaterColor[3] = t.visuals.WaterAlpha_f / 255.0;
			ImGui::PushItemWidth(-10);
			if (ImGui::ColorEdit3("##V2WickedWaterColor", &fWaterColor[0], ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_DisplayRGB))
			{
				t.gamevisuals.WaterRed_f = t.visuals.WaterRed_f = fWaterColor[0] * 255.0;
				t.gamevisuals.WaterGreen_f = t.visuals.WaterGreen_f = fWaterColor[1] * 255.0;
				t.gamevisuals.WaterBlue_f = t.visuals.WaterBlue_f = fWaterColor[2] * 255.0;
				t.gamevisuals.WaterAlpha_f = t.visuals.WaterAlpha_f = fWaterColor[3] * 255.0;

				g.projectmodified = 1;
				Wicked_Update_Visuals((void *)&t.visuals);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change the water base color applied as part of the overall effect");
			//if (ImGui::IsItemHovered()) ImGui::SetTooltip("Water Color");

			ImGui::PopItemWidth();

			ImGui::PushItemWidth(-10);
			if (ImGui::SliderFloat("##fWaterWaveAmplitude:", &t.visuals.fWaterWaveAmplitude, 0.0f, 1000.0f))
			{
				t.gamevisuals.fWaterWaveAmplitude = t.visuals.fWaterWaveAmplitude;
				Wicked_Update_Visuals((void *)&t.visuals);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Water Wave Size");
			ImGui::PopItemWidth();
			

			ImGui::PushItemWidth(-10);
			if (ImGui::SliderFloat("##fWaterWindDependency:", &t.visuals.fWaterWindDependency, 0.0f, 1.0f))
			{
				t.gamevisuals.fWaterWindDependency = t.visuals.fWaterWindDependency;
				Wicked_Update_Visuals((void *)&t.visuals);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Water Wind Contribution");
			ImGui::PopItemWidth();


			ImGui::PushItemWidth(-10);
			if (ImGui::SliderFloat("##fWaterPatchLength:", &t.visuals.fWaterPatchLength, 10.0f, 2000.0f))
			{
				t.gamevisuals.fWaterPatchLength = t.visuals.fWaterPatchLength;
				Wicked_Update_Visuals((void *)&t.visuals);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Water Tiling Patch Size");
			ImGui::PopItemWidth();


			ImGui::PushItemWidth(-10);
			if (ImGui::SliderFloat("##fWaterChoppyScale:", &t.visuals.fWaterChoppyScale, 0.0f, 10.0f))
			{
				t.gamevisuals.fWaterChoppyScale = t.visuals.fWaterChoppyScale;
				Wicked_Update_Visuals((void *)&t.visuals);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Water Wave Choppiness");
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(-10);
			if (ImGui::SliderFloat("##fWaterFogMinAmount", &t.visuals.WaterFogMinAmount, 0.0f, 1.0f, "%.3f", 1.0f))
			{
				t.gamevisuals.WaterFogMinAmount = t.visuals.WaterFogMinAmount;
				Wicked_Update_Visuals((void *)&t.visuals);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Water Fog Min Amount");
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(-10);
			if (ImGui::SliderFloat("##fWaterFogMinDist", &t.visuals.WaterFogMinDist, 0.0f, 100000.0f, "%.2f", 2.0f))
			{
				if ( t.visuals.WaterFogMinDist > t.visuals.WaterFogMaxDist ) t.visuals.WaterFogMaxDist = t.visuals.WaterFogMinDist+0.1;
				t.gamevisuals.WaterFogMinDist = t.visuals.WaterFogMinDist;
				Wicked_Update_Visuals((void *)&t.visuals);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Water Fog Min Distance");
			
			if (ImGui::SliderFloat("##fWaterFogMaxDist", &t.visuals.WaterFogMaxDist, 0.0f, 100000.0f, "%.2f", 2.0f))
			{
				if ( t.visuals.WaterFogMinDist > t.visuals.WaterFogMaxDist ) t.visuals.WaterFogMinDist = t.visuals.WaterFogMaxDist-0.1;
				t.gamevisuals.WaterFogMaxDist = t.visuals.WaterFogMaxDist;
				Wicked_Update_Visuals((void *)&t.visuals);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Water Fog Max Distance");
			ImGui::PopItemWidth();
			

			/*
			wave_amplitude 50
			patch_length 1000
			choppy_scale 1.3
			*/
		}
		ImGui::PopItemWidth();

		ImGui::Indent(-10);
	}
}



void imgui_Customize_Water_V2(int mode)
{
	void tab_tab_Column_text(char *text, float fColumn);

	//PE: NOTE tabtab mode < 3 normally used iLastOpenHeader = 5

	int wflags = ImGuiTreeNodeFlags_None;
	if (mode == 4)
	{
		// Displayed in terrain tools
		if (pref.bAutoClosePropertySections && iLastOpenHeader != 32)
			ImGui::SetNextItemOpen(false, ImGuiCond_Always);
	}
	else
	{
		// Displayed in tabtab visuals
		if (pref.bAutoClosePropertySections && iLastOpenHeader != 5)
			ImGui::SetNextItemOpen(false, ImGuiCond_Always);
	}
	//if (mode < 3)
	//{
		
	//}
	//else
	//{
	//	if (mode == 4)
	//		wflags = ImGuiTreeNodeFlags_DefaultOpen;
	//}

	ImVec2 vHoverRectStart = ImGui::GetCursorPos();
	if (ImGui::StyleCollapsingHeader("Water", wflags))
	{

		if (mode != 4)
			iLastOpenHeader = 5;
		else
			iLastOpenHeader = 32;

		ImGui::Indent(10);
		ImGui::PushItemWidth(-10);
		float w = ImGui::GetWindowContentRegionWidth();

		float fTabColumnWidth = 120.0f;

		if (pref.iEnableAdvancedWater)
		{
			// ZJ: Moved to the View Options menu
			/*
			if (ImGui::Checkbox("Enable Water##v2bEnableWater", &t.visuals.bWaterEnable)) {
				static int iRememberLastHeight = -1;
				if (!t.visuals.bWaterEnable)
				{
					iRememberLastHeight = g.gdefaultwaterheight;
					t.terrain.waterliney_f = g.gdefaultwaterheight = -10000.0f;
					t.visuals.bWaterEnable = true; //PE We need the new water height set.
					Wicked_Update_Visuals((void *)&t.visuals);
					t.visuals.bWaterEnable = false;
				}
				else
				{
					float fWaterHeight = GGORIGIN_Y;
					if (iRememberLastHeight != -1 && iRememberLastHeight > -9999)
						fWaterHeight = iRememberLastHeight;
					t.terrain.waterliney_f = g.gdefaultwaterheight = fWaterHeight;
				}
				t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
				Wicked_Update_Visuals((void *)&t.visuals);
				g.projectmodified = 1;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Enable Water");
			*/
		}
		else {
			t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable = true;
		}

		if (t.visuals.bWaterEnable)
		{

			t.terrain.waterliney_f = g.gdefaultwaterheight;

			// water height calc is confusing if need it as a unit (as it has min/max and power applied to match terrain biome params)
			if (pref.iEnableAdvancedWater)
			{
				// simpler Y axis units for working with water control
				ImGui::TextCenter("Water Height (units)");
				float fTmp = g.gdefaultwaterheight - 3937;
				int iMinValue = -100 * 39.37f;
				int iMaxValue = 300 * 39.37f;
				if (ImGui::MaxSliderInputFloat("##igdefaultwaterheightinunits", &fTmp, iMinValue, iMaxValue, "Set Water Height based on simple Y-axis unit value", iMinValue, fabs(iMinValue)+fabs(iMaxValue)))
				{
					g.gdefaultwaterheight = fTmp + 3937;
					Wicked_Update_Visuals((void*)&t.visuals);
					g.projectmodified = 1;
					ggterrain_extra_params.iUpdateTrees = 1;
				}
			}
			else
			{
				ImGui::TextCenter("Water Height (meters)");
				float fTmp = GGTerrain_UnitsToMeters(g.gdefaultwaterheight);
				if (ImGui::MaxSliderInputFloatPower("##igdefaultwaterheight", &fTmp, -100.0, 300.0, "Set Water Height using a meter scale tied to the original biome settings", -100, 300, 30, 1.2f, 1))
				{
					g.gdefaultwaterheight = GGTerrain_MetersToUnits(fTmp);
					Wicked_Update_Visuals((void*)&t.visuals);
					g.projectmodified = 1;
					ggterrain_extra_params.iUpdateTrees = 1;
				}
			}

			ImGui::TextCenter("Water Speed");
			float fTmp = t.visuals.WaterSpeed1 * 100.0f;
			if (ImGui::MaxSliderInputFloat("##fWaterSpeed1:", &fTmp, 0.0f, 50.0f, "Set Water Speed", 0, 50.0f))
			{
				t.visuals.WaterSpeed1 = fTmp * 0.01f;
				t.gamevisuals.WaterSpeed1 = t.visuals.WaterSpeed1;
				Wicked_Update_Visuals((void *)&t.visuals);
				g.projectmodified = 1;
			}

			ImGui::TextCenter("Water Base Color");
			ImVec4 mycolor = ImVec4(t.visuals.WaterRed_f / 255.0, t.visuals.WaterGreen_f / 255.0, t.visuals.WaterBlue_f / 255.0, 1.0);
			//ImGui::TextCenter("Water Color");
			//ImVec4 mycolor = ImVec4(t.visuals.WaterRed_f / 255.0, t.visuals.WaterGreen_f / 255.0, t.visuals.WaterBlue_f / 255.0, t.visuals.WaterAlpha_f / 255.0);

			bool open_popup = ImGui::ColorButton("##NewV2WickedWaterColor", mycolor, 0, ImVec2(w - 20.0, 0));
			if (open_popup)
				ImGui::OpenPopup("##pickV2WickedWaterColor");
			if (ImGui::BeginPopup("##pickV2WickedWaterColor", ImGuiWindowFlags_NoMove))
			{
				if (ImGui::ColorPicker4("##pickerV2WickedWaterColor", (float*)&mycolor, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview))
				{
					t.gamevisuals.WaterRed_f = t.visuals.WaterRed_f = mycolor.x * 255.0;
					t.gamevisuals.WaterGreen_f = t.visuals.WaterGreen_f = mycolor.y * 255.0;
					t.gamevisuals.WaterBlue_f = t.visuals.WaterBlue_f = mycolor.z * 255.0;
					t.gamevisuals.WaterAlpha_f = t.visuals.WaterAlpha_f = mycolor.w * 255.0;

					g.projectmodified = 1;
					Wicked_Update_Visuals((void *)&t.visuals);
				}
				ImGui::EndPopup();
			}

			ImGuiWindow* window = ImGui::GetCurrentWindow(); //PE: Add a pencil to all color gadgets.
			ID3D11ShaderResourceView* lpTexture = GetImagePointerView(TOOL_PENCIL);
			ImVec2 vDrawPos = { ImGui::GetCursorScreenPos().x + (ImGui::GetContentRegionAvail().x - 30.0f) ,ImGui::GetCursorScreenPos().y - (ImGui::GetFontSize()*1.5f) - 3.0f };
			window->DrawList->AddImage((ImTextureID)lpTexture, vDrawPos, vDrawPos + ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Water Color");

			extern void ControlAdvancedSetting(int&, const char*, bool* = nullptr);
			ControlAdvancedSetting(pref.iEnableAdvancedWater, "Advanced Water Settings");
		
			if (pref.iEnableAdvancedWater)
			{
				ImGui::TextCenter("Water Wave Size");
				/*if (ImGui::SliderFloat("##fWaterWaveAmplitude:", &t.visuals.fWaterWaveAmplitude, 0.0f, 800.0f, "%.1f", 2.0f))*/
				if (ImGui::MaxSliderInputFloat("##fWaterWaveAmplitude:", &t.visuals.fWaterWaveAmplitude, 0.0f, 800.0f, "Set Water Wave Size", 0, 800))
				{
					t.gamevisuals.fWaterWaveAmplitude = t.visuals.fWaterWaveAmplitude;
					Wicked_Update_Visuals((void *)&t.visuals);
					g.projectmodified = 1;
				}
				ImGui::TextCenter("Water Wind Contribution");
				//if (ImGui::SliderFloat("##fWaterWindDependency:", &t.visuals.fWaterWindDependency, 0.0f, 1.0f))
				fTmp = t.visuals.fWaterWindDependency * 100.0f;
				if (ImGui::MaxSliderInputFloat("##fWaterWindDependency:", &fTmp, 0.0f, 100.0f, "Set Water Wind Contribution", 0, 100.0f))
				{
					t.visuals.fWaterWindDependency = fTmp * 0.01f;
					t.gamevisuals.fWaterWindDependency = t.visuals.fWaterWindDependency;
					Wicked_Update_Visuals((void *)&t.visuals);
					g.projectmodified = 1;
				}
				ImGui::TextCenter("Water Tiling Patch Size");
				//if (ImGui::SliderFloat("##fWaterPatchLength:", &t.visuals.fWaterPatchLength, 10.0f, 300.0f))
				if (ImGui::MaxSliderInputFloat("##fWaterPatchLength:", &t.visuals.fWaterPatchLength, 10.0f, 300.0f, "Set Water Tile Size", 10.0f, 300.0f))
				{
					t.gamevisuals.fWaterPatchLength = t.visuals.fWaterPatchLength;
					Wicked_Update_Visuals((void *)&t.visuals);
					g.projectmodified = 1;
				}
				ImGui::TextCenter("Water Wave Choppiness");
				fTmp = t.visuals.fWaterChoppyScale * 10.0f;
				if (ImGui::MaxSliderInputFloat("##fWaterChoppyScale:", &fTmp, 0.0f, 100.0f, "Set Water Wave Choppiness"))
				{
					t.visuals.fWaterChoppyScale = fTmp * 0.1f;
					t.gamevisuals.fWaterChoppyScale = t.visuals.fWaterChoppyScale;
					Wicked_Update_Visuals((void *)&t.visuals);
					g.projectmodified = 1;
				}


				ImGui::TextCenter("Water Fog Start");
				if (ImGui::MaxSliderInputFloat("##fWaterFogMinDist", &t.visuals.WaterFogMinDist, 0.0f, 100000.0f, "Closer than this distance under water will have the minimum amount of fog"))
				{
					if (t.visuals.WaterFogMinDist > t.visuals.WaterFogMaxDist) t.visuals.WaterFogMaxDist = t.visuals.WaterFogMinDist;
					t.gamevisuals.WaterFogMinDist = t.visuals.WaterFogMinDist;
					Wicked_Update_Visuals((void *)&t.visuals);
					g.projectmodified = 1;
				}

				ImGui::TextCenter("Water Fog Maximum");
				if (ImGui::MaxSliderInputFloat("##fWaterFogMaxDist", &t.visuals.WaterFogMaxDist, 0.0f, 100000.0f, "After this distance under water it will be completely opaque"))
				{
					if (t.visuals.WaterFogMinDist > t.visuals.WaterFogMaxDist) t.visuals.WaterFogMinDist = t.visuals.WaterFogMaxDist;
					t.gamevisuals.WaterFogMaxDist = t.visuals.WaterFogMaxDist;
					Wicked_Update_Visuals((void *)&t.visuals);
					g.projectmodified = 1;
				}

				ImGui::TextCenter("Water Fog Minimum");
				if (ImGui::MaxSliderInputFloat("##fWaterFogMinAmount", &t.visuals.WaterFogMinAmount, 0.0f, 1.0f, "The minimum amount of under water fog that will always be present regardless of distance"))
				{
					t.gamevisuals.WaterFogMinAmount = t.visuals.WaterFogMinAmount;
					Wicked_Update_Visuals((void *)&t.visuals);
					g.projectmodified = 1;
				}
				//int iTmp = t.visuals.WaterFogMinAmount * 100;
				//if (ImGui::MaxSliderInputInt("##fWaterFogMinAmount", &iTmp, 0.0f, 100.0f, "The minimum amount of under water fog that will always be present regardless of distance"))
				//{
				//	t.visuals.WaterFogMinAmount = iTmp * 0.01f;
				//	t.gamevisuals.WaterFogMinAmount = t.visuals.WaterFogMinAmount;
				//	Wicked_Update_Visuals((void *)&t.visuals);
				//	g.projectmodified = 1;
				//}

			}

			//if (!pref.iEnableAdvancedWater)
			//{
			//	ImVec2 label_size = ImGui::CalcTextSize("Advanced Settings", NULL, true) + ImVec2(8.0f, 0.0f);
			//	ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((ImGui::GetContentRegionAvailWidth()*0.5) - (label_size.x*0.5), 0.0f));
			//	if (ImGui::HyberlinkButton("Advanced Settings##1", ImVec2(label_size.x, 0)))
			//	{
			//		extern int iSetSettingsFocusTab;
			//		extern bool bPreferences_Window;
			//		iSetSettingsFocusTab = 2;
			//		bPreferences_Window = true;
			//	}
			//}

		}
		ImGui::PopItemWidth();

		if (mode == 4)
		{
			//PE: To follow new auto close headers.
			if (pref.bAutoClosePropertySections)
			{
				// Causes bug where you can't sculpt after chaning water height
				//ggterrain_extra_params.edit_mode = 0;
				//ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;
			}
			else
			{
				ImVec2 vHoverRectEnd = ImGui::GetCursorPos();
				ImRect bbwin(ImGui::GetWindowPos() + ImVec2(0, vHoverRectStart.y - ImGui::GetScrollY()), ImGui::GetWindowPos() + ImVec2(ImGui::GetWindowSize().x + 20.0, vHoverRectEnd.y - ImGui::GetScrollY()));
				if (ImGui::IsMouseHoveringRect(bbwin.Min, bbwin.Max))
				{
					ggterrain_extra_params.edit_mode = 0;
					ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;
					bImGuiGotFocus = true;
				}
			}
		}
		ImGui::Indent(-10);
	}
}


void imgui_Customize_Weather_V2(int mode)
{
	int wflags = ImGuiTreeNodeFlags_None;
	bool bCHOpen = true;
	if (mode != 3)
	{
		if (pref.bAutoClosePropertySections && iLastOpenHeader != 4)
			ImGui::SetNextItemOpen(false, ImGuiCond_Always);

		bCHOpen = ImGui::StyleCollapsingHeader("Weather", wflags);
	}
	//ImGui::windowTabVisible()
	if (bCHOpen && ImGui::windowTabVisible() ) {

		if (mode != 3)
			iLastOpenHeader = 4;

		ImGui::Indent(10);
		ImGui::PushItemWidth(-10);

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

		float w = ImGui::GetWindowContentRegionWidth();

		int icon_size = (w - 20.0) / 3.0;
		icon_size -= 7; //Padding


		ImVec2 oldstyle = ImGui::GetStyle().FramePadding;
		ImGui::GetStyle().FramePadding = { 2,2 };

		if (t.visuals.iEnvironmentWeather == 0)
		{
			ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
			ImVec2 padding = { 2.0, 2.0 };
			const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(icon_size, icon_size));
			ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
		}

		if (ImGui::ImgBtn(ENV_SUN, ImVec2(icon_size, icon_size), ImColor(255,255,255,0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255) ,-1,0,0,0,false,false,false,false,false, bBoostIconColors))
		{
			t.visuals.iEnvironmentWeather = 0;
			t.gamevisuals.iEnvironmentWeather = t.visuals.iEnvironmentWeather;
			g.projectmodified = 1;
		}
		if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("Set No Weather Effect");
		ImGui::SameLine();

		if (t.visuals.iEnvironmentWeather == 1 || t.visuals.iEnvironmentWeather == 2 )
		{
			ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
			ImVec2 padding = { 2.0, 2.0 };
			const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(icon_size, icon_size));
			ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
		}
		if (ImGui::ImgBtn(ENV_RAIN, ImVec2(icon_size, icon_size), ImColor(255, 255, 255, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
		{
			t.visuals.iEnvironmentWeather = 1;
			t.gamevisuals.iEnvironmentWeather = t.visuals.iEnvironmentWeather;
			bEnableWeather = true;
			reset_env_particles();
			g.projectmodified = 1;
		}
		if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Rain Weather Effect");
		ImGui::SameLine();

		if (t.visuals.iEnvironmentWeather == 3 || t.visuals.iEnvironmentWeather == 4)
		{
			ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
			ImVec2 padding = { 2.0, 2.0 };
			const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(icon_size, icon_size));
			ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
		}
		if (ImGui::ImgBtn(ENV_SNOW, ImVec2(icon_size, icon_size), ImColor(255, 255, 255, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
		{
			t.visuals.iEnvironmentWeather = 3;
			t.gamevisuals.iEnvironmentWeather = t.visuals.iEnvironmentWeather;
			bEnableWeather = true;
			reset_env_particles();
			g.projectmodified = 1;
		}
		if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Snow Weather Effect");

		ImGui::GetStyle().FramePadding = oldstyle;

		ImGui::Separator();

		ImGui::TextCenter("Weather Intensity");
		if (ImGui::MaxSliderInputFloat("##fWeatherIntensity:", &t.visuals.fWeatherIntensity, 0.0f, 100.0f, "Set Weather Intensity"))
		{
			t.gamevisuals.fWeatherIntensity = t.visuals.fWeatherIntensity;
			Wicked_Update_Visuals((void *)&t.visuals);
			g.projectmodified = 1;
		}
	
		//Fog

		ImGui::TextCenter("Fog Range");

		float fogDist = sqrt( t.visuals.FogDistance_f );
		float fogStart = sqrt( t.visuals.FogNearest_f );
		if ( fogDist > 1000 ) fogDist = 1000;
		if (ImGui::MaxSliderInputRangeFloat("##WickedFogNewRange", &fogStart, &fogDist, 0.0, 1000.0f, "Set Fog Range"))
		{
			fogStart *= fogStart;
			fogDist *= fogDist;
			t.visuals.FogNearest_f = fogStart;
			t.visuals.FogDistance_f = fogDist;
			t.gamevisuals.FogNearest_f = t.visuals.FogNearest_f;
			t.gamevisuals.FogDistance_f = t.visuals.FogDistance_f;
			Wicked_Update_Visuals((void *)&t.visuals);
			g.projectmodified = 1;
		}
		
		/*
		if (ImGui::MaxSliderInputFloat("##WickedFogNearest_f", &t.visuals.FogNearest_f, 0.0f, 10000.0f, "Fog Nearest"))
		{
			t.gamevisuals.FogNearest_f = t.visuals.FogNearest_f;
			Wicked_Update_Visuals((void *)&t.visuals);
		}
		if (t.visuals.FogDistance_f >= 15000.0f)
			t.visuals.FogDistance_f = 15000.0f;
		if (ImGui::MaxSliderInputFloat("##WickedFogDistance_f", &t.visuals.FogDistance_f, 0.0f, 15000.0f, "Fog Distance"))
		{
			if (t.visuals.FogDistance_f >= 15000.0f)
				t.visuals.FogDistance_f = 1000000.0f; //Disable fog.
			t.gamevisuals.FogDistance_f = t.visuals.FogDistance_f;
			Wicked_Update_Visuals((void *)&t.visuals);
		}
		if (t.visuals.FogDistance_f >= 15000.0f)
			t.visuals.FogDistance_f = 1000000.0f; //Disable fog.
		*/

		ImGui::TextCenter("Fog Opacity");
		if (ImGui::MaxSliderInputFloat("##WickedFogHeight_f", &t.visuals.FogA_f, 0.0f, 1.0f, "Set Fog Opacity"))
		{
			ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_USE_FOG; //PE: Rick had it disabled at some point, think it has to do with developer mode (key_g), so just make sure to enable it here.
			t.gamevisuals.FogA_f = t.visuals.FogA_f;
			Wicked_Update_Visuals((void *)&t.visuals);
			g.projectmodified = 1;
		}

		//PE: Wicked fog height is not really usefull , as it dont set the height but turn into a "black" color ?
		//InWicked we use FogA_f as height.
		//if (ImGui::SliderFloat("##WickedFogHeight_f", &t.visuals.FogA_f, 0.0f, 2.0f))
		//{
		//	t.gamevisuals.FogA_f = t.visuals.FogA_f;
		//	bVisualUpdated = true;
		//}
		//if (ImGui::IsItemHovered()) ImGui::SetTooltip("Fog Height");

		ImGui::TextCenter("Horizon/Fog Color");
		ImVec4 mycolor = ImVec4(t.visuals.FogR_f / 255.0, t.visuals.FogG_f / 255.0, t.visuals.FogB_f / 255.0, 1.0);
		bool open_popup = ImGui::ColorButton("##NewV2WickedfHorizonColor", mycolor, 0, ImVec2(w - 20.0, 0));
		if (open_popup)
			ImGui::OpenPopup("##pickV2WickedfHorizonColor");
		if (ImGui::BeginPopup("##pickV2WickedfHorizonColor", ImGuiWindowFlags_NoMove))
		{
			if (ImGui::ColorPicker4("##pickerV2WickedfHorizonColor", (float*)&mycolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview))
			{
				t.gamevisuals.FogR_f = t.visuals.FogR_f = mycolor.x * 255.0;
				t.gamevisuals.FogG_f = t.visuals.FogG_f = mycolor.y * 255.0;
				t.gamevisuals.FogB_f = t.visuals.FogB_f = mycolor.z * 255.0;
				g.projectmodified = 1;
				Wicked_Update_Visuals((void *)&t.visuals);
				g.projectmodified = 1;
			}
			ImGui::EndPopup();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Fog Color");
		ImGuiWindow* window = ImGui::GetCurrentWindow(); //PE: Add a pencil to all color gadgets.
		ID3D11ShaderResourceView* lpTexture = GetImagePointerView(TOOL_PENCIL);
		ImVec2 vDrawPos = { ImGui::GetCursorScreenPos().x + (ImGui::GetContentRegionAvail().x - 30.0f) ,ImGui::GetCursorScreenPos().y - (ImGui::GetFontSize()*1.5f) - 3.0f };
		window->DrawList->AddImage((ImTextureID)lpTexture, vDrawPos, vDrawPos + ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));



#ifndef REMOVED_EARLYACCESS
		ImGui::TextCenter("Lighting");
		if (ImGui::MaxSliderInputFloat("##fWeatherLighting:", &t.visuals.fWeatherLighting, 0.0f, 100.0f, "Set Lighting (This feature is not yet available)"))
		{
			t.gamevisuals.fWeatherLighting = t.visuals.fWeatherLighting;
			Wicked_Update_Visuals((void *)&t.visuals);
			g.projectmodified = 1;
		}
		ImGui::TextCenter("Thunder");
		if (ImGui::MaxSliderInputFloat("##fWeatherThunder:", &t.visuals.fWeatherThunder, 0.0f, 100.0f, "Set Thunder (This feature is not yet available)"))
		{
			t.gamevisuals.fWeatherThunder = t.visuals.fWeatherThunder;
			Wicked_Update_Visuals((void *)&t.visuals);
			g.projectmodified = 1;
		}

		ImGui::TextCenter("Wind");
		if (ImGui::MaxSliderInputFloat("##fWeatherWind:", &t.visuals.fWeatherWind, 0.0f, 100.0f, "Set Wind (This feature is not yet available)"))
		{
			t.gamevisuals.fWeatherWind = t.visuals.fWeatherWind;
			Wicked_Update_Visuals((void *)&t.visuals);
			g.projectmodified = 1;
		}

#endif

		#ifdef POSTPROCESSRAIN
		//PE: Postprocess rain.
		if (t.visuals.iEnvironmentWeather == 1)
		{
			// Hidden for now
			/*
			if (ImGui::StyleCollapsingHeader("Rain Post Process", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushItemWidth(-10);
				if (ImGui::Checkbox("Rain Post Process##setRainEnabled", &t.visuals.bRainEnabled)) {
					t.gamevisuals.bRainEnabled = t.visuals.bRainEnabled;
					if (master_renderer)
						master_renderer->setRainEnabled(t.visuals.bRainEnabled); //PE: test post process shader.
					g.projectmodified = 1;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Enable Rain Post Process");
				ImGui::PopItemWidth();

				if (t.visuals.bRainEnabled)
				{
					ImGui::PushItemWidth(-10);
					ImGui::TextCenter("Rain Speed X");
					if (ImGui::SliderFloat("##Rain Speed X", &t.visuals.fRainSpeedX, -100.0, 100.0))
					{
						t.gamevisuals.fRainSpeedX = t.visuals.fRainSpeedX;
					}
					ImGui::PopItemWidth();

					ImGui::PushItemWidth(-10);
					ImGui::TextCenter("Rain Speed Y");
					if (ImGui::SliderFloat("##Rain Speed Y", &t.visuals.fRainSpeedY, -100.0, 100.0))
					{
						t.gamevisuals.fRainSpeedY = t.visuals.fRainSpeedY;
					}
					ImGui::PopItemWidth();


					ImGui::PushItemWidth(-10);
					ImGui::TextCenter("Rain Opacity");
					if (ImGui::SliderFloat("##Rain Opacity", &t.visuals.fRainOpacity, -1.0, 1.0))
					{
						master_renderer->setRainOpacity(t.visuals.fRainOpacity);
						t.gamevisuals.fRainOpacity = t.visuals.fRainOpacity;
					}
					ImGui::PopItemWidth();

					ImGui::PushItemWidth(-10);
					ImGui::TextCenter("Rain Scale X");
					if (ImGui::SliderFloat("##Rain Scal X", &t.visuals.fRainScaleX, -4.0, 4.0))
					{
						master_renderer->setRainScaleX(t.visuals.fRainScaleX);
						t.gamevisuals.fRainScaleX = t.visuals.fRainScaleX;
					}
					ImGui::PopItemWidth();

					ImGui::PushItemWidth(-10);
					ImGui::TextCenter("Rain Scale Y");
					if (ImGui::SliderFloat("##Rain Scal Y", &t.visuals.fRainScaleY, -4.0, 4.0))
					{
						master_renderer->setRainScaleY(t.visuals.fRainScaleY);
						t.gamevisuals.fRainScaleY = t.visuals.fRainScaleY;
					}
					ImGui::PopItemWidth();


					ImGui::PushItemWidth(-10);
					ImGui::TextCenter("Refreaction Scale");
					if (ImGui::SliderFloat("##RefreactionScale", &t.visuals.fRainRefreactionScale, 0.0, 0.5))
					{
						master_renderer->setRainRefreactionScale(t.visuals.fRainRefreactionScale);
						t.gamevisuals.fRainRefreactionScale = t.visuals.fRainRefreactionScale;
					}
					ImGui::PopItemWidth();
				}
			}
			*/
			//if (!bEnableWeather)
			//{
			//	if (master_renderer)
			//	{
			//		if(!t.visuals.bRainEnabled)
			//			master_renderer->setRainEnabled(false); //PE: test post process shader.
			//		else
			//			master_renderer->setRainEnabled(t.visuals.bRainEnabled); //PE: test post process shader.
			//	}
			//}
		}
		#endif

		#ifdef POSTPROCESSSNOW
		//PE: Postprocess rain.
		if (t.visuals.iEnvironmentWeather == 3)
		{
			// Hidden for now.
			/*if (ImGui::StyleCollapsingHeader("Snow Post Process", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushItemWidth(-10);
				if (ImGui::Checkbox("Snow Post Process##setSnowEnabled", &t.visuals.bSnowEnabled)) {
					t.gamevisuals.bSnowEnabled = t.visuals.bSnowEnabled;
					if (master_renderer)
						master_renderer->setSnowEnabled(t.visuals.bSnowEnabled); //PE: test post process shader.
					g.projectmodified = 1;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Enable Snow Post Process");
				ImGui::PopItemWidth();

				if (t.visuals.bSnowEnabled)
				{
					ImGui::PushItemWidth(-10);
					ImGui::TextCenter("Snow Speed");
					if (ImGui::SliderFloat("##Snow Speed", &t.visuals.fSnowSpeed, -30.0, 30.0))
					{
						t.gamevisuals.fSnowSpeed = t.visuals.fSnowSpeed;
					}
					ImGui::PopItemWidth();

					ImGui::PushItemWidth(-10);
					ImGui::TextCenter("Snow Layers");
					if (ImGui::SliderFloat("##Snow Layers", &t.visuals.fSnowLayers, 1.0, 20.0))
					{
						t.gamevisuals.fSnowLayers = t.visuals.fSnowLayers;
						master_renderer->setSnowLayers(t.visuals.fSnowLayers);
					}
					ImGui::PopItemWidth();


					ImGui::PushItemWidth(-10);
					ImGui::TextCenter("Snow Opacity");
					if (ImGui::SliderFloat("##Snow Opacity", &t.visuals.fSnowOpacity, 0.0, 1.0))
					{
						master_renderer->setSnowOpacity(t.visuals.fSnowOpacity);
						t.gamevisuals.fSnowOpacity = t.visuals.fSnowOpacity;
					}
					ImGui::PopItemWidth();

					ImGui::PushItemWidth(-10);
					ImGui::TextCenter("Snow Depth");
					if (ImGui::SliderFloat("##Snow Depth", &t.visuals.fSnowDepth, 0.01, 10.0))
					{
						master_renderer->setSnowDepth(t.visuals.fSnowDepth);
						t.gamevisuals.fSnowDepth = t.visuals.fSnowDepth;
					}
					ImGui::PopItemWidth();

					ImGui::PushItemWidth(-10);
					ImGui::TextCenter("Snow Wind");
					if (ImGui::SliderFloat("##Snow Wind", &t.visuals.fSnowWind, 0.01, 10.0))
					{
						master_renderer->setSnowWindiness(t.visuals.fSnowWind);
						t.gamevisuals.fSnowWind = t.visuals.fSnowWind;
					}
					ImGui::PopItemWidth();
				}
			}
			*/
		}
		#endif

		ImGui::Separator();

		if (ImGui::Checkbox("Display Weather in Editor##DisplayWeather", &bEnableWeather))
		{
			reset_env_particles();
			g.projectmodified = 1;
		}

		if (!bRenderTabTab && !pref.bHideTutorials)
		{
#ifndef REMOVED_EARLYACCESS
			if (ImGui::StyleCollapsingHeader("Tutorial (this feature is incomplete)", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Indent(10);
				cstr cShowTutorial = "01 - Getting started";
				char* tutorial_combo_items[] = { "01 - Getting started", "02 - Creating terrain", "03 - Add character and set a path" };
				SmallTutorialVideo(cShowTutorial.Get(), tutorial_combo_items, ARRAYSIZE(tutorial_combo_items), SECTION_WEATHER);
				float but_gadget_size = ImGui::GetFontSize()*12.0;
				float w = ImGui::GetWindowContentRegionWidth() - 20.0;
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
				#ifdef INCLUDESTEPBYSTEP
				if (ImGui::StyleButton("View Step by Step Tutorial", ImVec2(but_gadget_size, 0)))
				{
					bHelp_Window = true;
					bHelpVideo_Window = true;
					extern bool bSetTutorialSectionLeft;
					bSetTutorialSectionLeft = false;
					strcpy(cForceTutorialName, cShowTutorial.Get());
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Start Step by Step Tutorial");
				#endif

				ImGui::Indent(-10);
			}
#endif
		}


		ImGui::PopItemWidth();
		ImGui::Indent(-10);
	}
}

void imgui_Customize_Sky_V2(int mode)
{
	int wflags = ImGuiTreeNodeFlags_DefaultOpen;

	if (pref.bAutoClosePropertySections && iLastOpenHeader != 1)// && iLastOpenHeader < 14)
		ImGui::SetNextItemOpen(false, ImGuiCond_Always);

	if (ImGui::StyleCollapsingHeader("Customize Sky", wflags))
	{
		iLastOpenHeader = 1;
		ImGui::Indent(10);
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
		ImGui::PushItemWidth(-10);

		int oldSkyIndex = t.visuals.skyindex;

		char * current_sky = t.skybank_s[t.visuals.skyindex].Get();
		if (t.skybank_s[t.visuals.skyindex] == "None") current_sky = "Dynamic Clouds";
		if (!current_sky) current_sky = "NA";

		//PE: Simulated Sky
		bool bSimulatedSky = false;
		if (t.visuals.skyindex == 0)
			bSimulatedSky = true;

		static int iSkyType = 0;

		if (t.visuals.bDisableSkybox) iSkyType = 2;
		else if (!bSimulatedSky) iSkyType = 1;
		else iSkyType = 0;

		const char* sky_combo[] = { "Simulated Sky", "Sky Box" , "None" };
		if (ImGui::Combo("##Combosky_combo", &iSkyType, sky_combo, IM_ARRAYSIZE(sky_combo)))
		{
			bool bRunUpdateVisual = false;
			if (iSkyType == 0)
			{
				t.visuals.skyindex = 0;
				bSimulatedSky = true;
				t.gamevisuals.bDisableSkybox = t.visuals.bDisableSkybox = false;
			}
			if (iSkyType == 1)
			{
				t.visuals.skyindex = 1;
				bSimulatedSky = false;
				t.gamevisuals.bDisableSkybox = t.visuals.bDisableSkybox = false;
			}
			if (iSkyType == 2)
			{
				t.visuals.skyindex = 0;
				bSimulatedSky = false;
				t.gamevisuals.bDisableSkybox = t.visuals.bDisableSkybox = true;
			}

			//NONE only possible with new t.visuals.int
			extern wiECS::Entity g_weatherEntityID;
			wiScene::WeatherComponent* weather = wiScene::GetScene().weathers.GetComponent(g_weatherEntityID);
			int skyindex = 0;
			if (t.visuals.bDisableSkybox)
			{
				weather->SetRealisticSky(false);
				weather->SetVolumetricClouds(false);
			}
			else if (!bSimulatedSky) 
			{
				weather->SetRealisticSky( false );
				weather->SetVolumetricClouds( false );
				skyindex = 1;
			}
			else 
			{
				weather->SetRealisticSky( true );
				weather->SetVolumetricClouds( true );
				t.gamevisuals.iTimeOfday = t.visuals.iTimeOfday;
				visuals_calcsunanglefromtimeofday(t.gamevisuals.iTimeOfday, &t.gamevisuals.SunAngleX, &t.gamevisuals.SunAngleY, &t.gamevisuals.SunAngleZ);
				t.visuals.SunAngleX = t.gamevisuals.SunAngleX;
				t.visuals.SunAngleY = t.gamevisuals.SunAngleY;
				t.visuals.SunAngleZ = t.gamevisuals.SunAngleZ;
				bRunUpdateVisual = true;
			}

			t.visuals.skyindex = skyindex;
			g.projectmodified = 1;
			current_sky = t.skybank_s[skyindex].Get();//t.terrainstylebank_s[skyindex].Get();
			t.visuals.skyindex = skyindex;
			t.gamevisuals.skyindex = t.visuals.skyindex;

			g.skyindex = t.visuals.skyindex;
			t.visuals.sky_s = t.skybank_s[g.skyindex];
			t.gamevisuals.sky_s = t.skybank_s[g.skyindex];
			t.terrainskyspecinitmode = 0; sky_skyspec_init();
			t.sky.currenthour_f = 8.0;
			t.sky.daynightprogress = 0;

			visuals_justshaderupdate();

			// if change sky, regenerate env map
			t.visuals.refreshskysettingsfromlua = true;
			cubemap_generateglobalenvmap();
			t.visuals.refreshskysettingsfromlua = false;

			if (bRunUpdateVisual)
			{
				Wicked_Update_Visuals((void *)&t.visuals);
			}

			// when sky type changes, refresh env probes
			extern bool g_bLightProbeScaleChanged;
			g_bLightProbeScaleChanged = true;
			WickedCall_UpdateProbes();
		}

		bool somethingChanged = false;
		if ( oldSkyIndex != t.visuals.skyindex ) somethingChanged = true;

		ImGui::PopItemWidth();

		float w = ImGui::GetWindowContentRegionWidth();

		if (!bSimulatedSky)
		{
			ImGui::Indent(-10);
			//ImGui::Indent(4);
			ImGui::Indent(-2); //PE: Windows default to indent 4.

			int iMaxVisibleSkyBoxes = 8;
			static float fContentHeight = 0;
			static int iActiveSkybBoxes = 0;
			static ImVec2 vLastRunHeight = { 0,0 };
			if (fContentHeight <= 85) {
				fContentHeight = 85; //One line default. and prevent flicker.
			}
			vLastRunHeight = { 0 ,fContentHeight };
			//ImGui::BeginChild("##skybox4x4forscrollbar", vLastRunHeight, false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

			//if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {

			ImVec2 oldstylemain = ImGui::GetStyle().FramePadding;
			ImVec2 oldwinstylemain = ImGui::GetStyle().WindowPadding;
			ImGui::GetStyle().WindowPadding = { 0,0 };
			ImGui::GetStyle().FramePadding = { 0,0 };

			ImVec2 vWindowPos = ImGui::GetWindowPos();
			ImVec2 vWindowSize = ImGui::GetWindowSize();

			if(iActiveSkybBoxes > iMaxVisibleSkyBoxes)
				ImGui::BeginChild("##skybox4x4forscrollbar", vLastRunHeight, false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
			else
				ImGui::BeginChild("##skybox4x4forscrollbar", vLastRunHeight, false, ImGuiWindowFlags_NoScrollbar);

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 3));

			ImGui::Columns(4, "##skybox4x4columns", false);  //false no border

			iActiveSkybBoxes = 0;
			float curposy = ImGui::GetCursorPosY();
			for (int skyindex = 1; skyindex <= g.skymax; skyindex++)
			{

				if (t.skybank_s[skyindex].Len() > 0)
				{
					if (iActiveSkybBoxes++ <= iMaxVisibleSkyBoxes)
						fContentHeight = ImGui::GetCursorPosY() - curposy;

					cstr materialname = cstr(" ") + t.skybank_s[skyindex];

					int iLargerPreviewIconSize = 28;//PE: lowest possible icon
					float control_width = (iLargerPreviewIconSize + 3.0) * 4.0f + 6.0;

					if ( (w-ImGui::GetCurrentWindow()->ScrollbarSizes.x) > control_width) {
						//PE: fit perfectly with window width.
						iLargerPreviewIconSize = ( (w-ImGui::GetCurrentWindow()->ScrollbarSizes.x) - 20.0) / 4.0;
						//iLargerPreviewIconSize -= 6.0; //Padding.
						iLargerPreviewIconSize -= 7.0; //Padding.
						if (iLargerPreviewIconSize > 70) iLargerPreviewIconSize = 70;
					}
					iLargerPreviewIconSize += 2;

					ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;

					int iSkyIcon = TOOL_VISUALS;
					if (ImageExist(SKYBOX_ICONS + skyindex))
						iSkyIcon = SKYBOX_ICONS + skyindex;

					cStr sLabelChild = cStr("##skybox4x4") + cStr(skyindex);
					ImVec2 content_avail = { iLargerPreviewIconSize + 1.0f ,iLargerPreviewIconSize + 1.0f };

					ImGui::BeginChild(sLabelChild.Get(), content_avail, false, ImGuiWindowFlags_NoScrollbar);

					if (ImGui::ImgBtn(iSkyIcon, ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize), ImColor(0, 0, 0, 255)))
					{
						g.projectmodified = 1;
						current_sky = t.skybank_s[skyindex].Get();//t.terrainstylebank_s[skyindex].Get();
						t.visuals.skyindex = skyindex;
						t.gamevisuals.skyindex = t.visuals.skyindex;

						g.skyindex = t.visuals.skyindex;
						t.visuals.sky_s = t.skybank_s[g.skyindex];
						t.gamevisuals.sky_s = t.skybank_s[g.skyindex];
						t.terrainskyspecinitmode = 0;
						sky_skyspec_init();
						t.sky.currenthour_f = 8.0;
						t.sky.daynightprogress = 0;

						// must confirm new sky settings in t.gamevisuals
						t.gamevisuals.SunAngleX = t.visuals.SunAngleX;
						t.gamevisuals.SunAngleY = t.visuals.SunAngleY;
						t.gamevisuals.SunAngleZ = t.visuals.SunAngleZ;
						t.gamevisuals.SunRed_f = t.visuals.SunRed_f;
						t.gamevisuals.SunGreen_f = t.visuals.SunGreen_f;
						t.gamevisuals.SunBlue_f = t.visuals.SunBlue_f;
						t.gamevisuals.SunIntensity_f = t.visuals.SunIntensity_f;
						t.gamevisuals.fExposure = t.visuals.fExposure;
						t.gamevisuals.AmbienceRed_f = t.visuals.AmbienceRed_f;
						t.gamevisuals.AmbienceGreen_f = t.visuals.AmbienceGreen_f;
						t.gamevisuals.AmbienceBlue_f = t.visuals.AmbienceBlue_f;

						visuals_justshaderupdate();
						// if change sky, regenerate env map
						t.visuals.refreshskysettingsfromlua = true;
						cubemap_generateglobalenvmap();
						t.visuals.refreshskysettingsfromlua = false;

						WickedCall_UpdateProbes();
					}

					if (ImGui::IsItemHovered())
					{
						int tooltip_height = 200;
						int tooltip_width = 200;
						ImVec2 tooltip_position = ImVec2(ImGui::GetWindowPos());
						tooltip_position.x = vWindowPos.x - (tooltip_width + 10.0);
						if ((tooltip_position.y + tooltip_height) > (vWindowPos.y + vWindowSize.y))
							tooltip_position.y = (vWindowPos.y + vWindowSize.y) - tooltip_height - 10.0;
						ImGui::SetNextWindowPos(tooltip_position);
						ImGui::BeginTooltip();
						ImGui::ImgBtn(iSkyIcon, ImVec2(200, 200), ImColor(0, 0, 0, 255), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, true);
						ImGui::TextCenter(materialname.Get());
						ImGui::Separator();
						ImGui::EndTooltip();

						//ImGui::BeginTooltip();
						//ImGui::ImgBtn(iSkyIcon, ImVec2(200, 200), ImColor(0, 0, 0, 255), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, true);
						//ImGui::TextCenter(materialname.Get());
						//ImGui::Separator();
						//ImGui::EndTooltip();
					}

					ImGui::EndChild();

					ImGui::TextCenter(materialname.Get());

					if (current_sky == t.skybank_s[skyindex].Get())
					{
						ImVec2 padding = { 2.0, 2.0 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(iLargerPreviewIconSize, iLargerPreviewIconSize));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}

					ImGui::NextColumn();
				}
			}
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 2));

			if (iActiveSkybBoxes <= iMaxVisibleSkyBoxes)
				fContentHeight = 2.0+(ImGui::GetCursorPosY() - curposy);

			ImGui::GetStyle().WindowPadding = oldwinstylemain;
			ImGui::GetStyle().FramePadding = oldstylemain;

			ImGui::EndChild();
			//ImGui::Indent(-4);
			ImGui::Indent(2);
			ImGui::Columns(1);
			ImGui::Indent(10);
		}


		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

		//Must have, Sun color.
		float fAmbience[4], fSunColor[4], fHorizonColor[4], fZenith[4];
		extern wiECS::Entity g_weatherEntityID;
		wiScene::WeatherComponent* weather = wiScene::GetScene().weathers.GetComponent(g_weatherEntityID);

		ImGui::PushItemWidth(-10);

		ImGui::TextCenter("Lighting Color");
		ImVec4 mycolor = ImVec4(t.visuals.SunRed_f / 255.0, t.visuals.SunGreen_f / 255.0, t.visuals.SunBlue_f / 255.0,1.0);
		bool open_popup = ImGui::ColorButton("##NewV2WickedSunColor", mycolor, 0, ImVec2(w-20.0, 0));
		if (open_popup)
			ImGui::OpenPopup("##pickWickedSunColor");
		if ( ImGui::BeginPopup("##pickWickedSunColor", ImGuiWindowFlags_NoMove) )
		{
			if (ImGui::ColorPicker4("##pickerWickedSunColor", (float*)&mycolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview))
			{
				t.gamevisuals.SunRed_f = t.visuals.SunRed_f = mycolor.x * 255.0;
				t.gamevisuals.SunGreen_f = t.visuals.SunGreen_f = mycolor.y * 255.0;
				t.gamevisuals.SunBlue_f = t.visuals.SunBlue_f = mycolor.z * 255.0;
				g.projectmodified = 1;
				Wicked_Update_Visuals((void *)&t.visuals);
				// when sky type changes, refresh env probes
				extern bool g_bLightProbeScaleChanged;
				g_bLightProbeScaleChanged = true;
				WickedCall_UpdateProbes();
			}
			ImGui::EndPopup();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Sets the light that is being emitted from the sun into the scene");
		ImGuiWindow* window = ImGui::GetCurrentWindow(); //PE: Add a pencil to all color gadgets.
		ID3D11ShaderResourceView* lpTexture = GetImagePointerView(TOOL_PENCIL);
		ImVec2 vDrawPos = { ImGui::GetCursorScreenPos().x + (ImGui::GetContentRegionAvail().x - 30.0f) ,ImGui::GetCursorScreenPos().y - (ImGui::GetFontSize()*1.5f) - 3.0f };
		window->DrawList->AddImage((ImTextureID)lpTexture, vDrawPos, vDrawPos + ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));

		//PE: Add additional color options.
		ImGui::TextCenter("Ambient Color");
		mycolor = ImVec4(t.visuals.AmbienceRed_f / 255.0, t.visuals.AmbienceGreen_f / 255.0, t.visuals.AmbienceBlue_f / 255.0, 1.0);
		open_popup = ImGui::ColorButton("##NewV2WickedAmbientColor", mycolor, 0, ImVec2(w - 20.0, 0));
		if (open_popup)
			ImGui::OpenPopup("##pickWickedAmbientColor");
		if (ImGui::BeginPopup("##pickWickedAmbientColor", ImGuiWindowFlags_NoMove))
		{
			if (ImGui::ColorPicker4("##pickerWickedAmbientColor", (float*)&mycolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview))
			{
				t.gamevisuals.AmbienceRed_f = t.visuals.AmbienceRed_f = mycolor.x * 255.0;
				t.gamevisuals.AmbienceGreen_f = t.visuals.AmbienceGreen_f = mycolor.y * 255.0;
				t.gamevisuals.AmbienceBlue_f = t.visuals.AmbienceBlue_f = mycolor.z * 255.0;
				g.projectmodified = 1;
				Wicked_Update_Visuals((void *)&t.visuals);
				// when sky type changes, refresh env probes
				extern bool g_bLightProbeScaleChanged;
				g_bLightProbeScaleChanged = true;
				WickedCall_UpdateProbes();
			}
			ImGui::EndPopup();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("The general ambient light around the whole scene uses this color");
		ImGuiWindow* window2 = ImGui::GetCurrentWindow(); //PE: Add a pencil to all color gadgets.
		ID3D11ShaderResourceView* lpTexture2 = GetImagePointerView(TOOL_PENCIL);
		ImVec2 vDrawPos2 = { ImGui::GetCursorScreenPos().x + (ImGui::GetContentRegionAvail().x - 30.0f) ,ImGui::GetCursorScreenPos().y - (ImGui::GetFontSize()*1.5f) - 3.0f };
		window2->DrawList->AddImage((ImTextureID)lpTexture2, vDrawPos2, vDrawPos2 + ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));


		if (t.visuals.bDisableSkybox)
		{
			ImGui::TextCenter("Horizon/Fog Color");
			mycolor = ImVec4(t.visuals.FogR_f / 255.0, t.visuals.FogG_f / 255.0, t.visuals.FogB_f / 255.0, 1.0);
			open_popup = ImGui::ColorButton("##NewV2WickedHorizonColor", mycolor, 0, ImVec2(w - 20.0, 0));
			if (open_popup)
				ImGui::OpenPopup("##pickWickedHorizonColor");
			if (ImGui::BeginPopup("##pickWickedHorizonColor", ImGuiWindowFlags_NoMove))
			{
				if (ImGui::ColorPicker4("##pickerWickedHorizonColor", (float*)&mycolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview))
				{
					t.gamevisuals.FogR_f = t.visuals.FogR_f = mycolor.x * 255.0;
					t.gamevisuals.FogG_f = t.visuals.FogG_f = mycolor.y * 255.0;
					t.gamevisuals.FogB_f = t.visuals.FogB_f = mycolor.z * 255.0;
					g.projectmodified = 1;
					Wicked_Update_Visuals((void *)&t.visuals);
					// when sky type changes, refresh env probes
					extern bool g_bLightProbeScaleChanged;
					g_bLightProbeScaleChanged = true;
					WickedCall_UpdateProbes();
				}
				ImGui::EndPopup();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Horizon/Fog Color");
			window = ImGui::GetCurrentWindow(); //PE: Add a pencil to all color gadgets.
			lpTexture = GetImagePointerView(TOOL_PENCIL);
			vDrawPos = { ImGui::GetCursorScreenPos().x + (ImGui::GetContentRegionAvail().x - 30.0f) ,ImGui::GetCursorScreenPos().y - (ImGui::GetFontSize()*1.5f) - 3.0f };
			window->DrawList->AddImage((ImTextureID)lpTexture, vDrawPos, vDrawPos + ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));


			ImGui::TextCenter("Atmospheric Color");
			mycolor = ImVec4(t.visuals.ZenithRed_f / 255.0, t.visuals.ZenithGreen_f / 255.0, t.visuals.ZenithBlue_f / 255.0, 1.0);
			open_popup = ImGui::ColorButton("##NewV2WickedZenithColor", mycolor, 0, ImVec2(w - 20.0, 0));
			if (open_popup)
				ImGui::OpenPopup("##pickWickedZenithColor");
			if (ImGui::BeginPopup("##pickWickedZenithColor", ImGuiWindowFlags_NoMove))
			{
				if (ImGui::ColorPicker4("##pickerWickedZenithColor", (float*)&mycolor, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview))
				{
					t.gamevisuals.ZenithRed_f = t.visuals.ZenithRed_f = mycolor.x * 255.0;
					t.gamevisuals.ZenithGreen_f = t.visuals.ZenithGreen_f = mycolor.y * 255.0;
					t.gamevisuals.ZenithBlue_f = t.visuals.ZenithBlue_f = mycolor.z * 255.0;
					g.projectmodified = 1;
					Wicked_Update_Visuals((void *)&t.visuals);
					// when sky type changes, refresh env probes
					extern bool g_bLightProbeScaleChanged;
					g_bLightProbeScaleChanged = true;
					WickedCall_UpdateProbes();
				}
				ImGui::EndPopup();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Atmospheric Color");
			vDrawPos = { ImGui::GetCursorScreenPos().x + (ImGui::GetContentRegionAvail().x - 30.0f) ,ImGui::GetCursorScreenPos().y - (ImGui::GetFontSize()*1.5f) - 3.0f };
			window->DrawList->AddImage((ImTextureID)lpTexture, vDrawPos, vDrawPos + ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));

		}

		ImGui::TextCenter("Sun Intensity");
		float fTmp = t.visuals.SunIntensity_f * 10.0f;
		if (ImGui::SliderFloat("##WickedSunIntensity_f", &fTmp , 0.0, 500.0, "%.2f", 2.0f))
		{
			t.visuals.SunIntensity_f = fTmp * 0.1f;
			t.gamevisuals.SunIntensity_f = t.visuals.SunIntensity_f;
			Wicked_Update_Visuals((void *)&t.visuals);
			g.projectmodified = 1;
			// when sky type changes, refresh env probes
			extern bool g_bLightProbeScaleChanged;
			g_bLightProbeScaleChanged = true;
			WickedCall_UpdateProbes();
		}

		ImGui::TextCenter("Exposure");
		fTmp = t.visuals.fExposure * 100.0f;
		if (ImGui::SliderFloat("##fFixedExposure:", &fTmp, 0.0, 400.0, "%.2f", 2.0f))
		{
			t.visuals.fExposure = fTmp * 0.01f;
			t.gamevisuals.fExposure = t.visuals.fExposure;
			Wicked_Update_Visuals((void *)&t.visuals);
			g.projectmodified = 1;
			// when sky type changes, refresh env probes
			extern bool g_bLightProbeScaleChanged;
			g_bLightProbeScaleChanged = true;
			WickedCall_UpdateProbes();
		}

		if ( !bSimulatedSky || t.visuals.bDisableSkybox )
		{
		}
		else
		{
			
			ImGui::TextCenter("Cloud Density");
			fTmp = t.visuals.SkyCloudiness * 100.0f;
			if (ImGui::MaxSliderInputFloat("##V2WickedSkyCloudiness", &fTmp, 0.0f, 400.0f, "Sets how dense the clouds are", 0.0f, 400.0f))
			{
				t.visuals.SkyCloudiness = fTmp * 0.01f;
				t.gamevisuals.SkyCloudiness = t.visuals.SkyCloudiness;
				weather->cloudiness = t.visuals.SkyCloudiness;
				weather->volumetricCloudParameters.CoverageAmount = t.visuals.SkyCloudiness;
				g.projectmodified = 1;
				// when sky type changes, refresh env probes
				extern bool g_bLightProbeScaleChanged;
				g_bLightProbeScaleChanged = true;
				WickedCall_UpdateProbes();
			}
			ImGui::TextCenter("Cloud Coverage");
			//if (ImGui::SliderFloat("##V2WickedSkyCloudCoverage", &t.visuals.SkyCloudCoverage, 0.0f, 2.0f))
			fTmp = t.visuals.SkyCloudCoverage * 100.0f;
			if (ImGui::MaxSliderInputFloat("##V2WickedSkyCloudCoverage", &fTmp, 0.0f, 200.0f, "How much of the sky is covered with clouds", 0.0f, 200.0f ))
			{
				t.visuals.SkyCloudCoverage = fTmp * 0.01f;
				t.gamevisuals.SkyCloudCoverage = t.visuals.SkyCloudCoverage;
				weather->volumetricCloudParameters.CoverageMinimum = t.visuals.SkyCloudCoverage;
				g.projectmodified = 1;
				// when sky type changes, refresh env probes
				extern bool g_bLightProbeScaleChanged;
				g_bLightProbeScaleChanged = true;
				WickedCall_UpdateProbes();
			}
			ImGui::TextCenter("Cloud Height (meters)");
			float cloudHeight = GGTerrain_UnitsToMeters( t.visuals.SkyCloudHeight );
			//fTmp = cloudHeight * 0.1f;
			if (ImGui::SliderFloat("##V2WickedSkyCloudScale", &cloudHeight, -100, 3500, "%.0f", 2.0f))
			//if (ImGui::MaxSliderInputFloat("##V2WickedSkyCloudScale", &fTmp, -10, 350, "Positions the cloud layer higher up or lower down", -10, 350))
			{
				//cloudHeight = fTmp * 10.0f;
				t.visuals.SkyCloudHeight = GGTerrain_MetersToUnits( cloudHeight );
				t.gamevisuals.SkyCloudHeight = t.visuals.SkyCloudHeight;
				weather->cloudScale = t.visuals.SkyCloudHeight;
				weather->volumetricCloudParameters.CloudStartHeight = cloudHeight;
				g.projectmodified = 1;
				// when sky type changes, refresh env probes
				extern bool g_bLightProbeScaleChanged;
				g_bLightProbeScaleChanged = true;
				WickedCall_UpdateProbes();
			}
			ImGui::TextCenter("Cloud Thickness (meters)");
			cloudHeight = GGTerrain_UnitsToMeters( t.visuals.SkyCloudThickness );
		/*	if (ImGui::SliderFloat("##V2WickedSkyCloudThickness", &cloudHeight, 0, 4000))*/
			fTmp = cloudHeight * 0.1f;
			if (ImGui::MaxSliderInputFloat("##V2WickedSkyCloudThickness", &fTmp, 0, 400, "This is how thick the clouds are from top to bottom", 0, 400))
			{
				cloudHeight = fTmp * 10.0f;
				t.visuals.SkyCloudThickness = GGTerrain_MetersToUnits( cloudHeight );
				t.gamevisuals.SkyCloudThickness = t.visuals.SkyCloudThickness;
				weather->volumetricCloudParameters.CloudThickness = cloudHeight;
				g.projectmodified = 1;
				// when sky type changes, refresh env probes
				extern bool g_bLightProbeScaleChanged;
				g_bLightProbeScaleChanged = true;
				WickedCall_UpdateProbes();
			}
			ImGui::TextCenter("Cloud Speed");
			/*if (ImGui::SliderFloat("##V2WickedSkyCloudSpeed", &t.visuals.SkyCloudSpeed, 0.0f, 50.0f))*/
			if (ImGui::MaxSliderInputFloat("##V2WickedSkyCloudSpeed", &t.visuals.SkyCloudSpeed, 0.0f, 50.0f, "The cloud movement speed across the sky", 0.0f, 50.0f))
			{
				t.gamevisuals.SkyCloudSpeed = t.visuals.SkyCloudSpeed;
				weather->cloudSpeed = t.visuals.SkyCloudSpeed;
				weather->volumetricCloudParameters.CoverageWindSpeed = t.visuals.SkyCloudSpeed;
				weather->volumetricCloudParameters.WindSpeed = t.visuals.SkyCloudSpeed;
				g.projectmodified = 1;
			}
		}
		if (!t.visuals.bSimulate24Hours && (bSimulatedSky || t.visuals.bDisableSkybox) )
		{
			ImGui::TextCenter("Time of Day");
			const char* time_combo[] = { "Dawn", "Morning", "Midday","Afternoon", "Evening", "Dusk","Night" };
			if (ImGui::Combo("##ComboTimeOfDay", &t.visuals.iTimeOfday, time_combo, IM_ARRAYSIZE(time_combo)))
			{
				//PE: Now only used to set SunAngle one time, SunAngle is used everywhere from now on.
				// > 12 = 0-180 , < 12 = 180-360
				t.gamevisuals.iTimeOfday = t.visuals.iTimeOfday;
				visuals_calcsunanglefromtimeofday(t.gamevisuals.iTimeOfday, &t.gamevisuals.SunAngleX, &t.gamevisuals.SunAngleY, &t.gamevisuals.SunAngleZ);
				t.visuals.SunAngleX = t.gamevisuals.SunAngleX;
				t.visuals.SunAngleY = t.gamevisuals.SunAngleY;
				t.visuals.SunAngleZ = t.gamevisuals.SunAngleZ;
				Wicked_Update_Visuals((void *)&t.visuals);
				g.projectmodified = 1;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("This sets the sun at the correct position for the time of day");
 		}
		if (bSimulatedSky || t.visuals.bDisableSkybox )
		{
			extern void ControlAdvancedSetting(int&, const char*, bool* = nullptr);
			ControlAdvancedSetting(pref.iEnableAdvancedSky, "advanced sky settings");

			if (pref.iEnableAdvancedSky)
			{
				ImGui::TextCenter("Sun Direction X");
				if (ImGui::MaxSliderInputFloat("##sunAngleX:", &t.visuals.SunAngleX, 0.0, 360.0, "Sets the X direction of the sun"))
				{
					t.gamevisuals.SunAngleX = t.visuals.SunAngleX;
					Wicked_Update_Visuals((void *)&t.visuals);
					g.projectmodified = 1;
					// when sky type changes, refresh env probes
					extern bool g_bLightProbeScaleChanged;
					g_bLightProbeScaleChanged = true;
					WickedCall_UpdateProbes();
				}
				ImGui::TextCenter("Sun Direction Y");
				if (ImGui::MaxSliderInputFloat("##sunAngleY:", &t.visuals.SunAngleY, 0.0, 360.0, "Sets the Y direction of the sun"))
				{
					t.gamevisuals.SunAngleY = t.visuals.SunAngleY;
					Wicked_Update_Visuals((void *)&t.visuals);
					g.projectmodified = 1;
					// when sky type changes, refresh env probes
					extern bool g_bLightProbeScaleChanged;
					g_bLightProbeScaleChanged = true;
					WickedCall_UpdateProbes();
				}
				ImGui::TextCenter("Sun Direction Z");
				if (ImGui::MaxSliderInputFloat("##sunAngleZ:", &t.visuals.SunAngleZ, 0.0, 360.0, "Sets the Z direction of the sun"))
				{
					t.gamevisuals.SunAngleZ = t.visuals.SunAngleZ;
					Wicked_Update_Visuals((void *)&t.visuals);
					g.projectmodified = 1;
					// when sky type changes, refresh env probes
					extern bool g_bLightProbeScaleChanged;
					g_bLightProbeScaleChanged = true;
					WickedCall_UpdateProbes();
				}
			}
		}
		
		if ( somethingChanged )
		{
			Wicked_Update_Visuals((void *)&t.visuals);
			g.projectmodified = 1;
		}

		ImGui::Indent(-10);
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
	}
}

void terrain_resetfornewlevel (void)
{
	extern int g_iDisableTerrainSystem;
	if (g_iDisableTerrainSystem == 0)
	{
		// so loading new level, making new level clears sculpt data
		GGTerrain_ResetSculpting();
		reset_terrain_paint_date();
		undosys_terrain_preparefornewlevel();
	}
}

// Placeholders until new terrain system established
void terrain_initstyles(void) {}
void terrain_initstyles_reset ( void ) {}
void terrain_setupedit ( void ) {}
int terrain_loadcustomtexture(LPSTR pDestPathAndFile, int iTextureSlot) { return 0; }
cstr terrain_getterrainfolder ( void )  { return ""; }
void terrain_paintselector_hide ( void ) {}
void terrain_deletesupertexturepalette ( void ) {}
int terrain_createnewterraintexture ( LPSTR pDestTerrainTextureFile, int iWhichTextureOver, LPSTR pTexFileToLoad, int iSeamlessMode, int iCompressIt ) { return 0; }
void terrain_loadlatesttexture ( void ) {}
void terrain_changestyle ( void ) {}
void terrain_getpaintmode ( void ) {}
void terrain_terraintexturesystempainterentry ( void ) {}
void terrain_detectendofterraintexturesystempainter ( void ) {}

void terrain_UpdateInputAndEntities ( void )
{
	// copied from M-Terrain.cpp
	if ( t.conkit.entityeditmode == 0 )
	{
		// Control painter objects
		
		/*if ( t.inputsys.k_s == "1" )  t.terrain.terrainpaintermode = 1;
		if ( t.inputsys.k_s == "2" )  t.terrain.terrainpaintermode = 2;
		if ( t.inputsys.k_s == "3" )  t.terrain.terrainpaintermode = 3;
		if ( t.inputsys.k_s == "4" )  t.terrain.terrainpaintermode = 4;
		if ( t.inputsys.k_s == "5" )  t.terrain.terrainpaintermode = 5;
		if ( t.inputsys.k_s == "6" )  t.terrain.terrainpaintermode = 6;
		if ( t.inputsys.k_s == "7" )  t.terrain.terrainpaintermode = 7;
		if ( t.inputsys.k_s == "8" )  t.terrain.terrainpaintermode = 8;
		if ( t.inputsys.k_s == "9" )  t.terrain.terrainpaintermode = 9;
		if ( t.inputsys.k_s == "0" )  t.terrain.terrainpaintermode = 10;
		csForceKey2 = "1";
		*/

		// ZJ: Stopped checking actual input. 
		// From Task:"Some number keys seem to affect the right hand tool section ... This should not happen."
		if (csForceKey2 == "1")  t.terrain.terrainpaintermode = 1;
		if (csForceKey2 == "2")  t.terrain.terrainpaintermode = 2;
		if (csForceKey2 == "3")  t.terrain.terrainpaintermode = 3;
		if (csForceKey2 == "4")  t.terrain.terrainpaintermode = 4;
		if (csForceKey2 == "5")  t.terrain.terrainpaintermode = 5;
		if (csForceKey2 == "6")  t.terrain.terrainpaintermode = 6;
		if (csForceKey2 == "7")  t.terrain.terrainpaintermode = 7;
		if (csForceKey2 == "8")  t.terrain.terrainpaintermode = 8;
		if (csForceKey2 == "9")  t.terrain.terrainpaintermode = 9;
		if (csForceKey2 == "0")  t.terrain.terrainpaintermode = 10;
		if (csForceKey2 == "11")  t.terrain.terrainpaintermode = 11;
		if (csForceKey2 == "12")  t.terrain.terrainpaintermode = 12;

		t.tmin = 50; // 220216 - new standard size for both modes
		if ( t.inputsys.k_s == "-" && t.terrain.RADIUS_f > t.tmin )  t.terrain.RADIUS_f = t.terrain.RADIUS_f - ( 25 * t.terrain.ts_f );
		if ( t.inputsys.k_s == "=" && t.terrain.RADIUS_f < g.fTerrainBrushSizeMax )  t.terrain.RADIUS_f = t.terrain.RADIUS_f + ( 25 * t.terrain.ts_f );

		//PE: We will need to add t.entityelement [ t.e ].floorposy again to raise/lower objects when changing terrain.

		if ( t.terrain.terrainpaintermode >= 1 && t.terrain.terrainpaintermode <= 5 )
		{
			// this can also undo all InstanceStamp ( constructs )
			t.trevealallinstancestampentities = 0;

			// raise any entities subject to this terrain radius
			for ( t.e = 1; t.e <= g.entityelementlist; t.e++ )
			{
				t.entid = t.entityelement [ t.e ].bankindex;
				#ifdef WICKEDENGINE
				if (t.entityelement[t.e].floorposy > -89998 && t.entityelement[t.e].editorlock == false)
				#else
				if (t.entityelement[t.e].floorposy > 0 && t.entityelement[t.e].editorlock == false)
				#endif
				{
					t.obj = t.entityelement [ t.e ].obj;
					if ( g.gridlayershowsingle == 1 )
					{
						//  do not select if TAB slice mode active and entity too big (buildings, walls, etc)
						if ( t.obj > 0 )
						{
							if ( ObjectSizeX ( t.obj ) > 95 && ObjectSizeY ( t.obj ) > 95 && ObjectSizeZ ( t.obj ) > 95 )
							{
								t.obj = 0;
							}
						}
					}
					if ( t.obj > 0 )
					{
						if ( ObjectExist ( t.obj ) == 1 )
						{
							//LB: remembers original height from original floor, so can maintain things like items on tables, etc
							//t.tadjy_f = BT_GetGroundHeight (t.terrain.TerrainID, t.entityelement[t.e].x, t.entityelement[t.e].z) - t.entityelement[t.e].floorposy;
							t.tadjy_f = BT_GetGroundHeight (t.terrain.TerrainID, t.entityelement[t.e].x, t.entityelement[t.e].z) - t.entityelement[t.e].floorposy;
							if ( t.tadjy_f != 0 )
							{
								t.entityelement [ t.e ].y = t.entityelement [ t.e ].y + t.tadjy_f;
								if ( t.conkit.editmodeactive == 1 )
								{
									//  when in FPS 3D Edit Mode - when physics is ACTIVE - must reset entity in new position
									t.tphyobj = t.entityelement [ t.e ].obj;
									physics_disableobject ( );
									PositionObject ( t.tphyobj, t.entityelement [ t.e ].x, t.entityelement [ t.e ].y, t.entityelement [ t.e ].z );
									RotateObject ( t.tphyobj, t.entityelement [ t.e ].rx, t.entityelement [ t.e ].ry, t.entityelement [ t.e ].rz );
									physics_prepareentityforphysics ( );
									//  also record this change PERMINANTLY for return to editor
									if ( ArrayCount ( t.storedentityelement ) > 0 )
									{
										t.storedentityelement [ t.e ].y = t.storedentityelement [ t.e ].y + t.tadjy_f;
									}
								}
								else
								{
									PositionObject ( t.obj, t.entityelement [ t.e ].x, t.entityelement [ t.e ].y, t.entityelement [ t.e ].z );
								}
							}
						}
					}
					#ifdef WICKEDENGINE
					t.entityelement [ t.e ].floorposy = -90000.0;
					#else
					t.entityelement[t.e].floorposy = 0;
					#endif
				}
			}
			if ( t.trevealallinstancestampentities == 1 )
			{
				// we have edited the entities in the game itself, so remove
				// InstanceStamp (  system for manual entity edit mode )
				for ( t.e = 1; t.e <= g.entityelementlist; t.e++ )
				{
					t.entid = t.entityelement [ t.e ].bankindex;
					t.obj = t.entityelement [ t.e ].obj;
					if ( t.obj > 0 )
					{
						if ( ObjectExist ( t.obj ) == 1 )
						{
							if ( t.entityprofile [ t.entid ].ismarker == 0 )
							{
								ShowObject ( t.obj );
							}
						}
					}
				}
			}
		}
	}
}

void terrain_editcontrol ( void )
{
	//PE: Update status from input.
	terrain_UpdateInputAndEntities();

	/* g_pTerrain no longer used
	// check terrain is valid first
	if ( !g_pTerrain )
		return;

	// pass relevant information across to the terrain
	g_pTerrain->SetEditRadius   ( t.terrain.RADIUS_f );

	// pass the edit position - must make sure these values are different to 0 otherwise it screws up
	if ( t.inputsys.activemouse )
	{
		if ( t.inputsys.localx_f != 0 && t.inputsys.localy_f != 0 )
			g_pTerrain->SetEditPosition ( t.inputsys.localx_f, t.inputsys.localy_f );
	}

	g_pTerrain->SetDeltaTime    ( ImGui::GetIO ( ).DeltaTime );
	g_pTerrain->SetMouseState   ( ( unsigned int ) t.inputsys.mclick );

	if( t.inputsys.keyshift == 1 )
		g_pTerrain->SetRaiseMode(0);
	else
		g_pTerrain->SetRaiseMode    ( iTerrainRaiseMode );

	g_pTerrain->SetEditMode(0);

	if ( current_mode == TOOL_SHAPE )
	{
		g_pTerrain->SetEditMode ( MaxTerrain::TERRAIN_EDIT_SCULPT );
		g_pTerrain->SetBrushMode ( 2 );
	}
	else if ( current_mode == TOOL_LEVELMODE )
	{
		g_pTerrain->SetEditMode ( MaxTerrain::TERRAIN_EDIT_SCULPT );
		g_pTerrain->SetBrushMode ( 4 );
	}
	else if ( current_mode == TOOL_BLENDMODE )
	{
		g_pTerrain->SetEditMode ( MaxTerrain::TERRAIN_EDIT_SCULPT );
		g_pTerrain->SetBrushMode ( 5 );
	}
	else if ( current_mode == TOOL_STOREDLEVEL )
	{
		g_pTerrain->SetEditMode ( MaxTerrain::TERRAIN_EDIT_SCULPT );
		g_pTerrain->SetBrushMode ( 6 );
	}
	else if ( current_mode == TOOL_PAINTTEXTURE )
	{
		g_pTerrain->SetEditMode ( MaxTerrain::TERRAIN_EDIT_PAINT );
	}
	else if ( current_mode == TOOL_RAMPMODE )
	{
		// I don't know how this should work with the input. Please note the
		// actual function is set up and you can call Ramp and pass in the two
		// points that form the line
	}

	// allow editing
	g_pTerrain->Edit ( );
	*/
}

void terrain_recordbuffer ( void ) {}
void terrain_undo ( void ) {}
void terrain_redo ( void ) {}
void terrain_editcontrol_auxiliary ( void ) {}
void terrain_paintterrain ( void ) {}
void terrain_cursor ( void ) {}
void terrain_cursor_nograsscolor ( void ) {}
void terrain_cursor_off ( void ) {}
void terrain_renderonly ( void ) {}

float BT_GetGroundHeight ( unsigned long value, float x, float z )
{
	#ifdef GGTERRAIN_USE_NEW_TERRAIN
	extern int g_iDisableTerrainSystem;
	if (g_iDisableTerrainSystem == 0)
	{

		float height;
		if (GGTerrain_GetHeight(x, z, &height)) return height;
		else return GGORIGIN_Y;
	}
	else
		return GGORIGIN_Y;
	#endif

	/* g_pTerrain no longer used
	// moved this function into here so it's in the same place as the terrain
	if ( !g_pTerrain ) return g.gdefaultterrainheight;

	// this might be explained by the threads loading things in as you move around as
	// it will return 0 if meshes are being loaded in, this is because of a clash
	// and an alteration needs to be made with the way things happen when the thread
	// is active, see more details here - https://thegamecreators.teamwork.com/#/tasks/20283106
	float fMikeThisHeightIsNotConsistentAsCameraMovesAbout = g_pTerrain->GetHeight ( x, z );
	//PE: Only if it fails.
	if (fMikeThisHeightIsNotConsistentAsCameraMovesAbout == 0.0f)
		fMikeThisHeightIsNotConsistentAsCameraMovesAbout = g.gdefaultterrainheight;
	return fMikeThisHeightIsNotConsistentAsCameraMovesAbout;
	*/
}

float BT_GetGroundHeight ( unsigned long value, float x, float z, bool dsadsadsa )
{
	#ifdef GGTERRAIN_USE_NEW_TERRAIN
	extern int g_iDisableTerrainSystem;
	if (g_iDisableTerrainSystem == 0)
	{

		float height;
		if (GGTerrain_GetHeight(x, z, &height)) return height;
		else return GGORIGIN_Y;
	}
	else
		return GGORIGIN_Y;
	#endif

	/* g_pTerrain no longer used
	// moved this function into here so it's in the same place as the terrain - no idea what the final parameter is
	if ( !g_pTerrain ) return g.gdefaultterrainheight;
	float fHeight = g_pTerrain->GetHeight(x, z);
	if (fHeight <= 0) {
		//PE: g_pTerrain->GetHeight(x, z); dont seam to work when m_bMultithreading is true ?
		fHeight = g.gdefaultterrainheight;
	}
	return fHeight;
	*/
}

void terrain_clearterraindirtyregion ( void ) {}
void terrain_cleargrassdirtyregion ( void ) {}
void terrain_cleardirtyregion ( void ) {}
void terrain_waterineditor ( void ) {}
void terrain_assignnewshader ( void ) {}
void terrain_applyshader ( void ) {}
void terrain_make ( void ) {}
void terrain_make_image_only(void) {}

void terrain_load ( char* pLevelBankLocation )
{
	int k = 0;
}

void terrain_save ( char* pLevelBankLocation )
{
	/* g_pTerrain no longer used
	// nothing to save if no terrain
	if ( !g_pTerrain )
		return;

	// string passed in points to levelbank\testmap folder used to store all FPM level data
	g_pTerrain->Save ( pLevelBankLocation );	
	*/
}

void terrain_savetextures ( void ) {}
void terrain_generatevegandmask_grab ( void ) {}
void terrain_generatevegandmaskfromterrain ( void ) {}
void terrain_generateblanktextures ( void ) {}
void terrain_loaddata ( void ) {}
void terrain_delete ( void )
{
	int k = 0;
}
void terrain_updaterealheights ( void ) {}
void terrain_randomiseorflattenterrain ( void ) {}
void terrain_flattenterrain ( void ) {}
void terrain_randomiseterrain ( void ) {}
void terrain_refreshterrainmatrix ( void ) 
{
	int k = 0;
}
void terrain_skipifnowaterexposed ( void ) {}
void terrain_updatewatermask ( void ) {}
void terrain_updatewatermask_new ( void ) {}
void terrain_whitewashwatermask ( void ) {}
void terrain_createheightmapfromheightdata ( void ) {}
void terrain_quickupdateheightmapfromheightdata ( void ) 
{
	int k = 0;
}
void terrain_generatetextureselect ( void ) {}
void terrain_generatesupertexture ( bool bForceRecalcOfPalette );
void terrain_generateshadows ( void ) {}
void generate_terrain ( int seed, int scale, int mchunk_size );
void DiamondSquare(unsigned x1, unsigned y1, unsigned x2, unsigned y2, float range, unsigned level) ;
void terrain_start_play ( void ) 
{
	/* g_pTerrain no longer used
	// this must be called to ensure we refresh internal lists
	if ( !g_pTerrain )
		return;
	// thought this would be the place to start physics but calling it here crashes
	*/
}

void terrain_stop_play ( void ) 
{
	/* g_pTerrain no longer used
	// this must be called to ensure we refresh internal lists
	if ( g_pTerrain )
		g_pTerrain->PrepareLevel ( );
	*/
}

void terrain_setfog ( void ) {}
void terrain_water_init ( void ) {}
void terrain_water_free ( void ) {}
void terrain_updatewatermechanism ( void ) {}
void terrain_updatewaterphysics ( void ) {}
void terrain_water_setfog ( void ) {}

#ifdef PROCEDURALTERRAINWINDOW

//Variables to use else where.
float fTerrainHeightStart = 0.0f; //Meters
bool bShowEditArea = true;
bool bShow3DBoundary = true;
bool bShowMiniMap = true;
float fWaterDepthMeters = -200.0;

int iTriggerInvalidateAfterFrames = 0;
void check_new_terrain_parameters(void)
{
	//PE: Trees is now only controlled by visual.ini t.visuals.bEndableTreeDrawing
	//if (ggterrain_extra_params.iProceduralTerrainType == 3 || ggterrain_extra_params.iProceduralTerrainType == 5 || ggterrain_extra_params.iProceduralTerrainType == 7)
	//	ggtrees_global_params.draw_enabled = 1;
	//else
	//	ggtrees_global_params.draw_enabled = 0;

	iTriggerInvalidateAfterFrames = 22; //PE: Also Need to be delayed so new terrain data have been updated.
	GGTerrain::GGTerrain_InvalidateRegion(-1000000.0, -1000000.0, 1000000.0, 1000000.0, GGTERRAIN_INVALIDATE_ALL);

}

void procedural_set_heightmap_level(void)
{
	ggterrain_global_params.seed = Random2();
	ggterrain_global_params.offset_y = 0.0; GGTerrain_MetersToUnits(5.8f); //228.3
	ggterrain_global_params.offset_x = 0.0;
	ggterrain_global_params.height = 5000.0; //GGTerrain_MetersToUnits(131.5f); //5,177.155
	ggterrain_global_params.minHeight = 5000.0; // GGTerrain_MetersToUnits(131.5f); //5,177.155
	ggterrain_global_params.noise_power = 1.6f;
	ggterrain_global_params.noise_fallof_power = 0.27f;
	ggterrain_global_params.fractal_levels = 6;
	ggterrain_global_params.fractal_initial_freq = 0.3f;
	ggterrain_global_params.fractal_freq_increase = 2.5f;
	ggterrain_global_params.fractal_freq_weight = 0.4;
	ggterrain_global_render_params.baseLayerMaterial = 0x100 | 25;
	ggterrain_global_render_params.layerMatIndex[0] = 0x100 | 24;
	ggterrain_global_render_params.layerMatIndex[1] = 0x100 | 28;
	ggterrain_global_render_params.layerMatIndex[2] = 0x100 | 29;
	ggterrain_global_render_params.layerStartHeight[0] = GGTerrain_MetersToUnits(0.0f);
	ggterrain_global_render_params.layerStartHeight[1] = GGTerrain_MetersToUnits(4.572f);
	ggterrain_global_render_params.layerStartHeight[2] = GGTerrain_MetersToUnits(145.7f);
	ggterrain_global_render_params.layerStartHeight[3] = GGTerrain_MetersToUnits(1500.0f);
	ggterrain_global_render_params.layerStartHeight[4] = GGTerrain_MetersToUnits(1500.0f);
	ggterrain_global_render_params.layerEndHeight[0] = GGTerrain_MetersToUnits(1.524f);
	ggterrain_global_render_params.layerEndHeight[1] = GGTerrain_MetersToUnits(9.144f);
	ggterrain_global_render_params.layerEndHeight[2] = GGTerrain_MetersToUnits(205.7f);
	ggterrain_global_render_params.layerEndHeight[3] = GGTerrain_MetersToUnits(1500.0f);
	ggterrain_global_render_params.layerEndHeight[4] = GGTerrain_MetersToUnits(1500.0f);
	ggterrain_global_render_params.slopeMatIndex[0] = 0x100 | 26;
	ggterrain_global_render_params.slopeMatIndex[1] = 0x100 | 4;
	ggterrain_global_render_params.slopeStart[0] = 0.1f;
	ggterrain_global_render_params.slopeStart[1] = 1.0f;
	ggterrain_global_render_params.slopeEnd[0] = 0.3f;
	ggterrain_global_render_params.slopeEnd[1] = 1.0f;

	ggterrain_global_params.fractal_flags = (ggterrain_global_params.fractal_flags & ~GGTERRAIN_FRACTAL_RIDGES0) | GGTERRAIN_FRACTAL_VALLEYS0;
	ggterrain_global_params.fractal_flags = ggterrain_global_params.fractal_flags & ~(GGTERRAIN_FRACTAL_VALLEYS1 | GGTERRAIN_FRACTAL_RIDGES1);
	ggterrain_global_params.fractal_flags = ggterrain_global_params.fractal_flags & ~(GGTERRAIN_FRACTAL_VALLEYS2 | GGTERRAIN_FRACTAL_RIDGES2);
	ggterrain_global_params.fractal_flags = ggterrain_global_params.fractal_flags & ~(GGTERRAIN_FRACTAL_VALLEYS3 | GGTERRAIN_FRACTAL_RIDGES3);

	// ggtrees_global_params.draw_enabled = 0; //PE: Trees is now only controlled by visual.ini t.visuals.bEndableTreeDrawing

	t.visuals.bWaterEnable = true;
	t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
	Wicked_Update_Visuals((void *)&t.visuals);

	//Create Level
	ggterrain_extra_params.iProceduralTerrainType = 1;
}


void procedural_set_empty_level(bool bWaterReset)
{
	GGTerrain_RemoveHeightMap();
	ggterrain_extra_params.iProceduralTerrainType = 0;
	ggterrain_global_params.offset_y = GGTerrain_MetersToUnits(0.0f);
	ggterrain_global_params.height = GGTerrain_MetersToUnits(500); // now made flat with fractal_initial_amplitude
	ggterrain_global_params.minHeight = GGTerrain_MetersToUnits(500);
	ggterrain_global_params.noise_power = 1.6f;
	ggterrain_global_params.noise_fallof_power = 0.27f;
	ggterrain_global_params.fractal_levels = 1;
	ggterrain_global_params.fractal_initial_freq = 0.3f;
	ggterrain_global_params.fractal_initial_amplitude = 0.0f;
	ggterrain_global_params.fractal_freq_increase = 2.7f;
	ggterrain_global_params.fractal_freq_weight = 0.4;
	ggterrain_global_render_params.baseLayerMaterial = 0x100 | (32 - 1);
	ggterrain_global_render_params.layerMatIndex[0] = 0x100 | (32 - 1);
	ggterrain_global_render_params.layerMatIndex[1] = 0x100 | (32 - 1);
	ggterrain_global_render_params.layerMatIndex[2] = 0x100 | (32 - 1);
	ggterrain_global_render_params.layerStartHeight[0] = GGTerrain_MetersToUnits(0.0f);
	ggterrain_global_render_params.layerStartHeight[1] = GGTerrain_MetersToUnits(1500.0f);
	ggterrain_global_render_params.layerStartHeight[2] = GGTerrain_MetersToUnits(1500.0f);
	ggterrain_global_render_params.layerStartHeight[3] = GGTerrain_MetersToUnits(1500.0f);
	ggterrain_global_render_params.layerStartHeight[4] = GGTerrain_MetersToUnits(1500.0f);
	ggterrain_global_render_params.layerEndHeight[0] = GGTerrain_MetersToUnits(1.0f);
	ggterrain_global_render_params.layerEndHeight[1] = GGTerrain_MetersToUnits(1500.0f);
	ggterrain_global_render_params.layerEndHeight[2] = GGTerrain_MetersToUnits(1500.0f);
	ggterrain_global_render_params.layerEndHeight[3] = GGTerrain_MetersToUnits(1500.0f);
	ggterrain_global_render_params.layerEndHeight[4] = GGTerrain_MetersToUnits(1500.0f);
	ggterrain_global_render_params.slopeMatIndex[0] = 0x100 | (32 - 1);
	ggterrain_global_render_params.slopeMatIndex[1] = 0x100 | (32 - 1);
	ggterrain_global_render_params.slopeStart[0] = 0.2f;
	ggterrain_global_render_params.slopeStart[1] = 1.0f;
	ggterrain_global_render_params.slopeEnd[0] = 0.4f;
	ggterrain_global_render_params.slopeEnd[1] = 1.0f;

	if (bWaterReset)
	{
		// remove water from grey grid view
		t.visuals.bWaterEnable = false;
		t.terrain.waterliney_f = g.gdefaultwaterheight = -500.0f;
		t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
		Wicked_Update_Visuals((void *)&t.visuals);
		ggterrain_extra_params.iUpdateTrees = 10;
	}
}



void wicked_set_water_level(int water_height)
{
	g.gdefaultwaterheight = water_height;
	t.terrain.waterliney_f = g.gdefaultwaterheight;

	if (ggterrain_extra_params.iProceduralTerrainType == 0)
	{
		t.visuals.bWaterEnable = false;
		t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
	}
	else
	{
		t.visuals.bWaterEnable = true;
		t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
	}
	Wicked_Update_Visuals((void *)&t.visuals);
	ggterrain_extra_params.iUpdateTrees = 1;
}

#define NOMINIMAP
#define RANDOMSKYBOX
extern float fWickedMaxCenterTest;
bool bTriggerTerrainSaveAsWindow = false;
bool bFirstTimeVeg = true;
#define USEFULLVIEWPORT
#define DIGAHOLE

static inline ImVec2 ImGuiRotation(const ImVec2& v, float cos_a, float sin_a) { return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a); }

void procedural_new_level(void)
{
	bool bUseFullScreen = true;
	bool bUseNoTitleBar = true;
	bool bUseModal = false;
	float fMaxCameraY = 210000.0f;// 800000.0f PE: To hide the big ugly shadow max here.

	static int iLargePreviewImageID = 0;
	static float oldFogNear, oldFogFar, oldCameraFAR_f, oldfShadowFarPlane;
	static int oldSkyIndex;
	static bool oldbFXAAEnabled , oldbDisableSkybox;
	static bool bIs2DViewHovered = false;
	static float fOldCamx = 0, fOldCamy = 0, fOldCamz = 0, fOldCamAx = 0, fOldCamAy = 0, fOldCamAz = 0, oldSunAngleX, oldSunAngleY, oldSunAngleZ;
	static float oldFogR_f, oldFogG_f, oldFogB_f, oldFogA_f, oldZenithRed_f, oldZenithGreen_f, oldZenithBlue_f, oldSkyCloudHeight;
	static int iCamMode,oldflags2;
	static int movecameratotarget = 0;


	//For squere look (non fullscreen) , 830x800 so we are same size as terrain rect.
	int preview_size_x = 830;
	int preview_size_y = 800;

	if (bUseFullScreen)
	{
		//PE: Use full available area.
		preview_size_x = ImGui::GetMainViewport()->Size.x - 300.0;
		//if (!bUseNoTitleBar)
		//	preview_size_y = ImGui::GetMainViewport()->Size.y - 80.0;
		//else
		//	preview_size_y = ImGui::GetMainViewport()->Size.y - 60.0;

		//PE: without the lower buttons.
		if (!bUseNoTitleBar)
			preview_size_y = ImGui::GetMainViewport()->Size.y - 20.0;
		else
			preview_size_y = ImGui::GetMainViewport()->Size.y - 10.0;
	}

	//###################################
	//#### Procedural Level Preview. ####
	//###################################

	extern bool bDigAHoleToHWND;
	extern D3D11_RECT rD3D11DigAHole;
	static bool bNeedReloadTextures = true;

	if (bProceduralLevel)
	{
		if (bNeedReloadTextures)
		{
			g_iDeferTextureUpdateToNow = 1;
			t.visuals.customTexturesFolder = "";
			bNeedReloadTextures = false;
			for (int i = 0; i < 32; i++)
			{
				// Default to all generic material sounds
				g_iCustomTerrainMatSounds[i] = 10;
			}
			// Trigger update of material sounds
			extern bool g_bMapMatIDToMatIndexAvailable;
			g_bMapMatIDToMatIndexAvailable = false;
		}
		ImGuiIO& io = ImGui::GetIO();

		bImGuiGotFocus = true;

		static int TriggerCloseAllAfterSaveLevel = 0;

		if (TriggerCloseAllAfterSaveLevel > 0)
		{
			TriggerCloseAllAfterSaveLevel--;
			if (TriggerCloseAllAfterSaveLevel == 0)
			{
				bProceduralLevel = false;
				bNeedReloadTextures = true;
				extern bool bStoryboardWindow;
				bStoryboardWindow = false;
			}
		}
		
		int save_level_as(void);
		int iRet = save_level_as();
		if (iRet == 2)
		{
			//New level saved, close down and go to editor.
			TriggerCloseAllAfterSaveLevel = 5;
		}
		if (iRet == 1)
		{
			//Cancel save as. Restore settings and continue.
			ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
			ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP;
			ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
			ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_USE_FOG;
			#ifdef NOMINIMAP
			ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP;
			#endif
		}

		int i3DViewHeight = 1181; //30 meters. was(800)
		static bool bTriggerStableY = false;
		static bool bUseSphere = false;
		//PE: Need to check if backbuffer size changed.
		if (!bPopModalOpenProcedural)
		{
			//Open window and setup defaults.
			if(bUseModal) ImGui::OpenPopup("##ProceduralPreview");

			if (!bUseFullScreen)
			{
				ImGui::SetNextWindowSize(ImVec2(1196.0f, 888));
			}

			//Delete backbuffer and create a new.
			if (BitmapExist(99))
			{
				DeleteBitmapEx(99);
			}

			extern bool BackBufferIsGroup;
			BackBufferIsGroup = false;
			BackBufferEntityID = 0;
			BackBufferObjectID = 0;
			BackBufferImageID = g.importermenuimageoffset + 50;
			iLargePreviewImageID = BackBufferImageID;
			#ifdef USEFULLVIEWPORT
			BackBufferSizeX = ImGui::GetMainViewport()->Size.x; //preview_size_x;
			BackBufferSizeY = ImGui::GetMainViewport()->Size.y; //preview_size_y;
			#else
			BackBufferSizeX = preview_size_x;
			BackBufferSizeY = preview_size_y;
			#endif
			BackBufferZoom = 1.0f;
			BackBufferCamLeft = 0.0f;
			BackBufferCamUp = 0.0f;
			bRotateBackBuffer = false;
			bBackBufferAnimated = false;
			bLoopBackBuffer = false;
			BackBufferSaveCacheName = ""; //No saving on tooltip images
			//Dont fit snapshot to rubber band.
			fLastRubberBandX1 = fLastRubberBandX2 = fLastRubberBandY1 = fLastRubberBandY2 = 0.0f;

			//Control camera snap shot.
			#ifdef DIGAHOLE
			bSnapShotModeUseCamera = false;
			#else
			bSnapShotModeUseCamera = true;
			#endif

			if (!bPopModalOpenProceduralCameraMode)
			{
				//PE: Now start up high looking at the editable area.
				fSnapShotModeCameraX = 73800; //New distance.
				fSnapShotModeCameraZ = -74000;
				fSnapShotModeCameraY = 51600;
				fSnapShotModeCameraAngZ = 0.0f;
				fSnapShotModeCameraAngY = -37;
				fSnapShotModeCameraAngX = 25; //PE: New 3D angle, was 34;
				bTriggerStableY = true; //PE: Make sure after terrain generate that the Y is correct.
			}
			else
			{
				//Take cam directly. controlled by load level.
				fSnapShotModeCameraX = CameraPositionX();
				fSnapShotModeCameraY = CameraPositionY();
				fSnapShotModeCameraZ = CameraPositionZ();
				fSnapShotModeCameraAngX = CameraAngleX();
				fSnapShotModeCameraAngY = CameraAngleY();
				fSnapShotModeCameraAngZ = CameraAngleZ();
			}
			oldFogNear = t.visuals.FogNearest_f;
			oldFogFar = t.visuals.FogDistance_f;
			oldSkyIndex = t.visuals.skyindex;
			oldbDisableSkybox = t.visuals.bDisableSkybox;
			oldCameraFAR_f = t.visuals.CameraFAR_f;
			oldfShadowFarPlane = t.visuals.fShadowFarPlane;
			oldbFXAAEnabled = t.visuals.bFXAAEnabled;

			oldSunAngleX = t.visuals.SunAngleX;
			oldSunAngleY = t.visuals.SunAngleY;
			oldSunAngleZ = t.visuals.SunAngleZ;

			oldZenithRed_f = t.visuals.ZenithRed_f;
			oldZenithGreen_f = t.visuals.ZenithGreen_f;
			oldZenithBlue_f = t.visuals.ZenithBlue_f;

			oldFogR_f = t.visuals.FogR_f;
			oldFogG_f = t.visuals.FogG_f;
			oldFogB_f = t.visuals.FogB_f;
			oldFogA_f = t.visuals.FogA_f;

			oldSkyCloudHeight = t.visuals.SkyCloudHeight;

			//PE: Dont change visuals in bPopModalOpenProceduralCameraMode
			if (!bPopModalOpenProceduralCameraMode)
			{
				t.visuals.FogA_f = 0.30; //0.4; //PE: Just a little fog. so you still can see terrain when way up (2D view).
				t.visuals.FogNearest_f = 120000; //50000;
				t.visuals.FogDistance_f = 500000; //700000;
				t.visuals.skyindex = 0; //Disable clouds. just use static skybox settings (new sky not really activated).
				#ifdef DIGAHOLE
				t.visuals.bDisableSkybox = false; //PE: Test dynamic sky.
				#else
				t.visuals.bDisableSkybox = true;
				t.visuals.bFXAAEnabled = false; //FXAA ruin custom backbuffer ?
				#endif
				t.visuals.CameraFAR_f = fMaxCameraY + 100000.0f;
				fWickedMaxCenterTest = 320000.0f; //PE: Hide ugly shadow.
				Wicked_Update_Visuals((void *)&t.visuals);
			}
			else
			{
				#ifdef DIGAHOLE
				//PE: No changes when we have a hole to hwnd :)
				//t.visuals.bDisableSkybox = false; //PE: Test dynamic sky.
				#else
				//PE: Clouds look strange in take screenshot mode ? must disable for now!
				t.visuals.skyindex = 0; //Disable clouds. just use static skybox settings (new sky not really activated).
				t.visuals.bDisableSkybox = true;
				t.visuals.bFXAAEnabled = false; //FXAA ruin custom backbuffer ?
				Wicked_Update_Visuals((void *)&t.visuals);
				#endif
			}
			WickedCall_DisplayCubes(false); //Hide cubes.
			editor_toggle_element_vis(false);

			fOldCamAx = CameraAngleX();
			fOldCamAy = CameraAngleY();
			fOldCamAz = CameraAngleZ();
			fOldCamx = GetCameraPosition().x;
			fOldCamy = GetCameraPosition().y;
			fOldCamz = GetCameraPosition().z;
			iCamMode = t.editorfreeflight.mode;

			t.editorfreeflight.mode = 1;

			oldflags2 = ggterrain_global_render_params2.flags2;

			if (!bPopModalOpenProceduralCameraMode)
			{
				ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
				ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP;
				ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
				ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_USE_FOG;
				#ifdef NOMINIMAP
				ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP;
				#endif
			}
			else
			{
				//PE: No minimap ... in cam mode.
				ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
				ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
				ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP;
			}

			if (ImageExist(TERRAINGENERATOR_IMAGE)) DeleteImage(TERRAINGENERATOR_IMAGE);
			if (ObjectExist(TERRAINGENERATOR_OBJECT)) DeleteObject(TERRAINGENERATOR_OBJECT);
			if (!bPopModalOpenProceduralCameraMode)
			{
				WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_CURSOROBJECT);

				//UI3D_TERRAINMOVER
				if (!ObjectExist(TERRAINGENERATOR_OBJECT))
					LoadObject("editors\\uiv3\\terrain mover solid.dbo", TERRAINGENERATOR_OBJECT);

				if (!ObjectExist(TERRAINGENERATOR_OBJECT))
				{
					bUseSphere = true;
					MakeObjectSphere(TERRAINGENERATOR_OBJECT, 200.0f, 30, 30);
				}
				WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_NORMAL);
				float fCenterHeight = BT_GetGroundHeight(t.terrain.TerrainID, fSnapShotModeCameraX, fSnapShotModeCameraZ);
				PositionObject(TERRAINGENERATOR_OBJECT, GGORIGIN_X, fCenterHeight, GGORIGIN_Z);

				if (!bUseSphere && ImageExist(UI3D_TERRAINMOVER))
				{
					//PE: Disable transparent, or we cant see it when below water.
					SetObjectMask(TERRAINGENERATOR_OBJECT, 1);
					TextureObject(TERRAINGENERATOR_OBJECT, UI3D_TERRAINMOVER);
					SetObjectDiffuse(TERRAINGENERATOR_OBJECT, Rgb(200, 200, 200));
					SetObjectEmissive(TERRAINGENERATOR_OBJECT, Rgb(0, 0, 0));
					ShowObject(TERRAINGENERATOR_OBJECT);
				}
				else
				{
					SetObjectEmissive(TERRAINGENERATOR_OBJECT, Rgb(200, 200, 0));
					SetObjectTransparency(TERRAINGENERATOR_OBJECT, 6);
					SetAlphaMappingOn(TERRAINGENERATOR_OBJECT, 50);
					SetObjectDiffuse(TERRAINGENERATOR_OBJECT, Rgb(0, 0, 0));
				}
				sObject* pObject = GetObjectData(TERRAINGENERATOR_OBJECT);
				if (pObject)
				{
					WickedCall_SetObjectCastShadows(pObject, false);
					WickedCall_SetObjectLightToUnlit(pObject, (int)wiScene::MaterialComponent::SHADERTYPE_UNLIT);
				}
				DisableObjectZDepth(TERRAINGENERATOR_OBJECT); //PE: Thanks Lee :)
			}

			bFirstTimeVeg = true;
			#ifndef DIGAHOLE
			extern bool g_bNoTerrainRender;
			g_bNoTerrainRender = true;
			#endif
		}
		else
		{
			static int last_preview_size_x = -1;
			static int last_preview_size_y = -1;

			#ifdef USEFULLVIEWPORT
			if (ImGui::GetMainViewport()->Size.x != last_preview_size_x || ImGui::GetMainViewport()->Size.y != last_preview_size_y)
			{
				//Resize. Delete backbuffer and create a new.
				if (BitmapExist(99))
				{
					DeleteBitmapEx(99);
				}
				last_preview_size_x = ImGui::GetMainViewport()->Size.x;
				last_preview_size_y = ImGui::GetMainViewport()->Size.y;

				BackBufferSizeX = ImGui::GetMainViewport()->Size.x;
				BackBufferSizeY = ImGui::GetMainViewport()->Size.y;

			}
			#else
			if (preview_size_x != last_preview_size_x || preview_size_y != last_preview_size_y)
			{
				//Resize. Delete backbuffer and create a new.
				if (BitmapExist(99))
				{
					DeleteBitmapEx(99);
				}
				last_preview_size_x = preview_size_x;
				last_preview_size_y = preview_size_y;
				BackBufferSizeX = preview_size_x;
				BackBufferSizeY = preview_size_y;
			}
			#endif
		}

		static int iCountToUpdate = 0;
		static bool bDraggingActive = false;
		static bool bObjHoverActive = false;
		static ImRect rClipRect;
		int iMoveTerrainObjectHeight = 900;
		
		extern bool bPreferences_Window;

		//Not if video playing.
		extern bool bLastSmallVideoPlayerMaximized;
		static float last_fx, last_fy, last_fz;
		if (ObjectExist(TERRAINGENERATOR_OBJECT) && !bTriggerTerrainSaveAsWindow && !bPreferences_Window && iQuitProceduralLevel == 0 && !bLastSmallVideoPlayerMaximized )
		{
			bImGuiGotFocus = false;
			ShowObject(TERRAINGENERATOR_OBJECT);
			static float fCursorPosX = 0.0f, fCursorPosY = 0.0f, fCursorPosZ = 0.0f;
			float fCenterHeight = BT_GetGroundHeight(t.terrain.TerrainID, GGORIGIN_X, GGORIGIN_Z);
			if (!bDraggingActive && iCountToUpdate++ >= 10)
			{
				PositionObject(TERRAINGENERATOR_OBJECT, GGORIGIN_X, fCenterHeight + iMoveTerrainObjectHeight, GGORIGIN_Z);
				iCountToUpdate = 0;
				//PE: check hover.
				int layer = GGRENDERLAYERS_CURSOROBJECT;
				float fHitX, fHitY, fHitZ;
				bool bHit = WickedCall_GetPick(&fHitX, &fHitY, &fHitZ, NULL, NULL, NULL, NULL, layer);
				if (bHit)
				{
					bObjHoverActive = true;
				}
				else
				{
					bObjHoverActive = false;
				}
			}
			if(bDraggingActive) bObjHoverActive = false;

			//PE: Only scale when moving done.
			if(movecameratotarget == 0)
			{
				float fTDX = GGORIGIN_X - CameraPositionX();
				float fTDY = fCenterHeight - CameraPositionY();
				float fTDZ = GGORIGIN_Z - CameraPositionZ();
				float fTerrDist = sqrt(fabs(fTDX*fTDX) + fabs(fTDY*fTDY) + fabs(fTDZ*fTDZ));
				float newscale = fTerrDist / 5000.0f;
				if (newscale < 0.2) newscale = 0.2;
				if (newscale > 100.0)newscale = 100.0f;
				if (!bUseSphere)
				{
					ScaleObject(TERRAINGENERATOR_OBJECT, newscale*300.0, newscale*300.0, newscale*300.0);
				}
				else
					ScaleObject(TERRAINGENERATOR_OBJECT, newscale*100.0, newscale*100.0, newscale*100.0);
			}

			static float fObjOffsetX, fObjOffsetY, fObjOffsetZ;
			static float fHitOffsetX, fHitOffsetY, fHitOffsetZ;
			static float fTerrainHitX, fTerrainHitY, fTerrainHitZ;
			static float fTerrainStartHitX, fTerrainStartHitY, fTerrainStartHitZ;
			bool bAreaAlreadyDisplayed = false;
			static ImVec2 newTargetOffset, newTargetCamera;
			int iMoveCameraSteps = 28;

			if (ImGui::IsMouseDown(0))
			{
				if (!bDraggingActive)
				{
					//Setup start values.
					int layer = GGRENDERLAYERS_CURSOROBJECT;
					bool bHit = WickedCall_GetPick(&fHitOffsetX, &fHitOffsetY, &fHitOffsetZ, NULL, NULL, NULL, NULL, layer);
					if (bHit)
					{
						layer = GGRENDERLAYERS_TERRAIN;
						bool bHitTerrain = WickedCall_GetPick(&fTerrainStartHitX, &fTerrainStartHitY, &fTerrainStartHitZ, NULL, NULL, NULL, NULL, layer);
						if (bHitTerrain)
						{
							fObjOffsetX = fHitOffsetX - fTerrainStartHitX;
							fObjOffsetY = fHitOffsetY - fTerrainStartHitY;
							fObjOffsetZ = fHitOffsetZ - fTerrainStartHitZ;
							bDraggingActive = true;
						}
					}
				}
				else
				{
					int layer = GGRENDERLAYERS_TERRAIN;
					bool bHitTerrain = WickedCall_GetPick(&fTerrainHitX, &fTerrainHitY, &fTerrainHitZ, NULL, NULL, NULL, NULL, layer);
					if (bHitTerrain)
					{
						float fX, fY, fZ;
						fX = fTerrainHitX;
						fY = fTerrainHitY;
						fZ = fTerrainHitZ;
						//PE: Dragging.
						float fCenterHeight = BT_GetGroundHeight(t.terrain.TerrainID, fX, fZ);
						PositionObject(TERRAINGENERATOR_OBJECT, fX, fCenterHeight + iMoveTerrainObjectHeight, fZ);

						//PE: Draw the editable area in 2D.
						ImGuiViewport* mainviewport = ImGui::GetMainViewport();
						if (mainviewport)
						{
							ImDrawList* dl = ImGui::GetForegroundDrawList(mainviewport);
							if (dl)
							{
								ImRect bb,dd;
								ImVec2 Convert3DTo2D(float x, float y, float z);
								float fAreaSize = ggterrain_global_render_params2.editable_size;
								fAreaSize *= 0.985; //PE: Make it a bit smaller as we use center for height on all corners.
								ImVec4 col = ImVec4(1.0, 1.0, 0.0, 0.6);

								dl->PushClipRect(rClipRect.Min, rClipRect.Max);

								dl->AddCallback((ImDrawCallback)10, NULL); //force render.

								last_fx = fX;
								last_fy = fCenterHeight;
								last_fz = fZ;

								ImVec2 vecmin = Convert3DTo2D(fX - fAreaSize, fCenterHeight, fZ - fAreaSize);
								ImVec2 vecmax = Convert3DTo2D(fX + fAreaSize, fCenterHeight, fZ - fAreaSize);
								vecmin += ImGui::GetMainViewport()->Pos;
								vecmax += ImGui::GetMainViewport()->Pos;
								if ( !(vecmin.y <= -1300.0 || vecmax.y <= -1300.0 || vecmin.x <= -5000.0 || vecmax.x > 2500.0 ))
									dl->AddLine(vecmin, vecmax, ImGui::GetColorU32(col), 2.0f);

								vecmin = vecmax;
								vecmax = Convert3DTo2D(fX + fAreaSize, fCenterHeight, fZ + fAreaSize);
								vecmax += ImGui::GetMainViewport()->Pos;
								if (!(vecmin.y <= -1300.0 || vecmax.y <= -1300.0 || vecmin.x <= -5000.0 || vecmax.x > 2500.0))
									dl->AddLine(vecmin, vecmax, ImGui::GetColorU32(col), 2.0f);

								vecmin = vecmax;
								vecmax = Convert3DTo2D(fX - fAreaSize, fCenterHeight, fZ + fAreaSize);
								vecmax += ImGui::GetMainViewport()->Pos;
								if (!(vecmin.y <= -1300.0 || vecmax.y <= -1300.0 || vecmin.x <= -5000.0 || vecmax.x > 2500.0))
									dl->AddLine(vecmin, vecmax, ImGui::GetColorU32(col), 2.0f);

								vecmin = vecmax;
								vecmax = Convert3DTo2D(fX - fAreaSize, fCenterHeight, fZ - fAreaSize);
								vecmax += ImGui::GetMainViewport()->Pos;
								if (!(vecmin.y <= -1300.0 || vecmax.y <= -1300.0 || vecmin.x <= -5000.0 || vecmax.x > 2500.0))
									dl->AddLine(vecmin, vecmax, ImGui::GetColorU32(col), 2.0f);

								dl->AddCallback((ImDrawCallback)11, NULL); //disable force render.

								dl->PopClipRect();

								bAreaAlreadyDisplayed = true;
							}
						}
					}
				}
			}
			else
			{
				if (bDraggingActive)
				{
					//Move to new position.
					float offsetX = fTerrainStartHitX - fTerrainHitX;
					float offsetZ = fTerrainStartHitZ - fTerrainHitZ;
					movecameratotarget = iMoveCameraSteps;

					newTargetCamera.x = -offsetX;
					newTargetCamera.y = -offsetZ;

					newTargetOffset.x = ggterrain_global_params.offset_x + GGTerrain_MetersToOffset(GGTerrain_UnitsToMeters(offsetX));
					newTargetOffset.y = ggterrain_global_params.offset_z + GGTerrain_MetersToOffset(GGTerrain_UnitsToMeters(offsetZ));
				}
				bDraggingActive = false;
			}
			bImGuiGotFocus = true;

			if (movecameratotarget > 0)
			{
				iCountToUpdate = 0;
				extern bool g_bNoSwapchainPresent;
				static ImVec2 orgCamera, orgObjPosition;
				movecameratotarget--;
				if (movecameratotarget > 13)
				{
					if (movecameratotarget == iMoveCameraSteps - 1)
					{
						orgCamera.x = fSnapShotModeCameraX;
						orgCamera.y = fSnapShotModeCameraZ;
					}
					fSnapShotModeCameraX += (newTargetCamera.x / (float)(iMoveCameraSteps - 12.0));
					fSnapShotModeCameraZ += (newTargetCamera.y / (float)(iMoveCameraSteps - 12.0));

					if (movecameratotarget == 14)
					{
						fSnapShotModeCameraX = newTargetCamera.x + orgCamera.x;
						fSnapShotModeCameraZ = newTargetCamera.y + orgCamera.y;
					}
				}
				if (movecameratotarget == 13)
				{
					ggterrain_global_params.offset_x = newTargetOffset.x;
					ggterrain_global_params.offset_z = newTargetOffset.y;
				}
				//PE: Freeze while terrain is generating.
				if (movecameratotarget <= 13)
				{
					g_bNoSwapchainPresent = true;
					//PE: No vsync so sleep a bit.
					Sleep(30); //30: looks like terrain is done with this delay.
				}
				if (movecameratotarget == 0)
				{
					fSnapShotModeCameraX = orgCamera.x;
					fSnapShotModeCameraZ = orgCamera.y;
					g_bNoSwapchainPresent = false;
					iCountToUpdate = 10; //9
				}
			}

			//PE: Always display 3d area using lines if possible.
			bool b2DPossible = false;
			//PE: Disable 2D box when using camera offsetting , 2D calc is a bit behind looks strange.
			if (bShowEditArea && !bAreaAlreadyDisplayed && movecameratotarget == 0)
			{
				//PE: Convert3DTo2D only works in topdown as it need the projection matrix so return wrong values if outside projection.
				if (fSnapShotModeCameraY > 36000.0 && fSnapShotModeCameraAngX > 8.0 ) // fSnapShotModeCameraAngX > 85 && fSnapShotModeCameraAngX < 95 && fSnapShotModeCameraY > 40000.0 )
				{
					ImGuiViewport* mainviewport = ImGui::GetMainViewport();
					if (mainviewport)
					{
						ImDrawList* dl = ImGui::GetForegroundDrawList(mainviewport);
						if (dl)
						{
							b2DPossible = true;

							float fX, fY, fZ;
							fX = ObjectPositionX(TERRAINGENERATOR_OBJECT);
							fY = ObjectPositionY(TERRAINGENERATOR_OBJECT);
							fZ = ObjectPositionZ(TERRAINGENERATOR_OBJECT);

							ImVec2 Convert3DTo2D(float x, float y, float z);
							float fAreaSize = ggterrain_global_render_params2.editable_size;
							fAreaSize *= 0.985; //PE: Make it a bit smaller as we use center for height on all corners.
							ImVec4 col = ImVec4(1.0, 1.0, 0.0, 0.6);

							ImVec2 vecmin[4], vecmax[4];

							vecmin[0] = Convert3DTo2D(fX - fAreaSize, fY, fZ - fAreaSize);
							vecmax[0] = Convert3DTo2D(fX + fAreaSize, fY, fZ - fAreaSize);
							vecmin[0] += ImGui::GetMainViewport()->Pos;
							vecmax[0] += ImGui::GetMainViewport()->Pos;

							vecmin[1] = vecmax[0];
							vecmax[1] = Convert3DTo2D(fX + fAreaSize, fY, fZ + fAreaSize);
							vecmax[1] += ImGui::GetMainViewport()->Pos;

							vecmin[2] = vecmax[1];
							vecmax[2] = Convert3DTo2D(fX - fAreaSize, fY, fZ + fAreaSize);
							vecmax[2] += ImGui::GetMainViewport()->Pos;

							vecmin[3] = vecmax[2];
							vecmax[3] = Convert3DTo2D(fX - fAreaSize, fY, fZ - fAreaSize);
							vecmax[3] += ImGui::GetMainViewport()->Pos;

							for(int i = 0; i < 4; i++)
							{
								if(vecmin[i].y <= -1300.0 || vecmax[i].y <= -1300.0 || vecmin[i].x <= -8000.0 || vecmin[i].x > 2500.0 || vecmax[i].x > 2500.0 || vecmax[i].y > 5000.0)
								{
									b2DPossible = false;
									break;
								}
							}
							if (b2DPossible)
							{
								dl->PushClipRect(rClipRect.Min, rClipRect.Max);

								dl->AddCallback((ImDrawCallback)10, NULL); //force render.

								dl->AddLine(vecmin[0], vecmax[0], ImGui::GetColorU32(col), 2.0f); //1
								dl->AddLine(vecmin[1], vecmax[1], ImGui::GetColorU32(col), 2.0f);
								dl->AddLine(vecmin[2], vecmax[2], ImGui::GetColorU32(col), 2.0f);
								dl->AddLine(vecmin[3], vecmax[3], ImGui::GetColorU32(col), 2.0f);

								char text[80];
								float fTmp = GGTerrain_UnitsToMeters(ggterrain_global_render_params2.editable_size * 2.0) / 1000.0f;
								sprintf(text, "Editable Area: %.2f Km / %.2f Miles" , fTmp, fTmp * 0.62137);
								ImVec2 textsize = ImGui::CalcTextSize(text);
								
								ImVec2 vCenterText = vecmin[0] - (((vecmin[0] - vecmax[0]) * 0.45));
								ImGuiContext& g = *GImGui;

								vCenterText -= textsize * 0.5;

								//PE: Special rotation as we use the GetForegroundDrawList that have its own vertex buffer.
								int rotation_start_index = dl->VtxBuffer.Size;
								auto& buf = dl->VtxBuffer;

								//PE: Offset a little from line.
								dl->AddText(g.Font, g.FontSize*1.2, vCenterText + ImVec2(0,-14.0), ImGui::GetColorU32(col), text); //Below line: ImVec2(0,20.0)
								
								//PE Get center.
								ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds
								for (int i = rotation_start_index; i < buf.Size; i++)
									l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);
								ImVec2 centerdraw = ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);

								float rad = atan2(vecmin[0].x - vecmax[0].x, vecmin[0].y - vecmax[0].y) + PI;

								//PE: Invert text so we never read upside down.
								if (rad > 3.20) rad += PI;

								float s = sin(rad), c = cos(rad);
								ImVec2 center = ImGuiRotation(centerdraw, s, c) - centerdraw;

								auto& buf2 = dl->VtxBuffer;
								for (int i = rotation_start_index; i < buf2.Size; i++)
								{
									buf2[i].pos = ImGuiRotation(buf2[i].pos, s, c) - center;
								}

								dl->AddCallback((ImDrawCallback)11, NULL); //disable force render.

								dl->PopClipRect();
							}
						}
					}
				}
			}
			if (b2DPossible || bDraggingActive || movecameratotarget > 0)
			{
				ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
			}
			else
			{
				if (bShowEditArea) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
				else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
			}
		}
		else
		{
			bObjHoverActive = false;
		}

		//PE: Edit mode keep change when changing settings inside here, so keep disable paint "circles".
		ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;
		//PE: Make sure we dont have any paint "circles" here.
		set_terrain_sculpt_mode(0); // GGTERRAIN_SCULPT_NONE; Disable terrain sculpt circle.
		set_terrain_edit_mode(0); // GGTERRAIN_EDIT_NONE; Disable terrain paint circle.

		#ifdef DIGAHOLE
		BackBufferSnapShotMode = false;
		#else
		BackBufferSnapShotMode = true;
		#endif
		t.editorfreeflight.mode = 1; //PE: keep freeflight here.

		//PE: New full screen.
		if (bUseFullScreen)
		{
			ImVec2 viewPortPos = ImGui::GetMainViewport()->Pos;
			ImVec2 viewPortSize = ImGui::GetMainViewport()->Size;
			ImGui::SetNextWindowPos(viewPortPos);
			ImGui::SetNextWindowSize(viewPortSize);
		}

		//PE: Need windows background to be non transparent.
		ImVec4 style_winback = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
		style_winback.w = 1.0f;
		ImGui::PushStyleColor(ImGuiCol_WindowBg, style_winback);
		
		if (bUseModal)
		{
			if (!bUseNoTitleBar)
				bPopModalOpenProcedural = ImGui::BeginPopupModal("##ProceduralPreview", &bProceduralLevel, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
			else
				bPopModalOpenProcedural = ImGui::BeginPopupModal("##ProceduralPreview", &bProceduralLevel, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
		}
		else
		{
			if (!bUseNoTitleBar)
				bPopModalOpenProcedural = ImGui::Begin("##ProceduralPreview", &bProceduralLevel, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
			else
				bPopModalOpenProcedural = ImGui::Begin("##ProceduralPreview", &bProceduralLevel, ImGuiWindowFlags_NoScrollWithMouse |ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
		}

		// terrain will automatically detect if a change has happened and update ggterrain_extra_params.bTerrainChanged
		if (bPopModalOpenProcedural)
		{
			//PE: Make sure "settings" is always on top of this window.
			ImGuiWindow* storyboard_window = ImGui::GetCurrentWindow();
			void CheckWindowsOnTop(ImGuiWindow* storyboard_window);
			CheckWindowsOnTop(storyboard_window);
			float fPreviewImgSize = preview_size_x;
			float fPreviewImgHeight = preview_size_x *0.5625; //Default.

			rClipRect.Min = ImGui::GetWindowPos()+ImVec2(0.0,67.0); //PE: ,67 = add header.
			rClipRect.Max = ImGui::GetWindowPos() + ImVec2(preview_size_x, preview_size_y);

			#ifdef DIGAHOLE
			bDigAHoleToHWND = true;
			rD3D11DigAHole.left = 2.0;
			rD3D11DigAHole.top = 67.0;
			rD3D11DigAHole.right = fPreviewImgSize;
			rD3D11DigAHole.bottom = ImGui::GetWindowSize().y - 24.0;
			#endif

			ImGui::Columns(2, "ProceduralPreviewColumns", false);  //false no border
			ImGui::SetColumnOffset(0, 0.0f);
			ImGui::SetColumnOffset(1, fPreviewImgSize);

			//PE: Move camera.
			static float fMoveX = 0.0f, fMoveZ = 0.0f;
			static float camposxold = 0.0f, camposzold = 0.0f;

			//Panning speed depend on camera height
			float fCamDistance = ((fSnapShotModeCameraY - 600.0f)*0.75f) * g.timeelapsed_f;
			if (fCamDistance < 50.0f) fCamDistance = 50.0f;

			static float fLastY = -1;
			if (fLastY != fSnapShotModeCameraY)
			{
				if (!bPopModalOpenProceduralCameraMode)
				{
					float fFog = 0.60; //0.35; //0.5
					float ratio = 1.0 - (fSnapShotModeCameraY / fMaxCameraY);
					if (ratio < 0.15) ratio = 0.15;
					if (ratio > 1.0) ratio = 1.0;
					wiScene::WeatherComponent* weather = wiScene::GetScene().weathers.GetComponent(g_weatherEntityID);
					if (t.visuals.skyindex == 0 && t.visuals.bDisableSkybox == false)
						weather->fogColorAndOpacity.w = 0.0; //PE: No fog Opacity in dynamic skybox mode.
					else
					{
						weather->fogColorAndOpacity.w = fFog * ratio;
						t.gamevisuals.FogA_f = t.visuals.FogA_f = oldFogA_f = fFog * ratio;
					}

					float fCover = 0.8;
					weather->volumetricCloudParameters.CoverageAmount = fCover * ratio;
					if (fSnapShotModeCameraY + 4000 > t.visuals.SkyCloudHeight)
					{
						weather->volumetricCloudParameters.CloudStartHeight = GGTerrain_UnitsToMeters(fSnapShotModeCameraY + 4000);
					}
					else
					{
						weather->volumetricCloudParameters.CloudStartHeight = GGTerrain_UnitsToMeters(t.visuals.SkyCloudHeight);
					}

				}
				fLastY = fSnapShotModeCameraY;
			}
			#ifndef DIGAHOLE
			if (ImageExist(iLargePreviewImageID))
			#else
			if(1)
			#endif
			{
				float fMoveSpeed = 512.0f;
				#ifndef DIGAHOLE
				bLoopFullFPS = false;
				float ImgX = ImageWidth(iLargePreviewImageID);
				float ImgY = ImageHeight(iLargePreviewImageID);
				float Ratio = fPreviewImgSize / ImgX;
				//PE: Always fit to Y preview_size_y
				ImgY *= Ratio;
				if (ImgY < preview_size_y)
				{
					ImgY = ImageHeight(iLargePreviewImageID);
					Ratio = preview_size_y / ImgY;
					fPreviewImgSize *= Ratio;
					ImgY = preview_size_y;
					//Revert ratio.
				}
				fPreviewImgHeight = ImgY;
				#endif

				#ifdef USEFULLVIEWPORT
				ImRect avail_window_rect;
				avail_window_rect.Min = ImGui::GetWindowPos();
				avail_window_rect.Max = ImGui::GetWindowPos() + ImVec2(preview_size_x, preview_size_y); //preview_size_x
				ImGui::PushClipRect(avail_window_rect.Min + ImVec2(2, 2), avail_window_rect.Max, false);
				ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
				#ifndef DIGAHOLE
				ImGui::ImgBtn(iLargePreviewImageID, ImVec2(ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), 0, 0, 0, 0, false);
				#else
				ImGui::Dummy(ImVec2(ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y));
				#endif
				ImGui::PopClipRect();
				#else
				#ifndef DIGAHOLE
				ImGui::ImgBtn(iLargePreviewImageID, ImVec2(fPreviewImgSize, ImgY), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), 0, 0, 0, 0, false);
				#else
				ImGui::Dummy(ImVec2(fPreviewImgSize, ImgY));
				#endif			
				#endif
				bool bIsItemHovered = ImGui::IsItemHovered();
				if (!bTriggerTerrainSaveAsWindow && !bLastSmallVideoPlayerMaximized)
				{
					#ifndef DIGAHOLE
					if (ImGui::IsMouseDown(1) && bIsItemHovered && ImGui::IsMouseDragging(1))
					#else
					//PE: We need imgui positions here.
					D3D11_RECT rD3D11DigAHole_IMGUI_POS;
					rD3D11DigAHole_IMGUI_POS.left = ImGui::GetWindowPos().x + 2.0;
					rD3D11DigAHole_IMGUI_POS.top = ImGui::GetWindowPos().y + 44.0; //Header.
					rD3D11DigAHole_IMGUI_POS.right = ImGui::GetWindowPos().x + fPreviewImgSize;
					rD3D11DigAHole_IMGUI_POS.bottom = ImGui::GetWindowPos().y + ImGui::GetWindowSize().y - 24.0;

					ImVec2 min = { (float)rD3D11DigAHole_IMGUI_POS.left, (float)rD3D11DigAHole_IMGUI_POS.top };
					ImVec2 max = { (float)rD3D11DigAHole_IMGUI_POS.right, (float)rD3D11DigAHole_IMGUI_POS.bottom };
					bIsItemHovered = ImGui::IsMouseHoveringRect(min, max);
					if (ImGui::IsMouseDown(1) && bIsItemHovered && ImGui::IsMouseDragging(1))
					#endif
					{
						float speed = 6.0f;
						float xdiff = ImGui::GetIO().MouseDelta.x / speed;
						float ydiff = ImGui::GetIO().MouseDelta.y / speed;
						t.editorfreeflight.c.angx_f = fSnapShotModeCameraAngX + ydiff;
						t.editorfreeflight.c.angy_f = fSnapShotModeCameraAngY + xdiff;
						if (t.editorfreeflight.c.angx_f > 180.0f)  t.editorfreeflight.c.angx_f = t.editorfreeflight.c.angx_f - 360.0f;
						if (t.editorfreeflight.c.angx_f < -89.999f)  t.editorfreeflight.c.angx_f = -89.999f;
						if (t.editorfreeflight.c.angx_f > 89.999f)  t.editorfreeflight.c.angx_f = 89.999f;
						fSnapShotModeCameraAngX = t.editorfreeflight.c.angx_f;
						fSnapShotModeCameraAngY = t.editorfreeflight.c.angy_f;
						RotateCamera(fSnapShotModeCameraAngX, fSnapShotModeCameraAngY, 0);
					}
					if (!bDraggingActive && !bIs2DViewHovered && ImGui::IsMouseDown(0) && bIsItemHovered && ImGui::IsMouseDragging(0))
					{
						float xdiff = ImGui::GetIO().MouseDelta.x / fMoveSpeed * fCamDistance;
						MoveCameraLeft(0, xdiff);
						float ydiff = ImGui::GetIO().MouseDelta.y / fMoveSpeed * fCamDistance;
						MoveCameraUp(0, ydiff);
						fSnapShotModeCameraX = GetCameraPosition().x;
						fSnapShotModeCameraZ = GetCameraPosition().z;
						#ifndef DIGAHOLE
						bLoopFullFPS = true;
						#endif
						ImGui::SetMouseCursor(ImGuiMouseCursor_Pan);
					}
					else 
					{
						fMoveX = camposxold = fSnapShotModeCameraX;
						fMoveZ = camposzold = fSnapShotModeCameraZ;
					}

					float keyspeed = 15.0f;
					if (fCamDistance < 900.0f) fCamDistance = 900.0f; //Keys faster minimum.
					if (io.KeyShift)
					{
						keyspeed *= 3.5; //Large distances so really speed it up.
					}
					else if (io.KeyCtrl)
					{
						keyspeed *= 0.35; //Large distances so really speed it up.
					}

					if (bIsItemHovered && (ImGui::IsKeyDown(87)|| ImGui::IsKeyDown(38))) //W UP
					{
						MoveCamera(0, (keyspeed / fMoveSpeed * fCamDistance));
						fSnapShotModeCameraX = GetCameraPosition().x;
						fSnapShotModeCameraY = GetCameraPosition().y;
						fSnapShotModeCameraZ = GetCameraPosition().z;
					}
					if (bIsItemHovered && (ImGui::IsKeyDown(83)|| ImGui::IsKeyDown(40))) //S Down
					{
						MoveCamera(0, -(keyspeed / fMoveSpeed * fCamDistance));
						fSnapShotModeCameraX = GetCameraPosition().x;
						fSnapShotModeCameraY = GetCameraPosition().y;
						fSnapShotModeCameraZ = GetCameraPosition().z;
					}
					if (bIsItemHovered && (ImGui::IsKeyDown(68)|| ImGui::IsKeyDown(39))) //D 
					{
						MoveCameraLeft(0, -(keyspeed / fMoveSpeed * fCamDistance));
						fSnapShotModeCameraX = GetCameraPosition().x;
						fSnapShotModeCameraY = GetCameraPosition().y;
						fSnapShotModeCameraZ = GetCameraPosition().z;
					}
					if (bIsItemHovered && (ImGui::IsKeyDown(65)|| ImGui::IsKeyDown(37))) //A
					{
						MoveCameraLeft(0, (keyspeed / fMoveSpeed * fCamDistance));
						fSnapShotModeCameraX = GetCameraPosition().x;
						fSnapShotModeCameraY = GetCameraPosition().y;
						fSnapShotModeCameraZ = GetCameraPosition().z;
					}

					if (bIsItemHovered && ImGui::GetIO().MouseWheel != 0)
					{
						float cammove = fSnapShotModeCameraY;
						if (cammove < 1200) cammove = 1200;
						float speed = cammove / 40.0f; //Speed depent on camera height
						if (io.KeyShift) speed *= 2.0; //Faster when using shift.
						speed *= g.timeelapsed_f;
						MoveCamera(0, ImGui::GetIO().MouseWheel*speed);
						fSnapShotModeCameraX = GetCameraPosition().x;
						fSnapShotModeCameraY = GetCameraPosition().y;
						fSnapShotModeCameraZ = GetCameraPosition().z;
						#ifndef DIGAHOLE
						bLoopFullFPS = true;
						#endif
					}
				}
			}

			//PE: Toolbar look.
			float fToolbarHeight = 90.0f;
			ImGui::PushClipRect(ImGui::GetWindowPos()+ImVec2(2,2), ImGui::GetWindowPos() + ImVec2(preview_size_x+1.0, fToolbarHeight), false);
			ImGui::GetCurrentWindow()->DrawList->AddRectFilled(ImVec2(-1, -1), ImVec2(preview_size_x+1.0, fToolbarHeight), ImGui::GetColorU32(style_winback), 0.0f, ImDrawCornerFlags_None);
			ImGui::PopClipRect();

			ImVec2 vCurPos = ImGui::GetCursorPos();
			ImVec2 vIconSize = { (float) ImGui::GetFontSize()*4.0f, (float) ImGui::GetFontSize()*4.0f };
			if (bUseNoTitleBar)
				ImGui::SetCursorPos(ImVec2(3.0f, 3.0f));
			else
				ImGui::SetCursorPos(ImVec2(2.0f,22.0f));

			ImGui::SetItemAllowOverlap();
			if (ImGui::ImgBtn(TOOL_GOBACK, vIconSize, ImVec4(0, 0, 0, 0), drawCol_normal, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
			{
				if (!bPopModalOpenProceduralCameraMode)
				{
					if (bProceduralLevelFromStoryboard)
					{
						//PE: Now just goes back to storyboard with no save as.
						int iAction = askBoxCancel("Your new terrain is not saved, are you sure ?", "Confirmation"); //1==Yes 2=Cancel 0=No
						if (iAction == 1)
						{
							bProceduralLevel = false;
						}
					}
					else
					{
						//From level editor , just go back.
						bProceduralLevel = false;
					}
				}
				else
				{
					iQuitProceduralLevel = 5; // Quit with screenshot.
				}
			}
			if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Exit");

			// only allow view toggle if not in map snapshot mode
			if (bPopModalTakeMapSnapshot == true)
			{
				// When in map snapshot mode, always position camera to point at center of edit area.
				int iEditableSize = ggterrain_global_render_params2.editable_size;
				if (movecameratotarget == 0)
				{
					float fTmp = GGTerrain_UnitsToMeters(iEditableSize * 2.0) / 1000.0f;
					fSnapShotModeCameraY = fTmp * 41000.0f;
					if (fSnapShotModeCameraY > 344000) fSnapShotModeCameraY = 344000; //Hide ugly shadow for now.
					fSnapShotModeCameraX = GGORIGIN_X; // +ggterrain_global_params.offset_x; It dont actual move from center.
					fSnapShotModeCameraZ = GGORIGIN_Z; // +ggterrain_global_params.offset_z;
					fSnapShotModeCameraAngZ = fSnapShotModeCameraAngY = 0.0f;
					fSnapShotModeCameraAngX = 90.0f; //Look down.
				}

				// show edge of game area
				bool bShow = true;
				if (bShow) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
				else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;

				// ensure clouds are off
				extern wiECS::Entity g_weatherEntityID;
				wiScene::WeatherComponent* weather = wiScene::GetScene().weathers.GetComponent(g_weatherEntityID);
				weather->SetRealisticSky(false);
				weather->SetVolumetricClouds(false);

				// and override camera projection to create an ortho view
				wiScene::GetCamera().SetCustomProjectionEnabled(true);
				float fTmp = GGTerrain_UnitsToMeters(iEditableSize * 2.0) / 1000.0f;
				float fScaleProj = 50000.0f * fTmp;
				float fRatioProj = (float)master.masterrenderer.GetLogicalWidth() / (float)master.masterrenderer.GetLogicalHeight();
				XMMATRIX P = XMMatrixOrthographicLH(fScaleProj * fRatioProj, fScaleProj, wiScene::GetCamera().zFarP, wiScene::GetCamera().zNearP);
				XMStoreFloat4x4(&wiScene::GetCamera().Projection, P);
				wiScene::GetCamera().UpdateCamera();
			}
			else
			{
				//PE: Camera Tool Icon.
				ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvailWidth() - vIconSize.x, 3.0f));
				ImGui::SetItemAllowOverlap();
				if (fSnapShotModeCameraAngX > 85 && fSnapShotModeCameraAngX < 95)
				{
					if (ImGui::ImgBtn(TOOL_CAMERA, vIconSize, ImVec4(0, 0, 0, 0), drawCol_normal, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
					{
						if (movecameratotarget == 0)
						{
							if (fSnapShotModeCameraY > 344000) fSnapShotModeCameraY = 344000; //Hide ugly shadow for now.
							//PE: Now start up high looking at the editable area.
							fSnapShotModeCameraX = 73800;
							fSnapShotModeCameraZ = -74000;
							fSnapShotModeCameraY = 51600;
							fSnapShotModeCameraAngZ = 0.0f;
							fSnapShotModeCameraAngY = -37;
							fSnapShotModeCameraAngX = 25; //34;
						}
					}
					if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Change to 3D View");
				}
				else
				{
					if (ImGui::ImgBtn(TOOL_CAMERA, vIconSize, ImVec4(0, 0, 0, 0), drawCol_normal, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
					{
						if (movecameratotarget == 0)
						{
							float fTmp = GGTerrain_UnitsToMeters(ggterrain_global_render_params2.editable_size * 2.0) / 1000.0f;
							//Reset camera to point at center of edit area.
							fSnapShotModeCameraY = fTmp * 41000.0f;
							if (fSnapShotModeCameraY > 344000) fSnapShotModeCameraY = 344000; //Hide ugly shadow for now.
							fSnapShotModeCameraX = GGORIGIN_X; // +ggterrain_global_params.offset_x; It dont actual move from center.
							fSnapShotModeCameraZ = GGORIGIN_Z; // +ggterrain_global_params.offset_z;
							fSnapShotModeCameraAngZ = fSnapShotModeCameraAngY = 0.0f;
							fSnapShotModeCameraAngX = 90.0f; //Look down.
						}
					}
					if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Change to Top Down View");
				}
			}

			if (bUseNoTitleBar)
				ImGui::SetCursorPos(ImVec2(3.0f, 4.0));
			else
				ImGui::SetCursorPos(ImVec2(2.0f, 22.0f + 10.0));

			extern ImFont* customfont;
			extern ImFont* customfontlarge;

			if(customfontlarge) ImGui::PushFont(customfontlarge);
			ImGui::SetWindowFontScale(2.0);
			if (!bPopModalOpenProceduralCameraMode)
			{
				ImGui::TextCenter("Terrain Generator");
			}
			else
			{
				ImGui::TextCenter("Snapshot Mode");
			}
			ImGui::SetWindowFontScale(1.0);
			ImGui::PushFont(customfont);

			#ifdef NOMINIMAP
			if (!bPopModalOpenProceduralCameraMode && bPopModalTakeMapSnapshot == false)
			{
				if (bObjHoverActive) ImGui::SetTooltip("%s", "Grab and move around the terrain to choose the actual playable area you want for your game.");
			}
			if(0)
			#endif
			{
				if (!bTriggerTerrainSaveAsWindow)
				{
					ImVec2 vOverlayPos;

					//New 2D Overlay square.
					float fRatio = 0.18; //Larger
					float fOverlayOffsetX = 144.0f;
					float fOverlayOffsetY = 50.0f; //PE: Without lower buttons.
					float imgw = fPreviewImgSize * fRatio;
					float imgh = fPreviewImgSize * fRatio;
					bIs2DViewHovered = false;

					float fSquareRatio = (ggterrain_global_render_params2.editable_size / 39.3701 / 1000.0f) * 80.0f;

					float fSize = fSquareRatio; //44.0f;
					float fCenter = fSize * 0.5;
					static float move_areax = 0, move_areay = 0;
					static float bIsAreaMoving = false;

					if (!bPopModalOpenProceduralCameraMode)
					{
						vOverlayPos = ImVec2(ImVec2(ImGui::GetFontSize() + 10.0f + fOverlayOffsetX + (move_areax - fCenter), fPreviewImgHeight + (move_areay - fCenter)) - ImVec2(0.0f, imgh + ImGui::GetFontSize() + fOverlayOffsetY));
						ImGui::SetCursorPos(vOverlayPos);
						ImGui::SetItemAllowOverlap();
						ImGui::ImgBtn(SHAPE_SQUARE, ImVec2(fSize, fSize), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 0.0, 1.0), ImVec4(1.0, 1.0, 0.0, 1.0), ImVec4(1.0, 1.0, 0.0, 1.0), 0, 0, 0, 0, false);
						if (ImGui::IsItemHovered())
						{
							bIs2DViewHovered = true;
							ImGui::SetTooltip("%s", "Click to drag editable area");
						}
						static bool bGotNewArea = false;
						static float fNewOffsetX, fNewOffsetZ;
						if (ImGui::IsMouseDown(0) && ImGui::IsItemHovered())
						{
							bIsAreaMoving = true;
							if (ImGui::IsMouseDragging(0))
							{
								bGotNewArea = true;
								//PE: is ggterrain_global_params.offset_x now in meters ?
								float fMoveSpeed = 30.0; //Looks like this is the scale from mouse coords to mapsize.
								float fBorder = 20.0;
								float xdiff = ImGui::GetIO().MouseDelta.x;
								if ((move_areax + xdiff) < imgw - fBorder && (move_areax + xdiff) > fBorder)
								{
									move_areax += xdiff;
									fNewOffsetX -= (xdiff / fMoveSpeed);
								}

								float ydiff = ImGui::GetIO().MouseDelta.y;
								if ((move_areay + ydiff) < imgh - fBorder && (move_areay + ydiff) > fBorder)
								{
									move_areay += ydiff;
									fNewOffsetZ += (ydiff / fMoveSpeed);
								}
								#ifndef DIGAHOLE
								bLoopFullFPS = true;
								#endif

							}
							ImGui::SetMouseCursor(ImGuiMouseCursor_Pan);
						}
						else {
							static bool bAreaInit = false;
							if (bGotNewArea)
							{
								//Reposition and center.
								ggterrain_global_params.offset_x = fNewOffsetX;
								ggterrain_global_params.offset_z = fNewOffsetZ;
								bGotNewArea = false;
								bAreaInit = false;
							}

							fNewOffsetX = ggterrain_global_params.offset_x;
							fNewOffsetZ = ggterrain_global_params.offset_z;
							//Place in center.
							if (!bAreaInit)
							{
								move_areax = imgw * 0.5; //+2.0 to match Paul area location.
								move_areay = imgh * 0.5;
								bAreaInit = true;
							}
							bIsAreaMoving = false;
						}
					}
				}
			}

			ImGui::SetCursorPos(vCurPos);
			ImGui::Spacing();
			ImGui::NextColumn();

			if (!bPopModalOpenProceduralCameraMode)
			{
				//PE: After large "generate..." button.
				ImGui::BeginChild("##ChildProceduralPreview", ImVec2(0, preview_size_y - 78), false, ImGuiWindowFlags_ForceRender | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavInputs);

				//PE: Find a stable terrain height Y. To adjusty 3D view height if terrain change.
				static int stable_height_y = 0;
				static int stable_height_y_count = 0;
				static int iLastHeightY = -1;
				static int last_stable_height_y = -1;

				int iHeightY = -999999;
				float height;
				if (bTriggerStableY)
				{
					if (GGTerrain_GetHeight(fSnapShotModeCameraX, fSnapShotModeCameraZ, &height))
					{
						//PE: Only use valid Y.
						iHeightY = height;
						if (iHeightY != iLastHeightY)
						{
							stable_height_y_count = 15;
							iLastHeightY = iHeightY;
						}
						if (stable_height_y_count > 0)
						{
							if (--stable_height_y_count == 0)
							{
								stable_height_y = iHeightY;
								if (last_stable_height_y != stable_height_y)
								{
									//PE: Only if terrain changed from last update (dont break free flight).
									last_stable_height_y = stable_height_y;

									if ((stable_height_y + i3DViewHeight) > fSnapShotModeCameraY)
									{
										//PE: Only update if terrain got higher to not break free flight.
										fSnapShotModeCameraY = stable_height_y;
										fSnapShotModeCameraY += i3DViewHeight; //PE: A bit above terrain.
									}
								}
								bTriggerStableY = false;
							}
						}
					}
				}

				static bool bRandomizeTimeOfDay = true;
				static int iLastUserSelectedTimeOfDay = -1;
				static bool bSelectRandomSkybox = false;
				static bool bSelectDayRandomSkybox = false;
				static int iSelectedThemeChoice = 0;
				static bool bFirtTimeInTheme = false;
				static bool bFirstBiomes[9] = { true,true,true,true,true,true,true,true,true };

				if(bSelectRandomSkybox)
				{
					bSelectRandomSkybox = false;
					#ifdef RANDOMSKYBOX
					int iRandom = (rand() % (t.skybank_s.size() ));
					int skyindex = iRandom;
					bool bTriggerSetTimeOfDay = false;

					wiScene::WeatherComponent* weather = wiScene::GetScene().weathers.GetComponent(g_weatherEntityID);

					bool bForceDay = false;
					if (bFirstBiomes[iSelectedThemeChoice] || bSelectDayRandomSkybox)
					{
						bFirstBiomes[iSelectedThemeChoice] = false;
						bSelectDayRandomSkybox = false;
						bForceDay = true;
					}

					if ((rand() % 3) == 0 || bForceDay )
					{
						//PE: Dynamic every 3 times randomly.
						t.gamevisuals.skyindex = t.visuals.skyindex = 0;
						t.gamevisuals.bDisableSkybox = t.visuals.bDisableSkybox = false;
						if (weather)
						{
							if (weather->skyMap != nullptr && weather->skyMapName.length() > 0)
							{
								//PE: Make sure to free any old resources.
								WickedCall_DeleteImage(weather->skyMapName);
							}
							weather->skyMap = nullptr;
							weather->skyMapName = "";
							weather->cloudSpeed = t.visuals.SkyCloudSpeed;
							weather->cloudiness = t.visuals.SkyCloudiness;
						}
						Wicked_Update_Visuals((void *)&t.visuals); //PE: Switch to dynamic skybox.
						bTriggerSetTimeOfDay = true;
					}
					else if (iRandom == 0 || iRandom  >= t.skybank_s.size() )
					{
						//Delete if we already have one loaded.
						if (weather)
						{
							if (weather->skyMap != nullptr && weather->skyMapName.length() > 0)
							{
								//PE: Make sure to free any old resources.
								WickedCall_DeleteImage(weather->skyMapName);
							}
							weather->skyMap = nullptr;
							weather->skyMapName = "";
							weather->cloudSpeed = t.visuals.SkyCloudSpeed;
							weather->cloudiness = t.visuals.SkyCloudiness;
						}
						t.visuals.skyindex = 0;
						t.gamevisuals.skyindex = t.visuals.skyindex;
						t.gamevisuals.bDisableSkybox = t.visuals.bDisableSkybox = true;
						bTriggerSetTimeOfDay = true;
					}
					else
					{
						t.gamevisuals.bDisableSkybox = t.visuals.bDisableSkybox = false;
						t.visuals.skyindex = skyindex;
						t.gamevisuals.skyindex = t.visuals.skyindex;

						g.skyindex = t.visuals.skyindex;
						t.visuals.sky_s = t.skybank_s[g.skyindex];
						t.gamevisuals.sky_s = t.skybank_s[g.skyindex];
						t.terrainskyspecinitmode = 0;
						sky_skyspec_init();
						t.sky.currenthour_f = 8.0;
						t.sky.daynightprogress = 0;
						WickedCall_UpdateProbes();

						t.gamevisuals.SunAngleX = t.visuals.SunAngleX;
						t.gamevisuals.SunAngleY = t.visuals.SunAngleY;
						t.gamevisuals.SunAngleZ = t.visuals.SunAngleZ;

						oldSunAngleX = t.visuals.SunAngleX; //PE: Make sure to update after save.
						oldSunAngleY = t.visuals.SunAngleY;
						oldSunAngleZ = t.visuals.SunAngleZ;

					}
					fLastY = -1; //PE: Trigger a update to fog and dynamic sky.

					//PE: Check if we need to update time of day if skybox changed and we have no random time of day.
					if (bTriggerSetTimeOfDay)
					{
						if (!bRandomizeTimeOfDay && iLastUserSelectedTimeOfDay >= 0)
						{
							t.gamevisuals.iTimeOfday = t.visuals.iTimeOfday = iLastUserSelectedTimeOfDay;
							visuals_calcsunanglefromtimeofday(t.gamevisuals.iTimeOfday, &t.gamevisuals.SunAngleX, &t.gamevisuals.SunAngleY, &t.gamevisuals.SunAngleZ);
							t.visuals.SunAngleX = t.gamevisuals.SunAngleX;
							t.visuals.SunAngleY = t.gamevisuals.SunAngleY;
							t.visuals.SunAngleZ = t.gamevisuals.SunAngleZ;
							oldSunAngleX = t.visuals.SunAngleX; //PE: Make sure to update after save.
							oldSunAngleY = t.visuals.SunAngleY;
							oldSunAngleZ = t.visuals.SunAngleZ;
							Wicked_Update_Visuals((void *)&t.visuals);
						}
					}

					#endif
				}

				int iRandomTimeOfDayChoice = -1;

				static bool iLastTreeGrassSettings = -1;
				if (ImGui::StyleCollapsingHeader("Terraform Terrain", ImGuiTreeNodeFlags_DefaultOpen))
				{
					float feditable_size = ggterrain_global_render_params2.editable_size;

					ImGui::PushItemWidth(-10);
					float fButSizeX = ImGui::GetContentRegionAvailWidth() / 4.0;
					fButSizeX -= 6.0f; //Padding.
					float fButSizeY = fButSizeX * 0.60f;

					// when FIRST enter procedural terrain generator, randomly select a theme
					int iRandomThemeChoice = 0;
					extern bool bProceduralLevelStartup;
					if (bProceduralLevelStartup == true)
					{
						timestampactivity(0, "GGTerrain_RemoveAllFlatAreas");
						GGTerrain_RemoveAllFlatAreas(); //PE: Remove all flat areas.
						iRandomThemeChoice = 7; //PE: new design always rainforest.
						bProceduralLevelStartup = false;
						bTriggerStableY = true;
						iRandomTimeOfDayChoice = 3; // (1) PE: Default now afternoon 3
						fLastY = -1; //Trigger fog update.
						iLastTreeGrassSettings = -1;
					}


					ImVec4 outline_color = ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram];
					ImVec2 vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					if (ImGui::StyleButton("Plains", ImVec2(fButSizeX, fButSizeY)) || iRandomThemeChoice == 1)
					{

						if(bRandomizeTimeOfDay && iRandomThemeChoice == 0 ) iRandomTimeOfDayChoice = (rand() % 7);
						if (iRandomThemeChoice == 0) bSelectRandomSkybox = true;
						iSelectedThemeChoice = 1;

						//editors\biomes\plains.dat
						GGTerrainFile_LoadTerrainData("editors\\biomes\\plains.dat", false);
						ggterrain_global_params.seed = Random2();

						// ggtrees_global_params.draw_enabled = 0; //PE: Trees is now only controlled by visual.ini t.visuals.bEndableTreeDrawing
						t.showeditortrees = t.gamevisuals.bEndableTreeDrawing = t.visuals.bEndableTreeDrawing = 0;
						t.showeditorveg = t.gamevisuals.bEndableGrassDrawing = t.visuals.bEndableGrassDrawing = 0;
						t.showeditorterrain = t.gamevisuals.bEndableTerrainDrawing = t.visuals.bEndableTerrainDrawing = 1;
						
						//PE: Default tree and grass setup.
						if (iLastTreeGrassSettings != 0)
						{
							iLastTreeGrassSettings = 0;
							ggtrees_global_params.paint_tree_bitfield = 1; //Plains
							ggtrees_global_params.paint_scale_random_low = 40.0; //Default Scale Min.
							ggtrees_global_params.paint_scale_random_high = 200.0; //Default Scale Max.
							GGTrees::GGTrees_ChangeDensity(65); //Default density.
							GGTrees::ggtrees_global_params.hide_until_update = 1;
							GGTrees::ggtrees_global_params.draw_enabled = 0;
							gggrass_global_params.paint_type = 1; //Default grass
							gggrass_global_params.paint_density = 100; //Default Density
							gggrass_global_params.paint_material = 0; //Auto
							GGGrass::GGGrass_AddAll();
						}

						t.terrain.waterliney_f = g.gdefaultwaterheight = 13.1234331; //-23; // = 75; -500
						t.gamevisuals.WaterSpeed1 = t.visuals.WaterSpeed1 = 0.03; // = 3 0.06
						t.gamevisuals.WaterRed_f = t.visuals.WaterRed_f = 11; // 9
						t.gamevisuals.WaterGreen_f = t.visuals.WaterGreen_f = 17; //21
						t.gamevisuals.WaterBlue_f = t.visuals.WaterBlue_f = 25; //43
						t.gamevisuals.WaterAlpha_f = t.visuals.WaterAlpha_f = 255; //0
						t.gamevisuals.fWaterWaveAmplitude = t.visuals.fWaterWaveAmplitude = 20; //20
						t.gamevisuals.fWaterWindDependency = t.visuals.fWaterWindDependency = 0;// 0
						t.gamevisuals.fWaterPatchLength = t.visuals.fWaterPatchLength = 17; // 40
						t.gamevisuals.fWaterChoppyScale = t.visuals.fWaterChoppyScale = 0; // 0
						t.gamevisuals.WaterFogMinDist = t.visuals.WaterFogMinDist = 0; //start 0
						t.gamevisuals.WaterFogMaxDist = t.visuals.WaterFogMaxDist = 3000.0; // 11500
						t.gamevisuals.WaterFogMinAmount = t.visuals.WaterFogMinAmount = 0.57; // = 58 0.25

						//new

						t.visuals.bWaterEnable = true;
						t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
						Wicked_Update_Visuals((void *)&t.visuals);

						//Create Level
						ggterrain_extra_params.iProceduralTerrainType = 1;
						ggterrain_global_params.fractal_initial_amplitude = 1.0f;

						t.gamevisuals.bEndableAmbientMusicTrack = t.visuals.bEndableAmbientMusicTrack = true;
						t.visuals.sAmbientMusicTrack = "audiobank\\ambient\\Plains.wav";
						t.gamevisuals.sAmbientMusicTrack = t.visuals.sAmbientMusicTrack;
						t.gamevisuals.bEnableCombatMusicTrack = t.visuals.bEnableCombatMusicTrack = false;
						t.visuals.sCombatMusicTrack = "";
						t.gamevisuals.sCombatMusicTrack = t.visuals.sCombatMusicTrack;

						ggtrees_global_params.water_dist = 400.0;
						bTriggerStableY = true;
					}
					if (iSelectedThemeChoice == 1)
					{
						ImVec2 padding = { 0.0, 0.0 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(fButSizeX, fButSizeY));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(outline_color), 0.0f, 15, 2.0f);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Plains Theme.");
					ImGui::SameLine();

					vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					if (ImGui::StyleButton("Desert", ImVec2(fButSizeX, fButSizeY)) || iRandomThemeChoice == 2)
					{
						if (bRandomizeTimeOfDay && iRandomThemeChoice == 0) iRandomTimeOfDayChoice = (rand() % 7);
						if (iRandomThemeChoice == 0) bSelectRandomSkybox = true;
						iSelectedThemeChoice = 2;

						GGTerrainFile_LoadTerrainData("editors\\biomes\\desert.dat", false);
						ggterrain_global_params.seed = Random2();
						ggterrain_global_render_params.slopeMatIndex[0] = 0x100 | 25;
						ggterrain_global_render_params.slopeMatIndex[1] = 0x100 | 25;

						t.showeditortrees = t.gamevisuals.bEndableTreeDrawing = t.visuals.bEndableTreeDrawing = 1;
						t.showeditorveg = t.gamevisuals.bEndableGrassDrawing = t.visuals.bEndableGrassDrawing = 1;
						t.showeditorterrain = t.gamevisuals.bEndableTerrainDrawing = t.visuals.bEndableTerrainDrawing = 1;

						if (iLastTreeGrassSettings != 2)
						{
							iLastTreeGrassSettings = 2;

							//Dessert cactus + grass dessert.
							ggtrees_global_params.paint_tree_bitfield = 30; //PE: New trees 30 cactus , old 4;
							ggtrees_global_params.paint_scale_random_low = 40.0; //Scale Min.
							ggtrees_global_params.paint_scale_random_high = 200.0; //Scale Max.
							GGTrees::GGTrees_ChangeDensity(3); //3=Low density, Also set new type and scale.
							GGTrees::ggtrees_global_params.hide_until_update = 1;
							GGTrees::ggtrees_global_params.draw_enabled = 0;

							gggrass_global_params.paint_type = 528; //PE: New palette was 458752; //dessert types.
							gggrass_global_params.paint_density = 3; //Low Density
							gggrass_global_params.paint_material = 0; //Auto
							GGGrass::GGGrass_AddAll();
						}

						//PE: Default water settings.
						t.terrain.waterliney_f = g.gdefaultwaterheight = -500;
						t.gamevisuals.WaterSpeed1 = t.visuals.WaterSpeed1 = 0.06;
						t.gamevisuals.WaterRed_f = t.visuals.WaterRed_f = 9;
						t.gamevisuals.WaterGreen_f = t.visuals.WaterGreen_f = 21;
						t.gamevisuals.WaterBlue_f = t.visuals.WaterBlue_f = 43;
						t.gamevisuals.WaterAlpha_f = t.visuals.WaterAlpha_f = 0;
						t.gamevisuals.fWaterWaveAmplitude = t.visuals.fWaterWaveAmplitude = 20;
						t.gamevisuals.fWaterWindDependency = t.visuals.fWaterWindDependency = 0;
						t.gamevisuals.fWaterPatchLength = t.visuals.fWaterPatchLength = 40;
						t.gamevisuals.fWaterChoppyScale = t.visuals.fWaterChoppyScale = 0;
						t.gamevisuals.WaterFogMinDist = t.visuals.WaterFogMinDist = 0;
						t.gamevisuals.WaterFogMaxDist = t.visuals.WaterFogMaxDist = 11500;
						t.gamevisuals.WaterFogMinAmount = t.visuals.WaterFogMinAmount = 0.25;

						t.visuals.bWaterEnable = true;
						t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
						Wicked_Update_Visuals((void *)&t.visuals);

						//Create Level
						ggterrain_extra_params.iProceduralTerrainType = 2;
						ggterrain_global_params.fractal_initial_amplitude = 1.0f;

						t.gamevisuals.bEndableAmbientMusicTrack = t.visuals.bEndableAmbientMusicTrack = true;
						t.visuals.sAmbientMusicTrack = "audiobank\\ambient\\Desert.wav";
						t.gamevisuals.sAmbientMusicTrack = t.visuals.sAmbientMusicTrack;
						t.gamevisuals.bEnableCombatMusicTrack = t.visuals.bEnableCombatMusicTrack = false;
						t.visuals.sCombatMusicTrack = "";
						t.gamevisuals.sCombatMusicTrack = t.visuals.sCombatMusicTrack;

						ggtrees_global_params.water_dist = 400.0;
						bTriggerStableY = true;

					}
					if (iSelectedThemeChoice == 2)
					{
						ImVec2 padding = { 0.0, 0.0 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(fButSizeX, fButSizeY));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(outline_color), 0.0f, 15, 2.0f);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Desert Theme");
					ImGui::SameLine();

					vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					if (ImGui::StyleButton("Forest", ImVec2(fButSizeX, fButSizeY)) || iRandomThemeChoice == 3)
					{
						if (bRandomizeTimeOfDay && iRandomThemeChoice == 0) iRandomTimeOfDayChoice = (rand() % 7);
						if (iRandomThemeChoice == 0) bSelectRandomSkybox = true;
						iSelectedThemeChoice = 3;

						GGTerrainFile_LoadTerrainData("editors\\biomes\\forest.dat", false);
						ggterrain_global_params.seed = Random2();

						// ggtrees_global_params.draw_enabled = 1; //PE: Trees is now only controlled by visual.ini t.visuals.bEndableTreeDrawing
						t.showeditortrees = t.gamevisuals.bEndableTreeDrawing = t.visuals.bEndableTreeDrawing = 1;
						t.showeditorveg = t.gamevisuals.bEndableGrassDrawing = t.visuals.bEndableGrassDrawing = 1;
						t.showeditorterrain = t.gamevisuals.bEndableTerrainDrawing = t.visuals.bEndableTerrainDrawing = 1;

						if (iLastTreeGrassSettings != 3)
						{
							iLastTreeGrassSettings = 3;
							//PE: Default tree and grass setup.
							// ZJ: Removed dead tree from forest biome.
							//ggtrees_global_params.paint_tree_bitfield = 17195597825; //Forest
							ggtrees_global_params.paint_tree_bitfield = 7340033; //Forest
							ggtrees_global_params.paint_scale_random_low = 40.0; //Default Scale Min.
							ggtrees_global_params.paint_scale_random_high = 200.0; //Default Scale Max.
							GGTrees::GGTrees_ChangeDensity(90); //Default density.
							GGTrees::ggtrees_global_params.hide_until_update = 1;
							GGTrees::ggtrees_global_params.draw_enabled = 0;
							gggrass_global_params.paint_type = 1167; //Default grass
							gggrass_global_params.paint_density = 100; //Default Density
							gggrass_global_params.paint_material = 0; //Auto
							GGGrass::GGGrass_AddAll();
						}

						//PE: Default water settings.
						t.terrain.waterliney_f = g.gdefaultwaterheight = 0;
						t.gamevisuals.WaterSpeed1 = t.visuals.WaterSpeed1 = 0.1;
						t.gamevisuals.WaterRed_f = t.visuals.WaterRed_f = 4;
						t.gamevisuals.WaterGreen_f = t.visuals.WaterGreen_f = 19;
						t.gamevisuals.WaterBlue_f = t.visuals.WaterBlue_f = 51;
						t.gamevisuals.WaterAlpha_f = t.visuals.WaterAlpha_f = 0;
						t.gamevisuals.fWaterWaveAmplitude = t.visuals.fWaterWaveAmplitude = 20;
						t.gamevisuals.fWaterWindDependency = t.visuals.fWaterWindDependency = 0;
						t.gamevisuals.fWaterPatchLength = t.visuals.fWaterPatchLength = 27;
						t.gamevisuals.fWaterChoppyScale = t.visuals.fWaterChoppyScale = 0;
						t.gamevisuals.WaterFogMinDist = t.visuals.WaterFogMinDist = 0;
						t.gamevisuals.WaterFogMaxDist = t.visuals.WaterFogMaxDist = 1000;
						t.gamevisuals.WaterFogMinAmount = t.visuals.WaterFogMinAmount = 0.25;

						t.visuals.bWaterEnable = true;
						t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
						Wicked_Update_Visuals((void *)&t.visuals);

						//Create Level
						ggterrain_extra_params.iProceduralTerrainType = 3;
						ggterrain_global_params.fractal_initial_amplitude = 1.0f;

						t.gamevisuals.bEndableAmbientMusicTrack = t.visuals.bEndableAmbientMusicTrack = true;
						t.visuals.sAmbientMusicTrack = "audiobank\\ambient\\Forest.wav";
						t.gamevisuals.sAmbientMusicTrack = t.visuals.sAmbientMusicTrack;
						t.gamevisuals.bEnableCombatMusicTrack = t.visuals.bEnableCombatMusicTrack = false;
						t.visuals.sCombatMusicTrack = "";
						t.gamevisuals.sCombatMusicTrack = t.visuals.sCombatMusicTrack;

						ggtrees_global_params.water_dist = 41.0;
						bTriggerStableY = true;

					}
					if (iSelectedThemeChoice == 3)
					{
						ImVec2 padding = { 0.0, 0.0 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(fButSizeX, fButSizeY));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(outline_color), 0.0f, 15, 2.0f);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Forest Theme");
					ImGui::SameLine();

					vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					if (ImGui::StyleButton("Snow", ImVec2(fButSizeX, fButSizeY)) || iRandomThemeChoice == 4) //Arctic
					{
						if (bRandomizeTimeOfDay && iRandomThemeChoice == 0) iRandomTimeOfDayChoice = (rand() % 7);
						if (iRandomThemeChoice == 0) bSelectRandomSkybox = true;
						iSelectedThemeChoice = 4;

						GGTerrainFile_LoadTerrainData("editors\\biomes\\snow.dat", false);
						ggterrain_global_params.seed = Random2();
						t.showeditortrees = t.gamevisuals.bEndableTreeDrawing = t.visuals.bEndableTreeDrawing = 1;
						t.showeditorveg = t.gamevisuals.bEndableGrassDrawing = t.visuals.bEndableGrassDrawing = 1;
						t.showeditorterrain = t.gamevisuals.bEndableTerrainDrawing = t.visuals.bEndableTerrainDrawing = 1;

						if (iLastTreeGrassSettings != 4)
						{
							iLastTreeGrassSettings = 4;
							//PE: Default tree and grass setup.
							ggtrees_global_params.paint_tree_bitfield = 520093696; //Snow
							ggtrees_global_params.paint_scale_random_low = 40.0; //Default Scale Min.
							ggtrees_global_params.paint_scale_random_high = 200.0; //Default Scale Max.
							GGTrees::GGTrees_ChangeDensity(10); //Default density.
							GGTrees::ggtrees_global_params.hide_until_update = 1;
							GGTrees::ggtrees_global_params.draw_enabled = 0;

							gggrass_global_params.paint_type = 1167; //Default grass
							gggrass_global_params.paint_density = 4; //Default Density
							gggrass_global_params.paint_material = 0; //Auto
							GGGrass::GGGrass_AddAll();
						}

						//PE: Default water settings.
						t.terrain.waterliney_f = g.gdefaultwaterheight = -500;
						t.gamevisuals.WaterSpeed1 = t.visuals.WaterSpeed1 = 0.06;
						t.gamevisuals.WaterRed_f = t.visuals.WaterRed_f = 6;
						t.gamevisuals.WaterGreen_f = t.visuals.WaterGreen_f = 32;
						t.gamevisuals.WaterBlue_f = t.visuals.WaterBlue_f = 80;
						t.gamevisuals.WaterAlpha_f = t.visuals.WaterAlpha_f = 0;
						t.gamevisuals.fWaterWaveAmplitude = t.visuals.fWaterWaveAmplitude = 20;
						t.gamevisuals.fWaterWindDependency = t.visuals.fWaterWindDependency = 0;
						t.gamevisuals.fWaterPatchLength = t.visuals.fWaterPatchLength = 40;
						t.gamevisuals.fWaterChoppyScale = t.visuals.fWaterChoppyScale = 0;
						t.gamevisuals.WaterFogMinDist = t.visuals.WaterFogMinDist = 0;
						t.gamevisuals.WaterFogMaxDist = t.visuals.WaterFogMaxDist = 1000;
						t.gamevisuals.WaterFogMinAmount = t.visuals.WaterFogMinAmount = 0.2;

						t.visuals.bWaterEnable = true;
						t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
						Wicked_Update_Visuals((void *)&t.visuals);

						//Create Level
						ggterrain_extra_params.iProceduralTerrainType = 4;
						ggterrain_global_params.fractal_initial_amplitude = 1.0f;

						t.gamevisuals.bEndableAmbientMusicTrack = t.visuals.bEndableAmbientMusicTrack = true;
						t.visuals.sAmbientMusicTrack = "audiobank\\ambient\\Arctic.wav";
						t.gamevisuals.sAmbientMusicTrack = t.visuals.sAmbientMusicTrack;
						t.gamevisuals.bEnableCombatMusicTrack = t.visuals.bEnableCombatMusicTrack = false;
						t.visuals.sCombatMusicTrack = "";
						t.gamevisuals.sCombatMusicTrack = t.visuals.sCombatMusicTrack;

						ggtrees_global_params.water_dist = 1251.0;
						bTriggerStableY = true;

					}
					if (iSelectedThemeChoice == 4)
					{
						ImVec2 padding = { 0.0, 0.0 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(fButSizeX, fButSizeY));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(outline_color), 0.0f, 15, 2.0f);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Arctic Theme");

					//Next line.
					vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					if (ImGui::StyleButton("Canyon", ImVec2(fButSizeX, fButSizeY)) || iRandomThemeChoice == 5)
					{
						if (bRandomizeTimeOfDay && iRandomThemeChoice == 0) iRandomTimeOfDayChoice = (rand() % 7);
						if (iRandomThemeChoice == 0) bSelectRandomSkybox = true;
						iSelectedThemeChoice = 5;

						GGTerrainFile_LoadTerrainData("editors\\biomes\\canyon.dat", false);
						ggterrain_global_params.seed = Random2();
						t.showeditortrees = t.gamevisuals.bEndableTreeDrawing = t.visuals.bEndableTreeDrawing = 1;
						t.showeditorveg = t.gamevisuals.bEndableGrassDrawing = t.visuals.bEndableGrassDrawing = 1;
						t.showeditorterrain = t.gamevisuals.bEndableTerrainDrawing = t.visuals.bEndableTerrainDrawing = 1;

						if (iLastTreeGrassSettings != 5)
						{
							iLastTreeGrassSettings = 5;

							//PE: Default tree and grass setup.
							ggtrees_global_params.paint_tree_bitfield = 15728641; //Canyon
							ggtrees_global_params.paint_scale_random_low = 40.0; //Default Scale Min.
							ggtrees_global_params.paint_scale_random_high = 200.0; //Default Scale Max.
							GGTrees::ggtrees_global_params.hide_until_update = 1;
							GGTrees::ggtrees_global_params.draw_enabled = 0;
							GGTrees::GGTrees_ChangeDensity(66); //Default density.
							gggrass_global_params.paint_type = 15; //Default grass
							gggrass_global_params.paint_density = 100; //Default Density
							gggrass_global_params.paint_material = 0; //Auto
							GGGrass::GGGrass_AddAll();
						}

						//PE: Default water settings.
						t.terrain.waterliney_f = g.gdefaultwaterheight = -104;
						t.gamevisuals.WaterSpeed1 = t.visuals.WaterSpeed1 = 0.06;
						t.gamevisuals.WaterRed_f = t.visuals.WaterRed_f = 1;
						t.gamevisuals.WaterGreen_f = t.visuals.WaterGreen_f = 12;
						t.gamevisuals.WaterBlue_f = t.visuals.WaterBlue_f = 8;
						t.gamevisuals.WaterAlpha_f = t.visuals.WaterAlpha_f = 0;
						t.gamevisuals.fWaterWaveAmplitude = t.visuals.fWaterWaveAmplitude = 20;
						t.gamevisuals.fWaterWindDependency = t.visuals.fWaterWindDependency = 0;
						t.gamevisuals.fWaterPatchLength = t.visuals.fWaterPatchLength = 40;
						t.gamevisuals.fWaterChoppyScale = t.visuals.fWaterChoppyScale = 0;
						t.gamevisuals.WaterFogMinDist = t.visuals.WaterFogMinDist = 0;
						t.gamevisuals.WaterFogMaxDist = t.visuals.WaterFogMaxDist = 2000;
						t.gamevisuals.WaterFogMinAmount = t.visuals.WaterFogMinAmount = 0.089;

						t.visuals.bWaterEnable = true;
						t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
						Wicked_Update_Visuals((void *)&t.visuals);

						//Create Level
						ggterrain_extra_params.iProceduralTerrainType = 5;
						ggterrain_global_params.fractal_initial_amplitude = 1.0f;

						t.gamevisuals.bEndableAmbientMusicTrack = t.visuals.bEndableAmbientMusicTrack = true;
						t.visuals.sAmbientMusicTrack = "audiobank\\ambient\\Canyon.wav";
						t.gamevisuals.sAmbientMusicTrack = t.visuals.sAmbientMusicTrack;
						t.gamevisuals.bEnableCombatMusicTrack = t.visuals.bEnableCombatMusicTrack = false;
						t.visuals.sCombatMusicTrack = "";
						t.gamevisuals.sCombatMusicTrack = t.visuals.sCombatMusicTrack;

						ggtrees_global_params.water_dist = 3155.0; //551.0;
						bTriggerStableY = true;

					}
					if (iSelectedThemeChoice == 5)
					{
						ImVec2 padding = { 0.0, 0.0 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(fButSizeX, fButSizeY));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(outline_color), 0.0f, 15, 2.0f);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Canyon Theme");
					ImGui::SameLine();

					vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					if (ImGui::StyleButton("Mountain", ImVec2(fButSizeX, fButSizeY)) || iRandomThemeChoice == 6)
					{
						if (bRandomizeTimeOfDay && iRandomThemeChoice == 0) iRandomTimeOfDayChoice = (rand() % 7);
						if (iRandomThemeChoice == 0) bSelectRandomSkybox = true;
						iSelectedThemeChoice = 6;

						GGTerrainFile_LoadTerrainData("editors\\biomes\\mountain.dat", false);
						ggterrain_global_params.seed = Random2();
						t.showeditortrees = t.gamevisuals.bEndableTreeDrawing = t.visuals.bEndableTreeDrawing = 0;
						t.showeditorveg = t.gamevisuals.bEndableGrassDrawing = t.visuals.bEndableGrassDrawing = 0;
						t.showeditorterrain = t.gamevisuals.bEndableTerrainDrawing = t.visuals.bEndableTerrainDrawing = 1;

						if (iLastTreeGrassSettings != 6)
						{
							iLastTreeGrassSettings = 6;

							//PE: Default tree and grass setup.
							ggtrees_global_params.paint_tree_bitfield = 9673113696; //Mountain
							ggtrees_global_params.paint_scale_random_low = 40.0; //Default Scale Min.
							ggtrees_global_params.paint_scale_random_high = 200.0; //Default Scale Max.
							GGTrees::GGTrees_ChangeDensity(40); //Default density.
							GGTrees::ggtrees_global_params.hide_until_update = 1;
							GGTrees::ggtrees_global_params.draw_enabled = 0;
							gggrass_global_params.paint_type = 1; //Default grass
							gggrass_global_params.paint_density = 100; //Default Density
							gggrass_global_params.paint_material = 0; //Auto
							GGGrass::GGGrass_AddAll();
						}

						//PE: Default water settings.
						t.terrain.waterliney_f = g.gdefaultwaterheight = -500;
						t.gamevisuals.WaterSpeed1 = t.visuals.WaterSpeed1 = 0.06;
						t.gamevisuals.WaterRed_f = t.visuals.WaterRed_f = 9;
						t.gamevisuals.WaterGreen_f = t.visuals.WaterGreen_f = 21;
						t.gamevisuals.WaterBlue_f = t.visuals.WaterBlue_f = 43;
						t.gamevisuals.WaterAlpha_f = t.visuals.WaterAlpha_f = 0;
						t.gamevisuals.fWaterWaveAmplitude = t.visuals.fWaterWaveAmplitude = 20;
						t.gamevisuals.fWaterWindDependency = t.visuals.fWaterWindDependency = 0;
						t.gamevisuals.fWaterPatchLength = t.visuals.fWaterPatchLength = 40;
						t.gamevisuals.fWaterChoppyScale = t.visuals.fWaterChoppyScale = 0;
						t.gamevisuals.WaterFogMinDist = t.visuals.WaterFogMinDist = 0;
						t.gamevisuals.WaterFogMaxDist = t.visuals.WaterFogMaxDist = 11500;
						t.gamevisuals.WaterFogMinAmount = t.visuals.WaterFogMinAmount = 0.25;

						t.visuals.bWaterEnable = true;
						t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
						Wicked_Update_Visuals((void *)&t.visuals);

						ggterrain_extra_params.iProceduralTerrainType = 6;
						ggterrain_global_params.fractal_initial_amplitude = 1.0f;

						t.gamevisuals.bEndableAmbientMusicTrack = t.visuals.bEndableAmbientMusicTrack = true;
						t.visuals.sAmbientMusicTrack = "audiobank\\ambient\\Mountain.wav";
						t.gamevisuals.sAmbientMusicTrack = t.visuals.sAmbientMusicTrack;
						t.gamevisuals.bEnableCombatMusicTrack = t.visuals.bEnableCombatMusicTrack = false;
						t.visuals.sCombatMusicTrack = "";
						t.gamevisuals.sCombatMusicTrack = t.visuals.sCombatMusicTrack;

						ggtrees_global_params.water_dist = 400.0;
						bTriggerStableY = true;

					}
					if (iSelectedThemeChoice == 6)
					{
						ImVec2 padding = { 0.0, 0.0 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(fButSizeX, fButSizeY));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(outline_color), 0.0f, 15, 2.0f);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Mountain Theme");
					ImGui::SameLine();

					vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					if (ImGui::StyleButton("Rainforest", ImVec2(fButSizeX, fButSizeY)) || iRandomThemeChoice == 7)
					{
						if (bRandomizeTimeOfDay && iRandomThemeChoice == 0) iRandomTimeOfDayChoice = (rand() % 7);
						if (iRandomThemeChoice == 0) bSelectRandomSkybox = true;
						iSelectedThemeChoice = 7;

						GGTerrainFile_LoadTerrainData("editors\\biomes\\rainforest.dat", false);
						ggterrain_global_params.seed = Random2();
						t.showeditortrees = t.gamevisuals.bEndableTreeDrawing = t.visuals.bEndableTreeDrawing = 1;
						t.showeditorveg = t.gamevisuals.bEndableGrassDrawing = t.visuals.bEndableGrassDrawing = 1;
						t.showeditorterrain = t.gamevisuals.bEndableTerrainDrawing = t.visuals.bEndableTerrainDrawing = 1;

						if (iLastTreeGrassSettings != 7)
						{
							iLastTreeGrassSettings = 7;
							//PE: Default tree and grass setup.
							ggtrees_global_params.paint_tree_bitfield = 6443498496; //rain forest.
							ggtrees_global_params.paint_scale_random_low = 40.0; //Default Scale Min.
							ggtrees_global_params.paint_scale_random_high = 200.0; //Default Scale Max.
							GGTrees::GGTrees_ChangeDensity(90); //Default density.
							GGTrees::ggtrees_global_params.hide_until_update = 1;
							GGTrees::ggtrees_global_params.draw_enabled = 0;
							gggrass_global_params.paint_type = 15; //Default grass
							gggrass_global_params.paint_density = 100; //Default Density
							gggrass_global_params.paint_material = 0; //Auto
							GGGrass::GGGrass_AddAll();
						}

						//PE: Default water settings.
						t.terrain.waterliney_f = g.gdefaultwaterheight = 104;
						t.gamevisuals.WaterSpeed1 = t.visuals.WaterSpeed1 = 0.06;
						t.gamevisuals.WaterRed_f = t.visuals.WaterRed_f = 12;
						t.gamevisuals.WaterGreen_f = t.visuals.WaterGreen_f = 8;
						t.gamevisuals.WaterBlue_f = t.visuals.WaterBlue_f = 0;
						t.gamevisuals.WaterAlpha_f = t.visuals.WaterAlpha_f = 0;
						t.gamevisuals.fWaterWaveAmplitude = t.visuals.fWaterWaveAmplitude = 20;
						t.gamevisuals.fWaterWindDependency = t.visuals.fWaterWindDependency = 0;
						t.gamevisuals.fWaterPatchLength = t.visuals.fWaterPatchLength = 40;
						t.gamevisuals.fWaterChoppyScale = t.visuals.fWaterChoppyScale = 0;
						t.gamevisuals.WaterFogMinDist = t.visuals.WaterFogMinDist = 0;
						t.gamevisuals.WaterFogMaxDist = t.visuals.WaterFogMaxDist = 1000;
						t.gamevisuals.WaterFogMinAmount = t.visuals.WaterFogMinAmount = 0.25;

						t.visuals.bWaterEnable = true;
						t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
						Wicked_Update_Visuals((void *)&t.visuals);

						//Create Level
						ggterrain_extra_params.iProceduralTerrainType = 7;
						ggterrain_global_params.fractal_initial_amplitude = 1.0f;

						t.gamevisuals.bEndableAmbientMusicTrack = t.visuals.bEndableAmbientMusicTrack = true;
						t.visuals.sAmbientMusicTrack = "audiobank\\ambient\\Rainforest.wav";
						t.gamevisuals.sAmbientMusicTrack = t.visuals.sAmbientMusicTrack;
						t.gamevisuals.bEnableCombatMusicTrack = t.visuals.bEnableCombatMusicTrack = false;
						t.visuals.sCombatMusicTrack = "";
						t.gamevisuals.sCombatMusicTrack = t.visuals.sCombatMusicTrack;

						ggtrees_global_params.water_dist = 212.0; //22.0;
						bTriggerStableY = true;

					}
					if (iSelectedThemeChoice == 7)
					{
						ImVec2 padding = { 0.0, 0.0 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(fButSizeX, fButSizeY));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(outline_color), 0.0f, 15, 2.0f);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Rainforest Theme");
					ImGui::SameLine();

					vSelectionDraw = ImGui::GetCurrentWindow()->DC.CursorPos;
					if (ImGui::StyleButton("Empty", ImVec2(fButSizeX, fButSizeY)))
					{
						if (bRandomizeTimeOfDay && iRandomThemeChoice == 0) iRandomTimeOfDayChoice = (rand() % 7);
						if (iRandomThemeChoice == 0) bSelectRandomSkybox = true;
						iSelectedThemeChoice = 8;

						// sets ggterrain_global_params.iProceduralTerrainType to 0
						t.showeditortrees = t.gamevisuals.bEndableTreeDrawing = t.visuals.bEndableTreeDrawing = 0;
						t.showeditorveg = t.gamevisuals.bEndableGrassDrawing = t.visuals.bEndableGrassDrawing = 0;
						t.showeditorterrain = t.gamevisuals.bEndableTerrainDrawing = t.visuals.bEndableTerrainDrawing = 1;

						//PE: Default tree and grass setup.
						if (iLastTreeGrassSettings != 0)
						{
							iLastTreeGrassSettings = 0;
							ggtrees_global_params.paint_tree_bitfield = 1; //Default
							ggtrees_global_params.paint_scale_random_low = 40.0; //Default Scale Min.
							ggtrees_global_params.paint_scale_random_high = 200.0; //Default Scale Max.
							GGTrees::ggtrees_global_params.hide_until_update = 1;
							GGTrees::ggtrees_global_params.draw_enabled = 0;
							GGTrees::GGTrees_ChangeDensity(65); //Default density.
							gggrass_global_params.paint_type = 1; //Default grass
							gggrass_global_params.paint_material = 0; //Auto
							gggrass_global_params.paint_density = 100; //Default Density
							GGGrass::GGGrass_AddAll();
						}

						//PE: Default water settings.
						t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable = false;
						t.terrain.waterliney_f = g.gdefaultwaterheight = -500;
						t.gamevisuals.WaterSpeed1 = t.visuals.WaterSpeed1 = 0.06;
						t.gamevisuals.WaterRed_f = t.visuals.WaterRed_f = 9;
						t.gamevisuals.WaterGreen_f = t.visuals.WaterGreen_f = 21;
						t.gamevisuals.WaterBlue_f = t.visuals.WaterBlue_f = 43;
						t.gamevisuals.WaterAlpha_f = t.visuals.WaterAlpha_f = 0;
						t.gamevisuals.fWaterWaveAmplitude = t.visuals.fWaterWaveAmplitude = 20;
						t.gamevisuals.fWaterWindDependency = t.visuals.fWaterWindDependency = 0;
						t.gamevisuals.fWaterPatchLength = t.visuals.fWaterPatchLength = 40;
						t.gamevisuals.fWaterChoppyScale = t.visuals.fWaterChoppyScale = 0;
						t.gamevisuals.WaterFogMinDist = t.visuals.WaterFogMinDist = 0;
						t.gamevisuals.WaterFogMaxDist = t.visuals.WaterFogMaxDist = 11500;
						t.gamevisuals.WaterFogMinAmount = t.visuals.WaterFogMinAmount = 0.25;

						procedural_set_empty_level(true);

						t.gamevisuals.bEndableAmbientMusicTrack = t.visuals.bEndableAmbientMusicTrack = false;
						t.visuals.sAmbientMusicTrack = "";
						t.gamevisuals.sAmbientMusicTrack = t.visuals.sAmbientMusicTrack;
						t.gamevisuals.bEnableCombatMusicTrack = t.visuals.bEnableCombatMusicTrack = false;
						t.visuals.sCombatMusicTrack = "";
						t.gamevisuals.sCombatMusicTrack = t.visuals.sCombatMusicTrack;

						ggtrees_global_params.water_dist = 400.0;
						bTriggerStableY = true;
					}
					if (iSelectedThemeChoice == 8)
					{
						ImVec2 padding = { 0.0, 0.0 };
						const ImRect image_bb((vSelectionDraw - padding), vSelectionDraw + padding + ImVec2(fButSizeX, fButSizeY));
						ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(outline_color), 0.0f, 15, 2.0f);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select a Empty Level consisting of a blank grey grid");


					ggterrain_global_render_params2.editable_size = feditable_size;
					ImGui::PopItemWidth();
				}

				if (ImGui::StyleCollapsingHeader("Terrain Shape", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Indent(10);

					ImGui::TextCenter("Editable Area Size");
					float numericboxwidth = 60.0f;

					ImGui::PushItemWidth(-10 - 10 - numericboxwidth);

					float fTmp = GGTerrain_UnitsToMeters(ggterrain_global_render_params2.editable_size * 2.0) / 1000.0f;
					if (ImGui::SliderFloat("##UI2TerrainEditableSizeKilometers", &fTmp, 0.5, 5.0f, " "))
					{
						//Cam zoom. 420000 = 5.0 , 50000 = 0.5 , perhaps 48000 per 0.5
						ggterrain_global_render_params2.editable_size = GGTerrain_MetersToUnits(fTmp / 2.0) * 1000.0f;

						//Reset camera to point at center of edit area.
						if (fSnapShotModeCameraY < (fTmp * 41000.0f))
						{
							fSnapShotModeCameraY = fTmp * 41000.0f;
							if (fSnapShotModeCameraY > 344000) fSnapShotModeCameraY = 344000; //Hide ugly shadow for now.
							fSnapShotModeCameraX = GGORIGIN_X; // +ggterrain_global_params.offset_x; It dont actual move from center.
							fSnapShotModeCameraZ = GGORIGIN_Z; // +ggterrain_global_params.offset_z;
							fSnapShotModeCameraAngZ = fSnapShotModeCameraAngY = 0.0f;
							fSnapShotModeCameraAngX = 90.0f; //Look down.
						}
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Editable Area Size in Kilometers.");
					ImGui::PopItemWidth();
					ImGui::SameLine();
					ImGui::PushItemWidth(numericboxwidth);
					if (ImGui::InputFloat("##UI2TerrainEditableSizeKilometersText", &fTmp, 0, 0, "%.1f Km"))
					{
						//ggterrain_global_render_params2.editable_size = fTmp * 39.3701 * 1000.0f;
						ggterrain_global_render_params2.editable_size = GGTerrain_MetersToUnits(fTmp / 2.0) * 1000.0f;
						fSnapShotModeCameraY = fTmp * 48000.0f;
					}
					if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Editable Area Size in Kilometers.");

					ImGui::PopItemWidth();

					if (pref.iTerrainAdvanced)
					{
						if (ImGui::Checkbox("Show Editable Area", &bShowEditArea))
						{
						}
						ImGui::SameLine();

						bShow3DBoundary = (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D) != 0;
						if (ImGui::Checkbox("3D Boundary##bShow3DBoundary", &bShow3DBoundary))
						{
							if (bShow3DBoundary) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
							else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
						}
					}
					#ifndef NOMINIMAP
					bShowMiniMap = (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP) != 0;
					if (ImGui::Checkbox("Show Mini Map##bShowMiniMap", &bShowMiniMap))
					{
						if (bShowMiniMap) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP;
						else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP;
					}
					#endif
					ImGui::Indent(-10);
				}

				// allow terrain to be changed except for EMPTY mode
				if (ggterrain_extra_params.iProceduralTerrainType != 0)
				{
					if (ImGui::StyleCollapsingHeader("Terrain Values", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Indent(10);

						ImGui::TextCenter("Height Range");

						//Height Range
						//FYI: fTerrainHeightStart is missing in ggterrain_global_params.
						ImGui::PushItemWidth(-10 - 10 - 60);
						ImGui::TextCenter("Max Height (meters)");
						float meterValue = GGTerrain_UnitsToMeters(ggterrain_global_params.height);
						if(ImGui::MaxSliderInputFloatPower("##HeightRange", &meterValue, 0.0f, 1000.0f, 0, 0, 1000, 60, 2.0f, 2 ))
						{
							ggterrain_global_params.height = GGTerrain_MetersToUnits(meterValue);
							bTriggerStableY = true;
						}

						if (pref.iTerrainAdvanced)
						{
							ImGui::TextCenter("Valley Depth (meters)");
							meterValue = GGTerrain_UnitsToMeters(ggterrain_global_params.minHeight);
							if(ImGui::MaxSliderInputFloatPower("##MinHeightRange", &meterValue, 0.0f, 1000.0f, 0, 0, 1000, 60, 2.0f, 2 ))
							//if (ImGui::SliderFloat("##MinHeightRange", &meterValue, 0.0f, 1000.0f, "%.2f"))
							{
								ggterrain_global_params.minHeight = GGTerrain_MetersToUnits(meterValue);
								bTriggerStableY = true;
							}
						}
						ImGui::PopItemWidth();

						//Water Height
						//One meter = 39.3701 inch.
						ImGui::TextCenter("Water Height (meters)");
						float numericboxwidth = 60.0f;
						ImGui::PushItemWidth(-10 - 10 - numericboxwidth);
						float waterHeight = GGTerrain_UnitsToMeters(g.gdefaultwaterheight);
						if (ImGui::SliderFloat("##UI2fWaterHeightMeters", &waterHeight, -500.0, 1500.0f, "%.1f", 2.0f))
						{
							g.gdefaultwaterheight = (int)GGTerrain_MetersToUnits(waterHeight);
							t.terrain.waterliney_f = (float)g.gdefaultwaterheight;
							Wicked_Update_Visuals((void *)&t.visuals);
							ggterrain_extra_params.iUpdateTrees = 1;
							bTriggerStableY = true;
							fLastY = -1; //Trigger fog update.
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Water Height in Meters.");
						ImGui::PopItemWidth();
						ImGui::SameLine();
						ImGui::PushItemWidth(numericboxwidth);
						if (ImGui::InputFloat("##UI2fWaterHeightMetersText", &waterHeight, 0, 0, "%.1f M"))
						{
							g.gdefaultwaterheight = (int)GGTerrain_MetersToUnits(waterHeight);
							t.terrain.waterliney_f = (float)g.gdefaultwaterheight;
							Wicked_Update_Visuals((void *)&t.visuals);
							ggterrain_extra_params.iUpdateTrees = 1;
							bTriggerStableY = true;
							fLastY = -1; //Trigger fog update.

						}
						if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Water Height in Meters.");
						ImGui::PopItemWidth();

						ImGui::TextCenter("Time of Day");

						const char* time_combo[] = { "Dawn", "Morning", "Midday","Afternoon", "Evening", "Dusk","Night" ,"Skybox: Fixed Time of Day" };
						ImGui::PushItemWidth(-10);

						int iChoises = IM_ARRAYSIZE(time_combo) - 1;
						int iSelection = t.visuals.iTimeOfday;
						bool bReadOnlyMode = !t.visuals.bDisableSkybox;
						if (t.visuals.skyindex == 0 && t.visuals.bDisableSkybox == false) bReadOnlyMode = false; //PE: Dynamic.
						if (bReadOnlyMode) iChoises++;
						if (bReadOnlyMode) iSelection = 7;

						if (!bReadOnlyMode)
						{
							ImGui::Checkbox("Random Time of Day", &bRandomizeTimeOfDay);
						}

						if (bReadOnlyMode)
						{
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
						}

						if (ImGui::Combo("##ComboTimeOfDay2", &iSelection, time_combo, iChoises) || iRandomTimeOfDayChoice >= 0)
						{
							t.visuals.iTimeOfday = iSelection;
							static int iLastRandomTimeOfDayChoice = -1;
							if (iRandomTimeOfDayChoice >= 0)
							{
								if (iLastRandomTimeOfDayChoice == iRandomTimeOfDayChoice)
								{
									iRandomTimeOfDayChoice++;
									if (iLastRandomTimeOfDayChoice == 6) iRandomTimeOfDayChoice = 0;
								}
							}
							else
							{
								//PE: Disable randomize when user make a selection.
								iLastUserSelectedTimeOfDay = iSelection;
								bRandomizeTimeOfDay = false;
							}
							if (iRandomTimeOfDayChoice > 6) iRandomTimeOfDayChoice = 6;

							if (bFirstBiomes[iSelectedThemeChoice])
							{
								bFirstBiomes[iSelectedThemeChoice] = false;
								bSelectDayRandomSkybox = true;
								//Select a day time.
								if ((rand() % 3) == 1)
									iRandomTimeOfDayChoice = 1;
								else if ((rand() % 3) == 1)
									iRandomTimeOfDayChoice = 4;
								else
									iRandomTimeOfDayChoice = 3;
							}

							if(iRandomTimeOfDayChoice >=0 )
								t.gamevisuals.iTimeOfday = t.visuals.iTimeOfday = iRandomTimeOfDayChoice;
							else
								t.gamevisuals.iTimeOfday = t.visuals.iTimeOfday;
							iLastRandomTimeOfDayChoice = iRandomTimeOfDayChoice;
							visuals_calcsunanglefromtimeofday(t.gamevisuals.iTimeOfday, &t.gamevisuals.SunAngleX, &t.gamevisuals.SunAngleY, &t.gamevisuals.SunAngleZ);
							t.visuals.SunAngleX = t.gamevisuals.SunAngleX;
							t.visuals.SunAngleY = t.gamevisuals.SunAngleY;
							t.visuals.SunAngleZ = t.gamevisuals.SunAngleZ;

							oldSunAngleX = t.visuals.SunAngleX; //PE: Make sure to update after save.
							oldSunAngleY = t.visuals.SunAngleY;
							oldSunAngleZ = t.visuals.SunAngleZ;

							Wicked_Update_Visuals((void *)&t.visuals);
							bTriggerStableY = true;
							fLastY = -1; //Trigger fog update.
						}

						if (bReadOnlyMode)
						{
							ImGui::PopItemFlag();
							ImGui::PopStyleVar();
						}

						if (ImGui::IsItemHovered()) ImGui::SetTooltip("This sets the sun at the correct position for the time of day");
						ImGui::PopItemWidth();


						//Advanced start here!

						extern void ControlAdvancedSetting(int&, const char*, bool* = nullptr);
						bool bStateUnchanged = true;
						ControlAdvancedSetting(pref.iTerrainAdvanced, "advanced terrain tools", &bStateUnchanged);

						if (pref.iTerrainAdvanced)
						{
							cstr cSpecialTooltip = "";
							numericboxwidth = 60.0f;

							//ggterrain_global_params.noise_power
							ImGui::ImgBtn(ICON_INFO, ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false);
							if (ImGui::IsItemHovered())
							{
								cSpecialTooltip = "Values above 1 make lower areas flatter, \nvalues less than 1 make higher areas flatter, \na value of 1 does not modify the noise value";
							}
							ImGui::SameLine(); ImGui::SetCursorPosX(10);
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0);
							ImGui::TextCenter("Noise Curve");
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6.0);
							ImGui::PushItemWidth(-10 - 10 - numericboxwidth);
							if (ImGui::SliderFloat("##UI2fNoisePower", &ggterrain_global_params.noise_power, 0.0f, 10.0f, " "))
							{
								bTriggerStableY = true;
							}
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Curve.");
							ImGui::PopItemWidth();
							ImGui::SameLine();
							ImGui::PushItemWidth(numericboxwidth);
							if (ImGui::InputFloat("##UI2fNoisePowerText", &ggterrain_global_params.noise_power, 0, 0, "%.1f"))
							{
								bTriggerStableY = true;
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Curve.");
							ImGui::PopItemWidth();

							ImGui::ImgBtn(ICON_INFO, ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false);
							if (ImGui::IsItemHovered())
							{
								cSpecialTooltip = "A value of 0 does not modify the noise value, \nvalues greater than 0 make lower areas smoother";
							}
							ImGui::SameLine(); ImGui::SetCursorPosX(10);
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0);
							ImGui::TextCenter("Noise Falloff");
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6.0);
							ImGui::PushItemWidth(-10 - 10 - numericboxwidth);
							if (ImGui::SliderFloat("##UI2fNoiseFalloffPower", &ggterrain_global_params.noise_fallof_power, 0.0f, 10.0f, " "))
							{
								bTriggerStableY = true;
							}
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Falloff.");
							ImGui::PopItemWidth();
							ImGui::SameLine();
							ImGui::PushItemWidth(numericboxwidth);
							if (ImGui::InputFloat("##UI2fNoiseFalloffPowerText", &ggterrain_global_params.noise_fallof_power, 0, 0, "%.1f"))
							{
								bTriggerStableY = true;
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Falloff.");
							ImGui::PopItemWidth();

							ImGui::ImgBtn(ICON_INFO, ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false);
							if (ImGui::IsItemHovered())
							{
								cSpecialTooltip = "The number of iterations of noise to use which get \nlayered on top of each other, the \nhigher the value the more bumpy the \nterrain will be, good \nvalues are around 10";
							}
							ImGui::SameLine(); ImGui::SetCursorPosX(10);
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0);
							ImGui::TextCenter("Noise Iterations");
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6.0);
							ImGui::PushItemWidth(-10 - 10 - numericboxwidth);
							float fTmp = ggterrain_global_params.fractal_levels;
							if (ImGui::SliderFloat("##UI2fFractalIterations", &fTmp, 1.0f, 14.0f, " "))
							{
								ggterrain_global_params.fractal_levels = int(fTmp);
								bTriggerStableY = true;
							}
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Iterations.");
							ImGui::PopItemWidth();
							ImGui::SameLine();
							ImGui::PushItemWidth(numericboxwidth);
							if (ImGui::InputFloat("##UI2fFractalIterationsText", &fTmp, 0, 0, "%.0f"))
							{
								ggterrain_global_params.fractal_levels = int(fTmp);
								bTriggerStableY = true;
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Iterations.");
							ImGui::PopItemWidth();

							ImGui::ImgBtn(ICON_INFO, ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false);
							if (ImGui::IsItemHovered())
							{
								cSpecialTooltip = "The frequency of the first layer of noise, \nlarger values will have smaller terrain features, \nsmaller values will have larger terrain features. \nSmaller values often require a larger \nheight value to get a good effect";
							}
							ImGui::SameLine(); ImGui::SetCursorPosX(10);
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0);
							ImGui::TextCenter("Noise Initial Frequency");
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6.0);
							ImGui::PushItemWidth(-10 - 10 - numericboxwidth);
							if (ImGui::SliderFloat("##UI2fFractalInitialFrequence", &ggterrain_global_params.fractal_initial_freq, 0.01f, 8.0f, " "))
							{
								bTriggerStableY = true;

							}
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Initial Frequency.");
							ImGui::PopItemWidth();
							ImGui::SameLine();
							ImGui::PushItemWidth(numericboxwidth);
							if (ImGui::InputFloat("##UI2fFractalInitialFrequenceText", &ggterrain_global_params.fractal_initial_freq, 0, 0, "%.1f"))
							{
								bTriggerStableY = true;

							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Initial Frequency.");
							ImGui::PopItemWidth();

							//PE: Noise Initial Amplitude
							ImGui::ImgBtn(ICON_INFO, ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false);
							if (ImGui::IsItemHovered())
							{
								cSpecialTooltip = "Noise Initial Amplitude: The multiplier used on the first noise layer.\nSetting this to 0 will make everything flat, whereas a value of 1\nwill utilise the full height range set by the Max Height parameter";
							}
							ImGui::SameLine(); ImGui::SetCursorPosX(10);
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0);
							ImGui::TextCenter("Noise Initial Amplitude");
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6.0);
							ImGui::PushItemWidth(-10 - 10 - numericboxwidth);
							if (ImGui::SliderFloat("##UI2fNoise Initial Amplitude", &ggterrain_global_params.fractal_initial_amplitude, 0.0f, 1.0f, " "))
							{
								bTriggerStableY = true;
							}
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Initial Amplitude.");
							ImGui::PopItemWidth();
							ImGui::SameLine();
							ImGui::PushItemWidth(numericboxwidth);
							if (ImGui::InputFloat("##UI2fNoise Initial AmplitudeText", &ggterrain_global_params.fractal_initial_amplitude, 0, 0, "%.1f"))
							{
								bTriggerStableY = true;
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Initial Amplitude.");
							ImGui::PopItemWidth();

							ImGui::ImgBtn(ICON_INFO, ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false);
							if (ImGui::IsItemHovered())
							{
								cSpecialTooltip = "How the frequency of the noise changes with \neach additional iteration, typically the frequency \nwill change by a multiple of 2.4 for each iteration. \nFor best results this value should be changed \nwith the \"Noise Amplitude Change\" value so that \n(Noise Amplitude Change)*(Noise Frequency Change) is close to 1";
							}
							ImGui::SameLine(); ImGui::SetCursorPosX(10);
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0);
							ImGui::TextCenter("Noise Frequency Change");
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6.0);
							ImGui::PushItemWidth(-10 - 10 - numericboxwidth);
							if (ImGui::SliderFloat("##UI2fFractalFrequenceIncrease", &ggterrain_global_params.fractal_freq_increase, 0.01f, 8.0f, " "))
							{
								bTriggerStableY = true;
							}
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Frequency Change.");
							ImGui::PopItemWidth();
							ImGui::SameLine();
							ImGui::PushItemWidth(numericboxwidth);
							if (ImGui::InputFloat("##UI2fFractalFrequenceIncreaseText", &ggterrain_global_params.fractal_freq_increase, 0, 0, "%.1f"))
							{
								bTriggerStableY = true;
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Frequency Change.");
							ImGui::PopItemWidth();

							ImGui::ImgBtn(ICON_INFO, ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false);
							if (ImGui::IsItemHovered())
							{
								cSpecialTooltip = "How the amplitude of the noise changes with \neach additional iteration, typically the amplitude \nwill change by a multiple of 0.4 for each iteration. \nFor best results this value should be changed \nwith the \"Noise Frequency Change\" value so that \n(Noise Amplitude Change)*(Noise Frequency Change) is close to 1";
							}
							ImGui::SameLine(); ImGui::SetCursorPosX(10);
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0);
							ImGui::TextCenter("Noise Amplitude Change");
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6.0);
							ImGui::PushItemWidth(-10 - 10 - numericboxwidth);
							if (ImGui::SliderFloat("##UI2fFractalFrequenceWeight", &ggterrain_global_params.fractal_freq_weight, 0.0f, 2.0f, " "))
							{
								bTriggerStableY = true;
							}
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Amplitude Change.");
							ImGui::PopItemWidth();
							ImGui::SameLine();
							ImGui::PushItemWidth(numericboxwidth);
							if (ImGui::InputFloat("##UI2fFractalFrequenceWeightText", &ggterrain_global_params.fractal_freq_weight, 0, 0, "%.1f"))
							{
								bTriggerStableY = true;
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Noise Amplitude Change.");
							ImGui::PopItemWidth();

							//Seed value.
							ImGui::ImgBtn(ICON_INFO, ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false);
							if (ImGui::IsItemHovered())
							{
								cSpecialTooltip = "A random seed that is used as \nan input to the noise generator to \ncreate different results";
							}
							ImGui::SameLine(); ImGui::SetCursorPosX(10);
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0);
							ImGui::TextCenter("Noise Seed Value");
							ImGui::PushItemWidth(-10);
							char ctmp[80];
							sprintf(ctmp, "%u", ggterrain_global_params.seed);
							if (ImGui::InputText("##seedText", ctmp, 78, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsDecimal))
							{
								if (strlen(ctmp) > 0)
								{
									ggterrain_global_params.seed = atol(ctmp);
									bTriggerStableY = true;
							}
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Set Noise Seed Value");
							if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;
							ImGui::PopItemWidth();

							if (cSpecialTooltip != "")
							{
								//We are modal so special description popup.
								ImGui::SetNextWindowSize(ImVec2(500, 0), ImGuiCond_Always);
								ImGui::BeginTooltip();
								ImGui::Indent(12);
								ImGui::Text("");
								ImGui::PushItemWidth(-12);
								ImGui::TextWrapped(cSpecialTooltip.Get());
								ImGui::PopItemWidth();
								ImGui::Text("");
								ImGui::Indent(-12);
								ImGui::EndTooltip();
							}

						}

						ImGui::Indent(-10);
					}

					extern bool bProfilerEnable;
					if (bProfilerEnable)
					{
						ImGui::Separator();
						wiScene::Scene* pScene = &wiScene::GetScene();
						int iMeshes = pScene->meshes.GetCount();
						int iMaterials = pScene->materials.GetCount();

						int dc = wiProfiler::GetDrawCalls();
						int dcs = wiProfiler::GetDrawCallsShadows();
						int dct = wiProfiler::GetDrawCallsTransparent();

						int tris = wiProfiler::GetPolygons();
						int trisShadow = wiProfiler::GetPolygonsShadows();
						int trisTransparent = wiProfiler::GetPolygonsTransparent();

						ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
						ImGui::Text("DrawCalls: %d", dc);
						ImGui::Text("DrawCallsShadows: %d", dcs);
						ImGui::Text("DrawCallsTransparent: %d", dct);
						ImGui::Text("Triangles: %d", tris);
						ImGui::Text("TrianglesShadows: %d", trisShadow);
						ImGui::Text("TrianglesTransparent: %d", trisTransparent);
						ImGui::Text("Scene Meshes: %d", iMeshes);
						ImGui::Text("Scene Materials: %d", iMaterials);
						ImGui::Text("Scene Transforms: %d", (int)pScene->transforms.GetCount());
						ImGui::Text("Scene Hierarchy: %d", (int)pScene->hierarchy.GetCount());
						ImGui::Separator();
						std::string profiler_data = wiProfiler::GetProfilerData();
						ImGui::Text(profiler_data.c_str());
					}
				}
				else
				{
					ImGui::TextCenter("Time of Day");
				
					const char* time_combo[] = { "Dawn", "Morning", "Midday","Afternoon", "Evening", "Dusk","Night" , "Skybox: Fixed Time of Day" };

					int iChoises = IM_ARRAYSIZE(time_combo) - 1;
					int iSelection = t.visuals.iTimeOfday;
					bool bReadOnlyMode = !t.visuals.bDisableSkybox;
					if (t.visuals.skyindex == 0 && t.visuals.bDisableSkybox == false) bReadOnlyMode = false; //PE: Dynamic.
					if (bReadOnlyMode) iChoises++;
					if (bReadOnlyMode) iSelection = 7;

					if (!bReadOnlyMode)
					{
						ImGui::Checkbox("Random Time of Day", &bRandomizeTimeOfDay);
					}

					if (bReadOnlyMode)
					{
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
					}

					ImGui::PushItemWidth(-10);
					if (ImGui::Combo("##ComboTimeOfDay3", &iSelection, time_combo, iChoises) || iRandomTimeOfDayChoice >= 0)
					{
						t.visuals.iTimeOfday = iSelection;
						static int iLastRandomTimeOfDayChoice = -1;
						if (iRandomTimeOfDayChoice >= 0)
						{
							if (iLastRandomTimeOfDayChoice == iRandomTimeOfDayChoice)
							{
								iRandomTimeOfDayChoice++;
								if (iLastRandomTimeOfDayChoice == 6) iRandomTimeOfDayChoice = 0;
							}
						}
						else
						{
							//PE: Disable random when user make a selection.
							iLastUserSelectedTimeOfDay = iSelection;
							bRandomizeTimeOfDay = false;
						}
						if (iRandomTimeOfDayChoice > 6) iRandomTimeOfDayChoice = 6;

						if (bFirstBiomes[iSelectedThemeChoice])
						{
							bFirstBiomes[iSelectedThemeChoice] = false;
							bSelectDayRandomSkybox = true;
							//Select a day time.
							if ((rand() % 3) == 1)
								iRandomTimeOfDayChoice = 1;
							else if ((rand() % 3) == 1)
								iRandomTimeOfDayChoice = 4;
							else
								iRandomTimeOfDayChoice = 3;
						}

						if (iRandomTimeOfDayChoice >= 0)
							t.gamevisuals.iTimeOfday = t.visuals.iTimeOfday = iRandomTimeOfDayChoice;
						else
							t.gamevisuals.iTimeOfday = t.visuals.iTimeOfday;
						iLastRandomTimeOfDayChoice = iRandomTimeOfDayChoice;

						visuals_calcsunanglefromtimeofday(t.gamevisuals.iTimeOfday, &t.gamevisuals.SunAngleX, &t.gamevisuals.SunAngleY, &t.gamevisuals.SunAngleZ);
						t.visuals.SunAngleX = t.gamevisuals.SunAngleX;
						t.visuals.SunAngleY = t.gamevisuals.SunAngleY;
						t.visuals.SunAngleZ = t.gamevisuals.SunAngleZ;

						oldSunAngleX = t.visuals.SunAngleX; //PE: Make sure to update after save.
						oldSunAngleY = t.visuals.SunAngleY;
						oldSunAngleZ = t.visuals.SunAngleZ;

						Wicked_Update_Visuals((void *)&t.visuals);
						bTriggerStableY = true;
						fLastY = -1; //Trigger fog update.
					}

					if (bReadOnlyMode)
					{
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}

					if (ImGui::IsItemHovered()) ImGui::SetTooltip("This sets the sun at the correct position for the time of day");
					ImGui::PopItemWidth();
				}

				if (ImGui::StyleCollapsingHeader("Auto Populate Terrain", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Indent(10);
					float fButtonSizeX = (ImGui::GetContentRegionAvailWidth() - 10.0f);

					if (ImGui::Checkbox("Trees", &t.visuals.bEndableTreeDrawing))
					{
						t.showeditortrees = t.gamevisuals.bEndableTreeDrawing = t.visuals.bEndableTreeDrawing;
						if (t.visuals.bEndableTreeDrawing)
						{
							ggtrees_global_params.draw_enabled = 1;
						}
						else
						{
							ggtrees_global_params.draw_enabled = 0;
						}
					}
					
					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(fButtonSizeX*0.5, ImGui::GetCursorPosY()));
					if (ImGui::Checkbox("Vegetation", &t.visuals.bEndableGrassDrawing))
					{
						t.showeditorveg = t.gamevisuals.bEndableGrassDrawing = t.visuals.bEndableGrassDrawing;
					}
					if (t.visuals.bEndableGrassDrawing)
					{
						if (bFirstTimeVeg)
						{
							//PE: We need grass everywhere for this to work.
							if (gggrass_global_params.paint_type == 0)
								gggrass_global_params.paint_type = 1;
							GGGrass::GGGrass_AddAll();
							bFirstTimeVeg = false;
						}
						gggrass_global_params.draw_enabled = 1;
					}
					else
					{
						gggrass_global_params.draw_enabled = 0;
					}
					ImGui::Indent(-10);
				}

				if (ImGui::StyleCollapsingHeader("Import Heightmap", ImGuiTreeNodeFlags_DefaultOpen))
				{
					float fButtonSizeX = (ImGui::GetContentRegionAvailWidth() - 20.0f);

						ImGui::PushID(11116);
						extern void ControlAdvancedSetting(int&, const char*, bool* = nullptr);
						ControlAdvancedSetting(pref.iEnableTerrainHeightmaps, "Heightmap Settings");
						ImGui::PopID();

						//PE: Heightmaps.
						if (pref.iEnableTerrainHeightmaps)
						{
							ImGui::Indent(10);
							ImGui::TextCenter("Heightmap Scale");
							float numericboxwidth = 60.0f;
							ImGui::PushItemWidth(-10 - 10 - numericboxwidth);
							if (ImGui::SliderFloat("##UI2fheightmap_scaleslider", &ggterrain_global_params.heightmap_scale, 0.01, 10.0f, "%.2f", 2.0f))
							{
							bTriggerStableY = true;
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Heightmap Scale");
							ImGui::PopItemWidth();
							ImGui::SameLine();
							ImGui::PushItemWidth(numericboxwidth);
							if (ImGui::InputFloat("##UI2fheightmap_scaleText", &ggterrain_global_params.heightmap_scale, 0, 0, "%.2f"))
							{
							bTriggerStableY = true;
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Heightmap Scale");
							ImGui::PopItemWidth();


							ImGui::TextCenter("Heightmap File Format");

							static int iRawFormat = 0;

							//PE: Graphics program will normally export to small endian, so activate again.
							ImGui::RadioButton("Big Endian", &iRawFormat, 0);
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("Format Big Endian");
							ImGui::SameLine();
							ImGui::RadioButton("Little Endian", &iRawFormat, 1);
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("Format Little Endian");

							static bool bGenerateOutsideHeightmap = false;
							if (ImGui::Checkbox("Generate Terrain Outside Heightmap", &bGenerateOutsideHeightmap))
							{
								GGTerrain_SetGenerateTerrainOutsideHeightMap(bGenerateOutsideHeightmap);
								bTriggerStableY = true;
							}

							if (!bGenerateOutsideHeightmap)
							{
								ImGui::TextCenter("Height Value Outside Heightmap");
								ImGui::PushItemWidth(-10 - 10 - numericboxwidth);
								if (ImGui::SliderFloat("##UI2fheight_outside_heightmapslider", &ggterrain_global_params.height_outside_heightmap, -2000.0, 10000.0f, "%.2f", 2.0f))
								{
								}
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Height Value Outside Heightmap");
								ImGui::PopItemWidth();
								ImGui::SameLine();
								ImGui::PushItemWidth(numericboxwidth);
								if (ImGui::InputFloat("##UI2fheight_outside_heightmapText", &ggterrain_global_params.height_outside_heightmap, 0, 0, "%.2f"))
								{
								}
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Height Value Outside Heightmap");
								ImGui::PopItemWidth();
							}

							ImGui::TextCenter("Height Map Fade Distance");
							ImGui::PushItemWidth(-10 - 10 - numericboxwidth);
							if (ImGui::SliderFloat("##UI2fheight_outside_fadeslider", &ggterrain_global_params.fade_outside_heightmap, 0, 1000.0f, "%.2f", 2.0f))
							{
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Height Map Fade Distance");
							ImGui::PopItemWidth();
							ImGui::SameLine();
							ImGui::PushItemWidth(numericboxwidth);
							if (ImGui::InputFloat("##UI2fheight_outside_fadeText", &ggterrain_global_params.fade_outside_heightmap, 0, 0, "%.2f"))
							{
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Height Map Fade Distance");
							ImGui::PopItemWidth();

							//PE: Note - Should be able to import 16bit and 32bit ? and or also 32bit floats ?
							static int mapwidth = 4096, mapheight = 4096;
							const char* items_mapsize[] = { "1024x1024", "2048x2048", "4096x4096" ,"Custom" }; //PE: ,"Custom" later.
							static int item_current_mapsize = 2;

							ImGui::TextCenter("Heightmap Raw Import Size");

							ImGui::PushItemWidth(-10);
							if (ImGui::Combo("##HeightmapRawImportSize", &item_current_mapsize, items_mapsize, IM_ARRAYSIZE(items_mapsize)))
							{
								if (item_current_mapsize == 0)
								{
									mapwidth = mapheight = 1024;
								}
								else if (item_current_mapsize == 1)
								{
									mapwidth = mapheight = 2048;
								}
								else if (item_current_mapsize == 2)
								{
									mapwidth = mapheight = 4096;
								}
							}
							ImGui::PopItemWidth();
							float fContentWidth = ImGui::GetContentRegionAvailWidth();
							if (item_current_mapsize == 3)
							{
								ImGui::PushItemWidth(fContentWidth * 0.25 - 10.0);
								ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 3.0f));
								ImGui::Text("Width ");
								ImGui::SameLine();
								ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, -3.0f));
								if (ImGui::InputInt("##UI2imapwidthText", &mapwidth, 0, 0, 0))
								{

								}
								ImGui::PopItemWidth();
								ImGui::SameLine();
								ImGui::SetCursorPos(ImVec2(fContentWidth*0.5 - 10.0, ImGui::GetCursorPosY()));
								ImGui::Text("Height ");
								ImGui::SameLine();
								ImGui::PushItemWidth(fContentWidth * 0.25 - 10.0);
								if (ImGui::InputInt("##UI2imapheightText", &mapheight, 0, 0, 0))
								{

								}
								ImGui::PopItemWidth();
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Heightmap Raw Import Size");

							if (ImGui::StyleButton("Choose Heightmap Raw File", ImVec2(fButtonSizeX, 0.0f)))
							{
								cStr tOldDir = GetDir();
								char * cFileSelected;
								cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "All\0*.*\0Raw\0*.raw\0Dat\0*.dat\0", g.mysystem.mapbankAbs_s.Get(), NULL);
								SetDir(tOldDir.Get());
								if (cFileSelected && strlen(cFileSelected) > 0)
								{
									bTriggerStableY = true;
									cstr import_filename = cFileSelected;
									bool bImage = false;
									if (import_filename.Len() > 4)
									{
										if (cstr(Lower(Right(import_filename.Get(), 4))) == ".jpg") bImage = true;
										if (cstr(Lower(Right(import_filename.Get(), 4))) == ".png") bImage = true;
										if (cstr(Lower(Right(import_filename.Get(), 4))) == "jpeg") bImage = true;
									}

									if (bImage)
									{
										int heightmapSizeX = 0, heightmapSizeY = 0, heightmapChannels = 0;
										uint8_t* heightmapImageData = stbi_load(import_filename.Get(), &heightmapSizeX, &heightmapSizeY, &heightmapChannels, 4);

										int iDescIndex = 0;
										if (heightmapImageData && heightmapSizeX >= 64 && heightmapSizeY >= 64 && heightmapSizeX <= 4096 && heightmapSizeY <= 4096)
										{
											uint16_t *tmpdata = new uint16_t[heightmapSizeX * heightmapSizeY];
											if (tmpdata)
											{
												for (int y = 0; y < heightmapSizeY; y++)
												{
													for (int x = 0; x < heightmapSizeX; x++)
													{
														int index = y * heightmapSizeX + x;
														int iValue = heightmapImageData[4 * index + 0] + heightmapImageData[4 * index + 1] + heightmapImageData[4 * index + 2];
														if (x > 0 && y > 0 && x < heightmapSizeX - 1 && y < heightmapSizeY - 1)
														{
															//Some blur needed.
															int index1 = (y + 1) * heightmapSizeX + (x + 1);
															iValue += heightmapImageData[4 * index + 0] + heightmapImageData[4 * index + 1] + heightmapImageData[4 * index + 2];
															index1 = y * heightmapSizeX + (x + 1);
															iValue += heightmapImageData[4 * index + 0] + heightmapImageData[4 * index + 1] + heightmapImageData[4 * index + 2];
															index1 = (y + 1) * heightmapSizeX + x;
															iValue += heightmapImageData[4 * index + 0] + heightmapImageData[4 * index + 1] + heightmapImageData[4 * index + 2];

															index1 = (y - 1) * heightmapSizeX + (x - 1);
															iValue += heightmapImageData[4 * index + 0] + heightmapImageData[4 * index + 1] + heightmapImageData[4 * index + 2];
															index1 = y * heightmapSizeX + (x - 1);
															iValue += heightmapImageData[4 * index + 0] + heightmapImageData[4 * index + 1] + heightmapImageData[4 * index + 2];
															index1 = (y - 1) * heightmapSizeX - x;
															iValue += heightmapImageData[4 * index + 0] + heightmapImageData[4 * index + 1] + heightmapImageData[4 * index + 2];

															iValue *= 0.2;
														}
														tmpdata[index] = iValue;

													}
												}
												GGTerrain_SetHeightMap(tmpdata, heightmapSizeX, heightmapSizeY, false);
											}
											if (tmpdata) delete[] tmpdata;

										}

										if (heightmapImageData) delete[] heightmapImageData;
									}
									else
									{
										GGTerrain_SetGenerateTerrainOutsideHeightMap(bGenerateOutsideHeightmap);
										int iRet = 0;
										if (iRawFormat == 0)
											iRet = GGTerrain_LoadHeightMap(import_filename.Get(), mapwidth, mapheight, true);
										else
											iRet = GGTerrain_LoadHeightMap(import_filename.Get(), mapwidth, mapheight, false);
										bool bValid = false;
										if (iRet == 0)
										{
											//PE: If fail try finding the dimensions.
											//The width and height can be calculated from the file size by taking the size in bytes, dividing by 2 and then square rooting

											DWORD filesize = 0;
											HANDLE hfile = GG_CreateFile(import_filename.Get(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
											if (hfile != INVALID_HANDLE_VALUE)
											{
												filesize = GetFileSize(hfile, NULL);
												CloseHandle(hfile);
											}

											if (filesize > 0)
											{
												int iSize = sqrt(filesize*0.5);
												if (iRawFormat == 0)
													iRet = GGTerrain_LoadHeightMap(import_filename.Get(), iSize, iSize, true);
												else
													iRet = GGTerrain_LoadHeightMap(import_filename.Get(), iSize, iSize, false);

												if (iRet != 0)
												{
													item_current_mapsize = 3;
													mapwidth = mapheight = iSize;
												}
											}
											if (iRet == 0)
											{
												//PE: The heightmap selected was not in the correct format.
												MessageBoxA(NULL, "The heightmap selected was not in the correct format.", "Error", 0);
												//PE: Switch to custom raw date size if thats the problem.
												item_current_mapsize = 3;
											}
											else
											{
												bValid = true;
											}
										}
										else
										{
											bValid = true;
										}
										if (bValid)
										{
											if (pestrcasestr(import_filename.Get(), "grandcanyon.raw"))
											{
												ggterrain_global_params.noise_power = 1.0f;
												ggterrain_global_params.noise_fallof_power = 0.0f;
												ggterrain_global_params.fractal_initial_freq = 0.362f;
												ggterrain_global_params.fractal_levels = 7;
												ggterrain_global_params.fractal_freq_increase = 2.5f;
												ggterrain_global_params.fractal_freq_weight = 0.4f;
												ggterrain_global_params.heightmap_roughness = 0.5f;
												ggterrain_global_params.height = GGTerrain_MetersToUnits(2290.0f);
												ggterrain_global_params.offset_y = GGTerrain_MetersToUnits(-55.0f);
												ggterrain_global_params.heightmap_scale = 0.125f;
												ggterrain_global_render_params.baseLayerMaterial = 0x100 | 30;
												ggterrain_global_render_params.layerMatIndex[0] = 0x100 | 30;
												ggterrain_global_render_params.layerMatIndex[1] = 0x100 | 14;
												ggterrain_global_render_params.layerMatIndex[2] = 0x100 | 15;
												ggterrain_global_render_params.slopeMatIndex[0] = 0x100 | 16;
												ggterrain_global_render_params.layerStartHeight[0] = GGTerrain_MetersToUnits(0);
												ggterrain_global_render_params.layerStartHeight[1] = GGTerrain_MetersToUnits(7.6f);
												ggterrain_global_render_params.layerStartHeight[2] = GGTerrain_MetersToUnits(1500);
												ggterrain_global_render_params.layerStartHeight[3] = GGTerrain_MetersToUnits(10000);
												ggterrain_global_render_params.layerStartHeight[4] = GGTerrain_MetersToUnits(10000);
												ggterrain_global_render_params.layerEndHeight[0] = GGTerrain_MetersToUnits(2.5f);
												ggterrain_global_render_params.layerEndHeight[1] = GGTerrain_MetersToUnits(68.24f);
												ggterrain_global_render_params.layerEndHeight[2] = GGTerrain_MetersToUnits(1800);
												ggterrain_global_render_params.layerEndHeight[3] = GGTerrain_MetersToUnits(10000);
												ggterrain_global_render_params.layerEndHeight[4] = GGTerrain_MetersToUnits(10000);
												ggterrain_global_render_params.slopeStart[0] = 0.2f;
												ggterrain_global_render_params.slopeStart[1] = 1.0f;
												ggterrain_global_render_params.slopeEnd[0] = 0.4f;
												ggterrain_global_render_params.slopeEnd[1] = 1.0f;
											}
											if (pestrcasestr(import_filename.Get(), "snowden.raw"))
											{
												ggterrain_global_params.noise_power = 1.0f;
												ggterrain_global_params.noise_fallof_power = 0.0f;
												ggterrain_global_params.fractal_initial_freq = 0.274f;
												ggterrain_global_params.fractal_levels = 7;
												ggterrain_global_params.fractal_freq_increase = 2.5f;
												ggterrain_global_params.fractal_freq_weight = 0.4f;
												ggterrain_global_params.heightmap_roughness = 1.0f;
												ggterrain_global_params.heightmap_scale = 0.06f;
												ggterrain_global_params.height = GGTerrain_MetersToUnits(1000.0f);
												ggterrain_global_params.offset_y = GGTerrain_MetersToUnits(-60.0f);
												ggterrain_global_render_params.baseLayerMaterial = 0x100 | 17;
												ggterrain_global_render_params.layerMatIndex[0] = 0x100 | 2;
												ggterrain_global_render_params.layerMatIndex[1] = 0x100 | 19;
												ggterrain_global_render_params.layerMatIndex[2] = 0x100 | 0;
												ggterrain_global_render_params.slopeMatIndex[0] = 0x100 | 4;
												ggterrain_global_render_params.layerStartHeight[0] = GGTerrain_MetersToUnits(0);
												ggterrain_global_render_params.layerStartHeight[1] = GGTerrain_MetersToUnits(28.5f);
												ggterrain_global_render_params.layerStartHeight[2] = GGTerrain_MetersToUnits(10000);
												ggterrain_global_render_params.layerStartHeight[3] = GGTerrain_MetersToUnits(10000);
												ggterrain_global_render_params.layerStartHeight[4] = GGTerrain_MetersToUnits(10000);
												ggterrain_global_render_params.layerEndHeight[0] = GGTerrain_MetersToUnits(2.5f);
												ggterrain_global_render_params.layerEndHeight[1] = GGTerrain_MetersToUnits(89.9f);
												ggterrain_global_render_params.layerEndHeight[2] = GGTerrain_MetersToUnits(10000);
												ggterrain_global_render_params.layerEndHeight[3] = GGTerrain_MetersToUnits(10000);
												ggterrain_global_render_params.layerEndHeight[4] = GGTerrain_MetersToUnits(10000);
												ggterrain_global_render_params.slopeStart[0] = 0.2f;
												ggterrain_global_render_params.slopeStart[1] = 1.0f;
												ggterrain_global_render_params.slopeEnd[0] = 0.4f;
												ggterrain_global_render_params.slopeEnd[1] = 1.0f;
											}
										}

									}
								}
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Import Heightmap Raw File");

							if (ImGui::StyleButton("Remove Heightmap Data", ImVec2(fButtonSizeX, 0.0f)))
							{
								GGTerrain_RemoveHeightMap();
								bTriggerStableY = true;
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Remove Heightmap Data");
							ImGui::Indent(-10);
						}
					}


				//## Terrain Buttons ##
				ImGui::BeginChild("##ChildProceduralButtons", ImVec2(0, ImGui::GetFontSize() * 5.0 + 3.0), true, ImGuiWindowFlags_ForceRender | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoSavedSettings);
				ImGui::Indent(10);
				float fButtonSizeX = (ImGui::GetContentRegionAvailWidth() - 10.0f);

				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 10.0f));
				if (ImGui::StyleButton("Save Terrain Settings", ImVec2(fButtonSizeX, 0.0f)))
				{
					//PE: Make sure folder exists.
					char destination[MAX_PATH];
					strcpy(destination, "databank\\terrainsettings\\");
					GG_GetRealPath(destination, 1);
					MakeDirectory(destination);
					cstr terrainfile = "";

					t.returnstring_s = "";
					cStr tOldDir = GetDir();
					char * cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "ter\0*.ter\0", destination, NULL, true);
					SetDir(tOldDir.Get());
					if (cFileSelected && strlen(cFileSelected) > 0) 
					{
						t.returnstring_s = cFileSelected;
					}
					if (t.returnstring_s != "")
					{
						if (cstr(Lower(Right(t.returnstring_s.Get(), 4))) != ".ter")  t.returnstring_s = t.returnstring_s + ".ter";
						terrainfile = t.returnstring_s;

						bool oksave = true;
						if (FileExist(terrainfile.Get())) {
							oksave = overWriteFileBox(terrainfile.Get());
						}
						if (oksave)
						{
							//Save here.
							GGTerrainFile_SaveTerrainData(terrainfile.Get(), g.gdefaultwaterheight);
						}
					}

				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Save Terrain Settings");

				if (ImGui::StyleButton("Load Terrain Settings", ImVec2(fButtonSizeX, 0.0f)))
				{
					//PE: Make sure folder exists.
					char destination[MAX_PATH];
					strcpy(destination, "databank\\terrainsettings\\");
					GG_GetRealPath(destination, 1);
					MakeDirectory(destination);

					cStr tOldDir = GetDir();
					char * cFileSelected;
					cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "ter\0*.ter\0", destination, NULL, true);
					SetDir(tOldDir.Get());
					if (cFileSelected && strlen(cFileSelected) > 0)
					{
						t.returnstring_s = cFileSelected;
						if (t.returnstring_s != "")
						{
							if (cstr(Lower(Right(t.returnstring_s.Get(), 4))) == ".ter")
							{
								//Load settings.
								GGTerrainFile_LoadTerrainData(t.returnstring_s.Get(), true);
								bTreeGlobalInit = false;
								bTriggerStableY = true;
							}
						}
					}
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Load Terrain Settings");

				ImGui::SetWindowFontScale(1.0);
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 3.0f));
				ImGui::Indent(-10);
				ImGui::EndChild();

				//## Terrain Buttons END ##
				if (!pref.bHideTutorials)
				{
					if (ImGui::StyleCollapsingHeader("Tutorial", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Indent(10);
						char* my_combo_itemsp[] = { NULL,NULL,NULL };
						int my_combo_items = 0;
						int iVideoSection = 0;
						cstr cShowTutorial = "0501 - Terrain Generator";
						my_combo_itemsp[0] = "0501 - Terrain Generator";
						my_combo_itemsp[1] = "0502 - Terrain Height Maps";
						my_combo_items = 2;
						iVideoSection = SECTION_TERRAIN_GENERATOR;
						SmallTutorialVideo(cShowTutorial.Get(), my_combo_itemsp, my_combo_items, iVideoSection);
						float but_gadget_size = ImGui::GetFontSize()*12.0;
						float w = ImGui::GetWindowContentRegionWidth() - 10.0;
						ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
						ImGui::Indent(-10);
					}
				}

				if (fSnapShotModeCameraY > fMaxCameraY) fSnapShotModeCameraY = fMaxCameraY;

				// insert a keyboard shortcut component into panel
				UniversalKeyboardShortcut(eKST_TerrainGenerator);
			}
			else
			{
				if (ImGui::StyleCollapsingHeader("Screenshot Mode", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Indent(10);
					extern bool bEditorLight;
					if (ImGui::Checkbox("Editor Light", &bEditorLight))
					{
						WickedCall_EnableCameraLight(bEditorLight);
					}
					LPSTR pSnapshotButtonTitle = "Take Screenshot";
					if(bPopModalTakeMapSnapshot==true) pSnapshotButtonTitle = "Take Map Snapshot";
					if (ImGui::StyleButton(pSnapshotButtonTitle, ImVec2(ImGui::GetContentRegionAvailWidth() - 10.0f, 0.0f)))
					{
						//PE: Just exit, we will have the lastest screenshot available in thumbbank.
						iQuitProceduralLevel = 5;
					}
					ImGui::Indent(-10);
				}

				if (fSnapShotModeCameraY > fMaxCameraY) fSnapShotModeCameraY = fMaxCameraY;

				if (bPopModalTakeMapSnapshot == false)
				{
					// insert a keyboard shortcut component into panel
					UniversalKeyboardShortcut(eKST_ObjectLibrary);
				}
			}

			if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) 
			{
				//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
				ImGui::Text("");
				ImGui::Text("");
			}

			//When popup open , make sure we update the backbuffer all the time.
			#ifdef DIGAHOLE
			bLoopBackBuffer = false;
			bSnapShotModeUseCamera = false;
			#else
			bLoopBackBuffer = true;
			bSnapShotModeUseCamera = true;
			#endif

			if (!bPopModalOpenProceduralCameraMode)
			{
				ImGui::EndChild();
			}

			if (!bPopModalOpenProceduralCameraMode)
			{
				ImGui::SetWindowFontScale(1.4);
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0, 3.0));
				float fButtonSizeX = (ImGui::GetContentRegionAvailWidth() - 1.0f);
				//if (ImGui::StyleButton("Generate Terrain for Level", ImVec2(fButtonSizeX, 0.0f)))
				ImVec2 texthere = ImGui::GetCursorPos();
				if (ImGui::StyleButton("##Generate Terrain for Level", ImVec2(fButtonSizeX, ImGui::GetFontSize()*3.0)))
				{
					t.addentityfile_s = "_markers\\Player Start.fpe";
					if (t.addentityfile_s != "")
					{
						entity_adduniqueentity(false);
						t.tasset = t.entid;
						if (t.talreadyloaded == 0)
						{
							editor_filllibrary();
						}
					}

					int masterobj = g.entitybankoffset + t.entid;

					// duplicate new entity as clone of relevant original clipboard entity
					bool bLowestFound = false;
					t.gridentity = t.entid;

					//PE: all t.gridentity... need to be set for this to work correctly.
					entity_fillgrideleproffromprofile();  // t.entid
					t.gridentityposx_f = 0;
					t.gridentityposy_f = BT_GetGroundHeight(t.terrain.TerrainID, 0, 0);
					t.gridentityposz_f = 0;

					if (t.gridentityposy_f < g.gdefaultwaterheight)
					{
						// Position the start marker above water, in case the following search for land is unsuccessful.
						t.gridentityposy_f = g.gdefaultwaterheight;
						t.editorfreeflight.c.y_f = t.gridentityposy_f;
						t.editorfreeflight.s = t.editorfreeflight.c;
						PositionCamera(0, t.gridentityposy_f, 0);

						float fNewYPos = t.gridentityposy_f;

						const int iPointsOnCircle = 12;
						const float fSearchRadius = 2000; // inches
						const int iSearchesToAttempt = GGTerrain::GGTerrain_GetEditableSize() / fSearchRadius;

						// Get angles for points on a circle on the terrain.
						float fAngleIncrement = 2.0f * 3.14159265359f / iPointsOnCircle; // radians

						bool bLandFound = false;

						// Go around the circle with fSearchRadius, gradually make the circle bigger until land is found, or we run out of room.
						for (int j = 0; j < iSearchesToAttempt; j++)
						{
							for (int i = 0; i < iPointsOnCircle; i++)
							{
								float x = fSearchRadius * (j + 1) * cosf(fAngleIncrement * i);
								float z = fSearchRadius * (j + 1) * sinf(fAngleIncrement * i);

								fNewYPos = BT_GetGroundHeight(t.terrain.TerrainID, x, z);

								// Found some land, reposition the start marker.
								if (fNewYPos > g.gdefaultwaterheight)
								{
									// Do an accurate but slow height check to make sure the marker won't be placed inside the terrain.
									GGTerrain_GetHeight(x, z, &fNewYPos, 1); 
									t.gridentityposy_f = fNewYPos + 10.0f;
									t.gridentityposx_f = x;
									t.gridentityposz_f = z;
									t.editorfreeflight.c.y_f = t.gridentityposy_f;
									t.editorfreeflight.c.x_f = x;
									t.editorfreeflight.c.z_f = z;
									t.editorfreeflight.s = t.editorfreeflight.c;
									//PositionCamera(x, t.gridentityposy_f, z);
									bLandFound = true;
									newLevelCamera.set = 1;
									newLevelCamera.x = x; newLevelCamera.y = fNewYPos; newLevelCamera.z = z;
									break;
								}
							}

							if (bLandFound)
								break;
						}
					}

					t.gridentityrotatex_f = ObjectAngleX(masterobj);
					t.gridentityrotatey_f = ObjectAngleY(masterobj);
					t.gridentityrotatez_f = ObjectAngleZ(masterobj);
					t.gridentityrotatequatmode = 0;
					t.gridentityrotatequatx_f = 0;
					t.gridentityrotatequaty_f = 0;
					t.gridentityrotatequatz_f = 0;
					t.gridentityrotatequatw_f = 1;
					t.gridentityscalex_f = ObjectScaleX(masterobj);
					t.gridentityscaley_f = ObjectScaleY(masterobj);
					t.gridentityscalez_f = ObjectScaleZ(masterobj);

					gridedit_addentitytomap(); //Add it to map set t.e

					t.refreshgrideditcursor = 1;
					t.gridentity = 0;
					t.gridentityposoffground = 0;
					t.gridentityusingsoftauto = 0;
					t.gridentityautofind = 0;
					t.gridentityobj = 0;
					editor_refreshentitycursor();
					t.widget.pickedObject = 0;
					t.gridentityextractedindex = 0;

					// when choose a biome, ensure any ambient and combat music copied over to remote project
					extern bool entity_copytoremoteifnotthere(LPSTR);
					entity_copytoremoteifnotthere(t.visuals.sAmbientMusicTrack.Get());
					entity_copytoremoteifnotthere(t.visuals.sCombatMusicTrack.Get());

					//Code
					iQuitProceduralLevel = 5;
					bTreeGlobalInit = false;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Generates the terrain for your level and takes you to the Level Editor");

				ImVec2 restorehere = ImGui::GetCursorPos();
				ImGui::SetCursorPos(texthere+ImVec2(0.0,8.0));
				ImGui::TextCenter("Generate Terrain and");
				ImGui::TextCenter("Open the Level Editor");
				ImGui::SetWindowFontScale(1.0);
			}
			ImGui::Columns(1);

			//Render titlebar centered.
			ImVec2 titlebar_pos;
			cstr title;
			if (!bUseNoTitleBar)
			{
				if (bPopModalOpenProceduralCameraMode==false)
				{
					title = "Procedural Level Generator";
				}
				else
				{
					title = "Screenshot mode";
				}
				float fTextSize = ImGui::CalcTextSize(title.Get()).x;
				float xcenter = (ImGui::GetWindowSize().x*0.5) - (fTextSize*0.5);
				titlebar_pos = ImGui::GetWindowPos() + ImVec2(xcenter, 4);
			}

			//PE:Add help window here, corner.
			if (bPopModalOpenProceduralCameraMode==false && bPopModalTakeMapSnapshot == false)
			{
				if (pref.iDisplayTerrainGeneratorWelcome)
				{
					//PE: Render only window, so dont loose focus and go behind other windows.
					float zoomwindow = 1.2;
					float winheight = 250 * zoomwindow;
					float winwidth = 214 * zoomwindow;
					float margin = 5.0 * zoomwindow;
					ImGuiWindow* window = ImGui::GetCurrentWindow();
					extern ImFont* customfont;
					if (window->DrawList && customfont)
					{
						window->DrawList->AddCallback((ImDrawCallback)10, NULL); //force render.

						ImVec4 style_winback = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
						style_winback.w = 0.5f;

						float yspacer = 21.0 * zoomwindow;
						ImVec2 window_pos = ImVec2((ImGui::GetWindowPos().x + 10.0), (ImGui::GetWindowPos().y+ImGui::GetWindowSize().y) - winheight);
						window->DrawList->AddRectFilled(window_pos, window_pos + ImVec2(winwidth, winheight-10.0), ImGui::GetColorU32(style_winback), 6.0f, 15);
						window->DrawList->AddRectFilled(window_pos, window_pos + ImVec2(winwidth, 22 * zoomwindow), ImGui::GetColorU32(style_winback), 6.0f, 15);
						window->DrawList->AddText(customfont, 15 * zoomwindow, ImVec2(window_pos.x + margin, window_pos.y + 3.0), IM_COL32(255, 255, 255, 255), "About this editor");
						window_pos += ImVec2(0, 3.0 * zoomwindow);
						window_pos += ImVec2(0, yspacer);
						window->DrawList->AddText(customfont, 15 * zoomwindow, ImVec2(window_pos.x + margin, window_pos.y + 3.0), IM_COL32(255, 255, 255, 255), "With this editor you can create");
						window_pos += ImVec2(0, yspacer);
						window->DrawList->AddText(customfont, 15 * zoomwindow, ImVec2(window_pos.x + margin, window_pos.y + 3.0), IM_COL32(255, 255, 255, 255), "the terrain type for your new level.");
						window_pos += ImVec2(0, yspacer);
						window->DrawList->AddText(customfont, 15 * zoomwindow, ImVec2(window_pos.x + margin, window_pos.y + 3.0), IM_COL32(255, 255, 255, 255), "The choices you can make with the");
						window_pos += ImVec2(0, yspacer);
						window->DrawList->AddText(customfont, 15 * zoomwindow, ImVec2(window_pos.x + margin, window_pos.y + 3.0), IM_COL32(255, 255, 255, 255), "right panel options include:");
						window_pos += ImVec2(0, yspacer + yspacer);
						window->DrawList->AddText(customfont, 15 * zoomwindow, ImVec2(window_pos.x + margin, window_pos.y + 3.0), IM_COL32(255, 255, 255, 255), "- The Biome style");
						window_pos += ImVec2(0, yspacer);
						window->DrawList->AddText(customfont, 15 * zoomwindow, ImVec2(window_pos.x + margin, window_pos.y + 3.0), IM_COL32(255, 255, 255, 255), "- Size of level");
						window_pos += ImVec2(0, yspacer);
						window->DrawList->AddText(customfont, 15 * zoomwindow, ImVec2(window_pos.x + margin, window_pos.y + 3.0), IM_COL32(255, 255, 255, 255), "- Trees and vegetation");
						window_pos += ImVec2(0, yspacer + yspacer);
						window->DrawList->AddText(customfont, 15 * zoomwindow, ImVec2(window_pos.x + margin, window_pos.y + 3.0), IM_COL32(255, 255, 255, 255), "Watch the tutorial for more help.");
						ImGui::SetItemAllowOverlap();
						if (ImGui::CloseButton(ImGui::GetCurrentWindow()->GetID("#ClearSearch"), ImGui::GetWindowPos() + ImVec2(winwidth - 13.0, ImGui::GetWindowSize().y - winheight - 3.0)))
						{
							pref.iDisplayTerrainGeneratorWelcome = false;
						}

						window->DrawList->AddCallback((ImDrawCallback)11, NULL); //disableforce render.
					}
				}
			}

			if (customfontlarge) ImGui::PopFont();
			ImGui::PopFont();

			if (bUseModal)
			{
				ImGui::EndPopup();
			}
			else
			{
				ImGui::End();
			}
			ImGui::PopStyleColor();

			//Render title bar after End. end fill titlebar.
			if (!bUseNoTitleBar)
			{
				ImGuiWindow* window = ImGui::GetCurrentWindow();
				ImGuiContext& g = *GImGui;
				window->DrawList->AddText(g.Font, g.FontSize, titlebar_pos, ImGui::GetColorU32(ImGuiCol_Text), title.Get());
			}
		}

		if (iQuitProceduralLevel > 0)
		{
			iQuitProceduralLevel--;
			//Make a final screenshot. without minimap ...
			ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
			ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
			ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP;
			if (ObjectExist(TERRAINGENERATOR_OBJECT)) HideObject(TERRAINGENERATOR_OBJECT);
			if (iQuitProceduralLevel == 0)
			{
				//Create Screenshot.
				bool bForce = false;
				#ifdef DIGAHOLE
				bForce = true;
				#endif
				if (BitmapExist(99) || bForce)
				{
					SetGrabImageMode(1);
					LPGGSURFACE	pTmpSurface = g_pGlob->pCurrentBitmapSurface;
					ID3D11Texture2D *pBackBuffer = NULL;
					#ifdef DIGAHOLE
					//PE: We need 5 frames (iQuitProceduralLevel). before backbuffer is updated with latest without 2d. ?
					pBackBuffer = (ID3D11Texture2D *)wiRenderer::GetDevice()->GetBackBufferForGG(&master.swapChain);
					#else
					pBackBuffer = (ID3D11Texture2D *)GetBitmapTexture2D(99);
					#endif

					g_pGlob->pCurrentBitmapSurface = pBackBuffer;
					GGSURFACE_DESC ddsd;
					pBackBuffer->GetDesc(&ddsd);
					float grabx = 1280, graby = 720;
					if (bPopModalTakeMapSnapshot == true)
					{
						// square that matches the prepared view of the top down map
						grabx = 800;
						graby = 800;
					}
					if (graby > ddsd.Height)
						graby = ddsd.Height;
					if (grabx > ddsd.Width)
						grabx = ddsd.Width;

					float imgcx = (ddsd.Width*0.5) - (grabx*0.5);
					float imgcy = (ddsd.Height*0.5) - (graby*0.5);
					if (imgcx + grabx > ddsd.Width) imgcx = ddsd.Width - grabx;
					if (imgcy < 0) imgcy = 0;
					if (imgcx < 0) imgcx = 0;
					//PE: We need a unique id for this STORYBOARD_THUMBS+400
					GrabImage(STORYBOARD_THUMBS + 400, imgcx, imgcy, imgcx + grabx, imgcy + graby, 3);
					SetGrabImageMode(0);
					g_pGlob->pCurrentBitmapSurface = pTmpSurface;
					if (ImageExist(STORYBOARD_THUMBS + 400))
					{
						char destination[MAX_PATH];
						strcpy(destination, "thumbbank\\lastnewlevel.jpg");
						GG_SetWritablesToRoot(true);
						GG_GetRealPath(destination, 1);
						GG_SetWritablesToRoot(false);
						extern bool g_bDontUseImageAlpha;
						g_bDontUseImageAlpha = true;
						SaveImage(destination, STORYBOARD_THUMBS + 400);
						g_bDontUseImageAlpha = false;
						DeleteImage(STORYBOARD_THUMBS + 400);
					}
				}
				#ifndef DIGAHOLE
				if (ObjectExist(TERRAINGENERATOR_OBJECT)) ShowObject(TERRAINGENERATOR_OBJECT);
				#endif

				if (!bPopModalOpenProceduralCameraMode)
					bTriggerTerrainSaveAsWindow = true; //Open save as window.

				//PE: Now default to object mode
				bForceKey = true;
				csForceKey = "o";
				bTerrain_Tools_Window = false;
				extern bool Entity_Tools_Window;
				Entity_Tools_Window = true;

			}
			//PE: Need "save as" here so dont actually close it down yet.
			if (bPopModalOpenProceduralCameraMode)
			{
				if (iQuitProceduralLevel == 0)
					bProceduralLevel = false;
			}

			// LB: Disagree with this, ALL LEVELS must have a name and FPM or bad things happen!
			/*
			if (!bProceduralLevelFromStoryboard)
			{
				//Started from editor , just go back.
				bProceduralLevel = false;
				iQuitProceduralLevel = 0;
				bTriggerTerrainSaveAsWindow = false;
			}
			*/
		}
		if (!bProceduralLevel)
		{
			#ifndef DIGAHOLE
			extern bool g_bNoTerrainRender;
			g_bNoTerrainRender = false;
			#endif

			#ifdef DIGAHOLE
			bDigAHoleToHWND = false;
			#endif

			//Close down modal popup.
			BackBufferSaveCacheName = "";
			BackBufferObjectID = 0;
			BackBufferImageID = 0;
			bLoopBackBuffer = false;

			//Delete backbuffer and create a new.
			if (BitmapExist(99))
			{
				DeleteBitmapEx(99);
			}

			if (ImageExist(TERRAINGENERATOR_IMAGE)) DeleteImage(TERRAINGENERATOR_IMAGE);
			if (ObjectExist(TERRAINGENERATOR_OBJECT)) DeleteObject(TERRAINGENERATOR_OBJECT);

			//Restore fog.
			t.visuals.FogNearest_f = oldFogNear;
			t.visuals.FogDistance_f = oldFogFar;
			#ifndef RANDOMSKYBOX
			t.visuals.skyindex = oldSkyIndex;
			t.visuals.bDisableSkybox = oldbDisableSkybox;
			#endif
			t.visuals.CameraFAR_f = oldCameraFAR_f;
			t.visuals.fShadowFarPlane = oldfShadowFarPlane;
			t.visuals.bFXAAEnabled = oldbFXAAEnabled;

			fWickedMaxCenterTest = 0.0f; //Normal shadow matrix.
			t.visuals.SunAngleX = oldSunAngleX;
			t.visuals.SunAngleY = oldSunAngleY;
			t.visuals.SunAngleZ = oldSunAngleZ;

			t.visuals.ZenithRed_f = oldZenithRed_f;
			t.visuals.ZenithGreen_f = oldZenithGreen_f;
			t.visuals.ZenithBlue_f = oldZenithBlue_f;

			t.visuals.FogR_f = oldFogR_f;
			t.visuals.FogG_f = oldFogG_f;
			t.visuals.FogB_f = oldFogB_f;
			t.visuals.FogA_f = oldFogA_f;

			t.visuals.SkyCloudHeight = oldSkyCloudHeight;

			Wicked_Update_Visuals((void *)&t.visuals);

			//Display cubes again if needed.
			if (t.grideditselect == 0)
			{
				WickedCall_DisplayCubes(true);
			}

			editor_toggle_element_vis(t.showeditorelements);

			iLaunchAfterSync = 80; //Update all probes visuals ... after new level is generated.
			iSkibFramesBeforeLaunch = 5;

			//Restore camera.
			if (fOldCamx != 0 || fOldCamy != 0 || fOldCamz != 0)
			{
				t.editorfreeflight.mode = iCamMode;
				t.cx_f = t.editorfreeflight.c.x_f = fOldCamx;
				t.editorfreeflight.c.y_f = fOldCamy;
				t.cy_f = t.editorfreeflight.c.z_f = fOldCamz;
				PositionCamera(t.editorfreeflight.c.x_f, t.editorfreeflight.c.y_f, t.editorfreeflight.c.z_f);
				t.editorfreeflight.c.angx_f = fOldCamAx;
				t.editorfreeflight.c.angy_f = fOldCamAy;
				RotateCamera(t.editorfreeflight.c.angx_f, t.editorfreeflight.c.angy_f, 0);

				//Place in center.
				float camx = 350.0, camz = -360.0;
				float theight = BT_GetGroundHeight(t.terrain.TerrainID, camx, camz);
				if (theight < g.gdefaultwaterheight)
					theight = g.gdefaultwaterheight;
				t.editorfreeflight.mode = 3; //move camera to.
				t.editorfreeflight.c.y_f = theight + 1050.0; //From
				t.editorfreeflight.s.y_f = theight + 350.0; //To
				t.editorfreeflight.s.x_f = camx; //To
				t.editorfreeflight.s.z_f = camz; //To
				t.editorfreeflight.c.x_f = camx;
				t.editorfreeflight.c.z_f = camz;

				t.editorfreeflight.s.angx_f = 17.0;
				t.editorfreeflight.s.angy_f = 316.0;

				PositionCamera(t.editorfreeflight.c.x_f, t.editorfreeflight.c.y_f, t.editorfreeflight.c.z_f);

				if (newLevelCamera.set)
				{
					PositionCameraForNewLevel();
				}
			}

			ggterrain_global_render_params2.flags2 = oldflags2;
			set_terrain_sculpt_mode(GGTERRAIN_SCULPT_RAISE);
		}
		else
		{
			float theight = BT_GetGroundHeight(t.terrain.TerrainID, fSnapShotModeCameraX, fSnapShotModeCameraZ);
			if (fSnapShotModeCameraY < theight + 30.0f)  fSnapShotModeCameraY = theight + 30.0f;
			//When open always set main camera to the same as the new backbuffer.
			t.cx_f = t.editorfreeflight.c.x_f = fSnapShotModeCameraX;
			t.editorfreeflight.c.y_f = fSnapShotModeCameraY;
			t.cy_f = t.editorfreeflight.c.z_f = fSnapShotModeCameraZ;
			PositionCamera(t.editorfreeflight.c.x_f, t.editorfreeflight.c.y_f, t.editorfreeflight.c.z_f);

			//int oldcammode = t.cameraviewmode;
			t.editorfreeflight.c.angx_f = fSnapShotModeCameraAngX;
			t.editorfreeflight.c.angy_f = fSnapShotModeCameraAngY;
			RotateCamera(t.editorfreeflight.c.angx_f, t.editorfreeflight.c.angy_f, 0);
		}

	}
	else
	{
		bPopModalOpenProcedural = false;
	}

	//#################################
	//#### END Procedural Preview. ####
	//#################################
}
#endif

#ifdef WICKEDENGINE
void PositionCameraForNewLevel()
{
	if (newLevelCamera.set)
	{
		t.editorfreeflight.c.y_f = newLevelCamera.y + 100;
		t.editorfreeflight.c.x_f = newLevelCamera.x;
		t.editorfreeflight.c.z_f = newLevelCamera.z;
		t.editorfreeflight.c.angx_f = t.gridentityrotatex_f + 5.0f;
		t.editorfreeflight.c.angy_f = t.gridentityrotatey_f;
		t.editorfreeflight.s = t.editorfreeflight.c;

		newLevelCamera.set = 0;
	}
}
#endif

#ifdef CUSTOMTEXTURES
void ChooseTerrainTextureFolder(char* folder)
{
	cstr oldDir = GetDir();

	char writePath[MAX_PATH];

	extern const char* GG_GetWritePath();
	strcpy(writePath, GG_GetWritePath());
	
	// Determine default path to the writable folder textures
	char writableTextures[MAX_PATH];
	strcpy(writableTextures, writePath);
	strcat(writableTextures, "Files\\terraintextures\\");
	if (PathExist(writableTextures) == 0)
	{
		MakeDirectory(writableTextures);
	}

	char destination[MAX_PATH];
	bool bInMaxFolder = false;
	
	// User chooses new texture folder
	if (folder == nullptr)
	{
		// File dialog to choose texture directory
		char* cNewDirectory = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_DIR, "All\0*.*\0", writableTextures, NULL, true, "Choose a folder to take textures from");
		if (cNewDirectory && strlen(cNewDirectory) > 0)
		{
			// Check if chosen new location for textures is already in Max installation, or writable folder
			bool bInWritableFolder = false;
			if (strstr(cNewDirectory, writableTextures))
			{
				bInWritableFolder = true;
			}
			if (!bInWritableFolder)
			{
				char maxTextures[MAX_PATH];
				strcpy(maxTextures, g.fpscrootdir_s.Get());
				strcat(maxTextures, "\\Files\\");
				if (strstr(cNewDirectory, maxTextures))
				{
					bInMaxFolder = true;
				}
			}

			if (!bInWritableFolder && !bInMaxFolder)
			{
				// Chosen textures are not in Max install or writable, so we need to copy them to the writable folder
				std::vector<std::string> filesToCopy;
				CollectFilesWithExtension(".png", cNewDirectory, &filesToCopy);
				if (filesToCopy.empty())
				{
					extern bool bTriggerMessage;
					extern char cTriggerMessage[MAX_PATH];
					bTriggerMessage = true;
					strcpy(cTriggerMessage, "Could not find any PNG textures");
					ResetTextureSettings();
					return;
				}
				char folder[MAX_PATH];
				for (int i = strlen(cNewDirectory) - 1; i >= 0; i--)
				{
					if (cNewDirectory[i] == '/' || cNewDirectory[i] == '\\')
					{
						strcpy(folder, cNewDirectory + i + 1);
						strcat(folder, "\\");
						break;
					}
				}
				// Create the destination directory and copy the files into the writable folder
				strcpy(destination, writableTextures);
				strcat(destination, folder);
				int sourceDirectoryLength = strlen(cNewDirectory);
				char lastChar = cNewDirectory[sourceDirectoryLength - 1];
				if (lastChar != '/' && lastChar != '\\')
				{
					sourceDirectoryLength += 1;
				}
				if (PathExist(destination) == 0)
				{
					MakeDirectory(destination);
				}
				for (auto& file : filesToCopy)
				{
					// Extract filename only from the source file and copy to new location
					std::string newFilename = destination;
					const char* filePath = file.c_str() + sourceDirectoryLength;
					//char* filePath = strstr((char*)file.c_str(), folder);
					if (filePath)
					{
						// Check if the file is contained within another folder (will need to create the directory in the writable folder)
						char subfolder[MAX_PATH];
						/*strcpy(subfolder, filePath + folderLength);*/
						strcpy(subfolder, filePath);
						bool bIsFolder = false;
						for (int i = strlen(subfolder) - 1; i >= 0; i--)
						{
							if (subfolder[i] == '/' || subfolder[i] == '\\')
							{
								subfolder[i + 1] = 0;
								bIsFolder = true;
								break;
							}
						}
						if (bIsFolder)
						{
							char pathToSubfolder[MAX_PATH];
							strcpy(pathToSubfolder, destination);
							strcat(pathToSubfolder, subfolder);
							if (PathExist(pathToSubfolder) == 0)
							{
								MakeDirectory(pathToSubfolder);
							}
						}
						newFilename.append(filePath);
					}
					// Perform the copy
					CopyAFile((char*)file.c_str(), (char*)newFilename.c_str());
				}
			}
			else
			{
				// Already in max installation or writable, so no need to copy any files
				strcpy(destination, cNewDirectory);
				strcat(destination, "\\");
			}

			SetDir(oldDir.Get());

			// Change textures location to the new folder chosen by the user
			char cMaxPath[MAX_PATH];
			strcpy(cMaxPath, g.fpscrootdir_s.Get());
			strcat(cMaxPath, "\\Files\\");
			char* inMaxPath = strstr(destination, cMaxPath);
			if (inMaxPath)
			{
				t.visuals.customTexturesFolder = destination + strlen(cMaxPath);
			}
			else
			{
				char* inWritablePath = strstr(destination, writePath);
				if (inWritablePath)
				{
					t.visuals.customTexturesFolder = destination + strlen(writePath);
				}
				else
				{
					t.visuals.customTexturesFolder = destination;
				}
			}
		}
		else
		{
			return;
		}
	}
	else
	{
		// Determine full path to custom textures folder (writable or max install)
		char fullPath[MAX_PATH];
		strcpy(fullPath, writePath);
		strcat(fullPath, folder);
		if (PathExist(fullPath) == 0)
		{
			fullPath[0] = 0;
			strcpy(fullPath, g.fpscrootdir_s.Get());
			strcat(fullPath, "\\");
			strcat(fullPath, folder);
			if (PathExist(fullPath) == 0)
			{
				extern bool bTriggerMessage;
				extern char cTriggerMessage[MAX_PATH];
				bTriggerMessage = true;
				strcpy(cTriggerMessage, "Could not load terrain materials. Reverting to default settings.");
				ResetTextureSettings();
				SetDir(oldDir.Get());
				return;
			}
		}
		// Loading texture folder
		t.visuals.customTexturesFolder = folder;
		strcpy(destination, fullPath);
	}

	std::vector<std::string> files;
	CollectFilesWithExtension("color.dds", destination, &files);
	bool bNeedToConvertPNGs = false;
	if (files.empty())
	{
		CollectFilesWithExtension("color.png", destination, &files);
		bNeedToConvertPNGs = true;
	}

	char newDir[MAX_PATH];
	strcpy(newDir, destination);
	if (bInMaxFolder)
	{
		newDir[strlen(newDir) - strlen(t.visuals.customTexturesFolder.Get())] = 0;
	}
	else
	{
		newDir[strlen(newDir) - strlen(t.visuals.customTexturesFolder.Get()) + strlen("Files\\")] = 0;
	}

	char* localPath = strstr(destination, "Files\\");
	if (localPath)
	{
		int pathOffset = strlen(destination) - strlen(localPath) + strlen("Files\\");
		for (int i = 0; i < 32; i++)
		{
			// Delete existing image
			sTerrainTexturesID[i] = t.terrain.imagestartindex + 80 + i;
			if (ImageExist(t.terrain.imagestartindex + 80 + i))
			{
				DeleteImage(t.terrain.imagestartindex + 80 + i);
			}

			// Set new custom terrain textures
			if (files.size() > i)
			{
				t.visuals.sTerrainTextures[i] = files[i].c_str() + pathOffset;
				sTerrainSelectionID[i] = i;
			}
			else
			{
				if (i == files.size())
				{
					t.visuals.sTerrainTextures[i] = g.fpscrootdir_s.Get();
					strcat(t.visuals.sTerrainTextures[i].Get(), "\\Files\\");
					strcat(t.visuals.sTerrainTextures[i].Get(), "terraintextures\\mat32\\Color.dds");
					sTerrainSelectionID[i] = i;
				}
				else
				{
					t.visuals.sTerrainTextures[i] = "";
				}
			}
		}
	}

	// new path for custom terrain textures
	SetDir(newDir);
	g_DeferTextureUpdateCurrentFolder_s = newDir;

	// For now, load any color textures
	g_DeferTextureUpdate.clear();
	g_DeferTextureUpdate.reserve(32);
	for (int i = 0; i < 32; i++)
	{
		std::string texture = t.visuals.sTerrainTextures[i].Get();
		g_DeferTextureUpdate.push_back(texture);
	}
	if (bNeedToConvertPNGs)
	{
		TerrainConvertPNGFiles(&g_DeferTextureUpdate);
	}

	// Ensure terrain textures are stored as dds
	for (int i = 0; i < 32; i++)
	{
		char textureName[MAX_PATH];
		strcpy(textureName, t.visuals.sTerrainTextures[i].Get());
		if (strcmp(textureName + strlen(textureName) - 4, ".png") == 0)
		{
			textureName[strlen(textureName) - 4] = 0;
			strcat(textureName, ".dds");
			t.visuals.sTerrainTextures[i] = textureName;
		}
		strcpy(textureName, g_DeferTextureUpdate[i].c_str());
		if (strcmp(textureName + strlen(textureName) - 4, ".png") == 0)
		{
			textureName[strlen(textureName) - 4] = 0;
			strcat(textureName, ".dds");
			g_DeferTextureUpdate[i] = textureName;
		}
	}
	
	// trigger an update of the terrain textures when the time is right
	g_iDeferTextureUpdateToNow = 2;

	// restore before leave
	SetDir(oldDir.Get());
}

#include "Nlohmann JSON/json.hpp"
void SaveTerrainTextureFolder(LPSTR pFile)
{
	nlohmann::json j;
	j["TexturesFolder"] = t.visuals.customTexturesFolder.Get();
	j["MaterialSounds"] = g_iCustomTerrainMatSounds;
	std::string content = j.dump(4);

	if (FileExist(pFile) == 1)
	{
		DeleteAFile(pFile);
	}
	if (FileOpen(1) == 1) 
	{ 
		CloseFile(1); 
	}

	std::ofstream outputStream;
	outputStream.open(pFile);
	if (outputStream.is_open())
	{
		outputStream << content;
		outputStream.close();
	}
}

void LoadTerrainTextureFolder(LPSTR pFile)
{
	// if file exists, load it
	if (FileExist(pFile) == 1)
	{
		std::ifstream inputStream;
		inputStream.open(pFile);
		if (inputStream.is_open())
		{
			nlohmann::json j;
			inputStream >> j;
			inputStream.close();
			if (j.contains("TexturesFolder"))
			{
				std::string folder = j.at("TexturesFolder");
				t.visuals.customTexturesFolder = folder.c_str();
				if (folder.length() > 0)
				{
					ChooseTerrainTextureFolder(t.visuals.customTexturesFolder.Get());
				}
				else
				{
					// Default choice of textures
					//GGTerrain::GGTerrain_ReloadTextures();
					g_iDeferTextureUpdateToNow = 1;
				}
			}
			if (j.contains("MaterialSounds"))
			{
				j.at("MaterialSounds").get_to(g_iCustomTerrainMatSounds);
			}
		}
	}
}

#include "PNGToDDSTypes.h"
void TerrainConvertPNGFiles(std::vector<std::string>* colorFiles)
{
	extern int ConvertMaterialToDDSFromPNG(MaterialToConvert*);
	if (colorFiles)
	{
		for (auto& file : *colorFiles)
		{
			if (file.length() > 4)
			{
				char texName[MAX_PATH];
				strcpy(texName, file.c_str());
				MaterialToConvert mat;
				mat.color[0] = file;
				texName[strlen(texName) - 4] = 0;
				strcat(texName, ".dds");
				mat.color[1] = texName;
				texName[strlen(texName) - strlen("color.dds")] = 0;
				strcat(texName, "normal.png");
				mat.normal[0] = texName;
				texName[strlen(texName) - 4] = 0;
				strcat(texName, ".dds");
				mat.normal[1] = texName;
				texName[strlen(texName) - strlen("normal.dds")] = 0;
				strcat(texName, "ao.png");
				mat.ao = texName;
				texName[strlen(texName) - strlen("ao.png")] = 0;
				strcat(texName, "roughness.png");
				mat.roughness = texName;
				texName[strlen(texName) - strlen("roughness.png")] = 0;
				strcat(texName, "metalness.png");
				mat.metalness = texName;
				texName[strlen(texName) - strlen("metalness.png")] = 0;
				strcat(texName, "surface.png");
				mat.surface[0] = texName;
				texName[strlen(texName) - 4] = 0;
				strcat(texName, ".dds");
				mat.surface[1] = texName;
				ConvertMaterialToDDSFromPNG(&mat);
			}
		}
	}
}

void ResetTextureSettings()
{
	t.visuals.customTexturesFolder = "";
	for (int i = 0; i < 32; i++)
	{
		// Delete existing images
		sTerrainTexturesID[i] = t.terrain.imagestartindex + 80 + i;
		if (ImageExist(t.terrain.imagestartindex + 80 + i))
		{
			DeleteImage(t.terrain.imagestartindex + 80 + i);
		}
		t.visuals.sTerrainTextures[i] = "";
	}
	//cstr oldDir = GetDir();
	//SetDir(g.fpscrootdir_s.Get());
	//GGTerrain::GGTerrain_ReloadTextures();
	//SetDir(oldDir.Get());
	g_iDeferTextureUpdateToNow = 1;
	// Trigger update of material sounds
	extern bool g_bMapMatIDToMatIndexAvailable;
	g_bMapMatIDToMatIndexAvailable = false;
}
#endif



