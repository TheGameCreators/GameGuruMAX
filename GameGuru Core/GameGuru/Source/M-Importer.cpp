//----------------------------------------------------
//--- GAMEGURU - M-Importer
//----------------------------------------------------

// Includes
#include "stdafx.h"
#include "gameguru.h"

#ifdef ENABLEIMGUI
//PE: GameGuru IMGUI.
#include "..\Imgui\imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "..\Imgui\imgui_internal.h"
#include "..\Imgui\imgui_impl_win32.h"
#include "..\Imgui\imgui_gg_dx11.h"
#endif

#ifdef WICKEDENGINE
#define IMPORTER_TMP_IMAGE (g.importermenuimageoffset+48)
#else
#define IMPORTER_TMP_IMAGE (g.importermenuimageoffset+50)
#endif


// Enums
enum MaterialComponentTEXTURESLOT
{
	BASECOLORMAP,
	NORMALMAP,
	SURFACEMAP,
	EMISSIVEMAP,
	DISPLACEMENTMAP,
	OCCLUSIONMAP,
	TRANSMISSIONMAP,
	SHEENCOLORMAP,
	SHEENROUGHNESSMAP,
	CLEARCOATMAP,
	CLEARCOATROUGHNESSMAP,
	CLEARCOATNORMALMAP,
	TEXTURESLOT_COUNT
};

// Globals
bool g_bLoadedFBXModel = false;
int g_iFirstTimeFBXImport = 0;
int g_iPreferPBR = 0;
int g_iPreferPBRLateShaderChange = 0;
bool g_bCameraInSkyForImporter = false;
char g_pShowFilenameHoveringOver[1024];
DWORD g_dwMapImageListIndexToLimbCount = 0;
int* g_piMapImageListIndexToLimbIndex = NULL;

#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
extern bool bImporter_Window;
extern bool bCenterRenderView;
int iDelayedExecute = 0;
int iDelayedExecuteSelection = 0;
int iDelayedExecuteChannel = -1;
int iImporterScale = 100;
float fImporterScaleMultiply = 1.0;
int iImporterGenerateThumb = 0;
extern float custom_back_color[4];
//char cImportName[MAX_PATH];
char cImportName[128];
char cImportPath[MAX_PATH] = "entitybank\\user\\";
char cImportPathCropped[MAX_PATH] = "\\user";
extern bool bTriggerMessage;
extern char cTriggerMessage[MAX_PATH];
float fImportRotX = 0.0, fImportRotY = 0.0, fImportRotZ = 0.0;
float fImportPosX = 0.0, fImportPosY = 0.0, fImportPosZ = 0.0;
float fImportModelBaseX = 0.0, fImportModelBaseY = 0.0, fImportModelBaseZ = 0.0;
bool bFindFloor = false;
bool bUseRGBAButtons = false;
#endif

#ifndef PRODUCTCLASSIC
extern bool bBoostIconColors;
#endif

#ifdef VRTECH
bool bRemoveSprites = true;
#else
bool bRemoveSprites = false;
#endif

#ifdef WICKEDENGINE
extern bool bImGuiGotFocus;
wiScene::MaterialComponent* pSelectedMaterial = NULL;
sMesh * pSelectedMesh = NULL;
char scaling_combo_entry[256] = "Original Scaling\0"; //PE: Changed "Automatic Scaling\0";
char collision_combo_entry[256] = "Box\0";
char mesh_combo_entry[256] = "1:Noname\0";
char material_combo_entry[256] = "Silent\0";
int iSelectedMesh = 0;
char cPreSelectedFile[MAX_PATH] = "\0";
float BaseColor[4];
float EmmisiveColor[4];
float fReflectance = 0.04;
//float fReflectance = 0.002;
bool bTransparent = false;
bool bDoubleSided = false;
float fRenderOrderBias = 0.0f;
bool bPlanerReflection = false;
bool bCastShadows = true;
DWORD dwBaseColor;
DWORD dwEmmisiveColor;
bool bStickObjectToGround = false;
bool bUpdateGrideleprof = false;
bool bHaveMaterialUpdate = false;
bool bChooseSurfaceChannel = false;
//cstr oldtextimportpath = "";
std::vector<cstr> g_MeshNamesAssimp;

// animation slot global
std::vector<sAnimSlotStruct> g_pAnimSlotList;

// animation control
bool g_bShowBones = false;
bool bFoundanimSet = false;
bool g_bAnimatingObjectPreview = false;
bool g_bUpdateAnimationPreview = false;
int g_iCurrentAnimationSlotIndex = 0;

extern cstr cInfoMessage;
extern cstr cInfoImage;
extern bool bInfo_Window;
extern int iInfoUniqueId;

sImportedObjectData g_Data;
#endif

#ifndef PRODUCTCLASSIC
extern preferences pref;
#endif

// Prototypes
void LoadFBX ( LPSTR pFilename, int iObjectNumber );

#ifdef WICKEDENGINE
void importer_init_wicked(void)
{
	// deactivate any modes editor may have been in
	waypoint_hideall ( );

	// need to see all editor entity objects wiped from screen
	Sync(); Sync();

	// set essential importer settings
	t.importer.startDir = GetDir();
	t.importer.importerActive = 1;

	// importer UI panel
	#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	bCenterRenderView = true;
	bImporter_Window = true;

	if (!t.importer.bQuitForReload)
	{
		strcpy(cImportName, "");
		t.importer.bInvertNormalMap = false;
	}
	#endif

	// Load the textures used for the objects.
	image_setlegacyimageloading(false);
	LoadImage("texturebank\\terrain grey grid_color (uncompressed).dds", g.importerextraimageoffset);
	LoadImage("texturebank\\terrain grey grid_color (uncompressed).dds", g.importerextraimageoffset + 1);
	LoadImage("editors\\uiv3\\CharacterRefDecal.png", g.importerextraimageoffset + 2);
	//LoadImage("editors\\uiv3\\temp_infinity.png", IMPORTER_ALL_MESH);
	// To do here: Load another image so the larget plane doesn't need to use the same texture as the sphere.
	//LoadImage("editors\\uiv3\\temp_infinity.png", g.importerextraimageoffset + 3);
	//LoadImage("editors\\uiv3\\i-info.png", g.importerextraimageoffset + 3);

	// Store the camera's position so that it can be restored when exiting the importer.
	t.importer.fPrevCamX = CameraPositionX();
	t.importer.fPrevCamY = CameraPositionY();
	t.importer.fPrevCamZ = CameraPositionZ();
	t.importer.fPrevCamAngX = CameraAngleX();
	t.importer.fPrevCamAngY = CameraAngleY();

	// Initial state for the mesh names.
	t.importer.bModelMeshNamesSet = false;
	t.importer.cModelMeshNames.clear();

	//visualsdatastoragetype desiredVisuals;

	//// Set the visuals 
	//set_temp_visuals(t.editorvisuals, t.visualsStorage, desiredVisuals);
}
#endif

void importer_init ( void )
{
	#ifdef WICKEDENGINE
	// Wicked Importer does it differently
	importer_init_wicked();
	#else

	#ifdef VRTECH
	// reposition camera in sky so no terrain depth clipping can occur
	//PE: bug , fix object clipping invisible import objects.
	if (g_bCameraInSkyForImporter == false)
	{
		custom_back_color[0] = 0.60f; custom_back_color[1] = 0.60f; custom_back_color[2] = 0.60f; custom_back_color[3] = 1.0f;
		PositionCamera(0, CameraPositionX(0), CameraPositionY(0) + 50000, CameraPositionZ(0));
		SetCameraRange(0, DEFAULT_NEAR_PLANE, DEFAULT_FAR_PLANE);
		g_bCameraInSkyForImporter = true;
		t.editorfreeflight.mode = 1;
		t.editorfreeflight.c.x_f = CameraPositionX();
		t.editorfreeflight.c.y_f = CameraPositionY();
		t.editorfreeflight.c.z_f = CameraPositionZ();
	}
	#endif

	// hide entities and waypoints
	importer_fade_out ( );
	for ( t.te = 1 ; t.te <= g.entityelementlist; t.te++ )
	{
		t.tobj=t.entityelement[t.te].obj;
		if ( t.tobj>0 ) 
		{
			if ( ObjectExist(t.tobj) == 1 ) HideObject ( t.tobj );
			int iCCObjHead = g.charactercreatorrmodelsoffset+((t.te*3)-t.characterkitcontrol.offset)+0;
			int iCCObjBeard = g.charactercreatorrmodelsoffset+((t.te*3)-t.characterkitcontrol.offset)+1;
			int iCCObjHat = g.charactercreatorrmodelsoffset+((t.te*3)-t.characterkitcontrol.offset)+2;
			if ( ObjectExist(iCCObjHead) == 1 ) HideObject ( iCCObjHead );
			if ( ObjectExist(iCCObjBeard) == 1 ) HideObject ( iCCObjBeard );
			if ( ObjectExist(iCCObjHat) == 1 ) HideObject ( iCCObjHat );
		}
	}
	waypoint_hideall ( );

	// need to see all editor entity objects wiped from screen
	Sync(); Sync();

	// init importer vars
	t.timportermouseoff = 0;
	t.tImporterOriginalScreenWidth = GetDisplayWidth();
	t.tImporterOriginalScreenHeight = GetDisplayHeight();

	//  Multiplier to convert mouse coords to importer coords
#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	t.tadjustedtoimporterxbase_f = ((GetChildWindowWidth(-1) + 0.0) / (float)GetDisplayWidth());
	t.tadjustedtoimporterybase_f = ((GetChildWindowHeight(-1) + 0.0) / (float)GetDisplayHeight());
#else
	t.tadjustedtoimporterxbase_f=GetChildWindowWidth()/800.0;
	t.tadjustedtoimporterybase_f=GetChildWindowHeight()/600.0;
#endif
	t.importer.scaleMulti = 1.0;

	Dim2(  t.snapPosX_f,50, 9  );
	Dim2(  t.snapPosY_f,50, 9  );
	Dim2(  t.snapPosZ_f,50, 9  );

	t.importer.cancel = 0;
	t.importer.cancelCount = 0;
	t.importer.oldMouseClick = t.inputsys.mclick;
	t.importer.reload = 0;
	t.importer.changeSizeKeyDown = 0;
	t.importer.showCollisionOnly = 0;
	t.importer.oldShowCollision = 0;

	//  grab initial folder
	t.importer.startDir = GetDir();

	importer_checkForShaderFiles ( );
	importer_checkForScriptFiles ( );

	t.importer.importerActive = 1;
	t.importer.showScaleChange = 0;
	t.importer.collisionObjectMode = 0;
	t.importer.buttonPressedCount = 0;

	for ( int tCount = 1 ; tCount <= IMPORTERTEXTURESMAX; tCount++ )
	{
		t.importerTextures[tCount].imageID = 0;
		t.importerTextures[tCount].spriteID = 0;
		t.importerTextures[tCount].spriteID2 = 0;
		t.importerTextures[tCount].fileName = "";
		t.importerTextures[tCount].iExpandedThisSlot = 0;	
		t.importerTextures[tCount].iOptionalStage = 0;
		t.importerTextures[tCount].iAssociatedBaseImage = 0;
	}

	t.importer.helpShow = 0;

	//  load resources unique to importer
	#ifdef VRTECH
	#else
	LoadImage (  "languagebank\\neutral\\gamecore\\huds\\importer\\paneltop.png",g.importermenuimageoffset+1,1 );
	LoadImage (  "languagebank\\neutral\\gamecore\\huds\\importer\\panelmiddle.png",g.importermenuimageoffset+2,1 );
	LoadImage (  "languagebank\\neutral\\gamecore\\huds\\importer\\panelbottom.png",g.importermenuimageoffset+3,1 );
	LoadImage (  "languagebank\\neutral\\gamecore\\huds\\importer\\tab.png",g.importermenuimageoffset+4,0 );
	LoadImage (  "languagebank\\neutral\\gamecore\\huds\\importer\\tabselected.png",g.importermenuimageoffset+5,0 );
	LoadImage (  "languagebank\\neutral\\gamecore\\huds\\importer\\blankTex.png",g.importermenuimageoffset+6,0 );
	LoadImage (  "languagebank\\neutral\\gamecore\\huds\\importer\\texturePanel.png",g.importermenuimageoffset+7,0 );
	LoadImage (  "languagebank\\neutral\\gamecore\\huds\\importer\\help.png",g.importermenuimageoffset+8,0 );
	//offset 9 is used with help Text (  )
	LoadImage (  "languagebank\\neutral\\gamecore\\huds\\importer\\multiTex.png",g.importermenuimageoffset+10,0 );
	#endif

	//  Help Icon
	t.tSpriteID = 200;

	//  Find free Sprite (  slot )
	while (  SpriteExist(t.tSpriteID) ) 
	{
		++t.tSpriteID;
	}
	t.importer.helpSprite = t.tSpriteID;
	if (  t.importer.scaleMulti > 1.0 ) 
	{
		if(!bRemoveSprites) Sprite (  t.importer.helpSprite , (GetChildWindowWidth()/2) - 170 + 128 , 0, g.importermenuimageoffset+8 );
	}
	else
	{
		if (!bRemoveSprites) Sprite (  t.importer.helpSprite , (GetChildWindowWidth()/2) - 170 + 128 , (GetChildWindowHeight()/2) - 300 - 4, g.importermenuimageoffset+8 );
	}
	if (!bRemoveSprites) SizeSprite (  t.importer.helpSprite , 32 , 32 );

	t.importer.helpSprite2 = t.tSpriteID+1;
	if(!bRemoveSprites) Sprite (  t.importer.helpSprite2 , (GetChildWindowWidth()/2) - 303 , (GetChildWindowHeight()/2) - 213, g.importermenuimageoffset+4 );
	if (!bRemoveSprites) SizeSprite (  t.importer.helpSprite2 , 605 , 435 );
	if (!bRemoveSprites) SetSpriteDiffuse (  t.importer.helpSprite2 , 0,0,0 );
	if (!bRemoveSprites) SetSpriteAlpha (  t.importer.helpSprite2, 200 );

	t.importer.helpSprite3 = t.tSpriteID+2;
	if (!bRemoveSprites) Sprite (  t.importer.helpSprite3 , (GetChildWindowWidth()/2) - 303 , (GetChildWindowHeight()/2) - 213, g.importermenuimageoffset+4 );
	if (!bRemoveSprites) SizeSprite (  t.importer.helpSprite3 , 605 , 40 );
	if (!bRemoveSprites) SetSpriteDiffuse (  t.importer.helpSprite3 , 0,0,0 );
	if (!bRemoveSprites) SetSpriteAlpha (  t.importer.helpSprite3, 150 );

	t.importer.helpSprite4 = t.tSpriteID+3;

	if (!bRemoveSprites) HideSprite (  t.importer.helpSprite2 );
	if (!bRemoveSprites) HideSprite (  t.importer.helpSprite3 );

	t.importer.oldTabMode = g.tabmode;
	g.tabmode = IMPORTERTABPAGE1;

	t.importer.viewMessage = "Front";

	// Check if panels already exist
	if (  t.importer.panelsExist ==  0 ) //!bRemoveSprites &&
	{
		t.importer.panelsExist = 1;
		if (  t.importer.scaleMulti  <= 0  )  t.importer.scaleMulti  =  1.0;
		g.slidersmenumax = 30; // 020718 - start out of way of TAB TAB panels
		t.importer.properties1Index = g.slidersmenumax;
		t.slidersmenu[g.slidersmenumax].tabpage=IMPORTERTABPAGE1;
		t.slidersmenu[g.slidersmenumax].title_s="Properties";
		t.slidersmenu[g.slidersmenumax].thighlight=-1;
		t.slidersmenu[g.slidersmenumax].titlemargin=63;
		t.slidersmenu[g.slidersmenumax].leftmargin=25;
		t.slidersmenu[g.slidersmenumax].itemcount=12;
		t.slidersmenu[g.slidersmenumax].panelheight=30+(t.slidersmenu[g.slidersmenumax].itemcount*38);
		t.slidersmenu[g.slidersmenumax].ttop= (GetChildWindowHeight() / 2 ) - 281 -3;
		t.slidersmenu[g.slidersmenumax].tleft= (GetChildWindowWidth() / 2 ) + 330;
		t.slidersmenuvalue[g.slidersmenumax][1].name_s="Scale";
		t.slidersmenuvalue[g.slidersmenumax][1].value=100;
		t.slidersmenuvalue[g.slidersmenumax][1].readmodeindex=0;
		t.slidersmenuvalue[g.slidersmenumax][1].useCustomRange = 1;
		t.slidersmenuvalue[g.slidersmenumax][1].valueMin = 19;
		t.slidersmenuvalue[g.slidersmenumax][1].valueMax = 500;
		t.slidersmenuvalue[g.slidersmenumax][2].name_s="Shader";
		t.slidersmenuvalue[g.slidersmenumax][2].value=2;
		t.slidersmenuvalue[g.slidersmenumax][2].value_s=t.importerShaderFiles[2];
		t.slidersmenuvalue[g.slidersmenumax][2].gadgettype=1;
		t.slidersmenuvalue[g.slidersmenumax][2].gadgettypevalue=101;
		t.slidersmenuvalue[g.slidersmenumax][3].name_s="Y Rotation";
		t.slidersmenuvalue[g.slidersmenumax][3].value=0;
		t.slidersmenuvalue[g.slidersmenumax][3].useCustomRange = 1;
		t.slidersmenuvalue[g.slidersmenumax][3].valueMin = 0;
		t.slidersmenuvalue[g.slidersmenumax][3].valueMax = 360;
		t.slidersmenuvalue[g.slidersmenumax][4].name_s="Collision Mode";
		t.slidersmenuvalue[g.slidersmenumax][4].value=1;
		t.slidersmenuvalue[g.slidersmenumax][4].value_s="Box";
		t.slidersmenuvalue[g.slidersmenumax][4].gadgettypevalue=110;
		t.slidersmenuvalue[g.slidersmenumax][4].gadgettype=1;
		t.slidersmenuvalue[g.slidersmenumax][5].name_s="Default Static";
		t.slidersmenuvalue[g.slidersmenumax][5].value=1;
		t.slidersmenuvalue[g.slidersmenumax][5].value_s="Yes";
		t.slidersmenuvalue[g.slidersmenumax][5].gadgettype=1;
		t.slidersmenuvalue[g.slidersmenumax][5].gadgettypevalue=103;
		t.slidersmenuvalue[g.slidersmenumax][6].name_s="Strength";
		t.slidersmenuvalue[g.slidersmenumax][6].value=25;
		t.slidersmenuvalue[g.slidersmenumax][6].useCustomRange = 1;
		t.slidersmenuvalue[g.slidersmenumax][6].valueMin = 0;
		t.slidersmenuvalue[g.slidersmenumax][6].valueMax = 1000;
		t.slidersmenuvalue[g.slidersmenumax][7].name_s="Is A Character";
		t.slidersmenuvalue[g.slidersmenumax][7].value=1;
		t.slidersmenuvalue[g.slidersmenumax][7].value_s="No";
		t.slidersmenuvalue[g.slidersmenumax][7].gadgettype=1;
		t.slidersmenuvalue[g.slidersmenumax][7].gadgettypevalue=117;
		t.slidersmenuvalue[g.slidersmenumax][8].name_s="Cull Mode";
		t.slidersmenuvalue[g.slidersmenumax][8].value=2;
		t.slidersmenuvalue[g.slidersmenumax][8].value_s="No";
		t.slidersmenuvalue[g.slidersmenumax][8].gadgettype=1;
		t.slidersmenuvalue[g.slidersmenumax][8].gadgettypevalue=103;
		t.slidersmenuvalue[g.slidersmenumax][9].name_s="Transparency";
		t.slidersmenuvalue[g.slidersmenumax][9].value=1;
		t.slidersmenuvalue[g.slidersmenumax][9].value_s="None";
		t.slidersmenuvalue[g.slidersmenumax][9].gadgettype=1;
		t.slidersmenuvalue[g.slidersmenumax][9].gadgettypevalue=111;
		t.slidersmenuvalue[g.slidersmenumax][10].name_s="Material";
		t.slidersmenuvalue[g.slidersmenumax][10].value=1;
		t.slidersmenuvalue[g.slidersmenumax][10].value_s="Generic";
		t.slidersmenuvalue[g.slidersmenumax][10].gadgettype=1;
		t.slidersmenuvalue[g.slidersmenumax][10].gadgettypevalue=112;
		t.slidersmenuvalue[g.slidersmenumax][11].name_s="Script";
		t.slidersmenuvalue[g.slidersmenumax][11].value=1;
		t.slidersmenuvalue[g.slidersmenumax][11].value_s=t.importerScriptFiles[1];
		t.slidersmenuvalue[g.slidersmenumax][11].gadgettype=1;
		t.slidersmenuvalue[g.slidersmenumax][11].gadgettypevalue=114;
		t.slidersmenuvalue[g.slidersmenumax][12].name_s="Height Guide";
		t.slidersmenuvalue[g.slidersmenumax][12].value=1;
		t.slidersmenuvalue[g.slidersmenumax][12].value_s="";
		t.slidersmenuvalue[g.slidersmenumax][12].gadgettype=99;
		//t.slidersmenuvalue[g.slidersmenumax][13].name_s="Geo Twizzle";
		//t.slidersmenuvalue[g.slidersmenumax][13].value=1+g_iFBXGeometryToggleMode;
		//t.slidersmenuvalue[g.slidersmenumax][13].gadgettype=1;
		//t.slidersmenuvalue[g.slidersmenumax][13].gadgettypevalue=115;
		//t.slidersmenuvalue[g.slidersmenumax][14].name_s="Center Mesh";
		//t.slidersmenuvalue[g.slidersmenumax][14].value=1+g_iFBXGeometryCenterMesh;
		//t.slidersmenuvalue[g.slidersmenumax][14].gadgettype=1;
		//t.slidersmenuvalue[g.slidersmenumax][14].gadgettypevalue=116;
		for ( int n = 0; n < 13; n++ )
		{
			// prevent scale and rotate sliders being disabled for clickchange
			t.slidersmenuvalue[g.slidersmenumax][n].expanddetect = 0;
		}

		++g.slidersmenumax;
		t.importer.properties2Index = g.slidersmenumax;
		t.slidersmenu[g.slidersmenumax].tabpage=IMPORTERTABPAGE2;
		t.slidersmenu[g.slidersmenumax].title_s="Collision";
		t.slidersmenu[g.slidersmenumax].thighlight=-1;
		t.slidersmenu[g.slidersmenumax].titlemargin=63;
		t.slidersmenu[g.slidersmenumax].leftmargin=25;
		t.slidersmenu[g.slidersmenumax].itemcount=12;
		t.slidersmenu[g.slidersmenumax].panelheight=30+(t.slidersmenu[g.slidersmenumax].itemcount*38);
		t.slidersmenu[g.slidersmenumax].ttop= (GetChildWindowHeight() / 2 ) - 281-3;
		t.slidersmenu[g.slidersmenumax].tleft= (GetChildWindowWidth() / 2 ) + 330;
		t.slidersmenuvalue[g.slidersmenumax][1].name_s="View";
		t.slidersmenuvalue[g.slidersmenumax][1].value=1;
		t.slidersmenuvalue[g.slidersmenumax][1].value_s="Front";
		t.slidersmenuvalue[g.slidersmenumax][1].gadgettype=1;
		t.slidersmenuvalue[g.slidersmenumax][1].gadgettypevalue=109;

		t.slidersmenuvalue[g.slidersmenumax][2].name_s="New";
		t.slidersmenuvalue[g.slidersmenumax][2].value=1;
		t.slidersmenuvalue[g.slidersmenumax][2].value_s="";
		t.slidersmenuvalue[g.slidersmenumax][2].gadgettype=99;

		t.slidersmenuvalue[g.slidersmenumax][3].name_s="Duplicate";
		t.slidersmenuvalue[g.slidersmenumax][3].value=1;
		t.slidersmenuvalue[g.slidersmenumax][3].value_s="";
		t.slidersmenuvalue[g.slidersmenumax][3].gadgettype=99;

		t.slidersmenuvalue[g.slidersmenumax][4].name_s="Delete";
		t.slidersmenuvalue[g.slidersmenumax][4].value=1;
		t.slidersmenuvalue[g.slidersmenumax][4].value_s="";
		t.slidersmenuvalue[g.slidersmenumax][4].gadgettype=99;

		t.slidersmenuvalue[g.slidersmenumax][5].name_s="Next";
		t.slidersmenuvalue[g.slidersmenumax][5].value=1;
		t.slidersmenuvalue[g.slidersmenumax][5].value_s="";
		t.slidersmenuvalue[g.slidersmenumax][5].gadgettype=99;

		t.slidersmenuvalue[g.slidersmenumax][6].name_s="Previous";
		t.slidersmenuvalue[g.slidersmenumax][6].value=1;
		t.slidersmenuvalue[g.slidersmenumax][6].value_s="";
		t.slidersmenuvalue[g.slidersmenumax][6].gadgettype=99;

		t.slidersmenuvalue[g.slidersmenumax][7].name_s="Rotate X";
		t.slidersmenuvalue[g.slidersmenumax][7].value=0;
		t.slidersmenuvalue[g.slidersmenumax][7].useCustomRange = 1;
		t.slidersmenuvalue[g.slidersmenumax][7].valueMin = 0;
		t.slidersmenuvalue[g.slidersmenumax][7].valueMax = 360;
		t.slidersmenuvalue[g.slidersmenumax][7].readmodeindex=0;

		t.slidersmenuvalue[g.slidersmenumax][8].name_s="Rotate Y";
		t.slidersmenuvalue[g.slidersmenumax][8].value=0;
		t.slidersmenuvalue[g.slidersmenumax][8].useCustomRange = 1;
		t.slidersmenuvalue[g.slidersmenumax][8].valueMin = 0;
		t.slidersmenuvalue[g.slidersmenumax][8].valueMax = 360;
		t.slidersmenuvalue[g.slidersmenumax][8].readmodeindex=0;

		t.slidersmenuvalue[g.slidersmenumax][9].name_s="Rotate Z";
		t.slidersmenuvalue[g.slidersmenumax][9].value=0;
		t.slidersmenuvalue[g.slidersmenumax][9].useCustomRange = 1;
		t.slidersmenuvalue[g.slidersmenumax][9].valueMin = 0;
		t.slidersmenuvalue[g.slidersmenumax][9].valueMax = 360;
		t.slidersmenuvalue[g.slidersmenumax][9].readmodeindex=0;

		t.slidersmenuvalue[g.slidersmenumax][10].name_s="";
		t.slidersmenuvalue[g.slidersmenumax][10].value=1;
		t.slidersmenuvalue[g.slidersmenumax][10].value_s="";
		t.slidersmenuvalue[g.slidersmenumax][10].gadgettype=99;

		t.slidersmenuvalue[g.slidersmenumax][11].name_s="";
		t.slidersmenuvalue[g.slidersmenumax][11].value=1;
		t.slidersmenuvalue[g.slidersmenumax][11].value_s="";
		t.slidersmenuvalue[g.slidersmenumax][11].gadgettype=99;

		t.slidersmenuvalue[g.slidersmenumax][12].name_s="";
		t.slidersmenuvalue[g.slidersmenumax][12].value=1;
		t.slidersmenuvalue[g.slidersmenumax][12].value_s="";
		t.slidersmenuvalue[g.slidersmenumax][12].gadgettype=99;

		++g.slidersmenumax;
		t.importer.properties3Index = g.slidersmenumax;
		t.slidersmenu[g.slidersmenumax].tabpage=IMPORTERTABPAGE3;
		t.slidersmenu[g.slidersmenumax].title_s="Thumbnail";
		t.slidersmenu[g.slidersmenumax].thighlight=-1;
		t.slidersmenu[g.slidersmenumax].titlemargin=63;
		t.slidersmenu[g.slidersmenumax].leftmargin=25;
		t.slidersmenu[g.slidersmenumax].itemcount=12;
		t.slidersmenu[g.slidersmenumax].panelheight=30+(t.slidersmenu[g.slidersmenumax].itemcount*38);
		t.slidersmenu[g.slidersmenumax].ttop= (GetChildWindowHeight() / 2 ) - 281-3;
		t.slidersmenu[g.slidersmenumax].tleft= (GetChildWindowWidth() / 2 ) + 330;

		t.slidersmenuvalue[g.slidersmenumax][1].name_s="Rotate X";
		t.slidersmenuvalue[g.slidersmenumax][1].value=0;
		t.slidersmenuvalue[g.slidersmenumax][1].useCustomRange = 1;
		t.slidersmenuvalue[g.slidersmenumax][1].valueMin = 0;
		t.slidersmenuvalue[g.slidersmenumax][1].valueMax = 360;
		t.slidersmenuvalue[g.slidersmenumax][1].readmodeindex=0;

		t.slidersmenuvalue[g.slidersmenumax][2].name_s="Rotate Y";
		t.slidersmenuvalue[g.slidersmenumax][2].value=0;
		t.slidersmenuvalue[g.slidersmenumax][2].useCustomRange = 1;
		t.slidersmenuvalue[g.slidersmenumax][2].valueMin = 0;
		t.slidersmenuvalue[g.slidersmenumax][2].valueMax = 360;
		t.slidersmenuvalue[g.slidersmenumax][2].readmodeindex=0;

		t.slidersmenuvalue[g.slidersmenumax][3].name_s="Rotate Z";
		t.slidersmenuvalue[g.slidersmenumax][3].value=0;
		t.slidersmenuvalue[g.slidersmenumax][3].useCustomRange = 1;
		t.slidersmenuvalue[g.slidersmenumax][3].valueMin = 0;
		t.slidersmenuvalue[g.slidersmenumax][3].valueMax = 360;
		t.slidersmenuvalue[g.slidersmenumax][3].readmodeindex=0;

		for ( t.temp = 4 ; t.temp<=  12; t.temp++ )
		{
			t.slidersmenuvalue[g.slidersmenumax][t.temp].name_s="";
			t.slidersmenuvalue[g.slidersmenumax][t.temp].value=Rnd(100);
			t.slidersmenuvalue[g.slidersmenumax][t.temp].gadgettype=99;
		}

		++g.slidersmenumax;
		t.importer.properties4Index = g.slidersmenumax;
		t.slidersmenu[g.slidersmenumax].tabpage=IMPORTERTABPAGE4;
		t.slidersmenu[g.slidersmenumax].title_s="SETTINGS 4";
		t.slidersmenu[g.slidersmenumax].thighlight=-1;
		t.slidersmenu[g.slidersmenumax].titlemargin=63;
		t.slidersmenu[g.slidersmenumax].leftmargin=25;
		t.slidersmenu[g.slidersmenumax].itemcount=11;
		t.slidersmenu[g.slidersmenumax].panelheight=30+(t.slidersmenu[g.slidersmenumax].itemcount*38);
		t.slidersmenu[g.slidersmenumax].ttop= (GetChildWindowHeight() / 2 ) - 281;
		t.slidersmenu[g.slidersmenumax].tleft= (GetChildWindowWidth() / 2 ) + 330;
		for ( t.temp = 1 ; t.temp<=  11; t.temp++ )
		{
			t.slidersmenuvalue[g.slidersmenumax][t.temp].name_s = "" ; t.slidersmenuvalue[g.slidersmenumax][t.temp].name_s=t.slidersmenuvalue[g.slidersmenumax][t.temp].name_s+"Stuff " + Str(t.temp);
			t.slidersmenuvalue[g.slidersmenumax][t.temp].value=Rnd(100);
			t.slidersmenuvalue[g.slidersmenumax][t.temp].readmodeindex=t.temp;
		}

		t.importerTabs[1].x = (GetChildWindowWidth() / 2) + 65-128 + 250;
		t.importerTabs[1].y = (GetChildWindowHeight() / 2) - 304;
		t.importerTabs[1].label = "Properties";
		t.importerTabs[1].selected = 1;
		t.importerTabs[1].tabpage = IMPORTERTABPAGE1;

		t.importerTabs[2].x = t.importerTabs[1].x + 128;
		t.importerTabs[2].y = t.importerTabs[1].y;
		t.importerTabs[2].label = "Collision";
		t.importerTabs[2].selected = 0;
		t.importerTabs[2].tabpage = IMPORTERTABPAGE2;

		t.importerTabs[3].x = t.importerTabs[2].x + 128;
		t.importerTabs[3].y = t.importerTabs[1].y;
		t.importerTabs[3].label = "Thumbnail";
		t.importerTabs[3].selected = 0;
		t.importerTabs[3].tabpage = IMPORTERTABPAGE3;

		t.importerTabs[4].x = t.importerTabs[3].x + 256;
		t.importerTabs[4].y = t.importerTabs[1].y;
		t.importerTabs[4].label = "...";
		t.importerTabs[4].selected = 0;
		t.importerTabs[4].tabpage = IMPORTERTABPAGE4;

		//  Importer button
		t.importerTabs[5].x = (GetChildWindowWidth() / 2) - 320;
		t.importerTabs[5].y = (GetChildWindowHeight() / 2) - 304;
		t.importerTabs[5].label = "Save Entity";
		t.importerTabs[5].selected = 0;
		t.importerTabs[5].tabpage = -1;

		//  New button
		t.importerTabs[6].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
		t.importerTabs[6].y = t.importerTabs[1].y + 125;
		t.importerTabs[6].label = "Add New";
		t.importerTabs[6].selected = 0;
		t.importerTabs[6].tabpage = -1;

		//  Dupe button
		t.importerTabs[12].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
		t.importerTabs[12].y = t.importerTabs[1].y + 125 + 38;
		t.importerTabs[12].label = "Duplicate";
		t.importerTabs[12].selected = 0;
		t.importerTabs[12].tabpage = -1;

		//  Delete button
		t.importerTabs[7].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
		t.importerTabs[7].y = t.importerTabs[1].y + 125 + (38*2);
		t.importerTabs[7].label = "Delete";
		t.importerTabs[7].selected = 0;
		t.importerTabs[7].tabpage = -1;

		//  Next button
		t.importerTabs[8].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
		t.importerTabs[8].y = t.importerTabs[1].y + 125 + (38*3);
		t.importerTabs[8].label = "Next";
		t.importerTabs[8].selected = 0;
		t.importerTabs[8].tabpage = -1;

		//  Previous button
		t.importerTabs[9].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
		t.importerTabs[9].y = t.importerTabs[1].y + 125 + (38*4);
		t.importerTabs[9].label = "Previous";
		t.importerTabs[9].selected = 0;
		t.importerTabs[9].tabpage = -1;

		//  Show Guide button
		t.importerTabs[10].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
		t.importerTabs[10].y = t.importerTabs[1].y + 125 + (38*10) - 5;
		t.importerTabs[10].label = "Turn Guide Off";
		t.importerTabs[10].selected = 1;
		t.importerTabs[10].tabpage = -1;

		//  Importer button
		t.importerTabs[11].x = (GetChildWindowWidth() / 2) - 320 +128;
		t.importerTabs[11].y = (GetChildWindowHeight() / 2) - 304;
		t.importerTabs[11].label = "Cancel";
		t.importerTabs[11].selected = 0;
		t.importerTabs[11].tabpage = -1;

		t.importer.slidersmenumax=g.slidersmenumax;
	}
	else
	{
		g.slidersmenumax=t.importer.slidersmenumax;
	}

	if (!bRemoveSprites) {

		// get actual value string names
		t.slidersmenuvaluechoice = 115;
		t.slidersmenuvalueindex = t.slidersmenuvalue[t.importer.properties1Index][13].value;
		sliders_getnamefromvalue();
		t.slidersmenuvalue[t.importer.properties1Index][13].value_s = t.slidervaluename_s;
		t.slidersmenuvaluechoice = 116;
		t.slidersmenuvalueindex = t.slidersmenuvalue[t.importer.properties1Index][14].value;
		sliders_getnamefromvalue();
		t.slidersmenuvalue[t.importer.properties1Index][14].value_s = t.slidervaluename_s;

		if (t.importer.scaleMulti > 1.0)
		{
			t.importerTabs[1].y = 0;
			t.importerTabs[2].y = 0;
			t.importerTabs[3].y = 0;
			t.importerTabs[4].y = 0;
			t.importerTabs[5].y = 0;
			t.importerTabs[11].y = 0;
			t.slidersmenu[t.importer.properties1Index].tleft = GetChildWindowWidth() - 255;
			t.slidersmenu[t.importer.properties2Index].tleft = GetChildWindowWidth() - 255;
			t.slidersmenu[t.importer.properties3Index].tleft = GetChildWindowWidth() - 255;
			t.slidersmenu[t.importer.properties4Index].tleft = GetChildWindowWidth() - 255;

			//  New button
			t.importerTabs[6].x = GetChildWindowWidth() - 159;
			t.importerTabs[7].x = GetChildWindowWidth() - 159;
			t.importerTabs[8].x = GetChildWindowWidth() - 159;
			t.importerTabs[9].x = GetChildWindowWidth() - 159;
			t.importerTabs[10].x = GetChildWindowWidth() - 159;

			t.importerTabs[1].label = "Properties";
			t.importerTabs[1].selected = 1;
			t.importerTabs[1].tabpage = IMPORTERTABPAGE1;

			t.importerTabs[2].label = "Collision";
			t.importerTabs[2].selected = 0;
			t.importerTabs[2].tabpage = IMPORTERTABPAGE2;

			t.importerTabs[3].label = "Thumbnail";
			t.importerTabs[3].selected = 0;
			t.importerTabs[3].tabpage = IMPORTERTABPAGE3;

			t.importerTabs[4].label = "...";
			t.importerTabs[4].selected = 0;
			t.importerTabs[4].tabpage = IMPORTERTABPAGE4;

			//  Importer button
			t.importerTabs[5].label = "Save Entity";
			t.importerTabs[5].selected = 0;
			t.importerTabs[5].tabpage = -1;

			//  New button
			t.importerTabs[6].label = "Add New";
			t.importerTabs[6].selected = 0;
			t.importerTabs[6].tabpage = -1;

			//  New button
			t.importerTabs[12].label = "Duplicate";
			t.importerTabs[12].selected = 0;
			t.importerTabs[12].tabpage = -1;

			//  Delete button
			t.importerTabs[7].label = "Delete";
			t.importerTabs[7].selected = 0;
			t.importerTabs[7].tabpage = -1;

			//  Next button
			t.importerTabs[8].label = "Next";
			t.importerTabs[8].selected = 0;
			t.importerTabs[8].tabpage = -1;

			//  Previous button
			t.importerTabs[9].label = "Previous";
			t.importerTabs[9].selected = 0;
			t.importerTabs[9].tabpage = -1;

			//  Show Guide button
			t.importerTabs[10].label = "Turn Guide Off";
			t.importerTabs[10].selected = 1;
			t.importerTabs[10].tabpage = -1;

			//  Importer button
			t.importerTabs[11].label = "Cancel";
			t.importerTabs[11].selected = 0;
			t.importerTabs[11].tabpage = -1;

		}
		else
		{
			t.slidersmenu[t.importer.properties1Index].tleft = (GetChildWindowWidth() / 2) + 330;
			t.slidersmenu[t.importer.properties2Index].tleft = (GetChildWindowWidth() / 2) + 330;
			t.slidersmenu[t.importer.properties3Index].tleft = (GetChildWindowWidth() / 2) + 330;
			t.slidersmenu[t.importer.properties4Index].tleft = (GetChildWindowWidth() / 2) + 330;

			t.importerTabs[1].x = (GetChildWindowWidth() / 2) + 65 - 128 + 250;
			t.importerTabs[1].y = (GetChildWindowHeight() / 2) - 304;
			t.importerTabs[1].label = "Properties";
			t.importerTabs[1].selected = 1;
			t.importerTabs[1].tabpage = IMPORTERTABPAGE1;

			t.importerTabs[2].x = t.importerTabs[1].x + 128;
			t.importerTabs[2].y = t.importerTabs[1].y;
			t.importerTabs[2].label = "Collision";
			t.importerTabs[2].selected = 0;
			t.importerTabs[2].tabpage = IMPORTERTABPAGE2;

			t.importerTabs[3].x = t.importerTabs[2].x + 128;
			t.importerTabs[3].y = t.importerTabs[1].y;
			t.importerTabs[3].label = "Thumbnail";
			t.importerTabs[3].selected = 0;
			t.importerTabs[3].tabpage = IMPORTERTABPAGE3;

			t.importerTabs[4].x = t.importerTabs[3].x + 256;
			t.importerTabs[4].y = t.importerTabs[1].y;
			t.importerTabs[4].label = "...";
			t.importerTabs[4].selected = 0;
			t.importerTabs[4].tabpage = IMPORTERTABPAGE4;

			//  Importer button
			t.importerTabs[5].x = (GetChildWindowWidth() / 2) - 320;
			t.importerTabs[5].y = (GetChildWindowHeight() / 2) - 304;
			t.importerTabs[5].label = "Save Entity";
			t.importerTabs[5].selected = 0;
			t.importerTabs[5].tabpage = -1;

			//  New button
			t.importerTabs[6].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
			t.importerTabs[6].y = t.importerTabs[1].y + 125;
			t.importerTabs[6].label = "Add New";
			t.importerTabs[6].selected = 0;
			t.importerTabs[6].tabpage = -1;

			//  New button
			t.importerTabs[12].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
			t.importerTabs[12].y = t.importerTabs[1].y + 125 + 38;
			t.importerTabs[12].label = "Duplicate";
			t.importerTabs[12].selected = 0;
			t.importerTabs[12].tabpage = -1;

			//  Delete button
			t.importerTabs[7].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
			t.importerTabs[7].y = t.importerTabs[1].y + 125 + (38 * 2);
			t.importerTabs[7].label = "Delete";
			t.importerTabs[7].selected = 0;
			t.importerTabs[7].tabpage = -1;

			//  Next button
			t.importerTabs[8].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
			t.importerTabs[8].y = t.importerTabs[1].y + 125 + (38 * 3);
			t.importerTabs[8].label = "Next";
			t.importerTabs[8].selected = 0;
			t.importerTabs[8].tabpage = -1;

			//  Previous button
			t.importerTabs[9].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
			t.importerTabs[9].y = t.importerTabs[1].y + 125 + (38 * 4);
			t.importerTabs[9].label = "Previous";
			t.importerTabs[9].selected = 0;
			t.importerTabs[9].tabpage = -1;

			//  Show Guide button
			t.importerTabs[10].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
			t.importerTabs[10].y = t.importerTabs[1].y + 125 + (38 * 10) - 5;
			t.importerTabs[10].label = "Turn Guide Off";
			t.importerTabs[10].selected = 1;
			t.importerTabs[10].tabpage = -1;

			//  Importer button
			t.importerTabs[11].x = (GetChildWindowWidth() / 2) - 320 + 128;
			t.importerTabs[11].y = (GetChildWindowHeight() / 2) - 304;
			t.importerTabs[11].label = "Cancel";
			t.importerTabs[11].selected = 0;
			t.importerTabs[11].tabpage = -1;
		}
		t.timporterpickdepth_f = 1250;
	}

	#ifdef VRTECH
	#else
	// reposition camera in sky so no terrain depth clipping can occur
	//PE: bug , fix object clipping invisible import objects.
	if ( g_bCameraInSkyForImporter == false )
	{
		PositionCamera ( 0, CameraPositionX(0), CameraPositionY(0)+50000, CameraPositionZ(0) );
		SetCameraRange ( 0, DEFAULT_NEAR_PLANE, DEFAULT_FAR_PLANE );
		g_bCameraInSkyForImporter = true;
	}
	#endif

	#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	bCenterRenderView = true;
	bImporter_Window = true;
	strcpy(cImportName, "");
	#endif
	
	#endif // wicked endif

}

// Change the visual setting in currentVisuals to the settings from desiredVisuals, then store them in storage so they can be restored later.
void set_temp_visuals(visualstype& currentVisuals, visualsdatastoragetype& storage, const visualsdatastoragetype& desiredVisuals)
{
	// First, store all of the current visuals
	storage.AmbienceRed_f =currentVisuals.AmbienceRed_f;
	storage.AmbienceGreen_f = currentVisuals.AmbienceGreen_f;
	storage.AmbienceBlue_f = currentVisuals.AmbienceBlue_f;
	storage.FogR_f = currentVisuals.FogR_f;
	storage.FogG_f = currentVisuals.FogG_f;
	storage.FogB_f = currentVisuals.FogB_f;
	storage.FogA_f = currentVisuals.FogA_f;
	#ifdef WICKEDENGINE
	storage.SunIntensity_f = currentVisuals.SunIntensity_f;
	storage.SunRed_f = currentVisuals.SunRed_f;
	storage.SunGreen_f = currentVisuals.SunGreen_f;
	storage.SunBlue_f = currentVisuals.SunBlue_f;
	storage.ZenithRed_f = currentVisuals.ZenithRed_f;
	storage.ZenithGreen_f = currentVisuals.ZenithGreen_f;
	storage.ZenithBlue_f = currentVisuals.ZenithBlue_f;
	storage.bBloomEnabled = currentVisuals.bBloomEnabled;
	storage.bSSREnabled = currentVisuals.bSSREnabled;
	storage.bReflectionsEnabled = currentVisuals.bReflectionsEnabled;
	storage.bLightShafts = currentVisuals.bLightShafts;
	storage.bLensFlare = currentVisuals.bLensFlare;
	storage.bAutoExposure = currentVisuals.bAutoExposure;
	storage.fGamma = currentVisuals.fGamma;
	storage.SunAngleX = currentVisuals.SunAngleX;
	storage.SunAngleY = currentVisuals.SunAngleY;
	storage.SunAngleZ = currentVisuals.SunAngleZ;
	storage.iTimeOfday = currentVisuals.iTimeOfday;
	storage.fExposure = currentVisuals.fExposure;
	#endif
	storage.skyindex = currentVisuals.skyindex;
	
	// Set the visuals to the desired settings.
	currentVisuals.AmbienceRed_f = desiredVisuals.AmbienceRed_f;
	currentVisuals.AmbienceGreen_f = desiredVisuals.AmbienceGreen_f;
	currentVisuals.AmbienceBlue_f = desiredVisuals.AmbienceBlue_f;
	currentVisuals.FogR_f = desiredVisuals.FogR_f;
	currentVisuals.FogG_f = desiredVisuals.FogG_f;
	currentVisuals.FogB_f = desiredVisuals.FogB_f;
	currentVisuals.FogA_f = desiredVisuals.FogA_f;
	#ifdef WICKEDENGINE
	currentVisuals.SunIntensity_f = desiredVisuals.SunIntensity_f;
	currentVisuals.SunRed_f = desiredVisuals.SunRed_f;
	currentVisuals.SunGreen_f = desiredVisuals.SunGreen_f;
	currentVisuals.SunBlue_f = desiredVisuals.SunBlue_f;
	currentVisuals.ZenithRed_f = desiredVisuals.ZenithRed_f;
	currentVisuals.ZenithGreen_f = desiredVisuals.ZenithGreen_f;
	currentVisuals.ZenithBlue_f = desiredVisuals.ZenithBlue_f;
	currentVisuals.bBloomEnabled = desiredVisuals.bBloomEnabled;
	currentVisuals.bSSREnabled = desiredVisuals.bSSREnabled;
	currentVisuals.bReflectionsEnabled = desiredVisuals.bReflectionsEnabled;
	currentVisuals.bLightShafts = desiredVisuals.bLightShafts;
	currentVisuals.bLensFlare = desiredVisuals.bLensFlare;
	currentVisuals.bAutoExposure = desiredVisuals.bAutoExposure;
	currentVisuals.fGamma = desiredVisuals.fGamma;
	currentVisuals.SunAngleX = desiredVisuals.SunAngleX;
	currentVisuals.SunAngleY = desiredVisuals.SunAngleY;
	currentVisuals.SunAngleZ = desiredVisuals.SunAngleZ;
	currentVisuals.iTimeOfday = desiredVisuals.iTimeOfday;
	currentVisuals.fExposure = desiredVisuals.fExposure;
	#endif
	//currentVisuals.skyindex = desiredVisuals.skyindex;
	currentVisuals.refreshskysettings = 0; //1 ZJ: prevent sky_skyspec_init() resetting sun rotation.
	currentVisuals.refreshshaders = 1;
}

void restore_visuals(visualstype& currentVisuals, visualsdatastoragetype& storage)
{
	// Restore visual settings to what they were on init.
	currentVisuals.AmbienceRed_f = storage.AmbienceRed_f;
	currentVisuals.AmbienceGreen_f = storage.AmbienceGreen_f;
	currentVisuals.AmbienceBlue_f = storage.AmbienceBlue_f;
	currentVisuals.FogR_f = storage.FogR_f;
	currentVisuals.FogG_f = storage.FogG_f;
	currentVisuals.FogB_f = storage.FogB_f;
	currentVisuals.FogA_f = storage.FogA_f;
	#ifdef WICKEDENGINE
	currentVisuals.SunIntensity_f = storage.SunIntensity_f;
	currentVisuals.SunRed_f = storage.SunRed_f;
	currentVisuals.SunGreen_f = storage.SunGreen_f;
	currentVisuals.SunBlue_f = storage.SunBlue_f;
	currentVisuals.ZenithRed_f = storage.ZenithRed_f;
	currentVisuals.ZenithGreen_f = storage.ZenithGreen_f;
	currentVisuals.ZenithBlue_f = storage.ZenithBlue_f;
	currentVisuals.bBloomEnabled = storage.bBloomEnabled;
	currentVisuals.bSSREnabled = storage.bSSREnabled;
	currentVisuals.bReflectionsEnabled = storage.bReflectionsEnabled;
	currentVisuals.bLightShafts = storage.bLightShafts;
	currentVisuals.bLensFlare = storage.bLensFlare;
	currentVisuals.bAutoExposure = storage.bAutoExposure;
	currentVisuals.fGamma = storage.fGamma;
	currentVisuals.SunAngleX = storage.SunAngleX;
	currentVisuals.SunAngleY = storage.SunAngleY;
	currentVisuals.SunAngleZ = storage.SunAngleZ;
	currentVisuals.iTimeOfday = storage.iTimeOfday;
	currentVisuals.fExposure = storage.fExposure;
	#endif
	currentVisuals.skyindex = storage.skyindex;
	currentVisuals.refreshskysettings = 1;
	currentVisuals.refreshshaders = 1;
}

#ifdef WICKEDENGINE
void importer_free_wicked(void)
{
	// show previously hidden editor modes
	waypoint_showall ( );

	// free resources (with wicked, deleting objects CAN CRASH as threads may still be using them - need a way to signal there deleting at the right time)
	//if ( ObjectExist(t.importer.objectnumber) ) DeleteObject ( t.importer.objectnumber );
	//PE: Imgui can still be visible with the textures , so free after import window calls.
	if ( ObjectExist(t.importer.objectnumber) ) HideObject ( t.importer.objectnumber ); // while testing all import formats

	// deactivate importer mode
	t.importer.loaded = 0;

	// return tab mode back to original state
	g.tabmode = t.importer.oldTabMode;

	// hide debug bone visuals
	wiRenderer::SetToDrawDebugBoneLines(false);

	// delete any surface files that were created but not used on the imported model.
	importer_delete_old_surface_files();

	// set back to initial dir
	SetDir ( t.importer.startDir.Get() );
}
#endif

void importer_free ( void )
{
	#ifdef WICKEDENGINE
	importer_free_wicked();
	if (GetImageExistEx(IMPORTER_TMP_IMAGE))
	{
		DeleteImage(IMPORTER_TMP_IMAGE);
	}

	// Delete the sphere and planes (3 objects starting at g.importerextraobjectoffset).
	for (int i = 2; i >= 0; i--)
	{
		if (ObjectExist(g.importerextraobjectoffset + i))
		{
			DeleteObject(g.importerextraobjectoffset + i);
		}
	}
	
	//	Delete the textures used by the spheres and planes (3 textures starting at g.importerextraimageoffset).
	for (int i = 3; i >= 0; i--)
	{
		if (GetImageExistEx(g.importerextraimageoffset + i))
		{
			DeleteImage(g.importerextraimageoffset + i);
		}
	}

	// Restore the camera's orientation from before the importer loaded.
	PositionCamera(t.importer.fPrevCamX, t.importer.fPrevCamY, t.importer.fPrevCamZ);
	t.editorfreeflight.c.x_f = CameraPositionX();
	t.editorfreeflight.c.y_f = CameraPositionY();
	t.editorfreeflight.c.z_f = CameraPositionZ();
	t.editorfreeflight.c.angx_f = t.importer.fPrevCamAngX;
	t.editorfreeflight.c.angy_f = t.importer.fPrevCamAngY;

	restore_visuals(t.visuals, t.visualsStorage);
	restore_visuals(t.editorvisuals, t.visualsStorage);

	#else

	// restore camera from sky
	if ( g_bCameraInSkyForImporter == true )
	{
		//Turn off custom clear color.
		#ifdef VRTECH
		custom_back_color[0] = 0.0f; custom_back_color[1] = 0.0f; custom_back_color[2] = 0.0f; custom_back_color[3] = 0.0f;
		#endif
		PositionCamera ( 0, CameraPositionX(0), CameraPositionY(0)-50000, CameraPositionZ(0) );

		if ( t.editorfreeflight.mode == 1 )
			SetCameraRange ( 0, t.tcamneardistance_f, DEFAULT_FAR_PLANE );
		else
			SetCameraRange ( 0, t.tcamneardistance_f, t.tcamrange_f );
		g_bCameraInSkyForImporter = false;
		t.editorfreeflight.c.x_f = CameraPositionX();
		t.editorfreeflight.c.y_f = CameraPositionY();
		t.editorfreeflight.c.z_f = CameraPositionZ();
	}
	// show entities and waypoints
	importer_show_mouse ( );
	for ( t.te = 1 ; t.te <= g.entityelementlist; t.te++ )
	{
		t.tobj=t.entityelement[t.te].obj;
		if ( t.tobj>0 ) 
		{
			if ( ObjectExist(t.tobj) == 1 ) ShowObject ( t.tobj );
			int iCCObjHead = g.charactercreatorrmodelsoffset+((t.te*3)-t.characterkitcontrol.offset)+0;
			int iCCObjBeard = g.charactercreatorrmodelsoffset+((t.te*3)-t.characterkitcontrol.offset)+1;
			int iCCObjHat = g.charactercreatorrmodelsoffset+((t.te*3)-t.characterkitcontrol.offset)+2;
			if ( ObjectExist(iCCObjHead) == 1 ) ShowObject ( iCCObjHead );
			if ( ObjectExist(iCCObjBeard) == 1 ) ShowObject ( iCCObjBeard );
			if ( ObjectExist(iCCObjHat) == 1 ) ShowObject ( iCCObjHat );
		}
	}
	waypoint_showall ( );

	//  Free any resources
	for ( t.i = 1 ; t.i<=  15; t.i++ )
	{
		if (  ImageExist(g.importermenuimageoffset+t.i) == 1  )  DeleteImage (  g.importermenuimageoffset+t.i );
	}

	//  DeleteObjects (  used )
	if (  ObjectExist(t.importer.objectnumber) ) DeleteObject ( t.importer.objectnumber );
	if (  ObjectExist(t.importer.objectnumberpreeffectcopy) ) DeleteObject ( t.importer.objectnumberpreeffectcopy );
	if (  ObjectExist(t.importer.dummyCharacterObjectNumber)  )  DeleteObject (  t.importer.dummyCharacterObjectNumber );

	//  Delete any textures and sprites used
	int tCount = 1;
	for ( tCount = 1 ; tCount <= IMPORTERTEXTURESMAX; tCount++ )
	{
		if (  t.importerTextures[tCount].spriteID  !=  0 ) 
		{
			if ( SpriteExist ( t.importerTextures[tCount].spriteID ) ) DeleteSprite (  t.importerTextures[tCount].spriteID );
		}
		if (  t.importerTextures[tCount].spriteID2  !=  0 ) 
		{
			if ( SpriteExist ( t.importerTextures[tCount].spriteID2 ) ) DeleteSprite (  t.importerTextures[tCount].spriteID2 );
		}
		t.importerTextures[tCount].spriteID = 0;
		t.importerTextures[tCount].spriteID2 = 0;
		if (  t.importerTextures[tCount].imageID  !=  0 ) 
		{
			if (  ImageExist(t.importerTextures[tCount].imageID)  )  DeleteImage (  t.importerTextures[tCount].imageID );
		}
		t.importerTextures[tCount].imageID = 0;
		t.importerTextures[tCount].originalName = "";
		t.importerTextures[tCount].fileName = "";
		t.importerTextures[tCount].iExpandedThisSlot = 0;
		t.importerTextures[tCount].iOptionalStage = 0;
		t.importerTextures[tCount].iAssociatedBaseImage = 0;
	}

	if (  SpriteExist (t.importer.helpSprite) )   DeleteSprite (  t.importer.helpSprite );
	if (  SpriteExist (t.importer.helpSprite2)  )  DeleteSprite (  t.importer.helpSprite2 );
	if (  SpriteExist (t.importer.helpSprite3)  )  DeleteSprite (  t.importer.helpSprite3 );
	if (  SpriteExist (t.importer.helpSprite4)  )  DeleteSprite (  t.importer.helpSprite4 );

	//  Delete grid objects
	for ( tCount = 1 ; tCount<=  9; tCount++ )
	{
		if (  t.importerGridObject[tCount] > 0 ) 
		{
			if (  ObjectExist(t.importerGridObject[tCount])  ==  1  )  DeleteObject (  t.importerGridObject[tCount] );
		}
	}

	//  Free Collision boxes
	for ( tCount = 0 ; tCount<=  t.importer.collisionShapeCount; tCount++ )
	{
		if (  t.importerCollision[tCount].object > 0 ) 
		{
			if (  ObjectExist(t.importerCollision[tCount].object)  ==  1 ) 
			{
				DeleteObject (  t.importerCollision[tCount].object );
				DeleteObject (  t.importerCollision[tCount].object2 );
			}
		}
	}

	//  DeleteObject (  selected markers )
	for ( tCount = 1 ; tCount<=  9; tCount++ )
	{
		if (  t.selectedObjectMarkers[tCount] > 0 ) 
		{
			if (  ObjectExist(t.selectedObjectMarkers[tCount])  ==  1  )  DeleteObject (  t.selectedObjectMarkers[tCount] );
		}
	}

	t.importer.loaded = 0;
	t.importerTabs[11].selected = 0;

	//  Return tab mode back to original state
	g.tabmode = t.importer.oldTabMode;

	//  Set back to initial dir
	SetDir (  t.importer.startDir.Get() );

	UnDim (  t.tUnknown );
	UnDim (  t.snapPosX_f );
	UnDim (  t.snapPosY_f );
	UnDim (  t.snapPosZ_f );

	#endif // wicked endif
}

cstr importer_getfilenameonly ( LPSTR pFileAndPossiblePath )
{
	// Special case is when stripping path of a character creator body part, we need to retain the path
	if (pFileAndPossiblePath)
	{
		LPSTR pCCPPath = "charactercreatorplus\\parts\\";
		if ( strnicmp ( pFileAndPossiblePath, pCCPPath, strlen(pCCPPath) ) == NULL )
			return pFileAndPossiblePath;
	}

	cstr pFileNameOnly = pFileAndPossiblePath;
	for ( int n = strlen(pFileAndPossiblePath); n > 0; n-- )
	{
		if ( pFileAndPossiblePath[n] == '\\' || pFileAndPossiblePath[n] == '/' )
		{
			pFileNameOnly = cstr(pFileAndPossiblePath+n+1);
			break;
		}
	}
	return pFileNameOnly;
}

int importer_findtextureinlist ( LPSTR pFindFilename )
{
	for ( int iImageListIndex = 1; iImageListIndex < IMPORTERTEXTURESMAX; iImageListIndex++ )
	{
		if ( t.importerTextures[iImageListIndex].imageID > 0 )
		{
			cstr pCompareWith = t.importerTextures[iImageListIndex].fileName;
			if ( strnicmp ( pCompareWith.Get(), pFindFilename, strlen(pCompareWith.Get()) ) == NULL )
			{
				return iImageListIndex;
			}
		}
	}
	return 0;
}

int importer_findtextureindexinlist ( LPSTR pFindFilename )
{
	int iImageListindex = importer_findtextureinlist ( pFindFilename );
	if ( iImageListindex > 0 )
		return t.importerTextures[iImageListindex].imageID;
	else
		return 0;
}

int importer_addtexturefiletolist ( cstr fileName, cstr sourceName, int* tCount )
{
	// Check if we already have this texture
	t.tfound = 0;
	for ( t.tCount2 = 1 ; t.tCount2 <= IMPORTERTEXTURESMAX; t.tCount2++ )
	{
		if (  t.importerTextures[t.tCount2].fileName  == fileName ) 
		{
			t.tfound = 1;
			break;
		}
	}

	// If we don't have the texture, add it to list
	if ( t.tfound == 0 ) 
	{
		// find free slot
		t.tfound = 0;
		for ( int tCount3 = 1 ; tCount3 <= IMPORTERTEXTURESMAX; tCount3++ )
		{
			if ( strlen ( t.importerTextures[tCount3].fileName.Get() ) == 0 ) 
			{
				t.tfound = tCount3;
				break;
			}
		}

		// did we find a free texture slot?
		if ( t.tfound > 0 ) 
		{
			//  Add to importer texture list
			if ( t.tfound < IMPORTERTEXTURESMAX )
			{
				if ( t.tfound <= (*tCount) )
				{
					t.importerTextures[t.tfound].fileName = fileName;
					t.importerTextures[t.tfound].originalName = sourceName;
				}
				else
				{
					++(*tCount);
					t.importerTextures[(*tCount)].fileName = fileName;
					t.importerTextures[(*tCount)].originalName = sourceName;
				}
			}
		}
	}
	return t.tfound;
}

void importer_addtoimagelistandloadifexist ( LPSTR pImgFilename, int iOptionalStage, int iOptionalBaseImageSlotIndex )
{
	int tCount = t.tcounttextures;
	if ( FileExist ( pImgFilename ) )
	{
		// assign image to new slot in image list
		int iInsertedAtSlot = importer_addtexturefiletolist ( pImgFilename, pImgFilename, &tCount );

		// assign any special texture 'stage' value
		t.importerTextures[iInsertedAtSlot].iOptionalStage = iOptionalStage;
		t.importerTextures[iInsertedAtSlot].iAssociatedBaseImage = iOptionalBaseImageSlotIndex;

		// load image in
		t.tImageID = g.importermenuimageoffset+15;
		while ( ImageExist(t.tImageID) == 1 ) ++t.tImageID;
		LoadImage ( t.importerTextures[iInsertedAtSlot].fileName.Get(), t.tImageID );
		t.importerTextures[iInsertedAtSlot].imageID = t.tImageID;
	}
	t.tcounttextures = tCount;
}

void importer_findimagetypesfromlist ( cstr fileName, int iBaseImageSlotIndex, int* piImgColor, int* piImgNormal, int* piImgSpecular, int* piImgGloss, int* piImgAO, int* piImgHeight )
{
	// get base filename extension (deduct image format ext)
	LPSTR pExt = NULL;
	for ( int iImgFormat = 0; iImgFormat < 4; iImgFormat++ )
	{
		if ( iImgFormat == 0 ) pExt = ".png";
		if ( iImgFormat == 1 ) pExt = ".dds";
		if ( iImgFormat == 2 ) pExt = ".tga";
		if ( iImgFormat == 3 ) pExt = ".jpg";
		if ( strnicmp ( fileName.Get()+strlen(fileName.Get())-strlen(pExt), pExt, strlen(pExt) ) == NULL ) 
			break;
	}
	if ( pExt == NULL ) return;

	// get base filename extension (deduct color specifier)
	LPSTR pImgType = NULL;
	cstr pBaseNoFileExt = cstr(Left(fileName.Get(),Len(fileName.Get())-Len(pExt)));
	#ifdef VRTECH
	for ( int iImgType = 0; iImgType < 6; iImgType++ )
	#else
	for ( int iImgType = 0; iImgType < 5; iImgType++ )
	#endif
	{
		// find kind of 'color' image type
		if ( iImgType == 0 ) pImgType = "_diffuse";
		if ( iImgType == 1 ) pImgType = "_color";
		if ( iImgType == 2 ) pImgType = "_d";
		if ( iImgType == 3 ) pImgType = "_albedo";
		if ( iImgType == 4) pImgType = "_diff";
		if ( iImgType == 5) pImgType = "_dif";
		
		if ( strnicmp ( pBaseNoFileExt.Get()+strlen(pBaseNoFileExt.Get())-strlen(pImgType), pImgType, strlen(pImgType) ) == NULL )
		{
			// get base filename (minus image type)
			cstr pBaseFile = cstr(Left(pBaseNoFileExt.Get(),Len(pBaseNoFileExt.Get())-Len(pImgType)));

			// locate color image from image list
			cstr pColorFile = pBaseFile + cstr(pImgType) + cstr(pExt);
			*piImgColor = importer_findtextureindexinlist ( pColorFile.Get() );

			// attempt to locate other textures associated with color image
			cstr pNormalFile = pBaseFile + cstr("_normal") + cstr(pExt);
			importer_addtoimagelistandloadifexist ( pNormalFile.Get(), 2, iBaseImageSlotIndex );
			*piImgNormal = importer_findtextureindexinlist ( pNormalFile.Get() );
			#ifdef VRTECH
			if (*piImgNormal == 0) {
				cstr pNormalFile = pBaseFile + cstr("_ddn") + cstr(pExt);
				importer_addtoimagelistandloadifexist(pNormalFile.Get(), 2, iBaseImageSlotIndex);
				*piImgNormal = importer_findtextureindexinlist(pNormalFile.Get());
				if (*piImgNormal == 0) {
					cstr pNormalFile = pBaseFile + cstr("_nrm") + cstr(pExt);
					importer_addtoimagelistandloadifexist(pNormalFile.Get(), 2, iBaseImageSlotIndex);
					*piImgNormal = importer_findtextureindexinlist(pNormalFile.Get());
				}
			}
			#endif

			cstr pSpecularFile = pBaseFile + cstr("_specular") + cstr(pExt);
			importer_addtoimagelistandloadifexist ( pSpecularFile.Get(), 3, iBaseImageSlotIndex );
			*piImgSpecular = importer_findtextureindexinlist ( pSpecularFile.Get() );
			if ( *piImgSpecular == 0 )
			{
				cstr pMetalnessFile = pBaseFile + cstr("_metalness") + cstr(pExt);
				importer_addtoimagelistandloadifexist ( pMetalnessFile.Get(), 3, iBaseImageSlotIndex );
				*piImgSpecular = importer_findtextureindexinlist ( pMetalnessFile.Get() );
			}

			cstr pGlossFile = pBaseFile + cstr("_gloss") + cstr(pExt);
			importer_addtoimagelistandloadifexist ( pGlossFile.Get(), 4, iBaseImageSlotIndex );
			*piImgGloss = importer_findtextureindexinlist ( pGlossFile.Get() );

			cstr pAOFile = pBaseFile + cstr("_ao") + cstr(pExt);
			importer_addtoimagelistandloadifexist ( pAOFile.Get(), 1, iBaseImageSlotIndex );
			*piImgAO = importer_findtextureindexinlist ( pAOFile.Get() );
			if ( *piImgAO == 0 ) 
			{
				pAOFile = g.rootdir_s + cstr("effectbank\\reloaded\\media\\blank_O.dds");
				importer_addtoimagelistandloadifexist ( pAOFile.Get(), 1, iBaseImageSlotIndex );
				*piImgAO = importer_findtextureindexinlist ( pAOFile.Get() );
			}

			cstr pHeightFile = pBaseFile + cstr("_height") + cstr(pExt);
			importer_addtoimagelistandloadifexist ( pHeightFile.Get(), 5, iBaseImageSlotIndex );
			*piImgHeight = importer_findtextureindexinlist ( pHeightFile.Get() );
		}
	}
}

void importer_findandremoveentry ( cstr sFileToRemove )
{
	for ( int tCount = 1 ; tCount <= IMPORTERTEXTURESMAX; tCount++ )
	{
		if ( strnicmp ( t.importerTextures[tCount].fileName.Get(), sFileToRemove.Get(), strlen(sFileToRemove.Get())-4 ) == NULL ) 
		{
			if ( ImageExist ( t.importerTextures[tCount].imageID ) == 1 ) DeleteImage ( t.importerTextures[tCount].imageID );
			t.importerTextures[tCount].imageID = 0;
			t.importerTextures[tCount].fileName = "";
		}
	}
}

void importer_removeentryandassociatesof ( int tCount )
{
	// the file to delete (and all its associates)
	LPSTR pRemoveTextureFile = t.importerTextures[tCount].fileName.Get();

	// get base filename extension (deduct image format ext)
	LPSTR pExt = NULL;
	for ( int iImgFormat = 0; iImgFormat < 4; iImgFormat++ )
	{
		if ( iImgFormat == 0 ) pExt = ".png";
		if ( iImgFormat == 1 ) pExt = ".dds";
		if ( iImgFormat == 2 ) pExt = ".tga";
		if ( iImgFormat == 3 ) pExt = ".jpg";
		if ( strnicmp ( pRemoveTextureFile+strlen(pRemoveTextureFile)-strlen(pExt), pExt, strlen(pExt) ) == NULL ) 
			break;
	}
	if ( pExt == NULL ) return;

	// get base filename extension (deduct color specifier)
	LPSTR pImgType = NULL;
	cstr pBaseNoFileExt = cstr(Left(pRemoveTextureFile,Len(pRemoveTextureFile)-Len(pExt)));
	for ( int iImgType = 0; iImgType < 5; iImgType++ )
	{
		// find kind of 'color' image type
		if ( iImgType == 0 ) pImgType = "_diffuse";
		if ( iImgType == 1 ) pImgType = "_color";
		if ( iImgType == 2 ) pImgType = "_d";
		if ( iImgType == 3 ) pImgType = "_albedo";
		if ( strnicmp ( pBaseNoFileExt.Get()+strlen(pBaseNoFileExt.Get())-strlen(pImgType), pImgType, strlen(pImgType) ) == NULL ) 
		{
			// get base filename (minus image type)
			cstr pBaseFile = cstr(Left(pBaseNoFileExt.Get(),Len(pBaseNoFileExt.Get())-Len(pImgType)));

			// locate color image from image list
			importer_findandremoveentry( pBaseFile + cstr("_normal") + cstr(pExt) );
			importer_findandremoveentry( pBaseFile + cstr("_specular") + cstr(pExt) );
			importer_findandremoveentry( pBaseFile + cstr("_metalness") + cstr(pExt) );
			importer_findandremoveentry( pBaseFile + cstr("_gloss") + cstr(pExt) );
			importer_findandremoveentry( pBaseFile + cstr("_ao") + cstr(pExt) );
			importer_findandremoveentry( pBaseFile + cstr("_height") + cstr(pExt) );
		}
	}
}

void importer_applyimagelisttextures ( bool bCubeMapOnly, int iOptionalOnlyUpdateImageListIndex, bool bExpandOutPBRTextureSet )
{
	// either update cube map only (for model that already has its texture stages intact)
	// work out object texture stages (based on shader chosen)
	int iColorStage = 0;
	int iAOStage = 1;
	int iNormalStage = 2;
	int iSpecularStage = 3;
	int iGlossStage = 4;
	int iHeightStage = -1;
	int iEnvStage = 6;
	#ifdef VRTECH
	int iIllumStage = 7;
	#endif
	int iImageIndexForCUBE = 72543;
	cstr pShaderCUBE = "effectbank\\reloaded\\media\\CUBE.dds";
	LoadImage ( pShaderCUBE.Get(), iImageIndexForCUBE, 2 );
	PerformCheckListForLimbs ( t.importer.objectnumber );
	int iTextureCount = 0;
	for ( int tCount = 0 ; tCount <= ChecklistQuantity()-1; tCount++ )
	{
		LPSTR sTmp = LimbTextureName(t.importer.objectnumber, tCount);
		cstr pLimbTextureName = importer_getfilenameonly (sTmp);
		if (sTmp) delete[] sTmp;
		if ( strlen ( pLimbTextureName.Get() ) > 0 )
		{
			// new FPE field to specify limbs that are hair (i.e. no zwrite, no culling)
			cstr pBaseFile = Left ( pLimbTextureName.Get(), strlen(pLimbTextureName.Get())-4 );
			if ( strnicmp ( pBaseFile.Get() + strlen(pBaseFile.Get()) - 6, "_color", 6 ) == NULL )
			{
				pBaseFile = Left ( pBaseFile.Get(), strlen(pBaseFile.Get())-6 );
				if ( strnicmp ( pBaseFile.Get() + strlen(pBaseFile.Get()) - 5, "_hair", 5 ) == NULL )
				{
					// switch off culling (leave zwrite as distant hair rendered over nearer hair)
					SetLimbCull ( t.importer.objectnumber, tCount, false );
				}
			}

			// apply cube map regardless
			TextureLimbStage ( t.importer.objectnumber, tCount, iEnvStage, iImageIndexForCUBE );

			// also apply default textures where texture slot does not have image
			sObject* pObject = GetObjectData ( t.importer.objectnumber );
			if ( pObject )
			{
				if ( tCount <= pObject->iFrameCount )
				{
					sFrame* pFrame = pObject->ppFrameList[tCount];
					if ( pFrame->pMesh )
					{
						sMesh* pMesh = pFrame->pMesh;
						int iTextureMax = pMesh->dwTextureCount;
						#ifdef VRTECH
						if ( iTextureMax > 4 ) iTextureMax = 7;
						#else
						if ( iTextureMax > 4 ) iTextureMax = 4;
						#endif
						for ( int iTexture = 1; iTexture <= iTextureMax; iTexture++ )
						{
							if ( pMesh->pTextures[iTexture].iImageID == 0 )
							{
								int iDefaultImage = 0;
								if ( iTexture == iAOStage ) iDefaultImage = loadinternaltextureex("effectbank\\reloaded\\media\\blank_O.dds",1,t.tfullorhalfdivide);
								if ( iTexture == iNormalStage ) iDefaultImage = loadinternaltextureex("effectbank\\reloaded\\media\\blank_N.dds",1,t.tfullorhalfdivide);
								if ( iTexture == iSpecularStage ) iDefaultImage = loadinternaltextureex("effectbank\\reloaded\\media\\blank_black.dds",1,t.tfullorhalfdivide);
								if ( iTexture == iGlossStage ) iDefaultImage = loadinternaltextureex("effectbank\\reloaded\\media\\white_D.dds",1,t.tfullorhalfdivide);
								#ifdef VRTECH
								if ( iTexture == iIllumStage) iDefaultImage = loadinternaltextureex("effectbank\\reloaded\\media\\detail_default.dds", 1, t.tfullorhalfdivide);
								if(iTexture != 5 && iTexture != 6)
								#endif
									TextureLimbStage ( t.importer.objectnumber, tCount, iTexture, iDefaultImage );
							}
						}
					}
				}
			}

			// count textures specified in model
			iTextureCount++;
		}
	}

	// should map new texture choices to the original image slots
	if ( g_piMapImageListIndexToLimbIndex == NULL )
	{
		bool bAbsolutelyNoTexturesReferencedAnywhere = true;
		g_dwMapImageListIndexToLimbCount = ChecklistQuantity(); 
		g_piMapImageListIndexToLimbIndex = new int[g_dwMapImageListIndexToLimbCount];
		for ( int tLimbIndex = 0 ; tLimbIndex <= ChecklistQuantity()-1; tLimbIndex++ )
		{
			int iFindImageSlotIndex = -1;
			LPSTR sTmp = LimbTextureName(t.importer.objectnumber, tLimbIndex);
			cstr pLimbTextureName = importer_getfilenameonly (sTmp);
			if (sTmp) delete[] sTmp;
			LPSTR pSearch = pLimbTextureName.Get();
			if ( strlen ( pSearch ) > 0 )
			{
				iFindImageSlotIndex = 1; // 220618 - default to slot one if no specific match can be made (so model CAN be textured)
				bAbsolutelyNoTexturesReferencedAnywhere = false;
				for ( int tCount = 1; tCount <= t.tcounttextures; tCount++ )
				{
					LPSTR pThisImageItem = t.importerTextures[tCount].fileName.Get();
					if ( strnicmp ( pThisImageItem + strlen(pThisImageItem) - strlen(pSearch), pSearch, strlen(pSearch)-4 ) == NULL )
					{
						iFindImageSlotIndex = tCount;
					}
				}
			}
			g_piMapImageListIndexToLimbIndex [ tLimbIndex ] = iFindImageSlotIndex;
		}
		if ( bAbsolutelyNoTexturesReferencedAnywhere == true )
		{
			// okay, so now we know that no meshes are particular about their texture, 
			// we will use slot one for all textures on this model
			int iFindImageSlotIndex = 1;
			sObject* pObject = GetObjectData ( t.importer.objectnumber );
			if ( pObject )
			{
				for ( int tLimbIndex = 0 ; tLimbIndex <= ChecklistQuantity()-1; tLimbIndex++ )
				{
					sFrame* pFrame = pObject->ppFrameList[tLimbIndex];
					if ( pFrame )
					{
						if ( pFrame->pMesh )
						{
							g_piMapImageListIndexToLimbIndex [ tLimbIndex ] = iFindImageSlotIndex;
						}
					}
				}
			}
		}
	}

	// or full retexture model from imagelist
	if ( bCubeMapOnly == false )
	{
		/*
		// work out if object is single or multi-texture
		int iTextureCount = 0;
		char pStoreTextureNames[50][512];
		memset ( pStoreTextureNames, 0, sizeof(pStoreTextureNames) );
		PerformCheckListForLimbs ( t.importer.objectnumber );
		for ( int tCount = 0 ; tCount <= ChecklistQuantity()-1; tCount++ )
		{
			cstr pLimbTextureName = importer_getfilenameonly ( LimbTextureName ( t.importer.objectnumber, tCount ) );
			if ( strlen ( pLimbTextureName.Get() ) > 0 )
			{
				// only add/count if unique
				bool bTexNameUnique = true;
				for ( int iScan = 0; iScan < iTextureCount; iScan++ )
				{
					if ( stricmp ( pStoreTextureNames[iScan], pLimbTextureName.Get() ) == NULL )
					{
						bTexNameUnique = false; 
						break;
					}
				}

				// add to list of texture names found in object limbs
				if ( bTexNameUnique == true )
				{
					if ( iTextureCount < 50 )
					{
						strcpy ( pStoreTextureNames[iTextureCount], pLimbTextureName.Get() );
						iTextureCount++;
					}
				}
			}
		}
		*/

		/*
		// single or multi texture
		if ( iTextureCount <= 1 )
		{
			// SINGLE - assign texture to model from slot one 
			TextureObject (  t.importer.objectnumber, t.importerTextures[1].imageID );
			for ( int tCount = 0 ; tCount <= ChecklistQuantity()-1; tCount++ )
			{
				TextureLimb ( t.importer.objectnumber, tCount, t.importerTextures[1].imageID );
				int iImgColor=0, iImgNormal=0, iImgSpecular=0, iImgGloss=0, iImgAO=0, iImgHeight=0;
				importer_findimagetypesfromlist ( t.importerTextures[1].fileName, &iImgColor, &iImgNormal, &iImgSpecular, &iImgGloss, &iImgAO, &iImgHeight );
				if ( iImgColor > 0 ) TextureLimbStage ( t.importer.objectnumber, tCount, iColorStage, iImgColor );
				if ( iImgNormal > 0 ) TextureLimbStage ( t.importer.objectnumber, tCount, iNormalStage, iImgNormal );
				if ( iImgSpecular > 0 ) TextureLimbStage ( t.importer.objectnumber, tCount, iSpecularStage, iImgSpecular );
				if ( iImgGloss > 0 ) TextureLimbStage ( t.importer.objectnumber, tCount, iGlossStage, iImgGloss );
				if ( iImgAO > 0 && iAOStage != - 1 ) TextureLimbStage ( t.importer.objectnumber, tCount, iAOStage, iImgAO );
				if ( iImgHeight > 0 && iHeightStage != - 1 ) TextureLimbStage ( t.importer.objectnumber, tCount, iHeightStage, iImgHeight );
				TextureLimbStage ( t.importer.objectnumber, tCount, iEnvStage, iImageIndexForCUBE );
			}
		}
		else
		{
		*/

		// texture stage specific non-base (normal, ao, etc)
		int iOptionalStage = 0;
		if ( iOptionalOnlyUpdateImageListIndex > 0 ) iOptionalStage = t.importerTextures[iOptionalOnlyUpdateImageListIndex].iOptionalStage;
		if ( iOptionalStage > 0 )
		{
			int iAssociatedBaseImage = t.importerTextures[iOptionalOnlyUpdateImageListIndex].iAssociatedBaseImage;
			for ( int tLimbIndex = 0 ; tLimbIndex <= ChecklistQuantity()-1; tLimbIndex++ )
			{
				int iImageListIndex = g_piMapImageListIndexToLimbIndex [ tLimbIndex ];
				if ( iImageListIndex > 0 && t.importerTextures[iImageListIndex].iAssociatedBaseImage > 0 ) iImageListIndex = t.importerTextures[iImageListIndex].iAssociatedBaseImage;
				if ( iImageListIndex > 0 && iAssociatedBaseImage == iImageListIndex )
				{
					if ( iOptionalStage == 2 ) TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iNormalStage, t.importerTextures[iOptionalOnlyUpdateImageListIndex].imageID );
					if ( iOptionalStage == 3 ) TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iSpecularStage, t.importerTextures[iOptionalOnlyUpdateImageListIndex].imageID );
					if ( iOptionalStage == 4 ) TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iGlossStage, t.importerTextures[iOptionalOnlyUpdateImageListIndex].imageID );
					if ( iOptionalStage == 1 ) TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iAOStage, t.importerTextures[iOptionalOnlyUpdateImageListIndex].imageID );
				}
			}
		}
		else
		{
			// texture all model or just one limb
			for ( int tLimbIndex = 0 ; tLimbIndex <= ChecklistQuantity()-1; tLimbIndex++ )
			{
				int iImageListIndex = g_piMapImageListIndexToLimbIndex [ tLimbIndex ];
				if ( iImageListIndex > 0 && (iOptionalOnlyUpdateImageListIndex == -1 || iOptionalOnlyUpdateImageListIndex == iImageListIndex ))
				{
					cstr pLimbTextureName = importer_getfilenameonly ( t.importerTextures[iImageListIndex].fileName.Get() );
					if ( strlen ( pLimbTextureName.Get() ) > 0 )
					{
						if ( t.importerTextures[iImageListIndex].imageID > 0 )
						{
							// diffuse/albedo
							TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iColorStage, t.importerTextures[iImageListIndex].imageID );

							// find and load any associated PBR textures for this color texture choice
							if ( bExpandOutPBRTextureSet == true )
							{
								// only find every texture on initial texture load, not when replacing specific texture slots
								int iImgColor=0, iImgNormal=0, iImgSpecular=0, iImgGloss=0, iImgAO=0, iImgHeight=0;
								importer_findimagetypesfromlist ( t.importerTextures[iImageListIndex].fileName, iImageListIndex, &iImgColor, &iImgNormal, &iImgSpecular, &iImgGloss, &iImgAO, &iImgHeight );
								if ( iImgColor > 0 ) TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iColorStage, iImgColor );
								if ( iImgNormal > 0 ) TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iNormalStage, iImgNormal );
								if ( iImgSpecular > 0 ) TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iSpecularStage, iImgSpecular );
								if ( iImgGloss > 0 ) TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iGlossStage, iImgGloss );
								if ( iAOStage != - 1 && iImgAO > 0 ) TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iAOStage, iImgAO );
								if ( iImgHeight > 0 && iHeightStage != - 1 ) TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iHeightStage, iImgHeight );
							}

							/*
							// work out base filename by removing known albedo extensions
							cstr pBaseFile = Left ( pLimbTextureName.Get(), strlen(pLimbTextureName.Get())-4 );
							if ( strnicmp ( pBaseFile.Get() + strlen(pBaseFile.Get()) - 2, "_d", 2 ) == NULL )
							{
								pBaseFile = Left ( pBaseFile.Get(), strlen(pBaseFile.Get())-2 );
							}
							else if ( strnicmp ( pBaseFile.Get() + strlen(pBaseFile.Get()) - 6, "_color", 6 ) == NULL )
							{
								pBaseFile = Left ( pBaseFile.Get(), strlen(pBaseFile.Get())-6 );
							}
							else if ( strnicmp ( pBaseFile.Get() + strlen(pBaseFile.Get()) - 8, "_diffuse", 8 ) == NULL )
							{
								pBaseFile = Left ( pBaseFile.Get(), strlen(pBaseFile.Get())-8 );
							}
							if ( strnicmp ( pBaseFile.Get() + strlen(pBaseFile.Get()) - 5, "_hair", 5 ) == NULL )
							{
								// switch off culling (leave zwrite as distant hair rendered over nearer hair)
								SetLimbCull ( t.importer.objectnumber, tLimbIndex, false );
							}

							// detect normal
							cstr pNormalFile = pBaseFile + cstr("normal.png");
							int iImageIndexForNormal = importer_findtextureinlist ( pNormalFile.Get() );
							TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iNormalStage, t.importerTextures[iImageIndexForNormal].imageID );

							// detect specular or metalness
							cstr pSpecularFile = pBaseFile + cstr("specular.png");
							int iImageIndexForSpecular = importer_findtextureinlist ( pSpecularFile.Get() );
							if ( iImageIndexForSpecular == 0 )
							{
								pSpecularFile = pBaseFile + cstr("metalness.png");
								iImageIndexForSpecular = importer_findtextureinlist ( pSpecularFile.Get() );
								TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iSpecularStage, t.importerTextures[iImageIndexForSpecular].imageID );
							}
							TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iSpecularStage, t.importerTextures[iImageIndexForSpecular].imageID );

							// delect gloss
							cstr pGlossFile = pBaseFile + cstr("gloss.png");
							int iImageIndexForGloss = importer_findtextureinlist ( pGlossFile.Get() );
							TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iGlossStage, t.importerTextures[iImageIndexForGloss].imageID );
							*/

							// apply environment cube map
							TextureLimbStage ( t.importer.objectnumber, tLimbIndex, iEnvStage, iImageIndexForCUBE );
						}
					}
				}
			}
			//}
			/*
			for ( int tCount = 0 ; tCount <= ChecklistQuantity()-1; tCount++ )
			{
				cstr pLimbTextureName = importer_getfilenameonly ( LimbTextureName ( t.importer.objectnumber, tCount ) );
				if ( strlen ( pLimbTextureName.Get() ) > 0 )
				{
					for ( int iImageListIndex = 0; iImageListIndex < IMPORTERTEXTURESMAX; iImageListIndex++ )
					{
						if ( t.importerTextures[iImageListIndex].imageID > 0 )
						{
							cstr pCompareWith = importer_getfilenameonly ( t.importerTextures[iImageListIndex].fileName.Get() );
							if ( strnicmp ( pCompareWith.Get(), pLimbTextureName.Get(), strlen(pCompareWith.Get()) ) == NULL )
							{
								// diffuse/albedo
								TextureLimbStage ( t.importer.objectnumber, tCount, iColorStage, t.importerTextures[iImageListIndex].imageID );

								// work out base filename by removing known albedo extensions
								cstr pBaseFile = Left ( pLimbTextureName.Get(), strlen(pLimbTextureName.Get())-4 );
								if ( strnicmp ( pBaseFile.Get() + strlen(pBaseFile.Get()) - 2, "_d", 2 ) == NULL )
								{
									pBaseFile = Left ( pBaseFile.Get(), strlen(pBaseFile.Get())-2 );
								}
								else if ( strnicmp ( pBaseFile.Get() + strlen(pBaseFile.Get()) - 6, "_color", 6 ) == NULL )
								{
									pBaseFile = Left ( pBaseFile.Get(), strlen(pBaseFile.Get())-6 );
								}
								else if ( strnicmp ( pBaseFile.Get() + strlen(pBaseFile.Get()) - 8, "_diffuse", 8 ) == NULL )
								{
									pBaseFile = Left ( pBaseFile.Get(), strlen(pBaseFile.Get())-8 );
								}
								if ( strnicmp ( pBaseFile.Get() + strlen(pBaseFile.Get()) - 5, "_hair", 5 ) == NULL )
								{
									// switch off culling (leave zwrite as distant hair rendered over nearer hair)
									SetLimbCull ( t.importer.objectnumber, tCount, false );
								}

								// detect normal
								cstr pNormalFile = pBaseFile + cstr("normal.png");
								int iImageIndexForNormal = importer_findtextureinlist ( pNormalFile.Get() );
								TextureLimbStage ( t.importer.objectnumber, tCount, iNormalStage, t.importerTextures[iImageIndexForNormal].imageID );

								// detect specular or metalness
								cstr pSpecularFile = pBaseFile + cstr("specular.png");
								int iImageIndexForSpecular = importer_findtextureinlist ( pSpecularFile.Get() );
								if ( iImageIndexForSpecular == 0 )
								{
									pSpecularFile = pBaseFile + cstr("metalness.png");
									iImageIndexForSpecular = importer_findtextureinlist ( pSpecularFile.Get() );
									TextureLimbStage ( t.importer.objectnumber, tCount, iSpecularStage, t.importerTextures[iImageIndexForSpecular].imageID );
								}
								TextureLimbStage ( t.importer.objectnumber, tCount, iSpecularStage, t.importerTextures[iImageIndexForSpecular].imageID );

								// delect gloss
								cstr pGlossFile = pBaseFile + cstr("gloss.png");
								int iImageIndexForGloss = importer_findtextureinlist ( pGlossFile.Get() );
								TextureLimbStage ( t.importer.objectnumber, tCount, iGlossStage, t.importerTextures[iImageIndexForGloss].imageID );

								// detect CUBE
								TextureLimbStage ( t.importer.objectnumber, tCount, iEnvStage, iImageIndexForCUBE );
							}
						}
					}
				}
			}
			*/
		}
	}
}

void importer_assignsprite ( int tCount )
{
	if (bRemoveSprites)
		return;
	t.tSpriteID = 50;
	while (  SpriteExist(t.tSpriteID) == 1 ) ++t.tSpriteID;
	t.importerTextures[tCount].spriteID = t.tSpriteID+1;
	t.importerTextures[tCount].spriteID2 = t.tSpriteID;
	Sprite ( t.importerTextures[tCount].spriteID, -10000, -10000, g.importermenuimageoffset+6 );
	Sprite ( t.importerTextures[tCount].spriteID2, -10000, -10000, g.importermenuimageoffset+6 );
}

void importer_recreate_texturesprites ( void )
{
	if (bRemoveSprites)
		return;
	// Create sprite to represent texture 
	t.tSpriteID = 50;
	for ( int tCount = 1; tCount <= t.tcounttextures; tCount++ )
	{
		if ( SpriteExist ( t.tSpriteID+0 ) == 1 ) DeleteSprite ( t.tSpriteID+0 );
		if ( SpriteExist ( t.tSpriteID+1 ) == 1 ) DeleteSprite ( t.tSpriteID+1 );
		importer_assignsprite ( tCount );
		t.tSpriteID+=2;
	}
	for ( int tCount = t.tcounttextures+1; tCount < IMPORTERTEXTURESMAX; tCount++ )
	{
		int iSprite = t.importerTextures[tCount].spriteID;
		int iSprite2 = t.importerTextures[tCount].spriteID2;
		if ( iSprite > 0 && SpriteExist ( iSprite ) == 1 ) DeleteSprite ( iSprite );
		if ( iSprite2 > 0 && SpriteExist ( iSprite2 ) == 1 ) DeleteSprite ( iSprite2 );
	}
}

int giRememberLastEffectIndexInImporter = -1;

void importer_changeshader ( LPSTR pNewShaderFilename )
{
	// when shader changed in importer dialog, change shader of imported model
	if ( t.importer.objectnumber > 0 )
	{
		if ( ObjectExist ( t.importer.objectnumber ) == 1 )
		{
			char pRelativeEffectPath[512];
			strcpy ( pRelativeEffectPath, "effectbank\\reloaded\\" );
			strcat ( pRelativeEffectPath, pNewShaderFilename );
			if ( giRememberLastEffectIndexInImporter > 0 ) deleteinternaleffect ( giRememberLastEffectIndexInImporter );
			int iEffectID = loadinternaleffectunique ( pRelativeEffectPath, 1 ); //PE: old effect never deleted. ?
			DeleteObject ( t.importer.objectnumber );
			CloneObject ( t.importer.objectnumber, t.importer.objectnumberpreeffectcopy );
			//ReverseObjectFrames ( t.importer.objectnumber ); // hair rendered last
			importer_applyimagelisttextures ( true, -1, false );
			importer_recreate_texturesprites();
			LockObjectOn ( t.importer.objectnumber );

			//DisableObjectZRead ( t.importer.objectnumber ); // messes up import preview!
			//DisableObjectZDepth ( t.importer.objectnumber );
			SetObjectEffect ( t.importer.objectnumber, iEffectID ); 
			//SetEffectTechnique ( iEffectID, "LowestWithCutOutDepth" ); // messes up import preview!
			//SetObjectTransparency ( t.importer.objectnumber, 6 );

			GlueObjectToLimbEx ( t.importer.objectnumber, t.importerGridObject[8], 0 , 1 );
			giRememberLastEffectIndexInImporter = iEffectID;
			//PE: Bug. reset effect clip , so visible.
			t.tnothing = MakeVector4(g.characterkitvector);
			SetVector4(g.characterkitvector, 500000, 1, 0, 0);
			SetEffectConstantV(iEffectID, "EntityEffectControl", g.characterkitvector);
			t.tnothing = DeleteVector4(g.characterkitvector);
		}
	}

	#ifdef VRTECH
	//Reset colors and update.
	visuals_editordefaults();
	t.visuals.SurfaceSunFactor_f = 0.75;
	t.visuals.AmbienceIntensity_f = 195.0f;
	t.visuals.SurfaceIntensity_f = 0.9f;
	t.visuals.refreshshaders = 1;
	visuals_loop();
	#endif
}

#ifdef WICKEDENGINE
bool animsystem_buildanimslots(int objectnumber)
{
	// ensure we have animations, and add them to list
	sObject* pObject = GetObjectData(objectnumber);
	g_pAnimSlotList.clear();
	int iObjectFramesRunningTotal = 0;
	bFoundanimSet = false;
	if (pObject->pAnimationSet)
	{
		sAnimationSet* pAnimSet = pObject->pAnimationSet;
		while (pAnimSet != NULL)
		{
			// create anim slot from anim set
			// only list animsets that have animations (Take001 can have zero anims sometimes)
			sAnimSlotStruct animslotitem;
			animslotitem.fStep1 = 0;
			animslotitem.fStep2 = 0;
			animslotitem.fStep3 = 0;
			strcpy(animslotitem.pName, pAnimSet->szName);
			if (strlen(animslotitem.pName) == 0) strcpy(animslotitem.pName, "Included");
			animslotitem.fStart = iObjectFramesRunningTotal;
			animslotitem.fFinish = iObjectFramesRunningTotal + pAnimSet->ulLength;
			if (pAnimSet->dwAnimSetType == 124)
			{
				// when triggered, can copy start/finish references on reloading
				animslotitem.fStart = pAnimSet->fAnimSetStart;
				animslotitem.fFinish = pAnimSet->fAnimSetFinish;
				animslotitem.fStep1 = pAnimSet->fAnimSetStep1;
				animslotitem.fStep2 = pAnimSet->fAnimSetStep2;
				animslotitem.fStep3 = pAnimSet->fAnimSetStep3;
			}
			animslotitem.bLooped = true;
			g_pAnimSlotList.push_back(animslotitem);
			bFoundanimSet = true;
			// total all frames in all animsets
			iObjectFramesRunningTotal += pAnimSet->ulLength + 1;
			// next animation set
			pAnimSet = pAnimSet->pNext;
		}
	}
	return bFoundanimSet;
}

void animsystem_prepareobjectforanimtool(int objectnumber, int iNotUsed)
{
	// get object ptr
	sObject* pObject = GetObjectData(objectnumber);

	// ensure new animation list is created from animation data
	g_pAnimSlotList.clear();

	// ensure we have animations, and add them to list
	int iObjectFramesRunningTotal = 0;
	bFoundanimSet = false;
	if (pObject->pAnimationSet)
	{
		sAnimationSet* pAnimSet = pObject->pAnimationSet;
		while (pAnimSet != NULL)
		{
			// create anim slot from anim set
			if (1) // LB: We NEED anim slots of zero length as we can have start/finish slots with same frame value (for on/off anims) - pAnimSet->ulLength > 0)
			{
				// only list animsets that have animations (Take001 can have zero anims sometimes)
				sAnimSlotStruct animslotitem;
				animslotitem.fStep1 = 0;
				animslotitem.fStep2 = 0;
				animslotitem.fStep3 = 0;
				strcpy(animslotitem.pName, pAnimSet->szName);
				if (strlen(animslotitem.pName) == 0) strcpy(animslotitem.pName, "Included");
				animslotitem.fStart = iObjectFramesRunningTotal;
				animslotitem.fFinish = iObjectFramesRunningTotal + pAnimSet->ulLength;
				if (pAnimSet->dwAnimSetType == 124)
				{
					// when triggered, can copy start/finish references on reloading
					animslotitem.fStart = pAnimSet->fAnimSetStart;
					animslotitem.fFinish = pAnimSet->fAnimSetFinish;
					animslotitem.fStep1 = pAnimSet->fAnimSetStep1;
					animslotitem.fStep2 = pAnimSet->fAnimSetStep2;
					animslotitem.fStep3 = pAnimSet->fAnimSetStep3;
				}
				animslotitem.bLooped = true;
				g_pAnimSlotList.push_back(animslotitem);
				bFoundanimSet = true;

				// total all frames in all animsets
				iObjectFramesRunningTotal += pAnimSet->ulLength + 1;
			}
			// next animation set
			pAnimSet = pAnimSet->pNext;
		}
	}
	if (bFoundanimSet == true)
	{
		// now create "animset core zero" if one does not exist
		if (pObject->pAnimationSet->dwAnimSetType != 123)
		{
			// animset core zero holds ALL the separate animsets further down list
			// so that animation system can act on a single contiguous series of frame numbers
			sAnimationSet* pAnimCoreSet = new sAnimationSet();
			strcpy(pAnimCoreSet->szName, "All");
			pAnimCoreSet->dwAnimSetType = 123; // 123 = New AnimSet Core Zero (holds all other animset frames for single submission to wicked)
			pAnimCoreSet->ulLength = iObjectFramesRunningTotal;

			// first entry is the aminset core zero
			sAnimSlotStruct animslotitem;
			animslotitem.fStep1 = 0;
			animslotitem.fStep2 = 0;
			animslotitem.fStep3 = 0;
			strcpy(animslotitem.pName, pAnimCoreSet->szName);
			animslotitem.fStart = 0;
			animslotitem.fFinish = pAnimCoreSet->ulLength;
			animslotitem.bLooped = true;
			g_pAnimSlotList.insert(g_pAnimSlotList.begin(), animslotitem);

			// ensure we remove the wicked anim component which WAS linked to the first animset (which wicked uses for all animations on this object)
			if (pObject->pAnimationSet->wickedanimentityindex > 0)
			{
				wiScene::AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent( pObject->pAnimationSet->wickedanimentityindex );
				for( int i = 0; i < animationcomponent->samplers.size(); i++ )
				{
					wiScene::GetScene().Entity_Remove( animationcomponent->samplers[i].data );
				}

				wiScene::GetScene().Entity_Remove(pObject->pAnimationSet->wickedanimentityindex);
				pObject->pAnimationSet->wickedanimentityindex = 0;
			}

			// integrate animset core zero as new first in list
			pAnimCoreSet->pNext = pObject->pAnimationSet;
			pObject->pAnimationSet = pAnimCoreSet;

			// now create animation list for core zero, and include all animations in lists below this one
			iObjectFramesRunningTotal = 0;
			sAnimationSet* pAnimSet = pAnimCoreSet->pNext;
			while (pAnimSet != NULL)
			{
				// take the animations from this animset, and append to animset core zero
				if (pAnimSet->ulLength > 0)
				{
					sAnimation* pCurrent = pAnimSet->pAnimation;
					bool bAnimationAppended = AppendAnimationDataCore(pCurrent, &pAnimCoreSet, iObjectFramesRunningTotal, false);
					iObjectFramesRunningTotal += pAnimSet->ulLength + 1;
				}
				pAnimSet = pAnimSet->pNext;
			}

			// cleanup by removing slots that are called $NoName$
			for (int i = 0; i < g_pAnimSlotList.size(); i++)
			{
				if (strcmp (g_pAnimSlotList[i].pName, "$NoName$") == NULL)
				{
					g_pAnimSlotList.erase(g_pAnimSlotList.begin() + i);
					i = 0;
				}
			}
		}

		// recalculate animation data and bounds for object
		MapFramesToAnimations(pObject, true);

		// Create animations as they come from animset core zero only
		WickedCall_RefreshObjectAnimations(pObject, pObject->wickedloaderstateptr);

		// trigger animation zero to play
		extern bool g_bCharacterCreatorPlusActivated;

		g_bAnimatingObjectPreview = true;
		g_bUpdateAnimationPreview = true;

		// prefer an idle animation as starting anim if one exists
		g_iCurrentAnimationSlotIndex = 0;

		if (g_bCharacterCreatorPlusActivated)
		{
			for (int i = 0; i < g_pAnimSlotList.size(); i++)
			{
				if (strstr(g_pAnimSlotList[i].pName, "Idle_LookAround"))
				{
					g_iCurrentAnimationSlotIndex = i;
					break;
				}

				if (i == g_pAnimSlotList.size() - 1)
				{
					// Couldn't find LookAround, search for normal anims.
					for (int i = 0; i < g_pAnimSlotList.size(); i++)
					{
						if (stricmp(g_pAnimSlotList[i].pName, "idle") == NULL || stricmp(g_pAnimSlotList[i].pName, "Zombie_Idle") == NULL)
						{
							g_iCurrentAnimationSlotIndex = i;
							break;
						}
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < g_pAnimSlotList.size(); i++)
			{
				// ZJ: Altered to also include zombie idle.
				if (stricmp(g_pAnimSlotList[i].pName, "idle") == NULL || stricmp(g_pAnimSlotList[i].pName, "Zombie_Idle") == NULL)
				{
					g_iCurrentAnimationSlotIndex = i;
					break;
				}
			}
		}
	}

	// show debug bones to help visualise what we are importing
	wiRenderer::SetToDrawDebugBoneLines(g_bShowBones);
}

void importer_loadmodel_wicked(void)
{
	// free any previous import object
	if (ObjectExist(t.importer.objectnumber)) importer_quit();
	if (GetImageExistEx(IMPORTER_TMP_IMAGE)) DeleteImage(IMPORTER_TMP_IMAGE);

	// initialise if not already inited
	if (t.importer.importerActive == 0)
	{
		importer_init();
	}

	g_MeshNamesAssimp.clear();

	// detect if format in beta support?
	bool bIsBetaFormat = true;
	if (t.timporterfile_s.Len() > 4) 
	{
		cStr sExt = Lower(Right(t.timporterfile_s.Get(), 4));
		if (sExt == ".fbx") bIsBetaFormat = false;
		if (sExt == ".obj") bIsBetaFormat = false;
		if (sExt == ".dbo") bIsBetaFormat = false;
		if (sExt == "gltf") bIsBetaFormat = false;
		sExt = Lower(Right(t.timporterfile_s.Get(), 2));
		if (sExt == ".x") bIsBetaFormat = false;
	}

	// separate path from model filename
	char pAbsPathAndFilename[MAX_PATH];
	strcpy ( pAbsPathAndFilename, t.timporterfile_s.Get() );
	for (int n = strlen(pAbsPathAndFilename); n > 0; n--)
	{
		if (pAbsPathAndFilename[n] == '\\' || pAbsPathAndFilename[n] == '/')
		{
			t.importer.objectFilename = pAbsPathAndFilename + n + 1;
			pAbsPathAndFilename[n+1] = 0;
			break;
		}
	}
	t.importer.objectFileOriginalPath = pAbsPathAndFilename;

	// work out FPE form and extension too
	char pFilename[MAX_PATH];
	strcpy(pFilename, t.importer.objectFilename.Get());
	// We don't replace underscores with space anymore
	//bool bFoundExtension = false;
	//for (int n = strlen(pFilename); n > 0; n--)
	//{
	//	// ZJ: subsituting underscores for space in the filename.
	//	if (pFilename[n] == '_')
	//	{
	//		pFilename[n] = ' ';
	//	}

	//	// Remove the file extension from the file name.
	//	if (pFilename[n] == '.' && !bFoundExtension)
	//	{
	//		t.importer.objectFilenameExtension = pFilename + n + 1;
	//		pFilename[n] = 0;
	//		bFoundExtension = true;
	//		// ZJ: removed break and added boolean so that the entire array can be traversed.
	//		//break;
	//	}
	//}
	for (int n = strlen(pFilename); n > 0; n--)
	{
		// Remove the file extension from the file name.
		if (pFilename[n] == '.')
		{
			t.importer.objectFilenameExtension = pFilename + n + 1;
			pFilename[n] = 0;
			break;
		}
	}
	t.importer.objectFilenameFPE = pFilename;
	t.importer.objectFilenameFPE += ".fpe";

	// pre-populate import name
	if (!t.importer.bQuitForReload)
	{
		strcpy(cImportName, pFilename);
	}
	t.importer.bQuitForReload = false;

	// importer overrides scaling during the load
	extern void SetLoadScale (enumScalingMode eScalingMode);
	enumScalingMode eThisScalingMode = (enumScalingMode)t.importer.lastscalingmodeused;
	SetLoadScale (eThisScalingMode);
	
	// load the imported object (powered by AssImp when not loading a DBO file)
	t.importer.objectnumber=g.importermenuobjectoffset+1;
	t.strwork = t.importer.objectFileOriginalPath + t.importer.objectFilename;
	if ( FileExist ( t.strwork.Get() ) ) 
	{
		if ( ObjectExist(t.importer.objectnumber) == 1 ) DeleteObject ( t.importer.objectnumber );
		LoadObject ( t.strwork.Get() ,t.importer.objectnumber );
	}
	else
	{
		t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\"+t.importer.objectFilename;
		if ( FileExist( t.strwork.Get() ) ) 
		{
			if ( ObjectExist(t.importer.objectnumber) == 1 ) DeleteObject ( t.importer.objectnumber );
			LoadObject ( t.strwork.Get() ,t.importer.objectnumber );
		}
		else
		{
			if ( ObjectExist(t.importer.objectnumber) == 1 ) DeleteObject ( t.importer.objectnumber );
			MakeObjectCube ( t.importer.objectnumber, 100 );
		}
	}
	if ( ObjectExist ( t.importer.objectnumber ) == 0 )
	{
		if ( ObjectExist(t.importer.objectnumber) == 1 ) DeleteObject ( t.importer.objectnumber );
		MakeObjectCube ( t.importer.objectnumber, 100 );
	}

	// load scaling override off
	SetLoadScale (eScalingMode_Off);

	sObject* pImportedObject = GetObjectData(t.importer.objectnumber);
	if (pImportedObject->iMeshCount > 100)
	{
		strcpy(cTriggerMessage, "The imported model has over 100 meshes. GameGuru MAX only supports up to 100 mesh materials.");
		bTriggerMessage = true;
	}

	// LB: go through all frames of imported model
	bool bConvertedBoneNames = false;
	for (int iFrame = 0; iFrame < pImportedObject->iFrameCount; iFrame++)
	{
		sFrame* pFrame = pImportedObject->ppFrameList[iFrame];
		if (pFrame)
		{
			LPSTR pOldFrameName = pFrame->szName;
			if (pOldFrameName)
			{
				if (strlen(pOldFrameName) > 0)
				{
					// look for common bone names, and transform to GG standard skeleton name
					const char* pNewName = "";
					if (stricmp(pOldFrameName, "mixamorig_Hips") == NULL) pNewName = "Bip01_Pelvis";
					if (stricmp(pOldFrameName, "mixamorig_Spine") == NULL) pNewName = "Bip01_Spine";
					if (stricmp(pOldFrameName, "mixamorig_LeftUpLeg") == NULL) pNewName = "Bip01_L_Thigh";
					if (stricmp(pOldFrameName, "mixamorig_LeftLeg") == NULL) pNewName = "Bip01_L_Calf";
					if (stricmp(pOldFrameName, "mixamorig_LeftFoot") == NULL) pNewName = "Bip01_L_Foot";
					if (stricmp(pOldFrameName, "mixamorig_LeftToeBase") == NULL) pNewName = "Bip01_L_Toe0";
					if (stricmp(pOldFrameName, "mixamorig_RightUpLeg") == NULL) pNewName = "Bip01_R_Thigh";
					if (stricmp(pOldFrameName, "mixamorig_RightLeg") == NULL) pNewName = "Bip01_R_Calf";
					if (stricmp(pOldFrameName, "mixamorig_RightFoot") == NULL) pNewName = "Bip01_R_Foot";
					if (stricmp(pOldFrameName, "mixamorig_RightToeBase") == NULL) pNewName = "Bip01_R_Toe0";
					if (stricmp(pOldFrameName, "mixamorig_Spine1") == NULL) pNewName = "Bip01_Spine1";
					if (stricmp(pOldFrameName, "mixamorig_Spine2") == NULL) pNewName = "Bip01_Spine2";
					if (stricmp(pOldFrameName, "mixamorig_Neck") == NULL) pNewName = "Bip01_Neck";
					if (stricmp(pOldFrameName, "mixamorig_LeftShoulder") == NULL) pNewName = "Bip01_L_Clavicle";
					if (stricmp(pOldFrameName, "mixamorig_RightShoulder") == NULL) pNewName = "Bip01_R_Clavicle";
					if (stricmp(pOldFrameName, "mixamorig_Head") == NULL) pNewName = "Bip01_Head";
					if (stricmp(pOldFrameName, "mixamorig_HeadTop_End") == NULL) pNewName = "Bip01_HeadTop";
					if (stricmp(pOldFrameName, "mixamorig_LeftArm") == NULL) pNewName = "Bip01_L_UpperArm";
					if (stricmp(pOldFrameName, "mixamorig_LeftForeArm") == NULL) pNewName = "Bip01_L_Forearm";
					if (stricmp(pOldFrameName, "mixamorig_RightArm") == NULL) pNewName = "Bip01_R_UpperArm";
					if (stricmp(pOldFrameName, "mixamorig_RightForeArm") == NULL) pNewName = "Bip01_R_Forearm";
					if (stricmp(pOldFrameName, "mixamorig_LeftHand") == NULL) pNewName = "Bip01_L_Hand";
					if (stricmp(pOldFrameName, "mixamorig_RightHand") == NULL) pNewName = "Bip01_R_Hand";
					if (strstr(pOldFrameName, "mixamorig_LeftHandThumb1") > 0) pNewName = "Bip01_L_Finger0";
					if (strstr(pOldFrameName, "mixamorig_LeftHandThumb2") > 0) pNewName = "Bip01_L_Finger01";
					if (strstr(pOldFrameName, "mixamorig_LeftHandThumb3") > 0) pNewName = "Bip01_L_Finger02";
					if (strstr(pOldFrameName, "mixamorig_LeftHandThumb4") > 0) pNewName = "Bip01_L_Finger03";
					if (strstr(pOldFrameName, "mixamorig_LeftHandIndex1") > 0) pNewName = "Bip01_L_Finger1";
					if (strstr(pOldFrameName, "mixamorig_LeftHandIndex2") > 0) pNewName = "Bip01_L_Finger11";
					if (strstr(pOldFrameName, "mixamorig_LeftHandIndex3") > 0) pNewName = "Bip01_L_Finger12";
					if (strstr(pOldFrameName, "mixamorig_LeftHandIndex4") > 0) pNewName = "Bip01_L_Finger13";
					if (strstr(pOldFrameName, "mixamorig_LeftHandMiddle1") > 0) pNewName = "Bip01_L_Finger2";
					if (strstr(pOldFrameName, "mixamorig_LeftHandMiddle2") > 0) pNewName = "Bip01_L_Finger21";
					if (strstr(pOldFrameName, "mixamorig_LeftHandMiddle3") > 0) pNewName = "Bip01_L_Finger22";
					if (strstr(pOldFrameName, "mixamorig_LeftHandMiddle4") > 0) pNewName = "Bip01_L_Finger23";
					if (strstr(pOldFrameName, "mixamorig_LeftHandRing1") > 0) pNewName = "Bip01_L_Finger3";
					if (strstr(pOldFrameName, "mixamorig_LeftHandRing2") > 0) pNewName = "Bip01_L_Finger31";
					if (strstr(pOldFrameName, "mixamorig_LeftHandRing3") > 0) pNewName = "Bip01_L_Finger32";
					if (strstr(pOldFrameName, "mixamorig_LeftHandRing4") > 0) pNewName = "Bip01_L_Finger33";
					if (strstr(pOldFrameName, "mixamorig_LeftHandPinky1") > 0) pNewName = "Bip01_L_Finger4";
					if (strstr(pOldFrameName, "mixamorig_LeftHandPinky2") > 0) pNewName = "Bip01_L_Finger41";
					if (strstr(pOldFrameName, "mixamorig_LeftHandPinky3") > 0) pNewName = "Bip01_L_Finger42";
					if (strstr(pOldFrameName, "mixamorig_LeftHandPinky4") > 0) pNewName = "Bip01_L_Finger43";
					if (strstr(pOldFrameName, "mixamorig_RightHandThumb1") > 0) pNewName = "Bip01_R_Finger0";
					if (strstr(pOldFrameName, "mixamorig_RightHandThumb2") > 0) pNewName = "Bip01_R_Finger01";
					if (strstr(pOldFrameName, "mixamorig_RightHandThumb3") > 0) pNewName = "Bip01_R_Finger02";
					if (strstr(pOldFrameName, "mixamorig_RightHandThumb4") > 0) pNewName = "Bip01_R_Finger03";
					if (strstr(pOldFrameName, "mixamorig_RightHandIndex1") > 0) pNewName = "Bip01_R_Finger1";
					if (strstr(pOldFrameName, "mixamorig_RightHandIndex2") > 0) pNewName = "Bip01_R_Finger11";
					if (strstr(pOldFrameName, "mixamorig_RightHandIndex3") > 0) pNewName = "Bip01_R_Finger12";
					if (strstr(pOldFrameName, "mixamorig_RightHandIndex4") > 0) pNewName = "Bip01_R_Finger13";
					if (strstr(pOldFrameName, "mixamorig_RightHandMiddle1") > 0) pNewName = "Bip01_R_Finger2";
					if (strstr(pOldFrameName, "mixamorig_RightHandMiddle2") > 0) pNewName = "Bip01_R_Finger21";
					if (strstr(pOldFrameName, "mixamorig_RightHandMiddle3") > 0) pNewName = "Bip01_R_Finger22";
					if (strstr(pOldFrameName, "mixamorig_RightHandMiddle4") > 0) pNewName = "Bip01_R_Finger23";
					if (strstr(pOldFrameName, "mixamorig_RightHandRing1") > 0) pNewName = "Bip01_R_Finger3";
					if (strstr(pOldFrameName, "mixamorig_RightHandRing2") > 0) pNewName = "Bip01_R_Finger31";
					if (strstr(pOldFrameName, "mixamorig_RightHandRing3") > 0) pNewName = "Bip01_R_Finger32";
					if (strstr(pOldFrameName, "mixamorig_RightHandRing4") > 0) pNewName = "Bip01_R_Finger33";
					if (strstr(pOldFrameName, "mixamorig_RightHandPinky1") > 0) pNewName = "Bip01_R_Finger4";
					if (strstr(pOldFrameName, "mixamorig_RightHandPinky2") > 0) pNewName = "Bip01_R_Finger41";
					if (strstr(pOldFrameName, "mixamorig_RightHandPinky3") > 0) pNewName = "Bip01_R_Finger42";
					if (strstr(pOldFrameName, "mixamorig_RightHandPinky4") > 0) pNewName = "Bip01_R_Finger43";

					// also look for any spaces used in the bone name
					bool bFoundASpace = false;
					char constructNewName[MAX_PATH];
					strcpy(constructNewName, pOldFrameName);
					if (strlen(constructNewName) > 0)
					{
						for (int n = 0; n < strlen(constructNewName); n++)
						{
							if (constructNewName[n] == ' ')
							{
								constructNewName[n] = '_';
								bFoundASpace = true;
							}
						}
						if(bFoundASpace==true) pNewName = constructNewName;
					}

					// replace all instances of this name (if found)
					if (strlen(pNewName) > 0)
					{
						// rename animation reference name too
						if (pImportedObject->pAnimationSet)
						{
							sAnimation* pCurrent = pImportedObject->pAnimationSet->pAnimation;
							while (pCurrent)
							{
								if (pCurrent->szName)
								{
									if (stricmp(pCurrent->szName, pFrame->szName) == NULL)
									{
										strcpy(pCurrent->szName, pNewName);
										break;
									}
								}
								pCurrent = pCurrent->pNext;
							}
						}

						// also rename bone names within each mesh
						for (int iMesh = 0; iMesh < pImportedObject->iMeshCount; iMesh++)
						{
							sMesh* pMesh = pImportedObject->ppMeshList[iMesh];
							if (pMesh)
							{
								if (pMesh->pBones)
								{
									for (int iBone = 0; iBone < pMesh->dwBoneCount; iBone++)
									{
										if (pMesh->pBones[iBone].szName)
										{
											if (stricmp(pMesh->pBones[iBone].szName, pFrame->szName) == NULL)
											{
												strcpy(pMesh->pBones[iBone].szName, pNewName);
												break;
											}
										}
									}
								}
							}
						}

						// finally rename frame
						strcpy(pFrame->szName, pNewName);
						bConvertedBoneNames = true;
					}
				}
			}
		}
	}
	if (bConvertedBoneNames==true)
	{
		strcpy(cTriggerMessage, "The imported model had unconventional bone names (included spaces, etc), these have been converted to standard MAX rig bone names.");
		bTriggerMessage = true;
	}

	// if flagged, use real object col center to shift mesh data to centralize the object
	if ( t.importer.centermodelbyshiftingmesh == 1 )
	{
		sObject* pObject = GetObjectData(t.importer.objectnumber);
		GGVECTOR3 oldObjectCollisionCenter = GGVECTOR3(0,0,0);
		for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
		{
			GGVECTOR3 meshCollisionCenter = GGVECTOR3(0,0,0);
			sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
			if ( pMesh )
			{
				sOffsetMap offsetMap;
				GetFVFOffsetMapFixedForBones(pMesh, &offsetMap);
				for (unsigned int i = 0; i < pMesh->dwVertexCount; ++i)
				{
					float* fX = ((float*)pMesh->pVertexData + offsetMap.dwX + (offsetMap.dwSize * i));
					float* fY = ((float*)pMesh->pVertexData + offsetMap.dwY + (offsetMap.dwSize * i));
					float* fZ = ((float*)pMesh->pVertexData + offsetMap.dwZ + (offsetMap.dwSize * i));
					meshCollisionCenter.x += *fX;
					meshCollisionCenter.y += *fY;
					meshCollisionCenter.z += *fZ;
				}
				meshCollisionCenter /= pMesh->dwVertexCount;
			}
			oldObjectCollisionCenter += meshCollisionCenter;
		}
		oldObjectCollisionCenter /= pObject->iMeshCount;
		// now shift
		for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
		{
			sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
			if ( pMesh )
			{
				sOffsetMap offsetMap;
				GetFVFOffsetMapFixedForBones(pMesh, &offsetMap);
				for (unsigned int i = 0; i < pMesh->dwVertexCount; ++i)
				{
					float* fX = ((float*)pMesh->pVertexData + offsetMap.dwX + (offsetMap.dwSize * i));
					float* fY = ((float*)pMesh->pVertexData + offsetMap.dwY + (offsetMap.dwSize * i));
					float* fZ = ((float*)pMesh->pVertexData + offsetMap.dwZ + (offsetMap.dwSize * i));
					*fX -= oldObjectCollisionCenter.x;
					*fY -= oldObjectCollisionCenter.y;
					*fZ -= oldObjectCollisionCenter.z;
				}
			}
		}
		// trigger the model update in Wicked so can see new arrangement
		WickedCall_RemoveObject(pObject);
		WickedCall_AddObject( pObject );
		WickedCall_UpdateObject ( pObject );
		WickedCall_TextureObject ( pObject, NULL );
	}

	iImporterScale = 100;
	//ScaleObject(t.importer.objectnumber, iImporterScale*fImporterScaleMultiply, iImporterScale*fImporterScaleMultiply, iImporterScale*fImporterScaleMultiply);
	ScaleObject(t.importer.objectnumber, iImporterScale, iImporterScale, iImporterScale);

	sObject* pObject = GetObjectData(t.importer.objectnumber);
	uint64_t materialEntity = 0;

	cstr objectName = t.importer.objectFileOriginalPath + t.importer.objectFilename;

	if (stricmp(objectName.Get() + strlen(objectName.Get()) - 4, ".dbo") == 0 
		|| stricmp(objectName.Get() + strlen(objectName.Get()) - 2, ".x") == 0)
	{
		// Handle cases where dbo or .x files are missing textures.
		if (pObject)
		{
			bool bTexturesAlreadyExist = false;
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pObject->ppMeshList[0]->wickedmeshindex);
			if (mesh)
			{
				materialEntity = mesh->subsets[0].materialID;
				wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
				if (pObjectMaterial)
				{
					if (pObjectMaterial->textures[BASECOLORMAP].name.length() > 0)
					{
						bTexturesAlreadyExist = true;
					}
				}
			}

			if (!bTexturesAlreadyExist)
			{
				// No textures - check for fpe.
				cstr fpeFile = t.importer.objectFileOriginalPath + t.importer.objectFilenameFPE;
				char textureName[512] = { 0 };
				bool bFoundFPE = false;
				if (FileExist(fpeFile.Get()))
				{
					bFoundFPE = true;
				}
				else
				{
					// Try a different fpe name. Some classic assets use space instead of underscores and have a space before the number.
					char fpe[MAX_PATH];
					strcpy(fpe, t.importer.objectFilenameFPE.Get());

					// insert space before numbers.
					char numbers[8];
					int spaceIndex = strlen(fpe) - 6;
					strcpy(numbers, fpe + spaceIndex);
					fpe[spaceIndex] = ' ';
					fpe[spaceIndex + 1] = 0;
					strcpy(fpe + strlen(fpe), numbers);

					fpeFile = t.importer.objectFileOriginalPath + cstr(fpe);
					if (FileExist(fpeFile.Get()))
						bFoundFPE = true;
				}

				if (bFoundFPE)
				{
					if (FileOpen(1) == 1) CloseFile(1);

					OpenToRead(1, fpeFile.Get());
					while (FileEnd(1) == 0)
					{
						t.tline_s = ReadString(1);
						t.tcciStat_s = Lower(FirstToken(t.tline_s.Get(), " "));
						if (t.tcciStat_s == "textured" || t.tcciStat_s == "basecolormap0")
						{
							char* value = strstr(t.tline_s.Get(), "=");
							if (value)
							{
								// Found a texture name specified in the fpe.
								if (strncmp(value + 1, " ", 1) == 0)
									strcpy(textureName, value + 2);
								else
									strcpy(textureName, value + 1);
							}

							if(strlen(textureName) > 0)
								break;
						}
					}
					CloseFile(1);
				}

				// No FPE or no texture specified, so use the name of the model as the starting point.
				if (strlen(textureName) == 0)
				{
					strcpy(textureName, t.importer.objectFilenameFPE.Get());
					textureName[strlen(textureName) - 4] = 0;
					strcpy(textureName + strlen(textureName), "_color.dds");
				}

				// Use the basecolour texture name to try and find the rest of the textures.
				if (materialEntity > 0)
				{
					wiScene::MaterialComponent* pMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pMaterial)
					{
						cstr filename = t.importer.objectFileOriginalPath + cstr(textureName);
						if (FileExist(filename.Get()))
						{
							pMaterial->textures[BASECOLORMAP].name = std::string(filename.Get());
							pMaterial->textures[BASECOLORMAP].resource = WickedCall_LoadImage(pMaterial->textures[BASECOLORMAP].name);

							int legacyDNS = 0;
							if (stricmp(filename.Get() + filename.Len() - strlen("_d.dds"), "_d.dds") == 0)
								legacyDNS = 1;

							char texName[MAX_PATH];
							strcpy(texName, filename.Get());
							if(legacyDNS == 0)
								texName[strlen(texName) - strlen("color.dds")] = 0;
							else
								texName[strlen(texName) - strlen("d.dds")] = 0;

							for (int i = NORMALMAP; i <= EMISSIVEMAP; i++)
							{
								char textureToTry[MAX_PATH];
								strcpy(textureToTry, texName);
								if (i == NORMALMAP)
								{
									if (legacyDNS == 0)
										strcpy(textureToTry + strlen(textureToTry), "normal.dds");
									else
										strcpy(textureToTry + strlen(textureToTry), "n.dds");
								}
								else if (i == SURFACEMAP)
								{
									if (legacyDNS == 0)
									{
										strcpy(textureToTry + strlen(textureToTry), "surface.dds");
									}
									else
									{
										strcpy(textureToTry + strlen(textureToTry), "s.dds");
										if (FileExist(textureToTry))
										{
											char surfacePath[MAX_PATH];
											strcpy(surfacePath, GG_GetWritePath());
											strcat(surfacePath, "imported_models\\");
											for (int i = strlen(texName); i >= 0; i--)
											{
												if (texName[i] == '\\' || texName[i] == '/')
												{
													strcat(surfacePath, texName + i + 1);
													strcat(surfacePath, "surface.dds");
													ImageCreateSurfaceTexture(surfacePath, NULL, textureToTry, textureToTry);
													t.importer.pSurfaceFilesToDelete.push_back(surfacePath);
													textureToTry[0] = 0;
													strcpy(textureToTry, surfacePath);
													break;
												}
											}
										}
									}
								}
								else if (i == EMISSIVEMAP)
								{
									if (legacyDNS == 0)
										strcpy(textureToTry + strlen(textureToTry), "emissive.dds");
								}

								if (FileExist(textureToTry))
								{
									pMaterial->textures[i].name = std::string(textureToTry);
									pMaterial->textures[i].resource = WickedCall_LoadImage(pMaterial->textures[i].name);
								}
							}
						}
					}
				}
			}

			for (int i = 0; i < pObject->iMeshCount; i++)
			{
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pObject->ppMeshList[i]->wickedmeshindex);
				if (mesh)
				{
					materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						pObjectMaterial->SetBaseColor(XMFLOAT4(1, 1, 1, 1));
					}
				}
			}
		}
	}

	WickedCall_SetSelectedObject(pObject);

	// first populate WEMaterials for all meshes
	Wicked_Copy_Material_To_Grideleprof((void*)pObject, 0);

	// then select the first one to start off the importer
	Wicked_Set_Material_Defaults( (void*) pObject, 0);

	//oldtextimportpath = t.importer.objectFileOriginalPath;

	ShowObject(t.importer.objectnumber);

	if (bIsBetaFormat)
	{
		strcpy(cTriggerMessage, "The selected format is in dev mode.");
		bTriggerMessage = true;
	}

	// initialise animation sets for this model
	int iUseDefaultNonCombatAnimations = 0;
	animsystem_prepareobjectforanimtool(t.importer.objectnumber, iUseDefaultNonCombatAnimations);

	// reset rotation values
	fImportPosX = 0.0;
	fImportPosY = 0.0;
	fImportPosZ = 0.0;
	fImportRotX = 0.0;
	fImportRotY = 0.0;
	fImportRotZ = 0.0;

	// set some importer defaults
	strcpy(collision_combo_entry, "Box\0");
	t.importer.collisionshape = 0;
	t.importer.defaultstatic = 1;

	importer_load_scenery();

	//importer_find_floor(); //PE: Not default anymore.

	// Start importer loop
	t.importer.loaded = 1;
}
void importer_loadmodel_wicked(int objnumber)
{
	//PE: Need batch support here.
	importer_loadmodel_wicked();
}
#endif

#ifdef WICKEDENGINE
void importer_loadmodel(int objnumber)
{
	//PE: We need a object array so we can batch convert.

	// user prompt when loading model in
	popup_text("importing and converting model");

	// show loading prompt (FBX conversion can take a while)
	importer_loadmodel_wicked(objnumber);
}
#endif

void importer_loadmodel ( void )
{
	// user prompt when loading model in
	popup_text("importing and converting model");

	// show loading prompt (FBX conversion can take a while)
	#ifdef WICKEDENGINE
	// Wicked imports the object in-situ (so placed in center of current location in level scene)
	// Note: Will probably find floor if close enough, or in air if high above ground.
	// Using new load sequence as Wicked import is quite different from Classic/VRQ

	bool bRestoreData = t.importer.bQuitForReload;
	importer_loadmodel_wicked();

	if (bRestoreData)
		importer_restoreobjectdata();
	else
		importer_clearobjectdata();

	visualsdatastoragetype desiredVisuals;
	set_temp_visuals(t.editorvisuals, t.visualsStorage, desiredVisuals);
	set_temp_visuals(t.visuals, t.visualsStorage, desiredVisuals);
	
	//set_temp_visuals(t.gamevisuals, t.visualsStorage, desiredVisuals);
	//t.visuals.refreshshaders = 1;
	visuals_loop();
	#else
	#ifdef VRTECH
	// reposition camera in sky so no terrain depth clipping can occur
	if (g_bCameraInSkyForImporter == false)
	{
		custom_back_color[0] = 0.60f; custom_back_color[1] = 0.60f; custom_back_color[2] = 0.60f; custom_back_color[3] = 1.0f;
		PositionCamera(0, CameraPositionX(0), CameraPositionY(0) + 50000, CameraPositionZ(0));
		SetCameraRange(0, DEFAULT_NEAR_PLANE, DEFAULT_FAR_PLANE);
		g_bCameraInSkyForImporter = true;
		t.editorfreeflight.mode = 1;
		t.editorfreeflight.c.x_f = CameraPositionX();
		t.editorfreeflight.c.y_f = CameraPositionY();
		t.editorfreeflight.c.z_f = CameraPositionZ();
		Sync();
	}
	#endif

	// init
	#ifdef VRTECH
	if (ObjectExist(t.importer.objectnumber))
		importer_quit();
	#endif

	if ( t.importer.importerActive == 0 ) importer_init ( );
	importer_sort_names ( );
	for ( t.t = 0 ; t.t<=  50; t.t++ )
	{
		t.snapPosX_f[t.t][1] = -99999;
	}

	// check if importing an FBX for first time
	bool bHasFBXExtension = false;
	if ( strnicmp ( t.importer.objectFilename.Get() + strlen(t.importer.objectFilename.Get()) - 4, ".fbx", 4 ) == NULL ) bHasFBXExtension = true;
	if ( bHasFBXExtension == true && g_iFirstTimeFBXImport == 0 )
	{
		// set-up importer for FBX friendly import settings
		//g_iFBXGeometryToggleMode = 1;
		//g_iFBXGeometryCenterMesh = 1;
		g_iPreferPBR = 1;
		g_iFirstTimeFBXImport = 1;
	}

	//  load model specified by timporterfile$
	g_bLoadedFBXModel = false;
	t.importer.objectnumber=g.importermenuobjectoffset+1;

	t.importer.objectnumberpreeffectcopy=g.importermenuobjectoffset+3;
	t.strwork = ""; t.strwork = t.strwork + t.importer.objectFileOriginalPath + t.importer.objectFilename;
	if ( FileExist ( t.strwork.Get() ) ) 
	{
		if ( strnicmp ( t.strwork.Get() + strlen(t.strwork.Get()) - 4, ".fbx", 4 )==NULL )
		{
			LoadFBX ( t.strwork.Get(), t.importer.objectnumber );
			//SetObjectRenderMatrixMode ( t.importer.objectnumber, 1 );
			g_bLoadedFBXModel = true;
		}
		else
		{
			LoadObject ( t.strwork.Get() ,t.importer.objectnumber );
			//SetObjectRenderMatrixMode ( t.importer.objectnumber, 0 );
		}
	}
	else
	{
		t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\"+t.importer.objectFilename;
		if (  FileExist( t.strwork.Get() ) ) 
		{
			if ( strnicmp ( t.strwork.Get() + strlen(t.strwork.Get()) - 4, ".fbx", 4 )==NULL )
			{
				LoadFBX ( t.strwork.Get(), t.importer.objectnumber );
				//SetObjectRenderMatrixMode ( t.importer.objectnumber, 1 );
				g_bLoadedFBXModel = true;
			}
			else
			{
				LoadObject ( t.strwork.Get() ,t.importer.objectnumber );
				//SetObjectRenderMatrixMode ( t.importer.objectnumber, 0 );
			}
		}
		else
		{
			MakeObjectCube (  t.importer.objectnumber,100 );
		}
	}

	// 111118 - if import fails for any reason, ensure no crash
	if ( ObjectExist ( t.importer.objectnumber ) == 0 )
	{
		MakeObjectCube (  t.importer.objectnumber,100 );
	}

	// Position imported object at center
	LockObjectOn (  t.importer.objectnumber );
	PositionObject (  t.importer.objectnumber , 0 , 0 , 0 );

	//  Dummy character object
	t.importer.dummyCharacterObjectNumber=g.importermenuobjectoffset+2;
	if (  ObjectExist(t.importer.dummyCharacterObjectNumber)  )  DeleteObject (  t.importer.dummyCharacterObjectNumber );
	#ifdef PHOTONMP
	 LoadObject (  "entitybank\\characters\\Uber Character.dbo",t.importer.dummyCharacterObjectNumber );
	#else
	 LoadObject (  "entitybank\\characters\\Uber Soldier.X",t.importer.dummyCharacterObjectNumber );
	#endif
	TextureObject (  t.importer.dummyCharacterObjectNumber,0 );
	DisableObjectZDepth (  t.importer.dummyCharacterObjectNumber );
	LockObjectOn (  t.importer.dummyCharacterObjectNumber );
	ColorObject (  t.importer.dummyCharacterObjectNumber , Rgb(0,0,0) );
	SetObjectLight (  t.importer.dummyCharacterObjectNumber ,1 );
	SetObjectAmbience (  t.importer.dummyCharacterObjectNumber,0 );
	SetAlphaMappingOn (  t.importer.dummyCharacterObjectNumber , 30 );
	SetObjectEmissive (  t.importer.dummyCharacterObjectNumber, Rgb(255,0,0) );
	SetObjectEffect ( t.importer.dummyCharacterObjectNumber, g.guishadereffectindex );

	//  Set the pivot to the bottom of the object
	t.tTop_f = -10000;
	t.tBottom_f = 10000;
	PerformCheckListForLimbs (  t.importer.objectnumber );

	//  Find out the max and min Y vertex positions so we can work out where the pivot is now
	for ( t.tLimbCount = 1 ; t.tLimbCount<=  ChecklistQuantity()-1; t.tLimbCount++ )
	{
		LockVertexDataForLimb (  t.importer.objectnumber, t.tLimbCount );
		t.tVmax=GetVertexDataVertexCount();
		for ( t.tVertexCount = 0 ; t.tVertexCount<=  t.tVmax-1; t.tVertexCount++ )
		{
			t.tVertexY_f = GetVertexDataPositionY(t.tVertexCount);
			if (  t.tVertexY_f > t.tTop_f  )  t.tTop_f  =  t.tVertexY_f;
			if (  t.tVertexY_f < t.tBottom_f  )  t.tBottom_f  =  t.tVertexY_f;
		}
		UnlockVertexData (  );
	}

	//  MoveObject (  pivot to the bottom of the model for use in FPSC )
	t.importer.objectLeftOffset = 0;
	t.importer.objectFrontOffset = 0;
	t.importer.objectBottomOffset = 0;
	t.importer.objectTopOffset = 0;
	t.importer.objectCenterOffset = 0;

	//  calculate the radius that would encase the whole object (new mode 2 takes OriginalMatrix scaling into account)
	t.tObjectSize_f = ObjectSizeY(t.importer.objectnumber,2);
	if (  ObjectSizeX (t.importer.objectnumber,2) > t.tObjectSize_f  ) t.tObjectSize_f = ObjectSizeX (t.importer.objectnumber,2);
	if (  ObjectSizeZ (t.importer.objectnumber,2) > t.tObjectSize_f  ) t.tObjectSize_f = ObjectSizeZ (t.importer.objectnumber,2);
	t.importer.originalObjectSize = t.tObjectSize_f;

	//  Check the size and adjust scale to fit in view
	t.tScale = ( 300.00 / t.tObjectSize_f ) * 100.0;
	t.tScale = t.tScale * t.importer.scaleMulti;
	t.importer.objectScaleForEditing = t.tScale;
	t.importer.objectCenterOffset = -(ObjectSizeY (t.importer.objectnumber,0) / 2.0) * (t.tScale / 100.0) ;
	ScaleObject (  t.importer.objectnumber , t.tScale , t.tScale , t.tScale );
	t.tObjectSizeX_f = (ObjectSizeX(t.importer.objectnumber,0) * (t.tScale / 100.0)) * 0.95;
	t.tObjectSizeY_f = (ObjectSizeY(t.importer.objectnumber,0) * (t.tScale / 100.0)) * 0.95;
	t.tObjectSizeZ_f = (ObjectSizeZ(t.importer.objectnumber,0) * (t.tScale / 100.0)) * 0.95;
	PositionObject ( t.importer.objectnumber, 0, 0, 0 );

	t.tBoxOffsetLeft_f = 0;
	t.tBoxOffsetTop_f = 0;
	t.tBoxOffsetFront_f = 0;
	ScaleObject (  t.importer.dummyCharacterObjectNumber , t.tScale , t.tScale , t.tScale * 0.2 );
	PositionObject ( t.importer.dummyCharacterObjectNumber, 0, 0, 0 );

	//  Create the grid below the character to help show the camera rotation
	int tCount = 1;
	for ( tCount = 1 ; tCount<= 6; tCount++ )
	{
		t.importerGridObject[tCount] = findFreeObject();
		MakeObjectPlane (  t.importerGridObject[tCount], 0 , 400 * t.importer.scaleMulti );
		PositionObject ( t.importerGridObject[tCount], 0, 0, 0 );
		SetObjectWireframe (  t.importerGridObject[tCount] , 1 );
		ColorObject (  t.importerGridObject[tCount],Rgb(0,0,0) );
		SetObjectAmbience (  t.importerGridObject[tCount],0 );
		SetObjectEmissive (  t.importerGridObject[tCount], Rgb(0,0,0) );
		SetObjectLight (  t.importerGridObject[tCount] , 1 );
		XRotateObject (  t.importerGridObject[tCount],180 );
		LockObjectOn (  t.importerGridObject[tCount] );
		SetObjectEffect ( t.importerGridObject[tCount], g.guishadereffectindex );
	}

	//  Front marker
	t.importerGridObject[7] = findFreeObject();
	MakeObjectPlane (  t.importerGridObject[7], 10 , 20 * t.importer.scaleMulti );
	PositionObject ( t.importerGridObject[7], 0, 0, 0 );
	ColorObject (  t.importerGridObject[7],Rgb(0,0,0) );
	SetObjectLight (  t.importerGridObject[7] , 1 );
	SetObjectAmbience (  t.importerGridObject[7],0 );
	SetObjectEmissive (  t.importerGridObject[7], Rgb(0,0,0) );
	MoveObject (  t.importerGridObject[7], 200 );
	XRotateObject (  t.importerGridObject[7], -90 );
	LockObjectOn (  t.importerGridObject[7] );
	SetObjectEffect ( t.importerGridObject[7], g.guishadereffectindex );

	//  Position the grid
	MoveObjectLeft (  t.importerGridObject[1],200 * t.importer.scaleMulti );
	XRotateObject (  t.importerGridObject[1] , -90 );
	MoveObjectRight (  t.importerGridObject[2],200 * t.importer.scaleMulti );
	XRotateObject (  t.importerGridObject[2] , -90 );
	MoveObject (  t.importerGridObject[3],200 * t.importer.scaleMulti );
	YRotateObject (  t.importerGridObject[3],90 );
	XRotateObject (  t.importerGridObject[3] , -90 );
	MoveObject (  t.importerGridObject[4],-200 * t.importer.scaleMulti );
	YRotateObject (  t.importerGridObject[4],90 );
	XRotateObject (  t.importerGridObject[4] , -90 );
	YRotateObject (  t.importerGridObject[5],90 );
	XRotateObject (  t.importerGridObject[5] , -90 );
	XRotateObject (  t.importerGridObject[6] , -90 );

	t.importerGridObject[8] = findFreeObject();
	MakeObjectCube (  t.importerGridObject[8] , 1 );
	HideObject (  t.importerGridObject[8] );
	SetObjectLight (  t.importerGridObject[8],1 );
	LockObjectOn (  t.importerGridObject[8] );
	SetObjectEffect ( t.importerGridObject[8], g.guishadereffectindex );
	for ( tCount = 1 ; tCount<=  7; tCount++ )
	{
		FixObjectPivot (  t.importerGridObject[tCount] );
		GlueObjectToLimbEx (  t.importerGridObject[tCount], t.importerGridObject[8], 0 , 1 );
	}
	GlueObjectToLimbEx (  t.importer.dummyCharacterObjectNumber, t.importerGridObject[8], 0 , 1 );

	//  SetObject (  rotation mode and initial angle (angle may change if a fpe is associated with the model) )
	t.importer.objectRotateMode = 0;
	t.importer.objectAngleY = 0.0;

	//  create pivot for collision boxes
	t.importerGridObject[9] = findFreeObject();
	MakeObjectCube (  t.importerGridObject[9] , 1 );
	SetObjectLight (  t.importerGridObject[9],1 );
	ColorObject (  t.importerGridObject[9],Rgb(0,0,0) );
	HideObject (  t.importerGridObject[9] );
	LockObjectOn (  t.importerGridObject[9] );
	for ( tCount = 1 ; tCount<=  9; tCount++ )
	{
		SetObjectEmissive ( t.importerGridObject[tCount], Rgb(255,255,0) );
		SetObjectEffect ( t.importerGridObject[tCount], g.guishadereffectindex );
	}

	//  Check for FPE file, load and apply if one is found
	importer_load_fpe ( );

	// Autoset other FPE settings (if FBX import for first time)
	if ( g_iPreferPBR > 0 )
	{
		t.importer.objectFPE.effect = "effectbank\\reloaded\\apbr_basic.fx";
	}

	// update UI panel of any changes so far
	importer_apply_fpe();

	// ensure no value from emissive effect
	SetObjectEmissive ( t.importer.objectnumber, 0 );

	// reset image to texture limb mapping array
	g_dwMapImageListIndexToLimbCount = 0;
	SAFE_DELETE ( g_piMapImageListIndexToLimbIndex );

	// if FBX import, add appropriate texture files to importerlist
	if ( bHasFBXExtension == true )
	{
		// Clean importer Textures List
		int tCount = 0;
		for ( tCount = 1 ; tCount <= IMPORTERTEXTURESMAX; tCount++ )
		{
			t.importerTextures[tCount].imageID = 0;
			t.importerTextures[tCount].fileName = "";
			t.importerTextures[tCount].iExpandedThisSlot = 0;		
			t.importerTextures[tCount].iOptionalStage = 0;
			t.importerTextures[tCount].iAssociatedBaseImage = 0;
		}

		// first check if importer has textures from FBX conversion
		cstr sOldDir = GetDir();
		SetDir(cstr(g.rootdir_s+"importer\\temp").Get());
		bool bUseTexturesFromConversion = false;
		sObject* pObject = GetObjectData ( t.importer.objectnumber );
		if ( pObject )
		{
			tCount = 0;
			for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
			{
				sMesh* pMesh = pObject->ppMeshList[iMesh];
				int iStoreImageSlotIndexForBase = -1;
				for ( int iTextureStage = 0; iTextureStage < pMesh->dwTextureCount; iTextureStage++ )
				{
					int iSlotUsed = -1;
					cstr pTextureFile = pMesh->pTextures[iTextureStage].pName;
					if ( FileExist ( pTextureFile.Get() ) == 0 )
					{
						pTextureFile = Left ( pTextureFile.Get(), strlen(pTextureFile.Get())-4 );
						pTextureFile = pTextureFile + ".dds";
					}
					if ( FileExist ( pTextureFile.Get() ) == 1 )
					{
						cstr pAbsoluteFile = g.rootdir_s + "importer\\temp\\" + pTextureFile;
						iSlotUsed = importer_addtexturefiletolist ( pAbsoluteFile.Get(), pAbsoluteFile.Get(), &tCount );
						bUseTexturesFromConversion = true;
					}
					else
					{
						// FBX conversion had no textures produced, so add texture filename directly to importer image list
						// which we can then use later to find associate texture files
						if ( strlen ( pMesh->pTextures[iTextureStage].pName ) > 0 )
						{
							cstr pAbsoluteFile = t.importer.objectFileOriginalPath + pMesh->pTextures[iTextureStage].pName;
							iSlotUsed = importer_addtexturefiletolist ( pAbsoluteFile.Get(), pAbsoluteFile.Get(), &tCount );
						}
						#ifdef VRTECH
						else 
						{
							//PE:Some models have empty texture field, just add blank.
							//PE:We should always have one , so user can change the texture. (even if not found).
							cstr pAbsoluteFile = t.importer.objectFileOriginalPath + "blank";
							iSlotUsed = importer_addtexturefiletolist(pAbsoluteFile.Get(), pAbsoluteFile.Get(), &tCount);
						}
						#endif
					}
					if ( iSlotUsed > -1 )
					{
						if ( iTextureStage == 0 ) 
						{
							// base texture (color)
							iStoreImageSlotIndexForBase = iSlotUsed;
						}
						else
						{
							// associate other stages to above base stage
							t.importerTextures[iSlotUsed].iOptionalStage = iTextureStage;
							t.importerTextures[iSlotUsed].iAssociatedBaseImage = iStoreImageSlotIndexForBase;
						}
					}
				}
			}
			SetDir(sOldDir.Get());
		}
		if ( bUseTexturesFromConversion == true )
		{
			// if model did not have its own textures, texture with external ones next to FBX model
			importer_load_textures_finish ( tCount, true ); // add cube mapping texture stage only
		}
		else
		{
			// if model did not have its own textures, texture with external ones next to FBX model
			importer_load_textures_finish ( tCount, false ); // texture object from import image texture list
		}
	}
	else
	{
		//  Load textures the old fashioned way
		importer_load_textures ( );
	}

	// prior to setting effect (changing it) keep a copy of original import (but after setting textures)
	if ( ObjectExist( t.importer.objectnumberpreeffectcopy ) == 1 ) DeleteObject ( t.importer.objectnumberpreeffectcopy );
	CloneObject ( t.importer.objectnumberpreeffectcopy, t.importer.objectnumber );

	// Once shader wiping tetxure applied, apply shader to imported object (changed later if prefer PBR shader - see below)
	#ifdef VRTECH
	int iEffectID;
	if (g.gpbroverride == 1)
		iEffectID = loadinternaleffect("effectbank\\reloaded\\apbr_basic.fx");
	else
		iEffectID = loadinternaleffect("effectbank\\reloaded\\entity_basic.fx");
	SetObjectEffect ( t.importer.objectnumber, iEffectID );
	//PE: make sure it is changed. fix for: https://github.com/TheGameCreators/GameGuruRepo/issues/123
	if (g.gpbroverride == 1)
		importer_changeshader("apbr_basic.fx");
	else
		importer_changeshader("entity_basic.fx");
	#else
	int iEffectID = loadinternaleffect("effectbank\\reloaded\\entity_basic.fx");
	SetObjectEffect ( t.importer.objectnumber, iEffectID );
	//PE: make sure it is changed. fix for: https://github.com/TheGameCreators/GameGuruRepo/issues/123
	importer_changeshader("entity_basic.fx");


	//PE: Bug. make sure we dont get clipped, model was only half visible.
	//reuse g.characterkitvector = 46
	t.tnothing = MakeVector4(g.characterkitvector);
	SetVector4(g.characterkitvector, 500000, 1, 0, 0);
	SetEffectConstantV(iEffectID, "EntityEffectControl", g.characterkitvector);
	t.tnothing = DeleteVector4(g.characterkitvector);

	// attach to gimble so can manipulate imported object
	GlueObjectToLimbEx (  t.importer.objectnumber, t.importerGridObject[8], 0 , 1 );
	PositionObject (  t.importerGridObject[8], 0 , 0 , IMPORTERZPOSITION );

	// if no physics shapes attached to mode, make a default one covering the whole object
	if (  t.importer.collisionShapeCount  ==  0 ) 
	{
		importer_add_collision_box ( );
	}

	//  set up collision object selection boxes
	for ( tCount = 1 ; tCount<=  9; tCount++ )
	{
		t.selectedObjectMarkers[tCount] = findFreeObject();
		MakeObjectCube (  t.selectedObjectMarkers[tCount], 10 );
		SetObjectLight (  t.selectedObjectMarkers[tCount], 1 );
		SetObjectAmbience (  t.selectedObjectMarkers[tCount],0 );
		if (  tCount  !=  9 ) 
		{
			SetObjectEmissive (  t.selectedObjectMarkers[tCount], Rgb(255,255,0) );
		}
		else
		{
			SetObjectEmissive (  t.selectedObjectMarkers[tCount], Rgb(0,255,255) );
		}
		DisableObjectZDepth (  t.selectedObjectMarkers[tCount] );
		LockObjectOn (  t.selectedObjectMarkers[tCount] );
		SetObjectWireframe (  t.selectedObjectMarkers[tCount],1 );
		HideObject (  t.selectedObjectMarkers[tCount] );
		SetObjectEmissive ( t.selectedObjectMarkers[tCount], Rgb(255,255,0) );
		SetObjectEffect ( t.selectedObjectMarkers[tCount], g.guishadereffectindex );
	}

	//  Default selected collision object
	t.importer.selectedCollisionObject = 0;
	t.slidersmenuvalue[t.importer.properties2Index][6].value = ObjectAngleX(t.importerCollision[0].object2);
	t.slidersmenuvalue[t.importer.properties2Index][7].value = ObjectAngleY(t.importerCollision[0].object2);
	t.slidersmenuvalue[t.importer.properties2Index][8].value = ObjectAngleZ(t.importerCollision[0].object2);

	t.importer.scaleMulti = 2.0;
	importer_screenSwitch ( );

	//  Start importer loop
	t.importer.loaded=1;
	importer_fade_in ( );

	// reset camera vars for importer viewer
	float fDefaultCameraHeight = ObjectSizeY(t.importer.objectnumber,1) / 2.0;
	t.importer.cameraheight = fDefaultCameraHeight;
	t.importer.originalcameraheight = fDefaultCameraHeight;
	t.importer.lastcameraheightforshift = fDefaultCameraHeight;

	// if prefer PBR, effect needs to be switched now
	g_iPreferPBRLateShaderChange = 0;
	if ( g_iPreferPBR > 0 )
	{
		// apply shader to preferred PBR render view
		g_iPreferPBRLateShaderChange = 1;
	}
	#endif

	#endif // wicked endif

	// seems to be needed for VRQ too (for scaling)
	t.importer.camerazoom = 1.0f;

	// remove editor prompt
	popup_text_close();

	#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	iImporterScale = 100; //Default scale.
	t.slidersmenuvalue[t.importer.properties1Index][1].value = 100;
	iImporterGenerateThumb = 0;
	//Reset colors and update.
	//visuals_editordefaults();
	//t.visuals.SurfaceSunFactor_f = 0.75;
	////t.visuals.AmbienceIntensity_f = 195.0f;
	//t.visuals.SurfaceIntensity_f = 0.9f;
	
	//visualsdatastoragetype desiredVisuals;
	//set_temp_visuals(t.editorvisuals, t.visualsStorage, desiredVisuals);
	//set_temp_visuals(t.visuals, t.visualsStorage, desiredVisuals);
	//set_temp_visuals(t.gamevisuals, t.visualsStorage, desiredVisuals);
	//t.visuals.refreshshaders = 1;
	//visuals_loop();

	

	//t.visuals.refreshskysettings = 0;
	#endif 
}

#ifdef WICKEDENGINE
void importer_load_scenery()
{
	float fDefaultCharacterHeight = 65.0f;
	float fMinimumScenerySize = 750.0f;

	// The coordinate that the following objects are positioned relative to.
	t.importer.fSceneLocationY = 100000.f;
	int iImporterOrigin[3] = { 0.0f, t.importer.fSceneLocationY, 0.0f };

	sObject* pObject = GetObjectData(t.importer.objectnumber);

	// The size of the imported object (multiply by 4, to account for error in the collision radius).
	float fSize = pObject->collision.fRadius * 4.f;
	float xmax = pObject->collision.vecMax.x;
	sCollisionData& rCollision = pObject->collision;
	

	// Calculate the necessary size of the scenery to fit the imported model.
	t.importer.fSceneryDiameter += (fSize - t.importer.fSceneryDiameter);

	if (fSize < fMinimumScenerySize)
	{
		// Set the minimum size of the scenery.
		t.importer.fSceneryDiameter = fMinimumScenerySize;
	}

	// Position the model so that it is inside the importer scenery.
	PositionObject(t.importer.objectnumber, iImporterOrigin[0], iImporterOrigin[1], iImporterOrigin[2]);

	// Create the sphere.
	if (ObjectExist(g.importerextraobjectoffset) == 0)
	{
		MakeObjectSphere(g.importerextraobjectoffset, t.importer.fSceneryDiameter, 30, 30);
		PositionObject(g.importerextraobjectoffset, iImporterOrigin[0], iImporterOrigin[1], iImporterOrigin[2]);

		// Disable backface culling so we can see inside the sphere.
		SetObjectCull(g.importerextraobjectoffset, 0);

		// Texture the sphere (need emissive material so that outside light does not affect the inside of the sphere).
		TextureObject(g.importerextraobjectoffset, g.importerextraimageoffset);
		sObject* pObject = GetObjectData(g.importerextraobjectoffset);
		WickedCall_TextureObjectAsEmissive(pObject);

		// Disabling cast shadows for the sphere to avoid strange lighting artefacts.
		WickedCall_SetObjectCastShadows(pObject, false);
	}

	// Create the plane that intersects the sphere.
	if (ObjectExist(g.importerextraobjectoffset + 1) == 0)
	{
		// If the importer sphere is not being displayed, make the plane larger.
		float fScaleFactor = 1.0f;
		if (pref.iImporterDome == 0) fScaleFactor = 3.0f;
		MakeObjectPlane(g.importerextraobjectoffset + 1, t.importer.fSceneryDiameter * fScaleFactor, t.importer.fSceneryDiameter * fScaleFactor);
		RotateObject(g.importerextraobjectoffset + 1, 90.f, 0.f, 0.f);
		PositionObject(g.importerextraobjectoffset + 1, iImporterOrigin[0], iImporterOrigin[1], iImporterOrigin[2]);
		TextureObject(g.importerextraobjectoffset + 1, g.importerextraimageoffset + 1);
		sObject* pObject = GetObjectData(g.importerextraobjectoffset + 1);
		WickedCall_SetObjectLightToUnlit(pObject, wiScene::MaterialComponent::SHADERTYPE::SHADERTYPE_UNLIT);
	}

	// Create the plane that shows the relative scale of the imported model.
	if (ObjectExist(g.importerextraobjectoffset + 2) == 0)
	{
		MakeObjectPlane(g.importerextraobjectoffset + 2, 25, fDefaultCharacterHeight);

		// Offset the decal to the left or right of the model, based on how big the model is.
		float fDecalOffset = iImporterOrigin[0] - fabs(rCollision.vecMin.x);
		float fMaxDecalOffset = t.importer.fSceneryDiameter / 8.0f;
		if (fabs(fDecalOffset) > fMaxDecalOffset)
		{
			if (fDecalOffset < 0.0f)
				fDecalOffset = -fMaxDecalOffset;
			if (fDecalOffset > 0.0f)
				fDecalOffset = fMaxDecalOffset;
		}
		// Move the reference plane to the left of the object (additional 25.f is width of the plane).
		PositionObject(g.importerextraobjectoffset + 2, fDecalOffset - 25.f, iImporterOrigin[1] + (fDefaultCharacterHeight / 2.0f), iImporterOrigin[2]);

		// Texture the decal and force transparency and emissive.
		TextureObject(g.importerextraobjectoffset + 2, g.importerextraimageoffset + 2);
		sObject* pObject = GetObjectData(g.importerextraobjectoffset + 2);
		WickedCall_TextureObjectAsEmissive(pObject);
		WickedCall_SetObjectTransparentDirect(pObject, true);
		WickedCall_SetObjectCastShadows(pObject, false);
		// Need to retain alpha since TextureObjectAsEmissive() sets it to 0.
		WickedCall_SetObjectAlpha(pObject, 100.0f);
	}

	// Position the camera at the back of the dome, looking at the imported model and character decal.
	PositionCamera(iImporterOrigin[0], iImporterOrigin[1] + 75, iImporterOrigin[2] - (t.importer.fSceneryDiameter / 2.75f));

	// Make the camera look at the midpoint height between the decal and model.
	float fDecalLookPoint = iImporterOrigin[1] + (fDefaultCharacterHeight / 2.0f);
	float fModelLookPoint = iImporterOrigin[1] + (fSize / 4.0f);
	PointCamera(iImporterOrigin[0], (fDecalLookPoint + fModelLookPoint) / 2.0f, iImporterOrigin[2]);

	// Set the editor free flight camera orientation so that the new camera orientation is not overridden.
	t.editorfreeflight.c.x_f = CameraPositionX();
	t.editorfreeflight.c.y_f = CameraPositionY();
	t.editorfreeflight.c.z_f = CameraPositionZ();
	t.editorfreeflight.c.angx_f = CameraAngleX();
	t.editorfreeflight.c.angy_f = CameraAngleY();
	// Pressing F caused the camera to not be moved. So force it into free flight mode.
	t.editorfreeflight.mode = 1;


	if (!pref.iImporterDome)
	{
		HideObject(g.importerextraobjectoffset);
		//HideObject(g.importerextraobjectoffset + 1);
	}

}
#endif

void importer_RestoreCollisionShiftHeight ( void )
{
	float fShiftForCollisionObjects = t.importer.originalcameraheight - t.importer.lastcameraheightforshift;
	t.importer.lastcameraheightforshift = t.importer.originalcameraheight;
	for ( int tCount = 0 ; tCount<=  t.importer.collisionShapeCount-1; tCount++ )
	{
		if (  t.importerCollision[tCount].object > 0 ) 
		{
			if (  ObjectExist(t.importerCollision[tCount].object)  ==  1 ) 
			{
				float fX = ObjectPositionX(t.importerCollision[tCount].object);
				float fY = ObjectPositionY(t.importerCollision[tCount].object) - fShiftForCollisionObjects;
				float fZ = ObjectPositionZ(t.importerCollision[tCount].object);
				PositionObject ( t.importerCollision[tCount].object, fX, fY, fZ );
				PositionObject ( t.importerCollision[tCount].object2, fX, fY, fZ );
			}
		}
	}
}

#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 

#ifdef WICKEDENGINE
void animsystem_clearoldanimationfromobject ( sObject* pObject )
{
	// stop any animation playing
	WickedCall_StopObject(pObject);

	// remove any animcomponents
	sAnimationSet* pAnimSet = pObject->pAnimationSet;
	if (pAnimSet)
	{
		// only first animset is used by wicked engine
		if (pAnimSet->wickedanimentityindex > 0)
		{
			wiScene::AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent( pAnimSet->wickedanimentityindex );
			for( int i = 0; i < animationcomponent->samplers.size(); i++ )
			{
				wiScene::GetScene().Entity_Remove( animationcomponent->samplers[i].data );
			}

			wiScene::GetScene().Entity_Remove(pAnimSet->wickedanimentityindex);
			pAnimSet->wickedanimentityindex = 0;
		}
	}

	// erase all animation sets in object
	ClearAnimationFromObject(pObject);
	pObject->fAnimTotalFrames = 0;
}

void animsystem_processallmannequins (void)
{
	// establish location of dsadsa model
	char pMannequinPath[MAX_PATH];
	strcpy(pMannequinPath, g.fpscrootdir_s.Get());
	strcat(pMannequinPath, "\\Files\\charactercreatorplus\\");

	// scan all folders in "charactercreatorplus\animations"
	LPSTR pOldDir = GetDir();
	SetDir (g.fpscrootdir_s.Get());
	UnDim (t.filelist_s);
	buildfilelist("Files\\charactercreatorplus\\animations", "");
	if (ArrayCount(t.filelist_s) > 0)
	{
		timestampactivity (0, "Processing all 'charactercreatorplus animations'");
		for (t.chkfile = 0; t.chkfile <= ArrayCount(t.filelist_s); t.chkfile++)
		{
			t.file_s = t.filelist_s[t.chkfile];
			timestampactivity (0, t.file_s.Get());
			if (t.file_s != "." && t.file_s != "..")
			{
				// ignore the "noncharacter" folder 
				if (strnicmp(t.file_s.Get(), "noncharacter", strlen("noncharacter")) != NULL )
				{
					if (cstr(Lower(Right(t.file_s.Get(), 4))) == ".dbo")
					{
						// start with no object before the load
						if (ObjectExist(g.tempobjectoffset) == 1) DeleteObject(g.tempobjectoffset);

						// for each DBO found, load in mannequin.dbo
						LPSTR pThisDir = GetDir();
						SetDir (pMannequinPath);
						LoadObject("mannequin.dbo", g.tempobjectoffset);
						SetDir (pThisDir);

						// append the found DBO animation file
						char pSrcFilePath[MAX_PATH];
						strcpy(pSrcFilePath, g.fpscrootdir_s.Get());
						strcat(pSrcFilePath, "\\Files\\charactercreatorplus\\animations\\");
						strcat(pSrcFilePath, t.file_s.Get());
						AppendObject(pSrcFilePath, g.tempobjectoffset, 0);

						// save new DBO with replaced model
						char pDestFilePath[MAX_PATH];
						strcpy(pDestFilePath, pSrcFilePath);
						GG_GetRealPath(pDestFilePath, 1);
						if (FileExist(pDestFilePath) == 1) DeleteFileA(pDestFilePath);
						SaveObject(pDestFilePath, g.tempobjectoffset);

						// get name only
						char pNameOnly[MAX_PATH];
						strcpy(pNameOnly, pDestFilePath);
						if (strlen(pNameOnly) > 4)
						{
							for (int n = strlen(pDestFilePath) - 4; n > 0; n--)
							{
								if (pDestFilePath[n] == '\\' || pDestFilePath[n] == '/')
								{
									strcpy(pNameOnly, pDestFilePath+n+1);
									break;
								}
							}
							pNameOnly[strlen(pNameOnly) - 4] = 0;
						}

						// also save an FPE which will allow display in the library view
						char pFPEFile[MAX_PATH];
						strcpy(pFPEFile, pDestFilePath);
						pFPEFile[strlen(pFPEFile) - 4] = 0;
						strcat(pFPEFile, ".fpe");
						if (FileExist(pFPEFile) == 1) DeleteFileA(pFPEFile);
						OpenToWrite(1, pFPEFile);
						WriteString(1, "; header");
						char pLine[MAX_PATH];
						sprintf(pLine, "desc = %s", pNameOnly);
						WriteString(1, pLine);
						WriteString(1, "; scripts");
						WriteString(1, "aimain = people\\play_animation.lua");
						WriteString(1, "; geometry");
						sprintf(pLine, "model = %s.dbo", pNameOnly);
						WriteString(1, pLine);
						WriteString(1, "; visuals");
						WriteString(1, "texturepath = charactercreatorplus\\");
						WriteString(1, "textured = mannequin_color.dds");
						WriteString(1, "; anim set");
						WriteString(1, "animspeed = 100");
						WriteString(1, "animmax = 1");
						WriteString(1, "playanimineditor = All");
						WriteString(1, "; thumbnail");
						WriteString(1, "thumbnailbackdrop = Stage 1.dds");
						WriteString(1, "thumbnailzoom = 14.214919");
						WriteString(1, "thumbnailcamleft = 0.000000");
						WriteString(1, "thumbnailcamup = 0.000000");
						WriteString(1, "thumbnailrotatex = 0.000000");
						WriteString(1, "thumbnailrotatey = 0.000000");
						WriteString(1, "thumbnailanimset = 0");
						CloseFile(1);
					}
				}
			}
		}
	}

	// clear resources
	if (ObjectExist(g.tempobjectoffset) == 1) DeleteObject(g.tempobjectoffset);

	// restore path
	SetDir (pOldDir);
}

void animsystem_loadanimtextfile (sObject* pObject, cstr pAbsPathToAnim, char* cFileSelected)
{
	// anim file or anim list file
	struct sAnimFileToAppend
	{
		cstr file;
		int iStep1;
		int iStep2;
		int iStep3;
	};
	char pTheFile[MAX_PATH];
	std::vector<sAnimFileToAppend> filestoappend;
	filestoappend.clear();
	if (strnicmp(cFileSelected + strlen(cFileSelected) - 4, ".dbo", 4) == NULL)
	{
		// animation file chosen
		sAnimFileToAppend item;
		item.file = cFileSelected;
		item.iStep1 = 0;
		item.iStep2 = 0;
		item.iStep3 = 0;
		filestoappend.push_back(item);
	}
	else if (strnicmp(cFileSelected + strlen(cFileSelected) - 4, ".txt", 4) == NULL || strnicmp(cFileSelected + strlen(cFileSelected) - 4, ".dat", 4) == NULL)
	{
		// special case, if this file chosen, we want to replace ALL models with mannequin and store in writables for testing (and eventual use)
		LPSTR pProcessFileTrigger = "processallmannequins.txt";
		if (strnicmp(cFileSelected + strlen(cFileSelected) - strlen(pProcessFileTrigger), pProcessFileTrigger, strlen(pProcessFileTrigger)) == NULL )
		{
			// replace all DBOs in "charactercreatorplus\animations" with mannequin model (exclude non-character animations)
			animsystem_processallmannequins();
			return;
		}

		// anim list file (text file containing many references to animation files)
		OpenToRead(1, cFileSelected);
		while (FileEnd(1) == 0)
		{
			int iThisStep1 = 0;
			int iThisStep2 = 0;
			int iThisStep3 = 0;
			int iStage = 0;
			LPSTR pLine = ReadString (1);
			char pLineChomp[MAX_PATH];
			memset(pLineChomp, 0, sizeof(pLineChomp));
			strcpy (pLineChomp, pLine);
			if (strlen(pLineChomp) > 0)
			{
				while (strlen(pLineChomp) > 0)
				{
					char pThisBit[MAX_PATH];
					memset(pThisBit, 0, sizeof(pThisBit));
					strcpy (pThisBit, pLineChomp);
					LPSTR pStepValue = strstr (pThisBit, ";");
					if (pStepValue)
					{
						*pStepValue = 0;
						strcpy (pLineChomp, pStepValue + 1);
					}
					else
					{
						strcpy (pLineChomp, "");
					}
					if (iStage == 0) sprintf(pTheFile, "%s%s.dbo", pAbsPathToAnim.Get(), pThisBit);
					if (iStage == 1) iThisStep1 = atoi(pThisBit);
					if (iStage == 2) iThisStep2 = atoi(pThisBit);
					if (iStage == 3) iThisStep3 = atoi(pThisBit);
					iStage++;
				}
				sAnimFileToAppend item;
				item.file = pTheFile;
				item.iStep1 = iThisStep1;
				item.iStep2 = iThisStep2;
				item.iStep3 = iThisStep3;
				filestoappend.push_back(item);
			}
		}
		CloseFile(1);
	}
	else
	{
		// single animation file chosen (not DBO or TXT/DAT list)
		sAnimFileToAppend item;
		item.file = cFileSelected;
		item.iStep1 = 0;
		item.iStep2 = 0;
		item.iStep3 = 0;
		filestoappend.push_back(item);
	}

	// append all animation files specified in list
	for (int l = 0; l < filestoappend.size(); l++)
	{
		// this anim
		strcpy (pTheFile, filestoappend[l].file.Get());
		pAbsPathToAnim = pTheFile;

		// extract name from file
		char pAppendThisAnimFile[MAX_PATH];
		strcpy(pAppendThisAnimFile, "");
		for (int n = strlen(pTheFile); n > 0; n--)
		{
			if (pTheFile[n] == '\\' || pTheFile[n] == '/')
			{
				strcpy(pAppendThisAnimFile, pTheFile + n + 1);
				if (strnicmp(pAppendThisAnimFile + strlen(pAppendThisAnimFile) - 4, ".dbo", 4) == NULL)
				{
					pAppendThisAnimFile[strlen(pAppendThisAnimFile) - 4] = 0;
				}
				break;
			}
		}

		// keep non-combat animations from base model
		int iFrameToAppendFrom = pObject->fAnimTotalFrames;
		if (iFrameToAppendFrom > 0)
		{
			// after initial animation, should not overwrite last frame!
			iFrameToAppendFrom++;
		}

		// append animations to character
		if (AppendAnimationFromFile(pObject, pAbsPathToAnim.Get(), iFrameToAppendFrom) == false)
			break;

		// Create animations (from animation data stored in DBO)
		WickedCall_RefreshObjectAnimations(pObject, pObject->wickedloaderstateptr);

		// if successful, add to end of list
		sAnimSlotStruct animslotitem;
		animslotitem.fStep1 = 0;
		animslotitem.fStep2 = 0;
		animslotitem.fStep3 = 0;
		if (strlen(pAppendThisAnimFile) >= 32)
		{
			memcpy(animslotitem.pName, pAppendThisAnimFile, 31);
			animslotitem.pName[31] = 0;
		}
		else
		{
			strcpy(animslotitem.pName, pAppendThisAnimFile);
		}
		animslotitem.fStart = iFrameToAppendFrom;//LB: this caused a world of pain +1;
		animslotitem.fFinish = pObject->fAnimTotalFrames;
		if (filestoappend[l].iStep1 > 0) animslotitem.fStep1 = iFrameToAppendFrom + filestoappend[l].iStep1;
		if (filestoappend[l].iStep2 > 0) animslotitem.fStep2 = iFrameToAppendFrom + filestoappend[l].iStep2;
		if (filestoappend[l].iStep3 > 0) animslotitem.fStep3 = iFrameToAppendFrom + filestoappend[l].iStep3;
		animslotitem.bLooped = true;
		g_pAnimSlotList.push_back(animslotitem);

		// also update main anim count (first slot always shows all frames)
		g_pAnimSlotList[0].fFinish = pObject->fAnimTotalFrames;
	}
}

void animsystem_animationtoolui(int objectnumber)
{
	#ifdef WICKEDENGINE
	extern int iLastOpenHeader;
	if (pref.bAutoClosePropertySections && iLastOpenHeader != 70)
		ImGui::SetNextItemOpen(false, ImGuiCond_Always);

	// If the last open header was the animation tool simple ui(71) then the user toggled advanced settings so this header should be opened.
	if (pref.bAutoClosePropertySections && iLastOpenHeader == 71)
		ImGui::SetNextItemOpen(true, ImGuiCond_Always);
	#endif

	if (ImGui::StyleCollapsingHeader("Animation Tool", ImGuiTreeNodeFlags_DefaultOpen))
	{
		iLastOpenHeader = 70;

		// standard numeric box to hold 99999 value
		static int iNumericWidth = 30;
		iNumericWidth = 30;
		// object being edited with animation tool
		sObject* pObject = GetObjectData(objectnumber);

		// if have animations
		ImGui::Indent(10);
		if (bFoundanimSet == NULL)
		{
			ImGui::TextCenter("No Animations Found");
		}
		else
		{
			// prep of animation tool component
			ImGui::PushItemWidth(-10);

			// animation preview
			if (ImGui::Checkbox("Animate Preview", &g_bAnimatingObjectPreview))
			{
				g_bUpdateAnimationPreview = true;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Animate Object Preview");

			// animation preview
			if (ImGui::Checkbox("Show Bones", &g_bShowBones))
			{
				wiRenderer::SetToDrawDebugBoneLines(g_bShowBones);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Show debug bones associated with object");


			extern void ControlAdvancedSetting(int&, const char*, bool* = 0);
			if (t.importer.importerActive == 0)
				ControlAdvancedSetting(pref.iEnableAdvancedCharacterCreator, "Advanced Character Creator Settings");

			// animation frame scrubber
			ImGui::TextCenter("Animation Frame");
			static float fAnimFrameStart;
			fAnimFrameStart = WickedCall_GetObjectFrame(pObject);
			if (ImGui::MaxSliderInputFloat("##iAnimFrame:", &fAnimFrameStart, 0, pObject->fAnimTotalFrames, "Shows the current animation frame", 0, pObject->fAnimTotalFrames, iNumericWidth))
			{
				SetObjectFrame(objectnumber, fAnimFrameStart);
				g_bAnimatingObjectPreview = false;
				g_bUpdateAnimationPreview = true;
			}

			// animation speed
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Animation Speed");
			ImGui::TextCenter("Animation Speed");
			static float fAnimSpeed;
			fAnimSpeed = pObject->fAnimSpeed * 100;
			if (ImGui::MaxSliderInputFloat("##iAnimSpeed:", &fAnimSpeed, 0.0f, 500.0f, "Set the speed of the overall animations", 0, 500.0f, iNumericWidth))
			{
				SetObjectSpeed(objectnumber, fAnimSpeed);
			}

			iNumericWidth = 45;

			// layout of slot list
			static int iColumnLeft = ImGui::GetCursorPos().x;
			static int iColumnLabelXName = iColumnLeft + 30;
			static int iColumnLabelXStart = iColumnLeft + 110;
			static int iColumnLabelXFinish = iColumnLeft + 165;
			static int iColumnLabelXStep1 = iColumnLeft + 220;
			static int iColumnLabelXStep2 = iColumnLeft + 275;
			static int iColumnLabelXStep3 = iColumnLeft + 330;
			static int iColumnLabelXDelete = iColumnLeft + 365;
			static int iNameEntryWidth = 70.0f;

			// animation slot titles
			ImGui::TextCenter("Animations");
			ImGui::SetCursorPos(ImVec2(iColumnLabelXName, ImGui::GetCursorPos().y));
			ImGui::Text("Name");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(iColumnLabelXStart, ImGui::GetCursorPos().y));
			ImGui::Text("Start");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(iColumnLabelXFinish, ImGui::GetCursorPos().y));
			ImGui::Text("Finish");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(iColumnLabelXStep1, ImGui::GetCursorPos().y));
			ImGui::Text("Left");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(iColumnLabelXStep2, ImGui::GetCursorPos().y));
			ImGui::Text("Right");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(iColumnLabelXStep3, ImGui::GetCursorPos().y));
			ImGui::Text("Any");

			// list of animation slots
			int iDeleteAnimSlot = -1;
			for (int slot = 0; slot < (int)g_pAnimSlotList.size(); slot++)
			{
				// animation slot details
				sAnimSlotStruct* pAnimSlotItem = &(g_pAnimSlotList[slot]);

				// scrollable list of columns (radio, name, start, finish, delete)
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(1, 0));
				char pImGuiID[32];
				sprintf(pImGuiID, "##AnimSlotRadio%d", slot);
				if (ImGui::RadioButton(pImGuiID, &g_iCurrentAnimationSlotIndex, slot))
				{
					// change slot to preview
					g_bUpdateAnimationPreview = true;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select this animation slot to preview the animation");
				if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;

				ImGui::SameLine();
				ImGui::PushItemWidth(iNameEntryWidth);
				sprintf(pImGuiID, "##AnimSlotName%d", slot);
				if (ImGui::InputText(pImGuiID, pAnimSlotItem->pName, 32))
				{
					// animation item name changed
				}
				if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;
				if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip(pAnimSlotItem->pName);
				ImGui::PopItemWidth();
				ImGui::SameLine();
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(1, 0));
				ImGui::PushItemWidth(iNumericWidth);
				int iAnimSlotItemStart = (int)pAnimSlotItem->fStart;
				sprintf(pImGuiID, "##AnimSlotStart%d", slot);
				if (ImGui::InputInt(pImGuiID, &iAnimSlotItemStart, 0, 9999)) g_bUpdateAnimationPreview = true;
				pAnimSlotItem->fStart = (float)iAnimSlotItemStart;
				if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change animation starting frame");
				ImGui::PopItemWidth();
				ImGui::SameLine();
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(1, 0));
				ImGui::PushItemWidth(iNumericWidth);
				int iAnimSlotItemFinish = (int)pAnimSlotItem->fFinish;
				sprintf(pImGuiID, "##AnimSlotFinish%d", slot);
				if (ImGui::InputInt(pImGuiID, &iAnimSlotItemFinish, 0, 9999)) g_bUpdateAnimationPreview = true;
				if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change animation finishing frame");
				pAnimSlotItem->fFinish = (float)iAnimSlotItemFinish;
				ImGui::PopItemWidth();
				// additional info for footfall frames
				ImGui::SameLine();
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(1, 0));
				ImGui::PushItemWidth(iNumericWidth);
				int iAnimSlotItemStep1 = (int)pAnimSlotItem->fStep1;
				sprintf(pImGuiID, "##AnimSlotStep1%d", slot);
				if (ImGui::InputInt(pImGuiID, &iAnimSlotItemStep1, 0, 9999)) g_bUpdateAnimationPreview = true;
				if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("XXX");
				pAnimSlotItem->fStep1 = (float)iAnimSlotItemStep1;
				ImGui::PopItemWidth();
				ImGui::SameLine();
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(1, 0));
				ImGui::PushItemWidth(iNumericWidth);
				int iAnimSlotItemStep2 = (int)pAnimSlotItem->fStep2;
				sprintf(pImGuiID, "##AnimSlotStep2%d", slot);
				if (ImGui::InputInt(pImGuiID, &iAnimSlotItemStep2, 0, 9999)) g_bUpdateAnimationPreview = true;
				if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("XXX");
				pAnimSlotItem->fStep2 = (float)iAnimSlotItemStep2;
				ImGui::PopItemWidth();
				ImGui::SameLine();
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(1, 0));
				ImGui::PushItemWidth(iNumericWidth);
				int iAnimSlotItemStep3 = (int)pAnimSlotItem->fStep3;
				sprintf(pImGuiID, "##AnimSlotStep3%d", slot);
				if (ImGui::InputInt(pImGuiID, &iAnimSlotItemStep3, 0, 9999)) g_bUpdateAnimationPreview = true;
				if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("XXX");
				pAnimSlotItem->fStep3 = (float)iAnimSlotItemStep3;
				ImGui::PopItemWidth();

				if (slot > 0)
				{
					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(1, 0));
					ImGui::PushItemWidth(10);
					sprintf(pImGuiID, "X##AnimaionToolDeleteSlot%d", slot);
					if (ImGui::StyleButton(pImGuiID, ImVec2(0, 0)))
					{
						// flag to delete from g_pAnimSlotList after loop!
						iDeleteAnimSlot = slot;
						break;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Click to delete this animation slot");
					ImGui::PopItemWidth();
				}
			}

			// create and load buttons
			float w = ImGui::GetWindowContentRegionWidth();
			float but_gadget_size = ImGui::GetFontSize()*10.0;
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
			if (ImGui::StyleButton("Create Animation Slot##AnimaionToolCreateNewSlot", ImVec2(but_gadget_size, 0)))
			{
				sAnimSlotStruct animslotitem;
				animslotitem.fStep1 = 0;
				animslotitem.fStep2 = 0;
				animslotitem.fStep3 = 0;
				strcpy(animslotitem.pName, "new slot");
				animslotitem.fStart = 0.0f;
				animslotitem.fFinish = pObject->fAnimTotalFrames;
				animslotitem.bLooped = true;
				g_pAnimSlotList.push_back(animslotitem);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Click to create a new animation slot");

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
			if (ImGui::StyleButton("Load Animation File##AnimaionToolLoadAnimationFile", ImVec2(but_gadget_size, 0)))
			{
				// file requester to select animation file
				cStr tOldDir = GetDir();
				cstr pAbsPathToAnim = g.fpscrootdir_s + "\\Files\\charactercreatorplus\\animations\\";
				char pTheFolder[MAX_PATH];
				strcpy (pTheFolder, pAbsPathToAnim.Get());
				char* cFileSelected = (char*)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "All\0*.*\0", pTheFolder, NULL);
				SetDir(tOldDir.Get());
				if (cFileSelected && strlen(cFileSelected) > 0)
				{
					// handle DBO or TXT/DAT and append animations to imported model
					animsystem_loadanimtextfile (pObject, pAbsPathToAnim, cFileSelected);

					// switch to newly appended animation
					g_iCurrentAnimationSlotIndex = g_pAnimSlotList.size() - 1;
					g_bUpdateAnimationPreview = true;
				}
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Click to append an animation from a file");

			/* reintroduce this when we can clean up the animation data and slots
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
			if (ImGui::StyleButton("Save Animation File##AnimaionToolSaveAnimationFile", ImVec2(but_gadget_size, 0)))
			{
				// file requester to select animation file
				cStr tOldDir = GetDir();
				cstr pAbsPathToAnim = g.fpscrootdir_s + "\\Files\\charactercreatorplus\\animations\\";
				char* cFileSelected = (char*)noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "All\0*.*\0", pAbsPathToAnim.Get(), NULL);
				SetDir(tOldDir.Get());
				if (cFileSelected && strlen(cFileSelected) > 0)
				{
					// go through animation data and clean it up (after EA)
					// essentially, take all animslots and preserve their keyframes in a new anim data stack
					// removing all the anim data that belonged to animslots that have been deleted over time
					// this ensures the keyframe anim data is packed sequentially representing the anim slots with no gaps

					// save as DBO filename
					char pSaveThisAnimFile[MAX_PATH];
					strcpy(pSaveThisAnimFile, cFileSelected);
					if (strnicmp(pSaveThisAnimFile + strlen(pSaveThisAnimFile) - 4, ".dbo", 4) != NULL)
					{
						pSaveThisAnimFile[strlen(pSaveThisAnimFile) - 4] = 0;
						strcat(pSaveThisAnimFile, ".dbo");
					}

					// save animations out as a DBO file
					SaveObject(pSaveThisAnimFile, objectnumber);

					// clean and reload object animation data  (after EA)
					// by cleaning and reloading here, we can test quickly to ensure the save did not corrupt any animation data or slots
				}
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Click to save an animation file");
			*/

			// Need a way to wipe out any old animation data
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
			if (ImGui::StyleButton("Clear All Animation##AnimaionToolClearAll", ImVec2(but_gadget_size, 0)))
			{
				// stop object anim, remove wicked anim components and clear all animation from object
				animsystem_clearoldanimationfromobject(pObject);

				// clear slot list
				g_iCurrentAnimationSlotIndex = 0;
				g_pAnimSlotList.clear();

				// and create new blank All slot
				// if successful, add to end of list
				sAnimSlotStruct animslotitem;
				animslotitem.fStep1 = 0;
				animslotitem.fStep2 = 0;
				animslotitem.fStep3 = 0;
				strcpy(animslotitem.pName, "All");
				animslotitem.fStart = 0;
				animslotitem.fFinish = 0;
				animslotitem.bLooped = true;
				g_pAnimSlotList.push_back(animslotitem);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Click to clear all animation data");

			// developer mode can save template of animation slots created
			extern int g_iDevToolsOpen;
			if (g_iDevToolsOpen != 0 )
			{
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
				if (ImGui::StyleButton("Save Animation Template##AnimaionToolSaveTemplate", ImVec2(but_gadget_size, 0)))
				{
					cstr sTempFileMakeLifeEasier = g.fpscrootdir_s + "\\Files\\charactercreatorplus\\animations\\exportedtemplate.txt";
					char pTempFileMakeLifeEasier[MAX_PATH];
					strcpy (pTempFileMakeLifeEasier, sTempFileMakeLifeEasier.Get());
					GG_GetRealPath(pTempFileMakeLifeEasier, 1);
					OpenToWrite(1, pTempFileMakeLifeEasier);
					for (int i = 0; i < g_pAnimSlotList.size(); i++)
					{
						char pMyLine[MAX_PATH];
						if (g_pAnimSlotList[i].fStep1 == 0)
						{
							sprintf(pMyLine, "\\%s", g_pAnimSlotList[i].pName);
						}
						else
						{
							if (g_pAnimSlotList[i].fStep2 == 0)
							{
								sprintf(pMyLine, "\\%s;%d", g_pAnimSlotList[i].pName, (int)(g_pAnimSlotList[i].fStep1 - g_pAnimSlotList[i].fStart));
							}
							else
							{
								if (g_pAnimSlotList[i].fStep3 == 0)
								{
									sprintf(pMyLine, "\\%s;%d;%d", g_pAnimSlotList[i].pName, (int)(g_pAnimSlotList[i].fStep1 - g_pAnimSlotList[i].fStart), (int)(g_pAnimSlotList[i].fStep2 - g_pAnimSlotList[i].fStart));
								}
								else
								{
									sprintf(pMyLine, "\\%s;%d;%d;%d", g_pAnimSlotList[i].pName, (int)(g_pAnimSlotList[i].fStep1 - g_pAnimSlotList[i].fStart), (int)(g_pAnimSlotList[i].fStep2 - g_pAnimSlotList[i].fStart), (int)(g_pAnimSlotList[i].fStep3 - g_pAnimSlotList[i].fStart));
								}
							}
						}
						WriteString (1, pMyLine);
					}
					CloseFile(1);
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Click to save template of animation slots to writable 'charactercreatorplus\\animations'.");
			}

			// end of animation tool component
			ImGui::PopItemWidth();

			// delete slot if required
			if (iDeleteAnimSlot != -1 && g_pAnimSlotList.size() > 1)
			{
				// delete specified slot
				g_pAnimSlotList.erase(g_pAnimSlotList.begin() + iDeleteAnimSlot);
				if (g_iCurrentAnimationSlotIndex == iDeleteAnimSlot)
				{
					g_iCurrentAnimationSlotIndex = iDeleteAnimSlot - 1;
				}

				// ensure current animation is a valid slot
				if (g_iCurrentAnimationSlotIndex < 0) g_iCurrentAnimationSlotIndex = 0;
				if (g_iCurrentAnimationSlotIndex >= g_pAnimSlotList.size()) g_iCurrentAnimationSlotIndex = g_pAnimSlotList.size() - 1;

				// ensure model shows current animation index
				g_bUpdateAnimationPreview = true;
				iDeleteAnimSlot = -1;
			}

			// and refresh animation preview if flagged
			if (g_bUpdateAnimationPreview == true)
			{
				// animation slot details
				sAnimSlotStruct* pAnimSlotItem = &(g_pAnimSlotList[g_iCurrentAnimationSlotIndex]);
				if (pAnimSlotItem->fStart == pAnimSlotItem->fFinish)
				{
					fAnimFrameStart = pAnimSlotItem->fStart;
					g_bAnimatingObjectPreview = false;
				}
				if (g_bAnimatingObjectPreview == true)
				{
					if (pAnimSlotItem->bLooped == true)
					{
						SetObjectFrame(objectnumber, pAnimSlotItem->fStart);
						LoopObject(objectnumber, pAnimSlotItem->fStart, pAnimSlotItem->fFinish);
						SetObjectSpeed(objectnumber, fAnimSpeed);
					}
					else
					{
						SetObjectFrame(objectnumber, pAnimSlotItem->fStart);
						PlayObject(objectnumber, pAnimSlotItem->fStart, pAnimSlotItem->fFinish);
						SetObjectSpeed(objectnumber, fAnimSpeed);
					}
				}
				else
				{
					StopObject(objectnumber);
					SetObjectFrame(objectnumber, fAnimFrameStart);
				}
				g_bUpdateAnimationPreview = false;
			}
		}
		ImGui::Indent(-10);
	}
	#ifdef WICKEDENGINE
	else
	{
		// ZJ: Moved this here so the animations will play when the header is closed.
		sObject* pObject = GetObjectData(objectnumber);
		static float fAnimSpeed;
		fAnimSpeed = pObject->fAnimSpeed * 50;

		// and refresh animation preview if flagged
		if (g_bUpdateAnimationPreview == true)
		{
			// animation slot details
			sAnimSlotStruct* pAnimSlotItem = &(g_pAnimSlotList[g_iCurrentAnimationSlotIndex]);
			if (g_bAnimatingObjectPreview == true)
			{
				if (pAnimSlotItem->bLooped == true)
				{
					SetObjectFrame(objectnumber, pAnimSlotItem->fStart);
					LoopObject(objectnumber, pAnimSlotItem->fStart, pAnimSlotItem->fFinish);
					SetObjectSpeed(objectnumber, fAnimSpeed);
				}
				else
					PlayObject(objectnumber, pAnimSlotItem->fStart);
			}
			else
			{
				StopObject(objectnumber);
				SetObjectFrame(objectnumber, pAnimSlotItem->fStart);
			}
			g_bUpdateAnimationPreview = false;
	}
	#endif
	}
}

void animsystem_animationtoolsimpleui(int objectnumber)
{
#ifdef WICKEDENGINE
	extern int iLastOpenHeader;
	if (pref.bAutoClosePropertySections && iLastOpenHeader != 71)
		ImGui::SetNextItemOpen(false, ImGuiCond_Always);

	// If the last open header was the animation tool ui(70) then the user toggled advanced settings so this header should be opened.
	// As they are two different headers, but in the UI they appear to be the same one.
	if (pref.bAutoClosePropertySections && iLastOpenHeader == 70)
		ImGui::SetNextItemOpen(true, ImGuiCond_Always);
#endif

	// object being edited with animation tool
	sObject* pObject = GetObjectData(objectnumber);

	if (ImGui::StyleCollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		iLastOpenHeader = 71;

		// standard numeric box to hold 99999 value
		static int iNumericWidth = 45;

		// if have animations
		ImGui::Indent(10);
		if (bFoundanimSet == NULL)
		{
			ImGui::TextCenter("No Animations Found");
		}
		else
		{
			// prep of animation tool component
			ImGui::PushItemWidth(-10);

			// animation preview
			if (ImGui::Checkbox("Animate Preview", &g_bAnimatingObjectPreview))
			{
				g_bUpdateAnimationPreview = true;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Animate Object Preview");

			// animation speed
			//if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Animation Speed");
			//ImGui::TextCenter("Animation Speed");
			
		}

		//extern void DisplayAdvancedSettings(int&, const char*);
		//DisplayAdvancedSettings(pref.iEnableAdvancedCharacterCreator, "test");

		//ImGui::Indent(10);
		//ImVec2 label_size = ImGui::CalcTextSize("Advanced Settings", NULL, true) + ImVec2(8.0f, 0.0f);
		//ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((ImGui::GetContentRegionAvailWidth()*0.5) - (label_size.x*0.5), 0.0f));
		//if (ImGui::HyberlinkButton("Advanced Settings##1", ImVec2(label_size.x, 0)))
		//{
		//	extern int iSetSettingsFocusTab;
		//	extern bool bPreferences_Window;
		//	iSetSettingsFocusTab = 2;
		//	bPreferences_Window = true;
		//}
		//ImGui::Indent(-10);

		extern void ControlAdvancedSetting(int&, const char*, bool* = 0);
		if (t.importer.importerActive == 0)
			ControlAdvancedSetting(pref.iEnableAdvancedCharacterCreator, "Advanced Character Creator Settings");

		ImGui::Indent(-10);
	}
	
	// ZJ: Moved this here so the animations will play when the header is closed.
	static float fAnimSpeed;
	fAnimSpeed = pObject->fAnimSpeed * 50;

	// and refresh animation preview if flagged
	if (g_bUpdateAnimationPreview == true)
	{
		// animation slot details
		sAnimSlotStruct* pAnimSlotItem = &(g_pAnimSlotList[g_iCurrentAnimationSlotIndex]);
		if (g_bAnimatingObjectPreview == true)
		{
			if (pAnimSlotItem->bLooped == true)
			{
				SetObjectFrame(objectnumber, pAnimSlotItem->fStart);
				LoopObject(objectnumber, pAnimSlotItem->fStart, pAnimSlotItem->fFinish);
				SetObjectSpeed(objectnumber, fAnimSpeed);
			}
			else
				PlayObject(objectnumber, pAnimSlotItem->fStart);
		}
		else
		{
			StopObject(objectnumber);
			SetObjectFrame(objectnumber, pAnimSlotItem->fStart);
		}
		g_bUpdateAnimationPreview = false;
	}
}

bool importer_apply_materialformesh(MaterialComponentTEXTURESLOT eMatSlot, int iGGMeshTexSlot)
{
	// ensure the DBO mesh texture is large enough to hold references
	if (pSelectedMesh && pSelectedMesh->dwTextureCount <= GG_MESH_TEXTURE_SURFACE)
	{
		// if texture storage too small, increase it to store our references!
		extern bool EnsureTextureStageValid (sMesh* pMesh, int iTextureStage);
		EnsureTextureStageValid (pSelectedMesh, GG_MESH_TEXTURE_SURFACE);
	}
	if (pSelectedMaterial && !pSelectedMesh)
	{
		pSelectedMaterial->textures[eMatSlot].resource = nullptr;
		pSelectedMaterial->textures[eMatSlot].name = "";
		pSelectedMaterial->SetDirty();
		wiJobSystem::context ctx;
		wiJobSystem::Wait(ctx);
	}
	if (pSelectedMesh)
	{
		cStr tOldDir = GetDir();
		char * cFileSelected;
		if (strlen(cPreSelectedFile) > 0)
			cFileSelected = &cPreSelectedFile[0];
		else
		{
		
			bool bOpenExplorerAtPrevLocation = true;

			if (bChooseSurfaceChannel)
			{
				bOpenExplorerAtPrevLocation = false;
				bChooseSurfaceChannel = false;
			}

			cFileSelected = importer_selectfile(iGGMeshTexSlot, pSelectedMaterial->textures[iGGMeshTexSlot].name, bOpenExplorerAtPrevLocation);
		}

		// When applying a single texture to all meshes, this function will be called multiple times...
		if (t.importer.bEditAllMesh)
		{
			// Early return if user cancels file dialog, to ensure they don't need to cancel repeatedly.
			if (!cFileSelected) return false;

			// ...copy the selected file, to prevent opening the file dialog multiple times.
			strcpy(cPreSelectedFile, cFileSelected);
		}
		
		SetDir(tOldDir.Get());
		if (cFileSelected && strlen(cFileSelected) > 0)
		{
			// check if texture in the current texture path (where object and its textures are located)
			LPSTR pSelectedFilenameOnly = cFileSelected;
			char pDetermineSelectedPath[MAX_PATH];
			strcpy(pDetermineSelectedPath, cFileSelected);
			for (int n = strlen(pDetermineSelectedPath); n > 0; n--)
			{
				if (pDetermineSelectedPath[n] == '\\' || pDetermineSelectedPath[n] == '/')
				{
					pSelectedFilenameOnly = cFileSelected + n + 1;
					pDetermineSelectedPath[n+1] = 0;
					break;
				}
			}
			extern std::string g_pWickedTexturePath;
			LPSTR pWickedTexturePath = (LPSTR)g_pWickedTexturePath.c_str();
			if (strnicmp (pDetermineSelectedPath + strlen(pDetermineSelectedPath) - strlen(pWickedTexturePath), pWickedTexturePath, strlen(pWickedTexturePath)) != NULL)
			{
				// if not, we need to copy this selected texture into that area (writable folder) so that the object
				// can find this texture locally in the future, but ensure we do not overwrite anything, so can rename here
				char pNewFileLocation[MAX_PATH];
				strcpy (pNewFileLocation, g.fpscrootdir_s.Get());	//g.fpscrootdir_s.Get() returns the full path to the texture when in the importer.
				strcat (pNewFileLocation, "\\Files\\");
				strcat (pNewFileLocation, pWickedTexturePath);
				strcat (pNewFileLocation, pSelectedFilenameOnly);
				GG_GetRealPath(pNewFileLocation, 1); //This appears to get the writable area folder, but the path ends up containing the path that the user selected.
				// ^ An incorrect path is only generated when in the model importer, when editing custom materials in the object properties...
				// ... the path is perfectly fine.
				int iRenamedFile = 2;
				while (FileExist(pNewFileLocation) == 1)
				{
					// if need to rename, find a unique name
					char pExt[5];
					char pNewFilenameNoExt[MAX_PATH];
					strcpy (pNewFilenameNoExt, pSelectedFilenameOnly);
					strcpy(pExt, pNewFilenameNoExt + strlen(pNewFilenameNoExt) - 4);
					pNewFilenameNoExt[strlen(pNewFilenameNoExt) - 4] = 0;
					char pNewFilenameOnly[MAX_PATH];
					sprintf (pNewFilenameOnly, "%s%d%s", pNewFilenameNoExt, iRenamedFile, pExt);
					strcpy (pNewFileLocation, g.fpscrootdir_s.Get());
					strcat (pNewFileLocation, "\\Files\\");
					strcat (pNewFileLocation, pWickedTexturePath);
					strcat (pNewFileLocation, pNewFilenameOnly);
					GG_GetRealPath(pNewFileLocation, 1);
					iRenamedFile++;
				}
				CopyFileA(cFileSelected, pNewFileLocation, TRUE);

				if(t.importer.importerActive == 0)
					cFileSelected = pNewFileLocation;	// Wrong file path in the importer only.
			}

			// apply texture file to material
			pSelectedMaterial->textures[eMatSlot].name = cFileSelected;
			pSelectedMaterial->textures[eMatSlot].resource = WickedCall_LoadImage(pSelectedMaterial->textures[eMatSlot].name);
			if (pSelectedMaterial->textures[eMatSlot].resource)
			{
				// loaded okay, update material
				char* pTextureFilename = pSelectedMesh->pTextures[iGGMeshTexSlot].pName;
				strcpy(pTextureFilename, cFileSelected);
				switch (iGGMeshTexSlot)
				{
				case GG_MESH_TEXTURE_NORMAL : 
					pSelectedMaterial->SetNormalMapStrength(1.0f);
					t.importer.bInvertNormalMap = false;
					strcpy(t.importer.pOrigNormalMap, cFileSelected);
					break;
				case GG_MESH_TEXTURE_SURFACE : 
					pSelectedMaterial->SetRoughness(1.0f);
					pSelectedMaterial->SetMetalness(1.0f);
					pSelectedMaterial->SetOcclusionEnabled_Primary(true);
					pSelectedMaterial->SetOcclusionEnabled_Secondary(false);
					break;
				case GG_MESH_TEXTURE_EMISSIVE : 
					pSelectedMaterial->SetEmissiveStrength(1.0f);
					break;
				}
				pSelectedMaterial->SetDirty();
				wiJobSystem::context ctx;
				wiJobSystem::Wait(ctx);
			}
			else
			{
				// failed to load, reset slot
				pSelectedMaterial->textures[eMatSlot].resource = nullptr;
				pSelectedMaterial->textures[eMatSlot].name = "";
				pSelectedMaterial->SetDirty();
				wiJobSystem::context ctx;
				wiJobSystem::Wait(ctx);
			}
		}
	}

	return true;
}

LPSTR animsystem_getweapontype (LPSTR pSelectedWeapon, LPSTR pAnimSetOverride)
{
	LPSTR pWeaponType = "-melee";//""; no weapon MUST be melee!!
	/* hard coding removed and moved to gunspec as "animsetoverride	= -xxxx"
	if (stricmp(pSelectedWeapon, "enhanced\\Gloves_Unarmed") == NULL) pWeaponType = "-melee";//pWeaponType = "";
	if (stricmp(pSelectedWeapon, "enhanced\\B810") == NULL) pWeaponType = "-melee";//pWeaponType = "";
	if (stricmp(pSelectedWeapon, "enhanced\\SledgeHammer") == NULL) pWeaponType = "-melee";//pWeaponType = "";
	if (stricmp(pSelectedWeapon, "enhanced\\Mk19T") == NULL) pWeaponType = ""; //pistol
	if (stricmp(pSelectedWeapon, "enhanced\\M29S") == NULL) pWeaponType = ""; //pistol
	if (stricmp(pSelectedWeapon, "enhanced\\AK") == NULL) pWeaponType = "-rifle";
	if (stricmp(pSelectedWeapon, "enhanced\\AR") == NULL) pWeaponType = "-rifle";
	if (stricmp(pSelectedWeapon, "enhanced\\Mk18") == NULL) pWeaponType = "-rifle";
	if (stricmp(pSelectedWeapon, "enhanced\\R870") == NULL) pWeaponType = "-rifle";
	*/

	// can override from gunspec file now
	if (strlen(pAnimSetOverride) > 0)
	{
		if (stricmp(pAnimSetOverride, "-pistol") == NULL) 
			pWeaponType = "";
		else
			pWeaponType = pAnimSetOverride;
	}

	// correct animset to use
	return pWeaponType;
}

void animsystem_weaponproperty (int characterbasetype, bool readonly, entityeleproftype* edit_grideleprof, bool bForShooting, bool bForMelee)
{
	// weapon selection for character properties (object editing and character creator use this)
	LPSTR pAttachmentTitle = "Weapon";
	cstr sCurrentWeapon = edit_grideleprof->hasweapon_s;
	extern char* imgui_setpropertylist2c_v2(int, int, char*, char*, char*, int, bool, bool, bool, bool, int);
	edit_grideleprof->hasweapon_s = imgui_setpropertylist2c_v2(t.group, t.controlindex, edit_grideleprof->hasweapon_s.Get(), pAttachmentTitle, t.strarr_s[209].Get(), 1, readonly, true, bForShooting, bForMelee, 0);
	LPSTR pSelectedWeapon = edit_grideleprof->hasweapon_s.Get();
	if (stricmp(pSelectedWeapon, sCurrentWeapon.Get()) != NULL)
	{
		// iof weapon changes, refresh any overrideanimset_s that might be set
		edit_grideleprof->overrideanimset_s = "";
		extern bool g_bNowPopulateWithCorrectAnimSet;
		g_bNowPopulateWithCorrectAnimSet = true;
	}
	if (strlen(pSelectedWeapon) > 0)
	{
		// more freedom granted
		// check if weapon animations available
		//t.findgun_s = Lower(pSelectedWeapon);
		//gun_findweaponindexbyname ();
		//int gunid = t.foundgunid;
		//LPSTR pWeaponType = animsystem_getweapontype(pSelectedWeapon, t.gun[gunid].animsetoverride.Get());
		//LPSTR pGender = "adult male";
		//if (characterbasetype == 1) pGender = "adult female";
		//if (characterbasetype == 2) pGender = "zombie male";
		//if (characterbasetype == 3) pGender = "zombie female";
		//char pPathToWeaponAnim[MAX_PATH];
		//sprintf(pPathToWeaponAnim, "charactercreatorplus\\parts\\%s\\default animations%s.dbo", pGender, pWeaponType);
		//if (FileExist(pPathToWeaponAnim) == 0)
		//{
		//	edit_grideleprof->hasweapon_s = sCurrentWeapon;
		//	strcpy (cTriggerMessage, "Character animations not available to equip this weapon");
		//	bTriggerMessage = true;
		//}
	}

	// allow player to take weapon (and ammo)
	t.tfile_s = cstr("gamecore\\guns\\") + edit_grideleprof->hasweapon_s + cstr("\\HUD.dbo");
	if (FileExist(t.tfile_s.Get()) == 1)
	{
		bool bCanTakeWeapon = edit_grideleprof->cantakeweapon;
		if (ImGui::Checkbox("Player Can Take Weapon", &bCanTakeWeapon))
		{
			edit_grideleprof->cantakeweapon = bCanTakeWeapon;
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Tick to allow the player to take the weapon when this character dies");
		if (bCanTakeWeapon)
		{
			// only if weapon shoots, no sense for melee weapons
			t.findgun_s = Lower(edit_grideleprof->hasweapon_s.Get());
			gun_findweaponindexbyname ();
			int gunid = t.foundgunid;
			if (g.firemodes[gunid][0].settings.reloadqty != 0 || g.firemodes[gunid][1].settings.reloadqty != 0 )
			{
				ImGui::TextCenter("Taken Weapon Ammo");
				ImGui::MaxSliderInputInt("##ammo", &edit_grideleprof->quantity, 0, 1000, "The amount of ammo the weapon will have when picked up");
			}
		}
	}
}

int g_iSkipFrameBeforeResetAsLUADescChanges = 0;
bool g_bNowPopulateWithCorrectAnimSet = false;

void animsystem_animationsetproperty (int characterbasetype, bool readonly, entityeleproftype* edit_grideleprof, int iAnimationSetType, int elementID)
{
	// <Animation Set> included in LUA, so allow user to change the underlying animation set for this object
	// iAnimationSetType : 1-soldier, 2-melee, 3-zombie, 4-default
	bool bRefreshObjectAnimationSet = false;
	if (g_bNowPopulateWithCorrectAnimSet == true)
	{
		LPSTR pCurrentWeapon = edit_grideleprof->hasweapon_s.Get();
		LPSTR pWeaponType = "";
		if (strlen(pCurrentWeapon) > 0)
		{
			t.findgun_s = Lower(pCurrentWeapon);
			gun_findweaponindexbyname ();
			int gunid = t.foundgunid;
			pWeaponType = animsystem_getweapontype(pCurrentWeapon, t.gun[gunid].animsetoverride.Get());
		}
		LPSTR pGender = "";
		if (iAnimationSetType == 1 || iAnimationSetType == 2)
		{
			// soldier or melee
			pGender = "adult male";
			if (characterbasetype == 1 || characterbasetype == 3) pGender = "adult female";

			// melee will need melee animations if no specific weapon
			if (iAnimationSetType == 2)
			{
				if (strlen(pWeaponType) == 0 ) pWeaponType = "-melee";
			}
		}
		if (iAnimationSetType == 3)
		{
			// zombie
			pGender = "zombie male";
			if (characterbasetype == 1 || characterbasetype == 3) pGender = "zombie female";
		}
		if (iAnimationSetType == 4)
		{
			// default animations keeps field blank so can specify any anim (even non characters)
		}
		else
		{
			char pPathToWeaponAnim[MAX_PATH];
			sprintf(pPathToWeaponAnim, "charactercreatorplus\\parts\\%s\\default animations%s.dbo", pGender, pWeaponType);
			if (FileExist(pPathToWeaponAnim) == 0)
			{
				// if not weapon specific animation set, use regular base default
				sprintf(pPathToWeaponAnim, "charactercreatorplus\\parts\\%s\\default animations.dbo", pGender);
			}
			if (FileExist(pPathToWeaponAnim))
			{
				// correct default for base type and weapon held
				edit_grideleprof->overrideanimset_s = pPathToWeaponAnim;
			}
		}

		// in any event, refresh object when change behavior (and associated anim)
		g_bNowPopulateWithCorrectAnimSet = false;
		bRefreshObjectAnimationSet = true;
	}

	// standard anim choices or custom animset file 
	cstr newAnimSetFile_s;
	extern int g_iDevToolsOpen;
	if (g_iDevToolsOpen == 0 && iAnimationSetType != 4)
	{
		int iSpecialValue = 0;
		if (iAnimationSetType == 1)
		{
			if (characterbasetype == 0 || characterbasetype == 2)
			{
				iSpecialValue = 1;
			}
			else
			{
				iSpecialValue = 3;
			}
		}
		if (iAnimationSetType == 2)
		{
			if (characterbasetype == 0 || characterbasetype == 2)
			{
				iSpecialValue = 2;
			}
			else
			{
				iSpecialValue = 4;
			}
		}
		if (iAnimationSetType == 3)
		{
			if (characterbasetype == 0 || characterbasetype == 2)
			{
				iSpecialValue = 5;
			}
			else
			{
				iSpecialValue = 6;
			}
		}
		if (iSpecialValue > 0)
		{
			// standard users only see choices if a character base type
			extern char* imgui_setpropertylist2c_v2(int, int, char*, char*, char*, int, bool, bool, bool, bool, int);
			newAnimSetFile_s = imgui_setpropertylist2c_v2(t.group, t.controlindex, edit_grideleprof->overrideanimset_s.Get(), "Animation Choice", "Overrides the default animations used by default", 2, readonly, true, false, false, iSpecialValue);

			/* easier to add 'original animation' to drop down for "-" mode
			// if default, offer option to match to weapon selected (legacy behavior and the default mode)
			if (strlen(edit_grideleprof->overrideanimset_s.Get()) <= 1)
			{
				bool bDefaultToWeaponType = true;
				if (strcmp (edit_grideleprof->overrideanimset_s.Get(), "-") == NULL) bDefaultToWeaponType = false;
				if (ImGui::Checkbox("Default To Weapon Type", &bDefaultToWeaponType))
				{
					if (bDefaultToWeaponType == true)
						edit_grideleprof->overrideanimset_s = "";
					else
						edit_grideleprof->overrideanimset_s = "-";
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Tick to select a default animation that works best with chosen weapon");
			}
			*/
		}
		else
		{
			// default animations keep their last selection (and can also be blank)
			newAnimSetFile_s = edit_grideleprof->overrideanimset_s;
		}
	}
	else
	{
		// advanced users always have option to change animset file or non character specific
		if (strcmp(edit_grideleprof->overrideanimset_s.Get(), "-") == NULL)
		{
			// will use animation stored in DBO
			bool bUseDefaultAnimation = true;
			if (ImGui::Checkbox("Use Default Animation", &bUseDefaultAnimation))
			{
				// allow user to specify override
				newAnimSetFile_s = "";
			}
			else
			{
				// continue using default anim
				newAnimSetFile_s = edit_grideleprof->overrideanimset_s;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Untick to disable default animation and specify your own animation file");
		}
		else
		{
			// can enter own animation file
			extern char * imgui_setpropertyfile2_v2(int group, char* data_s, char* field_s, char* desc_s, char* within_s, bool readonly, char *startsearch);
			newAnimSetFile_s = imgui_setpropertyfile2_v2(t.group, edit_grideleprof->overrideanimset_s.Get(), "Animation Choice", "Overrides the default animation set with a custom choice", "charactercreatorplus\\animations", readonly, 0);

			// user can opt to use default
			bool bUseDefaultAnimation = false;
			if (ImGui::Checkbox("Use Default Animation", &bUseDefaultAnimation))
			{
				// allow user to specify override
				newAnimSetFile_s = "-";
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Tick to use the default animation stored in the object");
		}
	}
	if (stricmp(newAnimSetFile_s.Get(), edit_grideleprof->overrideanimset_s.Get()) != NULL)
	{
		// animation set changed, so refresh object so can get anim set names if needed (and see anim in level editor)
		edit_grideleprof->overrideanimset_s = newAnimSetFile_s;
		bRefreshObjectAnimationSet = true;

		// and trigger DLUA to refresh in case DisplayFPEBehavior needs to display latest animset names
		extern int fpe_current_loaded_script;
		fpe_current_loaded_script = -1;
	}
	if ( bRefreshObjectAnimationSet == true && elementID > 0)
	{
		// replace actual object animations
		int iObjID = t.entityelement[elementID].obj;
		if (iObjID > 0)
		{
			LPSTR pWeaponAnimFile = edit_grideleprof->overrideanimset_s.Get();
			if (strlen(pWeaponAnimFile) > 1) // "" = default to weapon type, "-" = default to object anim
			{
				if (!FileExist(pWeaponAnimFile))
				{
					strcpy(pWeaponAnimFile, "");
				}
			}
			//LB: this must match what happens when the test level is run, that is, with ZERO override, use the default anim sets!!
			//if (strlen(pWeaponAnimFile) <= 1 && strlen(edit_grideleprof->overrideanimset_s.Get()) > 1) // "" = default to weapon type, "-" = default to object anim
			if (strlen(pWeaponAnimFile) == 0 && strlen(edit_grideleprof->overrideanimset_s.Get()) != 1) // "" = default to weapon type, "-" = default to object anim
			{
				LPSTR pCurrentWeapon = edit_grideleprof->hasweapon_s.Get();
				LPSTR pWeaponType = "";
				if (strlen(pCurrentWeapon) > 0)
				{
					t.findgun_s = Lower(pCurrentWeapon);
					gun_findweaponindexbyname ();
					int gunid = t.foundgunid;
					pWeaponType = animsystem_getweapontype(pCurrentWeapon, t.gun[gunid].animsetoverride.Get());
				}
				LPSTR pGender = NULL;
				if (characterbasetype == 0) pGender = "adult male";
				if (characterbasetype == 1) pGender = "adult female";
				if (characterbasetype == 2) pGender = "zombie male";
				if (characterbasetype == 3) pGender = "zombie female";
				if (pGender != NULL)
				{
					if (t.entityprofile[t.entid].characterbasetype >= 0 && t.entityprofile[t.entid].characterbasetype <= 1)
						sprintf(pWeaponAnimFile, "charactercreatorplus\\parts\\%s\\default animations%s.dbo", pGender, pWeaponType);
					else
						sprintf(pWeaponAnimFile, "charactercreatorplus\\parts\\%s\\default animations.dbo", pGender);
				}
			}

			// when get the final anim file we need, append it
			if (strlen(pWeaponAnimFile) > 1) // "" = default to weapon type, "-" = default to object anim
			{
				if (FileExist(pWeaponAnimFile))
				{
					// appended animation
					sObject* pObject = GetObjectData(iObjID);
					AppendObject(pWeaponAnimFile, iObjID, 0);
					WickedCall_RefreshObjectAnimations(pObject, pObject->wickedloaderstateptr);
				}
			}
			else
			{
				// obtain the original animation from the object (in cases where user has created their own character an anims)
				int entid = t.entityelement[elementID].bankindex;
				if (entid > 0)
				{
					char pOrigModelPath[MAX_PATH];
					strcpy(pOrigModelPath, t.entitybank_s[entid].Get());
					for (int n = strlen(pOrigModelPath) - 1; n > 0; n--)
					{
						if (pOrigModelPath[n] == '\\' || pOrigModelPath[n] == '/')
						{
							pOrigModelPath[n] = 0;
							break;
						}
					}
					char pOrigModelFile[MAX_PATH];
					strcpy(pOrigModelFile, "entitybank\\");
					strcat(pOrigModelFile, pOrigModelPath);
					strcat(pOrigModelFile, "\\");
					strcat(pOrigModelFile, t.entityprofile[entid].model_s.Get());
					sObject* pObject = GetObjectData(iObjID);
					AppendObject(pOrigModelFile, iObjID, 0);
					WickedCall_RefreshObjectAnimations(pObject, pObject->wickedloaderstateptr);
				}
			}

			// and fire up first frame on the change
			SetObjectFrame (iObjID, 0); LoopObject (iObjID); StopObject (iObjID);

			// and apply correct starter animation as a preview
			extern void entity_loop_using_negative_playanimineditor(int e, int obj, cstr animname);
			entity_loop_using_negative_playanimineditor(elementID, iObjID, t.entityprofile[t.entityelement[elementID].bankindex].playanimineditor_name);
		}
	}
}

#endif

#ifdef WICKEDENGINE
void importer_applyframezerooffsets (int objectnumber, float fX, float fY, float fZ, float fRX, float fRY, float fRZ)
{
	sObject* pObject = g_ObjectList[objectnumber];
	if (pObject)
	{
		sFrame* pFrame = pObject->pFrame;
		if (pFrame)
		{
			// using first frame orientation to control an object wide adjustment (from importer (or from DBO))
			pFrame->vecOffset = GGVECTOR3(fX, fY, fZ);
			pFrame->vecRotation = GGVECTOR3(fRX, fRY, fRZ);
		}
		WickedCall_UpdateObject(pObject);
	}
}

void UpdateObjectWithAnimSlotList ( sObject* pObject )
{
	sAnimationSet* pPrevAnimSet = NULL;
	sAnimationSet* pAnimSetPtr = pObject->pAnimationSet;
	for (int slot = 0; slot < (int)g_pAnimSlotList.size(); slot++)
	{
		// sync UI anim list with animset list in DBO
		if (pAnimSetPtr == NULL)
		{
			sAnimationSet* pNewAnimSetForRef = new sAnimationSet();
			memset(pNewAnimSetForRef, 0, sizeof(pNewAnimSetForRef));
			pNewAnimSetForRef->ulLength = g_pAnimSlotList[slot].fFinish - g_pAnimSlotList[slot].fStart;
			strcpy(pNewAnimSetForRef->szName, g_pAnimSlotList[slot].pName);
			if(pPrevAnimSet) //PE: Got a crash here pPrevAnimSet == NULL.
				pPrevAnimSet->pNext = pNewAnimSetForRef;
			pAnimSetPtr = pNewAnimSetForRef;
		}
		else
		{
			strcpy(pAnimSetPtr->szName, g_pAnimSlotList[slot].pName);
		}
		if (slot > 0)
		{
			// 124 = reference animset (not containing animation data, only references main animset zero core)
			pAnimSetPtr->dwAnimSetType = 124;
			pAnimSetPtr->fAnimSetStart = g_pAnimSlotList[slot].fStart;
			pAnimSetPtr->fAnimSetFinish = g_pAnimSlotList[slot].fFinish;
			pAnimSetPtr->fAnimSetStep1 = g_pAnimSlotList[slot].fStep1;
			pAnimSetPtr->fAnimSetStep2 = g_pAnimSlotList[slot].fStep2;
			pAnimSetPtr->fAnimSetStep3 = g_pAnimSlotList[slot].fStep3;
		}
		pPrevAnimSet = pAnimSetPtr;
		pAnimSetPtr = pAnimSetPtr->pNext;
	}
	if (pAnimSetPtr && pPrevAnimSet)
	{
		// still have old animsets in DBO, remove them
		pPrevAnimSet->pNext = NULL;
		while (pAnimSetPtr)
		{
			sAnimationSet* pNextOne = pAnimSetPtr->pNext;
			pAnimSetPtr->pNext = NULL;
			delete pAnimSetPtr;
			pAnimSetPtr = pNextOne;
		}
	}
}
#endif

void imgui_importer_loop(void)
{
	switch (iDelayedExecute) 
	{
		case 2: //Quit Importer, need to be done heree , so we dont have any images in the draw call list.
		{
			iDelayedExecute = 0;
			importer_quit();
			bImporter_Window = false;
			break;
		}
		case 1:
		{
			iDelayedExecute = 0;
			t.tFileName_s = openFileBox("All Files|*.*|PNG|*.png|DDS|*.dds|JPEG|*.jpg|BMP|*.bmp|", "", "Open Texture", ".dds", IMPORTEROPENFILE);
			if (t.tFileName_s == "Error")  return;
			if (FileExist(t.tFileName_s.Get()) == 1)
			{
				popup_text("Loading chosen texture and associated files");

				// find free image
				t.tImageID = t.importerTextures[iDelayedExecuteSelection].imageID;
				if (t.tImageID == 0)
				{
					t.tImageID = g.importermenuimageoffset + 15;
					while (ImageExist(t.tImageID) == 1) ++t.tImageID;
				}

				// can expand out a color texture once (to add normal/gloss/etc)
				bool bExpandOutPBRTextureSet = false;

				// replace image details
				if (ImageExist(t.tImageID) == 1) DeleteImage(t.tImageID);
				LoadImage(t.tFileName_s.Get(), t.tImageID);
				if (ImageExist(t.tImageID) == 1)
				{
					// remove any previous references to associated files for the old filename
					if (t.importerTextures[iDelayedExecuteSelection].iExpandedThisSlot == 0)
					{
						// but only if its a base color texutre
						char pIsItTexColor[2048];
						strcpy(pIsItTexColor, t.importerTextures[iDelayedExecuteSelection].fileName.Get());
						if (strlen(pIsItTexColor) > 1 + 8 + 4)
						{
							pIsItTexColor[strlen(pIsItTexColor) - 4] = 0;
							if (strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 2, "_d", 2) == NULL
								|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 6, "_color", 6) == NULL
								|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 8, "_diffuse", 8) == NULL
								|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 7, "_albedo", 7) == NULL
								|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 8, "blankTex", 8) == NULL)
							{
								// for both!
								strcpy(pIsItTexColor, t.tFileName_s.Get());
								if (strlen(pIsItTexColor) > 1 + 8 + 4)
								{
									pIsItTexColor[strlen(pIsItTexColor) - 4] = 0;
									if (strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 2, "_d", 2) == NULL
										|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 6, "_color", 6) == NULL
										|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 8, "_diffuse", 8) == NULL
										|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 7, "_albedo", 7) == NULL)
									{
										importer_removeentryandassociatesof(iDelayedExecuteSelection);
										t.importerTextures[iDelayedExecuteSelection].iExpandedThisSlot = 1;
										bExpandOutPBRTextureSet = true;
									}
								}
							}
						}
					}

					// update image list data
					t.importerTextures[iDelayedExecuteSelection].fileName = t.tFileName_s;
					t.importerTextures[iDelayedExecuteSelection].imageID = t.tImageID;
				}

				int iImageCount = 0;
				for (int iCount = 1; iCount <= IMPORTERTEXTURESMAX; iCount++)
					if (strlen(t.importerTextures[iCount].fileName.Get()) > 0)
						iImageCount++;

				// ensure single texture is specified in FPE
				if (iImageCount == 1)
				{
					t.importer.objectFPE.textured = t.tFileName_s;
				}

				// reapply texture to model
				importer_applyimagelisttextures(false, iDelayedExecuteSelection, bExpandOutPBRTextureSet);
				importer_recreate_texturesprites();

				iImporterGenerateThumb = 0;
			}
			break;
		}
		case 3: //Save entity
		{
			iDelayedExecute = 0;
			t.timportersaveon = 1;
			cstr pFillFilename = cstr(cImportPath);
			if (cImportPath[strlen(cImportPath) - 1] != '\\')
				pFillFilename = pFillFilename + "\\";

			pFillFilename = pFillFilename + cImportName + ".dbo";

			#ifdef WICKEDENGINE
			// better way
			if (cImportPath[1] != ':') //Dont resolve if not using relative path.
			{
				char resolved[MAX_PATH];
				strcpy(resolved, g.fpscrootdir_s.Get());
				strcat(resolved, "\\Files\\");
				strcat(resolved, pFillFilename.Get());
				pFillFilename = resolved;
			}
			#else
			//Resolve path.
			char resolved[MAX_PATH];
			int retval = GetFullPathNameA(pFillFilename.Get(), MAX_PATH, resolved, NULL);
			if (retval > 0) pFillFilename = resolved;
			#endif

			#ifdef WICKEDENGINE
			bool bShouldSave = true;
			if(FileExist(pFillFilename.Get()))
				bShouldSave = overWriteFileBox(pFillFilename.Get());

			if (!bShouldSave)
				break;
			#endif

			importer_save_entity(pFillFilename.Get());
			if (t.tSaveFile_s == "Error") 
			{
				//Failed. ? or cancel ?
				#ifdef WICKEDENGINE
				popup_text("Cancel Save Object");
				#else
				popup_text("Cancel Save Entity");
				#endif
			}
			else 
			{
				#ifdef WICKEDENGINE
				//iDelayedExecute = 0; //PE: Setup for object library back button. This will crash as we still reuse images.
				iDelayedExecute = 2;
				#else
				iDelayedExecute = 2; // quit
				#endif
			}
			break;
		}
		case 4 : 
		{
			// trigger importer to quit, then reload with last loaded model (used when scaling mode changes)
			iDelayedExecute = 0; 
			importer_storeobjectdata();
			extern char pLaunchAfterSyncPreSelectModel[MAX_PATH];
			strcpy (pLaunchAfterSyncPreSelectModel, "");
			extern void importer_quit_for_reload (LPSTR pOptionalCopyModelFile);
			importer_quit_for_reload(pLaunchAfterSyncPreSelectModel); 
			extern int iLaunchAfterSync;
			iLaunchAfterSync = 8;
			break;
		}

		#ifdef WICKEDENGINE
		case 30: //Select baseColorMap Material
		{
			bHaveMaterialUpdate = true;

			importer_apply_materialformesh(MaterialComponentTEXTURESLOT::BASECOLORMAP, GG_MESH_TEXTURE_DIFFUSE);

			// auto generate other tex references under certain conditions
			char* pTextureFilename = pSelectedMesh->pTextures[GG_MESH_TEXTURE_DIFFUSE].pName;
			bool bIfColorValid = false;
			if (pSelectedMesh)
			{
				if (pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].resource)
				{
					// check if we can auto fill other tex refs
					if (strstr(pTextureFilename, "_color") != NULL) bIfColorValid = true;
				}
			}
			if (bIfColorValid == true)
			{
				// check if all other fields blank
				bool bAndAllOtherTexRefsEmptyAutoFill = false;
				if (pSelectedMesh->dwTextureCount > 1)
				{
					// scan all tex refs, all need to be blank
					bAndAllOtherTexRefsEmptyAutoFill = true;
					for (int n = 1; n < pSelectedMesh->dwTextureCount; n++)
					{
						if (strlen(pSelectedMesh->pTextures[n].pName) > 0)
						{
							// tex refs populated, no leave refs alone
							bAndAllOtherTexRefsEmptyAutoFill = false;
						}
					}
				}
				else
				{
					// texture array of one means no other tex refs
					bAndAllOtherTexRefsEmptyAutoFill = true;
				}
				if (bAndAllOtherTexRefsEmptyAutoFill == true)
				{
					// this model has blank tex refs, can auto fill

					// ensure the DBO mesh texture is large enough to hold references
					if (pSelectedMesh->dwTextureCount <= GG_MESH_TEXTURE_SURFACE)
					{
						// if texture storage too small, increase it to store our references!
						extern bool EnsureTextureStageValid(sMesh* pMesh, int iTextureStage);
						EnsureTextureStageValid(pSelectedMesh, GG_MESH_TEXTURE_SURFACE);
					}

					// finds base tex ref name
					char pNoExtFilename[MAX_PATH];
					strcpy(pNoExtFilename, pTextureFilename);
					pNoExtFilename[strlen(pNoExtFilename) - 4] = 0;
					LPSTR pNoColorPtr = strstr(pNoExtFilename, "_color"); if (pNoColorPtr != NULL) *pNoColorPtr = 0;

					// populate with auto tex refs (if present)
					char pTryTexRef[MAX_PATH];
					sprintf(pTryTexRef, "%s_normal.dds", pNoExtFilename);
					if (FileExist(pTryTexRef) == 1)
					{
						strcpy(cPreSelectedFile, pTryTexRef);
						importer_apply_materialformesh(MaterialComponentTEXTURESLOT::NORMALMAP, GG_MESH_TEXTURE_NORMAL);
					}
					sprintf(pTryTexRef, "%s_surface.dds", pNoExtFilename);
					if (FileExist(pTryTexRef) == 1)
					{
						// surface and RMA references all in one file
						strcpy(cPreSelectedFile, pTryTexRef);
						importer_apply_materialformesh(MaterialComponentTEXTURESLOT::SURFACEMAP, GG_MESH_TEXTURE_SURFACE);
						strcpy(pSelectedMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName, pTryTexRef);
						pSelectedMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].channelMask = (15) + (1 << 4);
						strcpy(pSelectedMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName, pTryTexRef);
						pSelectedMesh->pTextures[GG_MESH_TEXTURE_METALNESS].channelMask = (15) + (2 << 4);
						strcpy(pSelectedMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].pName, pTryTexRef);
						pSelectedMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].channelMask = (15) + (0 << 4);
					}
					else
					{
						// do not look for non-surface files, they could be anything!
					}
					sprintf(pTryTexRef, "%s_emissive.dds", pNoExtFilename);
					if (FileExist(pTryTexRef) == 1)
					{
						strcpy(cPreSelectedFile, pTryTexRef);
						importer_apply_materialformesh(MaterialComponentTEXTURESLOT::EMISSIVEMAP, GG_MESH_TEXTURE_EMISSIVE);
					}
				}
			}
			t.importer.bMeshesHaveDifferentBase = true;
			iDelayedExecute = 0;
			break;
		}
		case 31: //Select NormalMap Material
		{
			bHaveMaterialUpdate = true;
			importer_apply_materialformesh(MaterialComponentTEXTURESLOT::NORMALMAP, GG_MESH_TEXTURE_NORMAL);
			t.importer.bMeshesHaveDifferentNormal = true;
			iDelayedExecute = 0;
			break;
		}
		case 32: //Select surfaceMap Material
		{
			bHaveMaterialUpdate = true;
			importer_apply_materialformesh(MaterialComponentTEXTURESLOT::SURFACEMAP, GG_MESH_TEXTURE_SURFACE);
			t.importer.bMeshesHaveDifferentSurface = true;
			iDelayedExecute = 0;
			break;
		}

		case 33: //Select displacementMap Material
		{
			bHaveMaterialUpdate = true;
			if (pSelectedMaterial && !pSelectedMesh) 
			{
				pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].resource = nullptr;
				pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].name = "";
				pSelectedMaterial->SetDirty();
				wiJobSystem::context ctx;
				wiJobSystem::Wait(ctx);
			}

			if (pSelectedMesh)
			{
				char* pTextureFilename = pSelectedMesh->pTextures[0].pName;
				iDelayedExecute = 0;

				cStr tOldDir = GetDir();
				char * cFileSelected;
				if (strlen(cPreSelectedFile) > 0)
					cFileSelected = &cPreSelectedFile[0];
				else
					cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "All\0*.*\0DDS\0*.dds\0PNG\0*.png\0JPEG\0*.jpg\0TGA\0*.tga\0BMP\0*.bmp\0\0\0", NULL, NULL);

				SetDir(tOldDir.Get());
				if (cFileSelected && strlen(cFileSelected) > 0)
				{
					pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].name = cFileSelected;
					pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].resource = WickedCall_LoadImage(pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].name);
					if (pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].resource)
					{
						//worked activate.
						pSelectedMaterial->SetDisplacementMapping(0.1f); //Default.
						pSelectedMaterial->SetDirty();
						wiJobSystem::context ctx;
						wiJobSystem::Wait(ctx);
					}
					else 
					{
						//Failed reset slot.
						//strcpy(pTextureFilename, ""); //update mesh texture.
						pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].resource = nullptr;
						pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].name = "";
						pSelectedMaterial->SetDirty();
						wiJobSystem::context ctx;
						wiJobSystem::Wait(ctx);
					}
				}
			}
			iDelayedExecute = 0;
			break;
		}

		case 34: //Select emissiveMap Material
		{
			bHaveMaterialUpdate = true;
			importer_apply_materialformesh(MaterialComponentTEXTURESLOT::EMISSIVEMAP, GG_MESH_TEXTURE_EMISSIVE);
			t.importer.bMeshesHaveDifferentEmissive = true;
			iDelayedExecute = 0;
			break;
		}

		case 35: //Select occlusionMap Material
		{
			bHaveMaterialUpdate = true;
			if (pSelectedMaterial && !pSelectedMesh) 
			{
				pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].resource = nullptr;
				pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].name = "";
				pSelectedMaterial->SetOcclusionEnabled_Primary(true);
				pSelectedMaterial->SetOcclusionEnabled_Secondary(false);
				pSelectedMaterial->SetDirty();
				wiJobSystem::context ctx;
				wiJobSystem::Wait(ctx);
			}

			if (pSelectedMesh)
			{
				char* pTextureFilename = pSelectedMesh->pTextures[0].pName;
				iDelayedExecute = 0;

				cStr tOldDir = GetDir();
				char * cFileSelected;
				if (strlen(cPreSelectedFile) > 0)
					cFileSelected = &cPreSelectedFile[0];
				else
					cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "All\0*.*\0DDS\0*.dds\0PNG\0*.png\0JPEG\0*.jpg\0TGA\0*.tga\0BMP\0*.bmp\0\0\0", NULL, NULL);

				SetDir(tOldDir.Get());
				if (cFileSelected && strlen(cFileSelected) > 0)
				{
					pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].name = cFileSelected;
					pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].resource = WickedCall_LoadImage(pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].name);
					if (pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].resource)
					{
						//worked activate.
						pSelectedMaterial->SetOcclusionEnabled_Primary(false);
						pSelectedMaterial->SetOcclusionEnabled_Secondary(true);
						pSelectedMaterial->SetDirty();
						wiJobSystem::context ctx;
						wiJobSystem::Wait(ctx);
					}
					else 
					{
						//Failed reset slot.
						pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].resource = nullptr;
						pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].name = "";
						pSelectedMaterial->SetOcclusionEnabled_Primary(true);
						pSelectedMaterial->SetOcclusionEnabled_Secondary(false);
						pSelectedMaterial->SetDirty();
						wiJobSystem::context ctx;
						wiJobSystem::Wait(ctx);
					}
				}
			}
			iDelayedExecute = 0;
			break;
		}

		case 41: // Select OCCLUSION TEXTURE (to form SURFACE material)
		case 42: // Select ROUGHNESS TEXTURE (to form SURFACE material)
		case 43: // Select METALNESS TEXTURE (to form SURFACE material)
		case 44: // Select SURFACE TEXTURE (for when in custom materials)
		{
			// reference to original surface file
			char pOrigSurfaceFile[MAX_PATH];
			strcpy (pOrigSurfaceFile, "");

			// clear previous surface map and unload surface texture file (as we are replacing it here)
			bHaveMaterialUpdate = true;
			if (pSelectedMaterial)
			{
				strcpy (pOrigSurfaceFile, pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].name.c_str());
				// ZJ: Delete the original texture only if the user selects a file. If they cancel, we don't want to delete their previous selection.
				/*if (strlen(pOrigSurfaceFile) > 0) WickedCall_DeleteImage (pOrigSurfaceFile);
				pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].resource = nullptr;
				pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].name = "";
				pSelectedMaterial->SetDirty();
				wiJobSystem::context ctx;
				wiJobSystem::Wait(ctx);*/
			}

			// if a mesh selected for changing
			if (pSelectedMesh)
			{
				// base color name helps create unique temp surface files
				LPSTR pBaseColorTexFile = pSelectedMesh->pTextures[GG_MESH_TEXTURE_DIFFUSE].pName;
				char pBaseColorTextName[MAX_PATH];
				strcpy(pBaseColorTextName, "noname");
				if (strlen(pBaseColorTexFile) > 0)
				{
					for (int n = strlen(pBaseColorTexFile) - 1; n > 0; n--)
					{
						if (pBaseColorTexFile[n] == '\\' || pBaseColorTexFile[n] == '/')
						{
							strcpy (pBaseColorTextName, pBaseColorTexFile + n + 1);
							break;
						}
					}
					for (int n = strlen(pBaseColorTextName) - 1; n > 0; n--)
					{
						if (pBaseColorTextName[n] == '_')
						{
							pBaseColorTextName[n] = 0;
							break;
						}
					}
				}

				// call file requester to get new texture
				char* cFileSelected = "";
				if (iDelayedExecuteChannel != -2)
				{
					// ask for file or shoose one passed in
					if (strlen(cPreSelectedFile) > 0)
					{
						cFileSelected = &cPreSelectedFile[0];
					}
					else
					{
						cStr tOldDir = GetDir();
						bool bChoosingSurface = bChooseSurfaceChannel;
						if (bChooseSurfaceChannel) bChooseSurfaceChannel = false;
						if(pSelectedMaterial)
							cFileSelected = importer_selectfile(MaterialComponentTEXTURESLOT::SURFACEMAP, pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].name, !bChoosingSurface);
						else
							cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "All\0*.*\0DDS\0*.dds\0PNG\0*.png\0JPEG\0*.jpg\0TGA\0*.tga\0BMP\0*.bmp\0\0\0", NULL, NULL);
						SetDir(tOldDir.Get());
					}

					// if do not select a texture, consider this erasing the texture data
					if (cFileSelected == NULL) cFileSelected = "";
					if (strlen(cFileSelected) == 0)
					{
						iDelayedExecuteChannel = -2;

						break;
					}
					else
					{
						if (strlen(pOrigSurfaceFile) > 0) WickedCall_DeleteImage(pOrigSurfaceFile);
						pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].resource = nullptr;
						pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].name = "";
						pSelectedMaterial->SetDirty();
						wiJobSystem::context ctx;
						wiJobSystem::Wait(ctx);
					}
				}

				// ensure the DBO mesh texture is large enough to hold references
				if (pSelectedMesh->dwTextureCount <= GG_MESH_TEXTURE_SURFACE)
				{
					// if texture storage too small, increase it to store our references!
					extern bool EnsureTextureStageValid (sMesh* pMesh, int iTextureStage);
					EnsureTextureStageValid (pSelectedMesh, GG_MESH_TEXTURE_SURFACE);
				}

				// create bitmask to specify channel we are taking the data from
				unsigned char channelMask = 0;
				if (iDelayedExecuteChannel == -1) channelMask = (15) + (0 << 4); // red default
				if (iDelayedExecuteChannel >= 0) channelMask = (15) + (iDelayedExecuteChannel << 4);

				// assign channel masj to the specific texture ref
				if (iDelayedExecute == 41)
				{
					pSelectedMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].channelMask = channelMask;
				}
				if (iDelayedExecute == 42)
				{
					if (pSelectedMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].channelMask == 0 && channelMask > 0)
					{
						// if newly added data, set a default strength
						pSelectedMaterial->roughness = 1.0f;
						pSelectedMaterial->SetRoughness(pSelectedMaterial->roughness);
						pSelectedMaterial->SetDirty();
					}
					pSelectedMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].channelMask = channelMask;
				}
				if (iDelayedExecute == 43)
				{
					if (pSelectedMesh->pTextures[GG_MESH_TEXTURE_METALNESS].channelMask == 0 && channelMask > 0)
					{
						// if newly added data, set a default strength
						pSelectedMaterial->metalness = 1.0f;
						pSelectedMaterial->SetMetalness(pSelectedMaterial->metalness);
						pSelectedMaterial->SetDirty();
					}
					pSelectedMesh->pTextures[GG_MESH_TEXTURE_METALNESS].channelMask = channelMask;
				}
				if (iDelayedExecute == 44)
				{
					// changing surface texture!
				}

				// store name in DBO Mesh in correct slot
				if (iDelayedExecute == 41) strcpy (pSelectedMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].pName, cFileSelected);
				if (iDelayedExecute == 42) strcpy (pSelectedMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName, cFileSelected);
				if (iDelayedExecute == 43) strcpy (pSelectedMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName, cFileSelected);
				if (iDelayedExecute == 44)
				{
					strcpy (pSelectedMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].pName, cFileSelected);
					strcpy (pSelectedMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName, cFileSelected);
					strcpy (pSelectedMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName, cFileSelected);
					strcpy (pSelectedMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName, cFileSelected);
				}

				// do not generate surface if picked it directly (custom materials mode)
				char newSurfaceFileTemp[MAX_PATH];
				if ( iDelayedExecute != 44 )
				{
					// source is textures specified in DBO mesh (or if none exist, use original surface)
					LPSTR pAO = "", pGloss = "", pMetal = "";
					pAO = pSelectedMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].pName;
					pGloss = pSelectedMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName;
					pMetal = pSelectedMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName;

					// determine which channel the original ao,gloss,metal data resides for each file
					int iOcclusionChannel = 0, iRoughnessChannel = 0, iMetalnessChannel = 0, iReflectanceChannel = 3;
					unsigned char channelAOMask = pSelectedMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].channelMask;
					iOcclusionChannel = (channelAOMask >> 4) & (3);
					unsigned char channelGlossMask = pSelectedMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].channelMask;
					iRoughnessChannel = (channelGlossMask >> 4) & (3);
					unsigned char channelMetalMask = pSelectedMesh->pTextures[GG_MESH_TEXTURE_METALNESS].channelMask;
					iMetalnessChannel = (channelMetalMask >> 4) & (3);

					int iAOLength = strlen(pAO);
					int iGlossLength = strlen(pGloss);
					int iMetalLength = strlen(pMetal);

					// if there is no original raw texture file for ao,gloss and metal, assume it is a _surface texture (r=ao,g=gloss,b=metal,a=refl)
					if (iAOLength == 0) { pAO = pOrigSurfaceFile; iOcclusionChannel = 0; }
					if (iGlossLength == 0) { pGloss = pOrigSurfaceFile; iRoughnessChannel = 1; }
					if (iMetalLength == 0) { pMetal = pOrigSurfaceFile; iMetalnessChannel = 2; }

					// if reference indicates has no data channels, ensure surface created with defaults for that channel
					if (pSelectedMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].channelMask == 0)
					{
						pAO = "";
					}
					if (pSelectedMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].channelMask == 0)
					{
						pGloss = "";
					}
					if (pSelectedMesh->pTextures[GG_MESH_TEXTURE_METALNESS].channelMask == 0)
					{
						pMetal = "";
					}

					// Calculate string length again incase they have been changed.
					iAOLength = strlen(pAO);
					iGlossLength = strlen(pGloss);
					iMetalLength = strlen(pMetal);

					// Generate new surface map
					strcpy(newSurfaceFileTemp, GG_GetWritePath());
					strcat(newSurfaceFileTemp, "imported_models\\");

					// Get the name of the source file
					cstr sourceFile = importer_getfilenameonly((LPSTR)cFileSelected);
					
					bool bNameChosen = false;
					// If source file is a _surface.dds file, we can copy the name.
					if (strcmp(sourceFile.Get() + strlen(sourceFile.Get()) - strlen("_surface.dds"), "_surface.dds") == 0)
					{
						if (strlen(sourceFile.Get()) >= strlen("_surface.dds"))
						{
							strcat(newSurfaceFileTemp, sourceFile.Get());
							bNameChosen = true;
						}
					}
					
					if (!bNameChosen)
					{
						// Try to base the name off the color texture.
						sourceFile = importer_getfilenameonly((LPSTR)pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].name.c_str());

						if (sourceFile.Len() > 0)
						{
							char* replace = strstr(sourceFile.Get(), "_color.dds");
							if (replace)
							{
								strcpy(replace, "_surface.dds");
								strcat(newSurfaceFileTemp, sourceFile.Get());
								bNameChosen = true;
							}
						}
					}

					if (!bNameChosen)
					{
						// Base the name off of the chosen filename.
						sourceFile = importer_getfilenameonly((LPSTR)cFileSelected);
						char file[MAX_PATH];
						strcpy(file, sourceFile.Get());
						for (int i = strlen(file)-1; i >= 0; i--)
						{
							if (file[i] == '.')
							{
								strcpy(file + i, "_surface.dds");
								strcat(newSurfaceFileTemp, file);
								bNameChosen = true;
								break;
							}
						}
					}

					// generate surface texture from sources above
					ImageCreateSurfaceTextureChannels(newSurfaceFileTemp, pAO, pGloss, pMetal, iOcclusionChannel, iRoughnessChannel, iMetalnessChannel, iReflectanceChannel);

					// The texture will be copied from the imported_models folder when it is added to the object library. Afterwards, it can be deleted.
					t.importer.pSurfaceFilesToDelete.push_back(newSurfaceFileTemp);
				}
				else
				{
					// the surface file selected
					strcpy ( newSurfaceFileTemp, cFileSelected );
				}

				// recreate surface material from newly created surface file
				pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].name = newSurfaceFileTemp;
				pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].resource = WickedCall_LoadImage(pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].name);
				if (pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].resource)
				{
					// successfully loaded surface texture
				}
				else 
				{
					//Failed reset slot.
					pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].resource = nullptr;
					pSelectedMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].name = "";
				}
				pSelectedMaterial->SetDirty();
				wiJobSystem::context ctx;
				wiJobSystem::Wait(ctx);

				// resets
				iDelayedExecuteChannel = -1;
			}

			t.importer.bMeshesHaveDifferentSurface = true;
			iDelayedExecute = 0;
			break;
		}

		// Replace the normal map texture with a copy that has the green channel inverted.
		case 45:
		{
			if (!pSelectedMesh)
			{
				bHaveMaterialUpdate = false;
				iDelayedExecute = 0;
				break;
			}

			char newNormalMapFile[MAX_PATH];
			strcpy(newNormalMapFile, GG_GetWritePath());
			strcat(newNormalMapFile, "imported_models\\");
			char originalFile[MAX_PATH];
			strcpy(originalFile, pSelectedMesh->pTextures[GG_MESH_TEXTURE_NORMAL].pName);
			// Find filename without path or file extension.
			int iFoundFilename = -1;
			for (int n = strlen(originalFile) - 1; n > 0; n--)
			{
				if (originalFile[n] == '\\' || originalFile[n] == '/')
				{
					iFoundFilename = n;
					break;
				}
			}
			if (iFoundFilename != -1)
			{
				char pFilenameOnly[512];
				strcpy(pFilenameOnly, originalFile + iFoundFilename + 1);
				pFilenameOnly[strlen(pFilenameOnly) - 4] = 0;
				strcat(newNormalMapFile, pFilenameOnly);
				strcat(newNormalMapFile, "Inverted.dds");
			}
			else
			{
				// Couldn't find filename, so just use the mesh index.
				char meshID[32];
				sprintf(meshID, "%d", pSelectedMesh->wickedmeshindex);
				strcat(newNormalMapFile, meshID);
				strcat(newNormalMapFile, "Inverted.dds");
			}

			// source is textures specified in DBO mesh (or if none exist, use original surface)
			LPSTR pR = "", pG = "", pB = "";
			pR = pSelectedMesh->pTextures[GG_MESH_TEXTURE_NORMAL].pName;
			pG = pSelectedMesh->pTextures[GG_MESH_TEXTURE_NORMAL].pName;
			pB = pSelectedMesh->pTextures[GG_MESH_TEXTURE_NORMAL].pName;
			
			// can just change this to 0,1,2,3 since its all coming from the same texure.
			int iRedChannel = 0, iGreenChannel = 0, iBlueChannel = 0, iAlphaChannel = 3;
			unsigned char channelRMask = (15) + (0 << 4);
			iRedChannel = (channelRMask >> 4) & (3);
			unsigned char channelGMask = (15) + (1 << 4);
			iGreenChannel = (channelGMask >> 4) & (3);
			unsigned char channelBMask = (15) + (2 << 4);
			iBlueChannel = (channelBMask >> 4) & (3);

			// if there is no original raw texture file.
			if (strlen(pR) == 0) { pR = newNormalMapFile; iRedChannel = 0; }
			if (strlen(pG) == 0) { pG = newNormalMapFile; iGreenChannel = 1; }
			if (strlen(pB) == 0) { pB = newNormalMapFile; iBlueChannel = 2; }

			// generate normal texture from sources above
			ImageCreateNormalTextureInvertedGreen(newNormalMapFile, pR, pG, pB, iRedChannel, iGreenChannel, iBlueChannel, iAlphaChannel);
			pSelectedMaterial->textures[GG_MESH_TEXTURE_NORMAL].name = newNormalMapFile;
			pSelectedMaterial->textures[GG_MESH_TEXTURE_NORMAL].resource = WickedCall_LoadImage(pSelectedMaterial->textures[GG_MESH_TEXTURE_NORMAL].name);
			pSelectedMaterial->SetDirty();
			bHaveMaterialUpdate = true;
			iDelayedExecute = 0;
			break;
		}

		// Apply diffuse texture to all meshes.
		case 50:
		{
			importer_texture_all_meshes(MaterialComponentTEXTURESLOT::BASECOLORMAP);
			t.importer.bMeshesHaveDifferentBase = false;
			iDelayedExecute = 0;
			break;
		}
		// Apply normalmap to all meshes.
		case 51:
		{
			importer_texture_all_meshes(MaterialComponentTEXTURESLOT::NORMALMAP);
			t.importer.bMeshesHaveDifferentNormal = false;
			iDelayedExecute = 0;
			break;
		}
		// Apply surface map to all meshes.
		case 52:
		{
			importer_texture_all_meshes(MaterialComponentTEXTURESLOT::SURFACEMAP);
			t.importer.bMeshesHaveDifferentSurface = false;
			iDelayedExecute = 0;
			break;

		}
		// Apply emissive map to all meshes.
		case 53:
		{
			importer_texture_all_meshes(MaterialComponentTEXTURESLOT::EMISSIVEMAP);
			t.importer.bMeshesHaveDifferentEmissive = false;
			iDelayedExecute = 0;
			break;
		}

		#endif

		default:
			break;
	}

	if (bImporter_Window && t.importer.importerActive == 1)
	{
		int media_icon_size = 64;
		float col_start = 80.0f;

		//Execute code before we add any texture to the draw call list.
		switch (iDelayedExecute) 
		{
			default:
				break;
		}

		// generate thumbnail
		if (iImporterGenerateThumb <= 5)
		{
			if (iImporterGenerateThumb == 5)
			{
				#ifdef WICKEDENGINE
				// Find new wicked way to make thumbnail
				#else
				// delete previous thumbnail
				if (GetImageExistEx(IMPORTER_TMP_IMAGE)) DeleteImage(IMPORTER_TMP_IMAGE);
				import_generate_thumb();
				#endif
				iImporterGenerateThumb++;
			}
			else
			{
				iImporterGenerateThumb++;
			}
		}

		extern int iGenralWindowsFlags;
		ImGui::Begin("Importer##ImporterWindow", &bImporter_Window, iGenralWindowsFlags);

		float w = ImGui::GetWindowContentRegionWidth();

		extern int iLastOpenHeader;
		if (pref.bAutoClosePropertySections && iLastOpenHeader != 72 && iLastOpenHeader >= 70 && iLastOpenHeader <= 80)
			ImGui::SetNextItemOpen(false, ImGuiCond_Always);

		if (ImGui::StyleCollapsingHeader("Name", ImGuiTreeNodeFlags_DefaultOpen)) 
		{
			iLastOpenHeader = 72;
			//ZJ: Removed following Rick's feedback.
			//if (GetImageExistEx(IMPORTER_TMP_IMAGE))
			//{
			//	ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (media_icon_size*0.5), 0.0f));
			//	if(ImGui::ImgBtn(IMPORTER_TMP_IMAGE, ImVec2(media_icon_size, media_icon_size), ImColor(0, 0, 0, 255), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 255), ImColor(180, 180, 180, 255),2,0,0,0,true,false)) {
			//		iImporterGenerateThumb = 0;
			//	}
			//}

			ImGui::Indent(10);
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13)); //3
			ImGui::Text("Name");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
			ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY()));
			ImGui::PushItemWidth(-10);

			ImGui::InputText("##NameImport", &cImportName[0], 128);
			#ifdef WICKEDENGINE
			if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;
			if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Object Name");
			#else
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Object Name");
			#endif
			ImGui::PopItemWidth();
			ImGui::Indent(-10);
		}

		if (pref.bAutoClosePropertySections && iLastOpenHeader != 73)
			ImGui::SetNextItemOpen(false, ImGuiCond_Always);

		if (ImGui::StyleCollapsingHeader("Customize", ImGuiTreeNodeFlags_DefaultOpen))
		{
			iLastOpenHeader = 73;
			ImGui::Indent(10);

			#ifdef WICKEDENGINE
			// Scaling Mode
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13));
			ImGui::Text("Scaling");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
			ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY()));
			ImGui::PushItemWidth(-10);
			char* pScalingModes[5] = { "Original Scaling", "Units in Meters", "Units in Inches", "Units in Centimeters", "Automatic Scaling" };
			if (ImGui::BeginCombo("##ImporterScalingModes", &scaling_combo_entry[0], ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_HeightLarge))
			{
				for (int i = 0; i < 5; i++)
				{
					// get name
					char* pScalingModeName = pScalingModes[i];

					// assign correct item based on combo_entry
					bool is_selected = false;
					if (strcmp(scaling_combo_entry, pScalingModeName) == NULL)
					{
						is_selected = true;
					}
					if (ImGui::Selectable(pScalingModeName, is_selected))
					{
						strcpy(scaling_combo_entry, pScalingModeName);
						if (t.importer.lastscalingmodeused != i)
						{
							// change the scaling mode, but need to reload the model as
							// this is done at load time
							t.importer.lastscalingmodeused = i;
							iDelayedExecute = 4;
						}
					}
					if (is_selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set the scaling mode to set the model to the correct unit scale at load time");
			// center mesh data
			bool bCenterMeshData = false;
			if (t.importer.centermodelbyshiftingmesh == 1) bCenterMeshData = true;
			if (ImGui::Checkbox("Center Mesh Data", &bCenterMeshData))
			{
				if (bCenterMeshData == true)
					t.importer.centermodelbyshiftingmesh = 1;
				else
					t.importer.centermodelbyshiftingmesh = 0;
				iDelayedExecute = 4;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Sets whether to center the meshes data of the imported model");
			#endif

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13)); //3
			ImGui::Text("Scale");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
			ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY()));
			ImGui::PushItemWidth(-10);
			t.importer.showScaleChange = 0;
			float fImporterScale = iImporterScale;
			if (ImGui::MaxSliderInputFloat("##ImporterScale", &fImporterScale, 0.0f, 200.0f, "Change the scale of the model before saving object", 0, 200))
			{
				iImporterScale = fImporterScale;

				t.importer.showScaleChange = 1;
				t.tscale_f = t.importer.objectScaleForEditing;
				t.tScaleMultiplier_f = iImporterScale / 100.0;
				t.tscale_f = t.tscale_f / t.tScaleMultiplier_f;
				t.tscale_f = t.tscale_f * t.importer.camerazoom;
				ScaleObject(t.importer.dummyCharacterObjectNumber, t.tscale_f, t.tscale_f, t.tscale_f * 0.2);

				//Until we drop the slider menu.
				t.slidersmenuvalue[t.importer.properties1Index][1].value = iImporterScale;
				t.importer.oldScale = t.slidersmenuvalue[t.importer.properties1Index][1].value;

				#ifdef WICKEDENGINE
				//PE: Scale actual object.
				if (ObjectExist(t.importer.objectnumber) == 1)
				{
					ScaleObject(t.importer.objectnumber, iImporterScale*fImporterScaleMultiply, iImporterScale*fImporterScaleMultiply, iImporterScale*fImporterScaleMultiply);
				}

				//Make sure we dont go below terrain after scale.
				if (bStickObjectToGround)
				{
					/* removed so does not interfere with new offset parameter to position object
					float terrain_height = BT_GetGroundHeight(t.terrain.TerrainID, ObjectPositionX(t.importer.objectnumber), ObjectPositionZ(t.importer.objectnumber), 1);
					PositionObject(t.importer.objectnumber, ObjectPositionX(t.importer.objectnumber), terrain_height, ObjectPositionZ(t.importer.objectnumber));
					sObject* pObject = g_ObjectList[t.importer.objectnumber];
					if (pObject)
					{
						if (pObject->collision.vecMin.y < 0)
						{
							//PE: Assume its a center pivot , and adjust positions.
							float fPivotAdjust = fabs(pObject->collision.vecMin.y * pObject->position.vecScale.y);
							PositionObject(t.importer.objectnumber, ObjectPositionX(t.importer.objectnumber), ObjectPositionY(t.importer.objectnumber) + fPivotAdjust, ObjectPositionZ(t.importer.objectnumber));
						}
					}
					*/
				}
				#endif
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Object Scale");

			#ifdef WICKEDENGINE
			ImGui::TextCenter("Rotation Offset");
			if (ImGui::MaxSliderInputFloat("##XImportrotation", &fImportRotX, 0.0f, 360.0f, "Adjust rotation of model around X axis", 0, 360))
			{
				t.importer.objectFPE.rotx = fImportRotX;
				importer_applyframezerooffsets(t.importer.objectnumber, fImportPosX, fImportPosY, fImportPosZ, fImportRotX, fImportRotY, fImportRotZ);
			}
			if (ImGui::MaxSliderInputFloat("##YImportrotation", &fImportRotY, 0.0f, 360.0f, "Adjust rotation of model around Y axis", 0, 360))
			{
				t.importer.objectFPE.roty = fImportRotY;
				importer_applyframezerooffsets(t.importer.objectnumber, fImportPosX, fImportPosY, fImportPosZ, fImportRotX, fImportRotY, fImportRotZ);

			}
			if (ImGui::MaxSliderInputFloat("##ZImportrotation", &fImportRotZ, 0.0f, 360.0f, "Adjust rotation of model around Z axis", 0, 360))
			{
				t.importer.objectFPE.rotz = fImportRotZ;
				importer_applyframezerooffsets(t.importer.objectnumber, fImportPosX, fImportPosY, fImportPosZ, fImportRotX, fImportRotY, fImportRotZ);
			}
			ImGui::TextCenter("Position Offset");
			int iImportPos = fImportPosX;
			if ( ImGui::MaxSliderInputInt("##XImportoffset", &iImportPos, -1000.0f, 1000.0f, "Adjust offset of model on post-rotated X axis"))
			{
				fImportPosX = iImportPos;
				t.importer.objectFPE.offx = fImportPosX;
				importer_applyframezerooffsets(t.importer.objectnumber, fImportPosX, fImportPosY, fImportPosZ, fImportRotX, fImportRotY, fImportRotZ);
			}
			iImportPos = fImportPosY;
			if ( ImGui::MaxSliderInputInt("##YImportoffset", &iImportPos, -1000.0f, 1000.0f, "Adjust offset of model on post-rotated Y axis"))
			{
				fImportPosY = iImportPos;
				t.importer.objectFPE.offy = fImportPosY;
				importer_applyframezerooffsets(t.importer.objectnumber, fImportPosX, fImportPosY, fImportPosZ, fImportRotX, fImportRotY, fImportRotZ);

				// Reset find floor flag, as if the user changes the y offset when it is enabled, the model will no longer be at the floor.
				bFindFloor = false;
			}
			iImportPos = fImportPosZ;
			if ( ImGui::MaxSliderInputInt("##ZImportoffset", &iImportPos, -1000.0f, 1000.0f, "Adjust offset of model on post-rotated Z axis"))
			{
				fImportPosZ = iImportPos;
				t.importer.objectFPE.offz = fImportPosZ;
				importer_applyframezerooffsets(t.importer.objectnumber, fImportPosX, fImportPosY, fImportPosZ, fImportRotX, fImportRotY, fImportRotZ);
			}

			// Find the floor.
			if (ImGui::Checkbox("Find Floor", &bFindFloor))
			{
				importer_find_floor();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Auto offset the model on the Y axis");
			#endif

			#ifdef WICKEDENGINE
			// Collision Type
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13)); //3
			ImGui::Text("Collision");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
			ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY()));
			ImGui::PushItemWidth(-10);
			char* pCollisionShapes[10] = { "Box","Polygon","Sphere","Cylinder","Convex Hull","Character","Tree Collision","No Collision", "Hull Decomp", "Collision Mesh" };
			if (ImGui::BeginCombo("##ImporterCollisionShape", &collision_combo_entry[0], ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_HeightLarge))
			{
				for (int i = 0; i < 10; i++)
				{
					// Don't display "Polygon", "Convex Hull" or "Hull Decomp" collision for dynamic objects.
					if (i == 1 && t.importer.defaultstatic == 0) continue;
					else if (i == 4 && t.importer.defaultstatic == 0) continue;
					else if (i == 8 && t.importer.defaultstatic == 0) continue;

					// get collision shape name
					char* pCollisionShapeName = pCollisionShapes[i];

					// assign correct item based on collision_combo_entry
					bool is_selected = false;
					if (strcmp(collision_combo_entry, pCollisionShapeName) == NULL)
					{
						is_selected = true;
					}
					if (ImGui::Selectable(pCollisionShapeName, is_selected))
					{
						strcpy(collision_combo_entry, pCollisionShapeName);
						t.importer.collisionshape = i;
					}
					if (is_selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set the collision type the object will use for physics");
			ImGui::PopItemWidth();
			#endif

			#ifdef WICKEDENGINE
			if (t.importer.collisionshape == 5)
			{
				// Character Specific
				t.importer.defaultstatic = 0;
				if(t.importer.ischaracter==0) t.importer.ischaracter = 1;

				// Static/Dynamic Mode
				bool bIsFemale = false;
				if (t.importer.ischaracter == 2) bIsFemale = true;
				if (ImGui::Checkbox("Is Female", &bIsFemale))
				{
					if (bIsFemale == true)
						t.importer.ischaracter = 2;
					else
						t.importer.ischaracter = 1;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set if the character needs to use female animation sets");
			}
			else
			{
				// Static/Dynamic Mode
				t.importer.ischaracter = 0;
				bool bStaticDefault = false;
				if (t.importer.defaultstatic == 1) bStaticDefault = true;
				if (ImGui::Checkbox("Static Object Mode", &bStaticDefault))
				{
					if (bStaticDefault == true)
						t.importer.defaultstatic = 1;
					else
						t.importer.defaultstatic = 0;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Sets whether the object is a static unmoving part of the scene");
			}
			ImGui::Indent(-10);
		}
		#endif

		if (pref.bAutoClosePropertySections && iLastOpenHeader != 74)
			ImGui::SetNextItemOpen(false, ImGuiCond_Always);
		if (ImGui::StyleCollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen))
		{
			iLastOpenHeader = 74;
			#ifdef WICKEDENGINE
			ImGui::Indent(10);

			// Set the material index of the imported object.
			char* cMaterialTypes[4] = { "Silent", "Stone", "Metal", "Wood" };
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 15));
			ImGui::Text("Material Type");
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
			ImGui::PushItemWidth(-10);
			if (ImGui::BeginCombo("##ImporterMaterialType", &material_combo_entry[0], ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_HeightLarge))
			{
				for (int i = 0; i < 4; i++)
				{
					// assign correct item based on material_combo_entry
					bool is_selected = false;
					if (strcmp(material_combo_entry, cMaterialTypes[i]) == NULL)
					{
						is_selected = true;
					}
					if (ImGui::Selectable(cMaterialTypes[i], is_selected))
					{
						strcpy(material_combo_entry, cMaterialTypes[i]);
						t.slidersmenuvalue[t.importer.properties1Index][10].value = i + 1;
					}
					if (is_selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select the material index for this object");
			ImGui::PopItemWidth();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 15));

			//PE: WICKED - display textures directly from object.
			sObject* pObject = g_ObjectList[t.importer.objectnumber];
			//pObject = g_ObjectList[t.storegridentityobj];
			sMesh * pMesh = NULL;
			
			Wicked_Change_Object_Material((void*)pObject, 0, NULL, t.importer.bEditAllMesh); 
			//Wicked_Change_Object_Material((void*)pObject, 0, NULL, false); // false = allows roughness, metalness and occlusion to be edited
			
			#else
			if (t.tcounttextures > 0) 
			{
				for (int tCount = 1; tCount <= t.tcounttextures; tCount++) 
				{
					int img_id = g.importermenuimageoffset + 6;
					if (ImageExist(t.importerTextures[tCount].imageID))
						img_id = t.importerTextures[tCount].imageID;

					if (ImageExist(img_id)) 
					{
						float cur_pos = (ImGui::GetCursorPosX() + media_icon_size + media_icon_size + 6); //Used to check when to wrap.
						if (t.tcounttextures == 1)
							ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (media_icon_size*0.5), 0.0f));
						if (ImGui::ImgBtn(img_id, ImVec2(media_icon_size, media_icon_size), ImColor(0, 0, 0, 255))) 
						{
							//LB: disable ability to change texture, just view
							//iDelayedExecuteSelection = tCount;
							//iDelayedExecute = 1;
						}
						if (tCount < t.tcounttextures && (cur_pos < w)) ImGui::SameLine();
					}
				}
			}
			#endif
			ImGui::Indent(-10);
		}

		#ifdef WICKEDENGINE
		animsystem_animationtoolui(t.importer.objectnumber);
		#endif

		#ifdef WICKEDENGINE
		if (pref.bAutoClosePropertySections && iLastOpenHeader != 76)
			ImGui::SetNextItemOpen(false, ImGuiCond_Always);
		if (ImGui::CollapsingHeader("Import Object", ImGuiTreeNodeFlags_DefaultOpen))
		#else
		if (ImGui::CollapsingHeader("Save Entity", ImGuiTreeNodeFlags_DefaultOpen))
		#endif
		{
			#ifdef WICKEDENGINE
			iLastOpenHeader = 76;
			#endif
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
		
			ImGui::TextCenter("Path");
			
			//ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
			//ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY()));
			//ImGui::SetCursorPos(ImVec2(0, ImGui::GetCursorPosY()));
			//float path_gadget_size = ImGui::GetFontSize()*2.0;
			//ImGui::Indent(10);
			//ImGui::PushItemWidth(10 - path_gadget_size);
		
			//ImGui::Indent(38);
			//ImGui::PushItemWidth(ImGui::GetFontSize() * 10.0f);
			ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() - ImGui::GetFontSize() * 9 - 10);// ImGui::GetFontSize() * 10.0f);
			ImGui::Indent(ImGui::GetFontSize()*2.0 +40);
			
			//ImGui::InputText("##InputPathImporter", &cImportPath[0], 250);
			ImGui::InputText("##InputPathImporter", &cImportPathCropped[0], 250);
			if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set where you would like to save the object files");
			#ifdef WICKEDENGINE
			if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;
			#endif
			ImGui::PopItemWidth();

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetFontSize() * 2.0f);

			//	Let the user know they set an invalid save file path.
			if (ImGui::BeginPopup("##InvalidSavePath"))
			{
				ImGui::Text("Path must be within 'Max\\Files\\entitybank\\'");
				ImGui::EndPopup();
			}

			if (ImGui::StyleButton("...##Importerpath"))
			{
				//PE: filedialogs change dir so.
				cStr tOldDir = GetDir();
				char * cFileSelected;
				//cstr fulldir = tOldDir + "\\entitybank\\user\\";
				char defaultPath[MAX_PATH];
				strcpy(defaultPath, GG_GetWritePath());
				strcat(defaultPath, "Files\\entitybank\\user");
				cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_DIR, "All\0*.*\0", defaultPath, "", true, NULL);
				SetDir(tOldDir.Get());

				// Now that user has chosen a new path, refresh the folders, in case any new folders have been added 
				// If this proves to be too slow, we can just manually add the new folders that have been created, and the auto file scan should do the rest.
				extern bool bExternal_Entities_Init;
				bExternal_Entities_Init = false;
				extern void mapeditorexecutable_full_folder_refresh(void);
				mapeditorexecutable_full_folder_refresh();
				//,pvec code below intside the refresh call
				// Also ensure that the tree view in process_entity_library_v2 will be updated so the user can see any new folders they created.
				//extern bool bTreeViewInitInNextFrame;
				//bTreeViewInitInNextFrame = true;

				if (cFileSelected && strlen(cFileSelected) > 0) {

					//	Check that the new path still contains the entitybank folder.
					char* cCropped = strstr(cFileSelected, "\\entitybank");
					if (cCropped)
					{
						//	New location contains entitybank folder, so change the import path.
						strcpy(cImportPath, cFileSelected);
						strcpy(cImportPathCropped, cCropped);

						//	Drop the entitybank folder from the cropped file path.
						char pNewCroppedStr[MAX_PATH];
						strcpy(pNewCroppedStr, cCropped + strlen("\\entitybank"));
						strcpy(cImportPathCropped, pNewCroppedStr);
					}
					else
					{
						ImGui::OpenPopup("##InvalidSavePath");
					}
				}
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set where you would like to save the object files");

			ImGui::PopItemWidth();
			ImGui::Indent(-58);

			ImGui::Indent(-10); //unindent before center.

			float but_gadget_size = ImGui::GetFontSize()*10.0;
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
			#ifdef WICKEDENGINE
			if (ImGui::Button("Import Object##ImporterSaveUniqueId", ImVec2(but_gadget_size, 0))) 
			#else
			if (ImGui::Button("Save Entity##ImporterSaveUniqueId", ImVec2(but_gadget_size, 0)))
			#endif
			{
				//code
				if (strlen(cImportName) > 0) 
				{
					if (strlen(cImportPath) > 0) 
					{
						#ifdef WICKEDENGINE
						importer_storeobjectdata();

						// before save, match created animsets to actual DBO structure
						sObject* pObject = GetObjectData(t.importer.objectnumber);
						UpdateObjectWithAnimSlotList(pObject);
						#endif

						// Trigger save Object to happen
						iDelayedExecute = 3; 
					}
					else
					{
						strcpy(cTriggerMessage, "Please select a path where you like the object saved");
						bTriggerMessage = true;
					}
				}
				else 
				{
					strcpy(cTriggerMessage, "You must give your object a name before you can save it");
					bTriggerMessage = true;
				}
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Import Object");

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
			if (ImGui::StyleButton("Cancel", ImVec2(but_gadget_size, 0)))
			{
				iDelayedExecute = 2; //Quit
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cancel, Close Importer");
		}

		if (!pref.bHideTutorials)
		{
#ifndef REMOVED_EARLYACCESS
			if (ImGui::StyleCollapsingHeader("Tutorial (this feature is incomplete)", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Indent(10);
				void SmallTutorialVideo(char *tutorial, char* combo_items[] = NULL, int combo_entries = 0, int iVideoSection = 0, bool bAutoStart = false);
				cstr cShowTutorial = "03 - Add character and set a path";
				char* tutorial_combo_items[] = { "01 - Getting started", "02 - Creating terrain", "03 - Add character and set a path" };
				SmallTutorialVideo(cShowTutorial.Get(), tutorial_combo_items, ARRAYSIZE(tutorial_combo_items), SECTION_IMPORTER);
				float but_gadget_size = ImGui::GetFontSize()*12.0;
				float w = ImGui::GetWindowContentRegionWidth() - 10.0;
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
				#ifdef INCLUDESTEPBYSTEP
				if (ImGui::StyleButton("View Step by Step Tutorial", ImVec2(but_gadget_size, 0)))
				{
					// pre-select tutorial 03
					extern bool bHelpVideo_Window;
					extern bool bHelp_Window;
					extern char cForceTutorialName[1024];
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

		void CheckMinimumDockSpaceSize(float minsize);
		CheckMinimumDockSpaceSize(250.0f);

		if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
			//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
			ImGui::Text("");
			ImGui::Text("");
		}

		ImGui::End();
	}
	else 
{
		if (!bImporter_Window && t.importer.importerActive == 1)
		{
			//Window must have been close, shutdown.
			iDelayedExecute = 2; //Quit
		}
		else if (!bImporter_Window) 
		{
			#ifdef WICKEDENGINE
			//PE: we can delete the object here , so we are sure we dont use any textures from the object.
			if (ObjectExist(t.importer.objectnumber))
			{
				DeleteObject(t.importer.objectnumber);
				WickedCall_SetSelectedObject(NULL);
			}
			if (ObjectExist(t.importer.dummyCharacterObjectNumber)) DeleteObject(t.importer.dummyCharacterObjectNumber);
			#endif
		}
	}
}

#ifdef WICKEDENGINE
void importer_texture_all_meshes(int iTexSlot)
{
	//t.gridentityextractedindex; t.widget.pickedEntityIndex;
	sObject* pObject = nullptr;// = GetObjectData(t.importer.objectnumber);
	if (t.importer.importerActive == 1)
	{
		pObject = GetObjectData(t.importer.objectnumber);
	}
	else
	{
		// Need a way to get the object data for the current object being altered outside of the importer.
		int e = t.widget.pickedEntityIndex;
		if (e < 0) return;
		int obj = t.entityelement[e].obj;
		if (ObjectExist(obj))
		{
			pObject = GetObjectData(obj);
		}
	}
	
	if (!pObject)
	{
		return;
	}

	bool bUserCancel = false;
	for (int i = 0; i < pObject->iMeshCount; i++)
	{
		pSelectedMesh = pObject->ppMeshList[i];

		if (pSelectedMesh)
		{
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pSelectedMesh->wickedmeshindex);

			if (mesh)
			{
				// get material settings from mesh material or WEMaterial
				uint64_t materialEntity = mesh->subsets[0].materialID;
				wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
				pSelectedMaterial = pObjectMaterial;

				bUserCancel = !importer_apply_materialformesh(static_cast<MaterialComponentTEXTURESLOT>(iTexSlot), iTexSlot);
				if (bUserCancel) break;
			}
		}
	}

	if (!t.importer.importerActive)
	{
		// Ensure that the new material changes are applied to the object eleprof, so that when maps get saved/loaded, the changes are not lost.
		Wicked_Copy_Material_To_Grideleprof((void*)pObject, 0, &t.entityelement[t.widget.pickedEntityIndex].eleprof);
		Wicked_Set_Material_From_grideleprof((void*)pObject, 0, &t.entityelement[t.widget.pickedEntityIndex].eleprof);
	}

	// Reset the selected file, so that the file dialiog will open when adding other textures.
	strcpy(cPreSelectedFile, "");
}
#endif

void imgui_importer_draw(void)
{

}

#endif

void importer_loop_wicked(void)
{
	// New import system for Wicked, should use existing editor to move around object, etc

	// Rotate the character decal to face the camera.
	PointObject(g.importerextraobjectoffset + 2, CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0));
	XRotateObject(g.importerextraobjectoffset + 2, 0.0f);
	ZRotateObject(g.importerextraobjectoffset + 2, 0.0f);

	//PE: Normal navigation.
	editor_visuals();
}

void importer_loop ( void )
{
	#ifdef WICKEDENGINE
	importer_loop_wicked();

	#else
	#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	//	imgui_importer_loop();
	//	return;
	#endif

	// this gets triggered once importer underway
	if ( g_iPreferPBRLateShaderChange > 0 )
	{
		g_iPreferPBRLateShaderChange--;
		if ( g_iPreferPBRLateShaderChange == 0 ) 
		{
			importer_changeshader ( t.importer.objectFPE.effect.Get() ); Sync();
			importer_changeshader ( t.importer.objectFPE.effect.Get() ); Sync();
		}
	}

	importer_update_scale ( );
#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
#else
	t.inputsys.xmousemove = t.inputsys.xmousemove * 2;
	t.inputsys.ymousemove = t.inputsys.ymousemove * 2;
#endif

	#ifdef FPSEXCHANGE
	 OpenFileMap (  1, "FPSEXCHANGE" );
	 SetEventAndWait (  1 );
	 t.inputsys.kscancode=GetFileMapDWORD( 1, 100 );
	#else
	 t.inputsys.kscancode = ScanCode();
	#endif
	
	//  Multiplier to convert mouse coords to importer coords
#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	 t.tadjustedtoimporterxbase_f = ((GetChildWindowWidth(-1) + 0.0) / (float)GetDisplayWidth());
	 t.tadjustedtoimporterybase_f = ((GetChildWindowHeight(-1) + 0.0) / (float)GetDisplayHeight());
#else
	t.tadjustedtoimporterxbase_f=GetChildWindowWidth()/800.0;
	t.tadjustedtoimporterybase_f=GetChildWindowHeight()/600.0;
#endif
	t.importer.MouseX = t.inputsys.xmouse*t.tadjustedtoimporterxbase_f;
	t.importer.MouseY = t.inputsys.ymouse*t.tadjustedtoimporterybase_f;


	if (  t.timportermouseoff  ==  0 ) 
	{
		t.toldMouseBeforeEditX = t.importer.MouseX + ( GetDesktopWidth() - GetChildWindowWidth() );
		t.toldMouseBeforeEditY = t.importer.MouseY + ( GetDesktopHeight() - GetChildWindowHeight() );
	}

	//  Control gadgets in panel
	if (  t.importer.loaded == 1 ) 
	{
		int iCameraAdjustMode = 0;
		if ( t.importerTabs[1].selected == 1 ) iCameraAdjustMode = 2;
		if ( t.importerTabs[2].selected == 1 ) iCameraAdjustMode = 1;
		if ( iCameraAdjustMode > 0 )
		{
			float fFinalScale, fFinalScale2;
			if ( t.inputsys.keyup != 0 ) t.importer.cameraheight += 10.0f;
			if ( t.inputsys.keydown != 0 ) t.importer.cameraheight -= 10.0f;
			if ( iCameraAdjustMode == 2 )
			{
				if ( t.inputsys.wheelmousemove > 0 ) t.importer.camerazoom += 0.05f;
				if ( t.inputsys.wheelmousemove < 0 ) t.importer.camerazoom -= 0.05f;
				if ( t.importer.camerazoom < 0.05f ) t.importer.camerazoom = 0.05f;
				fFinalScale = t.importer.objectScaleForEditing * t.importer.camerazoom;
				fFinalScale2 = 100.0f * t.importer.camerazoom;
			}
			else
			{
				fFinalScale = t.importer.objectScaleForEditing;
				fFinalScale2 = 100.0f;
			}
			PositionObject ( t.importer.objectnumber, 0, -t.importer.cameraheight, 0 );
			ScaleObject ( t.importer.objectnumber, fFinalScale, fFinalScale, fFinalScale );
			PositionObject ( t.importer.dummyCharacterObjectNumber, 0, -t.importer.cameraheight, 0 );
			ScaleObject ( t.importer.dummyCharacterObjectNumber, fFinalScale, fFinalScale, fFinalScale * 0.2f );
			for ( int tCount = 1 ; tCount<= 6; tCount++ )
			{
				PositionObject ( t.importerGridObject[tCount], 0, -t.importer.cameraheight, 0 );
				ScaleObject ( t.importerGridObject[tCount], fFinalScale2, fFinalScale2, fFinalScale2 );
			}
			float fShiftForCollisionObjects = t.importer.cameraheight - t.importer.lastcameraheightforshift;
			t.importer.lastcameraheightforshift = t.importer.cameraheight;
			for ( int tCount = 0 ; tCount<=  t.importer.collisionShapeCount-1; tCount++ )
			{
				if (  t.importerCollision[tCount].object > 0 ) 
				{
					if (  ObjectExist(t.importerCollision[tCount].object)  ==  1 ) 
					{
						float fX = ObjectPositionX(t.importerCollision[tCount].object);
						float fY = ObjectPositionY(t.importerCollision[tCount].object) - fShiftForCollisionObjects;
						float fZ = ObjectPositionZ(t.importerCollision[tCount].object);
						PositionObject ( t.importerCollision[tCount].object, fX, fY, fZ );
						PositionObject ( t.importerCollision[tCount].object2, fX, fY, fZ );
					}
				}
			}
		}
		else
		{
			// viewing controls only for main panel
			float fFinalScale = t.importer.objectScaleForEditing;
			float fDefaultCameraHeight = -(ObjectSizeY(t.importer.objectnumber,0)/2.0);
			PositionObject ( t.importer.objectnumber, 0, fDefaultCameraHeight, 0 );
			ScaleObject ( t.importer.objectnumber, fFinalScale, fFinalScale, fFinalScale );
			PositionObject ( t.importer.dummyCharacterObjectNumber, 0, fDefaultCameraHeight, 0 );
			ScaleObject ( t.importer.dummyCharacterObjectNumber, fFinalScale, fFinalScale, fFinalScale * 0.2f );
			fFinalScale = 100.0f;
			for ( int tCount = 1 ; tCount<= 6; tCount++ )
			{
				PositionObject ( t.importerGridObject[tCount], 0, fDefaultCameraHeight, 0 );
				ScaleObject ( t.importerGridObject[tCount], fFinalScale, fFinalScale, fFinalScale );
			}
			importer_RestoreCollisionShiftHeight();
		}

		importer_help ( );

		//  This is to get round a gimble lock issue
		RotateObject (  t.importerGridObject[9],0,0,0 );

		if (  t.importerTabs[5].selected  ==  1 ) 
		{
			t.importerTabs[5].selected = 0;
			t.timportersaveon = 1;
			#ifdef VRTECH
			importer_save_entity ( );
			#else
			importer_save_entity ( NULL );
			#endif
		}

		if (  t.importer.helpShow  ==  0  )  t.importer.message  =  "Click and drag to rotate";

		//  Update or hide selection markers
		importer_update_selection_markers ( );

		//  Tab One Mode - settings
		if (  g.tabmode  ==  IMPORTERTABPAGE1 ) 
		{
			SetAlphaMappingOn (  t.importer.objectnumber , 100 );
			SetObjectTransparency (  t.importer.objectnumber, 255 );

			//  Front view
			YRotateObject (   t.importerGridObject[8] , 0 );
			XRotateObject (   t.importerGridObject[8] , 0 );
			YRotateObject (   t.importerGridObject[9] , 0 );
			XRotateObject (   t.importerGridObject[9] , 0 );

			// only if not over the properties panel
			
			if (!bRemoveSprites && t.importer.MouseX > t.slidersmenu[t.importer.properties1Index].tleft
			&&	 t.importer.MouseY > t.slidersmenu[t.importer.properties1Index].ttop )	
			{
				// mouse within properties panel, do not rotate object on click
			}
			else
			{
				//  Rotation control
				switch (  t.importer.objectRotateMode ) 
				{
					case 0:

						if (  t.inputsys.mclick  ==  1 && t.importer.oldMouseClick  ==  0 ) 
						{
							#ifdef DX11
#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
							float fMX = ((GetChildWindowWidth(-1) + 0.0) / (float)GetDisplayWidth());
							float fMY = ((GetChildWindowHeight(-1) + 0.0) / (float)GetDisplayHeight());
#else
							float fMX = (GetDisplayWidth()+0.0) / 800.0f;
							float fMY = (GetDisplayHeight()+0.0) / 600.0f;
#endif
							t.tadjustedtoareax_f = t.inputsys.xmouse*fMX;
							t.tadjustedtoareay_f = t.inputsys.ymouse*fMY;
							#else
							t.tadjustedtoareax_f = t.importer.MouseX;
							t.tadjustedtoareay_f = t.importer.MouseY;
							#endif
							if ( PickScreenObjectEx ( t.tadjustedtoareax_f, t.tadjustedtoareay_f, t.importer.objectnumber, t.importer.objectnumber, 1, 0 )  !=  0 )
							{
								t.importer.objectRotateMode = 1;
								t.tnothing = t.inputsys.xmousemove;
								importer_hide_mouse ( );
							}
						}

					break;
					case 1:

						if (  t.inputsys.mclick  ==  0 ) 
						{
							t.importer.objectRotateMode = 0;
							importer_show_mouse ( );
						}
						else
						{
							t.importer.message = "How down shift to lock to 45' Steps";
							if (  t.inputsys.keyshift  ==  0 ) 
							{
								t.importer.objectAngleY -= t.inputsys.xmousemove;
							}
							else
							{
								t.importer.message = "Locked to 45' steps";
								t.importer.mouseMoveSnap -= t.inputsys.xmousemove;
								if (  t.importer.mouseMoveSnap < -30 )  //was -100
								{
									t.importer.mouseMoveSnap = 0;
									t.importer.objectAngleY -= 45;
									if (  t.importer.objectAngleY > 0 && t.importer.objectAngleY < 45  )  t.importer.objectAngleY  =  0;
									if (  t.importer.objectAngleY > 45 && t.importer.objectAngleY < 90  )  t.importer.objectAngleY  =  45;
									if (  t.importer.objectAngleY > 90 && t.importer.objectAngleY < 135  )  t.importer.objectAngleY  =  90;
									if (  t.importer.objectAngleY > 135 && t.importer.objectAngleY < 225  )  t.importer.objectAngleY  =  135;
									if (  t.importer.objectAngleY > 225 && t.importer.objectAngleY < 315  )  t.importer.objectAngleY  =  225;
									if (  t.importer.objectAngleY > 315 && t.importer.objectAngleY  )  t.importer.objectAngleY  =  315;
								}
								if (  t.importer.mouseMoveSnap > 30 ) // was 100
								{
									t.importer.mouseMoveSnap = 0;
									t.importer.objectAngleY += 45;
									if (  t.importer.objectAngleY > 0 && t.importer.objectAngleY < 45  )  t.importer.objectAngleY  =  45;
									if (  t.importer.objectAngleY > 45 && t.importer.objectAngleY < 90  )  t.importer.objectAngleY  =  90;
									if (  t.importer.objectAngleY > 90 && t.importer.objectAngleY < 135  )  t.importer.objectAngleY  =  135;
									if (  t.importer.objectAngleY > 135 && t.importer.objectAngleY < 225  )  t.importer.objectAngleY  =  225;
									if (  t.importer.objectAngleY > 225 && t.importer.objectAngleY < 315  )  t.importer.objectAngleY  =  315;
									if (  t.importer.objectAngleY > 315 && t.importer.objectAngleY  )  t.importer.objectAngleY  =  0;
								}
							}

							//  Ensure angle is 0-360
							while (  t.importer.objectAngleY > 360 ) 
							{
								t.importer.objectAngleY -= 360;
							}
							while (  t.importer.objectAngleY < 0 ) 
							{
								t.importer.objectAngleY += 360;
							}

							//  Update slider based on object angle
							t.slidersmenuvalue[t.importer.properties1Index][3].value = t.importer.objectAngleY;
						}
					break;
				}
			}

			//  update angle base on slider
			t.importer.objectAngleY = t.slidersmenuvalue[t.importer.properties1Index][3].value;
			YRotateObject (  t.importer.objectnumber,t.importer.objectAngleY );
		}

		//  Tab Two Mode - collision
		if (  g.tabmode  ==  IMPORTERTABPAGE2 ) 
		{
			t.importer.message = "Collision";
			SetAlphaMappingOn (  t.importer.objectnumber , 40 );

			//Views Shortcut keys
			if (  t.inputsys.kscancode  ==  37 ) 
			{
				t.slidersmenuvalue[t.importer.properties2Index][1].value = 2;
				t.slidersmenuvalue[t.importer.properties2Index][1].value_s = "Left";
			}
			if (  t.inputsys.kscancode  ==  39 ) 
			{
				t.slidersmenuvalue[t.importer.properties2Index][1].value = 3;
				t.slidersmenuvalue[t.importer.properties2Index][1].value_s = "Right";
			}
			if (  t.inputsys.kscancode  ==  38 ) 
			{
				t.slidersmenuvalue[t.importer.properties2Index][1].value = 4;
				t.slidersmenuvalue[t.importer.properties2Index][1].value_s = "Top";
			}
			if (  t.inputsys.kscancode  ==  40 ) 
			{
				t.slidersmenuvalue[t.importer.properties2Index][1].value = 1;
				t.slidersmenuvalue[t.importer.properties2Index][1].value_s = "Front";
			}

			// Update Views
			if (  t.slidersmenuvalue[t.importer.properties2Index][1].value  ==  2 ) 
			{
				YRotateObject (   t.importerGridObject[8] , -90 );
				XRotateObject (   t.importerGridObject[8] , 0 );
				YRotateObject (  t.importerGridObject[9] , -90 );
				XRotateObject (   t.importerGridObject[9] , 0 );
				t.importer.viewMessage = "Left";
			}
			if (  t.slidersmenuvalue[t.importer.properties2Index][1].value  ==  3 ) 
			{
				YRotateObject (   t.importerGridObject[8] , 90 );
				XRotateObject (   t.importerGridObject[8] , 0 );
				YRotateObject (  t.importerGridObject[9] , 90 );
				XRotateObject (   t.importerGridObject[9] , 0 );
				t.importer.viewMessage = "Right";
			}
			if (  t.slidersmenuvalue[t.importer.properties2Index][1].value  ==  4 ) 
			{
				YRotateObject (   t.importerGridObject[8] , 0 );
				XRotateObject (   t.importerGridObject[8] , -90 );
				YRotateObject (  t.importerGridObject[9] , 0 );
				XRotateObject (   t.importerGridObject[9] , -90 );
				t.importer.viewMessage = "Top";
			}
			if (  t.slidersmenuvalue[t.importer.properties2Index][1].value  ==  1 ) 
			{
				YRotateObject (   t.importerGridObject[8] , 0 );
				XRotateObject (   t.importerGridObject[8] , 0 );
				YRotateObject (  t.importerGridObject[9] , 0 );
				XRotateObject (   t.importerGridObject[9] , 0 );
				t.importer.viewMessage = "Front";
			}

			//  Update collision pivot, using turn object (Sin ( ce we are rotating this x and y to Line (  up with the grid, then need to to rotate it with the child object ) )
			//  This gets round a gimble lock issue that occurs from using euler, resseting the rotation at the begnining of the loop sorts things out
			YRotateObject (  t.importer.objectnumber,0 );
			importer_check_for_physics_changes ( );
			importer_update_selection_markers ( );
		}

		if (  g.tabmode  ==  IMPORTERTABPAGE3 ) 
		{
			SetAlphaMappingOn (  t.importer.objectnumber , 100 );
			SetObjectTransparency (  t.importer.objectnumber, 255 );

			//  Front view
			YRotateObject (   t.importerGridObject[8] , 0 );
			XRotateObject (   t.importerGridObject[8] , 0 );
			YRotateObject (   t.importerGridObject[9] , 0 );
			XRotateObject (   t.importerGridObject[9] , 0 );

			//  Rotation control
			switch (  t.importer.objectRotateMode ) 
			{
				case 0:

					if (  t.inputsys.mclick  ==  1 && t.importer.oldMouseClick  ==  0 ) 
					{
						if (  PickScreenObjectEx ( t.importer.MouseX , t.importer.MouseY , t.importer.objectnumber , t.importer.objectnumber , 1, 0 )  !=  0 ) 
						{
							t.importer.objectRotateMode = 1;
							t.tnothing = t.inputsys.xmousemove;
							importer_hide_mouse ( );
						}
					}

				break;
				case 1:

					if (  t.inputsys.mclick  ==  0 ) 
					{
						t.importer.objectRotateMode = 0;
						importer_show_mouse ( );
					}
					else
					{
						t.importer.message = "How down shift to lock to 45' Steps";
						if (  t.inputsys.keyshift  ==  0 ) 
						{
							t.importer.objectAngleY2 -= t.inputsys.xmousemove;
						}
						else
						{
							t.importer.message = "Locked to 45' steps";
							t.importer.mouseMoveSnap -= t.inputsys.xmousemove;
							if (  t.importer.mouseMoveSnap < -100 ) 
							{

								t.importer.mouseMoveSnap = 0;
								t.importer.objectAngleY2 -= 45;

								if (  t.importer.objectAngleY2 > 0 && t.importer.objectAngleY2 < 45  )  t.importer.objectAngleY2  =  0;
								if (  t.importer.objectAngleY2 > 45 && t.importer.objectAngleY2 < 90  )  t.importer.objectAngleY2  =  45;
								if (  t.importer.objectAngleY2 > 90 && t.importer.objectAngleY2 < 135  )  t.importer.objectAngleY2  =  90;
								if (  t.importer.objectAngleY2 > 135 && t.importer.objectAngleY2 < 225  )  t.importer.objectAngleY2  =  135;
								if (  t.importer.objectAngleY2 > 225 && t.importer.objectAngleY2 < 315  )  t.importer.objectAngleY2  =  225;
								if (  t.importer.objectAngleY2 > 315 && t.importer.objectAngleY2  )  t.importer.objectAngleY2  =  315;

							}
							if (  t.importer.mouseMoveSnap > 100 ) 
							{

								t.importer.mouseMoveSnap = 0;
								t.importer.objectAngleY2 += 45;

								if (  t.importer.objectAngleY2 > 0 && t.importer.objectAngleY2 < 45  )  t.importer.objectAngleY2  =  45;
								if (  t.importer.objectAngleY2 > 45 && t.importer.objectAngleY2 < 90  )  t.importer.objectAngleY2  =  90;
								if (  t.importer.objectAngleY2 > 90 && t.importer.objectAngleY2 < 135  )  t.importer.objectAngleY2  =  135;
								if (  t.importer.objectAngleY2 > 135 && t.importer.objectAngleY2 < 225  )  t.importer.objectAngleY2  =  225;
								if (  t.importer.objectAngleY2 > 225 && t.importer.objectAngleY2 < 315  )  t.importer.objectAngleY2  =  315;
								if (  t.importer.objectAngleY2 > 315 && t.importer.objectAngleY2  )  t.importer.objectAngleY2  =  0;
							}
						}

						//  Ensure angle is 0-360
						while (  t.importer.objectAngleY2 > 360 ) 
						{
							t.importer.objectAngleY2 -= 360;
						}
						while (  t.importer.objectAngleY2 < 0 ) 
						{
							t.importer.objectAngleY2 += 360;
						}

						//  Update slider based on object angle
						t.slidersmenuvalue[t.importer.properties3Index][2].value = t.importer.objectAngleY2;
					}

				break;

			}

			//  update angle base on slider
			t.importer.objectAngleY2 = t.slidersmenuvalue[t.importer.properties3Index][2].value;
			RotateObject (  t.importer.objectnumber,t.slidersmenuvalue[t.importer.properties3Index][1].value,t.slidersmenuvalue[t.importer.properties3Index][2].value,t.slidersmenuvalue[t.importer.properties3Index][3].value );
		}

		if (  t.importer.showCollisionOnly  ==  1 ) 
		{
			t.importer.message = "Showing Collision Only";
			importer_ShowCollisionOnly ( );
		}

		//  handle scale
		importer_handleScale ( );

		//  update tabs
		importer_tabs_update ( );

		//  update textures
		importer_update_textures ( );

		//  handle exit (260416 - added trigger reload of model for FBX core flags)
#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
		if (t.importer.cancel == 1)//|| g_iTriggerReloadOfImportModel == 1 ) 
#else
		if (ControlKey() == 1 || t.importer.cancel == 1)//|| g_iTriggerReloadOfImportModel == 1 ) 
#endif
		{
			--t.importer.cancelCount;
			if ( t.importer.cancelCount <= 0 )
			{
				importer_quit ( );
			}
		}
	}

#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	t.importer.oldMouseClick = t.inputsys.mclick;
#else
	// escape importer
	if (  t.inputsys.kscancode  ==  27 && t.importer.oldShowCollision  ==  0 ) 
	{
		t.importer.showCollisionOnly = 1 - t.importer.showCollisionOnly;
		t.importer.oldShowCollision = 1;
		if ( t.importer.showCollisionOnly == 0 ) importer_ShowCollisionOnlyOff ( );
	}
	if (  t.inputsys.kscancode  !=  27  )  t.importer.oldShowCollision  =  0;
	t.importer.oldMouseClick = t.inputsys.mclick;
#endif

	//  110315 - 019 - fix for mouse dissapearing
	if (  t.timportersaveon  ==  1 ) 
	{
		importer_show_mouse ( );
	}

	#endif // wicked endif
}

void importer_update_selection_markers ( void )
{
	//  Show / hide collision depending which tab is selected
	if (  t.importerTabs[2].selected  ==  0 ) 
	{
		for ( int tCount = 0 ; tCount<=  t.importer.collisionShapeCount-1; tCount++ )
		{
			if (  t.importerCollision[tCount].object > 0 ) 
			{
				if (  ObjectExist(t.importerCollision[tCount].object)  ==  1 ) 
				{
					HideObject (  t.importerCollision[tCount].object );
					HideObject (  t.importerCollision[tCount].object2 );
				}
			}
		}
		for ( t.tCount2 = 1 ; t.tCount2<=  9; t.tCount2++ )
		{
			if (  t.selectedObjectMarkers[t.tCount2] > 0 ) 
			{
				if (  ObjectExist(t.selectedObjectMarkers[t.tCount2])  ==  1  )  HideObject (  t.selectedObjectMarkers[t.tCount2] );
			}
		}
	}
	else
	{
		if (  t.importer.collisionShapeCount > 0 ) 
		{
			if (t.importer.selectedCollisionObject >= 0) {
				if (t.importerCollision[t.importer.selectedCollisionObject].object > 0 && t.importerCollision[t.importer.selectedCollisionObject].object2 > 0)
				{
					if (ObjectExist(t.importerCollision[t.importer.selectedCollisionObject].object) == 1 && ObjectExist(t.importerCollision[t.importer.selectedCollisionObject].object2) == 1)
					{
						t.importerCollision[t.importer.selectedCollisionObject].rotx = t.slidersmenuvalue[t.importer.properties2Index][7].value;
						t.importerCollision[t.importer.selectedCollisionObject].roty = t.slidersmenuvalue[t.importer.properties2Index][8].value;
						t.importerCollision[t.importer.selectedCollisionObject].rotz = t.slidersmenuvalue[t.importer.properties2Index][9].value;
						RotateObject(t.importerCollision[t.importer.selectedCollisionObject].object, t.importerCollision[t.importer.selectedCollisionObject].rotx, t.importerCollision[t.importer.selectedCollisionObject].roty, t.importerCollision[t.importer.selectedCollisionObject].rotz);
						RotateObject(t.importerCollision[t.importer.selectedCollisionObject].object2, t.importerCollision[t.importer.selectedCollisionObject].rotx, t.importerCollision[t.importer.selectedCollisionObject].roty, t.importerCollision[t.importer.selectedCollisionObject].rotz);
					}
				}
			}
		}
		if (  t.importer.collisionShapeCount  ==  0 ) 
		{
			for ( t.tCount2 = 1 ; t.tCount2<=  9; t.tCount2++ )
			{
				if (  t.selectedObjectMarkers[t.tCount2] > 0 ) 
				{
					if (  ObjectExist(t.selectedObjectMarkers[t.tCount2])  ==  1  )  HideObject (  t.selectedObjectMarkers[t.tCount2] );
				}
			}
			return;
		}
		for ( int tCount = 0 ; tCount<=  t.importer.collisionShapeCount-1; tCount++ )
		{
			if (  t.importerCollision[tCount].object > 0 ) 
			{
				if (  ObjectExist(t.importerCollision[tCount].object)  ==  1 ) 
				{
					ShowObject (  t.importerCollision[tCount].object2 );
					if (  t.importer.selectedCollisionObject  ==  tCount ) 
					{
						for ( t.tCount2 = 1 ; t.tCount2<=  9; t.tCount2++ )
						{
							ShowObject (  t.selectedObjectMarkers[t.tCount2] );
							RotateObject (  t.selectedObjectMarkers[t.tCount2],0,0,0 );
						}

						//  position pivot at 0,0,0 so we can glue things to it
						PositionObject ( t.importerGridObject[9], 0, 0, 0 );

						//  top front left
						UnGlueObject (  t.selectedObjectMarkers[1] );
						PositionObject (  t.selectedObjectMarkers[1], ObjectPositionX (t.importerCollision[tCount].object2), ObjectPositionY (t.importerCollision[tCount].object2), ObjectPositionZ (t.importerCollision[tCount].object2) ) ;
						RotateObject (  t.selectedObjectMarkers[1], ObjectAngleX (t.importerCollision[tCount].object2),ObjectAngleY(t.importerCollision[tCount].object2),ObjectAngleZ(t.importerCollision[tCount].object2) );
						MoveObjectUp (  t.selectedObjectMarkers[1], LimbScaleY(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObject (  t.selectedObjectMarkers[1], -LimbScaleZ(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObjectLeft (  t.selectedObjectMarkers[1], LimbScaleX(t.importerCollision[tCount].object2,0) / 2.0 );
						GlueObjectToLimb (  t.selectedObjectMarkers[1], t.importerGridObject[9],0 );

						//  top front right
						UnGlueObject (  t.selectedObjectMarkers[2] );
						PositionObject (  t.selectedObjectMarkers[2], ObjectPositionX (t.importerCollision[tCount].object2), ObjectPositionY (t.importerCollision[tCount].object2), ObjectPositionZ (t.importerCollision[tCount].object2) ) ;
						RotateObject (  t.selectedObjectMarkers[2], ObjectAngleX (t.importerCollision[tCount].object2),ObjectAngleY(t.importerCollision[tCount].object2),ObjectAngleZ(t.importerCollision[tCount].object2) );
						MoveObjectUp (  t.selectedObjectMarkers[2], LimbScaleY(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObject (  t.selectedObjectMarkers[2], -LimbScaleZ(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObjectRight (  t.selectedObjectMarkers[2], LimbScaleX(t.importerCollision[tCount].object2,0) / 2.0 );
						GlueObjectToLimb (  t.selectedObjectMarkers[2], t.importerGridObject[9],0 );

						//  bottom front left
						UnGlueObject (  t.selectedObjectMarkers[3] );
						PositionObject (  t.selectedObjectMarkers[3], ObjectPositionX (t.importerCollision[tCount].object2), ObjectPositionY (t.importerCollision[tCount].object2), ObjectPositionZ (t.importerCollision[tCount].object2) ) ;
						RotateObject (  t.selectedObjectMarkers[3], ObjectAngleX (t.importerCollision[tCount].object2),ObjectAngleY(t.importerCollision[tCount].object2),ObjectAngleZ(t.importerCollision[tCount].object2) ) ;
						MoveObjectDown (  t.selectedObjectMarkers[3], LimbScaleY(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObject (  t.selectedObjectMarkers[3], -LimbScaleZ(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObjectLeft (  t.selectedObjectMarkers[3], LimbScaleX(t.importerCollision[tCount].object2,0) / 2.0 );
						GlueObjectToLimb (  t.selectedObjectMarkers[3], t.importerGridObject[9],0 );

						//  bottom front right
						UnGlueObject (  t.selectedObjectMarkers[4] );
						PositionObject (  t.selectedObjectMarkers[4], ObjectPositionX (t.importerCollision[tCount].object2), ObjectPositionY (t.importerCollision[tCount].object2), ObjectPositionZ (t.importerCollision[tCount].object2) )  ;
						RotateObject (  t.selectedObjectMarkers[4], ObjectAngleX (t.importerCollision[tCount].object2),ObjectAngleY(t.importerCollision[tCount].object2),ObjectAngleZ(t.importerCollision[tCount].object2) ) ;
						MoveObjectDown (  t.selectedObjectMarkers[4], LimbScaleY(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObject (  t.selectedObjectMarkers[4], -LimbScaleZ(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObjectRight (  t.selectedObjectMarkers[4], LimbScaleX(t.importerCollision[tCount].object2,0) / 2.0 );
						GlueObjectToLimb (  t.selectedObjectMarkers[4], t.importerGridObject[9],0 );

						//  top back left
						UnGlueObject (  t.selectedObjectMarkers[5] );
						PositionObject (  t.selectedObjectMarkers[5], ObjectPositionX (t.importerCollision[tCount].object2), ObjectPositionY (t.importerCollision[tCount].object2), ObjectPositionZ (t.importerCollision[tCount].object2) ) ;
						RotateObject (  t.selectedObjectMarkers[5], ObjectAngleX (t.importerCollision[tCount].object2),ObjectAngleY(t.importerCollision[tCount].object2),ObjectAngleZ(t.importerCollision[tCount].object2) );
						MoveObjectUp (  t.selectedObjectMarkers[5], LimbScaleY(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObject (  t.selectedObjectMarkers[5], LimbScaleZ(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObjectLeft (  t.selectedObjectMarkers[5], LimbScaleX(t.importerCollision[tCount].object2,0) / 2.0 );
						GlueObjectToLimb (  t.selectedObjectMarkers[5], t.importerGridObject[9],0 );

						//  top back right
						UnGlueObject (  t.selectedObjectMarkers[6] );
						PositionObject (  t.selectedObjectMarkers[6], ObjectPositionX (t.importerCollision[tCount].object2), ObjectPositionY (t.importerCollision[tCount].object2), ObjectPositionZ (t.importerCollision[tCount].object2) ) ;
						RotateObject (  t.selectedObjectMarkers[6], ObjectAngleX (t.importerCollision[tCount].object2),ObjectAngleY(t.importerCollision[tCount].object2),ObjectAngleZ(t.importerCollision[tCount].object2) ) ;
						MoveObjectUp (  t.selectedObjectMarkers[6], LimbScaleY(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObject (  t.selectedObjectMarkers[6], LimbScaleZ(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObjectRight (  t.selectedObjectMarkers[6], LimbScaleX(t.importerCollision[tCount].object2,0) / 2.0 );
						GlueObjectToLimb (  t.selectedObjectMarkers[6], t.importerGridObject[9],0 );

						//  bottom back left
						UnGlueObject (  t.selectedObjectMarkers[7] );
						PositionObject (  t.selectedObjectMarkers[7], ObjectPositionX (t.importerCollision[tCount].object2), ObjectPositionY (t.importerCollision[tCount].object2), ObjectPositionZ (t.importerCollision[tCount].object2) ) ;
						RotateObject (  t.selectedObjectMarkers[7], ObjectAngleX (t.importerCollision[tCount].object2),ObjectAngleY(t.importerCollision[tCount].object2),ObjectAngleZ(t.importerCollision[tCount].object2) );
						MoveObjectDown (  t.selectedObjectMarkers[7], LimbScaleY(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObject (  t.selectedObjectMarkers[7], LimbScaleZ(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObjectLeft (  t.selectedObjectMarkers[7], LimbScaleX(t.importerCollision[tCount].object2,0) / 2.0 );
						GlueObjectToLimb (  t.selectedObjectMarkers[7], t.importerGridObject[9],0 );

						//  bottom back right
						UnGlueObject (  t.selectedObjectMarkers[8] );
						PositionObject (  t.selectedObjectMarkers[8], ObjectPositionX (t.importerCollision[tCount].object2), ObjectPositionY (t.importerCollision[tCount].object2), ObjectPositionZ (t.importerCollision[tCount].object2) ) ;
						RotateObject (  t.selectedObjectMarkers[8], ObjectAngleX (t.importerCollision[tCount].object2),ObjectAngleY(t.importerCollision[tCount].object2),ObjectAngleZ(t.importerCollision[tCount].object2) );
						MoveObjectDown (  t.selectedObjectMarkers[8], LimbScaleY(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObject (  t.selectedObjectMarkers[8], LimbScaleZ(t.importerCollision[tCount].object2,0) / 2.0 );
						MoveObjectRight (  t.selectedObjectMarkers[8], LimbScaleX(t.importerCollision[tCount].object2,0) / 2.0 );
						GlueObjectToLimb (  t.selectedObjectMarkers[8], t.importerGridObject[9],0 );

						//  center
						UnGlueObject (  t.selectedObjectMarkers[9] );
						PositionObject (  t.selectedObjectMarkers[9], ObjectPositionX (t.importerCollision[tCount].object2), ObjectPositionY (t.importerCollision[tCount].object2), ObjectPositionZ (t.importerCollision[tCount].object2) ) ;
						RotateObject (  t.selectedObjectMarkers[9], ObjectAngleX (t.importerCollision[tCount].object2),ObjectAngleY(t.importerCollision[tCount].object2),ObjectAngleZ(t.importerCollision[tCount].object2) ) ;
						GlueObjectToLimb (  t.selectedObjectMarkers[9], t.importerGridObject[9],0 );

						//  put pivot back into the scene
						PositionObject (   t.importerGridObject[9],0,0,IMPORTERZPOSITION );

					}
				}
			}
		}
	}

	for ( int tCount = 1 ; tCount<=  9; tCount++ )
	{
		if (t.importer.selectedCollisionObject >= 0) {
			t.snapPosX_f[t.importer.selectedCollisionObject][tCount] = ObjectPositionX(t.selectedObjectMarkers[tCount]);
			t.snapPosY_f[t.importer.selectedCollisionObject][tCount] = ObjectPositionY(t.selectedObjectMarkers[tCount]);
			t.snapPosZ_f[t.importer.selectedCollisionObject][tCount] = ObjectPositionZ(t.selectedObjectMarkers[tCount]);
		}
	}
}

void importer_ShowCollisionOnly ( void )
{
	HideObject (  t.importer.objectnumber );
	for ( int tCount = 0 ; tCount<=  t.importer.collisionShapeCount-1; tCount++ )
	{
		if (  t.importerCollision[tCount].object > 0 ) 
		{
			if (  ObjectExist(t.importerCollision[tCount].object)  ==  1 ) 
			{
				ShowObject (  t.importerCollision[tCount].object );
				SetObjectLight (  t.importerCollision[tCount].object, 1 );
				ColorObject (  t.importerCollision[tCount].object , Rgb(255,255,100) );
				///GhostObjectOff (  t.importerCollision[tCount].object );
			}
		}
	}
	RotateObject (  t.importerGridObject[9], ObjectAngleX(t.importer.objectnumber), ObjectAngleY(t.importer.objectnumber), ObjectAngleZ(t.importer.objectnumber) );
}

void importer_ShowCollisionOnlyOff ( void )
{
	ShowObject (  t.importer.objectnumber );
	for ( int tCount = 0 ; tCount<=  t.importer.collisionShapeCount-1; tCount++ )
	{
		if (  t.importerCollision[tCount].object > 0 ) 
		{
			if (  ObjectExist(t.importerCollision[tCount].object)  ==  1 ) 
			{
				///GhostObjectOn (  t.importerCollision[tCount].object );
				HideObject (  t.importerCollision[tCount].object );
			}
		}
	}
}

void importer_snapLeft ( void )
{
	t.tx_f = ObjectPositionX(t.tSnapObject);
	for ( int tCount = 0 ; tCount<=  t.importer.collisionShapeCount-1; tCount++ )
	{
		if (  tCount  !=  t.importer.selectedCollisionObject ) 
		{
			for ( t.tCount2 = 1 ; t.tCount2<=  8; t.tCount2++ )
			{
				t.tdiff_f = t.tx_f - t.snapPosX_f[tCount][t.tCount2];
				if (  abs(t.tdiff_f) < 5 ) 
				{
					MoveObject (  t.tSnapObject, t.tdiff_f );
					return;
				}
			}
		}
	}
}

void importer_snapUp ( void )
{
	t.ty_f = ObjectPositionY(t.tSnapObject);
	for ( int tCount = 0 ; tCount<=  t.importer.collisionShapeCount-1; tCount++ )
	{
		if (  tCount  !=  t.importer.selectedCollisionObject ) 
		{
			for ( t.tCount2 = 1 ; t.tCount2<=  8; t.tCount2++ )
			{
				t.tdiff_f = t.ty_f - t.snapPosY_f[tCount][t.tCount2];
				if (  abs(t.tdiff_f) < 5 ) 
				{
					MoveObjectUp (  t.tSnapObject, t.tdiff_f );
					return;
				}
			}
		}
	}
}

void importer_snapforward ( void )
{
	t.tz_f = ObjectPositionZ(t.tSnapObject);
	for ( int tCount = 0 ; tCount<=  t.importer.collisionShapeCount-1; tCount++ )
	{
		if (  tCount  !=  t.importer.selectedCollisionObject ) 
		{
			for ( t.tCount2 = 1 ; t.tCount2<=  8; t.tCount2++ )
			{
				t.tdiff_f = t.tz_f - t.snapPosZ_f[tCount][t.tCount2];
				if (  abs(t.tdiff_f) < 5 ) 
				{
					MoveObject (  t.tSnapObject, t.tdiff_f );
					return;
				}
			}
		}
	}
}

void importer_check_for_physics_changes ( void )
{
	if (  t.importer.collisionShapeCount  ==  0  )  return;
	int tCount = 0;
	for ( tCount = 0 ; tCount<=  t.importer.collisionShapeCount-1; tCount++ )
	{
			if (  t.snapPosX_f[tCount][1]  ==  -99999 ) 
			{
				t.importer.selectedCollisionObject = tCount;
				t.slidersmenuvalue[t.importer.properties2Index][7].value = t.importerCollision[t.importer.selectedCollisionObject].rotx;
				t.slidersmenuvalue[t.importer.properties2Index][8].value = t.importerCollision[t.importer.selectedCollisionObject].roty;
				t.slidersmenuvalue[t.importer.properties2Index][9].value = t.importerCollision[t.importer.selectedCollisionObject].rotz;
				importer_update_selection_markers ( );
			}
	}

	// coordinate system for importer grabbing collision box corners
	//float fMX = (GetDisplayWidth()+0.0) / 800.0f; 230618 - not working for collision box control
	//float fMY = (GetDisplayHeight()+0.0) / 600.0f;
	//t.tadjustedtoareax_f = t.inputsys.xmouse*fMX;
	//t.tadjustedtoareay_f = t.inputsys.ymouse*fMY;
#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	t.tadjustedtoareax_f = ((float)t.inputsys.xmouse / (float)GetDisplayWidth()) / ((float)GetDisplayWidth() / (float)GetChildWindowWidth(-1));
	t.tadjustedtoareay_f = ((float)t.inputsys.ymouse / (float)GetDisplayHeight()) / ((float)GetDisplayHeight() / (float)GetChildWindowHeight(-1));
#else

	t.tadjustedtoareax_f=(GetDisplayWidth()+0.0)/(GetChildWindowWidth()+0.0);
	t.tadjustedtoareay_f=(GetDisplayHeight()+0.0)/(GetChildWindowHeight()+0.0);
	t.tadjustedtoareax_f=((t.inputsys.xmouse+0.0)/800.0)/t.tadjustedtoareax_f;
	t.tadjustedtoareay_f=((t.inputsys.ymouse+0.0)/600.0)/t.tadjustedtoareay_f;
#endif
	t.tadjustedtoareax_f = t.tadjustedtoareax_f*(GetDisplayWidth() + 0.0);
	t.tadjustedtoareay_f = t.tadjustedtoareay_f*(GetDisplayHeight() + 0.0);

	//  No marker selected yet
	if (  t.importer.collisionObjectMode  ==  0 ) 
	{
		if (  t.inputsys.mclick  ==  1 ) 
		{
			//  Have we clicked on a marker?
			t.importer.selectedMarker = 0;
			for ( tCount = 1 ; tCount<=  9; tCount++ )
			{
				t.picked = PickScreenObjectEx(t.tadjustedtoareax_f, t.tadjustedtoareay_f, t.selectedObjectMarkers[tCount], t.selectedObjectMarkers[tCount],1,0);
				ScaleObject (  t.selectedObjectMarkers[tCount],100,100,100 );
				SetObjectAmbience (  t.selectedObjectMarkers[tCount],0 );
				if (  tCount  !=  9 ) 
				{
					SetObjectEmissive (  t.selectedObjectMarkers[tCount], Rgb(255,255,0) );
				}
				else
				{
					SetObjectEmissive (  t.selectedObjectMarkers[tCount], Rgb(0,255,255) );
				}
				if (  t.picked  !=  0 ) 
				{
					//  Enlarge and recolor the selected corner marker
					t.importer.selectedMarker = tCount;
					t.tScaleX_f = t.inputsys.xmousemove ; t.tScaleY_f = t.inputsys.ymousemove;
					ScaleObject (  t.selectedObjectMarkers[tCount],100,100,100 );
					SetObjectAmbience (  t.selectedObjectMarkers[tCount],0 );
					if (  tCount  !=  9 ) 
					{
						SetObjectEmissive (  t.selectedObjectMarkers[tCount], Rgb(255,100,0) );
					}
					else
					{
						SetObjectEmissive (  t.selectedObjectMarkers[tCount], Rgb(0,100,255) );
					}

#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
					float xpick = ((float)t.inputsys.xmouse / (float)GetDisplayWidth()) / ((float)GetDisplayWidth() / (float)GetChildWindowWidth(-1));
					float ypick = ((float)t.inputsys.ymouse / (float)GetDisplayHeight()) / ((float)GetDisplayHeight() / (float)GetChildWindowHeight(-1));
#else
					float xpick=(GetDisplayWidth()+0.0)/(GetChildWindowWidth()+0.0);
					float ypick=(GetDisplayHeight()+0.0)/(GetChildWindowHeight()+0.0);
					//  scale full mouse to fit in visible area
					xpick=((t.inputsys.xmouse+0.0)/800.0)/t.widgetinputsysxmouse_f;
					ypick=((t.inputsys.ymouse+0.0)/600.0)/t.widgetinputsysymouse_f;
#endif
					//  then provide in a format for the pick-from-screen command
					xpick=xpick*(GetChildWindowWidth()+0.0);
					ypick=ypick*(GetChildWindowHeight()+0.0);
					PickScreen2D23D (  xpick , ypick , IMPORTERZPOSITION );
					t.timporterpickdepth_f =  LimbPositionZ(t.selectedObjectMarkers[t.importer.selectedMarker],0);
					t.timporterpickdepth_f = t.timporterpickdepth_f * t.tadjustedtoimporterxbase_f;
					PickScreen2D23D (  xpick, ypick, t.timporterpickdepth_f );
					t.timporteroldmousex = GetPickVectorX();
					t.timporteroldmousey = GetPickVectorZ();
				}
			}

			//  If we have not clicked on a marker, then perhaps another collision box?
			if (  t.importer.selectedMarker  ==  0 ) 
			{
				for ( tCount = 0 ; tCount<=  t.importer.collisionShapeCount-1; tCount++ )
				{
					if (  PickScreenObjectEx(t.tadjustedtoareax_f, t.tadjustedtoareay_f, t.importerCollision[tCount].object2, t.importerCollision[tCount].object2,1,0) ) 
					{
						t.importer.selectedCollisionObject = tCount;
						t.slidersmenuvalue[t.importer.properties2Index][7].value = ObjectAngleX(t.importerCollision[tCount].object2);
						t.slidersmenuvalue[t.importer.properties2Index][8].value = ObjectAngleY(t.importerCollision[tCount].object2);
						t.slidersmenuvalue[t.importer.properties2Index][9].value = ObjectAngleZ(t.importerCollision[tCount].object2);
						break;
					}
				}
			}
			else
			{
				t.importer.collisionObjectMode = 1;
			}
		}
	}
	else
	{
		//  Marker is selected, lets deal with it
		if (  t.inputsys.mclick  ==  1 ) 
		{
			for ( tCount = 1 ; tCount<=  9; tCount++ )
			{
				if (  t.importer.selectedMarker  !=  tCount  )  
				{
					ScaleObject (  t.selectedObjectMarkers[tCount],50,50,50 );
				}
			}

			importer_hide_mouse ( );

			t.tXOnly = 1;
			t.tYOnly = 1;
			t.tZOnly = 1;
			t.importer.message = "Hold X,C or V to lock an axis";
			if (  t.inputsys.kscancode  ==  88 || t.inputsys.kscancode  ==  67 || t.inputsys.kscancode  ==  86 ) 
			{
				t.tXOnly = 0 ; t.tYOnly = 0 ; t.tZOnly = 0;
			}
			if (  t.inputsys.kscancode  ==  88  )  t.tXOnly  =  1;
			if (  t.inputsys.kscancode  ==  67  )  t.tYOnly  =  1;
			if (  t.inputsys.kscancode  ==  86  )  t.tZOnly  =  1;

			t.tScaleX_f = 0.0;
			t.tScaleY_f = 0.0;
			t.tScaleZ_f = 0.0;
			t.tMultiX_f = 1.0;
			t.tMultiY_f = 1.0;
			t.tMultiZ_f = 1.0;

			t.tr1_f = t.slidersmenuvalue[t.importer.properties2Index][7].value;
			t.tr2_f = t.slidersmenuvalue[t.importer.properties2Index][8].value;
			t.tr3_f = t.slidersmenuvalue[t.importer.properties2Index][9].value;

			RotateObject (  t.selectedObjectMarkers[1], t.tr1x_f+360,t.tr2_f+360,t.tr3_f+360 );

			if (  t.importer.selectedMarker  ==  1 ) 
			{

				t.tSnapObject = t.selectedObjectMarkers[1];

				t.oldx1_f = ObjectPositionX(t.selectedObjectMarkers[1]);
				t.oldy1_f = ObjectPositionY(t.selectedObjectMarkers[1]);
				t.oldz1_f = ObjectPositionZ(t.selectedObjectMarkers[1]);
				t.oldx2_f = ObjectPositionX(t.selectedObjectMarkers[2]);
				t.oldy2_f = ObjectPositionY(t.selectedObjectMarkers[3]);
				t.oldz2_f = ObjectPositionZ(t.selectedObjectMarkers[5]);

				if (  t.importer.viewMessage  ==  "Front" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[1],90 );
					MoveObject (  t.selectedObjectMarkers[1], -t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[1], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapLeft ( );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Top" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[1],90 );
					MoveObject (  t.selectedObjectMarkers[1], -t.inputsys.xmousemove );
					TurnObjectLeft (  t.selectedObjectMarkers[1],-90 );
					MoveObject (  t.selectedObjectMarkers[1], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						TurnObjectLeft (  t.selectedObjectMarkers[1],90 );
						importer_snapLeft ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Left" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[1],180 );
					MoveObject (  t.selectedObjectMarkers[1], -t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[1], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[1],180 );
						importer_snapforward ( );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Right" ) 
				{
					TurnObjectRight (  t.selectedObjectMarkers[1],180 );
					MoveObject (  t.selectedObjectMarkers[1], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[1], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[1],180 );
						importer_snapforward ( );
						importer_snapUp ( );
					}
				}

				t.newx1_f = ObjectPositionX(t.selectedObjectMarkers[1]);
				t.newy1_f = ObjectPositionY(t.selectedObjectMarkers[1]);
				t.newz1_f = ObjectPositionZ(t.selectedObjectMarkers[1]);

				t.dist1x_f = GetDistance( t.oldx1_f,0,0,t.oldx2_f,0,0);
				t.dist2x_f = GetDistance( t.newx1_f,0,0,t.oldx2_f,0,0);

				//  X plane
				if (  t.oldx2_f > t.oldx1_f ) 
				{
					t.tScaleX_f = -(t.dist2x_f - t.dist1x_f);
				}
				else
				{
					t.tScaleX_f = (t.dist2x_f - t.dist1x_f);
				}

				//  Y plane
				t.dist1y_f = GetDistance( t.oldy1_f,0,0,t.oldy2_f,0,0);
				t.dist2y_f = GetDistance( t.newy1_f,0,0,t.oldy2_f,0,0);

				if (  t.oldy2_f > t.oldy1_f ) 
				{
					t.tScaleY_f = -(t.dist2y_f - t.dist1y_f);
				}
				else
				{
					t.tScaleY_f = (t.dist2y_f - t.dist1y_f);
				}

				//  Z plane
				t.dist1z_f = GetDistance( t.oldz1_f,0,0,t.oldz2_f,0,0);
				t.dist2z_f = GetDistance( t.newz1_f,0,0,t.oldz2_f,0,0);

				if (  t.oldz2_f > t.oldz1_f ) 
				{
					t.tScaleZ_f = -(t.dist2z_f - t.dist1z_f);
				}
				else
				{
					t.tScaleZ_f = (t.dist2z_f - t.dist1z_f);
				}
			}

			if (  t.importer.selectedMarker  ==  2 ) 
			{
				t.tSnapObject = t.selectedObjectMarkers[2];
				t.tMultiX_f = -1;
				t.oldx1_f = ObjectPositionX(t.selectedObjectMarkers[2]);
				t.oldy1_f = ObjectPositionY(t.selectedObjectMarkers[2]);
				t.oldz1_f = ObjectPositionZ(t.selectedObjectMarkers[2]);
				t.oldx2_f = ObjectPositionX(t.selectedObjectMarkers[1]);
				t.oldy2_f = ObjectPositionY(t.selectedObjectMarkers[4]);
				t.oldz2_f = ObjectPositionZ(t.selectedObjectMarkers[6]);

				if (  t.importer.viewMessage  ==  "Front" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[2],90 );
					MoveObject (  t.selectedObjectMarkers[2], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[2], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[2],180 );
						importer_snapLeft ( );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Top" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[2],90 );
					MoveObject (  t.selectedObjectMarkers[2], t.inputsys.xmousemove );
					TurnObjectLeft (  t.selectedObjectMarkers[2],-90 );
					MoveObject (  t.selectedObjectMarkers[2], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						TurnObjectLeft (  t.selectedObjectMarkers[2],180 );
						TurnObjectLeft (  t.selectedObjectMarkers[2],90 );
						importer_snapLeft ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Left" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[2],180 );
					MoveObject (  t.selectedObjectMarkers[2], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[2], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Right" ) 
				{
					TurnObjectRight (  t.selectedObjectMarkers[2],180 );
					MoveObject (  t.selectedObjectMarkers[2], -t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[2], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						importer_snapUp ( );
					}
				}

				t.newx1_f = ObjectPositionX(t.selectedObjectMarkers[2]);
				t.newy1_f = ObjectPositionY(t.selectedObjectMarkers[2]);
				t.newz1_f = ObjectPositionZ(t.selectedObjectMarkers[2]);

				t.dist1x_f = GetDistance( t.oldx1_f,0,0,t.oldx2_f,0,0);
				t.dist2x_f = GetDistance( t.newx1_f,0,0,t.oldx2_f,0,0);

				//  X plane
				if (  t.oldx2_f > t.oldx1_f ) 
				{
					t.tScaleX_f = -(t.dist2x_f - t.dist1x_f);
				}
				else
				{
					t.tScaleX_f = (t.dist2x_f - t.dist1x_f);
				}

				//  Y plane
				t.dist1y_f = GetDistance( t.oldy1_f,0,0,t.oldy2_f,0,0);
				t.dist2y_f = GetDistance( t.newy1_f,0,0,t.oldy2_f,0,0);

				if (  t.oldy2_f > t.oldy1_f ) 
				{
					t.tScaleY_f = -(t.dist2y_f - t.dist1y_f);
				}
				else
				{
					t.tScaleY_f = (t.dist2y_f - t.dist1y_f);
				}

				//  Z plane
				t.dist1z_f = GetDistance( t.oldz1_f,0,0,t.oldz2_f,0,0);
				t.dist2z_f = GetDistance( t.newz1_f,0,0,t.oldz2_f,0,0);

				if (  t.importer.viewMessage  !=  "Top" ) 
				{
					if (  t.oldz2_f > t.oldz1_f ) 
					{
						t.tScaleZ_f = (t.dist2z_f - t.dist1z_f);
					}
					else
					{
						t.tScaleZ_f = -(t.dist2z_f - t.dist1z_f);
					}
				}
				else
				{
					if (  t.oldz2_f > t.oldz1_f ) 
					{
						t.tScaleZ_f = -(t.dist2z_f - t.dist1z_f);
					}
					else
					{
						t.tScaleZ_f = (t.dist2z_f - t.dist1z_f);
					}
				}
			}

			if (  t.importer.selectedMarker  ==  3 ) 
			{
				t.tSnapObject = t.selectedObjectMarkers[3];

				t.tMultiY_f = -1.0;

				t.oldx1_f = ObjectPositionX(t.selectedObjectMarkers[3]);
				t.oldy1_f = ObjectPositionY(t.selectedObjectMarkers[3]);
				t.oldz1_f = ObjectPositionZ(t.selectedObjectMarkers[3]);
				t.oldx2_f = ObjectPositionX(t.selectedObjectMarkers[4]);
				t.oldy2_f = ObjectPositionY(t.selectedObjectMarkers[1]);
				t.oldz2_f = ObjectPositionZ(t.selectedObjectMarkers[8]);

				if (  t.importer.viewMessage  ==  "Front" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[3],90 );
					MoveObject (  t.selectedObjectMarkers[3], -t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[3], -t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapLeft ( );
						RollObjectRight (  t.selectedObjectMarkers[3],180 );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Top" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[3],90 );
					MoveObject (  t.selectedObjectMarkers[3], -t.inputsys.xmousemove );
					TurnObjectLeft (  t.selectedObjectMarkers[3],-90 );
					MoveObject (  t.selectedObjectMarkers[3], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						TurnObjectLeft (  t.selectedObjectMarkers[3],90 );
						importer_snapLeft ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Left" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[3],180 );
					MoveObject (  t.selectedObjectMarkers[3], -t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[3], -t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[3],180 );
						importer_snapforward ( );
						RollObjectRight (  t.selectedObjectMarkers[3],180 );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Right" ) 
				{
					TurnObjectRight (  t.selectedObjectMarkers[3],180 );
					MoveObject (  t.selectedObjectMarkers[3], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[3], -t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectRight (  t.selectedObjectMarkers[3],180 );
						importer_snapforward ( );
						RollObjectRight (  t.selectedObjectMarkers[3],180 );
						importer_snapUp ( );
					}
				}

				t.newx1_f = ObjectPositionX(t.selectedObjectMarkers[3]);
				t.newy1_f = ObjectPositionY(t.selectedObjectMarkers[3]);
				t.newz1_f = ObjectPositionZ(t.selectedObjectMarkers[3]);

				t.dist1x_f = GetDistance( t.oldx1_f,0,0,t.oldx2_f,0,0);
				t.dist2x_f = GetDistance( t.newx1_f,0,0,t.oldx2_f,0,0);

				//  X plane
				if (  t.oldx2_f > t.oldx1_f ) 
				{
					t.tScaleX_f = -(t.dist2x_f - t.dist1x_f);
				}
				else
				{
					t.tScaleX_f = (t.dist2x_f - t.dist1x_f);
				}

				//  Y plane
				t.dist1y_f = GetDistance( t.oldy1_f,0,0,t.oldy2_f,0,0);
				t.dist2y_f = GetDistance( t.newy1_f,0,0,t.oldy2_f,0,0);

				if (  t.oldy2_f > t.oldy1_f ) 
				{
					t.tScaleY_f = -(t.dist2y_f - t.dist1y_f);
				}
				else
				{
					t.tScaleY_f = (t.dist2y_f - t.dist1y_f);
				}

				//  Z plane
				t.dist1z_f = GetDistance( t.oldz1_f,0,0,t.oldz2_f,0,0);
				t.dist2z_f = GetDistance( t.newz1_f,0,0,t.oldz2_f,0,0);

				if (  t.oldz2_f > t.oldz1_f ) 
				{
					t.tScaleZ_f = -(t.dist2z_f - t.dist1z_f);
				}
				else
				{
					t.tScaleZ_f = (t.dist2z_f - t.dist1z_f);
				}

			}

			if (  t.importer.selectedMarker  ==  4 ) 
			{

				t.tSnapObject = t.selectedObjectMarkers[4];

				t.tMultiX_f = -1;
				t.tMultiY_f = -1;

				t.oldx1_f = ObjectPositionX(t.selectedObjectMarkers[4]);
				t.oldy1_f = ObjectPositionY(t.selectedObjectMarkers[4]);
				t.oldz1_f = ObjectPositionZ(t.selectedObjectMarkers[4]);
				t.oldx2_f = ObjectPositionX(t.selectedObjectMarkers[3]);
				t.oldy2_f = ObjectPositionY(t.selectedObjectMarkers[2]);
				t.oldz2_f = ObjectPositionZ(t.selectedObjectMarkers[5]);

				if (  t.importer.viewMessage  ==  "Front" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[4],90 );
					MoveObject (  t.selectedObjectMarkers[4], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[4], -t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[4],180 );
						importer_snapLeft ( );
						RollObjectRight (  t.selectedObjectMarkers[4],180 );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Top" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[4],90 );
					MoveObject (  t.selectedObjectMarkers[4], t.inputsys.xmousemove );
					TurnObjectLeft (  t.selectedObjectMarkers[4],-90 );
					MoveObject (  t.selectedObjectMarkers[4], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						TurnObjectLeft (  t.selectedObjectMarkers[4],-90 );
						importer_snapLeft ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Left" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[4],180 );
					MoveObject (  t.selectedObjectMarkers[4], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[4], -t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						RollObjectRight (  t.selectedObjectMarkers[4],180 );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Right" ) 
				{
					TurnObjectRight (  t.selectedObjectMarkers[4],180 );
					MoveObject (  t.selectedObjectMarkers[4], -t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[4], -t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						RollObjectRight (  t.selectedObjectMarkers[4],180 );
						importer_snapUp ( );
					}
				}

				t.newx1_f = ObjectPositionX(t.selectedObjectMarkers[4]);
				t.newy1_f = ObjectPositionY(t.selectedObjectMarkers[4]);
				t.newz1_f = ObjectPositionZ(t.selectedObjectMarkers[4]);

				t.dist1x_f = GetDistance( t.oldx1_f,0,0,t.oldx2_f,0,0);
				t.dist2x_f = GetDistance( t.newx1_f,0,0,t.oldx2_f,0,0);

				//  X plane
				if (  t.oldx2_f > t.oldx1_f ) 
				{
					t.tScaleX_f = -(t.dist2x_f - t.dist1x_f);
				}
				else
				{
					t.tScaleX_f = (t.dist2x_f - t.dist1x_f);
				}

				//  Y plane
				t.dist1y_f = GetDistance( t.oldy1_f,0,0,t.oldy2_f,0,0);
				t.dist2y_f = GetDistance( t.newy1_f,0,0,t.oldy2_f,0,0);

				if (  t.oldy2_f > t.oldy1_f ) 
				{
					t.tScaleY_f = -(t.dist2y_f - t.dist1y_f);
				}
				else
				{
					t.tScaleY_f = (t.dist2y_f - t.dist1y_f);
				}

				//  Z plane
				t.dist1z_f = GetDistance( t.oldz1_f,0,0,t.oldz2_f,0,0);
				t.dist2z_f = GetDistance( t.newz1_f,0,0,t.oldz2_f,0,0);

				if (  t.importer.viewMessage  !=  "Top" ) 
				{
					if (  t.oldz2_f > t.oldz1_f ) 
					{
						t.tScaleZ_f = (t.dist2z_f - t.dist1z_f);
					}
					else
					{
						t.tScaleZ_f = -(t.dist2z_f - t.dist1z_f);
					}
				}
				else
				{
					if (  t.oldz2_f > t.oldz1_f ) 
					{
						t.tScaleZ_f = -(t.dist2z_f - t.dist1z_f);
					}
					else
					{
						t.tScaleZ_f = (t.dist2z_f - t.dist1z_f);
					}
				}
			}

			if (  t.importer.selectedMarker  ==  5 ) 
			{

				t.tSnapObject = t.selectedObjectMarkers[5];

				t.tMultiZ_f = -1;

				t.oldx1_f = ObjectPositionX(t.selectedObjectMarkers[5]);
				t.oldy1_f = ObjectPositionY(t.selectedObjectMarkers[5]);
				t.oldz1_f = ObjectPositionZ(t.selectedObjectMarkers[5]);
				t.oldx2_f = ObjectPositionX(t.selectedObjectMarkers[6]);
				t.oldy2_f = ObjectPositionY(t.selectedObjectMarkers[7]);
				t.oldz2_f = ObjectPositionZ(t.selectedObjectMarkers[1]);

				if (  t.importer.viewMessage  ==  "Front" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[5],90 );
					MoveObject (  t.selectedObjectMarkers[5], -t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[5], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapLeft ( );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Top" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[5],90 );
					MoveObject (  t.selectedObjectMarkers[5], -t.inputsys.xmousemove );
					TurnObjectLeft (  t.selectedObjectMarkers[5],-90 );
					MoveObject (  t.selectedObjectMarkers[5], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						TurnObjectLeft (  t.selectedObjectMarkers[5],90 );
						importer_snapLeft ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Left" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[5],180 );
					MoveObject (  t.selectedObjectMarkers[5], -t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[5], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[5],180 );
						importer_snapforward ( );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Right" ) 
				{
					TurnObjectRight (  t.selectedObjectMarkers[5],180 );
					MoveObject (  t.selectedObjectMarkers[5], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[5], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[5],180 );
						importer_snapforward ( );
						importer_snapUp ( );
					}
				}

				t.newx1_f = ObjectPositionX(t.selectedObjectMarkers[5]);
				t.newy1_f = ObjectPositionY(t.selectedObjectMarkers[5]);
				t.newz1_f = ObjectPositionZ(t.selectedObjectMarkers[5]);

				t.dist1x_f = GetDistance( t.oldx1_f,0,0,t.oldx2_f,0,0);
				t.dist2x_f = GetDistance( t.newx1_f,0,0,t.oldx2_f,0,0);

				//  X plane
				if (  t.oldx2_f > t.oldx1_f ) 
				{
					t.tScaleX_f = -(t.dist2x_f - t.dist1x_f);
				}
				else
				{
					t.tScaleX_f = (t.dist2x_f - t.dist1x_f);
				}

				//  Y plane
				t.dist1y_f = GetDistance( t.oldy1_f,0,0,t.oldy2_f,0,0);
				t.dist2y_f = GetDistance( t.newy1_f,0,0,t.oldy2_f,0,0);

				if (  t.oldy2_f > t.oldy1_f ) 
				{
					t.tScaleY_f = -(t.dist2y_f - t.dist1y_f);
				}
				else
				{
					t.tScaleY_f = (t.dist2y_f - t.dist1y_f);
				}

				//  Z plane
				t.dist1z_f = GetDistance( t.oldz1_f,0,0,t.oldz2_f,0,0);
				t.dist2z_f = GetDistance( t.newz1_f,0,0,t.oldz2_f,0,0);

				if (  t.oldz2_f < t.oldz1_f ) 
				{
					t.tScaleZ_f = -(t.dist2z_f - t.dist1z_f);
				}
				else
				{
					t.tScaleZ_f = (t.dist2z_f - t.dist1z_f);
				}

			}


			if (  t.importer.selectedMarker  ==  6 ) 
			{

				t.tSnapObject = t.selectedObjectMarkers[6];

				t.tMultiX_f = -1;
				t.tMultiZ_f = -1;

				t.oldx1_f = ObjectPositionX(t.selectedObjectMarkers[6]);
				t.oldy1_f = ObjectPositionY(t.selectedObjectMarkers[6]);
				t.oldz1_f = ObjectPositionZ(t.selectedObjectMarkers[6]);
				t.oldx2_f = ObjectPositionX(t.selectedObjectMarkers[5]);
				t.oldy2_f = ObjectPositionY(t.selectedObjectMarkers[8]);
				t.oldz2_f = ObjectPositionZ(t.selectedObjectMarkers[2]);

				if (  t.importer.viewMessage  ==  "Front" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[6],90 );
					MoveObject (  t.selectedObjectMarkers[6], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[6], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[6],180 );
						importer_snapLeft ( );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Top" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[6],90 );
					MoveObject (  t.selectedObjectMarkers[6], t.inputsys.xmousemove );
					TurnObjectLeft (  t.selectedObjectMarkers[6],-90 );
					MoveObject (  t.selectedObjectMarkers[6], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						TurnObjectLeft (  t.selectedObjectMarkers[6],180 );
						TurnObjectLeft (  t.selectedObjectMarkers[6],90 );
						importer_snapLeft ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Left" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[6],180 );
					MoveObject (  t.selectedObjectMarkers[6], -t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[6], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[6],180 );
						importer_snapforward ( );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Right" ) 
				{
					TurnObjectRight (  t.selectedObjectMarkers[6],180 );
					MoveObject (  t.selectedObjectMarkers[6], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[6], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[6],180 );
						importer_snapforward ( );
						importer_snapUp ( );
					}
				}

				t.newx1_f = ObjectPositionX(t.selectedObjectMarkers[6]);
				t.newy1_f = ObjectPositionY(t.selectedObjectMarkers[6]);
				t.newz1_f = ObjectPositionZ(t.selectedObjectMarkers[6]);

				t.dist1x_f = GetDistance( t.oldx1_f,0,0,t.oldx2_f,0,0);
				t.dist2x_f = GetDistance( t.newx1_f,0,0,t.oldx2_f,0,0);

				//  X plane
				if (  t.oldx2_f > t.oldx1_f ) 
				{
					t.tScaleX_f = -(t.dist2x_f - t.dist1x_f);
				}
				else
				{
					t.tScaleX_f = (t.dist2x_f - t.dist1x_f);
				}

				//  Y plane
				t.dist1y_f = GetDistance( t.oldy1_f,0,0,t.oldy2_f,0,0);
				t.dist2y_f = GetDistance( t.newy1_f,0,0,t.oldy2_f,0,0);

				if (  t.oldy2_f > t.oldy1_f ) 
				{
					t.tScaleY_f = -(t.dist2y_f - t.dist1y_f);
				}
				else
				{
					t.tScaleY_f = (t.dist2y_f - t.dist1y_f);
				}

				//  Z plane
				t.dist1z_f = GetDistance( t.oldz1_f,0,0,t.oldz2_f,0,0);
				t.dist2z_f = GetDistance( t.newz1_f,0,0,t.oldz2_f,0,0);

				if (  t.importer.viewMessage  !=  "Top" ) 
				{
					if (  t.oldz2_f > t.oldz1_f ) 
					{
						t.tScaleZ_f = (t.dist2z_f - t.dist1z_f);
					}
					else
					{
						t.tScaleZ_f = -(t.dist2z_f - t.dist1z_f);
					}
				}
				else
				{
					if (  t.oldz2_f < t.oldz1_f ) 
					{
						t.tScaleZ_f = -(t.dist2z_f - t.dist1z_f);
					}
					else
					{
						t.tScaleZ_f = (t.dist2z_f - t.dist1z_f);
					}
				}

			}

			if (  t.importer.selectedMarker  ==  7 ) 
			{

				t.tSnapObject = t.selectedObjectMarkers[7];

				t.tMultiZ_f = -1;
				t.tMultiY_f = -1;

				t.oldx1_f = ObjectPositionX(t.selectedObjectMarkers[7]);
				t.oldy1_f = ObjectPositionY(t.selectedObjectMarkers[7]);
				t.oldz1_f = ObjectPositionZ(t.selectedObjectMarkers[7]);
				t.oldx2_f = ObjectPositionX(t.selectedObjectMarkers[4]);
				t.oldy2_f = ObjectPositionY(t.selectedObjectMarkers[2]);
				t.oldz2_f = ObjectPositionZ(t.selectedObjectMarkers[1]);

				if (  t.importer.viewMessage  ==  "Front" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[7],90 );
					MoveObject (  t.selectedObjectMarkers[7], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[7], -t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[7],180 );
						importer_snapLeft ( );
						RollObjectRight (  t.selectedObjectMarkers[7],180 );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Top" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[7],90 );
					MoveObject (  t.selectedObjectMarkers[7], t.inputsys.xmousemove );
					TurnObjectLeft (  t.selectedObjectMarkers[7],-90 );
					MoveObject (  t.selectedObjectMarkers[7], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						TurnObjectLeft (  t.selectedObjectMarkers[7],180+90 );
						importer_snapLeft ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Left" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[7],180 );
					MoveObject (  t.selectedObjectMarkers[7], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[7], -t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						RollObjectRight (  t.selectedObjectMarkers[7],180 );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Right" ) 
				{
					TurnObjectRight (  t.selectedObjectMarkers[7],180 );
					MoveObject (  t.selectedObjectMarkers[7], -t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[7], -t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						RollObjectRight (  t.selectedObjectMarkers[7],180 );
						importer_snapUp ( );
					}
				}

				t.newx1_f = ObjectPositionX(t.selectedObjectMarkers[7]);
				t.newy1_f = ObjectPositionY(t.selectedObjectMarkers[7]);
				t.newz1_f = ObjectPositionZ(t.selectedObjectMarkers[7]);

				t.dist1x_f = GetDistance( t.oldx1_f,0,0,t.oldx2_f,0,0);
				t.dist2x_f = GetDistance( t.newx1_f,0,0,t.oldx2_f,0,0);

				//  X plane
				if (  t.oldx2_f < t.oldx1_f ) 
				{
					t.tScaleX_f = -(t.dist2x_f - t.dist1x_f);
				}
				else
				{
					t.tScaleX_f = (t.dist2x_f - t.dist1x_f);
				}

				//  Y plane
				t.dist1y_f = GetDistance( t.oldy1_f,0,0,t.oldy2_f,0,0);
				t.dist2y_f = GetDistance( t.newy1_f,0,0,t.oldy2_f,0,0);

				if (  t.oldy2_f > t.oldy1_f ) 
				{
					t.tScaleY_f = -(t.dist2y_f - t.dist1y_f);
				}
				else
				{
					t.tScaleY_f = (t.dist2y_f - t.dist1y_f);
				}

				//  Z plane
				t.dist1z_f = GetDistance( t.oldz1_f,0,0,t.oldz2_f,0,0);
				t.dist2z_f = GetDistance( t.newz1_f,0,0,t.oldz2_f,0,0);

				if (  t.importer.viewMessage  !=  "Top" ) 
				{
					if (  t.oldz2_f < t.oldz1_f ) 
					{
						t.tScaleZ_f = (t.dist2z_f - t.dist1z_f);
					}
					else
					{
						t.tScaleZ_f = -(t.dist2z_f - t.dist1z_f);
					}
				}
				else
				{
					if (  t.oldz2_f < t.oldz1_f ) 
					{
						t.tScaleZ_f = -(t.dist2z_f - t.dist1z_f);
					}
					else
					{
						t.tScaleZ_f = (t.dist2z_f - t.dist1z_f);
					}
				}
			}

			if (  t.importer.selectedMarker  ==  8 ) 
			{

				t.tSnapObject = t.selectedObjectMarkers[8];

				t.tMultiX_f = -1;
				t.tMultiZ_f = -1;
				t.tMultiY_f = -1;

				t.oldx1_f = ObjectPositionX(t.selectedObjectMarkers[8]);
				t.oldy1_f = ObjectPositionY(t.selectedObjectMarkers[8]);
				t.oldz1_f = ObjectPositionZ(t.selectedObjectMarkers[8]);
				t.oldx2_f = ObjectPositionX(t.selectedObjectMarkers[7]);
				t.oldy2_f = ObjectPositionY(t.selectedObjectMarkers[6]);
				t.oldz2_f = ObjectPositionZ(t.selectedObjectMarkers[1]);

				if (  t.importer.viewMessage  ==  "Front" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[8],90 );
					MoveObject (  t.selectedObjectMarkers[8], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[8], -t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[8],180 );
						importer_snapLeft ( );
						RollObjectRight (  t.selectedObjectMarkers[8],180 );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Top" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[8],90 );
					MoveObject (  t.selectedObjectMarkers[8], t.inputsys.xmousemove );
					TurnObjectLeft (  t.selectedObjectMarkers[8],-90 );
					MoveObject (  t.selectedObjectMarkers[8], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						TurnObjectLeft (  t.selectedObjectMarkers[8],-90 );
						importer_snapLeft ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Left" ) 
				{
					TurnObjectLeft (  t.selectedObjectMarkers[8],180 );
					MoveObject (  t.selectedObjectMarkers[8], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[8], -t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						RollObjectRight (  t.selectedObjectMarkers[8],180 );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Right" ) 
				{
					TurnObjectRight (  t.selectedObjectMarkers[8],180 );
					MoveObject (  t.selectedObjectMarkers[8], -t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[8], -t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						RollObjectRight (  t.selectedObjectMarkers[8],180 );
						importer_snapUp ( );
					}
				}

				t.newx1_f = ObjectPositionX(t.selectedObjectMarkers[8]);
				t.newy1_f = ObjectPositionY(t.selectedObjectMarkers[8]);
				t.newz1_f = ObjectPositionZ(t.selectedObjectMarkers[8]);

				t.dist1x_f = GetDistance( t.oldx1_f,0,0,t.oldx2_f,0,0);
				t.dist2x_f = GetDistance( t.newx1_f,0,0,t.oldx2_f,0,0);

				//  X plane
				if (  t.oldx2_f > t.oldx1_f ) 
				{
					t.tScaleX_f = -(t.dist2x_f - t.dist1x_f);
				}
				else
				{
					t.tScaleX_f = (t.dist2x_f - t.dist1x_f);
				}

				//  Y plane
				t.dist1y_f = GetDistance( t.oldy1_f,0,0,t.oldy2_f,0,0);
				t.dist2y_f = GetDistance( t.newy1_f,0,0,t.oldy2_f,0,0);

				if (  t.oldy2_f > t.oldy1_f ) 
				{
					t.tScaleY_f = -(t.dist2y_f - t.dist1y_f);
				}
				else
				{
					t.tScaleY_f = (t.dist2y_f - t.dist1y_f);
				}

				//  Z plane
				t.dist1z_f = GetDistance( t.oldz1_f,0,0,t.oldz2_f,0,0);
				t.dist2z_f = GetDistance( t.newz1_f,0,0,t.oldz2_f,0,0);

				if (  t.importer.viewMessage  !=  "Top" ) 
				{
					if (  t.oldz2_f < t.oldz1_f ) 
					{
						t.tScaleZ_f = (t.dist2z_f - t.dist1z_f);
					}
					else
					{
						t.tScaleZ_f = -(t.dist2z_f - t.dist1z_f);
					}
				}
				else
				{
					if (  t.oldz2_f < t.oldz1_f ) 
					{
						t.tScaleZ_f = -(t.dist2z_f - t.dist1z_f);
					}
					else
					{
						t.tScaleZ_f = (t.dist2z_f - t.dist1z_f);
					}
				}
			}

			if (  t.importer.selectedMarker  ==  9 ) 
			{
				t.tSnapObject = t.selectedObjectMarkers[1];

				t.oldx1_f = ObjectPositionX(t.selectedObjectMarkers[9]);
				t.oldy1_f = ObjectPositionY(t.selectedObjectMarkers[9]);
				t.oldz1_f = ObjectPositionZ(t.selectedObjectMarkers[9]);

				if (  t.importer.viewMessage  ==  "Front" ) 
				{
					RotateObject (  t.selectedObjectMarkers[9],0,0,0 );
					if (t.importer.selectedCollisionObject >= 0) {
						TurnObjectLeft(t.selectedObjectMarkers[9], ObjectAngleY(t.importerCollision[t.importer.selectedCollisionObject].object));
						PitchObjectUp(t.selectedObjectMarkers[9], -ObjectAngleX(t.importerCollision[t.importer.selectedCollisionObject].object));
						RollObjectLeft(t.selectedObjectMarkers[9], ObjectAngleZ(t.importerCollision[t.importer.selectedCollisionObject].object));
					}
					TurnObjectLeft (  t.selectedObjectMarkers[9],90 );
					MoveObject (  t.selectedObjectMarkers[9], -t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[9], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapLeft ( );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Top" ) 
				{
					if (t.importer.selectedCollisionObject >= 0) {
						RotateObject(t.selectedObjectMarkers[9], -ObjectAngleX(t.importerCollision[t.importer.selectedCollisionObject].object), ObjectAngleY(t.importerCollision[t.importer.selectedCollisionObject].object), -ObjectAngleZ(t.importerCollision[t.importer.selectedCollisionObject].object));
					}
					RotateObject (  t.selectedObjectMarkers[9],0,0,0 );
					if (t.importer.selectedCollisionObject >= 0) {
						TurnObjectLeft(t.selectedObjectMarkers[9], ObjectAngleY(t.importerCollision[t.importer.selectedCollisionObject].object));
						PitchObjectUp(t.selectedObjectMarkers[9], -ObjectAngleX(t.importerCollision[t.importer.selectedCollisionObject].object));
						RollObjectLeft(t.selectedObjectMarkers[9], ObjectAngleZ(t.importerCollision[t.importer.selectedCollisionObject].object));
					}
					TurnObjectLeft (  t.selectedObjectMarkers[9],90 );
					MoveObject (  t.selectedObjectMarkers[9], -t.inputsys.xmousemove );
					TurnObjectLeft (  t.selectedObjectMarkers[9],-90 );
					MoveObject (  t.selectedObjectMarkers[9], -t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						importer_snapforward ( );
						TurnObjectLeft (  t.selectedObjectMarkers[9],90 );
						importer_snapLeft ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Left" ) 
				{
					RotateObject (  t.selectedObjectMarkers[9],0,0,0 );
					if (t.importer.selectedCollisionObject >= 0) {
						TurnObjectLeft(t.selectedObjectMarkers[9], ObjectAngleY(t.importerCollision[t.importer.selectedCollisionObject].object));
						PitchObjectUp(t.selectedObjectMarkers[9], -ObjectAngleX(t.importerCollision[t.importer.selectedCollisionObject].object));
						RollObjectLeft(t.selectedObjectMarkers[9], ObjectAngleZ(t.importerCollision[t.importer.selectedCollisionObject].object));
					}
					TurnObjectLeft (  t.selectedObjectMarkers[9],180 );
					MoveObject (  t.selectedObjectMarkers[9], t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[9], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[9],180 );
						importer_snapforward ( );
						importer_snapUp ( );
					}
				}
				if (  t.importer.viewMessage  ==  "Right" ) 
				{
					RotateObject (  t.selectedObjectMarkers[9],0,0,0 );
					if (t.importer.selectedCollisionObject >= 0) {
						TurnObjectLeft(t.selectedObjectMarkers[9], ObjectAngleY(t.importerCollision[t.importer.selectedCollisionObject].object));
						PitchObjectUp(t.selectedObjectMarkers[9], -ObjectAngleX(t.importerCollision[t.importer.selectedCollisionObject].object));
						RollObjectLeft(t.selectedObjectMarkers[9], ObjectAngleZ(t.importerCollision[t.importer.selectedCollisionObject].object));
					}
					TurnObjectRight (  t.selectedObjectMarkers[9],180 );
					MoveObject (  t.selectedObjectMarkers[9], -t.inputsys.xmousemove );
					MoveObjectUp (  t.selectedObjectMarkers[9], t.inputsys.ymousemove );
					if (  t.inputsys.keyshift ) 
					{
						TurnObjectLeft (  t.selectedObjectMarkers[9],180 );
						importer_snapforward ( );
						importer_snapUp ( );
					}
				}

				t.newx1_f = ObjectPositionX(t.selectedObjectMarkers[9]);
				t.newy1_f = ObjectPositionY(t.selectedObjectMarkers[9]);
				t.newz1_f = ObjectPositionZ(t.selectedObjectMarkers[9]);

				t.tmoveX_f = t.newx1_f - t.oldx1_f;
				t.tmoveY_f = t.newy1_f - t.oldy1_f;
				t.tmoveZ_f = t.newz1_f - t.oldz1_f;
	
				if (  t.tXOnly  ==  0  )  t.tmoveX_f  =  0;
				if (  t.tYOnly  ==  0  )  t.tmoveY_f  =  0;
				if (  t.tZOnly  ==  0  )  t.tmoveZ_f  =  0;
	
				if (t.importer.selectedCollisionObject >= 0) {
					MoveObjectRight(t.importerCollision[t.importer.selectedCollisionObject].object, t.tmoveX_f);
					MoveObjectDown(t.importerCollision[t.importer.selectedCollisionObject].object, t.tmoveY_f);
					MoveObject(t.importerCollision[t.importer.selectedCollisionObject].object, t.tmoveZ_f);
					ShowObject(t.importerCollision[t.importer.selectedCollisionObject].object);
					///GhostObjectOn (   t.importerCollision[t.importer.selectedCollisionObject].object,2 );
					SetObjectLight(t.importerCollision[t.importer.selectedCollisionObject].object, 1);
					ColorObject(t.importerCollision[t.importer.selectedCollisionObject].object, Rgb(0, 0, 0));
					SetObjectAmbience(t.importerCollision[t.importer.selectedCollisionObject].object, 0);
					SetObjectEmissive(t.importerCollision[t.importer.selectedCollisionObject].object, Rgb(100, 100, 0));

					MoveObjectRight(t.importerCollision[t.importer.selectedCollisionObject].object2, t.tmoveX_f);
					MoveObjectDown(t.importerCollision[t.importer.selectedCollisionObject].object2, t.tmoveY_f);
					MoveObject(t.importerCollision[t.importer.selectedCollisionObject].object2, t.tmoveZ_f);
				}
				return;
			}

			//  If any axis are not locked allow movement
			if (  t.tXOnly  ==  0  )  t.tScaleX_f  =  0;
			if (  t.tYOnly  ==  0  )  t.tScaleY_f  =  0;
			if (  t.tZOnly  ==  0  )  t.tScaleZ_f  =  0;

			//  stop the boxes becomes too small or inverted
			if (t.importer.selectedCollisionObject >= 0) {
				if (LimbScaleX(t.importerCollision[t.importer.selectedCollisionObject].object2, 0) - t.tScaleX_f < 5)
				{
					t.tScaleX_f = -(5.0 - LimbScaleX(t.importerCollision[t.importer.selectedCollisionObject].object2, 0));
				}
				if (LimbScaleY(t.importerCollision[t.importer.selectedCollisionObject].object2, 0) - t.tScaleY_f < 5)
				{
					t.tScaleY_f = -(5.0 - LimbScaleY(t.importerCollision[t.importer.selectedCollisionObject].object2, 0));
				}
				if (LimbScaleZ(t.importerCollision[t.importer.selectedCollisionObject].object2, 0) + t.tScaleZ_f < 5)
				{
					t.tScaleZ_f = (5.0 - LimbScaleZ(t.importerCollision[t.importer.selectedCollisionObject].object2, 0));
				}

				ScaleLimb(t.importerCollision[t.importer.selectedCollisionObject].object, 0, LimbScaleX(t.importerCollision[t.importer.selectedCollisionObject].object2, 0) - t.tScaleX_f, LimbScaleY(t.importerCollision[t.importer.selectedCollisionObject].object2, 0) - t.tScaleY_f, LimbScaleZ(t.importerCollision[t.importer.selectedCollisionObject].object2, 0) + t.tScaleZ_f);
				t.importerCollision[t.importer.selectedCollisionObject].sizex = LimbScaleX(t.importerCollision[t.importer.selectedCollisionObject].object, 0) - t.tScaleX_f;
				t.importerCollision[t.importer.selectedCollisionObject].sizey = LimbScaleY(t.importerCollision[t.importer.selectedCollisionObject].object, 0) - t.tScaleY_f;
				t.importerCollision[t.importer.selectedCollisionObject].sizez = LimbScaleZ(t.importerCollision[t.importer.selectedCollisionObject].object, 0) + t.tScaleZ_f;
				MoveObjectRight(t.importerCollision[t.importer.selectedCollisionObject].object, t.tScaleX_f * t.tMultiX_f / 2.0);
				MoveObjectDown(t.importerCollision[t.importer.selectedCollisionObject].object, t.tScaleY_f * t.tMultiY_f / 2.0);
				MoveObject(t.importerCollision[t.importer.selectedCollisionObject].object, -t.tScaleZ_f * t.tMultiZ_f / 2.0);
				ShowObject(t.importerCollision[t.importer.selectedCollisionObject].object);

				///GhostObjectOn (   t.importerCollision[t.importer.selectedCollisionObject].object,2 );

				SetObjectLight(t.importerCollision[t.importer.selectedCollisionObject].object, 1);
				ColorObject(t.importerCollision[t.importer.selectedCollisionObject].object, Rgb(0, 0, 0));
				SetObjectAmbience(t.importerCollision[t.importer.selectedCollisionObject].object, 0);
				SetObjectEmissive(t.importerCollision[t.importer.selectedCollisionObject].object, Rgb(100, 100, 0));

				ScaleLimb(t.importerCollision[t.importer.selectedCollisionObject].object2, 0, LimbScaleX(t.importerCollision[t.importer.selectedCollisionObject].object2, 0) - t.tScaleX_f, LimbScaleY(t.importerCollision[t.importer.selectedCollisionObject].object2, 0) - t.tScaleY_f, LimbScaleZ(t.importerCollision[t.importer.selectedCollisionObject].object2, 0) + t.tScaleZ_f);
				MoveObjectRight(t.importerCollision[t.importer.selectedCollisionObject].object2, t.tScaleX_f * t.tMultiX_f / 2.0);
				MoveObjectDown(t.importerCollision[t.importer.selectedCollisionObject].object2, t.tScaleY_f * t.tMultiY_f / 2.0);
				MoveObject(t.importerCollision[t.importer.selectedCollisionObject].object2, -t.tScaleZ_f * t.tMultiZ_f / 2.0);
			}
		}
		else
		{

			//  Mouse released, come out of editing
			importer_show_mouse ( );
			t.timporterpickdepth_f = 1250;
			if (t.importer.selectedCollisionObject >= 0) {
				HideObject(t.importerCollision[t.importer.selectedCollisionObject].object);
			}
			for ( tCount = 1 ; tCount<=  9; tCount++ )
			{
				t.picked = PickScreenObjectEx(t.importer.MouseX, t.importer.MouseY, t.selectedObjectMarkers[tCount], t.selectedObjectMarkers[tCount],1,0);
				ScaleObject (  t.selectedObjectMarkers[tCount],100,100,100 );
				SetObjectAmbience (  t.selectedObjectMarkers[tCount],0 );
				if (  tCount  !=  9 ) 
				{
					SetObjectEmissive (  t.selectedObjectMarkers[tCount], Rgb(255,255,0) );
				}
				else
				{
					SetObjectEmissive (  t.selectedObjectMarkers[tCount], Rgb(0,255,255) );
				}

			}
			t.importer.selectedMarker = 0;
			t.importer.collisionObjectMode = 0;
		}
	}
}

void importer_update_textures ( void )
{
	// count how many images are specified in image list
	int iImageCount = 0;
	for ( int tCount = 1 ; tCount <= IMPORTERTEXTURESMAX; tCount++ )
		if ( strlen ( t.importerTextures[tCount].fileName.Get() ) > 0 )
			iImageCount++;

	// show all textures associated with this model import (so user can change textures used)
	strcpy ( g_pShowFilenameHoveringOver, "" );
	for ( int tCount = 1 ; tCount <= IMPORTERTEXTURESMAX; tCount++ )
	{
		// skip image slots that have no filename
		if ( strlen ( t.importerTextures[tCount].fileName.Get() ) == 0 )
			continue;

		// determine what texture is shown in UI (if any)
		int iTexSlotImage = t.importerTextures[tCount].imageID;

		// work out texture panel position
		t.tOffsetX = 10;
		if (  tCount > 5  )  t.tOffsetX  =  10+128;
		if (  tCount > 10  )  t.tOffsetX  =  10+(128*2);
		if (  tCount > 15  )  t.tOffsetX  =  10+(128*3);
		if (  tCount > 20  )  t.tOffsetX  =  10+(128*4);
		//if (  t.importer.scaleMulti  !=  1.0  )  t.tOffsetX  =  0;
		t.tOffsetY = tCount;
		if (  tCount > 5  )  t.tOffsetY  =  tCount-5;
		if (  tCount > 10  )  t.tOffsetY  =  tCount-10;
		if (  tCount > 15  )  t.tOffsetY  =  tCount-15;
		if (  tCount > 20  )  t.tOffsetY  =  tCount-20;
		int iVertical = (t.tOffsetY-1);
		t.tOffsetY = 10 + (iVertical * 128);

		if (!bRemoveSprites) {
			if (t.importer.scaleMulti != 1.0)
				Sprite(t.importerTextures[tCount].spriteID2, t.tOffsetX, (GetChildWindowHeight() / 2) - 400 + t.tOffsetY - 19 + 20, g.importermenuimageoffset + 7);
			else
				Sprite(t.importerTextures[tCount].spriteID2, (GetChildWindowWidth() / 2) - 430 - t.tOffsetX - 19 - 20, (GetChildWindowHeight() / 2) - 400 + t.tOffsetY - 19 + 20, g.importermenuimageoffset + 7);
			SizeSprite(t.importerTextures[tCount].spriteID2, 128, 128);

			if (iTexSlotImage > 0)
			{
				if (t.importer.scaleMulti != 1.0)
					Sprite(t.importerTextures[tCount].spriteID, t.tOffsetX + 20, (GetChildWindowHeight() / 2) - 400 + t.tOffsetY + 20, iTexSlotImage);
				else
					Sprite(t.importerTextures[tCount].spriteID, (GetChildWindowWidth() / 2) - 430 - t.tOffsetX - 20, (GetChildWindowHeight() / 2) - 400 + t.tOffsetY + 20, iTexSlotImage);
				SizeSprite(t.importerTextures[tCount].spriteID, 90, 90);
				SetSpritePriority(t.importerTextures[tCount].spriteID, 1);
			}
			else
			{
				Sprite(t.importerTextures[tCount].spriteID, -10000, -10000, g.importermenuimageoffset + 7);
			}

			if (t.importer.MouseX >= SpriteX(t.importerTextures[tCount].spriteID2) - 5 && t.importer.MouseY >= (GetChildWindowHeight() / 2) - 400 + (iVertical * 128) + 5)
			{
				if (t.importer.MouseX <= SpriteX(t.importerTextures[tCount].spriteID2) + 128 - 5 && t.importer.MouseY <= 90 + (GetChildWindowHeight() / 2) - 400 + (iVertical * 128) + 30)
				{
					strcpy(g_pShowFilenameHoveringOver, t.importerTextures[tCount].fileName.Get());
					if (t.inputsys.mclick == 0)
					{
						if (t.importer.scaleMulti != 1.0)
							Sprite(t.importerTextures[tCount].spriteID2, t.tOffsetX, (GetChildWindowHeight() / 2) - 400 + t.tOffsetY - 19 + 20, g.importermenuimageoffset + 7);
						else
							Sprite(t.importerTextures[tCount].spriteID2, (GetChildWindowWidth() / 2) - 430 - t.tOffsetX - 19 - 20, (GetChildWindowHeight() / 2) - 400 + t.tOffsetY - 19 + 20, g.importermenuimageoffset + 7);
						SizeSprite(t.importerTextures[tCount].spriteID2, 128, 128);
						SizeSprite(t.importerTextures[tCount].spriteID, 106, 106);
						if (t.importer.scaleMulti == 1.0)
						{
							Sprite(t.importerTextures[tCount].spriteID, (GetChildWindowWidth() / 2) - 430 - 8 - 20, (GetChildWindowHeight() / 2) - 400 + tCount * 128 - 8 + 20, iTexSlotImage);
						}
						else
						{
							if (iTexSlotImage > 0)
							{
								Sprite(t.importerTextures[tCount].spriteID, t.tOffsetX + 10, (GetChildWindowHeight() / 2) - 400 + (iVertical * 128) - 8 + 20, iTexSlotImage);
							}
						}
						SetSpritePriority(t.importerTextures[tCount].spriteID, 1);
					}
					else
					{
						if (t.importer.oldMouseClick == 0)
						{
							t.tFileName_s = openFileBox("PNG|*.png|DDS|*.dds|JPEG|*.jpg|BMP|*.bmp|All Files|*.*|", "", "Open Texture", ".dds", IMPORTEROPENFILE);
							if (t.tFileName_s == "Error")  return;
							if (FileExist(t.tFileName_s.Get()) == 1)
							{
								// prompt as this may take some seconds
								LPSTR pDelayPrompt = "Loading chosen texture and associated files";
								for (int iSyncPass = 0; iSyncPass < 2; iSyncPass++)
								{
									pastebitmapfont(pDelayPrompt, (GetChildWindowWidth() / 2) - (getbitmapfontwidth(pDelayPrompt, 1) / 2), 860, 1, 255);
									Sync();
								}

								// find free image
								t.tImageID = t.importerTextures[tCount].imageID;
								if (t.tImageID == 0)
								{
									t.tImageID = g.importermenuimageoffset + 15;
									while (ImageExist(t.tImageID) == 1) ++t.tImageID;
								}

								// can expand out a color texture once (to add normal/gloss/etc)
								bool bExpandOutPBRTextureSet = false;

								// replace image details
								if (ImageExist(t.tImageID) == 1) DeleteImage(t.tImageID);
								LoadImage(t.tFileName_s.Get(), t.tImageID);
								if (ImageExist(t.tImageID) == 1)
								{
									// remove any previous references to associated files for the old filename
									if (t.importerTextures[tCount].iExpandedThisSlot == 0)
									{
										// but only if its a base color texutre
										char pIsItTexColor[2048];
										strcpy(pIsItTexColor, t.importerTextures[tCount].fileName.Get());
										if (strlen(pIsItTexColor) > 1 + 8 + 4)
										{
											pIsItTexColor[strlen(pIsItTexColor) - 4] = 0;
											if (strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 2, "_d", 2) == NULL
												|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 6, "_color", 6) == NULL
												|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 8, "_diffuse", 8) == NULL
												|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 7, "_albedo", 7) == NULL
												|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 8, "blankTex", 8) == NULL)
											{
												// for both!
												strcpy(pIsItTexColor, t.tFileName_s.Get());
												if (strlen(pIsItTexColor) > 1 + 8 + 4)
												{
													pIsItTexColor[strlen(pIsItTexColor) - 4] = 0;
													if (strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 2, "_d", 2) == NULL
														|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 6, "_color", 6) == NULL
														|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 8, "_diffuse", 8) == NULL
														|| strnicmp(pIsItTexColor + strlen(pIsItTexColor) - 7, "_albedo", 7) == NULL)
													{
														importer_removeentryandassociatesof(tCount);
														t.importerTextures[tCount].iExpandedThisSlot = 1;
														bExpandOutPBRTextureSet = true;
													}
												}
											}
										}
									}

									// update image list data
									t.importerTextures[tCount].fileName = t.tFileName_s;
									t.importerTextures[tCount].imageID = t.tImageID;
								}

								// ensure single texture is specified in FPE
								if (iImageCount == 1)
								{
									t.importer.objectFPE.textured = t.tFileName_s;
								}

								// reapply texture to model
								importer_applyimagelisttextures(false, tCount, bExpandOutPBRTextureSet);
								importer_recreate_texturesprites();
							}
						}
					}
				}
			}
		}
	}
	SetDir (  t.importer.startDir.Get() );

	/* old pre-180617
	// we only use one texture slot (for single texture or shows multi-tex image FOR NOW)
	// for ( int tCount = 1 ; tCount<=  10; tCount++ )
	int tCount = 1;

	// determine what texture is shown in UI
	int iTexSlotImage = t.importerTextures[tCount].imageID;
	if ( iTexSlotImage == 0  ) return;

	// is this a multi textures
	if ( t.importerTextures[2].imageID != 0 )
		iTexSlotImage = g.importermenuimageoffset+10;

	// is image valid
	if ( ImageExist (iTexSlotImage) == 0 )  
		return;

	t.tOffsetX = 0;
	if (  tCount > 5  )  t.tOffsetX  =  128;
	if (  t.importer.scaleMulti  !=  1.0  )  t.tOffsetX  =  0;
	t.tOffsetY = tCount;
	if (  tCount > 5  )  t.tOffsetY  =  tCount-5;
	t.tOffsetY = t.tOffsetY * 128;

	if (  t.importer.scaleMulti  !=  1.0 ) 
	{
		Sprite (  t.importerTextures[tCount].spriteID2 , 0, (GetChildWindowHeight()/2) - 400 + t.tOffsetY-19+20 , g.importermenuimageoffset+7 );
	}
	else
	{
		Sprite (  t.importerTextures[tCount].spriteID2 , (GetChildWindowWidth()/2) - 430 - t.tOffsetX -19 -20, (GetChildWindowHeight()/2) - 400 + t.tOffsetY-19+20 , g.importermenuimageoffset+7 );
	}
	SizeSprite (  t.importerTextures[tCount].spriteID2 , 128 , 128 );

	if (  t.importer.scaleMulti  !=  1.0 ) 
	{
		Sprite (  t.importerTextures[tCount].spriteID , 20 , (GetChildWindowHeight()/2) - 400 + t.tOffsetY+20 , iTexSlotImage );
	}
	else
	{
		Sprite (  t.importerTextures[tCount].spriteID , (GetChildWindowWidth()/2) - 430 - t.tOffsetX-20 , (GetChildWindowHeight()/2) - 400 + t.tOffsetY+20 , iTexSlotImage );
	}
	SizeSprite (  t.importerTextures[tCount].spriteID , 90 , 90 );
	SetSpritePriority (  t.importerTextures[tCount].spriteID, 1 );

	if (  t.importer.MouseX  >=  SpriteX (t.importerTextures[tCount].spriteID2) -20 && t.importer.MouseY >=  (GetChildWindowHeight()/2) - 400 + tCount * 128+20 )
	{
		if (  t.importer.MouseX  <=  SpriteX (t.importerTextures[tCount].spriteID2) +128+20 && t.importer.MouseY <=  90+(GetChildWindowHeight()/2) - 400 + tCount * 128+20 ) 
		{
			if (  t.inputsys.mclick  ==  0 ) 
			{
				if (  t.importer.scaleMulti  !=  1.0 ) 
				{
					Sprite (  t.importerTextures[tCount].spriteID2 , 0 , (GetChildWindowHeight()/2) - 400 + t.tOffsetY-19 + 20 , g.importermenuimageoffset+7 );
				}
				else
				{
					Sprite (  t.importerTextures[tCount].spriteID2 , (GetChildWindowWidth()/2) - 430 - t.tOffsetX -19 - 20, (GetChildWindowHeight()/2) - 400 + t.tOffsetY-19 + 20 , g.importermenuimageoffset+7 );
				}
				SizeSprite (  t.importerTextures[tCount].spriteID2 , 128 , 128 );
				SizeSprite (  t.importerTextures[tCount].spriteID , 106 , 106 );
				if (  t.importer.scaleMulti  ==  1.0 ) 
				{
					Sprite (  t.importerTextures[tCount].spriteID , (GetChildWindowWidth()/2) - 430 -8-20 , (GetChildWindowHeight()/2) - 400 + tCount * 128 - 8+20 , iTexSlotImage );
				}
				else
				{
					Sprite (  t.importerTextures[tCount].spriteID , 10 , (GetChildWindowHeight()/2) - 400 + tCount * 128 - 8+20 , iTexSlotImage );
				}
				SetSpritePriority (  t.importerTextures[tCount].spriteID, 1 );
			}
			else
			{
				if (  t.importer.oldMouseClick  ==  0 ) 
				{
					t.tFileName_s = openFileBox("PNG|*.png|DDS|*.dds|JPEG|*.jpg|BMP|*.bmp|All Files|*.*|", "", "Open Texture", ".dds", IMPORTEROPENFILE);
					if (  t.tFileName_s  ==  "Error"  )  return;
					if (  FileExist ( t.tFileName_s.Get() )  ==  1 )  
					{
						// prompt as this may take some seconds
						LPSTR pDelayPrompt = "Loading chosen texture and associated files";
						for ( int iSyncPass=0; iSyncPass<2; iSyncPass++ )
						{
							pastebitmapfont(pDelayPrompt,(GetChildWindowWidth()/2) - (getbitmapfontwidth (pDelayPrompt,1)/2),860,1,255);
							Sync();
						}

						// clear image list of old images
						for ( tCount = 2 ; tCount <= IMPORTERTEXTURESMAX; tCount++ )
						{
							t.importerTextures[tCount].fileName = "";
							t.importerTextures[tCount].imageID = 0;
						}

						// start again at image slot one
						tCount = 1;

						// first reset sprite so new image (same ID) can take hold
						Sprite ( t.importerTextures[tCount].spriteID, -99999, -99999, g.importermenuimageoffset+10 );

						// replace image details
						if ( ImageExist ( t.importerTextures[tCount].imageID ) ==1 ) DeleteImage ( t.importerTextures[tCount].imageID );
						LoadImage ( t.tFileName_s.Get(), t.importerTextures[tCount].imageID );
						t.importerTextures[tCount].fileName = t.tFileName_s;

						// ensure single texture is specified in FPE
						t.importer.objectFPE.textured = t.tFileName_s;

						// reapply texture to model
						importer_applyimagelisttextures(true);
					}
				}
			}
		}
	}
	SetDir (  t.importer.startDir.Get() );
	*/
}

void importer_load_textures_finish ( int tCount, bool bCubeMapOnly )
{
	// final image load count, load textures and sort txture button sprite
	t.tcounttextures = tCount;

	// Load textures
	for ( tCount = 1; tCount <= t.tcounttextures; tCount++ )
	{
		t.tImageID = g.importermenuimageoffset+15;
		while ( ImageExist(t.tImageID) == 1 ) ++t.tImageID;
		LoadImage ( t.importerTextures[tCount].fileName.Get(), t.tImageID );
		if ( ImageExist ( t.tImageID ) == 1 )
			t.importerTextures[tCount].imageID = t.tImageID;
		else
			t.importerTextures[tCount].imageID = 0;
	}
	
	// Apply textures to model
	importer_applyimagelisttextures ( bCubeMapOnly, -1, true );
	importer_recreate_texturesprites();
}

void importer_load_textures ( void )
{
	// Clean importer Textures List
	int tCount = 0;
	for ( tCount = 1 ; tCount <= IMPORTERTEXTURESMAX; tCount++ )
	{
		t.importerTextures[tCount].imageID = 0;
		t.importerTextures[tCount].fileName = "";
		t.importerTextures[tCount].iExpandedThisSlot = 0;		
		t.importerTextures[tCount].iOptionalStage = 0;
		t.importerTextures[tCount].iAssociatedBaseImage = 0;
	}

	// Byte scan of file - good for TXT or FBX based model files
	t.filesize = FileSize(t.timporterfile_s.Get());
	t.mbi=255;
	OpenToRead (  11,t.timporterfile_s.Get() );
	if (FileOpen(11) == 1)
	{
		MakeMemblockFromFile(t.mbi, 11);
		CloseFile(11);
		tCount = 0;
		t.leavetime = Timer();
		for (t.b = 0; t.b <= t.filesize - 5; t.b++)
		{
			//  JPG, PNG, DDS, BMP, TGA
			t.tokay = 0;
			if (ReadMemblockByte(t.mbi, t.b + 0) == Asc("."))
			{
				if (ReadMemblockByte(t.mbi, t.b + 1) == Asc("j"))
				{
					if (ReadMemblockByte(t.mbi, t.b + 2) == Asc("p"))
					{
						if (ReadMemblockByte(t.mbi, t.b + 3) == Asc("g"))
						{
							t.tokay = 1;
						}
					}
				}
				if (ReadMemblockByte(t.mbi, t.b + 1) == Asc("p"))
				{
					if (ReadMemblockByte(t.mbi, t.b + 2) == Asc("n"))
					{
						if (ReadMemblockByte(t.mbi, t.b + 3) == Asc("g"))
						{
							t.tokay = 1;
						}
					}
				}
				if (ReadMemblockByte(t.mbi, t.b + 1) == Asc("d"))
				{
					if (ReadMemblockByte(t.mbi, t.b + 2) == Asc("d"))
					{
						if (ReadMemblockByte(t.mbi, t.b + 3) == Asc("s"))
						{
							t.tokay = 1;
						}
					}
				}
				if (ReadMemblockByte(t.mbi, t.b + 1) == Asc("t"))
				{
					if (ReadMemblockByte(t.mbi, t.b + 2) == Asc("g"))
					{
						if (ReadMemblockByte(t.mbi, t.b + 3) == Asc("a"))
						{
							t.tokay = 1;
						}
					}
				}
				if (ReadMemblockByte(t.mbi, t.b + 1) == Asc("b"))
				{
					if (ReadMemblockByte(t.mbi, t.b + 2) == Asc("m"))
					{
						if (ReadMemblockByte(t.mbi, t.b + 3) == Asc("p"))
						{
							t.tokay = 1;
						}
					}
				}
			}
			if (t.tokay == 1)
			{
				// determine name of external texture file
				t.c = t.b;
				while (t.c > 0)
				{
					--t.c; if (t.c <= 0 || ReadMemblockByte(t.mbi, t.c) < 32 || ReadMemblockByte(t.mbi, t.c) > Asc("z"))  break;
				}
				++t.c;
				t.tthisfile_s = "";
				while (t.c <= t.b + 3)
				{
					t.tthisfile_s = t.tthisfile_s + Chr(ReadMemblockByte(t.mbi, t.c)); ++t.c;
				}
				t.b = t.b + 3;

				//  found texture specified in imported model
				t.tSourceName_s = t.tthisfile_s;

				//  check if texture file exists alonside model file
				char pFindFile[512];
				t.tFileName_s = t.importer.objectFileOriginalPath + t.tSourceName_s;
				strcpy(pFindFile, t.tFileName_s.Get());
				if (FileExist(pFindFile) == 0)
				{
					// if not, try just the filename itself (no path)
					int iFoundFilename = -1;
					for (int n = strlen(pFindFile) - 1; n > 0; n--)
					{
						if (pFindFile[n] == '\\' || pFindFile[n] == '/')
						{
							iFoundFilename = n;
							break;
						}
					}
					if (iFoundFilename != -1)
					{
						char pFilenameOnly[512];
						strcpy(pFilenameOnly, pFindFile + iFoundFilename + 1);
						strcpy(pFindFile, t.importer.objectFileOriginalPath.Get());
						strcat(pFindFile, pFilenameOnly);
					}
				}
				t.tFileName_s = pFindFile;

				//  ensure file is not a path or contains invalid characters
				if (strcmp(Right(t.tFileName_s.Get(), 1), "/") == 0 || strcmp(Right(t.tFileName_s.Get(), 1), "\\") == 0)  t.tFileName_s = "";
				for (t.fc = 1; t.fc <= Len(t.tFileName_s.Get()); t.fc++)
				{
					if (Asc(Mid(t.tFileName_s.Get(), t.fc)) < 32 || Asc(Mid(t.tFileName_s.Get(), t.fc)) > 126)
					{
						t.tFileName_s = "";
						break;
					}
				}

				// Add found texture to the importer texture list (if exists)
				if (FileExist(t.tFileName_s.Get()) == 1)
				{
					importer_addtexturefiletolist(t.tFileName_s, t.tSourceName_s, &tCount);
				}
			}
		}
		DeleteMemblock(t.mbi);
	}

	// If nothing in file byte scan, lets see if the FPE had a texture
	t.tFoundInFPE = 0;
	if ( tCount == 0 ) 
	{
		// get texture from FPE file
		if (  t.importer.objectFPE.textured  !=  "" ) 
		{
			t.tSourceName_s = t.importer.objectFPE.textured;
			if (  FileExist ( t.tSourceName_s.Get() ) == 1 )  
			{
				t.tFoundInFPE = 1;
				t.tFileName_s = t.tSourceName_s;
			}
			if (  t.tFoundInFPE  ==  0 ) 
			{
				t.tFileName_s = t.importer.objectFileOriginalPath + t.tSourceName_s;
				if (  FileExist ( t.tFileName_s.Get() ) == 1 )  
				{
					t.tFoundInFPE = 1;
				}
			}
		}

		// see if the FPE texture has a relative path
		if ( t.tFoundInFPE == 0 ) 
		{
			t.strwork = "" ; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\"+t.importer.objectFPE.textured;
			if (  FileExist( t.strwork.Get() ) == 1 ) 
			{
				t.tSourceName_s=""; t.tSourceName_s = t.tSourceName_s+g.fpscrootdir_s + "\\Files\\"+t.importer.objectFPE.textured;
				t.tFoundInFPE = 1;
				t.tFileName_s = ""; t.tFileName_s = t.tFileName_s + g.fpscrootdir_s + "\\Files\\"+t.importer.objectFPE.textured;
			}
		}

		// if FPE did have valid texture, add to list
		if ( t.tFoundInFPE == 1 ) 
		{
			importer_addtexturefiletolist ( t.tFileName_s, t.tSourceName_s, &tCount );
		}
	}

	// if above scans/fpe found no texture file(s), go through limb texture names (in case we're dealing with binary X file)
	if ( tCount == 0 ) 
	{
		SetObjectEffect (  t.importer.objectnumber,0 );
		//CloneMeshToNewFormat (  t.importer.objectnumber,530,1 ); // 220618 - for some reason importer adding second UV layer!
		PerformCheckListForLimbs (  t.importer.objectnumber );
		for ( t.tCount9 = 1 ; t.tCount9 <= ChecklistQuantity()-1; t.tCount9++ )
		{
			t.tlimbname_s = "";
			if (LimbExist(t.importer.objectnumber, t.tCount9))
			{
				LPSTR sTmp = LimbTextureName(t.importer.objectnumber, t.tCount9);
				t.tlimbname_s = sTmp;
				if (sTmp) delete[] sTmp;
			}
			if (  t.tlimbname_s != "" ) 
			{
				t.tSourceName_s=t.tlimbname_s;
				t.tFileName_s=t.importer.objectFileOriginalPath+t.tSourceName_s;
				if (  FileExist(t.tFileName_s.Get()) == 0 ) 
				{
					t.tSourceName_s = ""; t.tSourceName_s=t.tSourceName_s+Left(t.tlimbname_s.Get(),Len(t.tlimbname_s.Get())-4)+".dds";
					t.tFileName_s=t.importer.objectFileOriginalPath+t.tSourceName_s;
				}
				if (  FileExist(t.tFileName_s.Get()) == 1 ) 
				{
					importer_addtexturefiletolist ( t.tFileName_s, t.tSourceName_s, &tCount );
				}
			}
		}
	}

	// if still no images, revert to default blank texture placeholder
	if ( tCount == 0 ) 
	{
		// if no actual texture file or FPE texture valid, use blank texture
		#ifdef VRTECH
		#else
		t.tSourceName_s = "languagebank\\neutral\\gamecore\\huds\\importer\\blankTex.png";
		#endif
		importer_addtexturefiletolist ( t.tSourceName_s, t.tSourceName_s, &tCount );
	}

	// final image load count, load textures and sort txture button sprite
	importer_load_textures_finish ( tCount, false );
}

void importer_load_fpe ( void )
{
	//  reset physics shape count
	t.importer.isProtected = 0;
	t.importer.collisionShapeCount = 0;

	//  Split the filename into tokens to grab the path, object name and create fpe name
	Dim (  t.tArray,300 );
	Dim (  t.tUnknown,300 );

	t.importer.unknownFPELineCount = 0;

	//  Reset FPE data to default before attempting to load
	t.importer.objectFPE.desc = Left(t.importer.objectFilenameFPE.Get(),Len(t.importer.objectFilenameFPE.Get())-4);

	//  ;visualinfo
	t.importer.objectFPE.textured = "";
	if (LimbExist(t.importer.objectnumber, 1) == 1)
	{
		LPSTR sTmp = LimbTextureName(t.importer.objectnumber, 1);
		t.importer.objectFPE.textured = LimbTextureName(t.importer.objectnumber, 1);
		if (sTmp) delete[] sTmp;
	}
	t.importer.objectFPE.effect = "effectbank\\reloaded\\entity_basic.fx";
	t.importer.objectFPE.castshadow = "0";
	t.importer.objectFPE.transparency = "0";
	//  ;orientation
	t.importer.objectFPE.model = t.importer.objectFilename;
	t.importer.objectFPE.offx = "0";
	t.importer.objectFPE.offy = "0";
	t.importer.objectFPE.offz = "0";
	t.importer.objectFPE.rotx = "0";
	t.importer.objectFPE.roty = "0";
	t.importer.objectFPE.rotz = "0";
	t.importer.objectFPE.scale = "100";
	t.importer.objectFPE.collisionmode = "0";
	t.importer.objectFPE.defaultstatic = "1";
	t.importer.objectFPE.materialindex = "0";
	t.importer.objectFPE.matrixmode = "0";
	t.importer.objectFPE.cullmode = "0";
	//  ;identity details
	t.importer.objectFPE.ischaracter = "0";
	t.importer.objectFPE.hasweapon = "";
	t.importer.objectFPE.isobjective = "0";
	t.importer.objectFPE.cantakeweapon = "0";
	//  ;statistics
	t.importer.objectFPE.strength = "25";
	t.importer.objectFPE.explodable= "0";
	t.importer.objectFPE.debrisshape = "0";
	//  ;ai
	t.importer.objectFPE.aimain = "default.lua";
	//  ;spawn
	t.importer.objectFPE.spawnmax = "0";
	t.importer.objectFPE.spawndelay = "0";
	t.importer.objectFPE.spawnqty = "0";
	//  ;anim
	t.importer.objectFPE.animmax = "0";
	t.importer.objectFPE.animspeed = "100";
	t.tTotalFrames_s = "0";
	t.importer.objectFPE.anim0 = ""; t.importer.objectFPE.anim0 = t.importer.objectFPE.anim0+"0," + t.tTotalFrames_s;
	t.importer.objectFPE.playanimineditor = "0";
	t.importer.objectFPE.ignorecsirefs = "1";

	//  Check if an FPE exists, if so load it in
	if (  FileOpen(1) ) CloseFile (1);
	t.strwork = ""; t.strwork = t.strwork + t.importer.objectFileOriginalPath + t.importer.objectFilenameFPE;
	if (  FileExist ( t.strwork.Get() ) )  
	{
		t.strwork = ""; t.strwork = t.strwork +t.importer.objectFileOriginalPath + t.importer.objectFilenameFPE;
		OpenToRead (  1 , t.strwork.Get() );
		while (  FileEnd(1)  ==  0 ) 
		{
			t.tstring_s = ReadString (  1 );
			t.tempLine_s = t.tstring_s;

			t.tArrayMarker = 0;
			t.tToken_s=FirstToken(t.tstring_s.Get()," ");
			if (  t.tToken_s  !=  "" ) 
			{
				t.tArray[t.tArrayMarker] = t.tToken_s;
				++t.tArrayMarker;
			}
			do
			{
				t.tToken_s=NextToken(" ");
				if (  t.tToken_s  !=  "" ) 
				{
					t.tArray[t.tArrayMarker] = t.tToken_s;
					++t.tArrayMarker;
				}
			} while ( !(  t.tToken_s == "" ) );
			t.tStippedString_s = "";
			int tCount = 0;
			for ( tCount = 0 ; tCount<=  t.tArrayMarker-1; tCount++ )
			{
				if (  tCount < 3 ) 
				{
					t.tStippedString_s = t.tStippedString_s + t.tArray[tCount];
				}
				else
				{
					t.tStippedString_s = t.tStippedString_s + " " + t.tArray[tCount];
				}
			}
			if (  t.tStippedString_s  != "" && strcmp ( Left(t.tStippedString_s.Get(),1) , ";" ) != 0 ) 
			{
				t.tToken_s=FirstToken(t.tStippedString_s.Get(),"=");
				t.tToken2_s=NextToken("=");

				//  Get rid of any tabs that exist and replace with nothing (some files have tabs in sometimes)
				t.tstring_s = t.tToken_s ; t.tToken_s = "";
				for ( tCount = 1 ; tCount<=  Len(t.tstring_s.Get()); tCount++ )
				{
					if (  cstr(Mid(t.tstring_s.Get(),tCount))  !=  Chr(9)  )  t.tToken_s  =  t.tToken_s + Mid(t.tstring_s.Get(),tCount);
				}

				t.tstring_s = t.tToken2_s ; t.tToken2_s = "";
				for ( tCount = 1 ; tCount<=  Len(t.tstring_s.Get()); tCount++ )
				{
					if ( cstr( Mid(t.tstring_s.Get(),tCount))  !=  Chr(9)  )  t.tToken2_s  =  t.tToken2_s + Mid(t.tstring_s.Get(),tCount);
				}

				//  Header
				if ( t.tToken_s == "protected" ) { t.importer.isProtected  =  ValF(t.tToken2_s.Get()) ; }
				else if ( t.tToken_s == "desc" ) { t.importer.objectFPE.desc  =  t.tToken2_s ; }
				//  Visual Info
				else if ( t.tToken_s == "textured" ) { t.importer.objectFPE.textured  =  t.tToken2_s ; }
				else if ( t.tToken_s == "effect" ) { t.importer.objectFPE.effect  =  t.tToken2_s ; }
				else if ( t.tToken_s == "castshadow" ) { t.importer.objectFPE.castshadow  =  t.tToken2_s ; }
				else if ( t.tToken_s == "transparency" ) { t.importer.objectFPE.transparency  =  t.tToken2_s ; } 
				//  Orientation
				else if ( t.tToken_s == "model" ) { t.importer.objectFPE.model  =  t.tToken2_s ; }
				else if ( t.tToken_s == "offx" ) { t.importer.objectFPE.offx  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "offy" ) { t.importer.objectFPE.offy  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "offz" ) { t.importer.objectFPE.offz  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "rotx" ) { t.importer.objectFPE.rotx  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "roty" ) { t.importer.objectFPE.roty  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "rotz" ) { t.importer.objectFPE.rotz  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "scale" ) { t.importer.objectFPE.scale  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "collisionmode" ) { t.importer.objectFPE.collisionmode  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "defaultstatic" )
				{
					t.importer.objectFPE.defaultstatic  =  t.tToken2_s ;
				} 
				else if ( t.tToken_s == "materialindex" ) { t.importer.objectFPE.materialindex  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "matrixmode" ) { t.importer.objectFPE.matrixmode  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "cullmode" ) { t.importer.objectFPE.cullmode  =  t.tToken2_s ; } 
					
				//  Identity details
				else if ( t.tToken_s == "ischaracter" ) { t.importer.objectFPE.ischaracter  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "hasweapon" ) { t.importer.objectFPE.hasweapon  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "isobjective" ) { t.importer.objectFPE.isobjective  =  t.tToken2_s ; } 

				//  Statistics
				else if ( t.tToken_s == "strength" ) { t.importer.objectFPE.strength  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "explodable" ) { t.importer.objectFPE.explodable  =  t.tToken2_s ; } 
				else if ( t.tToken_s == "debrisshape" ) { t.importer.objectFPE.debrisshape  =  t.tToken2_s ; }
				//  AI
				else if ( t.tToken_s == "aimain" ) { importer_check_script_token_exists() ; t.importer.objectFPE.aimain  =  t.tToken2_s ; }
				//  Spawn
				else if ( t.tToken_s == "spawnmax" ) { t.importer.objectFPE.spawnmax  =  t.tToken2_s ; }
				else if ( t.tToken_s == "spawndelay" ) { t.importer.objectFPE.spawndelay  =  t.tToken2_s ; }
				else if ( t.tToken_s == "spawnqty" ) { t.importer.objectFPE.spawnqty  =  t.tToken2_s ; }
				//  Physics
				else if ( t.tToken_s == "physicscount" ) {
					t.tPhyscount = ValF(t.tToken2_s.Get());
					t.importer.collisionShapeCount = 0;
					while (  t.importer.collisionShapeCount < t.tPhyscount ) 
					{
						t.tstring_s = ReadString (  1 );

						t.tArrayMarker = 0;
						t.ttToken_s=FirstToken(t.tstring_s.Get()," ");
						if (  t.ttToken_s  !=  "" ) 
						{
							t.tArray[t.tArrayMarker] = t.ttToken_s;
							++t.tArrayMarker;
						}
						do
						{
							t.ttToken_s=NextToken(" ");
							if (  t.ttToken_s  !=  "" ) 
							{
								t.tArray[t.tArrayMarker] = t.ttToken_s;
								++t.tArrayMarker;
							}
						} while ( !(  t.ttToken_s == "" ) );
						t.tStippedString_s = "";
						for ( int tCount = 0 ; tCount<=  t.tArrayMarker-1; tCount++ )
						{
							if (  tCount < 3 ) 
							{
								t.tStippedString_s = t.tStippedString_s + t.tArray[tCount];
							}
							else
							{
								t.tStippedString_s = t.tStippedString_s + " " + t.tArray[tCount];
							}
						}
						if (  t.tStippedString_s  !=  "" && t.tStippedString_s.Get()[0]  !=  ';' ) 
						{
							t.ttToken_s=FirstToken(t.tStippedString_s.Get(),"=");
							t.ttToken2_s=NextToken("=");

							//  Get rid of any tabs that exist and replace with nothing (some files have tabs in sometimes)
							t.tstring_s = t.ttToken_s ; t.ttToken_s = "";
							for ( int tCount = 1 ; tCount<=  Len(t.tstring_s.Get()); tCount++ )
							{
								if (  cstr(Mid(t.tstring_s.Get(),tCount))  !=  Chr(9)  )  t.ttToken_s  =  t.ttToken_s + Mid(t.tstring_s.Get(),tCount);
							}

							t.tstring_s = t.ttToken2_s ; t.ttToken2_s = "";
							for ( int tCount = 1 ; tCount<=  Len(t.tstring_s.Get()); tCount++ )
							{
								if (  cstr(Mid(t.tstring_s.Get(),tCount))  !=  Chr(9)  )  t.ttToken2_s  =  t.ttToken2_s + Mid(t.tstring_s.Get(),tCount);
							}

							//  get rid of the quotation marks
							t.tStrip_s = t.ttToken2_s;
							t.tStrip_s = Left(t.tStrip_s.Get(), Len(t.tStrip_s.Get())-1);
							t.tStrip_s = Right(t.tStrip_s.Get(), Len(t.tStrip_s.Get())-1);

							t.tArrayMarker = 0;
							t.ttToken_s=FirstToken(t.tStrip_s.Get(),",");
							if (  t.ttToken_s  !=  "" ) 
							{
								t.tArray[t.tArrayMarker] = t.ttToken_s;
								++t.tArrayMarker;
							}
							do
							{
								t.ttToken_s=NextToken(",");
								if (  t.ttToken_s  !=  "" ) 
								{
									t.tArray[t.tArrayMarker] = t.ttToken_s;
									++t.tArrayMarker;
								}
							} while ( !(  t.ttToken_s == "" ) );

							//  Format; shapetype, sizex, sizey, sizez, offx, offy, offz, rotx, roty, rotz
							t.tPShapeType = ValF(t.tArray[0].Get());
							t.tPSizeX_f = ValF(t.tArray[1].Get());
							t.tPSizeY_f = ValF(t.tArray[2].Get());
							t.tPSizeZ_f = ValF(t.tArray[3].Get());
							t.tPOffX_f = ValF(t.tArray[4].Get());
							t.tPOffY_f = ValF(t.tArray[5].Get());
							t.tPOffZ_f = ValF(t.tArray[6].Get());
							t.tPRotX_f = ValF(t.tArray[7].Get());
							t.tPRotY_f = ValF(t.tArray[8].Get());
							t.tPRotZ_f = ValF(t.tArray[9].Get());
							importer_add_collision_box_loaded ( );
						}
					}
				}
				else
				{
					//  Store any unknown lines
					t.tUnknown[t.importer.unknownFPELineCount] = t.tempLine_s;
					++t.importer.unknownFPELineCount;
				}
			}
		}

	}

	CloseFile (  1 );

	//  Cleanup
	UnDim (  t.tArray );

	importer_apply_fpe ( );
}

void importer_apply_fpe ( void )
{
	//  Scale
	t.slidersmenuvalue[t.importer.properties1Index][1].value = ValF(t.importer.objectFPE.scale.Get());
	t.importer.oldScale = t.slidersmenuvalue[t.importer.properties1Index][1].value;

	//  angle
	t.importer.objectAngleY = ValF(t.importer.objectFPE.roty.Get());
	t.slidersmenuvalue[t.importer.properties1Index][3].value = t.importer.objectAngleY;
	YRotateObject (  t.importer.objectnumber,t.importer.objectAngleY );

	//  Collision Mode
	if (  t.importer.objectFPE.collisionmode  ==  "0" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][4].value_s = "Box";
		t.slidersmenuvalue[t.importer.properties1Index][4].value=1;
	}
	if (  t.importer.objectFPE.collisionmode  ==  "1" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][4].value_s = "Polygon";
		t.slidersmenuvalue[t.importer.properties1Index][4].value=2;
	}
	if (  t.importer.objectFPE.collisionmode  ==  "99" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][4].value_s = "No Collision";
		t.slidersmenuvalue[t.importer.properties1Index][4].value=3;
	}
	if (  t.importer.objectFPE.collisionmode  ==  "1001" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][4].value_s = "Limb One Box";
		t.slidersmenuvalue[t.importer.properties1Index][4].value=4;
	}
	if (  t.importer.objectFPE.collisionmode  ==  "2001" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][4].value_s = "Limb One Poly";
		t.slidersmenuvalue[t.importer.properties1Index][4].value=5;
	}
	if (  t.importer.objectFPE.collisionmode  ==  "40" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][4].value_s = "Collision Boxes";
		t.slidersmenuvalue[t.importer.properties1Index][4].value=6;
	}

	//  Default Static
	if (  t.importer.objectFPE.defaultstatic  ==  "0" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][5].value_s = "No";
		t.slidersmenuvalue[t.importer.properties1Index][5].value=2;
	}
	else
	{
		t.slidersmenuvalue[t.importer.properties1Index][5].value_s = "Yes";
		t.slidersmenuvalue[t.importer.properties1Index][5].value=1;
	}

	//  Strength
	t.slidersmenuvalue[t.importer.properties1Index][6].value = ValF(t.importer.objectFPE.strength.Get());

	//  Cull Mode
	if (  t.importer.objectFPE.cullmode  ==  "0" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][8].value_s = "No";
		t.slidersmenuvalue[t.importer.properties1Index][8].value=2;
	}
	else
	{
		t.slidersmenuvalue[t.importer.properties1Index][8].value_s = "Yes";
		t.slidersmenuvalue[t.importer.properties1Index][8].value=1;
	}

	//  Transparency
	if (  t.importer.objectFPE.transparency  ==  "0" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][9].value_s = "None";
		t.slidersmenuvalue[t.importer.properties1Index][9].value=1;
	}
	if (  t.importer.objectFPE.transparency  ==  "1" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][9].value_s = "Standard";
		t.slidersmenuvalue[t.importer.properties1Index][9].value=2;
	}
	if (  t.importer.objectFPE.transparency  ==  "2" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][9].value_s = "Render last";
		t.slidersmenuvalue[t.importer.properties1Index][9].value=3;
	}
	//  Material Inxed
	if (  t.importer.objectFPE.materialindex  ==  "0" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][10].value_s = "Generic";
		t.slidersmenuvalue[t.importer.properties1Index][10].value=1;
	}
	if (  t.importer.objectFPE.materialindex  ==  "1" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][10].value_s = "Stone";
		t.slidersmenuvalue[t.importer.properties1Index][10].value=2;
	}
	if (  t.importer.objectFPE.materialindex  ==  "2" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][10].value_s = "Metal";
		t.slidersmenuvalue[t.importer.properties1Index][10].value=3;
	}
	if (  t.importer.objectFPE.materialindex  ==  "3" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][10].value_s = "Wood";
		t.slidersmenuvalue[t.importer.properties1Index][10].value=4;
	}
	//  Is Character
	if (  t.importer.objectFPE.ischaracter  ==  "0" ) 
	{
		t.slidersmenuvalue[t.importer.properties1Index][7].value_s = "No";
		t.slidersmenuvalue[t.importer.properties1Index][7].value=1;
	}
	else
	{
		t.slidersmenuvalue[t.importer.properties1Index][7].value_s = "Yes";
		t.slidersmenuvalue[t.importer.properties1Index][7].value=2;
	}

	//  Shader effect
	t.importer.objectFPE.effect = Right(t.importer.objectFPE.effect.Get(), Len(t.importer.objectFPE.effect.Get())-20);

	for ( t.tc = 1 ; t.tc<=  t.importer.shaderFileCount; t.tc++ )
	{
		if (  t.importerShaderFiles[t.tc]  ==  t.importer.objectFPE.effect ) 
		{
			t.slidersmenuvalue[t.importer.properties1Index][2].value=t.tc;
			t.slidersmenuvalue[t.importer.properties1Index][2].value_s=t.importerShaderFiles[t.tc];
			break;
		}
	}
	//  AI MAIN
	for ( t.tc = 1 ; t.tc<=  t.importer.scriptFileCount; t.tc++ )
	{
		if (  t.importerScriptFiles[t.tc]  ==  t.importer.objectFPE.aimain ) 
		{
			t.slidersmenuvalue[t.importer.properties1Index][11].value=t.tc;
			t.slidersmenuvalue[t.importer.properties1Index][11].value_s=t.importerScriptFiles[t.tc];
			break;
		}
	}
}

#ifdef WICKEDENGINE
void imporer_save_multimeshsection(sObject* pObject, int iFileIndex)
{
	t.tString = ""; t.tString = t.tString + importerPadString("textured") + "= " ;WriteString(iFileIndex, t.tString.Get());
	for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
	{
		sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
		if (pMesh)
		{
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
			if (mesh)
			{
				uint64_t materialEntity = mesh->subsets[0].materialID;
				wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
				if (pObjectMaterial)
				{
					// texture names and control values
					if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].resource)
					{
						cStr TextureFilename = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].name.c_str());
						t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("baseColorMap") + cstr(iMeshIndex)).Get()) + "= " + TextureFilename; WriteString(iFileIndex, t.tString.Get());
						t.tString = ""; t.tString = t.tString + importerPadString(cStr(cStr("alphaRef") + cstr(iMeshIndex)).Get()) + "= " + cStr(pObjectMaterial->alphaRef); WriteString(iFileIndex, t.tString.Get());
					}
					if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::NORMALMAP].resource)
					{
						cStr TextureFilename = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::NORMALMAP].name.c_str());
						t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("normalMap") + cstr(iMeshIndex)).Get()) + "= " + TextureFilename; WriteString(iFileIndex, t.tString.Get());
						t.tString = ""; t.tString = t.tString + importerPadString(cStr(cStr("normalStrength") + cstr(iMeshIndex)).Get()) + "= " + cStr(pObjectMaterial->normalMapStrength); WriteString(iFileIndex, t.tString.Get());
					}
					if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::EMISSIVEMAP].resource)
					{
						cStr TextureFilename = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::EMISSIVEMAP].name.c_str());
						t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("emissiveMap") + cstr(iMeshIndex)).Get()) + "= " + TextureFilename; WriteString(iFileIndex, t.tString.Get());
						t.tString = ""; t.tString = t.tString + importerPadString(cStr(cStr("emissiveStrength") + cstr(iMeshIndex)).Get()) + "= " + cStr(pObjectMaterial->GetEmissiveStrength()); WriteString(iFileIndex, t.tString.Get());
					}
					if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].resource)
					{
						cStr TextureFilename = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].name.c_str());
						t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("surfaceMap") + cstr(iMeshIndex)).Get()) + "= " + TextureFilename; WriteString(iFileIndex, t.tString.Get());
						t.tString = ""; t.tString = t.tString + importerPadString(cStr(cStr("roughnessStrength") + cstr(iMeshIndex)).Get()) + "= " + cStr(pObjectMaterial->roughness); WriteString(iFileIndex, t.tString.Get());
						t.tString = ""; t.tString = t.tString + importerPadString(cStr(cStr("metalnessStrength") + cstr(iMeshIndex)).Get()) + "= " + cStr(pObjectMaterial->metalness); WriteString(iFileIndex, t.tString.Get());
					}
					if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].resource)
					{
						cStr TextureFilename = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].name.c_str());
						t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("occlusionMap") + cstr(iMeshIndex)).Get()) + "= " + TextureFilename; WriteString(iFileIndex, t.tString.Get());
					}
					if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].resource)
					{
						cStr TextureFilename = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].name.c_str());
						t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("displacementMap") + cstr(iMeshIndex)).Get()) + "= " + TextureFilename; WriteString(iFileIndex, t.tString.Get());
					}

					// mesh settings
					bTransparent = pObjectMaterial->userBlendMode == BLENDMODE_ALPHA;
					if (bTransparent)
					{
						t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("transparency") + cstr(iMeshIndex)).Get()) + "= 1"; WriteString(iFileIndex, t.tString.Get());
					}
					else
					{
						t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("transparency") + cstr(iMeshIndex)).Get()) + "= 0"; WriteString(iFileIndex, t.tString.Get());
					}
					bCastShadows = pObjectMaterial->IsCastingShadow();
					if (!bCastShadows)
					{
						t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("castshadow") + cstr(iMeshIndex)).Get()) + "= -1"; WriteString(iFileIndex, t.tString.Get());
					}
					bDoubleSided = mesh->IsDoubleSided();
					if (bDoubleSided)
					{
						t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("doublesided") + cstr(iMeshIndex)).Get()) + "= 1"; WriteString(iFileIndex, t.tString.Get());
					}
					sFrame* pFrame = pMesh->pFrameAttachedTo;
					if (pFrame)
					{
						wiScene::ObjectComponent* object = wiScene::GetScene().objects.GetComponent(pFrame->wickedobjindex);
						if (object)
						{
							fRenderOrderBias = object->GetRenderOrderBiasDistance();
							t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("renderorderbias") + cstr(iMeshIndex)).Get()) + "= " + cStr(fRenderOrderBias); WriteString(iFileIndex, t.tString.Get());
						}
					}
					bPlanerReflection = pObjectMaterial->shaderType == wiScene::MaterialComponent::SHADERTYPE_PBR_PLANARREFLECTION;
					if (bPlanerReflection)
					{
						t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("planerreflection") + cstr(iMeshIndex)).Get()) + "= 1"; WriteString(iFileIndex, t.tString.Get());
					}
					fReflectance = pObjectMaterial->reflectance;
					char cReflectance[80];
					sprintf(cReflectance, "%.4f", fReflectance); //PE: We need better precision on this one.
					t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("reflectance") + cstr(iMeshIndex)).Get()) + "= " + cStr(cReflectance); WriteString(iFileIndex, t.tString.Get());

					// material colors
					dwBaseColor = ((unsigned int)(pObjectMaterial->baseColor.x * 255) << 24);
					dwBaseColor += ((unsigned int)(pObjectMaterial->baseColor.y * 255) << 16);
					dwBaseColor += ((unsigned int)(pObjectMaterial->baseColor.z * 255) << 8);
					dwBaseColor += ((unsigned int)(pObjectMaterial->baseColor.w * 255));
					char tmp[256];
					sprintf(tmp, "%lu", (unsigned long)dwBaseColor);
					t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("basecolor") + cstr(iMeshIndex)).Get()) + "= " + cStr(tmp); WriteString(iFileIndex, t.tString.Get());
					dwEmmisiveColor = ((unsigned int)(pObjectMaterial->emissiveColor.x * 255) << 24);
					dwEmmisiveColor += ((unsigned int)(pObjectMaterial->emissiveColor.y * 255) << 16);
					dwEmmisiveColor += ((unsigned int)(pObjectMaterial->emissiveColor.z * 255) << 8);
					// ignore pObjectMaterial->emissiveColor.w, it is saved above in emissivestrength!
					sprintf(tmp, "%lu", (unsigned long)dwEmmisiveColor);
					t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("emissivecolor") + cstr(iMeshIndex)).Get()) + "= " + cStr(tmp); WriteString(iFileIndex, t.tString.Get());
				}
			}
		}
	}
}
#endif

void importer_save_fpe(void)
{
	// angle
	t.importer.objectFPE.roty = Str(t.importer.objectAngleY);

	// scale and collision
	#if defined(ENABLEIMGUI) && !defined(USEOLDIDE)
	#ifdef WICKEDENGINE
	t.importer.objectFPE.scale = (iImporterScale * fImporterScaleMultiply);
	#else
	t.importer.objectFPE.scale = iImporterScale;
	#endif
	#ifdef WICKEDENGINE
	LPSTR pCollisionType = "0";
	if (t.importer.collisionshape == 0) pCollisionType = "0";   // box
	if (t.importer.collisionshape == 1) pCollisionType = "1";   // polygon
	if (t.importer.collisionshape == 2) pCollisionType = "2";   // sphere
	if (t.importer.collisionshape == 3) pCollisionType = "3";   // cylinder
	if (t.importer.collisionshape == 4) pCollisionType = "9";   // hull
	if (t.importer.collisionshape == 5) pCollisionType = "21";  // character collision
	if (t.importer.collisionshape == 6) pCollisionType = "50";  // tree collision
	if (t.importer.collisionshape == 7) pCollisionType = "11";  // no collision
	if (t.importer.collisionshape == 8) pCollisionType = "10";	// hull decomp
	if (t.importer.collisionshape == 9) pCollisionType = "8"; // Collision Mesh
	t.importer.objectFPE.collisionmode = pCollisionType;
	#else
	t.importer.objectFPE.collisionmode = "1"; //Poly for now. just change to 9 for convex hull. We need a setting for this.
	#endif
	#else
	// Classic
	t.importer.objectFPE.scale = Str(t.slidersmenuvalue[t.importer.properties1Index][1].value);
	if (t.slidersmenuvalue[t.importer.properties1Index][4].value == 1)  t.importer.objectFPE.collisionmode = "0";
	if (t.slidersmenuvalue[t.importer.properties1Index][4].value == 2)  t.importer.objectFPE.collisionmode = "1";
	if (t.slidersmenuvalue[t.importer.properties1Index][4].value == 3) {
		//PE: 99 gives a collision box. we need 11 here if this is a static object.
		if ( t.slidersmenuvalue[t.importer.properties1Index][5].value != 2)
			t.importer.objectFPE.collisionmode = "11";
		else
			t.importer.objectFPE.collisionmode = "99";
	}
	if (t.slidersmenuvalue[t.importer.properties1Index][4].value == 4)  t.importer.objectFPE.collisionmode = "1001";
	if (t.slidersmenuvalue[t.importer.properties1Index][4].value == 5)  t.importer.objectFPE.collisionmode = "2001";
	if (t.slidersmenuvalue[t.importer.properties1Index][4].value == 6)  t.importer.objectFPE.collisionmode = "40";
	#endif

	//  Default Static or Dynamic
	#ifdef WICKEDENGINE
	LPSTR pStaticType = "0"; // dynamic by default
	if (t.importer.defaultstatic == 1) pStaticType = "1";  // static
	t.importer.objectFPE.defaultstatic = pStaticType;
	#else
	if (t.slidersmenuvalue[t.importer.properties1Index][5].value == 2)
		t.importer.objectFPE.defaultstatic = "0";
	else
		t.importer.objectFPE.defaultstatic = "1";
	#endif

	//  Cull Mode
	sObject* pObject = NULL;
	#ifdef WICKEDENGINE
	pObject = GetObjectData(t.importer.objectnumber);
	if (pObject)
	{
		t.importer.objectFPE.cullmode = "0";
		for (int i = 0; i < pObject->iMeshCount; i++)
		{

			sMesh * pMesh = pObject->ppMeshList[i];
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
			if (mesh && !mesh->IsDoubleSided())
			{
				t.importer.objectFPE.cullmode = "1";
				break;
			}
		}
	}
		
	#else
	if (t.slidersmenuvalue[t.importer.properties1Index][8].value == 2)
		t.importer.objectFPE.cullmode = "0";
	else
		t.importer.objectFPE.cullmode = "1";
	#endif

	//  Transparency
	t.importer.objectFPE.transparency = Str(t.slidersmenuvalue[t.importer.properties1Index][9].value - 1);

	// MaterialIndex
	// Strength
	if (t.importer.ischaracter > 0)
	{
		t.importer.objectFPE.materialindex = Str(6);
		t.importer.objectFPE.strength = Str(150);
	}
	else
	{
		t.importer.objectFPE.materialindex = Str(t.slidersmenuvalue[t.importer.properties1Index][10].value - 1);
		t.importer.objectFPE.strength = Str(t.slidersmenuvalue[t.importer.properties1Index][6].value);
	}

	//  Explodable
	t.importer.objectFPE.explodable = "0";

	//  castshadow
	t.importer.objectFPE.castshadow = "0";

	//  ischaracter
	if(t.importer.ischaracter > 0)//if (t.slidersmenuvalue[t.importer.properties1Index][7].value <= 1)
	{
		t.importer.objectFPE.ischaracter = "1";
		t.importer.objectFPE.aimain = "people\\melee_attack.lua";
	}
	else
	{
		t.importer.objectFPE.ischaracter = "0";
		t.importer.objectFPE.aimain = "";
	}

	//  isobjective
	t.importer.objectFPE.isobjective = "0";//Str(t.slidersmenuvalue[t.importer.properties1Index][8].value);//bug
	t.importer.objectFPE.cantakeweapon = "0";

	//  update shader selection
	#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	if (g.gpbroverride == 1) 
	{
		g_iPreferPBR = 1;
		t.importer.objectFPE.effect = "effectbank\\reloaded\\apbr_basic.fx";
	}
	else 
	{
		g_iPreferPBR = 0;
		t.importer.objectFPE.effect = "effectbank\\reloaded\\entity_basic.fx";
	}
	#else
	t.importer.objectFPE.effect = ""; t.importer.objectFPE.effect = t.importer.objectFPE.effect+"effectbank\\reloaded\\" + t.slidersmenuvalue[t.importer.properties1Index][2].value_s;
	if ( strnicmp ( t.slidersmenuvalue[t.importer.properties1Index][2].value_s.Get(), "apbr_", 5 ) == NULL ) 
		g_iPreferPBR = 1;
	else
		g_iPreferPBR = 0;
	#endif

	//  aimain
	//t.importer.objectFPE.aimain = t.slidersmenuvalue[t.importer.properties1Index][11].value_s;

	// could do with a nice cleanup this whole function!
	
	#ifdef WICKEDENGINE
	// animation state
	if (t.importer.ischaracter > 0)
	{
		t.importer.objectFPE.playanimineditor = "idle";
		t.importer.objectFPE.animmax = "0";
		t.importer.objectFPE.anim0 = "";
	}
	else
	{
		if (g_pAnimSlotList.size() > 0)
		{
			if (g_bAnimatingObjectPreview == true)
				t.importer.objectFPE.playanimineditor = "1";
			else
				t.importer.objectFPE.playanimineditor = "0";
			t.importer.objectFPE.animmax = cstr(((int)g_pAnimSlotList.size()) - 1);
			float fStart = g_pAnimSlotList[g_iCurrentAnimationSlotIndex].fStart;
			float fFinish = g_pAnimSlotList[g_iCurrentAnimationSlotIndex].fFinish;
			t.importer.objectFPE.anim0 = cstr((int)fStart) + cstr(",") + cstr((int)fFinish);
		}
		else
		{
			t.importer.objectFPE.playanimineditor = "0";
			t.importer.objectFPE.animmax = "0";
			t.importer.objectFPE.anim0 = "";
		}
	}
	pObject = GetObjectData(t.importer.objectnumber);
	t.importer.objectFPE.animspeed = cstr(pObject->fAnimSpeed * 100);
	#endif

	// ensure collision objects are back to reset position (ignoring camera height adjustment)
	importer_RestoreCollisionShiftHeight();

	t.chosenFileNameFPE_s = t.importer.tFPESaveName;
	if (  FileOpen (1)  )  CloseFile (1);
	if (  FileExist (t.chosenFileNameFPE_s.Get())  )  DeleteAFile (  t.chosenFileNameFPE_s.Get() ) ;
	OpenToWrite (  1, t.chosenFileNameFPE_s.Get() );

	t.tString = ";Saved by Model Importer" ;WriteString (  1 , t.tString.Get() );
	t.tString = ";header" ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("desc") + "= " + t.importer.objectFPE.desc ;WriteString (  1 , t.tString.Get() );

	//PE: We needed the ;orientation part in Classic , so ...
	#ifndef WICKEDENGINE
	t.tString = ";visualinfo" ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("textured") + "= " + t.importer.objectFPE.textured ;WriteString (  1 , t.tString.Get() );
	#endif

	if (t.importer.ischaracter > 0)
	{
		t.tString = ""; WriteString (1, t.tString.Get());
		t.tString = ";character"; WriteString (1, t.tString.Get());
		if (t.importer.ischaracter == 1)
		{
			t.tString = ""; t.tString = t.tString + importerPadString("ccpassembly") + "= " + "adult male"; WriteString (1, t.tString.Get());
			t.tString = ""; t.tString = t.tString + importerPadString("voice") + "= " + "David"; WriteString (1, t.tString.Get());
		}
		else
		{
			t.tString = ""; t.tString = t.tString + importerPadString("ccpassembly") + "= " + "adult female"; WriteString (1, t.tString.Get());
			t.tString = ""; t.tString = t.tString + importerPadString("voice") + "= " + "Microsoft Hazel Desktop - English (Great Britain)"; WriteString (1, t.tString.Get());
		}
	}

	#ifdef WICKEDENGINE
	// get object ptr
	pObject = GetObjectData(t.importer.objectnumber);
	#else
	// get object ptr
	pObject = GetObjectData(t.importer.objectnumber);
	#endif

	#ifdef WICKEDENGINE
	// defaults for FPE (actual settings above)
	#else
	t.tString = "" ; t.tString = t.tString + importerPadString("castshadow") + "= " + t.importer.objectFPE.castshadow ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("transparency") + "= " + t.importer.objectFPE.transparency ;WriteString (  1 , t.tString.Get() );
	#endif

	t.tString = "" ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + ";orientation" ; WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("model") + "= " + t.importer.objectFPE.model ;WriteString (  1 , t.tString.Get() );

	#ifdef WICKEDENGINE
	// offset and rotation now stored in frame zero vecPosition and vecRotation (inside DBO)
	#else
	t.tString = "" ; t.tString = t.tString + importerPadString("offx") + "= " + t.importer.objectFPE.offx ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("offy") + "= " + t.importer.objectFPE.offy ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("offz") + "= " + t.importer.objectFPE.offz ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("rotx") + "= " + t.importer.objectFPE.rotx ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("roty") + "= " + t.importer.objectFPE.roty ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("rotz") + "= " + t.importer.objectFPE.rotz ;WriteString (  1 , t.tString.Get() );
	#endif
	t.tString = "" ; t.tString = t.tString + importerPadString("scale") + "= " + t.importer.objectFPE.scale ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("collisionmode") + "= " + t.importer.objectFPE.collisionmode ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("defaultstatic") + "= " + t.importer.objectFPE.defaultstatic ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("materialindex") + "= " + t.importer.objectFPE.materialindex ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("cullmode") + "= " + t.importer.objectFPE.cullmode ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ;WriteString (  1 , t.tString.Get() );

	// if only one mesh in object
	#ifdef WICKEDENGINE
	t.tString = ";visualinfo" ;WriteString (  1 , t.tString.Get() );

	bool bTextured = false;
	if (pObject->iMeshCount==1) 
	{
		// single material export
		sMesh * pMesh = pObject->ppMeshList[0];
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
		if (mesh)
		{
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
			if (pObjectMaterial)
			{
				if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].resource)
				{
					cStr TextureFilename = importer_getfilenameonly((LPSTR) pObjectMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].name.c_str());
					t.tString = ""; t.tString = t.tString + importerPadString("textured") + "= " + TextureFilename; WriteString(1, t.tString.Get());
					t.tString = ""; t.tString = t.tString + importerPadString("baseColorMap") + "= " + TextureFilename; WriteString(1, t.tString.Get());
					t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("alpharef")).Get()) + "= " + cStr(pObjectMaterial->alphaRef); WriteString(1, t.tString.Get());
					bTextured = true;
				}
				if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::NORMALMAP].resource)
				{
					cStr TextureFilename = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::NORMALMAP].name.c_str());
					t.tString = ""; t.tString = t.tString + importerPadString("normalMap") + "= " + TextureFilename; WriteString(1, t.tString.Get());
					t.tString = ""; t.tString = t.tString + importerPadString("normalStrength") + "= " + cStr(pObjectMaterial->normalMapStrength); WriteString(1, t.tString.Get());
				}
				if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].resource)
				{
					cStr TextureFilename = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].name.c_str());
					t.tString = ""; t.tString = t.tString + importerPadString("surfaceMap") + "= " + TextureFilename; WriteString(1, t.tString.Get());
					t.tString = ""; t.tString = t.tString + importerPadString("roughnessStrength") + "= " + cStr(pObjectMaterial->roughness); WriteString(1, t.tString.Get());
					t.tString = ""; t.tString = t.tString + importerPadString("metalnessStrength") + "= " + cStr(pObjectMaterial->metalness); WriteString(1, t.tString.Get());
				}
				if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].resource)
				{
					cStr TextureFilename = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].name.c_str());
					t.tString = ""; t.tString = t.tString + importerPadString("displacementMap") + "= " + TextureFilename; WriteString(1, t.tString.Get());
				}
				if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::EMISSIVEMAP].resource)
				{
					cStr TextureFilename = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::EMISSIVEMAP].name.c_str());
					t.tString = ""; t.tString = t.tString + importerPadString("emissiveMap") + "= " + TextureFilename; WriteString(1, t.tString.Get());
					t.tString = ""; t.tString = t.tString + importerPadString("emissiveStrength") + "= " + cStr(pObjectMaterial->GetEmissiveStrength()); WriteString(1, t.tString.Get());
				}
				if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].resource)
				{
					cStr TextureFilename = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].name.c_str());
					t.tString = ""; t.tString = t.tString + importerPadString("occlusionMap") + "= " + TextureFilename; WriteString(1, t.tString.Get());
				}

				// do not like this duplication - can do better down the road
				// mesh settings
				bTransparent = pObjectMaterial->userBlendMode == BLENDMODE_ALPHA;
				if (bTransparent) 
				{
					t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("transparency")).Get()) + "= 1"; WriteString(1, t.tString.Get());
				}
				else 
				{
					t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("transparency")).Get()) + "= 0"; WriteString(1, t.tString.Get());
				}
				bCastShadows = pObjectMaterial->IsCastingShadow();
				if (!bCastShadows) 
				{
					t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("castshadow")).Get()) + "= -1"; WriteString(1, t.tString.Get());
				}
				bDoubleSided = mesh->IsDoubleSided();
				if (bDoubleSided) 
				{
					t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("doublesided")).Get()) + "= 1"; WriteString(1, t.tString.Get());
				}
				sFrame* pFrame = pMesh->pFrameAttachedTo;
				if (pFrame)
				{
					wiScene::ObjectComponent* object = wiScene::GetScene().objects.GetComponent(pFrame->wickedobjindex);
					if (object)
					{
						fRenderOrderBias = object->GetRenderOrderBiasDistance();
						t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("renderorderbias")).Get()) + "= " + cStr(fRenderOrderBias); WriteString(1, t.tString.Get());
					}
				}
				bPlanerReflection = pObjectMaterial->shaderType == wiScene::MaterialComponent::SHADERTYPE_PBR_PLANARREFLECTION;
				if (bPlanerReflection) 
				{
					t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("planerreflection")).Get()) + "= 1"; WriteString(1, t.tString.Get());
				}
				fReflectance = pObjectMaterial->reflectance;
				t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("reflectance") ).Get()) + "= " + cStr(fReflectance); WriteString(1, t.tString.Get());

				// material colors
				dwBaseColor = ((unsigned int)(pObjectMaterial->baseColor.x * 255) << 24);
				dwBaseColor += ((unsigned int)(pObjectMaterial->baseColor.y * 255) << 16);
				dwBaseColor += ((unsigned int)(pObjectMaterial->baseColor.z * 255) << 8);
				dwBaseColor += ((unsigned int)(pObjectMaterial->baseColor.w * 255));
				char tmp[256];
				sprintf(tmp, "%lu", (unsigned long)dwBaseColor);
				t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("basecolor")).Get()) + "= " + cStr(tmp); WriteString(1, t.tString.Get());
				dwEmmisiveColor = ((unsigned int)(pObjectMaterial->emissiveColor.x * 255) << 24);
				dwEmmisiveColor += ((unsigned int)(pObjectMaterial->emissiveColor.y * 255) << 16);
				dwEmmisiveColor += ((unsigned int)(pObjectMaterial->emissiveColor.z * 255) << 8);
				// ignore pObjectMaterial->emissiveColor.w, it is saved above in emissivestrength!
				sprintf(tmp, "%lu", (unsigned long)dwEmmisiveColor);
				t.tString = ""; t.tString = t.tString + importerPadString(cstr(cstr("emissivecolor")).Get()) + "= " + cStr(tmp); WriteString(1, t.tString.Get());
			}
		}
	}
	else
	{
		// multi material mesh export
		bTextured = true;
		imporer_save_multimeshsection(pObject,1);
	}
	if (!bTextured) 
	{
		t.tString = ""; t.tString = t.tString + importerPadString("textured") + "= " + t.importer.objectFPE.textured;WriteString(1, t.tString.Get());
	}
	#endif

	//PE: Wicked - Below keep textureref
	// a new system to record texture references in FPE so can save standalone better
	for ( int tCount = 1 ; tCount <= IMPORTERTEXTURESMAX; tCount++ )
	{
		if ( strlen ( t.importerTextures[tCount].fileName.Get() ) > 0 ) 
		{
			char pFileOnly[2048];
			strcpy ( pFileOnly, t.importerTextures[tCount].fileName.Get() );
			for ( int n = strlen(pFileOnly)-1; n > 0; n-- )
			{
				if ( pFileOnly[n] == '\\' || pFileOnly[n] == '/' )
				{
					strcpy ( pFileOnly, pFileOnly+n+1 );
					break;
				}
			}
			t.tString = "" ; t.tString = t.tString + importerPadString(cstr(cstr("textureref")+cstr(tCount)).Get()) + " = " + cstr(pFileOnly);
			WriteString (  1 , t.tString.Get() );
		}
	}
	t.tString = "" ; t.tString = t.tString + importerPadString("effect") + "= " + t.importer.objectFPE.effect ;WriteString (  1 , t.tString.Get() );
	t.tString = ""; WriteString (1, t.tString.Get());

	t.tString = ";identity details" ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("ischaracter") + "= " + t.importer.objectFPE.ischaracter ;WriteString (  1 , t.tString.Get() );

	#ifdef VRTECH
	t.tString = "" ; t.tString = t.tString + importerPadString("hasweapon") + "= " + t.importer.objectFPE.hasweapon ;WriteString (  1 , t.tString.Get() );
	//t.tString = "" ; t.tString = t.tString + importerPadString("isobjective") + "= " + t.importer.objectFPE.isobjective ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("cantakeweapon") + "= " + t.importer.objectFPE.cantakeweapon ;WriteString (  1 , t.tString.Get() );
	#else
	if (t.slidersmenuvalue[t.importer.properties1Index][7].value == 3) { //is uber
		t.tString = ""; t.tString = t.tString + importerPadString("hasweapon") + "= modern\\colt1911"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("isobjective") + "= 0"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("cantakeweapon") + "= 1"; WriteString(1, t.tString.Get());
	}
	else {
		t.tString = ""; t.tString = t.tString + importerPadString("hasweapon") + "= " + t.importer.objectFPE.hasweapon;WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("isobjective") + "= " + t.importer.objectFPE.isobjective;WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("cantakeweapon") + "= " + t.importer.objectFPE.cantakeweapon;WriteString(1, t.tString.Get());
	}
#endif
	t.tString = "" ;WriteString (  1 , t.tString.Get() );

	#ifdef VRTECH
	#else
	if (t.slidersmenuvalue[t.importer.properties1Index][7].value == 3) 
	{ 
		//is uber If model already have firespot (from artist) this will overwrite it, so only if uber. (assume uber dont have firespot).
		t.tString = ""; t.tString = t.tString + importerPadString("handfirespotoffx") + "= 0.0707"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("handfirespotoffy") + "= -2.3718"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("handfirespotoffz") + "= -1.6504"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("handfirespotrotx") + "= -44.2759"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("handfirespotroty") + "= -22.4883"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("handfirespotrotz") + "= -26.3792"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("handfirespotsize") + "= 100"; WriteString(1, t.tString.Get());
	}
	#endif

	t.tString = ";statistics" ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("strength") + "= " + t.importer.objectFPE.strength ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("explodable") + "= " + t.importer.objectFPE.explodable ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ;WriteString (  1 , t.tString.Get() );

	t.tString = ";ai" ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("aimain") + "= " + t.importer.objectFPE.aimain ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ;WriteString (  1 , t.tString.Get() );

	//t.tString = ";spawn" ;WriteString (  1 , t.tString.Get() );
	//t.tString = "" ; t.tString = t.tString + importerPadString("spawnmax") + "= " + t.importer.objectFPE.spawnmax ;WriteString (  1 , t.tString.Get() );
	//t.tString = "" ; t.tString = t.tString + importerPadString("spawndelay") + "= " + t.importer.objectFPE.spawndelay ;WriteString (  1 , t.tString.Get() );
	//t.tString = "" ; t.tString = t.tString + importerPadString("spawnqty") + "= " + t.importer.objectFPE.spawnqty ;WriteString (  1 , t.tString.Get() );
	//t.tString = "" ;WriteString (  1 , t.tString.Get() );

	t.tString = ";anim" ;WriteString (  1 , t.tString.Get() );
#ifdef VRTECH
#ifdef WICKEDENGINE
	t.tString = "" ; t.tString = t.tString + importerPadString("animspeed") + "= " + t.importer.objectFPE.animspeed ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("animmax") + "= " + t.importer.objectFPE.animmax ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("anim0") + "= " + t.importer.objectFPE.anim0 ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("playanimineditor") + "= " + t.importer.objectFPE.playanimineditor ;WriteString (  1 , t.tString.Get() );
#else
	t.tString = "" ; t.tString = t.tString + importerPadString("animmax") + "= " + t.importer.objectFPE.animmax ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("anim0") + "= " + t.importer.objectFPE.anim0 ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("playanimineditor") + "= " + t.importer.objectFPE.playanimineditor ;WriteString (  1 , t.tString.Get() );
	t.tString = "" ; t.tString = t.tString + importerPadString("ignorecsirefs") + "= " + t.importer.objectFPE.ignorecsirefs ;WriteString (  1 , t.tString.Get() );
#endif
#else
	if (t.slidersmenuvalue[t.importer.properties1Index][7].value == 3) { //is uber
																		 //PE: Only uber will use these animations.
		t.tString = ""; t.tString = t.tString + importerPadString("animmax") + "= 5"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("playanimineditor") + "= 1"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("ignorecsirefs") + "= 1"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("anim0") + "= 100,205   ;idle"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("anim1") + "= 687,707   ;move"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("anim2") + "= 5511,5553 ;kick"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("anim3") + "= 4812,4850 ;hurt"; WriteString(1, t.tString.Get());
		t.tString = ""; t.tString = t.tString + importerPadString("anim4") + "= 515,605   ;reload"; WriteString(1, t.tString.Get());
	}
	else {
		t.tString = "" ; t.tString = t.tString + importerPadString("animmax") + "= " + t.importer.objectFPE.animmax ;WriteString (  1 , t.tString.Get() );
		t.tString = "" ; t.tString = t.tString + importerPadString("anim0") + "= " + t.importer.objectFPE.anim0 ;WriteString (  1 , t.tString.Get() );
		t.tString = "" ; t.tString = t.tString + importerPadString("playanimineditor") + "= " + t.importer.objectFPE.playanimineditor ;WriteString (  1 , t.tString.Get() );
		t.tString = "" ; t.tString = t.tString + importerPadString("ignorecsirefs") + "= " + t.importer.objectFPE.ignorecsirefs ;WriteString (  1 , t.tString.Get() );
	}
#endif

	#ifdef WICKEDENGINE
	// must clean up FPE export before V1 release
	#else
	//  Save any lines that were not understand when the initial FPE was loaded in
	//  add a blank Line (  (  if there are extra Lines to add ) )
	if (  t.importer.unknownFPELineCount > 0 ) 
	{
		t.tString = "" ;WriteString (  1 , t.tString.Get() );
		t.tString = ";settings not understood by the exporter" ;WriteString (  1 , t.tString.Get() );
		for ( int tCount = 0 ; tCount<=  t.importer.unknownFPELineCount-1; tCount++ )
		{
			if (  t.tUnknown[tCount].Get()[0] !=  ';' && Len(t.tUnknown[tCount].Get() ) > 2 ) 
			{
				WriteString (  1 , t.tUnknown[tCount].Get() );
			}
		}

	}
	#endif

	#ifdef WICKEDENGINE
	//PE: Set default backdrop.
	t.tString = "";WriteString(1, t.tString.Get());
	t.tString = ";thumbnail";WriteString(1, t.tString.Get());
	t.tString = "thumbnailbackdrop = Blue showroom.dds";WriteString(1, t.tString.Get());
	#endif

	CloseFile (  1 );


	#ifdef WICKEDENGINE

	//PE: You cant change REMEMBERIMPORTFILES , its fixed in the pref file, so if more then 10 is needed something else should be done.
	#define REMEMBERIMPORTFILES 10
	cstr fullfpename = t.tSavePath_s + t.chosenFileNameFPE_s;
	char *find = (char *) pestrcasestr(fullfpename.Get(), "entitybank\\");
	if (find)
	{
		//Only store if entitybank is used, not if saved outside GG.
		
		find += 11; //We use relative path after entitybank\\ for all fpe files.

		//Make list of 10 last imported files, save in pref.
		int firstempty = -1;
		int i = 0;
		for (; i < REMEMBERIMPORTFILES; i++) {
			if (firstempty == -1 && strlen(pref.last_import_files[i]) <= 0)
				firstempty = i;

			if (strlen(pref.last_import_files[i]) > 0 && pestrcasestr(find, pref.last_import_files[i])) { //already there
				break;
			}
		}
		if (i >= REMEMBERIMPORTFILES) {
			if (firstempty == -1) {
				//No empty slots , rotate.
				for (int ii = 0; ii < REMEMBERIMPORTFILES - 1; ii++) {
					strcpy(pref.last_import_files[ii], pref.last_import_files[ii + 1]);
				}
				strcpy(pref.last_import_files[REMEMBERIMPORTFILES - 1], find);
			}
			else
				strcpy(pref.last_import_files[firstempty], find);
		}
		extern bool bMarketplace_Init;
		bMarketplace_Init = false; //Make sure to update import list in marketplace.

		extern cstr sGotoPreviewWithFile;
		extern int iGotoPreviewType;
		//Open preview to set thumb.
		sGotoPreviewWithFile = find;
		iGotoPreviewType = 2;

	}

	#endif

}

void importer_handleScale ( void )
{
	//  Show or hide guide model
	if (  t.importerTabs[10].selected  ==  1 && g.tabmode  ==  IMPORTERTABPAGE1 ) 
	{
		ShowObject (  t.importer.dummyCharacterObjectNumber );
	}
	else
	{
		HideObject (  t.importer.dummyCharacterObjectNumber );
	}
	if (  t.importer.oldScale  !=  t.slidersmenuvalue[t.importer.properties1Index][1].value || t.importer.showScaleChange > 0 ) 
	{
		t.importer.message = ""; t.importer.message = t.importer.message + "Scale:" + Str(t.slidersmenuvalue[t.importer.properties1Index][1].value) + "%. Object displayed in relation to t.a typical character";
		t.importer.oldScale = t.slidersmenuvalue[t.importer.properties1Index][1].value;
		if (  t.importer.showScaleChange  <=  0  )  t.importer.showScaleChange  =  1;
		if (  t.inputsys.mclick  ==  0  )  --t.importer.showScaleChange;
	}
	t.tscale_f = t.importer.objectScaleForEditing;
	t.tScaleMultiplier_f = t.slidersmenuvalue[t.importer.properties1Index][1].value / 100.0;
	t.tscale_f = t.tscale_f / t.tScaleMultiplier_f;
	t.tscale_f = t.tscale_f * t.importer.camerazoom;
	ScaleObject (  t.importer.dummyCharacterObjectNumber , t.tscale_f , t.tscale_f , t.tscale_f * 0.2 );
}

void importer_draw_wicked(void)
{
	// Wicked engine import different from Classic/VRQ
}

void importer_draw ( void )
{
	#ifdef WICKEDENGINE
	importer_draw_wicked();
	#else

	#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	//	imgui_importer_draw();
	//	return;
	#endif

	//  draw to screen if active
	if ( t.importer.loaded == 1 && ImageExist(g.importermenuimageoffset+1) ) 
	{
		#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
		if (t.importer.scaleMulti == 1.0)
		{
			t.timg = g.importermenuimageoffset;
			t.tscrx = (GetDisplayWidth() - ImageWidth(t.timg + 1)) / 2;
			t.tscry = (GetDisplayHeight() / 2) - 300;
			PasteImage(t.timg + 1, t.tscrx - 4, t.tscry, 1);
			t.ttopy = t.tscry + ImageHeight(t.timg + 1);
			for (t.tlong = 0; t.tlong <= 4; t.tlong++)
			{
				PasteImage(t.timg + 2, t.tscrx - 2, t.ttopy, 1);
				t.ttopy += ImageHeight(t.timg + 2);
			}
			PasteImage(t.timg + 3, t.tscrx, t.ttopy, 1);
			pastebitmapfont(t.importer.message.Get(), (GetDisplayWidth() / 2) - (getbitmapfontwidth(t.importer.message.Get(), 1) / 2), t.tscry + 525, 1, 255);
			pastebitmapfont(t.importer.viewMessage.Get(), (GetDisplayWidth() / 2) - 295, t.tscry + 90, 1, 255);
		}
		else
		{
			pastebitmapfont(t.importer.message.Get(), (GetDisplayWidth() / 2) - (getbitmapfontwidth(t.importer.message.Get(), 1) / 2), 40, 1, 255);
			t.strwork = ""; t.strwork = t.strwork + "View: " + t.importer.viewMessage;
			pastebitmapfont(t.strwork.Get(), 10, 5, 1, 255);
		}
		#else
		if ( t.importer.scaleMulti == 1.0 ) 
		{
			t.timg=g.importermenuimageoffset;
			t.tscrx=(GetChildWindowWidth()-ImageWidth(t.timg+1))/2;
			t.tscry=(GetChildWindowHeight()/2) - 300;
			PasteImage (  t.timg+1,t.tscrx-4,t.tscry,1 );
			t.ttopy=t.tscry+ImageHeight(t.timg+1);
			for ( t.tlong = 0 ; t.tlong<=  4; t.tlong++ )
			{
				PasteImage (  t.timg+2,t.tscrx-2,t.ttopy,1 );
				t.ttopy += ImageHeight(t.timg+2);
			}
			PasteImage (  t.timg+3,t.tscrx,t.ttopy,1 );
			pastebitmapfont(t.importer.message.Get(),(GetChildWindowWidth()/2) - (getbitmapfontwidth (t.importer.message.Get(),1)/2),t.tscry+525,1,255);
			pastebitmapfont(t.importer.viewMessage.Get(),(GetChildWindowWidth()/2) - 295,t.tscry+90,1,255);
		}
		else
		{
			pastebitmapfont(t.importer.message.Get(),(GetChildWindowWidth()/2) - (getbitmapfontwidth (t.importer.message.Get(),1)/2),40,1,255);
			t.strwork = ""; t.strwork = t.strwork + "View: " + t.importer.viewMessage;
			pastebitmapfont( t.strwork.Get() ,10,5,1,255);
		}
		#endif

		// extra help information
		if ( t.importerTabs[1].selected == 1 )
		{
			LPSTR pExtraHelp = "Use W and S to raise camera view, and mouse wheel to zoom in and out";
			if ( strlen ( g_pShowFilenameHoveringOver ) > 0 ) pExtraHelp = g_pShowFilenameHoveringOver;
			#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
			pastebitmapfont(pExtraHelp, (GetDisplayWidth() / 2) - (getbitmapfontwidth(pExtraHelp, 1) / 2), 860, 1, 255);
			#else
			pastebitmapfont(pExtraHelp,(GetChildWindowWidth()/2) - (getbitmapfontwidth (pExtraHelp,1)/2),860,1,255);
			#endif
		}

		// Draw the sliders
		if (!bRemoveSprites) sliders_readall ( );

		// Slider management
		if (!bRemoveSprites) sliders_loop ( );

		//  Draw sliders last
		if (!bRemoveSprites) sliders_draw ( );
	}

	#endif // wicked endif
}

void importer_quit_for_reload (LPSTR pOptionalCopyModelFile)
{
	// fade out importer
	#ifdef WICKEDENGINE
	#else
	importer_fade_out ( );
	#endif

	#ifdef WICKEDENGINE
	// store last good imported model and return if optionalcopy ptr valid
	if (pOptionalCopyModelFile)
	{
		extern char pLaunchAfterSyncLastImportedModel[MAX_PATH];
		strcpy (pOptionalCopyModelFile, pLaunchAfterSyncLastImportedModel);
		t.importer.bQuitForReload = true;
	}
	
	#endif

	// free import object (switch to hide while wicked crash can happen)
	if ( ObjectExist(t.importer.objectnumber) == 1 )  HideObject (  t.importer.objectnumber );

	// dummy character model
	if (ObjectExist(t.importer.dummyCharacterObjectNumber))  HideObject(t.importer.dummyCharacterObjectNumber);

	// free importer resources
	importer_free ( );
	t.importer.importerActive = 0;

	// finish UI panel
	#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	bImporter_Window = false;
	bCenterRenderView = false;
	#ifndef WICKEDENGINE
	visuals_editordefaults();
	#endif
	t.visuals.refreshshaders = 1;
	visuals_loop();
	#endif
}

void importer_quit (void)
{
	importer_quit_for_reload(NULL);
}

void ConvertWorldToRelative ( sFrame* pFrame, GGMATRIX* pStoreNewPoseFrames, GGMATRIX* pTraverseMatrix )
{
	if ( pFrame )
	{
		// take the world bone from pStoreNewPoseFrames for this frame ID and create relative matrix from it
		int iFrameIndex = pFrame->iID;
		GGMATRIX matWorldBone = pStoreNewPoseFrames[iFrameIndex];

		// go through hierarchy, create matCombined as you go, transforming worldbones into relative bones
		float fDet = 0;
		GGMATRIX matInverseOfTraverse = *pTraverseMatrix;
		GGMatrixInverse ( &matInverseOfTraverse, &fDet, pTraverseMatrix );
		GGMATRIX matWorldBoneInBaseSpace;
		GGMatrixMultiply ( &matWorldBoneInBaseSpace, &matWorldBone, &matInverseOfTraverse );
		GGMATRIX matRelativeBone = matWorldBoneInBaseSpace;

		// finally 'restore' the relative bone!
		pFrame->matTransformed = matRelativeBone;

		// use relative bone to calculate world bones as we go
		GGMatrixMultiply ( &pFrame->matCombined, &pFrame->matTransformed, pTraverseMatrix );

		// convert child frames
		ConvertWorldToRelative ( pFrame->pChild, pStoreNewPoseFrames, &pFrame->matCombined );

		// convert sibling frames
		ConvertWorldToRelative ( pFrame->pSibling, pStoreNewPoseFrames, pTraverseMatrix );	
	}
}

#ifdef VRTECH
void importer_save_entity ( char *filename )
{
	//  Check if user folder exists, if not create it
	t.strwork = ""; t.strwork = t.strwork + t.importer.startDir + "\\entitybank\\user";
	if (  PathExist( t.strwork.Get() )  ==  0 ) 
	{
		MakeDirectory (  t.strwork.Get() );
	}

	cStr tOldDir = GetDir();

	//  Ask for save filename
	t.tSaveFile_s = "";
	t.timporterprotected = 1;
	#ifdef WICKEDENGINE
	t.timportermessage_s = "Save Object";
	#else
	t.timportermessage_s = "Save Entity";
	#endif
	while (  t.timporterprotected  ==  1 ) 
	{
		if (!filename) 
		{
			if (t.importer.fpeIsMainFile == 0)
			{
				t.strwork = ""; t.strwork = t.strwork + t.importer.startDir + "\\entitybank\\user";
				t.tSaveFile_s = openFileBox("Model (.dbo)|*.dbo|All Files|*.*|", t.strwork.Get(), t.timportermessage_s.Get(), ".dbo", IMPORTERSAVEFILE);
			}
			else
			{
				t.strwork = ""; t.strwork = t.strwork + t.importer.startDir + "\\entitybank\\user";
				t.tSaveFile_s = openFileBox("GG Entity (.fpe)|*.fpe|All Files|*.*|", t.strwork.Get(), t.timportermessage_s.Get(), ".fpe", IMPORTERSAVEFILE);
			}
		}
		else {
			t.strwork = filename;
			t.tSaveFile_s = filename;
		}
		if (  t.tSaveFile_s  ==  "Error" ) 
		{
			t.timportersaveon = 0;
			return;
		}
		t.timporterprotected = importer_check_if_protected(t.tSaveFile_s.Get());
		if (  t.timporterprotected  ==  1 ) 
		{
			t.timportermessage_s = "You cannot overwrite protected media, please choose an alternative name";
		}
	}

	#ifdef WICKEDENGINE
	// convert to real save file location
	char pRealSaveFile[MAX_PATH];
	strcpy(pRealSaveFile, t.tSaveFile_s.Get());
	GG_GetRealPath(pRealSaveFile, 1);
	t.tSaveFile_s = pRealSaveFile;
	#endif

	//  Ensure it has the dbo extension
	Dim (  t.tArray,300  );
	t.tArrayMarker = 0;
	t.tstring_s=t.tSaveFile_s;
	t.tToken_s=FirstToken(t.tstring_s.Get(),".");
	if (  t.tToken_s  !=  "" ) 
	{
		t.tArray[t.tArrayMarker] = t.tToken_s;
		++t.tArrayMarker;
	}
	do
	{
		t.tToken_s=NextToken(".");
		if (  t.tToken_s  !=  "" ) 
		{
			t.tArray[t.tArrayMarker] = t.tToken_s;
			++t.tArrayMarker;
		}
	} while ( !(  t.tToken_s == "" ) );
	t.tStippedFileName_s = "";
	int tCount = 0;
	for ( tCount = 0 ; tCount<=  t.tArrayMarker-2; tCount++ )
	{
		t.tStippedFileName_s = t.tStippedFileName_s + t.tArray[tCount];
	}
	UnDim (  t.tArray );
	t.tStippedFileName_s = t.tSaveFile_s;
	if (  strcmp ( Mid(t.tStippedFileName_s.Get(),Len(t.tStippedFileName_s.Get())-1)  ,  "." ) == 0 ) 
	{
		t.tStippedFileName_s = Left(t.tStippedFileName_s.Get(),Len(t.tStippedFileName_s.Get())-2);
	}
	else
	{
		t.tStippedFileName_s = Left(t.tStippedFileName_s.Get(),Len(t.tStippedFileName_s.Get())-4);
	}

	//  Grab the folder path
	Dim (  t.tArray,300  );
	t.tArrayMarker = 0;
	t.tstring_s=t.tStippedFileName_s;
	t.tToken_s=FirstToken(t.tstring_s.Get(),"\\");
	if (  t.tToken_s  !=  "" ) 
	{
		t.tArray[t.tArrayMarker] = t.tToken_s;
		++t.tArrayMarker;
	}
	do
	{
		t.tToken_s=NextToken("\\");
		if (  t.tToken_s  !=  "" ) 
		{
			t.tArray[t.tArrayMarker] = t.tToken_s;
			++t.tArrayMarker;
		}
	} while ( !(  t.tToken_s == "" ) );
	t.tStippedFileName_s = "";
	for ( tCount = 0 ; tCount<=  t.tArrayMarker-2; tCount++ )
	{
		t.tStippedFileName_s = t.tStippedFileName_s + t.tArray[tCount] + "\\";
	}
	t.tSavePath_s = t.tStippedFileName_s;

	//PE: We need to be in the destination folder.
	if (filename) {
		if (PathExist(t.tSavePath_s.Get()) == 0)
		{
			MakeDirectory(t.tSavePath_s.Get());
		}
		SetDir(t.tSavePath_s.Get());
	}

	//  Store file names
	if (  t.importer.fpeIsMainFile  ==  0 ) 
	{
		t.tSaveFile_s = t.tArray[t.tArrayMarker-1] + ".dbo";
		t.tSaveThumb_s = t.tArray[t.tArrayMarker-1] + ".BMP";
		t.importer.tFPESaveName = t.tArray[t.tArrayMarker-1] + ".fpe";
		t.importer.objectFPE.desc = t.tArray[t.tArrayMarker-1];
		t.importer.objectFPE.model = t.tSaveFile_s;
	}
	else
	{
		t.tSaveFile_s = t.importer.objectFilename;
		t.tSaveThumb_s = t.tArray[t.tArrayMarker-1] + ".BMP";
		t.importer.tFPESaveName = t.tArray[t.tArrayMarker-1] + ".fpe";
		t.importer.objectFPE.model = t.tSaveFile_s;
	}

	// Just before save object, ensure all texture references don't include root path
	sObject* pObject = GetObjectData ( t.importer.objectnumber );
	if ( pObject )
	{
		for ( int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++ )
		{
			sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
			if ( pMesh )
			{
				for ( int iTextureStage = 0; iTextureStage < pMesh->dwTextureCount; iTextureStage++ )
				{
					LPSTR pTexName = pMesh->pTextures[iTextureStage].pName;
					if ( strlen ( pTexName ) > 2 )
					{
						if ( pTexName[1] == ':' )
						{
							for ( int n = strlen(pTexName)-1; n > 0; n-- )
							{
								if ( pTexName[n] == '\\' || pTexName[n] == '/' )
								{
									strcpy ( pTexName, pTexName+n+1 );
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	// if selected 'Use Uber Anims', then rename skeleton and apply uber animations automatically
	if ( t.slidersmenuvalue[t.importer.properties1Index][7].value == 3 ) 
	{
		sObject* pObject = GetObjectData(t.importer.objectnumber);
		if ( pObject )
		{
			// load in correct Y pose and overwrite imported modes transform matrices
			GGMATRIX* pStoreNewPoseFrames = NULL;
			int objectnumberforframedatacopy = findFreeObject();
			cstr pAbsPathToUberAnimFile = g.fpscrootdir_s + "\\Files\\entitybank\\Characters\\2CHAINFINGER-YPOSE.dbo";//Uber Soldier.X";//appendanims.x";
			LoadObject ( pAbsPathToUberAnimFile.Get(), objectnumberforframedatacopy );
			if ( ObjectExist ( objectnumberforframedatacopy ) == 1 )
			{
				sObject* pObjectWithYPoseData = GetObjectData ( objectnumberforframedatacopy );
				if ( pObjectWithYPoseData )
				{
					// go through and find differences between poses
					for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
					{
						sFrame* pFrame = pObject->ppFrameList[iFrame];
						if ( pFrame )
						{
							LPSTR pFrameName = pFrame->szName;
							if ( pFrameName )
							{
								if ( strlen(pFrameName) > 0 )
								{
									// for this imported model frame, find the equivilant frame in the appendanim model (with the Y pose hidden in the skinning transform)
									for ( int iFindFrame = 0; iFindFrame < pObjectWithYPoseData->iFrameCount; iFindFrame++ )
									{
										sFrame* pFindFrame = pObjectWithYPoseData->ppFrameList[iFindFrame];
										if ( pFindFrame )
										{
											LPSTR pFindFrameName = pFindFrame->szName;
											if ( pFindFrameName )
											{
												if ( strlen(pFindFrameName) > 0 )
												{
													if ( stricmp ( pFrameName, pFindFrameName ) == NULL )
													{
														// just grab the local relative transform from the Y pose model
														pFrame->matOriginal = pFindFrame->matOriginal;
														pFrame->matTransformed = pFindFrame->matOriginal;

														// done with this findframe
														break;
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
				DeleteObject ( objectnumberforframedatacopy );
			}

			/* LB: moving this to JUST when the importer loads the file as then we can see the animation in the importer
			// go through all frames of imported model
			for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
			{
				sFrame* pFrame = pObject->ppFrameList[iFrame];
				if ( pFrame )
				{
					LPSTR pOldFrameName = pFrame->szName;
					if ( pOldFrameName )
					{
						if ( strlen(pOldFrameName) > 0 )
						{
							// look for common bone names, and transform to GG standard skeleton name
							const char* pNewName = "";
							if ( stricmp ( pOldFrameName, "mixamorig_Hips" ) == NULL ) pNewName = "Bip01_Pelvis";
							if ( stricmp ( pOldFrameName, "mixamorig_Spine" ) == NULL ) pNewName = "Bip01_Spine";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftUpLeg" ) == NULL ) pNewName = "Bip01_L_Thigh";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftLeg" ) == NULL ) pNewName = "Bip01_L_Calf";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftFoot" ) == NULL) pNewName = "Bip01_L_Foot";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftToeBase" ) == NULL ) pNewName = "Bip01_L_Toe0";
							if ( stricmp ( pOldFrameName, "mixamorig_RightUpLeg" ) == NULL ) pNewName = "Bip01_R_Thigh";
							if ( stricmp ( pOldFrameName, "mixamorig_RightLeg" ) == NULL ) pNewName = "Bip01_R_Calf";
							if ( stricmp ( pOldFrameName, "mixamorig_RightFoot" ) == NULL ) pNewName = "Bip01_R_Foot";
							if ( stricmp ( pOldFrameName, "mixamorig_RightToeBase" ) == NULL ) pNewName = "Bip01_R_Toe0";
							if ( stricmp ( pOldFrameName, "mixamorig_Spine1" ) == NULL ) pNewName = "Bip01_Spine1";
							if ( stricmp ( pOldFrameName, "mixamorig_Spine2" )== NULL ) pNewName = "Bip01_Spine2";
							if ( stricmp ( pOldFrameName, "mixamorig_Neck" ) == NULL ) pNewName = "Bip01_Neck";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftShoulder" ) == NULL ) pNewName = "Bip01_L_Clavicle";
							if ( stricmp ( pOldFrameName, "mixamorig_RightShoulder" ) == NULL ) pNewName = "Bip01_R_Clavicle";
							if ( stricmp ( pOldFrameName, "mixamorig_Head" ) == NULL ) pNewName = "Bip01_Head";
							if ( stricmp ( pOldFrameName, "mixamorig_HeadTop_End" ) == NULL ) pNewName = "Bip01_HeadTop";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftArm" ) == NULL ) pNewName = "Bip01_L_UpperArm";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftForeArm" ) == NULL ) pNewName = "Bip01_L_Forearm";
							if ( stricmp ( pOldFrameName, "mixamorig_RightArm" ) == NULL ) pNewName = "Bip01_R_UpperArm";
							if ( stricmp ( pOldFrameName, "mixamorig_RightForeArm" ) == NULL ) pNewName = "Bip01_R_Forearm";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftHand" ) == NULL ) pNewName = "Bip01_L_Hand";
							if ( stricmp ( pOldFrameName, "mixamorig_RightHand" ) == NULL ) pNewName = "Bip01_R_Hand";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandThumb1" ) > 0 ) pNewName = "Bip01_L_Finger0";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandThumb2" ) > 0 ) pNewName = "Bip01_L_Finger01";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandThumb3" ) > 0 ) pNewName = "Bip01_L_Finger02";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandThumb4" ) > 0 ) pNewName = "Bip01_L_Finger03";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandIndex1" ) > 0 ) pNewName = "Bip01_L_Finger1";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandIndex2" ) > 0 ) pNewName = "Bip01_L_Finger11";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandIndex3" ) > 0 ) pNewName = "Bip01_L_Finger12";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandIndex4" ) > 0 ) pNewName = "Bip01_L_Finger13";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandMiddle1" ) > 0 ) pNewName = "Bip01_L_Finger2";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandMiddle2" ) > 0 ) pNewName = "Bip01_L_Finger21";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandMiddle3" ) > 0 ) pNewName = "Bip01_L_Finger22";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandMiddle4" ) > 0 ) pNewName = "Bip01_L_Finger23";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandRing1" ) > 0 ) pNewName = "Bip01_L_Finger3";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandRing2" ) > 0 ) pNewName = "Bip01_L_Finger31";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandRing3" ) > 0 ) pNewName = "Bip01_L_Finger32";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandRing4" ) > 0 ) pNewName = "Bip01_L_Finger33";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandPinky1" ) > 0 ) pNewName = "Bip01_L_Finger4";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandPinky2" ) > 0 ) pNewName = "Bip01_L_Finger41";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandPinky3" ) > 0 ) pNewName = "Bip01_L_Finger42";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandPinky4" ) > 0 ) pNewName = "Bip01_L_Finger43";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandThumb1" ) > 0 ) pNewName = "Bip01_R_Finger0";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandThumb2" ) > 0 ) pNewName = "Bip01_R_Finger01";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandThumb3" ) > 0 ) pNewName = "Bip01_R_Finger02";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandThumb4" ) > 0 ) pNewName = "Bip01_R_Finger03";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandIndex1" ) > 0 ) pNewName = "Bip01_R_Finger1";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandIndex2" ) > 0 ) pNewName = "Bip01_R_Finger11";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandIndex3" ) > 0 ) pNewName = "Bip01_R_Finger12";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandIndex4" ) > 0 ) pNewName = "Bip01_R_Finger13";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandMiddle1" ) > 0 ) pNewName = "Bip01_R_Finger2";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandMiddle2" ) > 0 ) pNewName = "Bip01_R_Finger21";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandMiddle3" ) > 0 ) pNewName = "Bip01_R_Finger22";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandMiddle4" ) > 0 ) pNewName = "Bip01_R_Finger23";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandRing1" ) > 0 ) pNewName = "Bip01_R_Finger3";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandRing2" ) > 0 ) pNewName = "Bip01_R_Finger31";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandRing3" ) > 0 ) pNewName = "Bip01_R_Finger32";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandRing4" ) > 0 ) pNewName = "Bip01_R_Finger33";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandPinky1" ) > 0 ) pNewName = "Bip01_R_Finger4";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandPinky2" ) > 0 ) pNewName = "Bip01_R_Finger41";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandPinky3" ) > 0 ) pNewName = "Bip01_R_Finger42";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandPinky4" ) > 0 ) pNewName = "Bip01_R_Finger43";

							// replace all instances of this name (if found)
							if ( strlen(pNewName) > 0 ) 
							{
								// rename animation reference name too
								if ( pObject->pAnimationSet )
								{
									sAnimation* pCurrent = pObject->pAnimationSet->pAnimation;
									while(pCurrent)
									{
										if ( pCurrent->szName )
										{
											if ( stricmp ( pCurrent->szName, pFrame->szName ) == NULL )
											{
												strcpy ( pCurrent->szName, pNewName );
												break;
											}
										}
										pCurrent = pCurrent->pNext;
									}
								}

								// also rename bone names within each mesh
								for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
								{
									sMesh* pMesh = pObject->ppMeshList[iMesh];
									if ( pMesh )
									{
										if ( pMesh->pBones )
										{
											for ( int iBone = 0; iBone < pMesh->dwBoneCount; iBone++ )
											{
												if ( pMesh->pBones [ iBone ].szName )
												{
													if ( stricmp ( pMesh->pBones [ iBone ].szName, pFrame->szName ) == NULL )
													{
														strcpy ( pMesh->pBones [ iBone ].szName, pNewName );
														break;
													}
												}								
											}
										}
									}
								}

								// finally rename frame
								strcpy ( pFrame->szName, pNewName );
							}
						}
					}
				}
			}
			*/

			// now insert Bip01 into the frame hierarchy between Sceene Root and Pelvis
			for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
			{
				sFrame* pFindPelvisFrame = pObject->ppFrameList[iFrame];
				if ( pFindPelvisFrame )
				{
					LPSTR pFrameName = pFindPelvisFrame->szName;
					if ( pFrameName )
					{
						if ( strlen(pFrameName) > 0 )
						{
							if ( stricmp ( pFrameName, "Bip01_Pelvis" ) == NULL )
							{
								// create new Bip01 frame
								sFrame* pPelvisParent_SceneRoot = pFindPelvisFrame->pParent;
								sFrame* pBip01Frame = new sFrame();

								// insert it between scene root and pelvis
								strcpy ( pBip01Frame->szName, "Bip01" );
								pPelvisParent_SceneRoot->pChild = pBip01Frame;
								pBip01Frame->pParent = pPelvisParent_SceneRoot;
								pFindPelvisFrame->pParent = pBip01Frame;
								pBip01Frame->pChild = pFindPelvisFrame;

								// also create a new Animation object for this new Bip01 frame (so appended anim data can go somewhere (below))
								if ( pObject->pAnimationSet )
								{
									sAnimation* pLastAnim = pObject->pAnimationSet->pAnimation;
									if ( pLastAnim )
									{
										while ( pLastAnim->pNext ) pLastAnim = pLastAnim->pNext;
									}
									if ( pLastAnim )
									{
										sAnimation* pNewAnim = new sAnimation();
										pNewAnim->bLinear = 1;
										pNewAnim->pFrame = pBip01Frame;
										strcpy ( pNewAnim->szName, "Bip01" );
										pLastAnim->pNext = pNewAnim;
									}
								}

								// we are done
								break;
							}
						}
					}
				}
			}

			// and insert FIRESPOT limb to the right hand with default values
			for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
			{
				sFrame* pFindRightHandFrame = pObject->ppFrameList[iFrame];
				if ( pFindRightHandFrame )
				{
					LPSTR pFrameName = pFindRightHandFrame->szName;
					if ( pFrameName )
					{
						if ( strlen(pFrameName) > 0 )
						{
							if ( stricmp ( pFrameName, "Bip01_R_Hand" ) == NULL )
							{
								// create new FIRESPOT frame
								sFrame* pFIRESPOTFrame = new sFrame();

								// insert it as child of right hand
								sFrame* pLastChildSybling = pFindRightHandFrame->pChild;
								while (pLastChildSybling->pSibling) pLastChildSybling = pLastChildSybling->pSibling;
								strcpy ( pFIRESPOTFrame->szName, "FIRESPOT" );
								pLastChildSybling->pSibling = pFIRESPOTFrame;
								pFIRESPOTFrame->pParent = pFindRightHandFrame;

								// also create a new Animation object for this new Bip01 frame (so appended anim data can go somewhere (below))
								if ( pObject->pAnimationSet )
								{
									sAnimation* pLastAnim = pObject->pAnimationSet->pAnimation;
									if ( pLastAnim )
									{
										while ( pLastAnim->pNext ) pLastAnim = pLastAnim->pNext;
									}
									if ( pLastAnim )
									{
										sAnimation* pNewAnim = new sAnimation();
										pNewAnim->bLinear = 1;
										pNewAnim->pFrame = pFIRESPOTFrame;
										strcpy ( pNewAnim->szName, "FIRESPOT" );
										pLastAnim->pNext = pNewAnim;
									}
								}

								// we are done
								break;
							}
						}
					}
				}
			}

			// now we need to change the model geometry from a T bone to a Y pose (using the relative local transforms we generated above)
			GGMATRIX matrix;
			if ( 1 )
			{
				GGMatrixIdentity ( &matrix );
				UpdateFrame ( pObject->pFrame, &matrix );
				for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
				{
					// wipe out original vertex data
					sMesh* pMesh = pObject->ppMeshList[iMesh];
					if ( pMesh->pOriginalVertexData ) SAFE_DELETE ( pMesh->pOriginalVertexData );

					// animate mesh on CPU
					AnimateBoneMeshBONE ( pObject, NULL, pMesh );

					// now record new vertex mesh shape
					if ( pMesh->pOriginalVertexData ) SAFE_DELETE ( pMesh->pOriginalVertexData );
					DWORD dwTotalVertSize = pMesh->dwVertexCount * pMesh->dwFVFSize;
					pMesh->pOriginalVertexData = (BYTE*)new char [ dwTotalVertSize ];
					memcpy ( pMesh->pOriginalVertexData, pMesh->pVertexData, dwTotalVertSize );
				}
			}

			// now replace frame transform matrices in imported model (copied from appendanims.x)
			// which zeros the rotation and puts model in Y shape pose (not T) ready for stock 
			// animation data (but retains skinning information for the unique mesh geometry of the model)
			objectnumberforframedatacopy = findFreeObject();
			pAbsPathToUberAnimFile = g.fpscrootdir_s + "\\Files\\entitybank\\Characters\\appendanims.x";
			LoadObject ( pAbsPathToUberAnimFile.Get(), objectnumberforframedatacopy );
			if ( ObjectExist ( objectnumberforframedatacopy ) == 1 )
			{
				sObject* pObjectWithGoodFrameData = GetObjectData ( objectnumberforframedatacopy );
				if ( pObjectWithGoodFrameData )
				{
					// go through all frames of imported object, match up name and copy frame transform matrix over from 'framegood' model
					for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
					{
						sFrame* pFrame = pObject->ppFrameList[iFrame];
						if ( pFrame )
						{
							LPSTR pFrameName = pFrame->szName;
							if ( pFrameName )
							{
								if ( strlen(pFrameName) > 0 )
								{
									// check to find this name in framegood model
									for ( int iFrameGood = 0; iFrameGood < pObjectWithGoodFrameData->iFrameCount; iFrameGood++ )
									{
										sFrame* pFrameGood = pObjectWithGoodFrameData->ppFrameList[iFrameGood];
										if ( pFrameGood )
										{
											LPSTR pFrameGoodName = pFrameGood->szName;
											if ( pFrameGoodName )
											{
												if ( strlen(pFrameGoodName) > 0 )
												{
													// check if we have a match
													if ( stricmp ( pFrameName, pFrameGoodName ) == NULL )
													{
														// now copy the frame data to the import model
														pFrame->matOriginal = pFrameGood->matOriginal;
														pFrame->matTransformed = pFrameGood->matTransformed;
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
				DeleteObject ( objectnumberforframedatacopy );
			}

			// and restore transformed matrix from originals
			for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
			{
				sFrame* pFrame = pObject->ppFrameList[iFrame];
				if ( pFrame )
				{
					pFrame->matTransformed = pFrame->matOriginal;
				}
			}

			// delete all keys so append can be fresh (and specifically remove matrix keys which WILL mess up overall animation)
			if ( pObject->pAnimationSet )
			{
				sAnimationSet* pAnimSet = pObject->pAnimationSet;
				while ( pAnimSet != NULL )
				{
					sAnimation* pAnim = pAnimSet->pAnimation;
					while ( pAnim != NULL )
					{
						// scans all animation data and creates the interpolation vectors between all keyframes (vital)
						if ( pAnim )
						{
							SAFE_DELETE(pAnim->pPositionKeys);
							pAnim->dwNumPositionKeys=0;
							SAFE_DELETE(pAnim->pRotateKeys);
							pAnim->dwNumRotateKeys=0;
							SAFE_DELETE(pAnim->pScaleKeys);
							pAnim->dwNumScaleKeys=0;
							SAFE_DELETE(pAnim->pMatrixKeys);
							pAnim->dwNumMatrixKeys=0;
						}
						pAnim = pAnim->pNext;
					}
					pAnimSet = pAnimSet->pNext;
				}
			}

			// append uber animations to character
			AppendAnimationFromFile ( pObject, pAbsPathToUberAnimFile.Get(), 0 );

			// now update model to first frame animation pose (to calculate combined matrix from pose)
			GGMatrixIdentity ( &matrix );
			UpdateAllFrameData ( pObject, 0.0f );
			UpdateFrame ( pObject->pFrame, &matrix );

			// and then calculate the inverse of those matCombined transforms to apply to the bone matTransforms
			// so bone matrix cancels out first frame pose leaving matCombined animations to shape shader verts
			for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
			{
				sMesh* pMesh = pObject->ppMeshList[iMesh];
				if ( pMesh )
				{
					sBone* pBones = pMesh->pBones;
					if ( pBones )
					{
						for ( int iBone = 0; iBone < pMesh->dwBoneCount; iBone++ )
						{
							float fDet = 0.0f;
							GGMatrixInverse ( &pBones[iBone].matTranslation, &fDet, pMesh->pFrameMatrices [ iBone ] );
						}
					}
				}
			}
		}
	}

	// Save Object
	if ( strcmp ( Lower(Right(t.tSaveFile_s.Get(),4)) , ".dbo" ) == 0 ) 
	{
		if ( FileExist (t.tSaveFile_s.Get()) == 1 ) DeleteAFile ( t.tSaveFile_s.Get() ) ;
		ScaleObject (  t.importer.objectnumber,100,100,100 );
		SaveObject (  t.tSaveFile_s.Get(),t.importer.objectnumber );
		ScaleObject (  t.importer.objectnumber,t.importer.objectScaleForEditing,t.importer.objectScaleForEditing,t.importer.objectScaleForEditing );
	}
	else
	{
		if (  t.importer.objectFileOriginalPath+t.importer.objectFilename  !=  t.tSavePath_s+t.importer.objectFilename ) 
		{
			t.strwork = ""; t.strwork = t.strwork +t.importer.objectFileOriginalPath+t.importer.objectFilename;
			if (  FileExist( t.strwork.Get() ) == 1 ) 
			{
				t.strwork = ""; t.strwork = t.strwork +t.tSavePath_s+t.importer.objectFilename;
				if (  FileExist( t.strwork.Get() ) == 0 ) 
				{
					t.strwork = ""; t.strwork = t.strwork +t.importer.objectFileOriginalPath+t.importer.objectFilename;
					cstr string1 = t.tSavePath_s+t.importer.objectFilename;
					char pRealSaveModelFile[MAX_PATH];
					strcpy(pRealSaveModelFile, string1.Get());
					GG_GetRealPath(pRealSaveModelFile, 1);
					CopyAFile ( t.strwork.Get() , string1.Get() );
				}
			}
		}
	}
	UnDim (  t.tArray );

	#ifdef WICKEDENGINE
	t.tcounttextures = 0;
	sMesh * pMesh = NULL;
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						int tCount = 1;

						if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].resource)
						{
							cStr TextureFilename = (char *) pObjectMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].name.c_str();
							int iInsertedAtSlot = importer_addtexturefiletolist(TextureFilename, TextureFilename, &t.tcounttextures);
							t.importerTextures[iInsertedAtSlot].imageID = 1; //Fake id. Reset later.
						}

						if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::NORMALMAP].resource)
						{
							cStr TextureFilename = (char *)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::NORMALMAP].name.c_str();
							int iInsertedAtSlot = importer_addtexturefiletolist(TextureFilename, TextureFilename, &t.tcounttextures);
							t.importerTextures[iInsertedAtSlot].imageID = 1; //Fake id. Reset later.
						}

						if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].resource)
						{
							cStr TextureFilename = (char *)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].name.c_str();
							int iInsertedAtSlot = importer_addtexturefiletolist(TextureFilename, TextureFilename, &t.tcounttextures);
							t.importerTextures[iInsertedAtSlot].imageID = 1; //Fake id. Reset later.
						}

						if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].resource)
						{
							cStr TextureFilename = (char *)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].name.c_str();
							int iInsertedAtSlot = importer_addtexturefiletolist(TextureFilename, TextureFilename, &t.tcounttextures);
							t.importerTextures[iInsertedAtSlot].imageID = 1; //Fake id. Reset later.
						}

						if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::EMISSIVEMAP].resource)
						{
							cStr TextureFilename = (char *)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::EMISSIVEMAP].name.c_str();
							int iInsertedAtSlot = importer_addtexturefiletolist(TextureFilename, TextureFilename, &t.tcounttextures);
							t.importerTextures[iInsertedAtSlot].imageID = 1; //Fake id. Reset later.
						}
						if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].resource)
						{
							cStr TextureFilename = (char *)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].name.c_str();
							int iInsertedAtSlot = importer_addtexturefiletolist(TextureFilename, TextureFilename, &t.tcounttextures);
							t.importerTextures[iInsertedAtSlot].imageID = 1; //Fake id. Reset later.
						}
					}
				}
			}
		}
	}
	#endif

	Dim ( t.tArray2, 220 );
	for ( tCount = 1; tCount <= IMPORTERTEXTURESMAX; tCount++ )
	{
		// skip files that do not exist
		if ( t.importerTextures[tCount].imageID == 0 )  
			continue;

		//  Split the filename into tokens to grab the path
		t.tSourceName_s = t.importerTextures[tCount].fileName;
		t.tArrayMarker = 0;
		t.tstring_s=t.tSourceName_s;
		t.tToken_s=FirstToken(t.tstring_s.Get(),"\\");
		if (  t.tToken_s  !=  "" ) 
		{
			t.tArray2[t.tArrayMarker] = t.tToken_s;
			++t.tArrayMarker;
		}
		do
		{
			t.tToken_s=NextToken("\\");
			if (  t.tToken_s  !=  "" ) 
			{
				t.tArray2[t.tArrayMarker] = t.tToken_s;
				++t.tArrayMarker;
			}
		} while ( !(  t.tToken_s == "" ) );

		if (t.tArrayMarker <= 0)
			continue;

		//  Now store just the file name
		t.tDestFileName = t.tSavePath_s + t.tArray2[t.tArrayMarker-1];

		//If source have subdirs , remove them from dest.
		char tmp[MAX_PATH];
		strcpy(tmp, t.tArray2[t.tArrayMarker - 1].Get() );
		if (pestrcasestr(tmp, "\\") || pestrcasestr(tmp, "/"))
		{
			int pos = strlen(tmp);
			while (pos > 0 && tmp[pos] != '\\' && tmp[pos] != '/') pos--;
			if (pos > 0) {
				strcpy(tmp, &tmp[pos + 1]);
				t.tDestFileName = t.tSavePath_s + tmp;
			}
		}

		if ( tCount == 1 )
		{
			// first texture in list is the 'color/diffuse' one (even if multitexture)
			t.importer.objectFPE.textured = t.tArray2[t.tArrayMarker-1];
		}

		//  Copy and save images to destination
		char pRealDestFileName[MAX_PATH];
		#ifndef WICKEDENGINE
		if (  ImageExist(t.importerTextures[tCount].imageID) == 1 ) 
		#endif
		{
			strcpy(pRealDestFileName, t.tDestFileName.Get());
			GG_GetRealPath(pRealDestFileName, 1);
			cstr test1 = ""; test1 = test1 + g.fpscrootdir_s + "\\Files\\" + t.importerTextures[tCount].fileName;
			if (  FileExist (t.importerTextures[tCount].fileName.Get())  ==  1 && t.tDestFileName  !=  test1 && t.tDestFileName  !=  t.importerTextures[tCount].fileName ) 
			{
				if (  FileExist(pRealDestFileName)  ==  0 ) 
				{
					CopyAFile ( t.importerTextures[tCount].fileName.Get(), pRealDestFileName );
				}
			}
			else
			{
				t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.importerTextures[tCount].fileName;
				cstr test2 = ""; test2 = test2 + g.fpscrootdir_s + "\\Files\\" + t.importerTextures[tCount].fileName;
				if (  FileExist ( t.strwork.Get() )  ==  1 && t.tDestFileName  !=  test2 ) 
				{
					if (  FileExist(pRealDestFileName)  ==  0 ) 
					{
						t.strwork = "" ; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.importerTextures[tCount].fileName;
						CopyAFile ( t.strwork.Get() , pRealDestFileName );
					}
				}
				#ifndef WICKEDENGINE
				else
				{
					t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.importerTextures[tCount].fileName;
					if (  t.tDestFileName  !=  t.strwork ) 
					{
						if (  ImageExist (t.importerTextures[tCount].imageID) ) 
						{
							if (  FileExist(t.tDestFileName.Get())  ==  0 ) 
							{
								SaveImage (  t.tDestFileName.Get() , t.importerTextures[tCount].imageID );
							}
						}
					}
				}
				#endif
			}
		}

		// Check for Normal and Specular and copy those if they exist
		if ( Len(t.importerTextures[tCount].fileName.Get()) > 6 ) 
		{
			if ( strcmp ( Lower(Right(t.importerTextures[tCount].fileName.Get(),6)) , "_d.dds" ) == 0 || strcmp ( Lower(Right(t.importerTextures[tCount].fileName.Get(),6)) , "_d.png" ) == 0 ) 
			{
				//  Normal map first
				t.tnormalmapfile_s = ""; t.tnormalmapfile_s = t.tnormalmapfile_s + Left(t.importerTextures[tCount].fileName.Get(),Len(t.importerTextures[tCount].fileName.Get())-6) + "_N.dds";
				t.tDestFileName = t.tSavePath_s + t.tArray2[t.tArrayMarker-1];
				cstr TempFileName = t.tDestFileName;
				t.tDestFileName = "" ; t.tDestFileName = t.tDestFileName + Left(TempFileName.Get(),Len(TempFileName.Get())-6) + "_N.dds";
				strcpy(pRealDestFileName, t.tDestFileName.Get());
				GG_GetRealPath(pRealDestFileName, 1);
				t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
				if ( FileExist (t.tnormalmapfile_s.Get())  ==  1 && t.tDestFileName  !=  t.strwork && t.tDestFileName  !=  t.tnormalmapfile_s )
				{
					if ( FileExist(pRealDestFileName)  ==  0 ) 
					{
						CopyAFile ( t.tnormalmapfile_s.Get(), pRealDestFileName );
					}
				}
				else
				{
					t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					if (  FileExist ( t.strwork.Get() )  ==  1 && t.tDestFileName  !=  g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s ) 
					{
						if ( FileExist(pRealDestFileName)  ==  0 ) 
						{
							t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
							CopyAFile ( t.strwork.Get(), pRealDestFileName );
						}
					}
				}

				//  Normal map continued
				t.tnormalmapfile_s = ""; t.tnormalmapfile_s = t.tnormalmapfile_s + Left(t.importerTextures[tCount].fileName.Get(),Len(t.importerTextures[tCount].fileName.Get())-6) + "_N.png";
				t.tDestFileName = t.tSavePath_s + t.tArray2[t.tArrayMarker-1];
				TempFileName = t.tDestFileName;
				t.tDestFileName = "" ; t.tDestFileName = t.tDestFileName + Left(TempFileName.Get(),Len(TempFileName.Get())-6) + "_N.png";
				strcpy(pRealDestFileName, t.tDestFileName.Get());
				GG_GetRealPath(pRealDestFileName, 1);
				t.strwork = ""; t.strwork = t.strwork + "\\Files\\" + t.tnormalmapfile_s;
				if (  FileExist (t.tnormalmapfile_s.Get())  ==  1 && t.tDestFileName  !=  g.fpscrootdir_s + t.strwork && t.tDestFileName  !=  t.tnormalmapfile_s )  
				{
					if (  FileExist(pRealDestFileName)  ==  0 ) 
					{
						CopyAFile (  t.tnormalmapfile_s.Get() ,pRealDestFileName );
					}
				}
				else
				{

					cstr work1 = ""; work1 = work1 + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					if (  FileExist ( work1.Get() )  ==  1 && t.tDestFileName  !=  t.strwork )  
					{
						if (  FileExist(pRealDestFileName)  ==  0 ) 
						{
							t.strwork = "" ; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
							CopyAFile (  t.strwork.Get() , pRealDestFileName );
						}
					}
				}

				//  Specular now
				t.tnormalmapfile_s = "" ; t.tnormalmapfile_s = t.tnormalmapfile_s + Left(t.importerTextures[tCount].fileName.Get(),Len(t.importerTextures[tCount].fileName.Get())-6) + "_S.dds";
				t.tDestFileName = t.tSavePath_s + t.tArray2[t.tArrayMarker-1];
				TempFileName = t.tDestFileName;
				t.tDestFileName = "" ; t.tDestFileName = t.tDestFileName + Left (TempFileName.Get(),Len(TempFileName.Get())-6) + "_S.dds";
				strcpy(pRealDestFileName, t.tDestFileName.Get());
				GG_GetRealPath(pRealDestFileName, 1);
				t.strwork = "" ; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
				if (  FileExist (t.tnormalmapfile_s.Get())  ==  1 && t.tDestFileName  !=  t.strwork && t.tDestFileName  !=  t.tnormalmapfile_s ) 
				{
					if (  FileExist(pRealDestFileName)  ==  0 ) 
					{
						CopyAFile (  t.tnormalmapfile_s.Get() , pRealDestFileName );
					}
				}
				else
				{
					cstr work2 = ""; work2 = work2 + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					if (  FileExist ( work2.Get() )  ==  1 && t.tDestFileName  !=  t.strwork ) 
					{
						if (  FileExist(pRealDestFileName)  ==  0 ) 
						{
							t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
							CopyAFile (  t.strwork.Get() ,pRealDestFileName );
						}
					}
				}

				t.tnormalmapfile_s = ""; t.tnormalmapfile_s = t.tnormalmapfile_s + Left(t.importerTextures[tCount].fileName.Get(),Len(t.importerTextures[tCount].fileName.Get())-6) + "_S.png";
				t.tDestFileName = t.tSavePath_s + t.tArray2[t.tArrayMarker-1];
				TempFileName = t.tDestFileName;
				t.tDestFileName = ""; t.tDestFileName = t.tDestFileName + Left(TempFileName.Get(),Len(TempFileName.Get())-6) + "_S.png";
				strcpy(pRealDestFileName, t.tDestFileName.Get());
				GG_GetRealPath(pRealDestFileName, 1);
				t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
				if (  FileExist (t.tnormalmapfile_s.Get())  ==  1 && t.tDestFileName  !=  t.strwork && t.tDestFileName  !=  t.tnormalmapfile_s ) 
				{
					if (  FileExist(pRealDestFileName)  ==  0 ) 
					{
						CopyAFile (  t.tnormalmapfile_s.Get(),pRealDestFileName );
					}
				}
				else
				{
					cstr work3 = ""; work3 = work3 + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					t.strwork = "" ; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					if (  FileExist ( work3.Get() )  ==  1 && t.tDestFileName  !=  t.strwork ) 
					{
						if (  FileExist(pRealDestFileName )  ==  0 ) 
						{
							t.strwork = "" ; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
							CopyAFile (  t.strwork.Get() ,pRealDestFileName );
						}
					}
				}
			}
		}
	}
	UnDim (  t.tArray2 );

	// If model has MORE than one texture, blank out FPE textured
	if ( t.tcounttextures > 1 ) 
	{
		// but make sure they are 'different' textures, not just DNS/PBR sets
		LPSTR pFile = t.importerTextures[1].fileName.Get();
		LPSTR pExt = "_d.png";
		if ( strnicmp ( pFile+strlen(pFile)-10, "_color.png", 10 ) == NULL ) pExt = "_color.png";
		if ( strnicmp ( pFile+strlen(pFile)-12, "_diffuse.png", 12 ) == NULL ) pExt = "_diffuse.png";
		if ( strnicmp ( pFile+strlen(pFile)-11, "_albedo.png", 11 ) == NULL ) pExt = "_albedo.png";
		if ( strnicmp ( pFile+strlen(pFile)-10, "_color.dds", 10 ) == NULL ) pExt = "_color.dds";
		if ( strnicmp ( pFile+strlen(pFile)-12, "_diffuse.dds", 12 ) == NULL ) pExt = "_diffuse.dds";
		if ( strnicmp ( pFile+strlen(pFile)-11, "_albedo.dds", 11 ) == NULL ) pExt = "_albedo.dds";
		cstr pBaseFilePart = Left(pFile,Len(pFile)-strlen(pExt));
		for ( int tCount = 2; tCount <= 10; tCount++ )
		{
			LPSTR pCompareFile = t.importerTextures[tCount].fileName.Get();
			if ( strlen ( pCompareFile ) > 4 )
			{
				if ( strnicmp ( pCompareFile, pBaseFilePart.Get(), strlen(pBaseFilePart.Get()) ) == NULL )
				{
					// matches base part, now exclude files of known extension
					bool bExclude = false;
					char pRest[1024];
					strcpy ( pRest, pCompareFile + strlen(pBaseFilePart.Get()) );
					pRest[strlen(pRest)-4] = 0;
					if ( stricmp ( pRest, "_normal" ) == NULL ) bExclude = true;
					if ( stricmp ( pRest, "_specular" ) == NULL ) bExclude = true;
					if ( stricmp ( pRest, "_metalness" ) == NULL ) bExclude = true;
					if ( stricmp ( pRest, "_gloss" ) == NULL ) bExclude = true;
					if ( stricmp ( pRest, "_ao" ) == NULL ) bExclude = true;
					if ( stricmp ( pRest, "_height" ) == NULL ) bExclude = true;
					if ( stricmp ( pRest, "_cube" ) == NULL ) bExclude = true;
					if ( bExclude == false )
					{
						// found a match to base texture, but unknown extra part
						t.importer.objectFPE.textured="";
						break;
					}
				}
				else
				{
					// found a different base texture, cannot be a single textured model
					t.importer.objectFPE.textured="";
					break;
				}
			}
		}
	}


	#ifdef WICKEDENGINE
	//PE: WICKED
	//t.importerTextures[tCount].fileName;
	//	for ( int tCount = 1 ; tCount <= IMPORTERTEXTURESMAX; tCount++ )
	//for single frames.
	//t.importer.objectFPE.textured
	//t.tcounttextures
	//PE: Get first mesh.
	//t.importer.objectFPE.textured =
	#endif

	// save FPE file
	importer_save_fpe ( );

	#ifdef WICKEDENGINE
	//Reset t.importerTextures[tCount].fileName
	for (int tCount = 1; tCount <= t.tcounttextures; tCount++)
	{
		t.importerTextures[tCount].fileName = "";
		t.importerTextures[tCount].imageID = 0;
	}
	t.tcounttextures = 0;

	Sleep(1000);
	//wiJobSystem::context ctx;
	//wiJobSystem::Wait(ctx);
	#endif

	// Save/Generate Thumbnail BMP
	char pRealSavethumb[MAX_PATH];
	strcpy(pRealSavethumb, t.tSaveThumb_s.Get());
	GG_GetRealPath(pRealSavethumb, 1);
	t.strwork = ""; t.strwork = t.strwork + t.importer.objectFileOriginalPath + t.importer.objectFilename;
	cstr pSourceBMP = cstr ( cstr(Left ( t.strwork.Get(), strlen(t.strwork.Get())-4)) + ".bmp" );
	if ( FileExist ( pSourceBMP.Get() ) ) 
	{
		// if BMP already existed in source area, use that one
		if ( FileExist ( pRealSavethumb ) == 1 ) DeleteAFile ( pRealSavethumb );
		CopyFileA ( pSourceBMP.Get(), pRealSavethumb, FALSE );
	}
	else
	{
		//PE: IMPORTER_TMP_IMAGE is the same as the importer object use as texture.
		//PE: Use IMPORTER_TMP_IMAGE+1 (same as char kit). so importer object dont disapear.
		int grab_image = IMPORTER_TMP_IMAGE;
		#ifdef WICKEDENGINE
			grab_image = IMPORTER_TMP_IMAGE+1;
		#endif

		if (GetImageExistEx(grab_image))
		{
			//Already generated so.
			SaveImage(pRealSavethumb, grab_image);
		}
		else 
		{
			if (FileExist(pRealSavethumb) == 1) DeleteAFile(pRealSavethumb);
			import_generate_thumb();
			SaveImage(pRealSavethumb, grab_image);
		}
		DeleteImage(grab_image);
	}

	#ifdef WICKEDENGINE
	// All this will be replaced I thinks
	#else
	t.aspect_f=GetDesktopWidth() ; t.aspect_f=t.aspect_f/GetDesktopHeight();
	SetCameraAspect (  t.aspect_f );
	SetCurrentBitmap (  0 );
	importer_update_selection_markers ( );
	importer_draw ( );
	importer_tabs_draw ( );
	Sync (  );
	#endif
	t.importer.cancel = 1;
	t.timportersaveon = 0;

	if (filename) {
		SetDir(tOldDir.Get());
	}
}

void import_generate_thumb(void)
{
	#ifdef WICKEDENGINE
	// Need a way to generate a thumbnail in Wicked Engine
	char pFullPathTempThumb[MAX_PATH];
	strcpy(pFullPathTempThumb, g.fpscrootdir_s.Get());
	strcat(pFullPathTempThumb, "\\Files\\editors\\uiv3\\ThumbnailTemplate.png");
	image_setlegacyimageloading(true);
	LoadImage(pFullPathTempThumb, IMPORTER_TMP_IMAGE+1); //PE: Same as char kit in wicked.
	image_setlegacyimageloading(false);
	#else
	// generate new BMP thumb from scratch
	CreateBitmap(32, 64, 64);
	SetCurrentBitmap(32);
	SetCameraAspect(1.0);

	//  Make a white background Box (  )
	t.twhiteobj = 1;
	t.tfound = 0;

	//  Find a free object number
	while (t.tfound == 0)
	{
		if (ObjectExist(t.twhiteobj) == 1)
		{
			++t.twhiteobj;
		}
		else
		{
			t.tfound = 1;
		}
	}

	MakeObjectBox(t.twhiteobj, 200000, 200000, 1);
	SetObjectLight(t.twhiteobj, 0);
	LockObjectOn(t.twhiteobj);
	PositionObject(t.twhiteobj, 0, 0, 3000);
	SetObjectEffect(t.twhiteobj, g.guishadereffectindex);
	SetObjectEmissive(t.twhiteobj, Rgb(255, 255, 255));

	t.tOldSelected = t.importerTabs[1].selected;

	if (!bRemoveSprites) HideSprite(t.importer.helpSprite2);
	if (!bRemoveSprites) HideSprite(t.importer.helpSprite3);
	if (SpriteExist(t.importer.helpSprite4))  DeleteSprite(t.importer.helpSprite4);

	t.importerTabs[1].selected = 1;
	importer_update_selection_markers();
	RotateObject(t.importerGridObject[8], 0, 0, 0);
	PositionObject(t.importerGridObject[8], 0, 0, IMPORTERZPOSITION);

#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	PositionObject(t.importerGridObject[8], 0, 0, IMPORTERZPOSITION * 5);
#endif

	RotateObject(t.importerGridObject[8], t.slidersmenuvalue[t.importer.properties3Index][1].value, t.slidersmenuvalue[t.importer.properties3Index][2].value, t.slidersmenuvalue[t.importer.properties3Index][3].value);
	SetAlphaMappingOn(t.importer.objectnumber, 255);
	HideObject(t.importer.dummyCharacterObjectNumber);
	t.importerTabs[1].selected = t.tOldSelected;
	for (t.tc = 1; t.tc <= 7; t.tc++)
	{
		HideObject(t.importerGridObject[t.tc]);
	}
	bool b6002 = false; if (ObjectExist(6002) == 1) { b6002 = GetVisible(6002); HideObject(6002); }
	bool b6003 = false; if (ObjectExist(6003) == 1) { b6003 = GetVisible(6003); HideObject(6003); }
	Sync();
	if (b6002 == true) ShowObject(6002);
	if (b6003 == true) ShowObject(6003);
	RotateObject(t.importerGridObject[8], 0, 0, 0);
	for (t.tc = 1; t.tc <= 7; t.tc++)
	{
		ShowObject(t.importerGridObject[t.tc]);
	}

#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 

	extern DBPRO_GLOBAL CCameraManager m_CameraManager;
	DBPRO_GLOBAL tagCameraData* m_mycam;
	m_mycam = m_CameraManager.GetData(0);
	float thumbnail_dimenions = 128;
	if (m_mycam)
	{
		//extern ImVec2 OldrenderTargetSize; //OldrenderTargetSize.x

		float dw = GetDisplayWidth(); //GetChildWindowWidth(-1);
		float dh = GetDisplayHeight(); //GetChildWindowHeight(-1);
		float gi_rectw = (dw*0.5) -(thumbnail_dimenions*0.5);
		float gi_recth = (dh*0.5) -(thumbnail_dimenions*0.5);
		extern GlobStruct* g_pGlob;
		LPGGSURFACE	pTmpSurface = g_pGlob->pCurrentBitmapSurface;
		g_pGlob->pCurrentBitmapSurface = m_mycam->pCameraToImageSurface;
		GrabImage(IMPORTER_TMP_IMAGE, gi_rectw, gi_recth, gi_rectw + thumbnail_dimenions, gi_recth + thumbnail_dimenions, 0);
		g_pGlob->pCurrentBitmapSurface = pTmpSurface;
	}
#else
	GrabImage(IMPORTER_TMP_IMAGE, 0, 0, 64, 64, 3);
#endif

	if (!bRemoveSprites) {
		t.tSprite = 100;
		while (SpriteExist(t.tSprite))
		{
			++t.tSprite;
		}
	}
	PositionObject(t.importerGridObject[8], 0, 0, IMPORTERZPOSITION);
	DeleteObject(t.twhiteobj);
	ShowObject(t.importer.dummyCharacterObjectNumber);

	t.aspect_f = GetDesktopWidth(); t.aspect_f = t.aspect_f / GetDesktopHeight();
	SetCameraAspect(t.aspect_f);
	SetCurrentBitmap(0);
	#endif
}
#else
void importer_save_entity ( char *filename )
{
	//  Check if user folder exists, if not create it
	t.strwork = ""; t.strwork = t.strwork + t.importer.startDir + "\\entitybank\\user";
	if (  PathExist( t.strwork.Get() )  ==  0 ) 
	{
		MakeDirectory (  t.strwork.Get() );
	}

	//  Ask for save filename
	t.tSaveFile_s = "";
	t.timporterprotected = 1;
	t.timportermessage_s = "Save Entity";
	while (  t.timporterprotected  ==  1 ) 
	{
		if (  t.importer.fpeIsMainFile  ==  0 ) 
		{
			t.strwork = ""; t.strwork = t.strwork +t.importer.startDir + "\\entitybank\\user";
			t.tSaveFile_s = openFileBox("Model (.dbo)|*.dbo|All Files|*.*|", t.strwork.Get() , t.timportermessage_s.Get(), ".dbo", IMPORTERSAVEFILE);
		}
		else
		{
			t.strwork = ""; t.strwork = t.strwork +t.importer.startDir + "\\entitybank\\user";
			t.tSaveFile_s = openFileBox("GG Entity (.fpe)|*.fpe|All Files|*.*|", t.strwork.Get(), t.timportermessage_s.Get(), ".fpe", IMPORTERSAVEFILE);
		}
		if (  t.tSaveFile_s  ==  "Error" ) 
		{
			t.timportersaveon = 0;
			return;
		}
		t.timporterprotected = importer_check_if_protected(t.tSaveFile_s.Get());
		if (  t.timporterprotected  ==  1 ) 
		{
			t.timportermessage_s = "You cannot overwrite protected media, please choose an alternative name";
		}
	}

	//  Ensure it has the dbo extension
	Dim (  t.tArray,300  );
	t.tArrayMarker = 0;
	t.tstring_s=t.tSaveFile_s;
	t.tToken_s=FirstToken(t.tstring_s.Get(),".");
	if (  t.tToken_s  !=  "" ) 
	{
		t.tArray[t.tArrayMarker] = t.tToken_s;
		++t.tArrayMarker;
	}
	do
	{
		t.tToken_s=NextToken(".");
		if (  t.tToken_s  !=  "" ) 
		{
			t.tArray[t.tArrayMarker] = t.tToken_s;
			++t.tArrayMarker;
		}
	} while ( !(  t.tToken_s == "" ) );
	t.tStippedFileName_s = "";
	int tCount = 0;
	for ( tCount = 0 ; tCount<=  t.tArrayMarker-2; tCount++ )
	{
		t.tStippedFileName_s = t.tStippedFileName_s + t.tArray[tCount];
	}
	UnDim (  t.tArray );
	t.tStippedFileName_s = t.tSaveFile_s;
	if (  strcmp ( Mid(t.tStippedFileName_s.Get(),Len(t.tStippedFileName_s.Get())-1)  ,  "." ) == 0 ) 
	{
		t.tStippedFileName_s = Left(t.tStippedFileName_s.Get(),Len(t.tStippedFileName_s.Get())-2);
	}
	else
	{
		t.tStippedFileName_s = Left(t.tStippedFileName_s.Get(),Len(t.tStippedFileName_s.Get())-4);
	}

	//  Grab the folder path
	Dim (  t.tArray,300  );
	t.tArrayMarker = 0;
	t.tstring_s=t.tStippedFileName_s;
	t.tToken_s=FirstToken(t.tstring_s.Get(),"\\");
	if (  t.tToken_s  !=  "" ) 
	{
		t.tArray[t.tArrayMarker] = t.tToken_s;
		++t.tArrayMarker;
	}
	do
	{
		t.tToken_s=NextToken("\\");
		if (  t.tToken_s  !=  "" ) 
		{
			t.tArray[t.tArrayMarker] = t.tToken_s;
			++t.tArrayMarker;
		}
	} while ( !(  t.tToken_s == "" ) );
	t.tStippedFileName_s = "";
	for ( tCount = 0 ; tCount<=  t.tArrayMarker-2; tCount++ )
	{
		t.tStippedFileName_s = t.tStippedFileName_s + t.tArray[tCount] + "\\";
	}
	t.tSavePath_s = t.tStippedFileName_s;

	//  Store file names
	if (  t.importer.fpeIsMainFile  ==  0 ) 
	{
		t.tSaveFile_s = t.tArray[t.tArrayMarker-1] + ".dbo";
		t.tSaveThumb_s = t.tArray[t.tArrayMarker-1] + ".BMP";
		t.importer.tFPESaveName = t.tArray[t.tArrayMarker-1] + ".fpe";
		t.importer.objectFPE.desc = t.tArray[t.tArrayMarker-1];
		t.importer.objectFPE.model = t.tSaveFile_s;
	}
	else
	{
		t.tSaveFile_s = t.importer.objectFilename;
		t.tSaveThumb_s = t.tArray[t.tArrayMarker-1] + ".BMP";
		t.importer.tFPESaveName = t.tArray[t.tArrayMarker-1] + ".fpe";
		t.importer.objectFPE.model = t.tSaveFile_s;
	}

	// Just before save object, ensure all texture references don't include root path
	sObject* pObject = GetObjectData ( t.importer.objectnumber );
	if ( pObject )
	{
		for ( int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++ )
		{
			sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
			if ( pMesh )
			{
				for ( int iTextureStage = 0; iTextureStage < pMesh->dwTextureCount; iTextureStage++ )
				{
					LPSTR pTexName = pMesh->pTextures[iTextureStage].pName;
					if ( strlen ( pTexName ) > 2 )
					{
						if ( pTexName[1] == ':' )
						{
							for ( int n = strlen(pTexName)-1; n > 0; n-- )
							{
								if ( pTexName[n] == '\\' || pTexName[n] == '/' )
								{
									strcpy ( pTexName, pTexName+n+1 );
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	// if selected 'Use Uber Anims', then rename skeleton and apply uber animations automatically
	if ( t.slidersmenuvalue[t.importer.properties1Index][7].value == 3 ) 
	{
		sObject* pObject = GetObjectData(t.importer.objectnumber);
		if ( pObject )
		{
			// load in correct Y pose and overwrite imported modes transform matrices
			GGMATRIX* pStoreNewPoseFrames = NULL;
			int objectnumberforframedatacopy = findFreeObject();
			cstr pAbsPathToUberAnimFile = g.fpscrootdir_s + "\\Files\\entitybank\\Characters\\2CHAINFINGER-YPOSE.dbo";//Uber Soldier.X";//appendanims.x";
			LoadObject ( pAbsPathToUberAnimFile.Get(), objectnumberforframedatacopy );
			if ( ObjectExist ( objectnumberforframedatacopy ) == 1 )
			{
				sObject* pObjectWithYPoseData = GetObjectData ( objectnumberforframedatacopy );
				if ( pObjectWithYPoseData )
				{
					// go through and find differences between poses
					for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
					{
						sFrame* pFrame = pObject->ppFrameList[iFrame];
						if ( pFrame )
						{
							LPSTR pFrameName = pFrame->szName;
							if ( pFrameName )
							{
								if ( strlen(pFrameName) > 0 )
								{
									// for this imported model frame, find the equivilant frame in the appendanim model (with the Y pose hidden in the skinning transform)
									for ( int iFindFrame = 0; iFindFrame < pObjectWithYPoseData->iFrameCount; iFindFrame++ )
									{
										sFrame* pFindFrame = pObjectWithYPoseData->ppFrameList[iFindFrame];
										if ( pFindFrame )
										{
											LPSTR pFindFrameName = pFindFrame->szName;
											if ( pFindFrameName )
											{
												if ( strlen(pFindFrameName) > 0 )
												{
													if ( stricmp ( pFrameName, pFindFrameName ) == NULL )
													{
														// just grab the local relative transform from the Y pose model
														pFrame->matOriginal = pFindFrame->matOriginal;
														pFrame->matTransformed = pFindFrame->matOriginal;

														// done with this findframe
														break;
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
				DeleteObject ( objectnumberforframedatacopy );
			}

			// go through all frames of imported model
			for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
			{
				sFrame* pFrame = pObject->ppFrameList[iFrame];
				if ( pFrame )
				{
					LPSTR pOldFrameName = pFrame->szName;
					if ( pOldFrameName )
					{
						if ( strlen(pOldFrameName) > 0 )
						{
							// look for common bone names, and transform to GG standard skeleton name
							const char* pNewName = "";
							if ( stricmp ( pOldFrameName, "mixamorig_Hips" ) == NULL ) pNewName = "Bip01_Pelvis";
							if ( stricmp ( pOldFrameName, "mixamorig_Spine" ) == NULL ) pNewName = "Bip01_Spine";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftUpLeg" ) == NULL ) pNewName = "Bip01_L_Thigh";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftLeg" ) == NULL ) pNewName = "Bip01_L_Calf";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftFoot" ) == NULL) pNewName = "Bip01_L_Foot";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftToeBase" ) == NULL ) pNewName = "Bip01_L_Toe0";
							if ( stricmp ( pOldFrameName, "mixamorig_RightUpLeg" ) == NULL ) pNewName = "Bip01_R_Thigh";
							if ( stricmp ( pOldFrameName, "mixamorig_RightLeg" ) == NULL ) pNewName = "Bip01_R_Calf";
							if ( stricmp ( pOldFrameName, "mixamorig_RightFoot" ) == NULL ) pNewName = "Bip01_R_Foot";
							if ( stricmp ( pOldFrameName, "mixamorig_RightToeBase" ) == NULL ) pNewName = "Bip01_R_Toe0";
							if ( stricmp ( pOldFrameName, "mixamorig_Spine1" ) == NULL ) pNewName = "Bip01_Spine1";
							if ( stricmp ( pOldFrameName, "mixamorig_Spine2" )== NULL ) pNewName = "Bip01_Spine2";
							if ( stricmp ( pOldFrameName, "mixamorig_Neck" ) == NULL ) pNewName = "Bip01_Neck";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftShoulder" ) == NULL ) pNewName = "Bip01_L_Clavicle";
							if ( stricmp ( pOldFrameName, "mixamorig_RightShoulder" ) == NULL ) pNewName = "Bip01_R_Clavicle";
							if ( stricmp ( pOldFrameName, "mixamorig_Head" ) == NULL ) pNewName = "Bip01_Head";
							if ( stricmp ( pOldFrameName, "mixamorig_HeadTop_End" ) == NULL ) pNewName = "Bip01_HeadTop";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftArm" ) == NULL ) pNewName = "Bip01_L_UpperArm";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftForeArm" ) == NULL ) pNewName = "Bip01_L_Forearm";
							if ( stricmp ( pOldFrameName, "mixamorig_RightArm" ) == NULL ) pNewName = "Bip01_R_UpperArm";
							if ( stricmp ( pOldFrameName, "mixamorig_RightForeArm" ) == NULL ) pNewName = "Bip01_R_Forearm";
							if ( stricmp ( pOldFrameName, "mixamorig_LeftHand" ) == NULL ) pNewName = "Bip01_L_Hand";
							if ( stricmp ( pOldFrameName, "mixamorig_RightHand" ) == NULL ) pNewName = "Bip01_R_Hand";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandThumb1" ) > 0 ) pNewName = "Bip01_L_Finger0";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandThumb2" ) > 0 ) pNewName = "Bip01_L_Finger01";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandThumb3" ) > 0 ) pNewName = "Bip01_L_Finger02";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandThumb4" ) > 0 ) pNewName = "Bip01_L_Finger03";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandIndex1" ) > 0 ) pNewName = "Bip01_L_Finger1";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandIndex2" ) > 0 ) pNewName = "Bip01_L_Finger11";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandIndex3" ) > 0 ) pNewName = "Bip01_L_Finger12";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandIndex4" ) > 0 ) pNewName = "Bip01_L_Finger13";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandMiddle1" ) > 0 ) pNewName = "Bip01_L_Finger2";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandMiddle2" ) > 0 ) pNewName = "Bip01_L_Finger21";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandMiddle3" ) > 0 ) pNewName = "Bip01_L_Finger22";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandMiddle4" ) > 0 ) pNewName = "Bip01_L_Finger23";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandRing1" ) > 0 ) pNewName = "Bip01_L_Finger3";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandRing2" ) > 0 ) pNewName = "Bip01_L_Finger31";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandRing3" ) > 0 ) pNewName = "Bip01_L_Finger32";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandRing4" ) > 0 ) pNewName = "Bip01_L_Finger33";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandPinky1" ) > 0 ) pNewName = "Bip01_L_Finger4";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandPinky2" ) > 0 ) pNewName = "Bip01_L_Finger41";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandPinky3" ) > 0 ) pNewName = "Bip01_L_Finger42";
							if ( strstr ( pOldFrameName, "mixamorig_LeftHandPinky4" ) > 0 ) pNewName = "Bip01_L_Finger43";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandThumb1" ) > 0 ) pNewName = "Bip01_R_Finger0";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandThumb2" ) > 0 ) pNewName = "Bip01_R_Finger01";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandThumb3" ) > 0 ) pNewName = "Bip01_R_Finger02";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandThumb4" ) > 0 ) pNewName = "Bip01_R_Finger03";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandIndex1" ) > 0 ) pNewName = "Bip01_R_Finger1";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandIndex2" ) > 0 ) pNewName = "Bip01_R_Finger11";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandIndex3" ) > 0 ) pNewName = "Bip01_R_Finger12";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandIndex4" ) > 0 ) pNewName = "Bip01_R_Finger13";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandMiddle1" ) > 0 ) pNewName = "Bip01_R_Finger2";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandMiddle2" ) > 0 ) pNewName = "Bip01_R_Finger21";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandMiddle3" ) > 0 ) pNewName = "Bip01_R_Finger22";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandMiddle4" ) > 0 ) pNewName = "Bip01_R_Finger23";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandRing1" ) > 0 ) pNewName = "Bip01_R_Finger3";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandRing2" ) > 0 ) pNewName = "Bip01_R_Finger31";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandRing3" ) > 0 ) pNewName = "Bip01_R_Finger32";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandRing4" ) > 0 ) pNewName = "Bip01_R_Finger33";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandPinky1" ) > 0 ) pNewName = "Bip01_R_Finger4";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandPinky2" ) > 0 ) pNewName = "Bip01_R_Finger41";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandPinky3" ) > 0 ) pNewName = "Bip01_R_Finger42";
							if ( strstr ( pOldFrameName, "mixamorig_RightHandPinky4" ) > 0 ) pNewName = "Bip01_R_Finger43";
							if ( strlen(pNewName) == 0 )
							{
								/* should not associate by happenstance, it meses up anim skin deformation if two frames are referenced under name name!
								if ( strstr ( pOldFrameName, "Hips" ) > 0 ) pNewName = "Bip01_Pelvis";
								if ( strstr ( pOldFrameName, "Spine" ) > 0 ) pNewName = "Bip01_Spine";
								if ( strstr ( pOldFrameName, "LeftUpLeg" ) > 0 ) pNewName = "Bip01_L_Thigh";
								if ( strstr ( pOldFrameName, "LeftLeg" ) > 0 ) pNewName = "Bip01_L_Calf";
								if ( strstr ( pOldFrameName, "LeftFoot" ) > 0 ) pNewName = "Bip01_L_Foot";
								if ( strstr ( pOldFrameName, "LeftToeBase" ) > 0 ) pNewName = "Bip01_L_Toe0";
								if ( strstr ( pOldFrameName, "RightUpLeg" ) > 0 ) pNewName = "Bip01_R_Thigh";
								if ( strstr ( pOldFrameName, "RightLeg" ) > 0 ) pNewName = "Bip01_R_Calf";
								if ( strstr ( pOldFrameName, "RightFoot" ) > 0 ) pNewName = "Bip01_R_Foot";
								if ( strstr ( pOldFrameName, "RightToeBase" ) > 0 ) pNewName = "Bip01_R_Toe0";
								if ( strstr ( pOldFrameName, "Spine1" ) > 0 ) pNewName = "Bip01_Spine1";
								if ( strstr ( pOldFrameName, "Spine2" ) > 0 ) pNewName = "Bip01_Spine2";
								if ( strstr ( pOldFrameName, "Neck" ) > 0 ) pNewName = "Bip01_Neck";
								if ( strstr ( pOldFrameName, "LeftShoulder" ) > 0 ) pNewName = "Bip01_L_Clavicle";
								if ( strstr ( pOldFrameName, "RightShoulder" ) > 0 ) pNewName = "Bip01_R_Clavicle";
								if ( strstr ( pOldFrameName, "Head" ) > 0 ) pNewName = "Bip01_Head";
								if ( strstr ( pOldFrameName, "LeftArm" ) > 0 ) pNewName = "Bip01_L_UpperArm";
								if ( strstr ( pOldFrameName, "LeftForeArm" ) > 0 ) pNewName = "Bip01_L_Forearm";
								if ( strstr ( pOldFrameName, "LeftHand" ) > 0 ) pNewName = "Bip01_L_Hand";
								if ( strstr ( pOldFrameName, "RightArm" ) > 0 ) pNewName = "Bip01_R_UpperArm";
								if ( strstr ( pOldFrameName, "RightForeArm" ) > 0 ) pNewName = "Bip01_R_Forearm";
								if ( strstr ( pOldFrameName, "RightHand" ) > 0 ) pNewName = "Bip01_R_Hand";
								if ( strstr ( pOldFrameName, "LeftHandThumb1" ) > 0 ) pNewName = "Bip01_L_Finger0";
								if ( strstr ( pOldFrameName, "LeftHandThumb2" ) > 0 ) pNewName = "Bip01_L_Finger01";
								if ( strstr ( pOldFrameName, "LeftHandThumb3" ) > 0 ) pNewName = "Bip01_L_Finger02";
								if ( strstr ( pOldFrameName, "LeftHandThumb4" ) > 0 ) pNewName = "Bip01_L_Finger03";
								if ( strstr ( pOldFrameName, "LeftHandIndex1" ) > 0 ) pNewName = "Bip01_L_Finger1";
								if ( strstr ( pOldFrameName, "LeftHandIndex2" ) > 0 ) pNewName = "Bip01_L_Finger11";
								if ( strstr ( pOldFrameName, "LeftHandIndex3" ) > 0 ) pNewName = "Bip01_L_Finger12";
								if ( strstr ( pOldFrameName, "LeftHandIndex4" ) > 0 ) pNewName = "Bip01_L_Finger13";
								if ( strstr ( pOldFrameName, "LeftHandMiddle1" ) > 0 ) pNewName = "Bip01_L_Finger2";
								if ( strstr ( pOldFrameName, "LeftHandMiddle2" ) > 0 ) pNewName = "Bip01_L_Finger21";
								if ( strstr ( pOldFrameName, "LeftHandMiddle3" ) > 0 ) pNewName = "Bip01_L_Finger22";
								if ( strstr ( pOldFrameName, "LeftHandMiddle4" ) > 0 ) pNewName = "Bip01_L_Finger23";
								if ( strstr ( pOldFrameName, "LeftHandRing1" ) > 0 ) pNewName = "Bip01_L_Finger3";
								if ( strstr ( pOldFrameName, "LeftHandRing2" ) > 0 ) pNewName = "Bip01_L_Finger31";
								if ( strstr ( pOldFrameName, "LeftHandRing3" ) > 0 ) pNewName = "Bip01_L_Finger32";
								if ( strstr ( pOldFrameName, "LeftHandRing4" ) > 0 ) pNewName = "Bip01_L_Finger33";
								if ( strstr ( pOldFrameName, "LeftHandPinky1" ) > 0 ) pNewName = "Bip01_L_Finger4";
								if ( strstr ( pOldFrameName, "LeftHandPinky2" ) > 0 ) pNewName = "Bip01_L_Finger41";
								if ( strstr ( pOldFrameName, "LeftHandPinky3" ) > 0 ) pNewName = "Bip01_L_Finger42";
								if ( strstr ( pOldFrameName, "LeftHandPinky4" ) > 0 ) pNewName = "Bip01_L_Finger43";
								if ( strstr ( pOldFrameName, "RightHandThumb1" ) > 0 ) pNewName = "Bip01_R_Finger0";
								if ( strstr ( pOldFrameName, "RightHandThumb2" ) > 0 ) pNewName = "Bip01_R_Finger01";
								if ( strstr ( pOldFrameName, "RightHandThumb3" ) > 0 ) pNewName = "Bip01_R_Finger02";
								if ( strstr ( pOldFrameName, "RightHandThumb4" ) > 0 ) pNewName = "Bip01_R_Finger03";
								if ( strstr ( pOldFrameName, "RightHandIndex1" ) > 0 ) pNewName = "Bip01_R_Finger1";
								if ( strstr ( pOldFrameName, "RightHandIndex2" ) > 0 ) pNewName = "Bip01_R_Finger11";
								if ( strstr ( pOldFrameName, "RightHandIndex3" ) > 0 ) pNewName = "Bip01_R_Finger12";
								if ( strstr ( pOldFrameName, "RightHandIndex4" ) > 0 ) pNewName = "Bip01_R_Finger13";
								if ( strstr ( pOldFrameName, "RightHandMiddle1" ) > 0 ) pNewName = "Bip01_R_Finger2";
								if ( strstr ( pOldFrameName, "RightHandMiddle2" ) > 0 ) pNewName = "Bip01_R_Finger21";
								if ( strstr ( pOldFrameName, "RightHandMiddle3" ) > 0 ) pNewName = "Bip01_R_Finger22";
								if ( strstr ( pOldFrameName, "RightHandMiddle4" ) > 0 ) pNewName = "Bip01_R_Finger23";
								if ( strstr ( pOldFrameName, "RightHandRing1" ) > 0 ) pNewName = "Bip01_R_Finger3";
								if ( strstr ( pOldFrameName, "RightHandRing2" ) > 0 ) pNewName = "Bip01_R_Finger31";
								if ( strstr ( pOldFrameName, "RightHandRing3" ) > 0 ) pNewName = "Bip01_R_Finger32";
								if ( strstr ( pOldFrameName, "RightHandRing4" ) > 0 ) pNewName = "Bip01_R_Finger33";
								if ( strstr ( pOldFrameName, "RightHandPinky1" ) > 0 ) pNewName = "Bip01_R_Finger4";
								if ( strstr ( pOldFrameName, "RightHandPinky2" ) > 0 ) pNewName = "Bip01_R_Finger41";
								if ( strstr ( pOldFrameName, "RightHandPinky3" ) > 0 ) pNewName = "Bip01_R_Finger42";
								if ( strstr ( pOldFrameName, "RightHandPinky4" ) > 0 ) pNewName = "Bip01_R_Finger43";
								*/
							}

							// replace all instances of this name (if found)
							if ( strlen(pNewName) > 0 ) 
							{
								// rename animation reference name too
								if ( pObject->pAnimationSet )
								{
									sAnimation* pCurrent = pObject->pAnimationSet->pAnimation;
									while(pCurrent)
									{
										if ( pCurrent->szName )
										{
											if ( stricmp ( pCurrent->szName, pFrame->szName ) == NULL )
											{
												strcpy ( pCurrent->szName, pNewName );
												break;
											}
										}
										pCurrent = pCurrent->pNext;
									}
								}

								// also rename bone names within each mesh
								for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
								{
									sMesh* pMesh = pObject->ppMeshList[iMesh];
									if ( pMesh )
									{
										if ( pMesh->pBones )
										{
											for ( int iBone = 0; iBone < pMesh->dwBoneCount; iBone++ )
											{
												if ( pMesh->pBones [ iBone ].szName )
												{
													if ( stricmp ( pMesh->pBones [ iBone ].szName, pFrame->szName ) == NULL )
													{
														strcpy ( pMesh->pBones [ iBone ].szName, pNewName );
														break;
													}
												}								
											}
										}
									}
								}

								// finally rename frame
								strcpy ( pFrame->szName, pNewName );
							}
							//else
							//	MessageBox ( NULL, "not found for", pOldFrameName, MB_OK );
						}
					}
				}
			}

			// now insert Bip01 into the frame hierarchy between Sceene Root and Pelvis
			for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
			{
				sFrame* pFindPelvisFrame = pObject->ppFrameList[iFrame];
				if ( pFindPelvisFrame )
				{
					LPSTR pFrameName = pFindPelvisFrame->szName;
					if ( pFrameName )
					{
						if ( strlen(pFrameName) > 0 )
						{
							if ( stricmp ( pFrameName, "Bip01_Pelvis" ) == NULL )
							{
								// create new Bip01 frame
								sFrame* pPelvisParent_SceneRoot = pFindPelvisFrame->pParent;
								sFrame* pBip01Frame = new sFrame();

								// insert it between scene root and pelvis
								strcpy ( pBip01Frame->szName, "Bip01" );
								pPelvisParent_SceneRoot->pChild = pBip01Frame;
								pBip01Frame->pParent = pPelvisParent_SceneRoot;
								pFindPelvisFrame->pParent = pBip01Frame;
								pBip01Frame->pChild = pFindPelvisFrame;

								// also create a new Animation object for this new Bip01 frame (so appended anim data can go somewhere (below))
								if ( pObject->pAnimationSet )
								{
									sAnimation* pLastAnim = pObject->pAnimationSet->pAnimation;
									if ( pLastAnim )
									{
										while ( pLastAnim->pNext ) pLastAnim = pLastAnim->pNext;
									}
									if ( pLastAnim )
									{
										sAnimation* pNewAnim = new sAnimation();
										pNewAnim->bLinear = 1;
										pNewAnim->pFrame = pBip01Frame;
										strcpy ( pNewAnim->szName, "Bip01" );
										pLastAnim->pNext = pNewAnim;
									}
								}

								// we are done
								break;
							}
						}
					}
				}
			}

			// and insert FIRESPOT limb to the right hand with default values
			for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
			{
				sFrame* pFindRightHandFrame = pObject->ppFrameList[iFrame];
				if ( pFindRightHandFrame )
				{
					LPSTR pFrameName = pFindRightHandFrame->szName;
					if ( pFrameName )
					{
						if ( strlen(pFrameName) > 0 )
						{
							if ( stricmp ( pFrameName, "Bip01_R_Hand" ) == NULL )
							{
								// create new FIRESPOT frame
								sFrame* pFIRESPOTFrame = new sFrame();

								// insert it as child of right hand
								sFrame* pLastChildSybling = pFindRightHandFrame->pChild;
								while (pLastChildSybling->pSibling) pLastChildSybling = pLastChildSybling->pSibling;
								strcpy ( pFIRESPOTFrame->szName, "FIRESPOT" );
								pLastChildSybling->pSibling = pFIRESPOTFrame;
								pFIRESPOTFrame->pParent = pFindRightHandFrame;

								// also create a new Animation object for this new Bip01 frame (so appended anim data can go somewhere (below))
								if ( pObject->pAnimationSet )
								{
									sAnimation* pLastAnim = pObject->pAnimationSet->pAnimation;
									if ( pLastAnim )
									{
										while ( pLastAnim->pNext ) pLastAnim = pLastAnim->pNext;
									}
									if ( pLastAnim )
									{
										sAnimation* pNewAnim = new sAnimation();
										pNewAnim->bLinear = 1;
										pNewAnim->pFrame = pFIRESPOTFrame;
										strcpy ( pNewAnim->szName, "FIRESPOT" );
										pLastAnim->pNext = pNewAnim;
									}
								}

								// we are done
								break;
							}
						}
					}
				}
			}

			// now we need to change the model geometry from a T bone to a Y pose (using the relative local transforms we generated above)
			GGMATRIX matrix;
			if ( 1 )
			{
				GGMatrixIdentity ( &matrix );
				UpdateFrame ( pObject->pFrame, &matrix );
				for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
				{
					// wipe out original vertex data
					sMesh* pMesh = pObject->ppMeshList[iMesh];
					if ( pMesh->pOriginalVertexData ) SAFE_DELETE ( pMesh->pOriginalVertexData );

					// animate mesh on CPU
					AnimateBoneMeshBONE ( pObject, NULL, pMesh );

					// now record new vertex mesh shape
					if ( pMesh->pOriginalVertexData ) SAFE_DELETE ( pMesh->pOriginalVertexData );
					DWORD dwTotalVertSize = pMesh->dwVertexCount * pMesh->dwFVFSize;
					pMesh->pOriginalVertexData = (BYTE*)new char [ dwTotalVertSize ];
					memcpy ( pMesh->pOriginalVertexData, pMesh->pVertexData, dwTotalVertSize );
				}
			}

			// now replace frame transform matrices in imported model (copied from appendanims.x)
			// which zeros the rotation and puts model in Y shape pose (not T) ready for stock 
			// animation data (but retains skinning information for the unique mesh geometry of the model)
			objectnumberforframedatacopy = findFreeObject();
			pAbsPathToUberAnimFile = g.fpscrootdir_s + "\\Files\\entitybank\\Characters\\appendanims.x";
			LoadObject ( pAbsPathToUberAnimFile.Get(), objectnumberforframedatacopy );
			if ( ObjectExist ( objectnumberforframedatacopy ) == 1 )
			{
				sObject* pObjectWithGoodFrameData = GetObjectData ( objectnumberforframedatacopy );
				if ( pObjectWithGoodFrameData )
				{
					// go through all frames of imported object, match up name and copy frame transform matrix over from 'framegood' model
					for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
					{
						sFrame* pFrame = pObject->ppFrameList[iFrame];
						if ( pFrame )
						{
							LPSTR pFrameName = pFrame->szName;
							if ( pFrameName )
							{
								if ( strlen(pFrameName) > 0 )
								{
									// check to find this name in framegood model
									for ( int iFrameGood = 0; iFrameGood < pObjectWithGoodFrameData->iFrameCount; iFrameGood++ )
									{
										sFrame* pFrameGood = pObjectWithGoodFrameData->ppFrameList[iFrameGood];
										if ( pFrameGood )
										{
											LPSTR pFrameGoodName = pFrameGood->szName;
											if ( pFrameGoodName )
											{
												if ( strlen(pFrameGoodName) > 0 )
												{
													// check if we have a match
													if ( stricmp ( pFrameName, pFrameGoodName ) == NULL )
													{
														// now copy the frame data to the import model
														pFrame->matOriginal = pFrameGood->matOriginal;
														pFrame->matTransformed = pFrameGood->matTransformed;
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
				DeleteObject ( objectnumberforframedatacopy );
			}

			// and restore transformed matrix from originals
			for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
			{
				sFrame* pFrame = pObject->ppFrameList[iFrame];
				if ( pFrame )
				{
					pFrame->matTransformed = pFrame->matOriginal;
				}
			}

			// delete all keys so append can be fresh (and specifically remove matrix keys which WILL mess up overall animation)
			if ( pObject->pAnimationSet )
			{
				sAnimationSet* pAnimSet = pObject->pAnimationSet;
				while ( pAnimSet != NULL )
				{
					sAnimation* pAnim = pAnimSet->pAnimation;
					while ( pAnim != NULL )
					{
						// scans all animation data and creates the interpolation vectors between all keyframes (vital)
						if ( pAnim )
						{
							SAFE_DELETE(pAnim->pPositionKeys);
							pAnim->dwNumPositionKeys=0;
							SAFE_DELETE(pAnim->pRotateKeys);
							pAnim->dwNumRotateKeys=0;
							SAFE_DELETE(pAnim->pScaleKeys);
							pAnim->dwNumScaleKeys=0;
							SAFE_DELETE(pAnim->pMatrixKeys);
							pAnim->dwNumMatrixKeys=0;
						}
						pAnim = pAnim->pNext;
					}
					pAnimSet = pAnimSet->pNext;
				}
			}

			// append uber animations to character
			AppendAnimationFromFile ( pObject, pAbsPathToUberAnimFile.Get(), 0 );

			// now update model to first frame animation pose (to calculate combined matrix from pose)
			GGMatrixIdentity ( &matrix );
			UpdateAllFrameData ( pObject, 0.0f );
			UpdateFrame ( pObject->pFrame, &matrix );

			// and then calculate the inverse of those matCombined transforms to apply to the bone matTransforms
			// so bone matrix cancels out first frame pose leaving matCombined animations to shape shader verts
			for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
			{
				sMesh* pMesh = pObject->ppMeshList[iMesh];
				if ( pMesh )
				{
					sBone* pBones = pMesh->pBones;
					if ( pBones )
					{
						for ( int iBone = 0; iBone < pMesh->dwBoneCount; iBone++ )
						{
							float fDet = 0.0f;
							GGMatrixInverse ( &pBones[iBone].matTranslation, &fDet, pMesh->pFrameMatrices [ iBone ] );
						}
					}
				}
			}
		}
	}

	// Save Object
	if ( strcmp ( Lower(Right(t.tSaveFile_s.Get(),4)) , ".dbo" ) == 0 ) 
	{
		if ( FileExist (t.tSaveFile_s.Get()) == 1 ) DeleteAFile ( t.tSaveFile_s.Get() ) ;
		ScaleObject (  t.importer.objectnumber,100,100,100 );
		SaveObject (  t.tSaveFile_s.Get(),t.importer.objectnumber );
		ScaleObject (  t.importer.objectnumber,t.importer.objectScaleForEditing,t.importer.objectScaleForEditing,t.importer.objectScaleForEditing );
	}
	else
	{
		if (  t.importer.objectFileOriginalPath+t.importer.objectFilename  !=  t.tSavePath_s+t.importer.objectFilename ) 
		{
			t.strwork = ""; t.strwork = t.strwork +t.importer.objectFileOriginalPath+t.importer.objectFilename;
			if (  FileExist( t.strwork.Get() ) == 1 ) 
			{
				t.strwork = ""; t.strwork = t.strwork +t.tSavePath_s+t.importer.objectFilename;
				if (  FileExist( t.strwork.Get() ) == 0 ) 
				{
					t.strwork = ""; t.strwork = t.strwork +t.importer.objectFileOriginalPath+t.importer.objectFilename;
					cstr string1 = t.tSavePath_s+t.importer.objectFilename;
					CopyAFile ( t.strwork.Get() , string1.Get() );
				}
			}
		}
	}
	UnDim (  t.tArray );

	Dim ( t.tArray2, 220 );
	for ( tCount = 1; tCount <= IMPORTERTEXTURESMAX; tCount++ )
	{
		// skip files that do not exist
		if ( t.importerTextures[tCount].imageID == 0 )  
			continue;

		//  Split the filename into tokens to grab the path
		t.tSourceName_s = t.importerTextures[tCount].fileName;
		t.tArrayMarker = 0;
		t.tstring_s=t.tSourceName_s;
		t.tToken_s=FirstToken(t.tstring_s.Get(),"\\");
		if (  t.tToken_s  !=  "" ) 
		{
			t.tArray2[t.tArrayMarker] = t.tToken_s;
			++t.tArrayMarker;
		}
		do
		{
			t.tToken_s=NextToken("\\");
			if (  t.tToken_s  !=  "" ) 
			{
				t.tArray2[t.tArrayMarker] = t.tToken_s;
				++t.tArrayMarker;
			}
		} while ( !(  t.tToken_s == "" ) );

		//  Now store just the file name
		t.tDestFileName = t.tSavePath_s + t.tArray2[t.tArrayMarker-1];
		if ( tCount == 1 )
		{
			// first texture in list is the 'color/diffuse' one (even if multitexture)
			t.importer.objectFPE.textured = t.tArray2[t.tArrayMarker-1];
		}

		//  Copy and save images to destination
		if (  ImageExist(t.importerTextures[tCount].imageID) == 1 ) 
		{
			cstr test1 = ""; test1 = test1 + g.fpscrootdir_s + "\\Files\\" + t.importerTextures[tCount].fileName;
			if (  FileExist (t.importerTextures[tCount].fileName.Get())  ==  1 && t.tDestFileName  !=  test1 && t.tDestFileName  !=  t.importerTextures[tCount].fileName ) 
			{
				if (  FileExist(t.tDestFileName.Get())  ==  0 ) 
				{
					CopyAFile (  t.importerTextures[tCount].fileName.Get(),t.tDestFileName.Get() );
				}
			}
			else
			{
				t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.importerTextures[tCount].fileName;
				cstr test2 = ""; test2 = test2 + g.fpscrootdir_s + "\\Files\\" + t.importerTextures[tCount].fileName;
				if (  FileExist ( t.strwork.Get() )  ==  1 && t.tDestFileName  !=  test2 ) 
				{
					if (  FileExist(t.tDestFileName.Get())  ==  0 ) 
					{
						t.strwork = "" ; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.importerTextures[tCount].fileName;
						CopyAFile (  t.strwork.Get() ,t.tDestFileName.Get() );
					}
				}
				else
				{
					t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.importerTextures[tCount].fileName;
					if (  t.tDestFileName  !=  t.strwork ) 
					{
						if (  ImageExist (t.importerTextures[tCount].imageID) ) 
						{
							if (  FileExist(t.tDestFileName.Get())  ==  0 ) 
							{
								SaveImage (  t.tDestFileName.Get() , t.importerTextures[tCount].imageID );
							}
						}
					}
				}
			}
		}

		//  Check for Normal and Specular and copy those if they exist
		if (  Len(t.importerTextures[tCount].fileName.Get()) > 6 ) 
		{
			if ( strcmp ( Lower(Right(t.importerTextures[tCount].fileName.Get(),6)) , "_d.dds" ) == 0 || strcmp ( Lower(Right(t.importerTextures[tCount].fileName.Get(),6)) , "_d.png" ) == 0 ) 
			{
				//  Normal map first
				t.tnormalmapfile_s = ""; t.tnormalmapfile_s = t.tnormalmapfile_s + Left(t.importerTextures[tCount].fileName.Get(),Len(t.importerTextures[tCount].fileName.Get())-6) + "_N.dds";
				t.tDestFileName = t.tSavePath_s + t.tArray2[t.tArrayMarker-1];
				cstr TempFileName = t.tDestFileName;
				t.tDestFileName = "" ; t.tDestFileName = t.tDestFileName + Left(TempFileName.Get(),Len(TempFileName.Get())-6) + "_N.dds";
				t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
				if (  FileExist (t.tnormalmapfile_s.Get())  ==  1 && t.tDestFileName  !=  t.strwork && t.tDestFileName  !=  t.tnormalmapfile_s )
				{
					if (  FileExist(t.tDestFileName.Get())  ==  0 ) 
					{
						CopyAFile (  t.tnormalmapfile_s.Get(),t.tDestFileName.Get() );
					}
				}
				else
				{
					t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					if (  FileExist ( t.strwork.Get() )  ==  1 && t.tDestFileName  !=  g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s ) 
					{
						if (  FileExist(t.tDestFileName.Get())  ==  0 ) 
						{
							t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
							CopyAFile ( t.strwork.Get() ,t.tDestFileName.Get() );
						}
					}
				}

				//  Normal map continued
				t.tnormalmapfile_s = ""; t.tnormalmapfile_s = t.tnormalmapfile_s + Left(t.importerTextures[tCount].fileName.Get(),Len(t.importerTextures[tCount].fileName.Get())-6) + "_N.png";
				t.tDestFileName = t.tSavePath_s + t.tArray2[t.tArrayMarker-1];
				TempFileName = t.tDestFileName;
				t.tDestFileName = "" ; t.tDestFileName = t.tDestFileName + Left(TempFileName.Get(),Len(TempFileName.Get())-6) + "_N.png";
				t.strwork = ""; t.strwork = t.strwork + "\\Files\\" + t.tnormalmapfile_s;
				if (  FileExist (t.tnormalmapfile_s.Get())  ==  1 && t.tDestFileName  !=  g.fpscrootdir_s + t.strwork && t.tDestFileName  !=  t.tnormalmapfile_s )  
				{
					if (  FileExist(t.tDestFileName.Get())  ==  0 ) 
					{
						CopyAFile (  t.tnormalmapfile_s.Get() ,t.tDestFileName.Get() );
					}
				}
				else
				{

					cstr work1 = ""; work1 = work1 + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					if (  FileExist ( work1.Get() )  ==  1 && t.tDestFileName  !=  t.strwork )  
					{
						if (  FileExist(t.tDestFileName.Get())  ==  0 ) 
						{
							t.strwork = "" ; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
							CopyAFile (  t.strwork.Get() , t.tDestFileName.Get() );
						}
					}
				}

				//  Specular now
				t.tnormalmapfile_s = "" ; t.tnormalmapfile_s = t.tnormalmapfile_s + Left(t.importerTextures[tCount].fileName.Get(),Len(t.importerTextures[tCount].fileName.Get())-6) + "_S.dds";
				t.tDestFileName = t.tSavePath_s + t.tArray2[t.tArrayMarker-1];
				TempFileName = t.tDestFileName;
				t.tDestFileName = "" ; t.tDestFileName = t.tDestFileName + Left (TempFileName.Get(),Len(TempFileName.Get())-6) + "_S.dds";
				t.strwork = "" ; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
				if (  FileExist (t.tnormalmapfile_s.Get())  ==  1 && t.tDestFileName  !=  t.strwork && t.tDestFileName  !=  t.tnormalmapfile_s ) 
				{
					if (  FileExist(t.tDestFileName.Get())  ==  0 ) 
					{
						CopyAFile (  t.tnormalmapfile_s.Get() , t.tDestFileName.Get() );
					}
				}
				else
				{
					cstr work2 = ""; work2 = work2 + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					if (  FileExist ( work2.Get() )  ==  1 && t.tDestFileName  !=  t.strwork ) 
					{
						if (  FileExist(t.tDestFileName.Get())  ==  0 ) 
						{
							t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
							CopyAFile (  t.strwork.Get() ,t.tDestFileName.Get() );
						}
					}
				}

				t.tnormalmapfile_s = ""; t.tnormalmapfile_s = t.tnormalmapfile_s + Left(t.importerTextures[tCount].fileName.Get(),Len(t.importerTextures[tCount].fileName.Get())-6) + "_S.png";
				t.tDestFileName = t.tSavePath_s + t.tArray2[t.tArrayMarker-1];
				TempFileName = t.tDestFileName;
				t.tDestFileName = ""; t.tDestFileName = t.tDestFileName + Left(TempFileName.Get(),Len(TempFileName.Get())-6) + "_S.png";
				t.strwork = ""; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
				if (  FileExist (t.tnormalmapfile_s.Get())  ==  1 && t.tDestFileName  !=  t.strwork && t.tDestFileName  !=  t.tnormalmapfile_s ) 
				{
					if (  FileExist(t.tDestFileName.Get())  ==  0 ) 
					{
						CopyAFile (  t.tnormalmapfile_s.Get(),t.tDestFileName.Get() );
					}
				}
				else
				{
					cstr work3 = ""; work3 = work3 + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					t.strwork = "" ; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
					if (  FileExist ( work3.Get() )  ==  1 && t.tDestFileName  !=  t.strwork ) 
					{
						if (  FileExist(t.tDestFileName.Get() )  ==  0 ) 
						{
							t.strwork = "" ; t.strwork = t.strwork + g.fpscrootdir_s + "\\Files\\" + t.tnormalmapfile_s;
							CopyAFile (  t.strwork.Get() ,t.tDestFileName.Get() );
						}
					}
				}
			}
		}
	}
	UnDim (  t.tArray2 );

	// If model has MORE than one texture, blank out FPE textured
	if ( t.tcounttextures > 1 ) 
	{
		// but make sure they are 'different' textures, not just DNS/PBR sets
		LPSTR pFile = t.importerTextures[1].fileName.Get();
		LPSTR pExt = "_d.png";
		if ( strnicmp ( pFile+strlen(pFile)-10, "_color.png", 10 ) == NULL ) pExt = "_color.png";
		if ( strnicmp ( pFile+strlen(pFile)-12, "_diffuse.png", 12 ) == NULL ) pExt = "_diffuse.png";
		if ( strnicmp ( pFile+strlen(pFile)-11, "_albedo.png", 11 ) == NULL ) pExt = "_albedo.png";
		if ( strnicmp ( pFile+strlen(pFile)-10, "_color.dds", 10 ) == NULL ) pExt = "_color.dds";
		if ( strnicmp ( pFile+strlen(pFile)-12, "_diffuse.dds", 12 ) == NULL ) pExt = "_diffuse.dds";
		if ( strnicmp ( pFile+strlen(pFile)-11, "_albedo.dds", 11 ) == NULL ) pExt = "_albedo.dds";
		cstr pBaseFilePart = Left(pFile,Len(pFile)-strlen(pExt));
		for ( int tCount = 2; tCount <= 10; tCount++ )
		{
			LPSTR pCompareFile = t.importerTextures[tCount].fileName.Get();
			if ( strlen ( pCompareFile ) > 4 )
			{
				if ( strnicmp ( pCompareFile, pBaseFilePart.Get(), strlen(pBaseFilePart.Get()) ) == NULL )
				{
					// matches base part, now exclude files of known extension
					bool bExclude = false;
					char pRest[1024];
					strcpy ( pRest, pCompareFile + strlen(pBaseFilePart.Get()) );
					pRest[strlen(pRest)-4] = 0;
					if ( stricmp ( pRest, "_normal" ) == NULL ) bExclude = true;
					if ( stricmp ( pRest, "_specular" ) == NULL ) bExclude = true;
					if ( stricmp ( pRest, "_metalness" ) == NULL ) bExclude = true;
					if ( stricmp ( pRest, "_gloss" ) == NULL ) bExclude = true;
					if ( stricmp ( pRest, "_ao" ) == NULL ) bExclude = true;
					if ( stricmp ( pRest, "_height" ) == NULL ) bExclude = true;
					if ( stricmp ( pRest, "_cube" ) == NULL ) bExclude = true;
					if ( bExclude == false )
					{
						// found a match to base texture, but unknown extra part
						t.importer.objectFPE.textured="";
						break;
					}
				}
				else
				{
					// found a different base texture, cannot be a single textured model
					t.importer.objectFPE.textured="";
					break;
				}
			}
		}
	}

	// save FPE file
	importer_save_fpe ( );

	// Save/Generate Thumbnail BMP
	t.strwork = ""; t.strwork = t.strwork + t.importer.objectFileOriginalPath + t.importer.objectFilename;
	cstr pSourceBMP = cstr ( cstr(Left ( t.strwork.Get(), strlen(t.strwork.Get())-4)) + ".bmp" );
	if ( FileExist ( pSourceBMP.Get() ) ) 
	{
		// if BMP already existed in source area, use that one
		if ( FileExist ( t.tSaveThumb_s.Get()) == 1 ) DeleteAFile ( t.tSaveThumb_s.Get() );
		CopyFile ( pSourceBMP.Get(), t.tSaveThumb_s.Get(), FALSE );
	}
	else
	{
		// generate new BMP thumb from scratch
		CreateBitmap ( 32, 64, 64 );
		SetCurrentBitmap ( 32 );
		SetCameraAspect ( 1.0 );

		//  Make a white background Box (  )
		t.twhiteobj = 1;
		t.tfound = 0;

		//  Find a free object number
		while (  t.tfound  ==  0 ) 
		{
			if (  ObjectExist(t.twhiteobj)  ==  1 ) 
			{
				++t.twhiteobj;
			}
			else
			{
				t.tfound = 1;
			}
		}

		MakeObjectBox (  t.twhiteobj,200000,200000,1 );
		SetObjectLight (  t.twhiteobj,0 );
		LockObjectOn (  t.twhiteobj );
		PositionObject (  t.twhiteobj,0,0,1000 );
		SetObjectEffect ( t.twhiteobj, g.guishadereffectindex );
		SetObjectEmissive ( t.twhiteobj, Rgb(255,255,255) );

		t.tOldSelected = t.importerTabs[1].selected;

		HideSprite (  t.importer.helpSprite2 );
		HideSprite (  t.importer.helpSprite3 );
		if (  SpriteExist(t.importer.helpSprite4)  )  DeleteSprite (  t.importer.helpSprite4 );

		t.importerTabs[1].selected = 1;
		importer_update_selection_markers ( );
		RotateObject (  t.importerGridObject[8], 0,0,0 );
		PositionObject (  t.importerGridObject[8],0,0,IMPORTERZPOSITION );
		RotateObject (  t.importerGridObject[8], t.slidersmenuvalue[t.importer.properties3Index][1].value, t.slidersmenuvalue[t.importer.properties3Index][2].value, t.slidersmenuvalue[t.importer.properties3Index][3].value );
		SetAlphaMappingOn (  t.importer.objectnumber , 255 );
		HideObject (  t.importer.dummyCharacterObjectNumber );
		t.importerTabs[1].selected = t.tOldSelected;
		for ( t.tc = 1 ; t.tc<=  7; t.tc++ )
		{
			HideObject (  t.importerGridObject[t.tc] );
		}
		bool b6002 = false; if ( ObjectExist(6002)==1) { b6002 = GetVisible(6002); HideObject(6002); }
		bool b6003 = false; if ( ObjectExist(6003)==1) { b6003 = GetVisible(6003); HideObject(6003); }
		Sync();
		if ( b6002 == true ) ShowObject ( 6002 );
		if ( b6003 == true ) ShowObject ( 6003 );
		RotateObject (  t.importerGridObject[8], 0,0,0 );
		for ( t.tc = 1 ; t.tc<=  7; t.tc++ )
		{
			ShowObject ( t.importerGridObject[t.tc] );
		}
		if ( FileExist (t.tSaveThumb_s.Get())  ==  1  )  DeleteAFile (  t.tSaveThumb_s.Get() ) ;
		GrabImage (IMPORTER_TMP_IMAGE, 0, 0, 64, 64, 3 );
		t.tSprite = 100;
		while (  SpriteExist (t.tSprite) ) 
		{
			++t.tSprite;
		}
		SaveImage ( t.tSaveThumb_s.Get(), IMPORTER_TMP_IMAGE);
		DeleteImage (IMPORTER_TMP_IMAGE);
		PositionObject ( t.importerGridObject[8],0,0,IMPORTERZPOSITION );
		DeleteObject ( t.twhiteobj );
	}

	t.aspect_f=GetDesktopWidth() ; t.aspect_f=t.aspect_f/GetDesktopHeight();
	SetCameraAspect (  t.aspect_f );
	SetCurrentBitmap (  0 );
	importer_update_selection_markers ( );
	importer_draw ( );
	importer_tabs_draw ( );
	Sync (  );
	t.importer.cancel = 1;
	t.timportersaveon = 0;
}
#endif

void importer_tabs_update ( void )
{
	if (bRemoveSprites)
		return;
	//  unselect buttons after a time
	if (  t.importer.buttonPressedCount > 0 ) 
	{
		--t.importer.buttonPressedCount;
		if (  t.importer.buttonPressedCount  <= 0 ) 
		{
			t.importerTabs[12].selected = 0;
			for ( int tCount = 5 ; tCount<=  9; tCount++ )
			{
				t.importerTabs[tCount].selected = 0;
			}
		}
	}

	if (  t.importer.oldMouseClick  ==  0 && t.inputsys.mclick  ==  1 ) 
	{
		for ( t.tCount5 = 1 ; t.tCount5 <= 12; t.tCount5++ )
		{
			if (  t.importer.MouseX  >=  t.importerTabs[t.tCount5].x && t.importer.MouseX  <=  t.importerTabs[t.tCount5].x + 128 ) 
			{
				if (  t.importer.MouseY  >=  t.importerTabs[t.tCount5].y && t.importer.MouseY  <=  t.importerTabs[t.tCount5].y + 32 ) 
				{
					if (  t.tCount5 < 5 ) 
					{

						for ( t.tCount2 = 1 ; t.tCount2<=  3; t.tCount2++ )
						{
							if (  t.tCount5  <=  3 ) 
							{
								if (  t.tCount2  ==  t.tCount5  )  t.importerTabs[t.tCount2].selected  =  1; else t.importerTabs[t.tCount2].selected  =  0;
							}
						}
						if (  t.tCount5  <= 3 ) 
						{
							g.tabmode = t.importerTabs[t.tCount5].tabpage;
							RotateObject (  t.importer.objectnumber,0,0,0 );
						}

					}
					else
					{

						//  check for save entity/object pressed
						if (  t.tCount5  ==  5 ) 
						{
							t.importerTabs[5].selected = 1;
						}

						//  cancel
						if (  t.tCount5  ==  11 ) 
						{
							t.importerTabs[11].selected = 1;
							t.importer.cancel = 1;
							t.importer.cancelCount = 10;
						}
						//  tab 1 buttons
						if (  t.tCount5  ==  10 && t.importerTabs[1].selected  ==  1 ) 
						{
							t.importerTabs[t.tCount5].selected = 1 - t.importerTabs[t.tCount5].selected;
						}

						//  tab 2 buttons
						if (  t.tCount5 > 5 && t.importerTabs[2].selected  ==  1 ) 
						{
							t.importerTabs[t.tCount5].selected = 1;
							t.importer.buttonPressedCount = 20;

							//  add new collision Box (  )
							if (  t.tCount5  ==  6 ) 
							{
								importer_add_collision_box ( );
							}

							//  delete collision Box (  )
							if (  t.tCount5  ==  7 ) 
							{
								importer_delete_collision_box ( );
							}

							//  next collision Box (  )
							if (  t.tCount5  ==  8 ) 
							{
								++t.importer.selectedCollisionObject;
								if (  t.importer.selectedCollisionObject  >=  t.importer.collisionShapeCount ) 
								{
									t.importer.selectedCollisionObject = 0;
								}
							}

							//  previous collision Box (  )
							if (  t.tCount5  ==  9 ) 
							{
								--t.importer.selectedCollisionObject;
								if (  t.importer.selectedCollisionObject < 0 ) 
								{
									t.importer.selectedCollisionObject = t.importer.collisionShapeCount-1;
								}
							}

							//  add new collision Box (  )
							if (  t.tCount5  ==  12 ) 
							{
								importer_dupe_collision_box ( );
							}

						}
					}
				}
			}
		}
	}

	if ( t.importerTabs[10].selected == 1 ) 
	{
		t.importerTabs[10].label = "Turn Guide Off";
	}
	else
	{
		t.importerTabs[10].label = "Turn Guide On";
	}
}

void importer_tabs_draw ( void )
{
	if(bRemoveSprites)
		return;
	// if image exist, importer active
	if ( t.importer.importerActive == 1 )
	{
		for ( int tCount = 1 ; tCount<=  12; tCount++ )
		{
			if (  tCount < 4 || tCount  >=  5 ) 
			{
				//  dont draw the collsion buttons if the tab isnt active
				t.skip = 0;
				if (  tCount > 5 && tCount < 10 && t.importerTabs[2].selected  ==  0  )  t.skip  =  1;
				if (  tCount  ==  12 && t.importerTabs[2].selected  ==  0  )  t.skip  =  1;
				if (  tCount  ==  10 && t.importerTabs[1].selected  ==  0  )  t.skip  =  1;

				// going to draw button
				if (  t.skip  ==  0 ) 
				{
					if (  t.importerTabs[tCount].selected  ==  1 ) 
					{
						PasteImage (  g.importermenuimageoffset+5 , t.importerTabs[tCount].x , t.importerTabs[tCount].y );
					}
					else
					{
						PasteImage (  g.importermenuimageoffset+4 , t.importerTabs[tCount].x , t.importerTabs[tCount].y );
					}
					if (  tCount > 5 && tCount < 11 || tCount  ==  12 ) 
					{
						pastebitmapfont(t.importerTabs[tCount].label.Get(),t.importerTabs[tCount].x + 64 - ((getbitmapfontwidth(t.importerTabs[tCount].label.Get(),2)) / 2) ,t.importerTabs[tCount].y + 9,2,255);
					}
					else
					{
						pastebitmapfont(t.importerTabs[tCount].label.Get(),t.importerTabs[tCount].x + 64 - ((getbitmapfontwidth(t.importerTabs[tCount].label.Get(),1)) / 2) ,t.importerTabs[tCount].y,1,255);
					}
				}
			}
		}
	}
}

void importer_add_collision_box ( void )
{
	if (bRemoveSprites)
		return;
	t.tScale = t.importer.objectScaleForEditing;
	if (  t.importer.collisionShapeCount < 49 ) 
	{
		t.importerCollision[t.importer.collisionShapeCount].object = findFreeObject();
		MakeObjectBox (  t.importerCollision[t.importer.collisionShapeCount].object , 100 , 100 , 100 );
		ScaleLimb (  t.importerCollision[t.importer.collisionShapeCount].object, 0, t.tObjectSizeX_f , t.tObjectSizeY_f, t.tObjectSizeZ_f );
		t.importerCollision[t.importer.collisionShapeCount].sizex = t.tObjectSizeX_f;
		t.importerCollision[t.importer.collisionShapeCount].sizey = t.tObjectSizeY_f;
		t.importerCollision[t.importer.collisionShapeCount].sizez = t.tObjectSizeZ_f;
		LockObjectOn (  t.importerCollision[t.importer.collisionShapeCount].object );
		PositionObject (  t.importerCollision[t.importer.collisionShapeCount].object , t.tBoxOffsetLeft_f , t.tBoxOffsetTop_f, t.tBoxOffsetFront_f );
		PositionObject (  t.importerGridObject[9], 0 , 0 , 0 );
		t.importerCollision[t.importer.collisionShapeCount].rotx = 0;
		t.importerCollision[t.importer.collisionShapeCount].roty = 0;
		t.importerCollision[t.importer.collisionShapeCount].rotz = 0;
		GlueObjectToLimbEx (  t.importerCollision[t.importer.collisionShapeCount].object,t.importerGridObject[9], 0 , 1 );
		DisableObjectZDepth (  t.importerCollision[t.importer.collisionShapeCount].object );
		SetObjectLight (  t.importerCollision[t.importer.collisionShapeCount].object,0 );
		HideObject (  t.importerCollision[t.importer.collisionShapeCount].object );
		SetObjectEffect ( t.importerCollision[t.importer.collisionShapeCount].object, g.guishadereffectindex );
		SetObjectEmissive ( t.importerCollision[t.importer.collisionShapeCount].object, Rgb(255,255,255) );
		SetAlphaMappingOn ( t.importerCollision[t.importer.collisionShapeCount].object, 10 );

		t.importerCollision[t.importer.collisionShapeCount].object2 = findFreeObject();
		MakeObjectBox (  t.importerCollision[t.importer.collisionShapeCount].object2 , 100,100,100 );
		ScaleLimb (  t.importerCollision[t.importer.collisionShapeCount].object2, 0, t.tObjectSizeX_f , t.tObjectSizeY_f, t.tObjectSizeZ_f );
		LockObjectOn (  t.importerCollision[t.importer.collisionShapeCount].object2 );
		PositionObject (  t.importerCollision[t.importer.collisionShapeCount].object2 , t.tBoxOffsetLeft_f , t.tBoxOffsetTop_f, t.tBoxOffsetFront_f );
		SetObjectCull (  t.importerCollision[t.importer.collisionShapeCount].object,0 );
		SetObjectCull (  t.importerCollision[t.importer.collisionShapeCount].object2,0 );
		PositionObject (  t.importerGridObject[9], 0 , 0 , 0 );
		t.importerCollision[t.importer.collisionShapeCount].rotx = 0;
		t.importerCollision[t.importer.collisionShapeCount].roty = 0;
		t.importerCollision[t.importer.collisionShapeCount].rotz = 0;
		GlueObjectToLimbEx (  t.importerCollision[t.importer.collisionShapeCount].object2,t.importerGridObject[9], 0 , 1 );
		DisableObjectZDepth (  t.importerCollision[t.importer.collisionShapeCount].object2 );
		SetObjectWireframe (  t.importerCollision[t.importer.collisionShapeCount].object2 , 1 );
		SetObjectLight (  t.importerCollision[t.importer.collisionShapeCount].object2,0 );
		HideObject (  t.importerCollision[t.importer.collisionShapeCount].object2 );
		SetObjectEffect ( t.importerCollision[t.importer.collisionShapeCount].object2, g.guiwireframeshadereffectindex );
		SetObjectEmissive ( t.importerCollision[t.importer.collisionShapeCount].object2, Rgb(255,255,255) );

		PositionObject (  t.importerGridObject[9], 0 , 0 , IMPORTERZPOSITION );

		t.importer.selectedCollisionObject = t.importer.collisionShapeCount;
		t.slidersmenuvalue[t.importer.properties2Index][7].value = 0;
		t.slidersmenuvalue[t.importer.properties2Index][8].value = 0;
		t.slidersmenuvalue[t.importer.properties2Index][9].value = 0;
		++t.importer.collisionShapeCount;
	}
}

void importer_dupe_collision_box ( void )
{
	if (bRemoveSprites)
		return;
	t.tScale = t.importer.objectScaleForEditing;
	if (  t.importer.selectedCollisionObject  ==  -1  )  return;
	if (  t.importer.collisionShapeCount < 49 ) 
	{
		t.importerCollision[t.importer.collisionShapeCount].object = findFreeObject();
		MakeObjectBox (  t.importerCollision[t.importer.collisionShapeCount].object , 100 , 100 , 100 );
		ScaleLimb (  t.importerCollision[t.importer.collisionShapeCount].object, 0, LimbScaleX(t.importerCollision[t.importer.selectedCollisionObject].object,0) ,  LimbScaleY(t.importerCollision[t.importer.selectedCollisionObject].object,0),  LimbScaleZ(t.importerCollision[t.importer.selectedCollisionObject].object,0) );
		t.importerCollision[t.importer.collisionShapeCount].sizex = LimbScaleX(t.importerCollision[t.importer.selectedCollisionObject].object,0);
		t.importerCollision[t.importer.collisionShapeCount].sizey = LimbScaleY(t.importerCollision[t.importer.selectedCollisionObject].object,0);
		t.importerCollision[t.importer.collisionShapeCount].sizez = LimbScaleZ(t.importerCollision[t.importer.selectedCollisionObject].object,0);
		LockObjectOn (  t.importerCollision[t.importer.collisionShapeCount].object );
		PositionObject (  t.importerCollision[t.importer.collisionShapeCount].object , ObjectPositionX(t.importerCollision[t.importer.selectedCollisionObject].object) ,  ObjectPositionY(t.importerCollision[t.importer.selectedCollisionObject].object),  ObjectPositionZ(t.importerCollision[t.importer.selectedCollisionObject].object) );
		PositionObject (  t.importerGridObject[9], 0 , 0 , 0 );
		t.importerCollision[t.importer.collisionShapeCount].rotx = ObjectAngleX(t.importerCollision[t.importer.selectedCollisionObject].object2);
		t.importerCollision[t.importer.collisionShapeCount].roty = ObjectAngleY(t.importerCollision[t.importer.selectedCollisionObject].object2);
		t.importerCollision[t.importer.collisionShapeCount].rotz = ObjectAngleZ(t.importerCollision[t.importer.selectedCollisionObject].object2);
		GlueObjectToLimbEx (  t.importerCollision[t.importer.collisionShapeCount].object,t.importerGridObject[9], 0 , 1 );
		DisableObjectZDepth (  t.importerCollision[t.importer.collisionShapeCount].object );
		SetObjectLight (  t.importerCollision[t.importer.collisionShapeCount].object,0 );
		HideObject (  t.importerCollision[t.importer.collisionShapeCount].object );
		SetObjectEffect ( t.importerCollision[t.importer.collisionShapeCount].object, g.guishadereffectindex );
		SetObjectEmissive ( t.importerCollision[t.importer.collisionShapeCount].object, Rgb(255,255,255) );
		SetAlphaMappingOn ( t.importerCollision[t.importer.collisionShapeCount].object, 10 );

		t.importerCollision[t.importer.collisionShapeCount].object2 = findFreeObject();
		MakeObjectBox (  t.importerCollision[t.importer.collisionShapeCount].object2 , 100,100,100 );
		ScaleLimb (  t.importerCollision[t.importer.collisionShapeCount].object2, 0, LimbScaleX(t.importerCollision[t.importer.selectedCollisionObject].object,0) ,  LimbScaleY(t.importerCollision[t.importer.selectedCollisionObject].object,0),  LimbScaleZ(t.importerCollision[t.importer.selectedCollisionObject].object,0) );
		LockObjectOn (  t.importerCollision[t.importer.collisionShapeCount].object2 );
		PositionObject (  t.importerCollision[t.importer.collisionShapeCount].object2 , ObjectPositionX(t.importerCollision[t.importer.selectedCollisionObject].object) ,  ObjectPositionY(t.importerCollision[t.importer.selectedCollisionObject].object),  ObjectPositionZ(t.importerCollision[t.importer.selectedCollisionObject].object) );
		SetObjectCull (  t.importerCollision[t.importer.collisionShapeCount].object,0 );
		SetObjectCull (  t.importerCollision[t.importer.collisionShapeCount].object2,0 );

		PositionObject (  t.importerGridObject[9], 0 , 0 , 0 );
		t.importerCollision[t.importer.collisionShapeCount].rotx = ObjectAngleX(t.importerCollision[t.importer.selectedCollisionObject].object2);
		t.importerCollision[t.importer.collisionShapeCount].roty = ObjectAngleY(t.importerCollision[t.importer.selectedCollisionObject].object2);
		t.importerCollision[t.importer.collisionShapeCount].rotz = ObjectAngleZ(t.importerCollision[t.importer.selectedCollisionObject].object2);
		RotateObject (   t.importerCollision[t.importer.collisionShapeCount].object, ObjectAngleX(t.importerCollision[t.importer.selectedCollisionObject].object2),ObjectAngleY(t.importerCollision[t.importer.selectedCollisionObject].object2),ObjectAngleZ(t.importerCollision[t.importer.selectedCollisionObject].object2) );
		RotateObject (   t.importerCollision[t.importer.collisionShapeCount].object2, ObjectAngleX(t.importerCollision[t.importer.selectedCollisionObject].object2),ObjectAngleY(t.importerCollision[t.importer.selectedCollisionObject].object2),ObjectAngleZ(t.importerCollision[t.importer.selectedCollisionObject].object2) );
		GlueObjectToLimbEx (  t.importerCollision[t.importer.collisionShapeCount].object2,t.importerGridObject[9], 0 , 1 );
		DisableObjectZDepth (  t.importerCollision[t.importer.collisionShapeCount].object2 );
		SetObjectWireframe (  t.importerCollision[t.importer.collisionShapeCount].object2 , 1 );
		SetObjectLight (  t.importerCollision[t.importer.collisionShapeCount].object2,0 );
		HideObject (  t.importerCollision[t.importer.collisionShapeCount].object2 );
		SetObjectEffect ( t.importerCollision[t.importer.collisionShapeCount].object2, g.guiwireframeshadereffectindex );
		SetObjectEmissive ( t.importerCollision[t.importer.collisionShapeCount].object2, Rgb(255,255,255) );

		PositionObject (  t.importerGridObject[9], 0 , 0 , IMPORTERZPOSITION );
		t.importer.selectedCollisionObject = t.importer.collisionShapeCount;
		t.slidersmenuvalue[t.importer.properties2Index][7].value = t.importerCollision[t.importer.collisionShapeCount].rotx;
		t.slidersmenuvalue[t.importer.properties2Index][8].value = t.importerCollision[t.importer.collisionShapeCount].roty;
		t.slidersmenuvalue[t.importer.properties2Index][9].value = t.importerCollision[t.importer.collisionShapeCount].rotz;
		++t.importer.collisionShapeCount;
	}
}

void importer_add_collision_box_loaded ( void )
{
	if (bRemoveSprites)
		return;

	if (  t.importer.collisionShapeCount < 49 ) 
	{
		t.importerCollision[t.importer.collisionShapeCount].object = findFreeObject();
		MakeObjectBox (  t.importerCollision[t.importer.collisionShapeCount].object , 100 , 100 , 100 );
		ScaleLimb (  t.importerCollision[t.importer.collisionShapeCount].object, 0, t.tPSizeX_f * (t.tScale / 100.0) , t.tPSizeY_f * (t.tScale / 100.0) , t.tPSizeZ_f * (t.tScale / 100.0) );
		t.importerCollision[t.importer.collisionShapeCount].sizex = t.tPSizeX_f * (t.tScale / 100.0);
		t.importerCollision[t.importer.collisionShapeCount].sizey = t.tPSizeY_f * (t.tScale / 100.0);
		t.importerCollision[t.importer.collisionShapeCount].sizez = t.tPSizeZ_f * (t.tScale / 100.0);
		LockObjectOn (  t.importerCollision[t.importer.collisionShapeCount].object );
		PositionObject (  t.importerCollision[t.importer.collisionShapeCount].object, t.tPOffX_f * (t.tScale / 100.0) , (t.tPOffY_f * (t.tScale / 100.0)) , t.tPOffZ_f * (t.tScale / 100.0) );
		RotateObject (  t.importerCollision[t.importer.collisionShapeCount].object, t.tPRotX_f , t.tPRotY_f , t.tPRotZ_f );
		SetObjectEffect ( t.importerCollision[t.importer.collisionShapeCount].object, g.guishadereffectindex );
		SetObjectEmissive ( t.importerCollision[t.importer.collisionShapeCount].object, Rgb(255,255,255) );
		SetAlphaMappingOn ( t.importerCollision[t.importer.collisionShapeCount].object, 10 );

		PositionObject (  t.importerGridObject[9], 0 , 0 , 0 );
		t.importerCollision[t.importer.collisionShapeCount].rotx = t.tPRotX_f;
		t.importerCollision[t.importer.collisionShapeCount].roty = t.tPRotY_f;
		t.importerCollision[t.importer.collisionShapeCount].rotz = t.tPRotZ_f;

		GlueObjectToLimbEx (  t.importerCollision[t.importer.collisionShapeCount].object,t.importerGridObject[9], 0 , 1 );
		DisableObjectZDepth (  t.importerCollision[t.importer.collisionShapeCount].object );
		SetObjectLight (  t.importerCollision[t.importer.collisionShapeCount].object,0 );
		HideObject (  t.importerCollision[t.importer.collisionShapeCount].object );
		t.importerCollision[t.importer.collisionShapeCount].object2 = findFreeObject();
		MakeObjectBox (  t.importerCollision[t.importer.collisionShapeCount].object2 , 100,100,100 );
		ScaleLimb (  t.importerCollision[t.importer.collisionShapeCount].object2, 0, t.tPSizeX_f * (t.tScale / 100.0) , t.tPSizeY_f * (t.tScale / 100.0) , t.tPSizeZ_f * (t.tScale / 100.0) );
		LockObjectOn (  t.importerCollision[t.importer.collisionShapeCount].object2 );
		PositionObject (  t.importerCollision[t.importer.collisionShapeCount].object2, t.tPOffX_f * (t.tScale / 100.0) , (t.tPOffY_f * (t.tScale / 100.0)) , t.tPOffZ_f * (t.tScale / 100.0) );
		RotateObject (  t.importerCollision[t.importer.collisionShapeCount].object2, t.tPRotX_f , t.tPRotY_f , t.tPRotZ_f );
		SetObjectCull (  t.importerCollision[t.importer.collisionShapeCount].object,0 );
		SetObjectCull (  t.importerCollision[t.importer.collisionShapeCount].object2,0 );
		SetObjectEffect ( t.importerCollision[t.importer.collisionShapeCount].object2, g.guiwireframeshadereffectindex );
		SetObjectEmissive ( t.importerCollision[t.importer.collisionShapeCount].object2, Rgb(255,255,255) );

		PositionObject (  t.importerGridObject[9], 0 , 0 , 0 );
		t.importerCollision[t.importer.collisionShapeCount].rotx = t.tPRotX_f;
		t.importerCollision[t.importer.collisionShapeCount].roty = t.tPRotY_f;
		t.importerCollision[t.importer.collisionShapeCount].rotz = t.tPRotZ_f;
		GlueObjectToLimbEx (  t.importerCollision[t.importer.collisionShapeCount].object2,t.importerGridObject[9], 0 , 1 );
		DisableObjectZDepth (  t.importerCollision[t.importer.collisionShapeCount].object2 );
		SetObjectWireframe (  t.importerCollision[t.importer.collisionShapeCount].object2 , 1 );
		SetObjectLight (  t.importerCollision[t.importer.collisionShapeCount].object2,0 );
		HideObject (  t.importerCollision[t.importer.collisionShapeCount].object2 );

		PositionObject (  t.importerGridObject[9], 0 , 0 , IMPORTERZPOSITION );

		t.importer.selectedCollisionObject = t.importer.collisionShapeCount;
		t.slidersmenuvalue[t.importer.properties2Index][7].value = t.tPRotX_f;
		t.slidersmenuvalue[t.importer.properties2Index][8].value = t.tPRotY_f;
		t.slidersmenuvalue[t.importer.properties2Index][9].value = t.tPRotZ_f;
		++t.importer.collisionShapeCount;
	}
}

void importer_delete_collision_box ( void )
{
	if (bRemoveSprites)
		return;
	if (  t.importer.collisionShapeCount > 0 && t.importer.selectedCollisionObject >= 0)
	{
		DeleteObject (  t.importerCollision[t.importer.selectedCollisionObject].object );
		t.importerCollision[t.importer.selectedCollisionObject].object = 0;
		DeleteObject (  t.importerCollision[t.importer.selectedCollisionObject].object2 );
		t.importerCollision[t.importer.selectedCollisionObject].object2 = 0;

		for ( int tCount = t.importer.selectedCollisionObject ; tCount<=  t.importer.collisionShapeCount-1; tCount++ )
		{
			t.importerCollision[tCount].object = t.importerCollision[tCount+1].object;
			t.importerCollision[tCount].object2 = t.importerCollision[tCount+1].object2;
			t.importerCollision[tCount].sizex = t.importerCollision[tCount+1].sizex;
			t.importerCollision[tCount].sizey = t.importerCollision[tCount+1].sizey;
			t.importerCollision[tCount].sizez = t.importerCollision[tCount+1].sizez;
			t.importerCollision[tCount].rotx = t.importerCollision[tCount+1].rotx;
			t.importerCollision[tCount].roty = t.importerCollision[tCount+1].roty;
			t.importerCollision[tCount].rotz = t.importerCollision[tCount+1].rotz;
		}

		--t.importer.collisionShapeCount;
		t.importer.selectedCollisionObject = t.importer.collisionShapeCount-1;
		if (  t.importer.selectedCollisionObject  >=  0 ) 
		{
			t.slidersmenuvalue[t.importer.properties2Index][7].value = t.importerCollision[t.importer.selectedCollisionObject].rotx;
			t.slidersmenuvalue[t.importer.properties2Index][8].value = t.importerCollision[t.importer.selectedCollisionObject].roty;
			t.slidersmenuvalue[t.importer.properties2Index][9].value = t.importerCollision[t.importer.selectedCollisionObject].rotz;
		}
		if (  t.importer.collisionShapeCount > 0 ) 
		{
			if (  t.importerCollision[t.importer.selectedCollisionObject].object > 0 && t.importerCollision[t.importer.selectedCollisionObject].object2 > 0 ) 
			{
				if (  ObjectExist(t.importerCollision[t.importer.selectedCollisionObject].object)  ==  1 && ObjectExist(t.importerCollision[t.importer.selectedCollisionObject].object2)  ==  1 ) 
				{
					RotateObject (  t.importerCollision[t.importer.selectedCollisionObject].object, t.slidersmenuvalue[t.importer.properties2Index][7].value, t.slidersmenuvalue[t.importer.properties2Index][8].value, t.slidersmenuvalue[t.importer.properties2Index][9].value );
					RotateObject (  t.importerCollision[t.importer.selectedCollisionObject].object2, t.slidersmenuvalue[t.importer.properties2Index][7].value, t.slidersmenuvalue[t.importer.properties2Index][8].value, t.slidersmenuvalue[t.importer.properties2Index][9].value );
					t.importerCollision[t.importer.selectedCollisionObject].rotx = t.slidersmenuvalue[t.importer.properties2Index][7].value;
					t.importerCollision[t.importer.selectedCollisionObject].roty = t.slidersmenuvalue[t.importer.properties2Index][8].value;
					t.importerCollision[t.importer.selectedCollisionObject].rotz = t.slidersmenuvalue[t.importer.properties2Index][9].value;
				}
			}
		}
	}
}

void importer_checkForShaderFiles ( void )
{
	t.tOriginalPath_s = t.importer.startDir;
	if (  strcmp ( Right(t.tOriginalPath_s.Get(),1) , "\\" ) != 0  )  t.tOriginalPath_s  =  t.tOriginalPath_s + "\\";
	SetDir (  cstr(t.tOriginalPath_s + "effectbank\\reloaded").Get() );
	t.importer.shaderFileCount = 0;
	FindFirst (  );
	do
	{
		t.ts_s = GetFileName();
		t.ts_s = Lower(t.ts_s.Get());

		//  Remove the shaders that are not for imported entities
		if ( t.ts_s == "apbr_core.fx" ) t.ts_s = "";
		if ( t.ts_s == "apbr_terrain.fx" ) t.ts_s = "";
		if ( t.ts_s == "cascadeshadows.fx" ) t.ts_s = "";
		if ( t.ts_s == "character_editor.fx" ) t.ts_s = "";
		if ( t.ts_s == "character_static.fx" ) t.ts_s = "";
		if ( t.ts_s == "constantbuffers.fx" ) t.ts_s = "";
		if ( t.ts_s == "dynamicterrainshadow_basic.fx" ) t.ts_s = "";
		if ( t.ts_s == "ebe_basic.fx" ) t.ts_s = "";
		if ( t.ts_s == "entity_core.fx" ) t.ts_s = "";
		if ( t.ts_s == "gui_basic.fx" ) t.ts_s = "";
		if ( t.ts_s == "gui_diffuse.fx" ) t.ts_s = "";
		if ( t.ts_s == "gui_showdepth.fx" ) t.ts_s = "";
		if ( t.ts_s == "gui_wireframe.fx" ) t.ts_s = "";
		if ( t.ts_s == "post-bloom.fx" ) t.ts_s = "";
		if ( t.ts_s == "post-core.fx" ) t.ts_s = "";
		if ( t.ts_s == "post-none.fx" ) t.ts_s = "";
		if ( t.ts_s == "post-rift.fx" ) t.ts_s = "";
		if ( t.ts_s == "post-sao.fx" ) t.ts_s = "";
		if ( t.ts_s == "scatter.fx" ) t.ts_s = "";
		if ( t.ts_s == "settings.fx" ) t.ts_s = "";
		if ( t.ts_s == "shadow_basic.fx" ) t.ts_s = "";
		if ( t.ts_s == "sky_basic.fx" ) t.ts_s = "";
		if ( t.ts_s == "sky_core.fx" ) t.ts_s = "";
		if ( t.ts_s == "sky_scroll.fx" ) t.ts_s = "";
		if ( t.ts_s == "skyscroll_basic.fx" ) t.ts_s = "";
		if ( t.ts_s == "sprite_basic.fx" ) t.ts_s = "";
		if ( t.ts_s == "static_basic.fx" ) t.ts_s = "";
		if ( t.ts_s == "terrain_basic.fx" ) t.ts_s = "";
		if ( t.ts_s == "vegetation_basic.fx" ) t.ts_s = "";
		if ( t.ts_s == "water_basic.fx" ) t.ts_s = "";
		if ( t.ts_s == "weapon_basic.fx" ) t.ts_s = "";
		if ( t.ts_s == "weapon_bone.fx" ) t.ts_s = "";
		if ( strcmp ( Right(t.ts_s.Get(),3) , ".fx" ) == 0 ) 
		{
			if ( t.importer.shaderFileCount < IMPORTERSHADERFILESMAX ) 
			{
				t.importerShaderFiles[t.importer.shaderFileCount+1] = t.ts_s;
				++t.importer.shaderFileCount;
			}
		}
		FindNext (  );
	} while ( !(  GetFileType() == -1 ) );
	SetDir (  t.importer.startDir.Get() );
}

void importer_checkForScriptFiles ( void )
{
	t.tOriginalPath_s = t.importer.startDir;
	if (  cstr(Right(t.tOriginalPath_s.Get(),1))  !=  "\\"  )  t.tOriginalPath_s  =  t.tOriginalPath_s + "\\";
	SetDir (  cstr(t.tOriginalPath_s + "scriptbank").Get() );

	t.importer.scriptFileCount = 0;

	FindFirst (  );
	do
	{

		t.ts_s = GetFileName();
		t.ts_s = Lower(t.ts_s.Get());

		if (  cstr(Right(t.ts_s.Get(),4))  ==  ".lua" ) 
		{
			if (  t.importer.scriptFileCount < IMPORTERSCRIPTFILESMAX ) 
			{
				t.importerScriptFiles[t.importer.scriptFileCount+1] = t.ts_s;
				++t.importer.scriptFileCount;
			}
		}
		FindNext (  );
	} while ( !(  GetFileType() == -1 ) );

	SetDir (  t.importer.startDir.Get() );
}

void importer_help ( void )
{
	if (bRemoveSprites)
		return;
	if (  t.inputsys.mclick  ==  1 && t.importer.oldMouseClick  ==  0 ) 
	{
		if (  t.importer.MouseX  >=  SpriteX(t.importer.helpSprite) && t.importer.MouseX  <=  SpriteX(t.importer.helpSprite) + 32 && t.importer.MouseY >=  SpriteY(t.importer.helpSprite) && t.importer.MouseY  <=  SpriteY(t.importer.helpSprite) + 32 ) 
		{
			t.importer.helpShow = 1 - t.importer.helpShow;
			t.importer.oldMouseClick = 1;
			if (  t.importer.helpShow  ==  1  )  t.importer.helpFade  =  0;
		}
		else
		{
			t.importer.helpShow = 0;
		}
	}

	if (  t.importer.helpShow  ==  0 ) 
	{
		SetSpriteDiffuse (  t.importer.helpSprite , 255,255,255 );
		if (  SpriteExist(t.importer.helpSprite4) ) 
		{
			SetSpriteAlpha (  t.importer.helpSprite4, t.importer.helpFade );
			if (  t.importer.helpFade > 0  )  t.importer.helpFade  =  t.importer.helpFade - 20;
			if (  t.importer.helpFade  <=  0 ) 
			{
				t.importer.helpFade = 0;
				DeleteSprite (  t.importer.helpSprite4 );
				if (  ImageExist(g.importermenuimageoffset+9)  )  DeleteImage (  g.importermenuimageoffset+9 );
			}
			else
			{
				Sprite (  t.importer.helpSprite4 , (GetChildWindowWidth()/2) - 303 , (GetChildWindowHeight()/2) - 213, g.importermenuimageoffset+9 );
			}
		}
		return;
	}

	t.importer.message ="Left click to hide help";

	SetSpriteDiffuse (  t.importer.helpSprite , 100,100,100 );

	t.tx = 40;
	t.ty = 64;

	if (  ImageExist(g.importermenuimageoffset+9)  ==  0 ) 
	{
		CreateBitmap (  32,605,435 );
		SetCurrentBitmap (  32 );

		CLS (  Rgb(28,34,39) );

		pastebitmapfont("Importer Help",(605/2) - getbitmapfontwidth("Importer Help",1)/2,20,1,255);

		t.tT_s = "Properties Shortcut Keys";
		pastebitmapfont(t.tT_s.Get(),t.tx,t.ty,1,255);

		t.ty += 32;
		t.tT_s = "When the Properties tab is selected, clicking and dragging on the object will allow you to rotate it.";
		pastebitmapfont(t.tT_s.Get(),t.tx,t.ty,2,255);
		t.ty += 20;
		t.tT_s  =  "Holding shift while dragging the object will limit the rotation to 45 degree steps" ;
		pastebitmapfont(t.tT_s.Get(),t.tx,t.ty,2,255);

		t.ty += 40;
		t.tT_s = "Collision Shortcut Keys";
		pastebitmapfont(t.tT_s.Get(),t.tx,t.ty,1,255);

		t.ty += 32;
		t.tT_s = "When the Collsion tab is selected, You can change the view by using the cursor keys:";
		pastebitmapfont(t.tT_s.Get(),t.tx,t.ty,2,255);

		t.ty += 20;
		t.tT_s = "UP: Top view, LEFT: Left view, RIGHT: Right View, DOWN: Front view";
		pastebitmapfont(t.tT_s.Get(),t.tx,t.ty,2,255);

		t.ty += 32;
		t.tT_s = "When resizing t.a collision Box ( , you can limit t.movement to an axis ):";
		pastebitmapfont(t.tT_s.Get(),t.tx,t.ty,2,255);

		t.ty += 20;
		t.tT_s = "X: Limit to X axis, C: Limit to Y axis, V: Limit to Z axis";
		pastebitmapfont(t.tT_s.Get(),t.tx,t.ty,2,255);

		t.ty += 20;
		t.tT_s = "You can limit more than one axis at t.a time by holding multiple keys down";
		pastebitmapfont(t.tT_s.Get(),t.tx,t.ty,2,255);

		t.ty += 20;
		t.tT_s  =  "Holding shift while dragging collision boxes will snap them to other boxes" ; 
		pastebitmapfont(t.tT_s.Get(),t.tx,t.ty,2,255);

		t.ty += 40;
		t.tT_s = "Thumbnail Shortcut Keys";
		pastebitmapfont(t.tT_s.Get(),t.tx,t.ty,1,255);

		t.ty += 32;
		t.tT_s = "When the Thumbnail tab is selected, clicking and dragging on the object will allow you to rotate it.";
		pastebitmapfont(t.tT_s.Get(),t.tx,t.ty,2,255);
		t.ty += 20;
		t.tT_s  =  "Holding shift while dragging the object will limit the rotation to 45 degree steps" ; 
		pastebitmapfont(t.tT_s.Get(),t.tx,t.ty,2,255);

		GrabImage (  g.importermenuimageoffset+9,0,0,605,435,3 );
		SetCurrentBitmap (  0 );
		DeleteBitmapEx (  32 );
	}
	Sprite (  t.importer.helpSprite4 , (GetChildWindowWidth()/2) - 303 , (GetChildWindowHeight()/2) - 213, g.importermenuimageoffset+9 );
	SetSpritePriority (  t.importer.helpSprite4,2 );
	SetSpriteAlpha (  t.importer.helpSprite4, t.importer.helpFade );
	if (  t.importer.helpFade < 255  )  t.importer.helpFade  =  t.importer.helpFade + 20;
	if (  t.importer.helpFade > 255  )  t.importer.helpFade  =  255;
}

void importer_screenSwitch ( void )
{ 
	if (bRemoveSprites)
		return;

	if (  t.importer.scaleMulti > 1.0 ) 
	{
		Sprite (  t.importer.helpSprite , (GetChildWindowWidth()/2) - 170 + 128 , 0, g.importermenuimageoffset+8 );
	}
	else
	{
		Sprite (  t.importer.helpSprite , (GetChildWindowWidth()/2) - 170 + 128 , (GetChildWindowHeight()/2) - 300 - 4, g.importermenuimageoffset+8 );
	}

	if (  t.importer.scaleMulti > 1.0 ) 
	{
		t.importerTabs[1].y = 0;
		t.importerTabs[2].y = 0;
		t.importerTabs[3].y = 0;
		t.importerTabs[4].y = 0;
		t.importerTabs[5].y = 0;
		t.importerTabs[11].y = 0;
		t.slidersmenu[t.importer.properties1Index].tleft= GetChildWindowWidth() - 255;
		t.slidersmenu[t.importer.properties2Index].tleft= GetChildWindowWidth() - 255;
		t.slidersmenu[t.importer.properties3Index].tleft= GetChildWindowWidth() - 255;
		t.slidersmenu[t.importer.properties4Index].tleft= GetChildWindowWidth() - 255;

		//  New button
		t.importerTabs[6].x = GetChildWindowWidth() - 159;
		t.importerTabs[7].x = GetChildWindowWidth() - 159;
		t.importerTabs[8].x = GetChildWindowWidth() - 159;
		t.importerTabs[9].x = GetChildWindowWidth() - 159;
		t.importerTabs[10].x = GetChildWindowWidth() - 159;
		t.importerTabs[12].x = GetChildWindowWidth() - 159;

	}
	else
	{
		t.slidersmenu[t.importer.properties1Index].tleft= (GetChildWindowWidth() / 2 ) + 330;
		t.slidersmenu[t.importer.properties2Index].tleft= (GetChildWindowWidth() / 2 ) + 330;
		t.slidersmenu[t.importer.properties3Index].tleft= (GetChildWindowWidth() / 2 ) + 330;
		t.slidersmenu[t.importer.properties4Index].tleft= (GetChildWindowWidth() / 2 ) + 330;
	
		t.importerTabs[1].x = (GetChildWindowWidth() / 2) + 65-128 + 250;
		t.importerTabs[1].y = (GetChildWindowHeight() / 2) - 304;

		t.importerTabs[2].x = t.importerTabs[1].x + 128;
		t.importerTabs[2].y = t.importerTabs[1].y;

		t.importerTabs[3].x = t.importerTabs[2].x + 128;
		t.importerTabs[3].y = t.importerTabs[1].y;

		t.importerTabs[4].x = t.importerTabs[3].x + 256;
		t.importerTabs[4].y = t.importerTabs[1].y;

		//  Importer button
		t.importerTabs[5].x = (GetChildWindowWidth() / 2) - 320;
		t.importerTabs[5].y = (GetChildWindowHeight() / 2) - 304;

		//  New button
		t.importerTabs[6].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
		t.importerTabs[6].y = t.importerTabs[1].y + 125;

		//  New button
		t.importerTabs[12].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
		t.importerTabs[12].y = t.importerTabs[1].y + 125 + 38;

		//  Delete button
		t.importerTabs[7].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
		t.importerTabs[7].y = t.importerTabs[1].y + 125 + (38*2);

		//  Next button
		t.importerTabs[8].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
		t.importerTabs[8].y = t.importerTabs[1].y + 125 + (38*3);

		//  Previous button
		t.importerTabs[9].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
		t.importerTabs[9].y = t.importerTabs[1].y + 125 + (38*4);

		//  Show Guide button
		t.importerTabs[10].x = t.importerTabs[4].x - 28 + 10 - 256 + 1;
		t.importerTabs[10].y = t.importerTabs[1].y + 125 + (38*10) - 5;

		//  Importer button
		t.importerTabs[11].x = (GetChildWindowWidth() / 2) - 320 +128;
		t.importerTabs[11].y = (GetChildWindowHeight() / 2) - 304;
	}
}

// 
//  File dialog functions
// 

int findFreeDll ( void )
{
	int tFoundFree = 0;
	int tCount = 0;

	tFoundFree = 0;

	for ( tCount = 1 ; tCount<=  50; tCount++ )
	{
		if (  DLLExist ( tCount )  ==  0 ) 
		{
			tFoundFree = tCount;
			break;
		}
	}
	return tFoundFree;
}

int findFreeMemblock ( void )
{
	int tFoundFree = 0;
	int tCount = 0;

	tFoundFree = 0;

	for ( tCount = 1 ; tCount<=  50; tCount++ )
	{
		if (  MemblockExist (  tCount )  ==  0 ) 
		{
			tFoundFree = tCount;
			break;
		}
	}
	return tFoundFree;
}

#ifdef WICKEDENGINE
char* openFileBox(char* filter_, char* initdir_, char* dtitle_, char* defext_, unsigned char open)
{
	cStr tOldDir = GetDir();
	char* cFileSelected = NULL;
	cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, filter_, initdir_, NULL);
	SetDir(tOldDir.Get());
	if (cFileSelected && strlen(cFileSelected) > 0)
		return cFileSelected;
	else
		return "";
}
#else
char* openFileBox ( char* filter_, char* initdir_, char* dtitle_, char* defext_, unsigned char open )
{
	LPSTR filebufferptr = 0;
	int OPENFILENAME = 0;
	cstr filebuffer =  "";
	int comdlg32 = 0;
	cstr tCode_s =  "";
	int tCount2 = 0;
	cstr code_s =  "";
	LPSTR retval = 0;
	int tFound = 0;
	int user32 = 0;
	DWORD flags = 0;
	LPSTR lpofn = 0;
	DWORD hwnd = 0;
	int size = 0;
	char filter[512];
	char initdir[512];
	char dtitle[512];
	char defext[512];

	strcpy ( filter , filter_ );
	strcpy ( initdir , initdir_ );
	strcpy ( dtitle , dtitle_ );
	strcpy ( defext , defext_ );

	#ifdef VRTECH
	//if ( t.importer.loaded == 1 ) importer_show_mouse ( );
	#else
	if ( t.importer.loaded == 1 ) importer_show_mouse ( );
	#endif
	
	//  Load in required DLL's
	user32 = findFreeDll();
	DLLLoad (  "user32.dll",user32 );
	comdlg32 = findFreeDll();
	DLLLoad (  "comdlg32.dll",comdlg32 );

	//  Get handle ( unique ID ) to the calling ( this ) window
	hwnd = CallDLL(user32,"GetForegroundWindow");

	//  Unload DLL as it is no longer needed
	DLLDelete (  user32 );

	//  Get the Memblock Number
	OPENFILENAME = findFreeMemblock();

	//  Make The Memblock containing the OPENFILENAME structure
	MakeMemblock (  OPENFILENAME,76 );

	//  Get the pointer to the just created Structure
	lpofn = GetMemblockPtr(OPENFILENAME);

	strcat ( filter , "|" );
	strcat ( initdir ,  "|" );
	strcat ( dtitle , "|" );
	strcat ( defext , "|" );
	filebuffer = "" ; filebuffer = filebuffer + "|";
	filebuffer+= Spaces(255);
	filebuffer+= "|";
	filebufferptr = (LPSTR)_get_str_ptr(filebuffer.Get());
	flags = 0x00001000 || 0x00000004 || 0x00000002;
	size = 0;

	// warning - nasty 32 bit only code here!!
	WriteMemblockDWord (  OPENFILENAME,0,76 );
	WriteMemblockDWord (  OPENFILENAME,4,hwnd );
	WriteMemblockDWord (  OPENFILENAME,12,(DWORD)_get_str_ptr(filter) );
	WriteMemblockDWord (  OPENFILENAME,24,1 );
	WriteMemblockDWord (  OPENFILENAME,28,(DWORD)filebufferptr );
	WriteMemblockDWord (  OPENFILENAME,32,256 );
	WriteMemblockDWord (  OPENFILENAME,44,(DWORD)_get_str_ptr(initdir) );
	WriteMemblockDWord (  OPENFILENAME,48,(DWORD)_get_str_ptr(dtitle) );
	WriteMemblockDWord (  OPENFILENAME,52,flags );
	WriteMemblockDWord (  OPENFILENAME,60,(DWORD)_get_str_ptr(defext) );

	//  Call the Command to open the dialouge
	if ( open )
	{
		retval = (LPSTR)CallDLL(comdlg32,"GetOpenFileNameA",(DWORD)lpofn);
	}
	else
	{
		retval = (LPSTR)CallDLL(comdlg32,"GetSaveFileNameA",(DWORD)lpofn);
	}
	//  Check if it was sucecfull
	if  ( retval !=  0 )
	{
		code_s = _get_str(filebufferptr,256);
		tCode_s = "";
		tFound = 0;
		for ( tCount2 = Len(code_s.Get()) ; tCount2>=  1 ; tCount2+= -1 )
		{
			if (  code_s.Get()[tCount2-1]  !=  ' '  )  tFound  =  1;
			t.strwork = ""; t.strwork = t.strwork +  Mid(code_s.Get(),tCount2) + tCode_s;;
			if (  tFound  !=  0  )  tCode_s  = t.strwork;
		}
		code_s = tCode_s;
	}
	else
	{
		retval = (LPSTR)CallDLL(comdlg32,"CommDlgExtendedError");
		switch ( (DWORD)retval )
		{
			case 0xFFFF :  code_s  =  "The dialog box could not be created. The common dialog box function's call to the DialogBox function failed. For example, this error occurs if the common dialog box call specifies an invalid window handle."; break;
			case 0x0006 : code_s = "The common dialog box function failed to find a specified resource." ; break;
			case 0x0004 : code_s = "The ENABLETEMPLATE flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a corresponding instance handle." ; break;
			case 0x0002 : code_s = "The common dialog box function failed during initialization. This error often occurs when sufficient memory is not available." ; break;
			case 0x000B : code_s = "The ENABLEHOOK flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a pointer to a corresponding hook procedure." ; break;
			case 0x0008 : code_s = "The common dialog box function failed to lock a specified resource." ; break;
			case 0x0003 : code_s = "The ENABLETEMPLATE flag was set in the Flags member of the initialization structure for the corresponding common dialog box, but you failed to provide a corresponding template." ; break;
			case 0x0007 : code_s = "The common dialog box function failed to load a specified string." ; break;
			case 0x0001 : code_s = "The lStructSize member of the initialization structure for the corresponding common dialog box is invalid." ; break;
			case 0x0005 : code_s = "The common dialog box function failed to load a specified string." ;break;
			case 0x3003 : code_s = "The buffer pointed to by the lpstrFile member of the OPENFILENAME structure is too small for the file name specified by the user. The first two bytes of the lpstrFile buffer contain an integer value specifying the size, in TCHARs, required to receive the full name." ; break;
			case 0x0009 : code_s = "The common dialog box function was unable to allocate memory for internal structures." ; break;
			case 0x3002 : code_s = "A file name is invalid." ; break;
			case 0x000A : code_s = "The common dialog box function was unable to lock the memory associated with a handle." ; break;
			case 0x3001 : code_s = "An attempt to subclass a list box failed because sufficient memory was not available." ; break;
			default : code_s = "Error" ; break;
		}
	}

	DLLDelete (  comdlg32 );

	DeleteMemblock (  OPENFILENAME );

	strcpy ( t.szreturn , code_s.Get() );

	#ifdef VRTECH
	// ensure cursor restored when leave dialog
	ChangeMouse(0);
	#endif

	return t.szreturn;
}

LPSTR _get_str_ptr ( char* pstr )
{
	int memnum = 0;
	LPSTR memptr = 0;
	int strlen = 0;
	LPSTR strptr = 0;
	char cchar = 0;

	memnum = findFreeMemblock();
	strlen = Len(pstr);

	MakeMemblock (  memnum,strlen );

	for ( int i = 1 ; i <= strlen ; i++ )
	{
		if ( pstr[i-1] == '|' )
		{
			cchar = 0;
		}
		else
		{
			cchar = pstr[i-1];
		}

		if ( cchar > 255 ) cchar = 255;
		if ( cchar < 0 ) cchar = 0;

		WriteMemblockByte (  memnum,(i - 1), (byte)cchar );
	}

	memptr = GetMemblockPtr(memnum);
	strptr = MakeMemory(strlen);

	CopyMemory (  (LPSTR) strptr, (LPSTR) memptr,strlen );

	DeleteMemblock (  memnum );

	return strptr;
}

char* _get_str ( LPSTR strptr, int strsize )
{
	int memnum = 0;
	LPSTR memptr = 0;
	cstr cchar =  "";
	cstr str = "";

	memnum = findFreeMemblock();
	MakeMemblock (  memnum,strsize );
	memptr = GetMemblockPtr(memnum);

	CopyMemory ( (LPSTR) memptr, (LPSTR) strptr,strsize );

	for ( int  i = 1 ; i <= strsize; i++ )
	{
		str = str + Chr(ReadMemblockByte(memnum,i - 1));
	}

	DeleteMemblock (  memnum );

	t.returnstring_s = str;

	return t.returnstring_s.Get();

}
#endif

char* importerPadString ( char* tString )
{
	t.returnstring_s = tString;

	while (  Len(t.returnstring_s.Get()) < 17 ) 
	{
		t.returnstring_s += " ";
	}

	return t.returnstring_s.Get();
}

int findFreeObject ( void )
{
	int found = 0;

	found = g.importermenuobjectoffset;
	while (  ObjectExist ( found )  ==  1 )
	{
		++found;
	}
	return found;
}

float GetDistance ( float x1, float  y1, float  z1, float  x2, float  y2, float  z2 )
{
	float result_f = 0;
	float tXd_f = 0;
	float tYd_f = 0;
	float yZd_f = 0;

	tXd_f = x2 - x1;
	tYd_f = y2 - y1;
	yZd_f = z2 - z1;

	result_f = Sqrt ( tXd_f * tXd_f + tYd_f * tYd_f + yZd_f * yZd_f );

	return result_f;
}

int importer_check_if_protected ( char* timporterfilecheck_s )
{
	int timporterprotectedcheck = 0;
	cstr tStippedFileName_s =  "";
	cstr tStippedString_s =  "";
	int tArrayMarker = 0;
	cstr tempLine_s =  "";
	cstr tstring_s =  "";
	cstr tToken2_s =  "";
	cstr tToken_s =  "";
	int tCount = 0;

	timporterprotectedcheck = 0;

	//  Ensure it has the dbo extension
	Dim (  t.tArray,400  );
	tArrayMarker = 0;
	tstring_s=timporterfilecheck_s;
	tToken_s=FirstToken(tstring_s.Get(),".");
	if (  tToken_s  !=  "" ) 
	{
		t.tArray[tArrayMarker] = tToken_s;
		++tArrayMarker;
	}
	do
	{
		tToken_s=NextToken(".");
		if (  tToken_s  !=  "" ) 
		{
			t.tArray[tArrayMarker] = tToken_s;
			++tArrayMarker;
		}
	} while ( !(  tToken_s == "" ) );
	tStippedFileName_s = "";
	for ( tCount = 0 ; tCount<=  tArrayMarker-2; tCount++ )
	{
		tStippedFileName_s = tStippedFileName_s + t.tArray[tCount];
	}
	UnDim (  t.tArray );
	Dim (  t.tArray,400  );

	tStippedFileName_s = timporterfilecheck_s;
	if (  cstr(Mid(tStippedFileName_s.Get(),Len(tStippedFileName_s.Get())-1))  ==  "." ) 
	{
			tStippedFileName_s = Left(tStippedFileName_s.Get(),Len(tStippedFileName_s.Get())-2);
	}
	else
	{
		tStippedFileName_s = Left(tStippedFileName_s.Get(),Len(tStippedFileName_s.Get())-4);
	}
	t.strwork = ""; t.strwork =  tStippedFileName_s + ".fpe";
	strcpy ( timporterfilecheck_s , t.strwork.Get() );

	//  Check if an FPE exists, if so load it in
	if (  FileOpen(1)  )  CloseFile (1) ;
	if (  FileExist (timporterfilecheck_s) ) 
	{
		OpenToRead (  1 , timporterfilecheck_s );
		while (  FileEnd(1)  ==  0 ) 
		{
			t.tstring_s = ReadString (  1 );
			tempLine_s = t.tstring_s;

			tArrayMarker = 0;
			tToken_s=FirstToken(t.tstring_s.Get()," ");
			if (  tToken_s  !=  "" ) 
			{
				t.tArray[tArrayMarker] = tToken_s;
				++tArrayMarker;
			}
			do
			{
				tToken_s=NextToken(" ");
				if (  tToken_s  !=  "" ) 
				{
					t.tArray[tArrayMarker] = tToken_s;
					++tArrayMarker;
				}
			} while ( !(  tToken_s == "" ) );
			tStippedString_s = "";
			for ( tCount = 0 ; tCount<=  tArrayMarker-1; tCount++ )
			{
				if (  tCount < 3 ) 
				{
					tStippedString_s = tStippedString_s + t.tArray[tCount];
				}
				else
				{
					tStippedString_s = tStippedString_s + " " + t.tArray[tCount];
				}
			}
			if (  tStippedString_s  !=  "" && tStippedString_s.Get()[0]  !=  ';' )
			{
				tToken_s=FirstToken(tStippedString_s.Get(),"=");
				tToken2_s=NextToken("=");

				//  Get rid of any tabs that exist and replace with nothing (some files have tabs in sometimes)
				t.tstring_s = tToken_s ; tToken_s = "";
				for ( tCount = 1 ; tCount<=  Len(t.tstring_s.Get()); tCount++ )
				{
					if ( cstr( Mid(t.tstring_s.Get(),tCount))  !=  Chr(9)  )  tToken_s  =  tToken_s + Mid(t.tstring_s.Get(),tCount);
				}

				t.tstring_s = tToken2_s ; tToken2_s = "";
				for ( tCount = 1 ; tCount<=  Len(t.tstring_s.Get()); tCount++ )
				{
					if (  cstr(Mid(t.tstring_s.Get(),tCount))  !=  Chr(9)  )  tToken2_s  =  tToken2_s + Mid(t.tstring_s.Get(),tCount);
				}

				//  find out which token we have and set the importer fpe string
				if ( tToken_s == "protected" ) timporterprotectedcheck  =  ValF(tToken2_s.Get()) ;
			}
		}

	}

	CloseFile (  1 );
	UnDim (  t.tArray );

	if (  timporterprotectedcheck > 1  )  timporterprotectedcheck  =  1;
	if (  timporterprotectedcheck < 0  )  timporterprotectedcheck  =  0;

	return timporterprotectedcheck;
}

void importer_sort_names ( void )
{
	//  Split the filename into tokens to grab the path, object name and create fpe name
	Dim (  t.tArray,400  );
	t.tArrayMarker = 0;
	t.tstring_s=t.timporterfile_s;
	t.tToken_s=FirstToken(t.tstring_s.Get(),"\\");
	if (  t.tToken_s  !=  "" ) 
	{
		t.tArray[t.tArrayMarker] = t.tToken_s;
		++t.tArrayMarker;
	}
	do
	{
		t.tToken_s=NextToken("\\");
		if (  t.tToken_s  !=  "" ) 
		{
			t.tArray[t.tArrayMarker] = t.tToken_s;
			++t.tArrayMarker;
		}
	} while ( !(  t.tToken_s == "" ) );
	t.tStippedFileName_s = "";

	//  Grab path only
	int tCount = 0;
	for ( tCount = 0 ; tCount<=  t.tArrayMarker-2; tCount++ )
	{
		t.tStippedFileName_s = t.tStippedFileName_s + t.tArray[tCount] + "\\";
	}

	//  Store file path
	t.importer.objectFileOriginalPath = t.tStippedFileName_s;
	if (  cstr(Left(t.importer.objectFileOriginalPath.Get(),2))  ==  ".." ) 
	{
		t.tFPSDir_s = t.importer.startDir;
		t.tFPSDir_s = Left(t.tFPSDir_s.Get(),Len(t.tFPSDir_s.Get())-6);
		t.importer.objectFileOriginalPath = t.tFPSDir_s + Right(t.importer.objectFileOriginalPath.Get(),Len(t.importer.objectFileOriginalPath.Get())-2);
	}

	//  Now store just the file names
	t.tStippedFileName_s = t.tArray[t.tArrayMarker-1];
	t.tOriginalName_s = t.tStippedFileName_s;
	t.tArrayMarker = 0;
	t.tstring_s=t.tStippedFileName_s;
	t.tToken_s=FirstToken(t.tstring_s.Get(),".");
	if (  t.tToken_s  !=  "" ) 
	{
		t.tArray[t.tArrayMarker] = t.tToken_s;
		++t.tArrayMarker;
	}
	do
	{
		t.tToken_s=NextToken(".");
		if (  t.tToken_s  !=  "" ) 
		{
			t.tArray[t.tArrayMarker] = t.tToken_s;
			++t.tArrayMarker;
		}
	} while ( !(  t.tToken_s == "" ) );
	t.tStippedFileName_s = "";
	for ( tCount = 0 ; tCount<=  t.tArrayMarker-2; tCount++ )
	{
		t.tStippedFileName_s = t.tStippedFileName_s + t.tArray[tCount];
	}

	t.tStippedFileName_s = t.tOriginalName_s;
	if (  cstr(Mid(t.tStippedFileName_s.Get(),Len(t.tStippedFileName_s.Get())-1))  ==  "." ) 
	{
			t.tStippedFileName_s = Left(t.tStippedFileName_s.Get(),Len(t.tStippedFileName_s.Get())-2);
	}
	else
	{
		t.tStippedFileName_s = Left(t.tStippedFileName_s.Get(),Len(t.tStippedFileName_s.Get())-4);
	}

	//  Store file names
	t.importer.objectFilename = t.tStippedFileName_s + "." + t.tArray[t.tArrayMarker-1];
	t.importer.objectFilenameFPE = t.tStippedFileName_s +  ".fpe";
	t.importer.objectFilenameExtension = cstr(".") + t.tArray[t.tArrayMarker-1];
	UnDim (  t.tArray );
	
	//  check if it is an fpe that has been loaded in, if so we need to change the model name
	if (  cstr(Lower(Right(t.timporterfile_s.Get(),4)))  ==  ".fpe" ) 
	{
		t.importer.fpeIsMainFile = 1;
		importer_find_object_name_from_fpe ( );
	}
	else
	{
		t.importer.fpeIsMainFile = 0;
	}
}

void importer_find_object_name_from_fpe ( void )
{
	t.timporterprotectedcheck = 0;
	Dim (  t.tArray,400  );
	t.timporterfilecheck_s = t.timporterfile_s;

	//  Check if an FPE exists, if so load it in
	if ( FileOpen(1) )  CloseFile (1);
	if (  FileExist (t.timporterfilecheck_s.Get()) ) 
	{
		OpenToRead (  1 , t.timporterfilecheck_s.Get() );
		while (  FileEnd(1)  ==  0 ) 
		{
			t.tstring_s = ReadString (  1 );
			t.tempLine_s = t.tstring_s;

			t.tArrayMarker = 0;
			t.tToken_s=FirstToken(t.tstring_s.Get()," ");
			if (  t.tToken_s  !=  "" ) 
			{
				t.tArray[t.tArrayMarker] = t.tToken_s;
				++t.tArrayMarker;
			}
			do
			{
				t.tToken_s=NextToken(" ");
				if (  t.tToken_s  !=  "" ) 
				{
					t.tArray[t.tArrayMarker] = t.tToken_s;
					++t.tArrayMarker;
				}
			} while ( !(  t.tToken_s == "" ) );
			t.tStippedString_s = "";
			int tCount = 0;
			for ( tCount = 0 ; tCount<=  t.tArrayMarker-1; tCount++ )
			{
				if (  tCount < 3 ) 
				{
					t.tStippedString_s = t.tStippedString_s + t.tArray[tCount];
				}
				else
				{
					t.tStippedString_s = t.tStippedString_s + " " + t.tArray[tCount];
				}
			}
			if (  t.tStippedString_s  !=  "" && t.tStippedString_s.Get()[0]  !=  ';' )
			{
				t.tToken_s=FirstToken(t.tStippedString_s.Get(),"=");
				t.tToken2_s=NextToken("=");

				//  Get rid of any tabs that exist and replace with nothing (some files have tabs in sometimes)
				t.tstring_s = t.tToken_s ; t.tToken_s = "";
				for ( int tCount2 = 1 ; tCount2 <=  Len(t.tstring_s.Get()); tCount2++ )
				{
					if (  cstr(Mid(t.tstring_s.Get(),tCount2))  !=  Chr(9)  )  t.tToken_s  =  t.tToken_s + Mid(t.tstring_s.Get(),tCount2);
				}

				t.tstring_s = t.tToken2_s ; t.tToken2_s = "";
				for ( tCount = 1 ; tCount<=  Len(t.tstring_s.Get()); tCount++ )
				{
					if ( cstr( Mid(t.tstring_s.Get(),tCount) ) !=  Chr(9)  )  t.tToken2_s  =  t.tToken2_s + Mid(t.tstring_s.Get(),tCount);
				}

				if ( t.tToken_s == "model" ) t.importer.objectFilename  =  t.tToken2_s ;
			}
		}

	}

	CloseFile (  1 );
	UnDim (  t.tArray );

return;

}

void importer_hide_mouse ( void )
{
	HideMouse (  );
}

void importer_show_mouse ( void )
{
	ShowMouse (  );
}

void importer_fade_out ( void )
{
	t.tfound = 0;
	t.twhiteobj = 1;
	while (  t.tfound  ==  0 ) 
	{
		if (  ObjectExist(t.twhiteobj)  ==  1 ) 
		{
			++t.twhiteobj;
		}
		else
		{
			t.tfound = 1;
		}
	}

	MakeObjectBox (  t.twhiteobj,200000,200000,1 );
	SetObjectLight (  t.twhiteobj,1 );
	LockObjectOn (  t.twhiteobj );
	PositionObject (  t.twhiteobj,0,0,4000 );
	DisableObjectZDepth (  t.twhiteobj );
	SetAlphaMappingOn (  t.twhiteobj,0 );
	ColorObject (  t.twhiteobj , Rgb(0,0,0) );
	SetObjectAmbience (  t.twhiteobj,0 );
	SetObjectEmissive (  t.twhiteobj, Rgb(40,104,131) );

	for ( t.tc = 0 ; t.tc <=  100 ; t.tc += 10 )
	{
			SetAlphaMappingOn (  t.twhiteobj,t.tc );
			Sync (  );
	}
	DeleteObject (  t.twhiteobj );
}

void importer_fade_in ( void )
{
	t.tfound = 0;
	t.twhiteobj = 1;
	while (  t.tfound  ==  0 ) 
	{
		if (  ObjectExist(t.twhiteobj)  ==  1 ) 
		{
			++t.twhiteobj;
		}
		else
		{
			t.tfound = 1;
		}
	}

	MakeObjectBox (  t.twhiteobj,200000,200000,1 );
	SetObjectLight (  t.twhiteobj,1 );
	LockObjectOn (  t.twhiteobj );
	PositionObject (  t.twhiteobj,0,0,4000 );
	DisableObjectZDepth (  t.twhiteobj );
	SetAlphaMappingOn (  t.twhiteobj,100 );
	ColorObject (  t.twhiteobj , Rgb(0,0,0) );
	SetObjectAmbience (  t.twhiteobj,0 );
	SetObjectEmissive (  t.twhiteobj, Rgb(40,104,131) );

	for ( t.tc = 100 ; t.tc >=  0 ; t.tc+= -10 )
	{
			SetAlphaMappingOn (  t.twhiteobj,t.tc );
			Sync (  );
	}
	DeleteObject (  t.twhiteobj );
}

void importer_check_script_token_exists ( void )
{
	t.tfound = 0;
	for ( t.tloop = 0 ; t.tloop<=  t.importer.scriptFileCount-1; t.tloop++ )
	{
			if (  t.tToken2_s  ==  t.importerScriptFiles[t.tloop]  )  t.tfound  =  1;
	}

	if (  t.tfound  ==  0  )  t.tToken2_s  =  "default.lua";
}

void importer_update_scale ( void )
{
	if (bRemoveSprites)
		return;

	if (  t.inputsys.mclick  ==  0 ) 
	{
		t.importer.dropDownListNumber = 0;
		t.importer.oldTime = 0;

		if (  t.timporterprevscale  !=  t.slidersmenuvalue[t.importer.properties1Index][1].value ) 
		{
			t.timporterprevscale = t.slidersmenuvalue[t.importer.properties1Index][1].value;
			t.slidersmenuvalue[t.importer.properties1Index][1].value_s=Str(t.slidersmenuvalue[t.importer.properties1Index][1].value);
			t.slidersmenuvalue[t.importer.properties1Index][1].readmodeindex=50;
			t.slidersmenuvalue[t.importer.properties1Index][1].useCustomRange = 1;
			t.slidersmenuvalue[t.importer.properties1Index][1].valueMin = t.slidersmenuvalue[t.importer.properties1Index][1].value-49;
			t.slidersmenuvalue[t.importer.properties1Index][1].valueMax = t.slidersmenuvalue[t.importer.properties1Index][1].value+50;
			if (  t.slidersmenuvalue[t.importer.properties1Index][1].valueMin < 1 ) 
			{
				t.slidersmenuvalue[t.importer.properties1Index][1].valueMin = 1;
				t.slidersmenuvalue[t.importer.properties1Index][1].valueMax = 100;//400; for better lower scale resolution work
			}
		}
	}
}

/* does not seem to be used any more
float GetScaleMultiply(int iObj)
{
	float fScaleMultiply = 1.0;
	float objsize = ObjectSize(iObj, 1); //include scale.
	if (objsize < 0.1) {
		//Resize so object is visible.
		fScaleMultiply = 100.0;
	}
	else if (objsize < 1.0) {
		//Resize so object is visible.
		fScaleMultiply = 20.0;
	}
	else if (objsize < 2.0) {
		//Resize so object is visible.
		fScaleMultiply = 10.0;
	}
	else if (objsize > 5000.0) {
		//Resize so object is visible.
		fScaleMultiply = 0.05;
	}
	else if (objsize > 3000.0) {
		//Resize so object is visible.
		fScaleMultiply = 0.1;
	}
	else if (objsize > 2000.0) {
		//Resize so object is visible.
		fScaleMultiply = 0.5;
	}
	return(fScaleMultiply);
}
*/

#ifdef WICKEDENGINE
int update_all_count = 0;

void Wicked_Change_Object_Material(void* pVObject, int mode, entityeleproftype *edit_grideleprof, bool bFromCustomMaterials)
{
	
	//Mode 0 Allow selection of all meshes.
	//Mode 1 (NOT USED) Single mesh , texture all meshes with the same material.
	//Mode 3 EBE
	//Mode 5 readonly.

	if (t.importer.importerActive && !t.importer.bEditAllMesh)
	{
		bFromCustomMaterials = !bUseRGBAButtons;
	}

	// store eleprof being edited
	if (!edit_grideleprof)
	{
		edit_grideleprof = &t.grideleprof;
	}

	// update grideleprof if EBE editing
	if ( mode == 0 || mode == 3)
		bUpdateGrideleprof = true;
	else
		bUpdateGrideleprof = false;

	// copy material to grideleprof if not latest
	if (bUpdateGrideleprof && bHaveMaterialUpdate) 
	{
		Wicked_Copy_Material_To_Grideleprof(pVObject, mode, edit_grideleprof);
		if (mode == 0 || mode == 3) 
		{
			// update wicked settings on all meshes.
			update_all_count = 25;
		}
		bHaveMaterialUpdate = false;
	}

	// count down and trigger ALL materials in object to be updated
	if (update_all_count > 0) 
	{
		update_all_count--;
		if (update_all_count == 0) 
		{
			Wicked_Update_All_Materials(pVObject, mode);
		}
	}

	// get object ptr
	sObject* pObject = (sObject*) pVObject;
	if (!pObject) return;

	// can only act on one mesh at a time (chosen by a dropdown for object)
	sMesh* pChosenMesh = NULL;

	// width of panel
	float w = ImGui::GetContentRegionAvailWidth();

	// mesh dropdown only for objects and readonly, not EBE
	int comboflags = ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_HeightLarge;
	if (mode == 0 || mode == 5)
	{
		bool bDisplayCombo = true;
		if (mode == 5)
		{
			// choose mesh
			sMesh* pSelMesh = NULL;
			int iMeshes = 0;
			for (int i = 0; i < pObject->iMeshCount; i++)
			{
				sMesh* pMesh = pObject->ppMeshList[i];
				if (pMesh)
				{
					if (!pSelMesh)
					{
						pSelMesh = pMesh;
					}
					iMeshes++;
				}
			}
			if (iMeshes==1) 
			{
				if (pSelMesh)
				{
					pChosenMesh = pSelMesh;
				}
				bDisplayCombo = false;
			}
		}

		// if have mehses, display drop down to select one
		if (bDisplayCombo)
		{
			char meshname[MAX_PATH] = "";
			ImGui::PushItemWidth(-10);
			if (ImGui::BeginCombo("##combolastsearch", &mesh_combo_entry[0], comboflags))
			{
				// If selected, the user will be able to apply a single texture to all meshes in the object.
				if (ImGui::Selectable("Edit All Mesh Materials"))
				{
					strcpy(mesh_combo_entry, "Edit All Mesh Materials");

					t.importer.bEditAllMesh = true;
				}

				int count = pObject->iMeshCount;
				if (count > MAXMESHMATERIALS)
					count = MAXMESHMATERIALS;
				for (int i = 0; i < count; i++)
				{
					sMesh* pMesh = pObject->ppMeshList[i];
					if (pMesh)
					{	
						// Get names for the mesh from the base colour texture, unless they have already been created.
						if (!t.importer.bModelMeshNamesSet)
						{
							sFrame* pFrame = nullptr;
							if (i + 1 < pObject->iFrameCount)
							{
								// Sometimes objects will have a root node frame that doesn't have any meshes
								pFrame = pObject->ppFrameList[i + 1];
							}
							else
							{
								pFrame = pObject->ppFrameList[i];
							}
							if (pFrame && strlen(pFrame->szName) > 0 && strcmp(pFrame->szName, "sibling frame"))
							{
								t.importer.cModelMeshNames.push_back(pFrame->szName);
							}
							else
							{
								// Crops the filepath and extension, to leave only the mesh name.
								LPSTR pNameFromMesh = pMesh->pTextures[0].pName;

								extern void Wicked_CreateShortName(int, LPSTR, LPSTR);
								Wicked_CreateShortName(i, meshname, pNameFromMesh);

								t.importer.cModelMeshNames.push_back(meshname);
							}
							if (i == (pObject->iMeshCount - 1))
								t.importer.bModelMeshNamesSet = true;
						}
						else
						{
							strcpy(meshname, t.importer.cModelMeshNames[i].Get());
						}

						// assign correct mesh based on mesh_combo_entry
						//bool is_selected = false;
						//if (strcmp(mesh_combo_entry, meshname) == NULL)
						//{
						//	is_selected = true;
						//}
						bool is_selected = iSelectedMesh == i;
						ImGui::PushID(5723 + i);
						if (ImGui::Selectable(meshname, is_selected))
						{
							strcpy(mesh_combo_entry, meshname);
							iSelectedMesh = i;
							t.importer.bEditAllMesh = false;

							if (iSelectedMesh >= MAXMESHMATERIALS - 1) //PE: We can crash if we go above the MAXMESHMATERIALS.
								iSelectedMesh = MAXMESHMATERIALS - 1;
							pChosenMesh = pMesh;
						}
						ImGui::PopID();
						if (is_selected) ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select this mesh from all the meshes stored in the object");
			ImGui::PopItemWidth();
		}
	}

	// if no mesh found/selected, find first in object
	if (!pChosenMesh) 
	{
		for (int i = 0; i < pObject->iMeshCount; i++)
		{
			if (pObject->ppMeshList == nullptr)
			{
				// Got a crash here, check for valid mesh list
				// Not getting the crash in debug, and have a valid mesh list the following frame
				// Likely related to this issue:https://github.com/TheGameCreators/GameGuruRepo/issues/3276
				//   Memory corruption occurs somewhere during the entity copy and paste process
				return;
			}
			sMesh* pMesh = pObject->ppMeshList[i];
			if (pMesh && pMesh->wickedmeshindex > 0)
			{
				if (mode == 3)
				{
					// select first mesh if EBE
					iSelectedMesh = i;
					if (iSelectedMesh >= MAXMESHMATERIALS - 1) //PE: We can crash if we go above the MAXMESHMATERIALS.
						iSelectedMesh = MAXMESHMATERIALS - 1;
					pChosenMesh = pMesh;
					break;
				}
				else
				{
					// select chosen mesh otherwise
					if (iSelectedMesh == i)
					{
						pChosenMesh = pMesh;
						break;
					}
				}
			}
		}
	}

	bool launch_file = false;

	// with mesh available to edit
	if (pChosenMesh)
	{
		// ensure we are aligned with the chosen mesh material
		WickedSetMeshNumber(iSelectedMesh);

		// LB: ensure newly selected mesh updates material
		static int iOldSelectedMesh;
		if (iSelectedMesh != iOldSelectedMesh)
		{
			iOldSelectedMesh = iSelectedMesh;
			Wicked_Set_Material_From_grideleprof_ThisMesh((void*)pObject, 0, edit_grideleprof, iSelectedMesh);
		}

		// get mesh from DBO mesh ptr
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pChosenMesh->wickedmeshindex);
		if (mesh)
		{
			// get material from mesh
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
			if (pObjectMaterial)
			{
				// get IMGUI panel settings
				float col_start = ImGui::GetCursorPosX() + 80.0f;
				float help_start = ImGui::GetCursorPosX() + 60.0f;
				int preview_icon_size = ImGui::GetFontSize();
				float path_gadget_size = ImGui::GetFontSize()*2.0;
				if (mode == 3 || mode == 5) path_gadget_size = 4.0f;

				// vars for material view/editing
				int tCount = 1;
				char materialname[MAX_PATH];
				memset(materialname, 0, sizeof(materialname));
				/*bool launch_file = false;*/

				// if read only
				int mode5_materials = 0;
				int mode5_displayed_materials = 0;
				float mode5_icon_size = 32.0f;
				if (mode == 5)
				{
					if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].resource && pObjectMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].resource)
						mode5_materials++;
					if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::NORMALMAP].resource && pObjectMaterial->textures[MaterialComponentTEXTURESLOT::NORMALMAP].resource)
						mode5_materials++;
					if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].resource && pObjectMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].resource)
						mode5_materials++;
					if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::EMISSIVEMAP].resource && pObjectMaterial->textures[MaterialComponentTEXTURESLOT::EMISSIVEMAP].resource)
						mode5_materials++;

					//Padding,Margin.
					if (mode5_materials > 0) mode5_icon_size = ImGui::GetContentRegionAvailWidth() / mode5_materials;
					mode5_icon_size -= 16.0f;
					if (mode5_icon_size < 20.0f) mode5_icon_size = 20.0f;
				}

				// read only states if EBE or readonly
				int iInputFlags = ImGuiInputTextFlags_EnterReturnsTrue;
				if (mode == 3 || mode == 5)	iInputFlags = ImGuiInputTextFlags_ReadOnly;

				// base color
				ImVec4 mycolor;
				bool open_popup;
				ImGuiWindow* window;
				ID3D11ShaderResourceView* lpTexture;
				ImVec2 vDrawPos;

				// base color editing (if not readonly)
				if (mode != 5)
				{
					// mesh base color
					BaseColor[0] = pChosenMesh->mMaterial.Diffuse.r;
					BaseColor[1] = pChosenMesh->mMaterial.Diffuse.g;
					BaseColor[2] = pChosenMesh->mMaterial.Diffuse.b;
					BaseColor[3] = pChosenMesh->mMaterial.Diffuse.a;
					ImGui::TextCenter("Base Color");
					mycolor = ImVec4(BaseColor[0], BaseColor[1], BaseColor[2], BaseColor[3]);
					open_popup = ImGui::ColorButton("##NewV2WickedBaseColor", mycolor, 0, ImVec2(w - 10.0, 0));
					if (open_popup) ImGui::OpenPopup("##pickWickedBaseColor");
					if (ImGui::BeginPopup("##pickWickedBaseColor", ImGuiWindowFlags_NoMove))
					{
						if (ImGui::ColorPicker4("##BaseColor", &BaseColor[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_DisplayRGB)) 
						{
							pChosenMesh->mMaterial.Diffuse.r = BaseColor[0];
							pChosenMesh->mMaterial.Diffuse.g = BaseColor[1];
							pChosenMesh->mMaterial.Diffuse.b = BaseColor[2];
							pChosenMesh->mMaterial.Diffuse.a = BaseColor[3];
							dwBaseColor = ((unsigned int)(BaseColor[0] * 255) << 24);
							dwBaseColor += ((unsigned int)(BaseColor[1] * 255) << 16);
							dwBaseColor += ((unsigned int)(BaseColor[2] * 255) << 8);
							dwBaseColor += ((unsigned int)(BaseColor[3] * 255));
							WickedCall_SetMeshMaterial(pChosenMesh);
							bHaveMaterialUpdate = true;

							importer_set_all_material_colour(0, BaseColor);
						}
						ImGui::EndPopup();
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Base Color");
					window = ImGui::GetCurrentWindow(); //PE: Add a pencil to all color gadgets.
					lpTexture = GetImagePointerView(TOOL_PENCIL);
					vDrawPos = { ImGui::GetCursorScreenPos().x + (ImGui::GetContentRegionAvail().x - 30.0f) ,ImGui::GetCursorScreenPos().y - (ImGui::GetFontSize()*1.5f) - 3.0f };
					if (lpTexture)
						window->DrawList->AddImage((ImTextureID)lpTexture, vDrawPos, vDrawPos + ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));
				}

				// loop through all textures required
				for (int texslot = 0; texslot < 6; texslot++)
				{
					// texture gadget labels
					LPSTR pInputLabel = "", pInputBtnLabel = "", pInputControlLabel = "";
					if (texslot == 0) { pInputLabel = "##InputMeshColor"; pInputBtnLabel = "...##InputMeshColorFile"; pInputControlLabel = "##AlphaRef"; }
					if (texslot == 1) { pInputLabel = "##InputMeshNormal"; pInputBtnLabel = "...##InputMeshNormalFile"; pInputControlLabel = "##NormalStrength"; }
					if (texslot == 2 && t.importer.importerActive)
					{
						if (ImGui::Checkbox("Use RGBA buttons##importersurfacemode", &bUseRGBAButtons))
						{
							// Choose whether to select surface texture directly, or take data from individual channels of a source texture.
						}
					}
					if (bFromCustomMaterials == true)
					{
						// viewed from object properties or when editing all meshes.
						if (texslot == 2) { pInputLabel = "##InputMeshSurface"; pInputBtnLabel = "...##InputMeshSurfaceFile"; pInputControlLabel = "##RoughnessStrength"; }
						if (texslot == 3) { pInputLabel = ""; pInputBtnLabel = ""; pInputControlLabel = "##MetalnessStrength"; }
						if (texslot == 4) { pInputLabel = ""; pInputBtnLabel = ""; pInputControlLabel = ""; }
					}
					else
					{
							// viewed from importer
							if (texslot == 2) { pInputLabel = "##InputMeshRoughness"; pInputBtnLabel = "...##InputMeshRoughnessFile"; pInputControlLabel = "##RoughnessStrength"; }
							if (texslot == 3) { pInputLabel = "##InputMeshMetalness"; pInputBtnLabel = "...##InputMeshMetalnessFile"; pInputControlLabel = "##MetalnessStrength"; }
							if (texslot == 4) { pInputLabel = "##InputMeshOcclusion"; pInputBtnLabel = "...##InputMeshOcclusionFile"; pInputControlLabel = "##OcclusionStrength"; }						
					}
					if (texslot == 5) { pInputLabel = "##InputMeshEmissive"; pInputBtnLabel = "...##InputMeshEmissiveFile"; pInputControlLabel = "##EmissiveStrength"; }

					// texture types
					LPSTR pTitle = "", pTip = "";
					if (texslot == 0) { pTitle = "Albedo"; pTip = "Allows you to change the albedo / color texture of the object"; }
					if (texslot == 1) { pTitle = "Normal"; pTip = "Allows you to change the normal map texture of the object";  }
					if (bFromCustomMaterials == true)
					{
						if (texslot == 2) { pTitle = "Surface"; pTip = "Allows you to change the surface texture of the object."; }
						if (texslot == 3) { pTitle = ""; pTip = ""; }
						if (texslot == 4) { pTitle = ""; pTip = ""; }
					}
					else
					{
						if (texslot == 2) { pTitle = "Roughness"; pTip = "Allows you to change the roughness texture of the object."; }
						if (texslot == 3) { pTitle = "Metalness"; pTip = "Allows you to change the metalness texture of the object."; }
						if (texslot == 4) { pTitle = "Occlusion"; pTip = "Allows you to change the ambient occlusion texture of the object."; }
					}
					if (texslot == 5) { pTitle = "Emissive"; pTip = "Allows you to change the emissive texture of the object."; }

					// control title and desc
					LPSTR pControlTitle = "", pControlTip = "";
					if (texslot == 0) { pControlTitle = "Alpha Clipping";  pControlTip = "Alpha channel values below this value are clipped and not rendered."; }
					if (texslot == 1) { pControlTitle = "Normal Strength";  pControlTip = "Controls how strong the normal mapping effect is on the object."; }
					if (texslot == 2) { pControlTitle = "Roughness Strength";  pControlTip = "Controls how much of the roughness texture is applied to the object."; }
					if (texslot == 3) { pControlTitle = "Metalness Strength";  pControlTip = "Controls how much of the metalness texture is applied to the object."; }
					if (texslot == 4) { pControlTitle = "";  pControlTip = ""; }
					if (texslot == 5) { pControlTitle = "Emissive Strength";  pControlTip = "Controls how much of the emissive texture is applied to the object."; }

					// extra info message and image
					LPSTR pTexSlotInfo = "", pTexSlotInfoImage = "";
					if (texslot == 0) { pTexSlotInfo = "An albedo texture is an image texture that represents the color of an object, before any lighting calculations have been applied to it."; pTexSlotInfoImage = ""; }
					if (texslot == 1) { pTexSlotInfo = "A normal map texture is a special type of image that describes the undulations of a surface. Each pixel represents an angle offset away from the surface - for example it could be the edge of a rock or how clothes have folds in them. Normal maps are used to add extra depth and detail to an object without increasing the number of polygons, thus allowing for more polygons to be used elsewhere in your game."; pTexSlotInfoImage = ""; }
					if (bFromCustomMaterials == true)
					{
						if (texslot == 2) { pTexSlotInfo = "Surface textures represent the physically-based material of an object. It combines ambient occlusion, roughness and metalness into its RGBA channels. Surface textures are used to control how light interacts with an object and alters the extent to which it resembles metal, wood or another material type from the real world. This allows for the object to look physically accurate."; pTexSlotInfoImage = ""; }
						if (texslot == 3) { pTexSlotInfo = ""; pTexSlotInfoImage = ""; }
						if (texslot == 4) { pTexSlotInfo = ""; pTexSlotInfoImage = ""; }
					}
					else
					{
						if (texslot == 2) { pTexSlotInfo = "A roughness map describes which part of the surface is rough, and by inference, which parts are smooth and potentially glossy. Roughness is placed into the green channel of the surface texture."; pTexSlotInfoImage = ""; }
						if (texslot == 3) { pTexSlotInfo = "A metalness map is an image that describes how much metal is represented on the surface, with white being fully metallic and black being no metal at all. Metalness is placed into the blue channel of the surface texture."; pTexSlotInfoImage = ""; }
						if (texslot == 4) { pTexSlotInfo = "An ambient occlusion (AO) map describes how much baked-in shadows are part of the surface, and can be used to add very small scale lighting information to compliment the global lighting system. Ambient Occlusion is placed into the red channel of the surface texture."; pTexSlotInfoImage = ""; }
					}
					if (texslot == 5) { pTexSlotInfo = "An emissive map describes which parts of the object will generate its own light. By varying the emissive strength, you can alter how much of the surface color is projected outward from the object. Example use cases include using an emissive map to produce a glow effect, or adding lighting to a TV screen placed in your level."; pTexSlotInfoImage = ""; }

					// the actual wicked slot, and iDelayedExecute update code
					int iDelayedExecuteCodeForThisSlot = 0;
					MaterialComponentTEXTURESLOT wickedTextureSlot;
					if (texslot == 0) { 
						wickedTextureSlot = MaterialComponentTEXTURESLOT::BASECOLORMAP; 
						iDelayedExecuteCodeForThisSlot = 30;
						if (t.importer.bEditAllMesh)
							iDelayedExecuteCodeForThisSlot = 50;
					}
					if (texslot == 1) 
					{ 
						wickedTextureSlot = MaterialComponentTEXTURESLOT::NORMALMAP; 
						iDelayedExecuteCodeForThisSlot = 31; 
						if (t.importer.bEditAllMesh)
							iDelayedExecuteCodeForThisSlot = 51;
					}
					if (bFromCustomMaterials == true)
					{
						if (texslot == 2) 
						{ 
							wickedTextureSlot = MaterialComponentTEXTURESLOT::SURFACEMAP; 
							iDelayedExecuteCodeForThisSlot = 44; 
							if (t.importer.bEditAllMesh)
								iDelayedExecuteCodeForThisSlot = 52;
						}
						if (texslot == 3) 
						{
							wickedTextureSlot = MaterialComponentTEXTURESLOT::SURFACEMAP; 
							iDelayedExecuteCodeForThisSlot = 44; 
							if (t.importer.bEditAllMesh)
								iDelayedExecuteCodeForThisSlot = 52;
						}
						if (texslot == 4) 
						{ 
							wickedTextureSlot = MaterialComponentTEXTURESLOT::SURFACEMAP;
							iDelayedExecuteCodeForThisSlot = 44;
							if (t.importer.bEditAllMesh)
								iDelayedExecuteCodeForThisSlot = 52;
						}
					}
					else
					{
						if (texslot == 2) { wickedTextureSlot = MaterialComponentTEXTURESLOT::SURFACEMAP; iDelayedExecuteCodeForThisSlot = 42; }
						if (texslot == 3) { wickedTextureSlot = MaterialComponentTEXTURESLOT::SURFACEMAP; iDelayedExecuteCodeForThisSlot = 43; }
						if (texslot == 4) { wickedTextureSlot = MaterialComponentTEXTURESLOT::SURFACEMAP; iDelayedExecuteCodeForThisSlot = 41; }
					}
					if (texslot == 5) 
					{ 
						wickedTextureSlot = MaterialComponentTEXTURESLOT::EMISSIVEMAP;
						iDelayedExecuteCodeForThisSlot = 34; 
						if (t.importer.bEditAllMesh)
							iDelayedExecuteCodeForThisSlot = 53;
					}

					// current texture name
					if (texslot >= 2 && texslot <= 4)
					{
						// from DBO mesh texture ref (for surface components)
						if (pChosenMesh->dwTextureCount > GG_MESH_TEXTURE_OCCLUSION)
						{
							// takes reference from DBO
							if (bFromCustomMaterials == true)
							{
								if (texslot == 2) strcpy(materialname, pChosenMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName);
								if (texslot == 3) strcpy(materialname, pChosenMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName);
								if (texslot == 4) strcpy(materialname, pChosenMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName);
							}
							else
							{
								if (texslot == 2) strcpy(materialname, pChosenMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName);
								if (texslot == 3) strcpy(materialname, pChosenMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName);
								if (texslot == 4) strcpy(materialname, pChosenMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].pName);
							}
						}
						else
						{
							// has no reference for now
							strcpy(materialname, "");
						}
					}
					else
					{
						// or direct from wicked material resource
						if (pObjectMaterial->textures[wickedTextureSlot].resource)
							strcpy(materialname, pObjectMaterial->textures[wickedTextureSlot].name.c_str());
						else
							strcpy(materialname, "");
					}

					// layout for specific texture type
					if (mode == 5)
					{
						// show as grid of texture if readonly
						if (pObjectMaterial->textures && pObjectMaterial->textures[wickedTextureSlot].resource) 
						{
							void *pmat = (void *)pObjectMaterial->textures[wickedTextureSlot].GetGPUResource();
							ImGui::ImgBtnWicked((void*)pmat, ImVec2(mode5_icon_size, mode5_icon_size), ImColor(0, 0, 0, 255));
							if (ImGui::IsItemHovered())
							{
								ImGui::BeginTooltip();
								ImGui::ImgBtnWicked((void*)pmat, ImVec2(280, 280), ImColor(0, 0, 0, 255));
								ImGui::TextCenter(pTitle);
								ImGui::EndTooltip();
							}
							mode5_displayed_materials++;
							if (mode5_displayed_materials < mode5_materials)
								ImGui::SameLine();
						}
					}
					else
					{
						// is the texture slot a valid one
						bool bValidTexSlot = false;
						if (strlen(pTitle) > 0) bValidTexSlot = true;

						// title available
						ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
						if ( bValidTexSlot == true )
						{
							// title valid
							ImGui::Text(pTitle);

							// extra help icon
							ImGui::SameLine();
							ImGui::SetCursorPos(ImVec2(help_start, ImGui::GetCursorPosY() - 5));
							ImGui::PushID(iInfoUniqueId++);
							if (ImGui::ImgBtn(ICON_INFO, ImVec2(preview_icon_size, preview_icon_size), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false))
							{
								// Display additional information on click.
								cInfoMessage = pTexSlotInfo;
								cInfoImage = pTexSlotInfoImage;
								bInfo_Window = true;
							}
							ImGui::PopID();
							if (ImGui::IsItemHovered()) ImGui::SetTooltip(pTip);
						}

						// make room for RGBA buttons
						ImGui::SameLine();
						bool bShowRGBAButtons = false;
						if (texslot >= 2 && texslot <= 4 && bFromCustomMaterials==false && bUseRGBAButtons)
						{
							bShowRGBAButtons = true;
							ImGui::PushItemWidth(-50 - path_gadget_size);
						}
						else
						{
							ImGui::PushItemWidth(-10 - path_gadget_size);
						}

						// name of texture
						if ( bValidTexSlot == true )
						{
							if (ImGui::InputText(pInputLabel, &materialname[0], MAX_PATH, iInputFlags))
							{
								pSelectedMaterial = pObjectMaterial;
								pSelectedMesh = pChosenMesh;
								iDelayedExecute = iDelayedExecuteCodeForThisSlot;
								strcpy(cPreSelectedFile, materialname);
								if (strlen(cPreSelectedFile) == 0)
								{
									// triggers relevant texture data to be erased from surface texture
									iDelayedExecuteChannel = -2;
								}
							}
							if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip((materialname && *materialname) ? materialname : "Select Texture File");
							#ifdef WICKEDENGINE
							if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;
							#endif
						}
						ImGui::PopItemWidth();

						// if not readonly or EBE, allow new texture file to be specified
						if (mode != 3 && mode != 5)
						{
							// control R,G,B,A channels
							if (bShowRGBAButtons==true && bFromCustomMaterials==false)
							{
								// Decide which buttons should be darkened to convey to the user which channel the texture data will go into.
								int iBoldTexSlot = -1;
								switch (texslot)
								{
								case 2: iBoldTexSlot = 1;// Green
									break;
								case 3: iBoldTexSlot = 2;// Blue
									break;
								case 4: iBoldTexSlot = 0;// Red
									break;
								}
								ImGui::SameLine();
								ImGui::PushItemWidth(path_gadget_size);
								char pInputTextureChannel[MAX_PATH];
								sprintf(pInputTextureChannel, "R%sR", pInputLabel);
								if (0 - iBoldTexSlot == 0)
								{
									if (ImGui::StyleButtonDark(pInputTextureChannel)) 
									{ 
										iDelayedExecuteChannel = 0;  
										launch_file = true; 
										bChooseSurfaceChannel = true;
									}
								}
								else
								{
									if (ImGui::StyleButton(pInputTextureChannel)) 
									{ 
										iDelayedExecuteChannel = 0;  
										launch_file = true;
										bChooseSurfaceChannel = true; 
									}
								}
								if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select texture file to take texture data from the red channels");
								ImGui::SameLine();
								ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX()-10, ImGui::GetCursorPosY()));
								sprintf(pInputTextureChannel, "G%sG", pInputLabel);
								if (1 - iBoldTexSlot == 0)
								{
									if (ImGui::StyleButtonDark(pInputTextureChannel)) 
									{ 
										iDelayedExecuteChannel = 1;  
										launch_file = true; 
										bChooseSurfaceChannel = true;
									}
								}
								else
								{
									if (ImGui::StyleButton(pInputTextureChannel)) 
									{ 
										iDelayedExecuteChannel = 1;  
										launch_file = true; 
										bChooseSurfaceChannel = true;
									}
								}
								if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select texture file to take texture data from the green channels");
								ImGui::SameLine();
								ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX()-10, ImGui::GetCursorPosY()));
								sprintf(pInputTextureChannel, "B%sB", pInputLabel);
								if (2 - iBoldTexSlot == 0)
								{
									if (ImGui::StyleButtonDark(pInputTextureChannel)) 
									{ 
										iDelayedExecuteChannel = 2;  
										launch_file = true;
										bChooseSurfaceChannel = true;
									}
								}
								else
								{
									if (ImGui::StyleButton(pInputTextureChannel)) 
									{
										iDelayedExecuteChannel = 2;  
										launch_file = true; 
										bChooseSurfaceChannel = true;
									}
								}
								if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select texture file to take texture data from the blue channels");
								ImGui::SameLine();
								ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX()-10, ImGui::GetCursorPosY()));
								sprintf(pInputTextureChannel, "A%sA", pInputLabel);
								if (ImGui::StyleButton(pInputTextureChannel)) { iDelayedExecuteChannel = 3;  launch_file = true; bChooseSurfaceChannel = true; }
								if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select texture file to take texture data from the alpha channels");
								if (launch_file && iDelayedExecute == 0)
								{
									pSelectedMaterial = pObjectMaterial;
									pSelectedMesh = pChosenMesh;
									strcpy(cPreSelectedFile, "");
									iDelayedExecute = iDelayedExecuteCodeForThisSlot;
								}
								ImGui::PopItemWidth();
							}
							else
							{
								if ( bValidTexSlot == true )
								{
									ImGui::SameLine();
									ImGui::PushItemWidth(path_gadget_size);
									if (ImGui::StyleButton(pInputBtnLabel))
									{
										cPreSelectedFile[0] = 0;
										launch_file = true;
									}
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select texture file to take texture data from all channels");
									if (launch_file && iDelayedExecute == 0)
									{
										pSelectedMaterial = pObjectMaterial;
										pSelectedMesh = pChosenMesh;
										strcpy(cPreSelectedFile, "");
										iDelayedExecute = iDelayedExecuteCodeForThisSlot;
									}
									ImGui::PopItemWidth();
								}
								else
								{
									if (texslot >= 2 && texslot <= 3)
									{
										// spacing for when in custom materials mode between roughness and metalness
										ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 25));
									}
								}
							}
						}

						// display texture and preview if hovered over
						if (pObjectMaterial->textures[wickedTextureSlot].resource)
						{
							if ( bValidTexSlot == true )
							{
								ImVec2 vOldPos = ImGui::GetCursorPos();
								ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 5, ImGui::GetCursorPosY() - 10));
								// LB: would be nice if the occlusion, roughness and metalness could show just the relevant channel from surface texture!
								// @Lee you can do that like this:
								// Red Only: if (ImGui::ImgBtnWicked((void*)pmat, ImVec2(preview_icon_size*2.9f, preview_icon_size*2.9f), ImColor(0, 0, 0, 255),ImColor(220, 0, 0, 255),ImColor(255, 0, 0, 255),ImColor(180, 0, 0, 255)))
								// Green Only: if (ImGui::ImgBtnWicked((void*)pmat, ImVec2(preview_icon_size*2.9f, preview_icon_size*2.9f), ImColor(0, 0, 0, 255),ImColor(0, 220, 0, 255),ImColor(0, 255, 0, 255),ImColor(0, 180, 0, 255)))
								// Blue Only: if (ImGui::ImgBtnWicked((void*)pmat, ImVec2(preview_icon_size*2.9f, preview_icon_size*2.9f), ImColor(0, 0, 0, 255),ImColor(0, 0, 220, 255),ImColor(0, 0, 255, 255),ImColor(0, 0, 180, 255)))
								// Alpha Only: would need some special setup for that :)
								
								// Flag that controls whether or not the texture will be displayed or a symbol...
								// ...that lets the user know that the meshes do not share the same texture.
								bool bMeshesHaveDifferentTex = true;
								char cTextureSettingTooltip[256];
								strcpy(cTextureSettingTooltip, "Meshes have different ");

								switch (wickedTextureSlot)
								{
									case MaterialComponentTEXTURESLOT::BASECOLORMAP:
									{
										bMeshesHaveDifferentTex = t.importer.bMeshesHaveDifferentBase;
										strcat(cTextureSettingTooltip, "base colour texture.");
										break;
									}
									case MaterialComponentTEXTURESLOT::NORMALMAP:
									{
										bMeshesHaveDifferentTex = t.importer.bMeshesHaveDifferentNormal;
										strcat(cTextureSettingTooltip, "normal texture.");
										break;
									}
									case MaterialComponentTEXTURESLOT::SURFACEMAP:
									{
										bMeshesHaveDifferentTex = t.importer.bMeshesHaveDifferentSurface;
										strcat(cTextureSettingTooltip, "surface texture.");
										break;
									}
									case MaterialComponentTEXTURESLOT::EMISSIVEMAP:
									{
										bMeshesHaveDifferentTex = t.importer.bMeshesHaveDifferentEmissive;
										strcat(cTextureSettingTooltip, "emissive texture");
										break;
									}
								}

								if (t.importer.bEditAllMesh && bMeshesHaveDifferentTex)
								{
									ImGui::PushID(iInfoUniqueId++);
									// Display the symbol that tells the user that not all meshes share the same texture.
									if (ImGui::ImgBtn(IMPORTER_ALL_MESH, ImVec2(preview_icon_size*2.9f, preview_icon_size*2.9f), ImColor(255, 255, 255, 255)))
									{
										if (mode != 3 && mode != 5)
										{
											launch_file = true;
										}
									}

									if (ImGui::IsItemHovered())
									{
										ImGui::BeginTooltip();
										ImGui::Text(cTextureSettingTooltip);
										ImGui::EndTooltip();
									}
									ImGui::PopID();
								}
								else
								{
									// Display the texture normally.
									void *pmat = (void *)pObjectMaterial->textures[wickedTextureSlot].GetGPUResource();
									if (ImGui::ImgBtnWicked((void*)pmat, ImVec2(preview_icon_size*2.9f, preview_icon_size*2.9f), ImColor(0, 0, 0, 255)))
										if (mode != 3 && mode != 5)
										{
											if (texslot >= 2 && texslot <= 4)
											{
												// Using the image button will not take texture data from a single channel, so should not copy the channel to a new texture. It should replace it entirely.
												iDelayedExecuteCodeForThisSlot = 44;
												//if (bFromCustomMaterials)
												//{
													// Editing all meshes.
													launch_file = true;
												//}
											}
											else
											{
												launch_file = true;
											}
										}
										
									if (ImGui::IsItemHovered())
									{
										ImGui::BeginTooltip();
										ImGui::ImgBtnWicked((void*)pmat, ImVec2(180, 180), ImColor(0, 0, 0, 255));
										ImGui::EndTooltip();
									}
								}

								// If the user tried to change the texture, set the correct code for the delayed execution loop.
								if (launch_file && iDelayedExecute == 0)
								{
									pSelectedMaterial = pObjectMaterial;
									pSelectedMesh = pChosenMesh;
									strcpy(cPreSelectedFile, "");
									iDelayedExecute = iDelayedExecuteCodeForThisSlot;
								}

								ImGui::SetCursorPos(vOldPos);
							}
						}

						// if not readonly or EBE, offer control value
						if ( strlen(pControlTitle) > 0 )
						{
							if (pObjectMaterial->textures[wickedTextureSlot].resource)
							{
								if (mode != 3 && mode != 5)
								{
									// control title
									ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY() - 3));
									ImGui::PushItemWidth(-10 - 4);
									ImGui::TextCenter(pControlTitle);
									ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY() - 3));

									// control slider (specific code)
									float fValue = 0.0f;
									switch (texslot)
									{
									case 0: // albedo alphaclip
										fValue = pObjectMaterial->alphaRef;
										if (ImGui::SliderFloat(pInputControlLabel, &fValue, 0.0, 1.0))
										{
											pObjectMaterial->SetAlphaRef(fValue);
											
											// This only does something if edit all meshes is enabled. 
											importer_set_all_material_settings(texslot, fValue);

											bHaveMaterialUpdate = true;
										}
										if (ImGui::IsItemHovered()) ImGui::SetTooltip(pControlTip);
										break;

									case 1: // normal map settings.
										fValue = pObjectMaterial->normalMapStrength;
										if (ImGui::SliderFloat(pInputControlLabel, &fValue, 0.0, 4.0))
										{
											pObjectMaterial->SetNormalMapStrength(fValue);

											// This only does something if edit all meshes is enabled. 
											importer_set_all_material_settings(texslot, fValue);

											bHaveMaterialUpdate = true;
										}
										if (ImGui::IsItemHovered()) ImGui::SetTooltip(pControlTip);
										ImGui::Indent(80.0f);
										if (ImGui::Checkbox("Invert Green Channel", &t.importer.bInvertNormalMap))
										{
											if (!t.importer.bInvertNormalMap)
											{
												// Revert to the original normal map.
												strcpy(cPreSelectedFile, t.importer.pOrigNormalMap);
												//t.importer.pOrigNormalMap[0] = 0;
												iDelayedExecute = 31;
												
												//WickedCall_DeleteImage(pSelectedMesh->pTextures[GG_MESH_TEXTURE_NORMAL].pName);

											}
											else
											{
												// Generate a new normal map, with an inverted green channel to solve handedness issues between Wicked and other software.
												/*strcpy(t.importer.pOrigNormalMap, pSelectedMesh->pTextures[GG_MESH_TEXTURE_NORMAL].pName);*/
												iDelayedExecute = 45;
												//fWickedCall_DeleteImage(pSelectedMesh->pTextures[GG_MESH_TEXTURE_NORMAL].pName);
											}
											
										}
										if (ImGui::IsItemHovered()) ImGui::SetTooltip("Used for fixing compatibility issues between DirectX and OpenGL normal maps");
										
										ImGui::Indent(-80.0f);
										break;
									case 2: // roughness strength
										fValue = pObjectMaterial->roughness;
										if (ImGui::SliderFloat(pInputControlLabel, &fValue, 0.0, 1.0))
										{
											pObjectMaterial->SetRoughness(fValue);

											// This only does something if edit all meshes is enabled. 
											importer_set_all_material_settings(texslot, fValue);

											bHaveMaterialUpdate = true;
											if (ImGui::IsItemHovered()) ImGui::SetTooltip(pControlTip);
										}
										break;

									case 3: // metalness strength
										fValue = pObjectMaterial->metalness;
										if (ImGui::SliderFloat(pInputControlLabel, &fValue, 0.0, 1.0))
										{
											pObjectMaterial->SetMetalness(fValue);

											// This only does something if edit all meshes is enabled. 
											importer_set_all_material_settings(texslot, fValue);

											bHaveMaterialUpdate = true;
											if (ImGui::IsItemHovered()) ImGui::SetTooltip(pControlTip);
										}
										break;

									case 4: // occlusion strength - none
										break;

									case 5: // emissive strength
										fValue = pObjectMaterial->GetEmissiveStrength();
										if (ImGui::SliderFloat(pInputControlLabel, &fValue, 0.0, 30.0))
										{
											pObjectMaterial->SetEmissiveStrength(fValue);
											pObjectMaterial->SetDirty();
											WickedCall_SetMeshMaterial(pChosenMesh);

											// This only does something if edit all meshes is enabled. 
											importer_set_all_material_settings(texslot, fValue);

											bHaveMaterialUpdate = true;
										}
										if (ImGui::IsItemHovered()) ImGui::SetTooltip(pControlTip);

										break;
									}
									//if (ImGui::IsItemHovered()) ImGui::SetTooltip(pControlTip);
									ImGui::PopItemWidth();
								}
							}
						}
						else
						{
							// no control value
							if (mode != 3 && mode != 5)
							{
								ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY() - 3));
								ImGui::PushItemWidth(-10 - 4);
								ImGui::TextCenter("");
								ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY() - 3));
								ImGui::TextCenter("");
								ImGui::PopItemWidth();
							}
						}
					}
				}

				// emissive color
				if (mode != 5)
				{
					EmmisiveColor[0] = pChosenMesh->mMaterial.Emissive.r;
					EmmisiveColor[1] = pChosenMesh->mMaterial.Emissive.g;
					EmmisiveColor[2] = pChosenMesh->mMaterial.Emissive.b;
					EmmisiveColor[3] = pChosenMesh->mMaterial.Emissive.a;
					ImGui::TextCenter("Emissive Color");
					mycolor = ImVec4(EmmisiveColor[0], EmmisiveColor[1], EmmisiveColor[2], EmmisiveColor[3]);
					open_popup = ImGui::ColorButton("##NewV2WickedEmissiveColor", mycolor, 0, ImVec2(w - 10.0, 0));
					if (open_popup)	ImGui::OpenPopup("##pickWickedEmissiveColor");
					if (ImGui::BeginPopup("##pickWickedEmissiveColor", ImGuiWindowFlags_NoMove))
					{
						if (ImGui::ColorPicker4("##EmmisiveColor", &EmmisiveColor[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_DisplayRGB)) 
						{
							pChosenMesh->mMaterial.Emissive.r = EmmisiveColor[0];
							pChosenMesh->mMaterial.Emissive.g = EmmisiveColor[1];
							pChosenMesh->mMaterial.Emissive.b = EmmisiveColor[2];
							pChosenMesh->mMaterial.Emissive.a = EmmisiveColor[3];
							dwEmmisiveColor = ((unsigned int)(EmmisiveColor[0] * 255) << 24);
							dwEmmisiveColor += ((unsigned int)(EmmisiveColor[1] * 255) << 16);
							dwEmmisiveColor += ((unsigned int)(EmmisiveColor[2] * 255) << 8);
							dwEmmisiveColor += ((unsigned int)(EmmisiveColor[3] * 255));
							WickedCall_SetMeshMaterial(pChosenMesh);
							bHaveMaterialUpdate = true;

							importer_set_all_material_colour(5, EmmisiveColor);
						}
						ImGui::EndPopup();
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change Emissive Color");
					window = ImGui::GetCurrentWindow(); //PE: Add a pencil to all color gadgets.
					lpTexture = GetImagePointerView(TOOL_PENCIL);
					vDrawPos = { ImGui::GetCursorScreenPos().x + (ImGui::GetContentRegionAvail().x - 30.0f) ,ImGui::GetCursorScreenPos().y - (ImGui::GetFontSize()*1.5f) - 3.0f };
					if (lpTexture)
						window->DrawList->AddImage((ImTextureID)lpTexture, vDrawPos, vDrawPos + ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));
				}

				if (mode != 5)
				{
					//Reflectance
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
					ImGui::Text("Reflectance");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(help_start+20, ImGui::GetCursorPosY() - 5));
					ImGui::PushID(iInfoUniqueId++);
					if (ImGui::ImgBtn(ICON_INFO, ImVec2(preview_icon_size, preview_icon_size), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false))
					{
						//Display additional information on click.
						cInfoMessage = "The reflectance of the surface describes how much of the light received by the surface is reflected away unmodified. Setting your surface to maximum reflectance would turn it into a crude mirror.";
						cInfoImage = ""; //Image that descripe this information window. "tutorialbank\\information-default.jpg".
						bInfo_Window = true; //Open information window.
					}
					ImGui::PopID();
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Allows you to change the reflectance of the object.");
					ImGui::SameLine();

					ImGui::PushItemWidth(-10 - 4);
					fReflectance = pObjectMaterial->reflectance;
					if (ImGui::SliderFloat("##Reflectance strength", &fReflectance, 0.0, 1.0)) 
					{
						pObjectMaterial->SetReflectance(fReflectance);
						bHaveMaterialUpdate = true;

						// This only does something if t.importer.bEditAllMesh is true.
						importer_set_all_material_settings(6, fReflectance);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Surface reflectance strength");
					ImGui::PopItemWidth();

					//

					//Render Order Bias
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
					ImGui::Text("Render Bias");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(help_start+20, ImGui::GetCursorPosY() - 5));
					ImGui::PushID(iInfoUniqueId++);
					if (ImGui::ImgBtn(ICON_INFO, ImVec2(preview_icon_size, preview_icon_size), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false))
					{
						//Display additional information on click.
						cInfoMessage = "The render order bias can control the priority at which an object is rendered by adding an artificial distance to the final distance sort when deciding which objects should render from the most distant to the nearest to the camera.";
						cInfoImage = ""; //Image that descripe this information window. "tutorialbank\\information-default.jpg".
						bInfo_Window = true; //Open information window.
					}
					ImGui::PopID();
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set the transparency render order distance bias to prioritise rendering of overlapping objects");
					ImGui::SameLine();

					ImGui::PushItemWidth(-10 - 4);
					float fNeedToGoInPropertiesSoCanSave = WickedCall_GetRenderOrderBias(pChosenMesh);
					if (ImGui::SliderFloat("##Transparency distance bias", &fNeedToGoInPropertiesSoCanSave, -500.0f, 500.0f))
					{
						WickedCall_SetRenderOrderBias(pChosenMesh, fNeedToGoInPropertiesSoCanSave);
						bHaveMaterialUpdate = true;

						// This only does something if t.importer.bEditAllMesh is true.
						importer_set_all_material_settings(7, fNeedToGoInPropertiesSoCanSave);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Render Order Distance Bias");
					ImGui::PopItemWidth();

					//Object checkboxes.
					bTransparent = pObjectMaterial->userBlendMode == BLENDMODE_ALPHA;
					if (ImGui::Checkbox("Transparent", &bTransparent))
					{
						if (bTransparent)
						{
							pObjectMaterial->userBlendMode = BLENDMODE_ALPHA;
							pObjectMaterial->SetDirty();
						}
						else
						{
							pObjectMaterial->userBlendMode = BLENDMODE_OPAQUE;
							pObjectMaterial->SetDirty();
						}

						if (t.importer.bEditAllMesh)
						{
							importer_set_all_material_transparent(bTransparent);
						}

						bHaveMaterialUpdate = true;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Transparent");

					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(help_start + 80, ImGui::GetCursorPosY() ));
					ImGui::PushID(iInfoUniqueId++);
					if (ImGui::ImgBtn(ICON_INFO, ImVec2(preview_icon_size, preview_icon_size), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false))
					{
						//Display additional information on click.
						cInfoMessage = "Transparency controls how much of the surface is see-through, so a low transparency would make the surface opaque (solid) and a high transparency would allow you to see through it.";
						cInfoImage = ""; //Image that descripe this information window. "tutorialbank\\information-default.jpg".
						bInfo_Window = true; //Open information window.
					}
					ImGui::PopID();
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Allows you to make the object transparent.");

					bDoubleSided = mesh->IsDoubleSided();
					if (ImGui::Checkbox("Double Sided", &bDoubleSided)) 
					{
						mesh->SetDoubleSided(bDoubleSided);

						if (t.importer.bEditAllMesh)
						{
							importer_set_all_mesh_double_sided(bDoubleSided);
						}
						bHaveMaterialUpdate = true;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set object to render both sides of its polygons");

					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(help_start + 80, ImGui::GetCursorPosY()));
					ImGui::PushID(iInfoUniqueId++);
					if (ImGui::ImgBtn(ICON_INFO, ImVec2(preview_icon_size, preview_icon_size), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false))
					{
						//Display additional information on click.
						cInfoMessage = "Objects are made up of polygons, but are only visible from one side. By setting an object to render double sided, it will render both sides of all polygons. This could be used for things like leaves and cobwebs, among many other cases.";
						cInfoImage = ""; //Image that descripe this information window. "tutorialbank\\information-default.jpg".
						bInfo_Window = true; //Open information window.
					}
					ImGui::PopID();
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Allows you to set objects so they render both sides of their polygons");

					bPlanerReflection = pObjectMaterial->shaderType == wiScene::MaterialComponent::SHADERTYPE_PBR_PLANARREFLECTION;
					if (ImGui::Checkbox("Planar Reflection", &bPlanerReflection)) 
					{
						if (bPlanerReflection) 
						{
							pObjectMaterial->shaderType = wiScene::MaterialComponent::SHADERTYPE_PBR_PLANARREFLECTION;
						}
						else 
						{
							pObjectMaterial->shaderType = wiScene::MaterialComponent::SHADERTYPE_PBR;
						}

						importer_set_all_material_planar_reflection(bPlanerReflection);

						bHaveMaterialUpdate = true;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Planar Reflection");

					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(help_start + 80, ImGui::GetCursorPosY()));
					ImGui::PushID(iInfoUniqueId++);
					if (ImGui::ImgBtn(ICON_INFO, ImVec2(preview_icon_size, preview_icon_size), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false))
					{
						//Display additional information on click.
						cInfoMessage = "Planar reflection is a technique which assumes the reflective surface is horizontal, such as a puddle or reflective floor, and can improve the visuals when specified. However, this does come with an increased performance cost.";
						cInfoImage = ""; //Image that descripe this information window. "tutorialbank\\information-default.jpg".
						bInfo_Window = true; //Open information window.
					}
					ImGui::PopID();
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Allows you to enable planer reflection on the object.");

					bCastShadows = pObjectMaterial->IsCastingShadow();
					if (ImGui::Checkbox("Cast Shadows", &bCastShadows)) 
					{

						pObjectMaterial->SetCastShadow(bCastShadows);

						importer_set_all_material_cast_shadow(bCastShadows);

						bHaveMaterialUpdate = true;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Set Cast Shadows");

					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(help_start + 80, ImGui::GetCursorPosY()));
					ImGui::PushID(iInfoUniqueId++);
					if (ImGui::ImgBtn(ICON_INFO, ImVec2(preview_icon_size, preview_icon_size), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false)) //, bBoostIconColors
					{
						//Display additional information on click.
						cInfoMessage = "All objects have the ability to cast shadows, and this ability can be switched off. You may choose to switch off shadows for small objects, or effect objects such as a puddle that is so close to the floor it does not need to cast a shadow.";
						cInfoImage = ""; //Image that descripe this information window. "tutorialbank\\information-default.jpg".
						bInfo_Window = true; //Open information window.
					}
					ImGui::PopID();
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Allows you to make the object cast shadows.");
				}
			}
		}
	}
}

void Wicked_CreateShortName ( int iMeshIndex, LPSTR pDest, LPSTR pNameFromMesh )
{
	// shorten texture name to basic material name
	char pShorten[MAX_PATH];
	strcpy (pShorten, "Noname");

	for (int n = strlen(pNameFromMesh); n > 0; n--)
	{
		if (pNameFromMesh[n] == '\\' || pNameFromMesh[n] == '/')
		{
			strcpy (pShorten, pNameFromMesh + n + 1);
			break;
		}
	}

	// If no name from texture, see if there are any names from when the object was imported into ASSIMP
	if (strlen(pNameFromMesh) == 0)
	{
		sObject* pObject = GetObjectData(t.importer.objectnumber);
		if (pObject && pObject->iMeshCount == g_MeshNamesAssimp.size())
		{
			pNameFromMesh = g_MeshNamesAssimp[iMeshIndex].Get();
			strcpy(pShorten, pNameFromMesh);
		}
	}

	LPSTR pChopString = "";
	char pLowerCaseVewrsion[MAX_PATH];
	strcpy(pLowerCaseVewrsion, pShorten);
	strlwr(pLowerCaseVewrsion);
	for (int choopychoppy = 0; choopychoppy < 7; choopychoppy++)
	{
		LPSTR pWhatToChop = "";
		if (choopychoppy == 0) pWhatToChop = ".dds";
		if (choopychoppy == 1) pWhatToChop = ".png";
		if (choopychoppy == 2) pWhatToChop = ".jpg";
		if (choopychoppy == 3) pWhatToChop = ".tga";
		if (choopychoppy == 4) pWhatToChop = "_color";
		if (choopychoppy == 5) pWhatToChop = "_basecolor";
		if (choopychoppy == 6) pWhatToChop = "_d";
		pChopString = strstr (pLowerCaseVewrsion, pWhatToChop); 
		if (pChopString)
		{
			int iChopAtPos = pChopString - pLowerCaseVewrsion;
			pShorten[iChopAtPos] = 0;
			strcpy(pLowerCaseVewrsion, pShorten);
			strlwr(pLowerCaseVewrsion);
		}
	}

	// copy it back up to be used
	sprintf(pDest, "%d:%s", iMeshIndex, pShorten);
}

void Wicked_FindChosenMesh (sObject* pObject, sMesh** ppChosenMesh, int iUseThisMeshIndex)
{
	char meshname[256];
	strcpy(mesh_combo_entry, "1:Noname\0");
	if (iUseThisMeshIndex == -1)
	{
		for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
		{
			sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
			if (pMesh && pMesh->wickedmeshindex > 0)
			{
				sFrame* pFrame = nullptr;
				if (iMeshIndex + 1 < pObject->iFrameCount)
				{
					// Sometimes objects will have a root node frame that doesn't have any meshes
					pFrame = pObject->ppFrameList[iMeshIndex + 1];
				}
				else
				{
					pFrame = pObject->ppFrameList[iMeshIndex];
				}
				if (pFrame && strlen(pFrame->szName) > 0 && strcmp(pFrame->szName, "sibling frame"))
				{
					strcpy(meshname, pFrame->szName);
				}
				else
				{
					LPSTR pNameFromMesh = pMesh->pTextures[0].pName;
					Wicked_CreateShortName(iMeshIndex, meshname, pNameFromMesh);
				}
				strcpy(mesh_combo_entry, meshname);
				iSelectedMesh = iMeshIndex;
				if (iSelectedMesh >= MAXMESHMATERIALS - 1) //PE: We can crash if we go above the MAXMESHMATERIALS.
					iSelectedMesh = MAXMESHMATERIALS - 1;

				*ppChosenMesh = pMesh;
				break;
			}
		}
	}
	else
	{
		sMesh* pMesh = pObject->ppMeshList[iUseThisMeshIndex];
		if (pMesh && pMesh->wickedmeshindex > 0)
		{
			sFrame* pFrame = nullptr;
			if (iUseThisMeshIndex + 1 < pObject->iFrameCount)
			{
				// Sometimes objects will have a root node frame that doesn't have any meshes
				pFrame = pObject->ppFrameList[iUseThisMeshIndex + 1];
			}
			else
			{
				pFrame = pObject->ppFrameList[iUseThisMeshIndex];
			}
			if (pFrame && strlen(pFrame->szName) > 0 && strcmp(pFrame->szName, "sibling frame"))
			{
				strcpy(meshname, pFrame->szName);
			}
			else
			{
				LPSTR pNameFromMesh = pMesh->pTextures[0].pName;
				Wicked_CreateShortName(iUseThisMeshIndex, meshname, pNameFromMesh);
			}
			strcpy(mesh_combo_entry, meshname);
			iSelectedMesh = iUseThisMeshIndex;
			if (iSelectedMesh >= MAXMESHMATERIALS - 1) //PE: We can crash if we go above the MAXMESHMATERIALS.
				iSelectedMesh = MAXMESHMATERIALS - 1;

			*ppChosenMesh = pMesh;
		}
	}
}

void Wicked_Set_Material_From_grideleprof_ThisMesh(void* pVObject, int mode, entityeleproftype *edit_grideleprof, int iThisMeshIndex)
{
	// get object ptr and grideleprof
	sObject* pObject = (sObject*)pVObject;
	if (!pObject) return;
	if (!edit_grideleprof)
	{
		edit_grideleprof = &t.grideleprof;
	}

	// find first mesh
	sMesh* pChosenMesh = NULL;
	Wicked_FindChosenMesh(pObject, &pChosenMesh, iThisMeshIndex);

	// if not read only, set material from 'edit_grideleprof'
	if ( (mode == 0 || mode == 3) && pChosenMesh) 
	{
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pChosenMesh->wickedmeshindex);
		if (mesh)
		{
			// get material settings from mesh material or WEMaterial
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);

			// base color
			if (edit_grideleprof->WEMaterial.dwBaseColor[iSelectedMesh] == -1)
			{
				BaseColor[0] = pObjectMaterial->baseColor.x;
				BaseColor[1] = pObjectMaterial->baseColor.y;
				BaseColor[2] = pObjectMaterial->baseColor.z;
				BaseColor[3] = pObjectMaterial->baseColor.w;
			}
			else 
			{
				BaseColor[0] = ((edit_grideleprof->WEMaterial.dwBaseColor[iSelectedMesh] & 0xff000000) >> 24) / 255.0f;
				BaseColor[1] = ((edit_grideleprof->WEMaterial.dwBaseColor[iSelectedMesh] & 0x00ff0000) >> 16) / 255.0f;
				BaseColor[2] = ((edit_grideleprof->WEMaterial.dwBaseColor[iSelectedMesh] & 0x0000ff00) >> 8) / 255.0f;
				BaseColor[3] = (edit_grideleprof->WEMaterial.dwBaseColor[iSelectedMesh] & 0x000000ff) / 255.0f;
			}
			pObjectMaterial->baseColor.x = BaseColor[0];
			pObjectMaterial->baseColor.y = BaseColor[1];
			pObjectMaterial->baseColor.z = BaseColor[2];
			pObjectMaterial->baseColor.w = BaseColor[3];

			// emissive color
			if (edit_grideleprof->WEMaterial.dwEmmisiveColor[iSelectedMesh] == -1)
			{
				EmmisiveColor[0] = pObjectMaterial->emissiveColor.x;
				EmmisiveColor[1] = pObjectMaterial->emissiveColor.y;
				EmmisiveColor[2] = pObjectMaterial->emissiveColor.z;
				EmmisiveColor[3] = pObjectMaterial->emissiveColor.w;
			}
			else
			{
				EmmisiveColor[0] = ((edit_grideleprof->WEMaterial.dwEmmisiveColor[iSelectedMesh] & 0xff000000) >> 24) / 255.0f;
				EmmisiveColor[1] = ((edit_grideleprof->WEMaterial.dwEmmisiveColor[iSelectedMesh] & 0x00ff0000) >> 16) / 255.0f;
				EmmisiveColor[2] = ((edit_grideleprof->WEMaterial.dwEmmisiveColor[iSelectedMesh] & 0x0000ff00) >> 8) / 255.0f;
				EmmisiveColor[3] = (edit_grideleprof->WEMaterial.dwEmmisiveColor[iSelectedMesh] & 0x000000ff) / 255.0f;
			}
			pObjectMaterial->emissiveColor.x = EmmisiveColor[0];
			pObjectMaterial->emissiveColor.y = EmmisiveColor[1];
			pObjectMaterial->emissiveColor.z = EmmisiveColor[2];
			pObjectMaterial->emissiveColor.w = EmmisiveColor[3];

			// update DBO mesh copy of base and emissive colors (?)
			pChosenMesh->mMaterial.Diffuse.r = pObjectMaterial->baseColor.x;
			pChosenMesh->mMaterial.Diffuse.g = pObjectMaterial->baseColor.y;
			pChosenMesh->mMaterial.Diffuse.b = pObjectMaterial->baseColor.z;
			pChosenMesh->mMaterial.Diffuse.a = pObjectMaterial->baseColor.w;
			pChosenMesh->mMaterial.Emissive.r = pObjectMaterial->emissiveColor.x;
			pChosenMesh->mMaterial.Emissive.g = pObjectMaterial->emissiveColor.y;
			pChosenMesh->mMaterial.Emissive.b = pObjectMaterial->emissiveColor.z;
			pChosenMesh->mMaterial.Emissive.a = pObjectMaterial->emissiveColor.w;

			// control values
			pObjectMaterial->SetAlphaRef(edit_grideleprof->WEMaterial.fAlphaRef[iSelectedMesh]);
			pObjectMaterial->SetNormalMapStrength(edit_grideleprof->WEMaterial.fNormal[iSelectedMesh]);
			pObjectMaterial->SetEmissiveStrength(edit_grideleprof->WEMaterial.fEmissive[iSelectedMesh]);
			pObjectMaterial->SetRoughness(edit_grideleprof->WEMaterial.fRoughness[iSelectedMesh]);
			pObjectMaterial->SetMetalness(edit_grideleprof->WEMaterial.fMetallness[iSelectedMesh]);

			// transparency
			bTransparent = edit_grideleprof->WEMaterial.bTransparency[iSelectedMesh];
			if (bTransparent)
			{
				pObjectMaterial->userBlendMode = BLENDMODE_ALPHA;
			}
			else 
			{
				pObjectMaterial->userBlendMode = BLENDMODE_OPAQUE;
			}

			// cast shadows
			bCastShadows = edit_grideleprof->WEMaterial.bCastShadows[iSelectedMesh];
			if (bCastShadows) 
			{
				pObjectMaterial->SetCastShadow(true);
			}
			else 
			{
				pObjectMaterial->SetCastShadow(false);
			}

			// planar reflections (buggy?)
			bPlanerReflection = edit_grideleprof->WEMaterial.bPlanerReflection[iSelectedMesh];
			if (bPlanerReflection) 
				pObjectMaterial->shaderType = wiScene::MaterialComponent::SHADERTYPE_PBR_PLANARREFLECTION;
			else 
				pObjectMaterial->shaderType = wiScene::MaterialComponent::SHADERTYPE_PBR;

			// double sided
			bDoubleSided = edit_grideleprof->WEMaterial.bDoubleSided[iSelectedMesh];
			if (bDoubleSided) 
				mesh->SetDoubleSided(true);
			else
				mesh->SetDoubleSided(false);

			// render order bias
			float fRenderOrderBias = edit_grideleprof->WEMaterial.fRenderOrderBias[iSelectedMesh];
			WickedCall_SetRenderOrderBias(pChosenMesh, fRenderOrderBias);

			// reflectance
			fReflectance = edit_grideleprof->WEMaterial.fReflectance[iSelectedMesh];

			// reset base and emissive color triggers
			dwBaseColor = -1;
			dwEmmisiveColor = -1;

			// material changed, update it
			pObjectMaterial->SetReflectance(fReflectance);
			pObjectMaterial->SetDirty();
		}
	}
	else 
	{
		// default settings 
		BaseColor[0] = 1.0f;
		BaseColor[1] = 1.0f;
		BaseColor[2] = 1.0f;
		BaseColor[3] = 1.0f;
		EmmisiveColor[0] = 0.0;
		EmmisiveColor[1] = 0.0;
		EmmisiveColor[2] = 0.0;
		EmmisiveColor[3] = 0.0;
		bTransparent = false;
		bDoubleSided = false;
		fRenderOrderBias = 0.0f;
		bPlanerReflection = false;
		bCastShadows = true;
		/*fReflectance = 0.002;*/
		fReflectance = 0.04;
		dwBaseColor = -1;
		dwEmmisiveColor = -1;
	}
}

void Wicked_Set_Material_From_grideleprof(void* pVObject, int mode, entityeleproftype *edit_grideleprof)
{
	Wicked_Set_Material_From_grideleprof_ThisMesh (pVObject, mode, edit_grideleprof, -1 );
}

void Wicked_Set_Material_Defaults(void* pVObject, int mode)
{
	// get object ptr
	sObject* pObject = (sObject*)pVObject;
	if (!pObject) return;

	// find first mesh
	sMesh* pChosenMesh = NULL;
	Wicked_FindChosenMesh(pObject, &pChosenMesh, -1);

	// if not read only, set material default for UI to edit
	if ((mode == 0 || mode == 3) && pChosenMesh) 
	{
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pChosenMesh->wickedmeshindex);
		if (mesh)
		{
			// get material from mesh
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);

			BaseColor[0] = pObjectMaterial->baseColor.x;
			BaseColor[1] = pObjectMaterial->baseColor.y;
			BaseColor[2] = pObjectMaterial->baseColor.z;
			BaseColor[3] = pObjectMaterial->baseColor.w;
			EmmisiveColor[0] = pObjectMaterial->emissiveColor.x;
			EmmisiveColor[1] = pObjectMaterial->emissiveColor.y;
			EmmisiveColor[2] = pObjectMaterial->emissiveColor.z;
			EmmisiveColor[3] = pObjectMaterial->emissiveColor.w;

			BLENDMODE bm = pObjectMaterial->GetBlendMode();
			if (bm == BLENDMODE_ALPHA)
				bTransparent = true;
			else
				bTransparent = false;

			bDoubleSided = false;
			fRenderOrderBias = 0.0f;
			bPlanerReflection = pObjectMaterial->HasPlanarReflection();
			bCastShadows = pObjectMaterial->IsCastingShadow();
			fReflectance = pObjectMaterial->reflectance;

			dwBaseColor = -1;
			dwEmmisiveColor = -1;
		}
	}
	else
	{
		// set defaults
		BaseColor[0] = 1.0f;
		BaseColor[1] = 1.0f;
		BaseColor[2] = 1.0f;
		BaseColor[3] = 1.0f;
		EmmisiveColor[0] = 0.0;
		EmmisiveColor[1] = 0.0;
		EmmisiveColor[2] = 0.0;
		EmmisiveColor[3] = 0.0;
		bTransparent = false;
		bDoubleSided = false;
		fRenderOrderBias = 0.0f;
		bPlanerReflection = false;
		bCastShadows = true;
		dwBaseColor = -1;
		dwEmmisiveColor = -1;
		/*fReflectance = 0.002;*/
		//ZJ: 0.04 is the most common setting for reflectance in PBR
		fReflectance = 0.04;
	}
}

void Wicked_Update_All_Materials(void* pVObject, int mode)
{
	// if not EBE, do not paint all meshes with single material choice!
	if ( mode != 3 ) return;

	/* this will probably break EBE, but the feature is to be overhauled so no foul
	sObject* pObject = (sObject*)pVObject;
	sMesh * pDefaultMesh = NULL;
	wiScene::MaterialComponent* pObjectSourceMaterial = NULL;
	if (!pObject) return;

	int iUseMesh = 0;
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh && pObject->ppFrameList[i]->pMesh->wickedmeshindex > 0)
		{
			iUseMesh = i;
			if (!pDefaultMesh) 
			{
				pDefaultMesh = pObject->ppFrameList[i]->pMesh;
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pDefaultMesh->wickedmeshindex);
				if (mesh)
				{
					// get material from mesh
					uint64_t materialEntity = mesh->subsets[0].materialID;
					pObjectSourceMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
				}
			}
			else 
			{
				sMesh * pMesh = pObject->ppFrameList[i]->pMesh;
				if (pDefaultMesh)
				{
					pMesh->mMaterial.Diffuse.r = pDefaultMesh->mMaterial.Diffuse.r;
					pMesh->mMaterial.Diffuse.g = pDefaultMesh->mMaterial.Diffuse.g;
					pMesh->mMaterial.Diffuse.b = pDefaultMesh->mMaterial.Diffuse.b;
					pMesh->mMaterial.Diffuse.a = pDefaultMesh->mMaterial.Diffuse.a;
					pMesh->mMaterial.Emissive.r = pDefaultMesh->mMaterial.Emissive.r;
					pMesh->mMaterial.Emissive.g = pDefaultMesh->mMaterial.Emissive.g;
					pMesh->mMaterial.Emissive.b = pDefaultMesh->mMaterial.Emissive.b;
					pMesh->mMaterial.Emissive.a = pDefaultMesh->mMaterial.Emissive.a;
				}

				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh && pObjectSourceMaterial)
				{
					// get material from mesh
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);

					pObjectMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].resource = pObjectSourceMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].resource;
					pObjectMaterial->alphaRef = pObjectSourceMaterial->alphaRef;
					pObjectMaterial->textures[MaterialComponentTEXTURESLOT::NORMALMAP].resource = pObjectSourceMaterial->textures[MaterialComponentTEXTURESLOT::NORMALMAP].resource;
					pObjectMaterial->normalMapStrength = pObjectSourceMaterial->normalMapStrength;
					pObjectMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].resource = pObjectSourceMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].resource;
					pObjectMaterial->SetOcclusionEnabled_Primary(true);
					pObjectMaterial->SetOcclusionEnabled_Secondary(false);

					pObjectMaterial->roughness = pObjectSourceMaterial->roughness;
					pObjectMaterial->metalness = pObjectSourceMaterial->metalness;
					pObjectMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].resource = pObjectSourceMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].resource;

					pObjectMaterial->baseColor.x = pObjectSourceMaterial->baseColor.x;
					pObjectMaterial->baseColor.y = pObjectSourceMaterial->baseColor.y;
					pObjectMaterial->baseColor.z = pObjectSourceMaterial->baseColor.z;
					pObjectMaterial->baseColor.w = pObjectSourceMaterial->baseColor.w;

					// emissive color
					pObjectMaterial->emissiveColor.x = pObjectSourceMaterial->emissiveColor.x;
					pObjectMaterial->emissiveColor.y = pObjectSourceMaterial->emissiveColor.y;
					pObjectMaterial->emissiveColor.z = pObjectSourceMaterial->emissiveColor.z;
					pObjectMaterial->emissiveColor.w = pObjectSourceMaterial->emissiveColor.w;

					pObjectMaterial->textures[MaterialComponentTEXTURESLOT::EMISSIVEMAP].resource = pObjectSourceMaterial->textures[MaterialComponentTEXTURESLOT::EMISSIVEMAP].resource;

					pObjectMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].resource = pObjectSourceMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].resource;
					if (pObjectMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].resource)
					{
						pObjectMaterial->SetOcclusionEnabled_Primary(false);
						pObjectMaterial->SetOcclusionEnabled_Secondary(true);
					}

					pObjectMaterial->_flags = pObjectSourceMaterial->_flags;
					pObjectMaterial->userBlendMode = pObjectSourceMaterial->userBlendMode;
					pObjectMaterial->reflectance = pObjectSourceMaterial->reflectance;
					pObjectMaterial->SetDirty();
				}
			}
		}
	}
	*/
}

void Wicked_Copy_Material_To_Grideleprof(void* pVObject, int mode, entityeleproftype *edit_grideleprof)
{
	// get object ptr and grideleprof
	sObject* pObject = (sObject*)pVObject;
	if (!pObject) return;
	if (!edit_grideleprof)
	{
		edit_grideleprof = &t.grideleprof;
	}

	// go through all meshes and update eleprof
	for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
	{
		sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
		if (pMesh && pMesh->wickedmeshindex > 0)
		{
			// copy material data to grideleprof structure
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
			if (mesh)
			{
				// get material from mesh
				uint64_t materialEntity = mesh->subsets[0].materialID;
				wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);

				// for each mesh texture set
				if (iMeshIndex < MAXMESHMATERIALS)
				{
					// ensure DBO meshes match wicked materials (later applied with Wicked_SetMeshMaterial)
					pMesh->mMaterial.Diffuse.r = pObjectMaterial->baseColor.x;
					pMesh->mMaterial.Diffuse.g = pObjectMaterial->baseColor.y;
					pMesh->mMaterial.Diffuse.b = pObjectMaterial->baseColor.z;
					pMesh->mMaterial.Diffuse.a = pObjectMaterial->baseColor.w;
					pMesh->mMaterial.Emissive.r = pObjectMaterial->emissiveColor.x;
					pMesh->mMaterial.Emissive.g = pObjectMaterial->emissiveColor.y;
					pMesh->mMaterial.Emissive.b = pObjectMaterial->emissiveColor.z;
					pMesh->mMaterial.Emissive.a = pObjectMaterial->emissiveColor.w;

					// copy wicked material for this mesh to WEMaterial
					edit_grideleprof->WEMaterial.baseColorMapName[iMeshIndex] = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].name.c_str());
					edit_grideleprof->WEMaterial.normalMapName[iMeshIndex] = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::NORMALMAP].name.c_str());
					edit_grideleprof->WEMaterial.surfaceMapName[iMeshIndex] = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].name.c_str());
					edit_grideleprof->WEMaterial.displacementMapName[iMeshIndex] = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].name.c_str());
					edit_grideleprof->WEMaterial.emissiveMapName[iMeshIndex] = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::EMISSIVEMAP].name.c_str());
					#ifndef DISABLEOCCLUSIONMAP
					edit_grideleprof->WEMaterial.occlusionMapName[iMeshIndex] = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].name.c_str());
					#endif
					edit_grideleprof->WEMaterial.fNormal[iMeshIndex] = pObjectMaterial->normalMapStrength;
					edit_grideleprof->WEMaterial.fRoughness[iMeshIndex] = pObjectMaterial->roughness;
					edit_grideleprof->WEMaterial.fMetallness[iMeshIndex] = pObjectMaterial->metalness;
					edit_grideleprof->WEMaterial.fEmissive[iMeshIndex] = pObjectMaterial->GetEmissiveStrength();
					edit_grideleprof->WEMaterial.fAlphaRef[iMeshIndex] = pObjectMaterial->alphaRef;
					edit_grideleprof->WEMaterial.bCastShadows[iMeshIndex] = pObjectMaterial->IsCastingShadow();
					edit_grideleprof->WEMaterial.bDoubleSided[iMeshIndex] = mesh->IsDoubleSided();
					edit_grideleprof->WEMaterial.fRenderOrderBias[iMeshIndex] = 0.0f;
					sFrame* pFrame = pMesh->pFrameAttachedTo;
					if (pFrame)
					{
						wiScene::ObjectComponent* object = wiScene::GetScene().objects.GetComponent(pFrame->wickedobjindex);
						if (object)
						{
							fRenderOrderBias = object->GetRenderOrderBiasDistance();
							edit_grideleprof->WEMaterial.fRenderOrderBias[iMeshIndex] = fRenderOrderBias;
						}
					}
					edit_grideleprof->WEMaterial.bPlanerReflection[iMeshIndex] = pObjectMaterial->HasPlanarReflection();
					if (pObjectMaterial->GetBlendMode() == BLENDMODE_ALPHA)
						edit_grideleprof->WEMaterial.bTransparency[iMeshIndex] = true;
					else
						edit_grideleprof->WEMaterial.bTransparency[iMeshIndex] = false;
					edit_grideleprof->WEMaterial.fReflectance[iMeshIndex] = pObjectMaterial->reflectance;
					edit_grideleprof->WEMaterial.dwBaseColor[iMeshIndex] = ((unsigned int)(pObjectMaterial->baseColor.x * 255) << 24);
					edit_grideleprof->WEMaterial.dwBaseColor[iMeshIndex] += ((unsigned int)(pObjectMaterial->baseColor.y * 255) << 16);
					edit_grideleprof->WEMaterial.dwBaseColor[iMeshIndex] += ((unsigned int)(pObjectMaterial->baseColor.z * 255) << 8);
					edit_grideleprof->WEMaterial.dwBaseColor[iMeshIndex] += ((unsigned int)(pObjectMaterial->baseColor.w * 255));
					edit_grideleprof->WEMaterial.dwEmmisiveColor[iMeshIndex] = ((unsigned int)(pObjectMaterial->emissiveColor.x * 255) << 24);
					edit_grideleprof->WEMaterial.dwEmmisiveColor[iMeshIndex] += ((unsigned int)(pObjectMaterial->emissiveColor.y * 255) << 16);
					edit_grideleprof->WEMaterial.dwEmmisiveColor[iMeshIndex] += ((unsigned int)(pObjectMaterial->emissiveColor.z * 255) << 8);
					if (pObjectMaterial->emissiveColor.w <= 1.0)
						edit_grideleprof->WEMaterial.dwEmmisiveColor[iMeshIndex] += ((unsigned int)(pObjectMaterial->emissiveColor.w * 255));
					else
						edit_grideleprof->WEMaterial.dwEmmisiveColor[iMeshIndex] += 255;
				}
			}
		}
	}
}

void Wicked_Copy_JustTextureNames_To_Grideleprof(void* pVObject, int mode)
{
	// get object ptr
	sObject* pObject = (sObject*)pVObject;
	if (!pObject) return;

	// find first mesh
	sMesh* pChosenMesh = NULL;
	Wicked_FindChosenMesh(pObject, &pChosenMesh, -1);

	// copy texture names into mesh chosen
	if (pChosenMesh) 
	{
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pChosenMesh->wickedmeshindex);
		if (mesh)
		{
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
			t.grideleprof.WEMaterial.baseColorMapName[iSelectedMesh] = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::BASECOLORMAP].name.c_str());
			t.grideleprof.WEMaterial.normalMapName[iSelectedMesh] = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::NORMALMAP].name.c_str());
			t.grideleprof.WEMaterial.surfaceMapName[iSelectedMesh] = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::SURFACEMAP].name.c_str());
			t.grideleprof.WEMaterial.displacementMapName[iSelectedMesh] = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::DISPLACEMENTMAP].name.c_str());
			t.grideleprof.WEMaterial.emissiveMapName[iSelectedMesh] = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::EMISSIVEMAP].name.c_str());
			#ifndef DISABLEOCCLUSIONMAP
			t.grideleprof.WEMaterial.occlusionMapName[iSelectedMesh] = importer_getfilenameonly((LPSTR)pObjectMaterial->textures[MaterialComponentTEXTURESLOT::OCCLUSIONMAP].name.c_str());
			#endif
		}
	}
}

bool LocateTexture(char *name)
{
	//PE: Need to search for another names here.
	if (FileExist(name) == 1)
		return true;
	return false;
}

// Loop through all the meshes in the imported object and change material settings.
void importer_set_all_material_settings(int slot, float value)
{
	if (!t.importer.bEditAllMesh)
	{
		return;
	}

	sObject* pObject = nullptr;// = GetObjectData(t.importer.objectnumber);
	if (t.importer.importerActive == 1)
	{
		pObject = GetObjectData(t.importer.objectnumber);
	}
	else
	{
		// Need a way to get the object data for the current object being altered outside of the importer.
		int e = t.widget.pickedEntityIndex;
		if (e < 0) return;
		int obj = t.entityelement[e].obj;
		if (ObjectExist(obj))
		{
			pObject = GetObjectData(obj);
		}
	}

	if (!pObject)
	{
		return;
	}

	for (int i = 0; i < pObject->iMeshCount; i++)
	{
		sMesh* mesh = pObject->ppMeshList[i];

		if (mesh)
		{
			wiScene::MeshComponent* meshComponent = wiScene::GetScene().meshes.GetComponent(mesh->wickedmeshindex);

			if (meshComponent)
			{
				// get material settings from mesh material or WEMaterial
				uint64_t materialEntity = meshComponent->subsets[0].materialID;
				wiScene::MaterialComponent* pMeshMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);

				// Switch statement values match the texslot switch values from Wicked_Change_Object_Material()
				switch (slot)
				{
				case 0:
				{
					pMeshMaterial->SetAlphaRef(value);
					break;
				}
				case 1:
				{
					pMeshMaterial->SetNormalMapStrength(value);
					break;
				}
				case 2:
				{
					pMeshMaterial->SetRoughness(value);
					break;
				}
				case 3:
				{
					pMeshMaterial->SetMetalness(value);
					break;
				}
				case 4:
				{
					// Unused.
					break;
				}
				case 5:
				{
					pMeshMaterial->SetEmissiveStrength(value);
					break;
				}
				case 6:
				{
					pMeshMaterial->SetReflectance(value);
				}
				case 7:
				{
					WickedCall_SetRenderOrderBias(mesh, value);
				}
				default:
					break;
				}
				
				pMeshMaterial->SetDirty();
			}
		}
	}
}

void importer_set_all_material_colour(int slot, float values[4])
{
	if (!t.importer.bEditAllMesh)
	{
		return;
	}

	sObject* pObject = nullptr;// = GetObjectData(t.importer.objectnumber);
	if (t.importer.importerActive == 1)
	{
		pObject = GetObjectData(t.importer.objectnumber);
	}
	else
	{
		// Need a way to get the object data for the current object being altered outside of the importer.
		int e = t.widget.pickedEntityIndex;
		if (e < 0) return;
		int obj = t.entityelement[e].obj;
		if (ObjectExist(obj))
		{
			pObject = GetObjectData(obj);
		}
	}

	if (!pObject)
	{
		return;
	}

	for (int i = 0; i < pObject->iMeshCount; i++)
	{
		sMesh* mesh = pObject->ppMeshList[i];

		if (mesh)
		{
			wiScene::MeshComponent* meshComponent = wiScene::GetScene().meshes.GetComponent(mesh->wickedmeshindex);

			if (meshComponent)
			{
				// get material settings from mesh material or WEMaterial
				uint64_t materialEntity = meshComponent->subsets[0].materialID;
				wiScene::MaterialComponent* pMeshMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);

				if (slot == 0)
				{
					pMeshMaterial->SetBaseColor(XMFLOAT4(&values[0]));
				}
				else if (slot == 5)
				{
					pMeshMaterial->SetEmissiveColor(XMFLOAT4(&values[0]));
				}
			}
		}
	}
}

void importer_set_all_material_transparent(bool bIsTransparent)
{
	if (!t.importer.bEditAllMesh)
	{
		return;
	}

	sObject* pObject = nullptr;// = GetObjectData(t.importer.objectnumber);
	if (t.importer.importerActive == 1)
	{
		pObject = GetObjectData(t.importer.objectnumber);
	}
	else
	{
		// Need a way to get the object data for the current object being altered outside of the importer.
		int e = t.widget.pickedEntityIndex;
		if (e < 0) return;
		int obj = t.entityelement[e].obj;
		if (ObjectExist(obj))
		{
			pObject = GetObjectData(obj);
		}
	}

	if (!pObject)
	{
		return;
	}

	for (int i = 0; i < pObject->iMeshCount; i++)
	{
		sMesh* mesh = pObject->ppMeshList[i];

		if (mesh)
		{
			wiScene::MeshComponent* meshComponent = wiScene::GetScene().meshes.GetComponent(mesh->wickedmeshindex);

			if (meshComponent)
			{
				// get material settings from mesh material or WEMaterial
				uint64_t materialEntity = meshComponent->subsets[0].materialID;
				wiScene::MaterialComponent* pMeshMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);

				if (bIsTransparent)
				{
					pMeshMaterial->userBlendMode = BLENDMODE_ALPHA;
				}
				else
				{
					pMeshMaterial->userBlendMode = BLENDMODE_OPAQUE;
				}
			}
		}
	}
}

void importer_set_all_mesh_double_sided(bool bIsDoubleSided)
{
	if (!t.importer.bEditAllMesh)
	{
		return;
	}

	sObject* pObject = nullptr;// = GetObjectData(t.importer.objectnumber);
	if (t.importer.importerActive == 1)
	{
		pObject = GetObjectData(t.importer.objectnumber);
	}
	else
	{
		// Need a way to get the object data for the current object being altered outside of the importer.
		int e = t.widget.pickedEntityIndex;
		if (e < 0) return;
		int obj = t.entityelement[e].obj;
		if (ObjectExist(obj))
		{
			pObject = GetObjectData(obj);
		}
	}

	if (!pObject)
	{
		return;
	}

	for (int i = 0; i < pObject->iMeshCount; i++)
	{
		sMesh* mesh = pObject->ppMeshList[i];

		if (mesh)
		{
			wiScene::MeshComponent* meshComponent = wiScene::GetScene().meshes.GetComponent(mesh->wickedmeshindex);

			meshComponent->SetDoubleSided(bIsDoubleSided);
		}
	}
}

void importer_set_all_material_planar_reflection(bool planarReflection)
{
	if (!t.importer.bEditAllMesh)
	{
		return;
	}

	sObject* pObject = nullptr;// = GetObjectData(t.importer.objectnumber);
	if (t.importer.importerActive == 1)
	{
		pObject = GetObjectData(t.importer.objectnumber);
	}
	else
	{
		// Need a way to get the object data for the current object being altered outside of the importer.
		int e = t.widget.pickedEntityIndex;
		if (e < 0) return;
		int obj = t.entityelement[e].obj;
		if (ObjectExist(obj))
		{
			pObject = GetObjectData(obj);
		}
	}

	if (!pObject)
	{
		return;
	}

	for (int i = 0; i < pObject->iMeshCount; i++)
	{
		sMesh* mesh = pObject->ppMeshList[i];

		if (mesh)
		{
			wiScene::MeshComponent* meshComponent = wiScene::GetScene().meshes.GetComponent(mesh->wickedmeshindex);

			if (meshComponent)
			{
				// get material settings from mesh material or WEMaterial
				uint64_t materialEntity = meshComponent->subsets[0].materialID;
				wiScene::MaterialComponent* pMeshMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);

				if (planarReflection)
				{
					pMeshMaterial->shaderType = wiScene::MaterialComponent::SHADERTYPE_PBR_PLANARREFLECTION;
				}
				else
				{
					pMeshMaterial->shaderType = wiScene::MaterialComponent::SHADERTYPE_PBR;
				}
			}
		}
	}
}

void importer_set_all_material_cast_shadow(bool bCastShadow)
{
	if (!t.importer.bEditAllMesh)
	{
		return;
	}

	sObject* pObject = nullptr;// = GetObjectData(t.importer.objectnumber);
	if (t.importer.importerActive == 1)
	{
		pObject = GetObjectData(t.importer.objectnumber);
	}
	else
	{
		// Need a way to get the object data for the current object being altered outside of the importer.
		int e = t.widget.pickedEntityIndex;
		if (e < 0) return;
		int obj = t.entityelement[e].obj;
		if (ObjectExist(obj))
		{
			pObject = GetObjectData(obj);
		}
	}

	if (!pObject)
	{
		return;
	}

	for (int i = 0; i < pObject->iMeshCount; i++)
	{
		sMesh* mesh = pObject->ppMeshList[i];

		if (mesh)
		{
			wiScene::MeshComponent* meshComponent = wiScene::GetScene().meshes.GetComponent(mesh->wickedmeshindex);

			if (meshComponent)
			{
				// get material settings from mesh material or WEMaterial
				uint64_t materialEntity = meshComponent->subsets[0].materialID;
				wiScene::MaterialComponent* pMeshMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);

				pMeshMaterial->SetCastShadow(bCastShadow);
			}
		}
	}
}

// Set the Y import offset so that the bottom of the objects bounding box is the same height as the floor.
void importer_find_floor(void)
{
	sObject* pObject = GetObjectData(t.importer.objectnumber);

	if (bFindFloor)
	{
		fImportPosY = 0.0f;
		fImportPosY += (fImportPosY - pObject->collision.vecMin.y);
		t.importer.objectFPE.offy = fImportPosY;
		importer_applyframezerooffsets(t.importer.objectnumber, fImportPosX, fImportPosY, fImportPosZ, fImportRotX, fImportRotY, fImportRotZ);
	}
}

void importer_delete_old_surface_files()
{
	cstr pOriginalDir = GetDir();
	
	const char* pWritePath = GG_GetWritePath();
	char pWriteDirectory[MAX_PATH];
	strcpy(pWriteDirectory, pWritePath);
	cstr pPath(pWriteDirectory);
	pPath += "imported_models\\";

	for (int i = 0; i < t.importer.pSurfaceFilesToDelete.size(); i++)
	{
		// Only want to delete files from the writable area, so get the filename only and we'll check the writable folder only.
		cstr pFilenameOnly = importer_getfilenameonly(t.importer.pSurfaceFilesToDelete[i].Get());
		cstr pFileToDelete = pPath + pFilenameOnly;
		if (FileExist(pFileToDelete.Get()) == 1)
		{
			DeleteAFile(pFileToDelete.Get());
		}
	}

	// imported_models folder only needed as a temporary location to store files.
	extern BOOL DB_DeleteDir(char* Dirname);
	DB_DeleteDir(pPath.Get()); // Will only delete the folder if it is empty  (if there were any files in there before this importer session, they will be left untouched.)

	SetDir(pOriginalDir.Get());
}

#endif

void importer_storeobjectdata()
{
#ifdef WICKEDENGINE
	// Copy all of the imported object data, so it can be restored if the user goes back after import.
	strcpy(g_Data.cImportPath, t.importer.objectFileOriginalPath.Get());
	strcpy(g_Data.cName, cImportName);
	g_Data.iScaleMode = t.importer.lastscalingmodeused;
	g_Data.iCentreMeshData = t.importer.centermodelbyshiftingmesh;
	g_Data.iScale = iImporterScale;
	g_Data.iRotationOffset[0] = fImportRotX; g_Data.iRotationOffset[1] = fImportRotY; g_Data.iRotationOffset[2] = fImportRotZ;
	g_Data.iPositionOffset[0] = fImportPosX; g_Data.iPositionOffset[1] = fImportPosY; g_Data.iPositionOffset[2] = fImportPosZ;
	g_Data.iFindFloor = bFindFloor;
	g_Data.iCollisionShape = t.importer.collisionshape;
	g_Data.iStatic = t.importer.defaultstatic;
	g_Data.iMaterialType = t.slidersmenuvalue[t.importer.properties1Index][10].value;

	sObject* pObject = nullptr;
	pObject = GetObjectData(t.importer.objectnumber);
	if (pObject)
	{
		int iMeshCount = pObject->iMeshCount;
		g_Data.baseColours.resize(iMeshCount);
		g_Data.albedoFiles.resize(iMeshCount);
		g_Data.normalFiles.resize(iMeshCount);
		g_Data.surfaceFiles.resize(iMeshCount);
		g_Data.emissiveFiles.resize(iMeshCount);
		g_Data.emissiveColours.resize(iMeshCount);
		g_Data.reflectance.resize(iMeshCount);
		g_Data.renderBias.resize(iMeshCount);
		g_Data.transparent.resize(iMeshCount);
		g_Data.doubleSided.resize(iMeshCount);
		g_Data.planarReflection.resize(iMeshCount);
		g_Data.castShadows.resize(iMeshCount);
		g_Data.animSlots.resize(g_pAnimSlotList.size());

		for (int i = 0; i < pObject->iMeshCount; i++)
		{
			sMesh* mesh = pObject->ppMeshList[i];
			if (mesh)
			{
				wiScene::MeshComponent* meshComponent = wiScene::GetScene().meshes.GetComponent(mesh->wickedmeshindex);
				if (meshComponent)
				{
					uint64_t materialEntity = meshComponent->subsets[0].materialID;
					wiScene::MaterialComponent* pMeshMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pMeshMaterial)
					{
						std::array<float, 4>& baseColour = g_Data.baseColours[i];
						baseColour[0] = pMeshMaterial->baseColor.x;
						baseColour[1] = pMeshMaterial->baseColor.y;
						baseColour[2] = pMeshMaterial->baseColor.z;
						baseColour[3] = pMeshMaterial->baseColor.w;

						std::array<char, MAX_PATH>& albedofile = g_Data.albedoFiles[i];
						strcpy(&albedofile[0], pMeshMaterial->textures[BASECOLORMAP].name.c_str());

						std::array<char, MAX_PATH>& normalfile = g_Data.normalFiles[i];
						strcpy(&normalfile[0], pMeshMaterial->textures[NORMALMAP].name.c_str());

						std::array<char, MAX_PATH>& surfacefile = g_Data.surfaceFiles[i];
						strcpy(&surfacefile[0], pMeshMaterial->textures[SURFACEMAP].name.c_str());

						std::array<char, MAX_PATH>& emissivefile = g_Data.emissiveFiles[i];
						strcpy(&emissivefile[0], pMeshMaterial->textures[EMISSIVEMAP].name.c_str());

						std::array<float, 4>& emissiveColour = g_Data.emissiveColours[i];
						emissiveColour[0] = pMeshMaterial->emissiveColor.x;
						emissiveColour[1] = pMeshMaterial->emissiveColor.y;
						emissiveColour[2] = pMeshMaterial->emissiveColor.z;
						emissiveColour[3] = pMeshMaterial->emissiveColor.w;

						g_Data.reflectance[i] = pMeshMaterial->reflectance;
						g_Data.renderBias[i] = WickedCall_GetRenderOrderBias(mesh);
						g_Data.transparent[i] = pMeshMaterial->userBlendMode;
						g_Data.doubleSided[i] = meshComponent->IsDoubleSided();
						g_Data.planarReflection[i] = pMeshMaterial->shaderType;
						g_Data.castShadows[i] = pMeshMaterial->IsCastingShadow();

						if(g_Data.animSlots.size() > 0)
							memcpy(&g_Data.animSlots[0], &g_pAnimSlotList[0], sizeof(sAnimSlotStruct) * g_Data.animSlots.size());
					}
				}
			}
		}
	}
#endif
}

void importer_restoreobjectdata()
{
#ifdef WICKEDENGINE
	// Copy all of the imported object data, so it can be restored if the user goes back after import.
	strcpy(cImportName, g_Data.cName);
	t.importer.lastscalingmodeused = g_Data.iScaleMode;
	t.importer.centermodelbyshiftingmesh = g_Data.iCentreMeshData;
	iImporterScale = g_Data.iScale;
	fImportRotX = g_Data.iRotationOffset[0]; fImportRotY = g_Data.iRotationOffset[1]; fImportRotZ = g_Data.iRotationOffset[2];
	fImportPosX = g_Data.iPositionOffset[0]; fImportPosY = g_Data.iPositionOffset[1]; fImportPosZ = g_Data.iPositionOffset[2];
	bFindFloor = g_Data.iFindFloor;
	t.importer.collisionshape = g_Data.iCollisionShape;
	t.importer.defaultstatic = g_Data.iStatic;
	t.slidersmenuvalue[t.importer.properties1Index][10].value = g_Data.iMaterialType;

	sObject* pObject = nullptr;
	pObject = GetObjectData(t.importer.objectnumber);
	if (pObject)
	{
		int iMeshCount = pObject->iMeshCount;

		for (int i = 0; i < iMeshCount; i++)
		{
			sMesh* mesh = pObject->ppMeshList[i];
			if (mesh)
			{
				wiScene::MeshComponent* meshComponent = wiScene::GetScene().meshes.GetComponent(mesh->wickedmeshindex);
				if (meshComponent)
				{
					uint64_t materialEntity = meshComponent->subsets[0].materialID;
					wiScene::MaterialComponent* pMeshMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pMeshMaterial)
					{
						std::array<float, 4>& baseColour = g_Data.baseColours[i];
						pMeshMaterial->baseColor = XMFLOAT4(baseColour[0], baseColour[1], baseColour[2], baseColour[3]);

						std::array<char, MAX_PATH>& albedofile = g_Data.albedoFiles[i];
						pMeshMaterial->textures[BASECOLORMAP].name = std::string(&albedofile[0]);

						std::array<char, MAX_PATH>& normalfile = g_Data.normalFiles[i];
						pMeshMaterial->textures[NORMALMAP].name = std::string(&normalfile[0]);

						std::array<char, MAX_PATH>& surfacefile = g_Data.surfaceFiles[i];
						pMeshMaterial->textures[SURFACEMAP].name = std::string(&surfacefile[0]);

						std::array<char, MAX_PATH>& emissivefile = g_Data.emissiveFiles[i];
						pMeshMaterial->textures[EMISSIVEMAP].name = std::string(&emissivefile[0]);

						std::array<float, 4>& emissiveColour = g_Data.emissiveColours[i];
						pMeshMaterial->emissiveColor = XMFLOAT4(emissiveColour[0], emissiveColour[1], emissiveColour[2], emissiveColour[3]);

						pMeshMaterial->reflectance = g_Data.reflectance[i];
						WickedCall_SetRenderOrderBias(mesh, g_Data.renderBias[i]);
						pMeshMaterial->userBlendMode = (BLENDMODE)g_Data.transparent[i];
						meshComponent->SetDoubleSided(g_Data.doubleSided[i]);
						pMeshMaterial->shaderType = (wiScene::MaterialComponent::SHADERTYPE)g_Data.planarReflection[i];
						pMeshMaterial->SetCastShadow(g_Data.castShadows[i]);

						g_pAnimSlotList.resize(g_Data.animSlots.size());
						if (g_Data.animSlots.size() > 0)
							memcpy(&g_pAnimSlotList[0], &g_Data.animSlots[0], sizeof(sAnimSlotStruct) * g_Data.animSlots.size());

						if (i == 0)
						{
							pSelectedMaterial = pMeshMaterial;
							pSelectedMesh = mesh;
						}

						pMeshMaterial->SetDirty();
					}
				}
			}
		}

		

		importer_applyframezerooffsets(t.importer.objectnumber, fImportPosX, fImportPosY, fImportPosZ, fImportRotX, fImportRotY, fImportRotZ);
		char* pCollisionShapes[10] = { "Box","Polygon","Sphere","Cylinder","Convex Hull","Character Collision","Tree Collision","No Collision", "Hull Decomp", "Collision Mesh" };
		strcpy(collision_combo_entry, pCollisionShapes[t.importer.collisionshape]);
	}
#endif
}

void importer_clearobjectdata()
{
#ifdef WICKEDENGINE
	g_Data.cImportPath[0] = 0;
	g_Data.cName[0] = 0;
	g_Data.iScaleMode = 0;
	g_Data.iCentreMeshData = 0;
	g_Data.iScale = 100;
	g_Data.iRotationOffset[0] = 0; g_Data.iRotationOffset[1] = 0; g_Data.iRotationOffset[2] = 0;
	g_Data.iPositionOffset[0] = 0; g_Data.iPositionOffset[1] = 0; g_Data.iPositionOffset[2] = 0;
	g_Data.iFindFloor = 0;
	g_Data.iCollisionShape = 0;
	g_Data.iStatic = 0;
	g_Data.iMaterialType = 0;
	g_Data.baseColours.clear();
	g_Data.albedoFiles.clear();
	g_Data.normalFiles.clear();
	g_Data.surfaceFiles.clear();
	g_Data.emissiveFiles.clear();
	g_Data.emissiveColours.clear();
	g_Data.reflectance.clear();
	g_Data.renderBias.clear();
	g_Data.transparent.clear();
	g_Data.doubleSided.clear();
	g_Data.planarReflection.clear();
	g_Data.castShadows.clear();
	g_Data.animSlots.clear();
#endif
}

// Select a texture file, if currentfilename is specified then it will open the file selector wherever it is located.
char* importer_selectfile(int texslot, std::string currentfilename, bool bPresetExplorer)
{
	#ifdef WICKEDENGINE

	// Want to always open where the model is when importing.
	if(t.importer.importerActive)
		bPresetExplorer = false;

	bool bOpenCustomLocation = true;
	std::string texPath = currentfilename;

	if (texPath.length() > 0)
	{
		if (!strstr(texPath.c_str(), ":"))
		{
			// If using relative paths then make sure we get the full path to the write folder.
			std::string fullPath = std::string(GG_GetWritePath()) + std::string("Files\\") + texPath;
			if (!FileExist((LPSTR)fullPath.c_str()))
			{
				// File was not in the write folder, check the install location.
				fullPath = std::string(GetDir()) + std::string("\\") + texPath;
				if (!FileExist((LPSTR)fullPath.c_str()))
					bOpenCustomLocation = false;
			}
			texPath = fullPath;
		}

		// Found an existing file that is being replaced, check if we should open the file explorer there.
		if (bOpenCustomLocation)
			bOpenCustomLocation = bPresetExplorer;

		if (bOpenCustomLocation)
		{
			// Determine the path to the currently applied texture so we can start the file selector there.
			int count = 0;
			for (std::string::reverse_iterator reverseIt = texPath.rbegin(); reverseIt != texPath.rend(); ++reverseIt)
			{
				if (*reverseIt == '\\' || *reverseIt == '/')
					break;

				count++;
			}

			texPath.resize(texPath.length() - count);
		}
	}

	return (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "All\0*.*\0DDS\0*.dds\0PNG\0*.png\0JPEG\0*.jpg\0TGA\0*.tga\0BMP\0*.bmp\0\0\0", texPath.c_str(), NULL, bOpenCustomLocation);
	#endif
	return nullptr;
}

void importer_collectmeshname(char* meshName)
{
	return;
#ifdef WICKEDENGINE
	g_MeshNamesAssimp.push_back(cstr(meshName));
#endif
}

// Used by ASSIMP to check if it should allow import of a model with no meshes (animation data without skin)
bool importer_havevalidobject()
{
	sObject* pObject = GetObjectData(t.importer.objectnumber);
	if (pObject)
		return true;
	else
		return false;
}