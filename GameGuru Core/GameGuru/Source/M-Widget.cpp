//----------------------------------------------------
//--- GAMEGURU - M-Widget
//----------------------------------------------------

#include "stdafx.h"
#include "gameguru.h"

//PE: GameGuru IMGUI.
#ifdef ENABLEIMGUI
#include "..\Imgui\imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "..\Imgui\imgui_internal.h"
#include "..\Imgui\imgui_impl_win32.h"
#include "..\Imgui\imgui_gg_dx11.h"
#endif


#ifdef WICKEDENGINE
#include ".\..\..\Guru-WickedMAX\wickedcalls.h"
bool widget_temp_disabled = false;
extern bool g_bRefreshRotationValuesFromObjectOnce;
#include "GGTerrain/GGTerrain.h"
using namespace GGTerrain;
#endif

//PE: All selections has been changed from multiply system (PickScreenObject,PickScreen2D23D) to only use PickScreen2D23D.
//PE: This makes sure that a start selection using PickScreenObject, dont get changed to a PickScreen2D23D and make huge jumps/moves.
//PE: The PickScreenObject system has been kept if needed.
//PE: MAXMOVEPERSYNC set how far you max can move per sync.

//float debugobj = 0;
float MAXMOVEPERSYNC = 1000.0f; //When position objects, max move per sync.

// Prototypes
void gridedit_clearentityrubberbandlist ( void );
void RotateAndMoveRubberBand(int iActiveObj, float fMovedActiveObjectX, float fMovedActiveObjectY, float fMovedActiveObjectZ, GGQUATERNION quatRotationEvent);// float fMovedActiveObjectRX, float fMovedActiveObjectRY, float fMovedActiveObjectRZ);
void SetStartPositionsForRubberBand(int iActiveObj);

void SlowPositionObject(int iID, float fX, float fY, float fZ)
{
#ifdef ENABLEIMGUI
	if (t.inputsys.keyshift)
		MAXMOVEPERSYNC = 0.1f; //Fine adjust when holding SHIFT
	else
		MAXMOVEPERSYNC = 1000.0f; //PE: Disabled for now, this should be a config setting. when you need precise placement.
#endif

	float fCurX = ObjectPositionX(iID);
	float fCurY = ObjectPositionY(iID);
	float fCurZ = ObjectPositionZ(iID);
	float fNewX= fX, fNewY= fY, fNewZ = fZ;

	if (fNewX > fCurX + MAXMOVEPERSYNC) fNewX = fCurX + MAXMOVEPERSYNC;
	else if (fNewX < fCurX - MAXMOVEPERSYNC) fNewX = fCurX - MAXMOVEPERSYNC;
	else fNewX = fX;

	if (fNewY > fCurY + MAXMOVEPERSYNC) fNewY = fCurY + MAXMOVEPERSYNC;
	else if (fNewY < fCurY - MAXMOVEPERSYNC) fNewY = fCurY - MAXMOVEPERSYNC;
	else fNewY = fY;

	if (fNewZ > fCurZ + MAXMOVEPERSYNC) fNewZ = fCurZ + MAXMOVEPERSYNC;
	else if (fNewZ < fCurZ - MAXMOVEPERSYNC) fNewZ = fCurZ - MAXMOVEPERSYNC;
	else fNewZ = fZ;

	PositionObject(iID, fNewX, fNewY, fNewZ);
}

void widget_init ( void )
{
	// set widget up
	t.widget.pickedObject = 0;
	t.widget.activeObject = 0;
	t.widget.widgetXObj = g.widgetobjectoffset;
	t.widget.widgetYObj = g.widgetobjectoffset+1;
	t.widget.widgetZObj = g.widgetobjectoffset+2;
	t.widget.widgetXYObj = g.widgetobjectoffset+3;
	t.widget.widgetXZObj = g.widgetobjectoffset+4;
	t.widget.widgetYZObj = g.widgetobjectoffset+5;
	t.widget.widgetXRotObj = g.widgetobjectoffset+6;
	t.widget.widgetYRotObj = g.widgetobjectoffset+7;
	t.widget.widgetZRotObj = g.widgetobjectoffset+8;
	t.widget.widgetXScaleObj = g.widgetobjectoffset+9;
	t.widget.widgetYScaleObj = g.widgetobjectoffset+10;
	t.widget.widgetZScaleObj = g.widgetobjectoffset+11;
	t.widget.widgetXYZScaleObj = g.widgetobjectoffset+12;
	#ifdef WICKEDENGINE
	t.widget.widgetMAXObj = 12;
	#else
	t.widget.widgetXColObj = g.widgetobjectoffset+16;
	t.widget.widgetYColObj = g.widgetobjectoffset+17;
	t.widget.widgetZColObj = g.widgetobjectoffset+18;
	t.widget.widgetPOSObj = g.widgetobjectoffset+19;
	t.widget.widgetROTObj = g.widgetobjectoffset+20;
	t.widget.widgetSCLObj = g.widgetobjectoffset+21;
	t.widget.widgetPRPObj = g.widgetobjectoffset+22;
	t.widget.widgetDUPObj = g.widgetobjectoffset+23;
	t.widget.widgetDELObj = g.widgetobjectoffset+24;
	t.widget.widgetLCKObj = g.widgetobjectoffset+25;
	t.widget.widgetMAXObj = 25;
	#endif
	t.widget.widgetPlaneObj = g.widgetobjectoffset+26;

	//  Load in media
	#ifdef WICKEDENGINE
	WickedCall_PresetObjectIgnoreTextures(true);
	cStr pWidgetPath = "editors\\gfx\\widget\\";
	LoadObject ( cStr(pWidgetPath+"widgetX.dbo").Get(),t.widget.widgetXObj );
	LoadObject ( cStr(pWidgetPath+"widgetY.dbo").Get(),t.widget.widgetYObj );
	LoadObject ( cStr(pWidgetPath+"widgetZ.dbo").Get(),t.widget.widgetZObj );
	LoadObject ( cStr(pWidgetPath+"widgetXY.dbo").Get(),t.widget.widgetXYObj );
	LoadObject ( cStr(pWidgetPath+"widgetXZ.dbo").Get(),t.widget.widgetXZObj );
	LoadObject ( cStr(pWidgetPath+"widgetYZ.dbo").Get(),t.widget.widgetYZObj );
	LoadObject ( cStr(pWidgetPath+"widgetXRot.dbo").Get(),t.widget.widgetXRotObj );
	LoadObject ( cStr(pWidgetPath+"widgetYRot.dbo").Get(),t.widget.widgetYRotObj );
	LoadObject ( cStr(pWidgetPath+"widgetZRot.dbo").Get(),t.widget.widgetZRotObj );
	LoadObject ( cStr(pWidgetPath+"widgetXScale.dbo").Get(),t.widget.widgetXScaleObj );
	LoadObject ( cStr(pWidgetPath+"widgetYScale.dbo").Get(),t.widget.widgetYScaleObj );
	LoadObject ( cStr(pWidgetPath+"widgetZScale.dbo").Get(),t.widget.widgetZScaleObj );
	#else
	LoadObject (  "editors\\gfx\\widget\\widgetX.x",t.widget.widgetXObj );
	LoadObject (  "editors\\gfx\\widget\\widgetY.x",t.widget.widgetYObj );
	LoadObject (  "editors\\gfx\\widget\\widgetZ.x",t.widget.widgetZObj );
	LoadObject (  "editors\\gfx\\widget\\widgetXY.x",t.widget.widgetXYObj );
	LoadObject (  "editors\\gfx\\widget\\widgetXZ.x",t.widget.widgetXZObj );
	LoadObject (  "editors\\gfx\\widget\\widgetYZ.x",t.widget.widgetYZObj );
	LoadObject (  "editors\\gfx\\widget\\widgetXRot.x",t.widget.widgetXRotObj );
	LoadObject (  "editors\\gfx\\widget\\widgetYRot.x",t.widget.widgetYRotObj );
	LoadObject (  "editors\\gfx\\widget\\widgetZRot.x",t.widget.widgetZRotObj );
	LoadObject (  "editors\\gfx\\widget\\widgetXScale.x",t.widget.widgetXScaleObj );
	LoadObject (  "editors\\gfx\\widget\\widgetYScale.x",t.widget.widgetYScaleObj );
	LoadObject (  "editors\\gfx\\widget\\widgetZScale.x",t.widget.widgetZScaleObj );
	LoadObject (  "editors\\gfx\\widget\\widgetXCol.x",t.widget.widgetXColObj );
	LoadObject (  "editors\\gfx\\widget\\widgetYCol.x",t.widget.widgetYColObj );
	LoadObject (  "editors\\gfx\\widget\\widgetZCol.x",t.widget.widgetZColObj );
	if (0)
	{
		// I need DBOs for WickedMAX, so create them here for consistency (temp, though could turn these into DBO loads!)
		SaveObject ( "editors\\gfx\\widget\\widgetX.dbo",t.widget.widgetXObj );
		SaveObject ( "editors\\gfx\\widget\\widgetY.dbo",t.widget.widgetYObj );
		SaveObject ( "editors\\gfx\\widget\\widgetZ.dbo",t.widget.widgetZObj );
		SaveObject ( "editors\\gfx\\widget\\widgetXY.dbo",t.widget.widgetXYObj );
		SaveObject ( "editors\\gfx\\widget\\widgetXZ.dbo",t.widget.widgetXZObj );
		SaveObject ( "editors\\gfx\\widget\\widgetYZ.dbo",t.widget.widgetYZObj );
		SaveObject ( "editors\\gfx\\widget\\widgetXRot.dbo",t.widget.widgetXRotObj );
		SaveObject ( "editors\\gfx\\widget\\widgetYRot.dbo",t.widget.widgetYRotObj );
		SaveObject ( "editors\\gfx\\widget\\widgetZRot.dbo",t.widget.widgetZRotObj );
		SaveObject ( "editors\\gfx\\widget\\widgetXScale.dbo",t.widget.widgetXScaleObj );
		SaveObject ( "editors\\gfx\\widget\\widgetYScale.dbo",t.widget.widgetYScaleObj );
		SaveObject ( "editors\\gfx\\widget\\widgetZScale.dbo",t.widget.widgetZScaleObj );
		SaveObject ( "editors\\gfx\\widget\\widgetXCol.dbo",t.widget.widgetXColObj );
		SaveObject ( "editors\\gfx\\widget\\widgetYCol.dbo",t.widget.widgetYColObj );
		SaveObject ( "editors\\gfx\\widget\\widgetZCol.dbo",t.widget.widgetZColObj );
	}
	#endif
	
	// scale all object
	MakeObjectCube (  t.widget.widgetXYZScaleObj,2 );

	// give widgets a base texture
	TextureObject ( t.widget.widgetXObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetYObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetZObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetXYObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetXZObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetYZObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetXRotObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetYRotObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetZRotObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetXScaleObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetYScaleObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetZScaleObj, g.editorimagesoffset+14 );
	#ifdef WICKEDENGINE
	#else
	TextureObject ( t.widget.widgetXColObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetYColObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetZColObj, g.editorimagesoffset+14 );
	#endif

	// Widget Property Buttons
	#ifdef WICKEDENGINE
	#else
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\widget\\pos.png";
	LoadImage ( t.strwork.Get(),t.widget.imagestart+1 );
	t.strwork = ""; t.strwork = t.strwork +"languagebank\\"+g.language_s+"\\artwork\\widget\\rot.png";
	LoadImage ( t.strwork.Get(),t.widget.imagestart+2 );
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\widget\\scl.png";
	LoadImage (t .strwork.Get(),t.widget.imagestart+3 );
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\widget\\prp.png";
	LoadImage ( t.strwork.Get(),t.widget.imagestart+4 );
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\widget\\dup.png";
	LoadImage ( t.strwork.Get(),t.widget.imagestart+5 );
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\widget\\del.png";
	LoadImage ( t.strwork.Get(),t.widget.imagestart+6 );
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\widget\\lck.png";
	LoadImage ( t.strwork.Get(),t.widget.imagestart+7 );
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\widget\\edt.png";
	LoadImage ( t.strwork.Get(),t.widget.imagestart+8 );
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\widget\\sav.png";
	LoadImage ( t.strwork.Get(),t.widget.imagestart+9 );
	t.tbuttscale_f=0.015f;
	MakeObjectBox (  t.widget.widgetPOSObj,4,t.tbuttscale_f*50,t.tbuttscale_f  ); TextureObject (  t.widget.widgetPOSObj,t.widget.imagestart+1 );
	MakeObjectBox (  t.widget.widgetROTObj,4,t.tbuttscale_f*50,t.tbuttscale_f  ); TextureObject (  t.widget.widgetROTObj,t.widget.imagestart+2 );
	MakeObjectBox (  t.widget.widgetSCLObj,4,t.tbuttscale_f*50,t.tbuttscale_f  ); TextureObject (  t.widget.widgetSCLObj,t.widget.imagestart+3 );
	MakeObjectBox (  t.widget.widgetPRPObj,4,t.tbuttscale_f*50,t.tbuttscale_f  ); TextureObject (  t.widget.widgetPRPObj,t.widget.imagestart+4 );
	MakeObjectBox (  t.widget.widgetDUPObj,4,t.tbuttscale_f*50,t.tbuttscale_f  ); TextureObject (  t.widget.widgetDUPObj,t.widget.imagestart+5 );
	MakeObjectBox (  t.widget.widgetDELObj,4,t.tbuttscale_f*50,t.tbuttscale_f  ); TextureObject (  t.widget.widgetDELObj,t.widget.imagestart+6 );
	MakeObjectBox (  t.widget.widgetLCKObj,4,t.tbuttscale_f*50,t.tbuttscale_f  ); TextureObject (  t.widget.widgetLCKObj,t.widget.imagestart+7 );
	t.tmovezup_f=0.0f ; t.tmovestep_f=0.8f;
	OffsetLimb (  t.widget.widgetPOSObj,0,-1.2f,3.0f+(t.tmovestep_f*5),t.tmovezup_f  ); RotateLimb (  t.widget.widgetPOSObj,0,0,0,0 );
	OffsetLimb (  t.widget.widgetROTObj,0,-1.2f,3.0f+(t.tmovestep_f*4),t.tmovezup_f  ); RotateLimb (  t.widget.widgetROTObj,0,0,0,0 );
	OffsetLimb (  t.widget.widgetSCLObj,0,-1.2f,3.0f+(t.tmovestep_f*3),t.tmovezup_f  ); RotateLimb (  t.widget.widgetSCLObj,0,0,0,0 );
	OffsetLimb (  t.widget.widgetPRPObj,0,-1.2f,3.0f+(t.tmovestep_f*2),t.tmovezup_f  ); RotateLimb (  t.widget.widgetPRPObj,0,0,0,0 );
	OffsetLimb (  t.widget.widgetDUPObj,0,-1.2f,3.0f+t.tmovestep_f,t.tmovezup_f  ); RotateLimb (  t.widget.widgetDUPObj,0,0,0,0 );
	OffsetLimb (  t.widget.widgetDELObj,0,-1.2f,2.0f+t.tmovestep_f,t.tmovezup_f  ); RotateLimb (  t.widget.widgetDELObj,0,0,0,0 );
	OffsetLimb (  t.widget.widgetLCKObj,0,-1.2f,2.0f,t.tmovezup_f  ); RotateLimb (  t.widget.widgetLCKObj,0,0,0,0 );
	HideObject ( t.widget.widgetXColObj );
	HideObject ( t.widget.widgetYColObj );
	HideObject ( t.widget.widgetZColObj );
	#endif

	// Colorize the widget gadgets
	SetObjectEmissive ( t.widget.widgetXObj, Rgb(255,0,0) );
	SetObjectEmissive ( t.widget.widgetYObj, Rgb(0,255,0) );
	SetObjectEmissive ( t.widget.widgetZObj, Rgb(0,0,255) );
	SetObjectEmissive ( t.widget.widgetXYObj, Rgb(255,0,0) );
	SetObjectEmissive ( t.widget.widgetXZObj, Rgb(0,255,0) );
	SetObjectEmissive ( t.widget.widgetYZObj, Rgb(0,0,255) );
	SetObjectEmissive ( t.widget.widgetXRotObj, Rgb(255,0,0) );
	SetObjectEmissive ( t.widget.widgetYRotObj, Rgb(0,255,0) );
	SetObjectEmissive ( t.widget.widgetZRotObj, Rgb(0,0,255) );
	SetObjectEmissive ( t.widget.widgetXScaleObj, Rgb(255,0,0) );
	SetObjectEmissive ( t.widget.widgetYScaleObj, Rgb(0,255,0) );
	SetObjectEmissive ( t.widget.widgetZScaleObj, Rgb(0,0,255) );
	SetObjectEmissive ( t.widget.widgetXYZScaleObj, Rgb(255,255,255) );

	// Widget object render settings
	#ifdef WICKEDENGINE
	SetObjectDiffuse ( t.widget.widgetXObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetYObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetZObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetXYObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetXZObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetYZObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetXRotObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetYRotObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetZRotObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetXScaleObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetYScaleObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetZScaleObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetXYZScaleObj, Rgb(0,0,0) );
	SetObjectTransparency ( t.widget.widgetXObj, 6 );
	SetObjectTransparency ( t.widget.widgetYObj, 6 );
	SetObjectTransparency ( t.widget.widgetZObj, 6 );
	SetObjectTransparency ( t.widget.widgetXYObj, 6 );
	SetObjectTransparency ( t.widget.widgetXZObj, 6 );
	SetObjectTransparency ( t.widget.widgetYZObj, 6 );
	SetObjectTransparency ( t.widget.widgetXRotObj, 6 );
	SetObjectTransparency ( t.widget.widgetYRotObj, 6 );
	SetObjectTransparency ( t.widget.widgetZRotObj, 6 );
	SetObjectTransparency ( t.widget.widgetXScaleObj, 6 );
	SetObjectTransparency ( t.widget.widgetYScaleObj, 6 );
	SetObjectTransparency ( t.widget.widgetZScaleObj, 6 );
	SetObjectTransparency ( t.widget.widgetXYZScaleObj, 6 );
	#else
	DisableObjectZDepth (  t.widget.widgetXObj );
	DisableObjectZDepth (  t.widget.widgetYObj );
	DisableObjectZDepth (  t.widget.widgetZObj );
	DisableObjectZDepth (  t.widget.widgetXYObj );
	DisableObjectZDepth (  t.widget.widgetXZObj );
	DisableObjectZDepth (  t.widget.widgetYZObj );
	DisableObjectZDepth (  t.widget.widgetXRotObj );
	DisableObjectZDepth (  t.widget.widgetYRotObj );
	DisableObjectZDepth (  t.widget.widgetZRotObj );
	DisableObjectZDepth (  t.widget.widgetXScaleObj );
	DisableObjectZDepth (  t.widget.widgetYScaleObj );
	DisableObjectZDepth (  t.widget.widgetZScaleObj );
	DisableObjectZDepth (  t.widget.widgetXYZScaleObj );
	DisableObjectZDepth (  t.widget.widgetPOSObj );
	DisableObjectZDepth (  t.widget.widgetROTObj );
	DisableObjectZDepth (  t.widget.widgetSCLObj );
	DisableObjectZDepth (  t.widget.widgetPRPObj );
	DisableObjectZDepth (  t.widget.widgetDUPObj );
	DisableObjectZDepth (  t.widget.widgetDELObj );
	DisableObjectZDepth (  t.widget.widgetLCKObj );
	DisableObjectZRead (  t.widget.widgetXObj );
	DisableObjectZRead (  t.widget.widgetYObj );
	DisableObjectZRead (  t.widget.widgetZObj );
	DisableObjectZRead (  t.widget.widgetXYObj );
	DisableObjectZRead (  t.widget.widgetXZObj );
	DisableObjectZRead (  t.widget.widgetYZObj );
	DisableObjectZRead (  t.widget.widgetXRotObj );
	DisableObjectZRead (  t.widget.widgetYRotObj );
	DisableObjectZRead (  t.widget.widgetZRotObj );
	DisableObjectZRead (  t.widget.widgetXScaleObj );
	DisableObjectZRead (  t.widget.widgetYScaleObj );
	DisableObjectZRead (  t.widget.widgetZScaleObj );
	DisableObjectZRead (  t.widget.widgetXYZScaleObj );
	DisableObjectZRead (  t.widget.widgetPOSObj );
	DisableObjectZRead (  t.widget.widgetROTObj );
	DisableObjectZRead (  t.widget.widgetSCLObj );
	DisableObjectZRead (  t.widget.widgetPRPObj );
	DisableObjectZRead (  t.widget.widgetDUPObj );
	DisableObjectZRead (  t.widget.widgetDELObj );
	DisableObjectZRead (  t.widget.widgetLCKObj );
	SetObjectTransparency (  t.widget.widgetXYObj,6 );
	SetObjectTransparency (  t.widget.widgetXZObj,6 );
	SetObjectTransparency (  t.widget.widgetYZObj,6 );
	SetObjectTransparency (  t.widget.widgetPOSObj,6 );
	SetObjectTransparency (  t.widget.widgetROTObj,6 );
	SetObjectTransparency (  t.widget.widgetSCLObj,6 );
	SetObjectTransparency (  t.widget.widgetPRPObj,6 );
	for ( t.a = 0 ; t.a<=  t.widget.widgetMAXObj; t.a++ )
	{
		if ( ObjectExist(g.widgetobjectoffset+t.a) )  
		{
			SetObjectEffect ( g.widgetobjectoffset+t.a, g.guishadereffectindex );
		}
	}
	#endif

	// hide all widget objects
	for ( t.a = 0 ; t.a <= t.widget.widgetMAXObj; t.a++ )
	{
		if ( ObjectExist(g.widgetobjectoffset+t.a) ) HideObject ( g.widgetobjectoffset+t.a );
	}
	#ifdef WICKEDENGINE
	extern bool bTriggerVisibleWidget;
	bTriggerVisibleWidget = false;
	#endif

	// create a common widget plane we can use to detect mouse and control objects with widget gadgets
	#ifdef WICKEDENGINE
	WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_WIDGETPLANE);
	#endif
	#ifdef WICKEDENGINE
	MakeObjectBox (t.widget.widgetPlaneObj, 5000000, 5000000, 1);
	WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_NORMAL);
	WickedCall_PresetObjectIgnoreTextures(false);
	#else
	MakeObjectBox ( t.widget.widgetPlaneObj, 100000, 100000, 1 );
	#endif
	RotateObject ( t.widget.widgetPlaneObj,-90,0,0 );
	HideObject ( t.widget.widgetPlaneObj );

	// Offset to rotation objects
	t.twidgetRotStartObject = g.widgetobjectoffset+30;

	// create widget helper objects
	MakeObjectSphere ( t.twidgetRotStartObject+11,60  ); HideObject (  t.twidgetRotStartObject+11 );
	MakeObjectSphere ( t.twidgetRotStartObject+12,70  ); HideObject (  t.twidgetRotStartObject+12 );
	MakeObjectSphere ( t.twidgetRotStartObject+13,65  ); HideObject (  t.twidgetRotStartObject+13 );
	MakeObjectPlane ( t.twidgetRotStartObject+14,10000,10000  ); HideObject (  t.twidgetRotStartObject+14 );

	// widget vector and matrices
	t.r=MakeVector3(g.widgetvectorindex+1);
	t.r=MakeMatrix(g.widgetStartMatrix+2);
	t.r=MakeMatrix(g.widgetStartMatrix+3);
	t.r=MakeMatrix(g.widgetStartMatrix+4);
	t.r=MakeMatrix(g.widgetStartMatrix+5);
	t.r=MakeVector3(g.widgetvectorindex+6);
	t.r=MakeVector3(g.widgetvectorindex+7);
	t.r=MakeVector3(g.widgetvectorindex+8);
	t.r=MakeVector3(g.widgetvectorindex+9);
	t.r=MakeVector3(g.widgetvectorindex+10);
	t.r=MakeVector3(g.widgetvectorindex+11);
	t.r=MakeVector3(g.widgetvectorindex+12);

	// reset var to store when widget gadget selected
	t.widget.pickedSection = 0;
}

void widget_movezonesandlights(int e)
{
	// handle zones if moved
	t.waypointindex = t.entityelement[e].eleprof.trigger.waypointzoneindex;
	t.thisx_f = t.entityelement[e].x;
	t.thisy_f = t.entityelement[e].y;
	t.thisz_f = t.entityelement[e].z;
	waypoint_movetothiscoordinate();

	// also update light data for this entity
	if (t.entityprofile[t.entityelement[e].bankindex].ismarker == 2)
	{
		int iLightIndex = t.entityelement[e].eleprof.light.index;
		if (iLightIndex >= 0)
		{
			t.infinilight[iLightIndex].x = t.entityelement[e].x;
			t.infinilight[iLightIndex].y = t.entityelement[e].y;
			t.infinilight[iLightIndex].z = t.entityelement[e].z;
			t.infinilight[iLightIndex].is_spot_light = t.entityelement[e].eleprof.usespotlighting;
			t.infinilight[iLightIndex].range = t.entityelement[e].eleprof.light.range;
			#ifdef WICKEDENGINE
			t.infinilight[iLightIndex].spotlightradius = t.entityelement[e].eleprof.light.offsetup;
			t.infinilight[iLightIndex].fLightHasProbe = t.entityelement[e].eleprof.light.fLightHasProbe;
			if (t.entityelement[e].eleprof.castshadow == 1)
				t.infinilight[iLightIndex].bCanShadow = false;
			else
				t.infinilight[iLightIndex].bCanShadow = true;
			#endif
			t.infinilight[iLightIndex].colrgb.r = RgbR(t.entityelement[e].eleprof.light.color);
			t.infinilight[iLightIndex].colrgb.g = RgbG(t.entityelement[e].eleprof.light.color);
			t.infinilight[iLightIndex].colrgb.b = RgbB(t.entityelement[e].eleprof.light.color);
			sObject* pObject = g_ObjectList[t.entityelement[e].obj];
			if (pObject)
			{
				#ifdef WICKEDENGINE
				//PE: In wicked we use the rotation vector for spot light.
				t.infinilight[iLightIndex].f_angle_x = pObject->position.vecRotate.x;
				t.infinilight[iLightIndex].f_angle_y = pObject->position.vecRotate.y;
				t.infinilight[iLightIndex].f_angle_z = pObject->position.vecRotate.z;
				#else
				t.infinilight[iLightIndex].f_angle_x = pObject->position.vecLook.x;
				t.infinilight[iLightIndex].f_angle_y = pObject->position.vecLook.y;
				t.infinilight[iLightIndex].f_angle_z = pObject->position.vecLook.z;
				#endif
			}
		}
	}

	// also update if particle emitter
	#ifdef WICKEDENGINE
	entity_updateparticleemitter(e);
	entity_updateautoflatten(e);
	#endif
}

#ifdef WICKEDENGINE
int PickObjectUsingWicked(void)
{
	//PE: Dont change anything when right mouse down.
	if (ImGui::IsMouseDown(1)) return 0;

	//PE: We was sending two very very slow WickedCall_GetPick calls per frame (GGRENDERLAYERS_NORMAL), now reuse if we already sent one. mouse coords has not changed ...
	extern int iReusePickObjectID;
	extern sObject* pReusePickObject;
	if (iReusePickObjectID != -1)
	{
		//We already sent a ray.
		if (pReusePickObject)
		{
			sObject* pHitObject = pReusePickObject;
			if (pHitObject)
			{
				int iObjectID = pHitObject->dwObjectNumber;
				if (iObjectID >= g.widgetobjectoffset + 0 && iObjectID <= g.widgetobjectoffset + t.widget.widgetMAXObj)
				{
					return iObjectID;
				}
			}
		}
		return(0);
	}
	uint64_t hitentity = 0;
	float fHitX, fHitY, fHitZ;
	WickedCall_GetPick(&fHitX, &fHitY, &fHitZ, NULL, NULL, NULL, &hitentity, GGRENDERLAYERS_NORMAL);
	if (hitentity > 0)
	{
		// found object under hovering cursor, match to entity index
		sObject* pHitObject = m_ObjectManager.FindObjectFromWickedObjectEntityID(hitentity);
		if (pHitObject)
		{
			int iObjectID = pHitObject->dwObjectNumber;
			if (iObjectID >= g.widgetobjectoffset + 0 && iObjectID <= g.widgetobjectoffset + t.widget.widgetMAXObj)
			{
				return iObjectID;
			}
		}
	}
	return 0;
}
#endif

#ifdef WICKEDENGINE
bool widget_getplanepos ( float fActivePosX, float fActivePosY, float fActivePosZ, float* pPlanePosX, float* pPlanePosY, float* pPlanePosZ )
{
	// when drag in object initially, use new smart placement system
	extern int iObjectMoveMode;
	bool bJustForInitialDragIn = false;
	extern bool bDraggingActiveInitial;
	if(bDraggingActiveInitial==true )
		bJustForInitialDragIn = true;

	bool bPlanePosRegistered = false;
	if (ObjectExist(t.widget.widgetPlaneObj))
	{
		PositionObject(t.widget.widgetPlaneObj, fActivePosX, fActivePosY, fActivePosZ);
		if (t.widget.pickedSection == -99)
		{
			float fUpDownAngle = WrapValue(CameraAngleX(0));
			if (iObjectMoveMode == 0 && bJustForInitialDragIn == false)
			{
				// for XZ mode, always use horizon plane (after initial drag in)
				fUpDownAngle = 45.0f;
			}
			// if facing to the side, and in smart mode, prefer floor plane
			int iForwardFacing = t.entityprofile[t.gridentity].forwardfacing;
			if (bDraggingActiveInitial == false)
			{
				if (fUpDownAngle > 10.0f && fUpDownAngle < 350.0f)
				{
					// as you were
				}
				else
				{
					if (iObjectMoveMode == 2 && iForwardFacing == 0)
					{
						// prefer floor plane for ground and wall objects
						fUpDownAngle = 45.0f;
					}
					else
					{
						// ceiling objects keep to up and down
					}
				}
			}
			if (bDraggingActiveInitial == false && fUpDownAngle > 10.0f && fUpDownAngle < 350.0f)
			{
				// a horizontal plane for up/down views
				if (iForwardFacing == 0)
				{
					RotateObject(t.widget.widgetPlaneObj, -90, 0, 0);
				}
				if (iForwardFacing == 1)
				{
					// plane takes on angle of forward facing object (which can orient to the ray detected jormal direction)
					SetObjectToObjectOrientation(t.widget.widgetPlaneObj, t.gridentityobj);
				}
				if (iForwardFacing == 2)
				{
					RotateObject(t.widget.widgetPlaneObj, -90, 0, 0);
				}
			}
			else
			{
				// a vertical plane for sidelong views
				RotateObject(t.widget.widgetPlaneObj, 0, CameraAngleY(0), 0);
			}
		}
		if (t.widget.pickedSection == -98)
		{
			// will provide good Y up and down no matter the angle!
			//PointObject(t.widget.widgetPlaneObj, CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0)); 
			RotateObject(t.widget.widgetPlaneObj, 0, CameraAngleY(0), 0);
		}
		if (t.widget.pickedSection == t.widget.widgetXObj) RotateObject(t.widget.widgetPlaneObj, -90, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetYObj) RotateObject(t.widget.widgetPlaneObj, 0, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetZObj) RotateObject(t.widget.widgetPlaneObj, -90, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetXYObj) RotateObject(t.widget.widgetPlaneObj, 0, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetXZObj) RotateObject(t.widget.widgetPlaneObj, -90, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetYZObj) RotateObject(t.widget.widgetPlaneObj, 0, 90, 0);
		if (t.widget.pickedSection == t.widget.widgetYRotObj) RotateObject(t.widget.widgetPlaneObj, 90, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetXRotObj) RotateObject(t.widget.widgetPlaneObj, 0, 90, 0);
		if (t.widget.pickedSection == t.widget.widgetZRotObj) RotateObject(t.widget.widgetPlaneObj, 0, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetXScaleObj) PointObject(t.widget.widgetPlaneObj, CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0));
		if (t.widget.pickedSection == t.widget.widgetYScaleObj) PointObject(t.widget.widgetPlaneObj, CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0));
		if (t.widget.pickedSection == t.widget.widgetZScaleObj) PointObject(t.widget.widgetPlaneObj, CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0));
		if (t.widget.pickedSection == t.widget.widgetXYZScaleObj) PointObject(t.widget.widgetPlaneObj, CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0));

		WickedCall_UpdateObject(GetObjectData(t.widget.widgetPlaneObj));
		WickedCall_UpdateSceneForPick();
		ShowObject(t.widget.widgetPlaneObj); //PE: Need to be visible while pick.
		int layer = GGRENDERLAYERS_WIDGETPLANE;
		// LB: All this commented out below messed up -99 (horiz position mode) which NEEDS
		// the plane to be at the pPlanePosY (typically taken from the object being manipulated
		// we cannot use the terrain as this forces the ray to pick something in the massive distance!!
		//#if 1 // TERRAIN_RAYCAST_HACK
		// -99 should only need terrain? widget plane is placed at wrong height and not big enough
		// this seems to fight with WickedCall_GetPick in E-GridEdit.cpp line 19138?
		//if (t.widget.pickedSection == -99) layer = GGRENDERLAYERS_TERRAIN; 
		//#endif
		bPlanePosRegistered = WickedCall_GetPick(pPlanePosX, pPlanePosY, pPlanePosZ, NULL, NULL, NULL, NULL, layer);
		
		//
		// A good way to debug what is being clicked and coordinates returned
		//
		HideObject(t.widget.widgetPlaneObj);
		
		if (t.widget.pickedSection == t.widget.widgetXObj) { *pPlanePosZ = fActivePosZ; *pPlanePosY = fActivePosY; }
		if (t.widget.pickedSection == t.widget.widgetYObj) { *pPlanePosX = fActivePosX; *pPlanePosZ = fActivePosZ; }
		if (t.widget.pickedSection == t.widget.widgetZObj) { *pPlanePosX = fActivePosX; *pPlanePosY = fActivePosY; }
		if (t.widget.pickedSection == t.widget.widgetXYObj) { *pPlanePosZ = fActivePosZ; }
		if (t.widget.pickedSection == t.widget.widgetXZObj) { *pPlanePosY = fActivePosY; }
		if (t.widget.pickedSection == -99)
		{
			if ( iObjectMoveMode == 2 || bJustForInitialDragIn == true )
			{
				// allow smart mode 2 full access to plane XYZ
			}
			else
			{
				*pPlanePosY = fActivePosY;
			}
		}
		if (t.widget.pickedSection == -98) 
		{ 
			// LB: keep purity of the plane click (have adjusted it to be exactly where clicked in code elsewhere)
			//*pPlanePosX = fActivePosX;
			//*pPlanePosZ = fActivePosZ;
		}
		if (t.widget.pickedSection == t.widget.widgetYZObj) { *pPlanePosX = fActivePosX; }
	}
	return bPlanePosRegistered;
}

void widget_cancelplanerotation( float *pPlanePosX, float *pPlanePosY, float *pPlanePosZ)
{
	float fDet;
	GGVECTOR3 vecRelPlanePos = GGVECTOR3(*pPlanePosX, *pPlanePosY, *pPlanePosZ);
	sObject* pObjPtr = GetObjectData(t.widget.widgetPlaneObj);
	GGMATRIX matInvPlaneRotation = pObjPtr->position.matRotation;
	GGMatrixInverse(&matInvPlaneRotation, &fDet, &matInvPlaneRotation);
	GGVec3TransformCoord(&vecRelPlanePos, &vecRelPlanePos, &matInvPlaneRotation);
	*pPlanePosX = vecRelPlanePos.x;
	*pPlanePosY = vecRelPlanePos.y;
	*pPlanePosZ = vecRelPlanePos.z;
}
#endif

void widget_loop ( void )
{
	// fixed camera projection
	SetCurrentCamera (  0 );
	SetCameraRange (  DEFAULT_NEAR_PLANE, DEFAULT_FAR_PLANE );

	// real widget mouse X Y
	if ( t.widget.protoineffect == 1 ) 
	{
		t.widgetinputsysxmouse_f=t.inputsys.xmouse;
		t.widgetinputsysymouse_f=t.inputsys.ymouse;
	}
	else
	{
		// only update if mouse within 3D view
		if ( t.inputsys.xmouse != 500000 )
		{
			#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
			//PE: imgui Need testing.
			t.widgetinputsysxmouse_f = ((float)t.inputsys.xmouse / (float)GetDisplayWidth()) / ((float)GetDisplayWidth() / (float)GetChildWindowWidth(-1));
			t.widgetinputsysymouse_f = ((float)t.inputsys.ymouse / (float)GetDisplayHeight()) / ((float)GetDisplayHeight() / (float)GetChildWindowHeight(-1));
			#else
			//  work out visible part of full backbuffer (i.e. 1212 of 1360)
			t.widgetinputsysxmouse_f=(GetDisplayWidth()+0.0)/(GetChildWindowWidth()+0.0);
			t.widgetinputsysymouse_f=(GetDisplayHeight()+0.0)/(GetChildWindowHeight()+0.0);
			//  scale full mouse to fit in visible area
			t.widgetinputsysxmouse_f=((t.inputsys.xmouse+0.0)/800.0)/t.widgetinputsysxmouse_f;
			t.widgetinputsysymouse_f=((t.inputsys.ymouse+0.0)/600.0)/t.widgetinputsysymouse_f;
			#endif
			//  then provide in a format for the pick-from-screen command
			#ifdef DX11
			t.widgetinputsysxmouse_f=t.widgetinputsysxmouse_f*(GetDisplayWidth()+0.0);
			t.widgetinputsysymouse_f=t.widgetinputsysymouse_f*(GetDisplayHeight()+0.0);
			#else
			t.widgetinputsysxmouse_f=t.widgetinputsysxmouse_f*(GetChildWindowWidth()+0.0);
			t.widgetinputsysymouse_f=t.widgetinputsysymouse_f*(GetChildWindowHeight()+0.0);
			#endif
		}
	}

	// highlighter
	if ( t.widget.activeObject>0 ) 
	{
		if ( t.widget.pickedSection != 0  ) t.tdim_f = 15; else t.tdim_f = 50;
		#ifdef WICKEDENGINE
		#else
		if ( t.widget.pickedSection >= t.widget.widgetPOSObj && t.widget.pickedSection <= t.widget.widgetLCKObj ) t.tdim_f = 50;
		#endif
		for ( t.a = 0 ; t.a <= t.widget.widgetMAXObj; t.a++ )
		{
			if ( ObjectExist(g.widgetobjectoffset+t.a) ) SetAlphaMappingOn ( g.widgetobjectoffset+t.a, t.tdim_f );
		}
		if ( t.widget.pickedSection != 0 ) 
		{
			t.thighlighterobj = t.widget.pickedSection;
		}
		else
		{
			#ifdef WICKEDENGINE
			t.thighlighterobj = PickObjectUsingWicked();
			#else
			t.thighlighterobj = PickScreenObject(t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, g.widgetobjectoffset+0, g.widgetobjectoffset+t.widget.widgetMAXObj);
			#endif
		}
		if (t.thighlighterobj > 0)
		{
			SetAlphaMappingOn(t.thighlighterobj, 75.0);
		}
	}
	else
	{
		//  ensure no widget button detection if no active object
		t.thighlighterobj=0;
		t.widget.pickedSection=0;
	}

	//  If no picked object and the widget has been in use, switch it off
	widget_updatewidgetobject ( );

	// Ensure any old widget object ptr is updated
	if ( t.widget.activeObject>0 ) 
	{
		if ( ObjectExist(t.widget.activeObject) == 0 ) 
		{
			t.widget.activeObject=0;
		}
	}

	#ifdef WICKEDENGINE
	extern bool Shooter_Tools_Window_Active;
	extern preferences pref;
	extern bool bTriggerVisibleWidget;
	if (Shooter_Tools_Window_Active || (pref.iEnableDragDropEntityMode && !bTriggerVisibleWidget))
	{
		//Hide for now.
		if (!widget_temp_disabled)
		{
			for (t.a = 0; t.a <= t.widget.widgetMAXObj; t.a++)
			{
				if (ObjectExist(g.widgetobjectoffset + t.a)) HideObject(g.widgetobjectoffset + t.a);
			}
			widget_temp_disabled = true;
			extern bool bTriggerVisibleWidget;
			bTriggerVisibleWidget = false;
		}
	}
	#endif

	#ifdef WICKEDENGINE
	if (t.widget.pickedEntityIndex > 0 && t.entityelement[t.widget.pickedEntityIndex].editorlock == 1)
	{
		//PE: hide. but keep t.widget.pickedEntityIndex for properties.
		for (t.a = 0; t.a <= t.widget.widgetMAXObj; t.a++)
		{
			if (ObjectExist(g.widgetobjectoffset + t.a)) HideObject(g.widgetobjectoffset + t.a);
		}
		extern bool bTriggerVisibleWidget;
		bTriggerVisibleWidget = false;
	}
	#endif

	// check if the widget is needed
	if ( t.widget.activeObject == 0 )
	{
		widget_check_for_new_object_selection ( );
	}
	else
	{
		#ifdef WICKEDENGINE
		if ( (!Shooter_Tools_Window_Active && (!pref.iEnableDragDropEntityMode || bTriggerVisibleWidget ) )  && widget_temp_disabled)
		{
			widget_temp_disabled = false;
			widget_show_widget();
		}
		#endif
		// Setup positions for widget objects
		if ( ObjectExist(t.widget.activeObject) == 1 ) 
		{
			for ( t.a = 0 ; t.a <= t.widget.widgetMAXObj; t.a++ )
			{
				if ( ObjectExist (g.widgetobjectoffset+t.a) )  
				{
					#ifdef WICKEDENGINE
					if ( GetVisible (g.widgetobjectoffset+t.a) )
					#endif
					{
						PositionObject(g.widgetobjectoffset + t.a, CameraPositionX(), CameraPositionY(), CameraPositionZ());
						if ( t.widget.mode == 0 )
							PointObject(g.widgetobjectoffset + t.a, ObjectPositionX(t.widget.activeObject) + t.widget.offsetx, ObjectPositionY(t.widget.activeObject) + t.widget.offsety, ObjectPositionZ(t.widget.activeObject) + t.widget.offsetz);
						else
							PointObject(g.widgetobjectoffset + t.a, ObjectPositionX(t.widget.activeObject) + 0, ObjectPositionY(t.widget.activeObject) + 0, ObjectPositionZ(t.widget.activeObject) + 0);
						MoveObject(g.widgetobjectoffset + t.a, 40);
						RotateObject(g.widgetobjectoffset + t.a, 0, 0, 0);
					}
					#ifdef WICKEDENGINE
					else
					{
						PositionObject(g.widgetobjectoffset + t.a, -100000, -100000, -100000);
					}
					#endif
				}
			}

			//  detect if widget panel off screen, and shift back in
			widget_correctwidgetpanel ( );

			// orient widget rot gadgets to active object
			RotateObject(g.widgetobjectoffset + 6, 0, 0, 0);
			RotateObject(g.widgetobjectoffset + 7, 0, 0, 0);
			RotateObject(g.widgetobjectoffset + 8, 0, 0, 0);

			// orient widget scale gadgets to active object
			RotateObject(g.widgetobjectoffset + 9, ObjectAngleX(t.widget.activeObject), ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject));
			RotateObject(g.widgetobjectoffset + 10, ObjectAngleX(t.widget.activeObject), ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject));
			RotateObject(g.widgetobjectoffset + 11, ObjectAngleX(t.widget.activeObject), ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject));
			RotateObject(g.widgetobjectoffset + 12, ObjectAngleX(t.widget.activeObject), ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject));
		}

		//  If we havent picked a section of the widget, lets test for it
		if ( t.widget.pickedSection == 0 )//|| g.fForceYRotationOfRubberBandFromKeyPress > 0.0f )
		{
			//  Test for the widget selection keys X,C and V
			t.tscancode = t.inputsys.kscancode;
			//if ( g.fForceYRotationOfRubberBandFromKeyPress != 0.0f )
			//{
			//	t.toldmode = t.widget.mode; t.widget.mode = 1;
			//	if ( t.toldmode != t.widget.mode ) widget_show_widget ( );
			//	t.widget.mclickpress = 2;
			//}
			//else
			{
				if (  t.widget.oldScanCode !=  t.tscancode ) 
				{
					t.toldmode = t.widget.mode;
					if (  t.tscancode  ==  WIDGET_KEY_TRANSLATE  )  t.widget.mode  =  0;
					if (  t.tscancode  ==  WIDGET_KEY_ROTATE  )  t.widget.mode  =  1;
					if (  t.tscancode  ==  WIDGET_KEY_SCALE  )  t.widget.mode  =  2;
					if ( t.toldmode != t.widget.mode ) widget_show_widget ( );
				}
			}
			t.widget.oldScanCode = t.tscancode;

			// some setup code for picking widget section
			#ifdef WICKEDENGINE
			#else
			if ( t.thighlighterobj == t.widget.widgetPRPObj ) 
			{
				// click and release
				if ( t.inputsys.mclick == 1 && t.widget.oldMouseClick == 0 && t.widget.mclickpress == 0  )  t.widget.mclickpress = 1;
				if ( t.inputsys.mclick == 0 && t.widget.mclickpress == 1  )  t.widget.mclickpress = 2;
			}
			else
			#endif
			{
				// just click
				if ( t.inputsys.mclick == 1 && t.widget.oldMouseClick == 0 ) t.widget.mclickpress = 2;
			}
				
			if (t.widget.mclickpress == 2)
			{
				// See if a section has been chosen
				#ifdef WICKEDENGINE
				t.widget.pickedSection = PickObjectUsingWicked();

				if (t.widget.pickedSection > 0)
				{
					//Make sure to highlight all objects the object belong to.
					void CheckGroupListForRubberbandSelections(int entityindex);
					if (t.widget.pickedEntityIndex > 0)
						CheckGroupListForRubberbandSelections(t.widget.pickedEntityIndex);
				}
				#else
				t.widget.pickedSection = PickScreenObject(t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f, g.widgetobjectoffset+0, g.widgetobjectoffset+t.widget.widgetMAXObj);
				#endif

				//  as soon as click down, record for possible undo
				if ( t.widget.pickedSection > 0 )
				{
					// but only if we select a gadget (if click nothing we keep older position)
					t.sttentitytoselect=t.tentitytoselect;
					t.tentitytoselect=t.widget.pickedEntityIndex;
					if ( g.entityrubberbandlist.size() > 0 )
						gridedit_moveentityrubberband();
					else
					{
						#ifdef WICKEDENGINE
						entity_createundoaction(eUndoSys_Object_ChangePosRotScl, t.tentitytoselect);
						#else
						entity_recordbuffer_move ( );
						#endif
					}
					t.tentitytoselect=t.sttentitytoselect;
				}

				// Translation, rotation and scale
				#ifdef WICKEDENGINE
				t.widget.mclickpress=0;
				t.toriginalTranslateClickDeferred = 3;
				t.toriginalTranslateClickX_f = 0;
				t.toriginalTranslateClickY_f = 0;
				t.toriginalTranslateClickZ_f = 0;
				t.toriginalRotationClickX_f = 0;
				t.toriginalRotationClickY_f = 0;
				t.toriginalRotationClickZ_f = 0;
				float fPlanePosX, fPlanePosY, fPlanePosZ;
				float fActivePosX = ObjectPositionX(t.widget.activeObject) + 0;
				float fActivePosY = ObjectPositionY(t.widget.activeObject) + 0;
				float fActivePosZ = ObjectPositionZ(t.widget.activeObject) + 0;
				if (t.widget.mode == 0)
				{
					fActivePosX = ObjectPositionX(t.widget.activeObject) + t.widget.offsetx;
					fActivePosY = ObjectPositionY(t.widget.activeObject) + t.widget.offsety;
					fActivePosZ = ObjectPositionZ(t.widget.activeObject) + t.widget.offsetz;
				}
				bool bPlanePosRegistered = widget_getplanepos(fActivePosX, fActivePosY, fActivePosZ, &fPlanePosX, &fPlanePosY, &fPlanePosZ );
				if (bPlanePosRegistered == true)
				{
					//LB: also use t.toriginalTranslateClickDeferred as the REAL plane pos is not good until NEXT cycle (for some reason)
					// record position where clicked
					t.toriginalTranslateClickX_f = fPlanePosX - fActivePosX;
					t.toriginalTranslateClickY_f = fPlanePosY - fActivePosY;
					t.toriginalTranslateClickZ_f = fPlanePosZ - fActivePosZ;

					//LB: moved below now using toriginalTranslateClickDeferred system (plane not detecting on first cycle - grr)
					/*
					// if rotation mode, additional init stuff when first click
					if (t.widget.mode == 1)
					{
						// now reorient the plane pos to account for the planes rotation (if rotating the widget)
						widget_cancelplanerotation(&t.toriginalTranslateClickX_f, &t.toriginalTranslateClickY_f, &t.toriginalTranslateClickZ_f);

						// record original angle of active object
						t.toriginalAngleX_f = ObjectAngleX(t.widget.activeObject);
						t.toriginalAngleY_f = ObjectAngleY(t.widget.activeObject);
						t.toriginalAngleZ_f = ObjectAngleZ(t.widget.activeObject);

						// get the quaternion of the objects rotation
						GGQUATERNION QuatAroundX, QuatAroundY, QuatAroundZ;
						GGQuaternionRotationAxis(&QuatAroundX, &GGVECTOR3(1, 0, 0), GGToRadian(t.toriginalAngleX_f));
						GGQuaternionRotationAxis(&QuatAroundY, &GGVECTOR3(0, 1, 0), GGToRadian(t.toriginalAngleY_f));
						GGQuaternionRotationAxis(&QuatAroundZ, &GGVECTOR3(0, 0, 1), GGToRadian(t.toriginalAngleZ_f));
						t.toriginalAngle = QuatAroundX * QuatAroundY * QuatAroundZ;

						// work out angle between start pos and new pos
						t.toriginalRotationClickX_f = GGToDegree(atan2(t.toriginalTranslateClickY_f, t.toriginalTranslateClickX_f));
						t.toriginalRotationClickY_f = GGToDegree(atan2(t.toriginalTranslateClickY_f, t.toriginalTranslateClickX_f));
						t.toriginalRotationClickZ_f = GGToDegree(atan2(t.toriginalTranslateClickY_f, t.toriginalTranslateClickX_f));
					}

					// if scale mode, additional init stuff when first click
					if (t.widget.mode == 2)
					{
						// now reorient the plane pos to account for the planes rotation (if scaling the widget)
						widget_cancelplanerotation(&t.toriginalTranslateClickX_f, &t.toriginalTranslateClickY_f, &t.toriginalTranslateClickZ_f);

						// record original scale of active object
						t.toriginalScalingX_f = ObjectScaleX(t.widget.activeObject);
						t.toriginalScalingY_f = ObjectScaleY(t.widget.activeObject);
						t.toriginalScalingZ_f = ObjectScaleZ(t.widget.activeObject);
					}
					*/
				}

				// record entity RY for ragdoll/character rotation code lower down
				t.tlastgoody_f = ObjectAngleY(t.widget.activeObject);

				// leelee, move below once it works so this code is not duplicated!! (also duplicated in gridedit for right panel -argzz)
				// record all current offsets from primary widget object
				bool bDisableRubberBandMoving = false;
				#ifdef WICKEDENGINE
				extern int current_selected_group;
				extern bool group_editing_on;
				if (current_selected_group >= 0 && group_editing_on)
				{
					bDisableRubberBandMoving = true;
				}
				#endif
				if (!bDisableRubberBandMoving)
				{
					// storing offsets in g.entityrubberbandlist[i].x/y/z/quat
					SetStartPositionsForRubberBand(t.widget.activeObject);

					/* old and duplicated
					for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
					{
						int e = g.entityrubberbandlist[i].e;
						GGVECTOR3 VecPos;
						VecPos.x = t.entityelement[e].x - ObjectPositionX(t.widget.activeObject);
						VecPos.y = t.entityelement[e].y - ObjectPositionY(t.widget.activeObject);
						VecPos.z = t.entityelement[e].z - ObjectPositionZ(t.widget.activeObject);
						// transform offset with current inversed orientation of primary object
						int tobj = t.entityelement[e].obj;
						if (tobj > 0)
						{
							float fDet = 0.0f;
							sObject* pObject = GetObjectData(tobj);
							GGMATRIX inverseMatrix = pObject->position.matObjectNoTran;
							GGMatrixInverse(&inverseMatrix, &fDet, &inverseMatrix);
							GGVec3TransformCoord(&VecPos, &VecPos, &inverseMatrix);
							g.entityrubberbandlist[i].x = VecPos.x;
							g.entityrubberbandlist[i].y = VecPos.y;
							g.entityrubberbandlist[i].z = VecPos.z;
						}
					}
					*/
				}
				#else
				// offset+20 is the plane object
				t.widget.mclickpress=0;
				ShowObject (  t.widget.widgetPlaneObj );
				PositionObject (  t.widget.widgetPlaneObj,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
				RotateObject (  t.widget.widgetPlaneObj,-90,0,0 );
				if (1)
				{
					t.ttdx_f = CameraPositionX() - ObjectPositionX(t.widget.activeObject);
					t.ttdy_f = CameraPositionY() - ObjectPositionY(t.widget.activeObject);
					t.ttdz_f = CameraPositionZ() - ObjectPositionZ(t.widget.activeObject);
					t.ttdist_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdy_f*t.ttdy_f) + abs(t.ttdz_f*t.ttdz_f));
					PickScreen2D23D(t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.ttdist_f);
				}

				t.toriginalClickX_f = CameraPositionX() + GetPickVectorX();
				t.toriginalClickY_f = CameraPositionY() + GetPickVectorY();
				t.toriginalClickZ_f = CameraPositionZ() + GetPickVectorZ();
				t.toriginalClick2X_f = t.toriginalClickX_f;
				t.toriginalClick2Y_f = t.toriginalClickY_f;
				t.toriginalClick2Z_f = t.toriginalClickZ_f;
				t.tdx_f = t.toriginalClickX_f - ObjectPositionX(t.widget.activeObject);
				t.tdy_f = t.toriginalClickY_f - ObjectPositionY(t.widget.activeObject);
				t.tdz_f = t.toriginalClickZ_f - ObjectPositionZ(t.widget.activeObject);
				t.toriginalDistance_f = Sqrt(t.tdx_f*t.tdx_f + t.tdy_f*t.tdy_f + t.tdz_f*t.tdz_f);
				t.fOriginalDistanceX = t.tdx_f;
				t.fOriginalDistanceZ = t.tdz_f;
				if (  t.widget.pickedSection  ==  t.widget.widgetYScaleObj ) 
				{
					RotateObject (  t.widget.widgetPlaneObj,0,0,0 );
					//t.a=PickScreenObject (t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.widget.widgetPlaneObj, t.widget.widgetPlaneObj);
					if (1) //(t.a == 0) //PE: Fixed
					{
						t.ttdx_f = CameraPositionX() - ObjectPositionX(t.widget.activeObject);
						t.ttdy_f = CameraPositionY() - ObjectPositionY(t.widget.activeObject);
						t.ttdz_f = CameraPositionZ() - ObjectPositionZ(t.widget.activeObject);
						t.ttdist_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdy_f*t.ttdy_f) + abs(t.ttdz_f*t.ttdz_f));
						PickScreen2D23D(t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.ttdist_f);
					}
					t.toriginalClick2X_f = CameraPositionX() + GetPickVectorX();
					t.toriginalClick2Y_f = CameraPositionY() + GetPickVectorY();
					t.toriginalClick2Z_f = CameraPositionZ() + GetPickVectorZ();
				}

				if (  t.widget.pickedSection  ==  t.widget.widgetXScaleObj ) 
				{
					if (  t.tdx_f<0  )  t.toriginalDistance_f = t.toriginalDistance_f*-1;
				}
				if (  t.widget.pickedSection  ==  t.widget.widgetYScaleObj ) 
				{
					if (  t.tdy_f<0  )  t.toriginalDistance_f = t.toriginalDistance_f*-1;
				}
				if (  t.widget.pickedSection  ==  t.widget.widgetZScaleObj ) 
				{
					if (  t.tdz_f<0  )  t.toriginalDistance_f = t.toriginalDistance_f*-1;
				}
				t.toriginalScaleX_f = ObjectScaleX(t.widget.activeObject);
				t.toriginalScaleY_f = ObjectScaleY(t.widget.activeObject);
				t.toriginalScaleZ_f = ObjectScaleZ(t.widget.activeObject);
				t.toriginalDistance2_f = Sqrt(t.tdx_f*t.tdx_f);
				if (  t.tdx_f<0  )  t.toriginalDistance2_f = t.toriginalDistance2_f*-1;

				// record all current offsets from primary widget object
				for ( int i = 0; i < (int)g.entityrubberbandlist.size(); i++ )
				{
					int e = g.entityrubberbandlist[i].e;
					GGVECTOR3 VecPos;
					VecPos.x = t.entityelement[e].x - ObjectPositionX ( t.widget.activeObject );
					VecPos.y = t.entityelement[e].y - ObjectPositionY ( t.widget.activeObject );
					VecPos.z = t.entityelement[e].z - ObjectPositionZ ( t.widget.activeObject );
					// transform offset with current inversed orientation of primary object
					int tobj = t.entityelement[e].obj;
					if ( tobj > 0 )
					{
						float fDet = 0.0f;
						sObject* pObject = GetObjectData(tobj);
						GGMATRIX inverseMatrix = pObject->position.matObjectNoTran;
						GGMatrixInverse ( &inverseMatrix, &fDet, &inverseMatrix );
						GGVec3TransformCoord ( &VecPos, &VecPos, &inverseMatrix );
						g.entityrubberbandlist[i].x = VecPos.x;
						g.entityrubberbandlist[i].y = VecPos.y;
						g.entityrubberbandlist[i].z = VecPos.z;
					}
				}

				RotateObject (  t.widget.widgetPlaneObj,0,0,0 );
				//t.a=PickScreenObject (t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.widget.widgetPlaneObj, t.widget.widgetPlaneObj) ;
				if (1) //(t.a == 0) //PE: Had wrong pick vectors if not found.
				{
					t.ttdx_f = CameraPositionX() - ObjectPositionX(t.widget.activeObject);
					t.ttdy_f = CameraPositionY() - ObjectPositionY(t.widget.activeObject);
					t.ttdz_f = CameraPositionZ() - ObjectPositionZ(t.widget.activeObject);
					t.ttdist_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdy_f*t.ttdy_f) + abs(t.ttdz_f*t.ttdz_f));
					PickScreen2D23D(t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.ttdist_f);
				}
				t.toriginalClickX_f = CameraPositionX() + GetPickVectorX();
				t.toriginalClickY_f = CameraPositionY() + GetPickVectorY();
				t.toriginalClickZ_f = CameraPositionZ() + GetPickVectorZ();
				t.tdx_f = t.toriginalClickX_f - ObjectPositionX(t.widget.activeObject);
				t.tdy_f = t.toriginalClickY_f - ObjectPositionY(t.widget.activeObject);
				t.tdz_f = t.toriginalClickZ_f - ObjectPositionZ(t.widget.activeObject);
				t.toriginalTranslateClickY_f = CameraPositionY() + GetPickVectorY() - ObjectPositionY(t.widget.activeObject);

				//  for the YZ position modifier
				RotateObject (  t.widget.widgetPlaneObj,0,90,0 );
				//t.a=PickScreenObject (t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.widget.widgetPlaneObj, t.widget.widgetPlaneObj);
				if (1) //(t.a == 0) //PE: Had wrong pick vectors if not found.
				{
					t.ttdx_f = CameraPositionX() - ObjectPositionX(t.widget.activeObject);
					t.ttdy_f = CameraPositionY() - ObjectPositionY(t.widget.activeObject);
					t.ttdz_f = CameraPositionZ() - ObjectPositionZ(t.widget.activeObject);
					t.ttdist_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdy_f*t.ttdy_f) + abs(t.ttdz_f*t.ttdz_f));
					PickScreen2D23D(t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.ttdist_f);
				}
				t.toriginalTranslateClickYonZ_f = CameraPositionY() + GetPickVectorY() - ObjectPositionY(t.widget.activeObject);

				//  record entity RY for ragdoll/character rotation code lower down
				t.tlastgoody_f = ObjectAngleY(t.widget.activeObject);

				ShowObject (  t.widget.widgetPlaneObj );
				PositionObject (  t.widget.widgetPlaneObj,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
				RotateObject (  t.widget.widgetPlaneObj, -90,0,0 );
				//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
				if (1) //(t.a == 0) //PE: Tested OK.
				{
					//PE: We get some huge jumpes/moves if we get here. it will use PicVector from prev call.
					t.ttdx_f = CameraPositionX() - ObjectPositionX(t.widget.activeObject);
					t.ttdy_f = CameraPositionY() - ObjectPositionY(t.widget.activeObject);
					t.ttdz_f = CameraPositionZ() - ObjectPositionZ(t.widget.activeObject);
					t.ttdist_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdy_f*t.ttdy_f) + abs(t.ttdz_f*t.ttdz_f));
					PickScreen2D23D(t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.ttdist_f);
				}
				//debugobj = t.a;
				t.toriginalTranslateClickX_f = CameraPositionX() + GetPickVectorX() - ObjectPositionX(t.widget.activeObject);
				t.toriginalTranslateClickZ_f = CameraPositionZ() + GetPickVectorZ() - ObjectPositionZ(t.widget.activeObject);

				//  310315 - XZ startclick for XY and ZY modding
				RotateObject (  t.widget.widgetPlaneObj,0,0,0 );
				//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
				if (1) //(t.a == 0) //PE: Had wrong pick vectors if not found.
				{
					t.ttdx_f = CameraPositionX() - ObjectPositionX(t.widget.activeObject);
					t.ttdy_f = CameraPositionY() - ObjectPositionY(t.widget.activeObject);
					t.ttdz_f = CameraPositionZ() - ObjectPositionZ(t.widget.activeObject);
					t.ttdist_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdy_f*t.ttdy_f) + abs(t.ttdz_f*t.ttdz_f));
					PickScreen2D23D(t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.ttdist_f);
				}
				t.toriginalTranslateClickX1_f = CameraPositionX() + GetPickVectorX() - ObjectPositionX(t.widget.activeObject);
				RotateObject (  t.widget.widgetPlaneObj,0,90,0 );
				//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
				if (1) //(t.a == 0) //PE: Had wrong pick vectors if not found.
				{
					t.ttdx_f = CameraPositionX() - ObjectPositionX(t.widget.activeObject);
					t.ttdy_f = CameraPositionY() - ObjectPositionY(t.widget.activeObject);
					t.ttdz_f = CameraPositionZ() - ObjectPositionZ(t.widget.activeObject);
					t.ttdist_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdy_f*t.ttdy_f) + abs(t.ttdz_f*t.ttdz_f));
					PickScreen2D23D(t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.ttdist_f);
				}
				t.toriginalTranslateClickZ2_f = CameraPositionZ() + GetPickVectorZ() - ObjectPositionZ(t.widget.activeObject);

				HideObject (  t.widget.widgetPlaneObj );

				//  mode 0 = translate, 1 = rotate, 2 = scale
				if (  t.widget.mode  ==  0 || t.widget.mode  ==  2 ) 
				{
					ShowObject (  t.widget.widgetXColObj );
					ShowObject (  t.widget.widgetYColObj );
					ShowObject (  t.widget.widgetZColObj );
					if (  t.widget.mode  ==  0 ) 
					{
						RotateObject (  g.widgetobjectoffset+16, 0,0,0 );
						RotateObject (  g.widgetobjectoffset+17, 0,0,0 );
						RotateObject (  g.widgetobjectoffset+18, 0,0,0 );
					}
					else
					{
						RotateObject (  g.widgetobjectoffset+16, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
						RotateObject (  g.widgetobjectoffset+17, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
						RotateObject (  g.widgetobjectoffset+18, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
					}
				}

				if (  t.widget.mode  ==  0 || t.widget.mode  ==  2 ) 
				{
					HideObject (  t.widget.widgetXColObj );
					HideObject (  t.widget.widgetYColObj );
					HideObject (  t.widget.widgetZColObj );
				}
				#endif
				#ifdef WICKEDENGINE
				// No widget property popup
				#else
				if ( t.widget.pickedSection >= t.widget.widgetPOSObj && t.widget.pickedSection <= t.widget.widgetLCKObj ) 
				{
					t.toldmode=t.widget.mode;
					if (  t.widget.pickedSection == t.widget.widgetPOSObj  )  t.widget.mode = 0;
					if (  t.widget.pickedSection == t.widget.widgetROTObj  )  t.widget.mode = 1;
					if (  t.widget.pickedSection == t.widget.widgetSCLObj  )  
					{
						bool bIsThisAnEBE = false;
						if ( t.widget.pickedEntityIndex > 0 ) 
						{
							int iEntID = t.entityelement[t.widget.pickedEntityIndex].bankindex;
							if ( iEntID > 0 ) 
								if ( t.entityprofile[iEntID].isebe != 0 )
									bIsThisAnEBE = true;
						}
						if ( bIsThisAnEBE == true )
						{
							//  Edit EBE construction
							t.widget.propertybuttonselected = 1;
							t.ebe.bReleaseMouseFirst = true;							
						}
						else
						{
							// default is scale
							t.widget.mode = 2;
						}
					}
					if (  t.widget.pickedSection == t.widget.widgetPRPObj ) 
					{
						//  entity properties / Save EBE construction
						t.widget.propertybuttonselected = 2;
					}
					if (  t.widget.pickedSection == t.widget.widgetDUPObj ) 
					{
						//  duplicate / now extract
						t.widget.duplicatebuttonselected=1;
					}
					if (  t.widget.pickedSection == t.widget.widgetDELObj ) 
					{
						//  delete
						t.widget.deletebuttonselected=1;
					}
					if ( t.widget.pickedSection == t.widget.widgetLCKObj ) 
					{
						//  entity lock/unlock
						if ( t.widget.pickedEntityIndex>0 ) 
						{
							int iLoopMax = 1;
							if ( g.entityrubberbandlist.size() > 0 ) iLoopMax = g.entityrubberbandlist.size();
							for ( int i = 0; i < iLoopMax; i++ )
							{
								// get entity index
								int e = t.widget.pickedEntityIndex;
								if ( g.entityrubberbandlist.size() > 0 )
									e = g.entityrubberbandlist[i].e;

								// toggle lock flag
								t.entityelement[e].editorlock = 1 - t.entityelement[e].editorlock;

								// also recreate entity as a clone and set as semi-transparent
								// this messes up depth render order, totally, best to leave as solid, just locked
								// LB - but the feature was useful to some users, so reinstated
								if ( t.entityelement[e].editorlock == 1 ) 
								{
									t.tte=e; t.tobj=t.entityelement[t.tte].obj;
									if ( t.tobj>0 ) 
									{
										if ( ObjectExist(t.tobj) == 1 ) 
										{
											t.entityelement[t.tte].isclone=0;
											entity_converttoclonetransparent ( );
										}
									}
								}
							}
							gridedit_clearentityrubberbandlist();
						}
						//  exit widget when lock entity
						t.widget.pickedSection=0;
					}
					if ( t.toldmode != t.widget.mode ) widget_show_widget ( );
				}
				else
				{
					if ( t.widget.pickedSection >= g.widgetobjectoffset+16 && t.widget.pickedSection <= g.widgetobjectoffset+t.widget.widgetMAXObj ) 
					{
						if ( t.widget.mode  ==  0  ) t.widget.pickedSection -= 16;
						if ( t.widget.mode  ==  2  ) t.widget.pickedSection -= 7;
					}
				}
				#endif

				// No section selected, so switch the widget off
				if ( t.widget.pickedSection == 0 )//&& g.fForceYRotationOfRubberBandFromKeyPress == 0.0f ) 
				{
					t.widget.activeObject = 0;
					t.widget.pickedObject = 0;
					if ( t.widget.activeObject == 0 ) 
					{
						for ( t.a = 0 ; t.a <= t.widget.widgetMAXObj; t.a++ )
						{
							if (  ObjectExist(g.widgetobjectoffset+t.a) ) 
							{
								PositionObject (  g.widgetobjectoffset+t.a,-100000,-100000,-100000 );
								HideObject (  g.widgetobjectoffset+t.a );
							}
						}
						extern bool bTriggerVisibleWidget;
						bTriggerVisibleWidget = false;
					}
				}
			}
		}
		if ( t.widget.pickedSection == 0 )//&& g.fForceYRotationOfRubberBandFromKeyPress == 0 )
		{
			// code done above, needed here to handle g.fForceYRotationOfRubberBandFromKeyPress logic
		}
		else
		{
			// store old active object position (for later if we need to also move rubber band highlighted objects)
			float fOldActiveObjectX = ObjectPositionX ( t.widget.activeObject );
			float fOldActiveObjectY = ObjectPositionY ( t.widget.activeObject );
			float fOldActiveObjectZ = ObjectPositionZ ( t.widget.activeObject );
			#ifdef WICKEDENGINE
			#else
			if ( t.widget.pickedSection >= t.widget.widgetPOSObj && t.widget.pickedSection <= t.widget.widgetLCKObj ) 
			{
				// widget button selected
			}
			else
			#endif
			{
				#ifdef WICKEDENGINE
				// defer detect system to fix the issue of the plane not being ready to return correct XYZ until frame AFTER plane is positioned
				if (t.toriginalTranslateClickDeferred > 0) t.toriginalTranslateClickDeferred--;
				// position
				if ( t.widget.mode == 0 && ObjectExist(t.widget.activeObject) == 1 && t.toriginalTranslateClickDeferred <= 1) //LB: mclickpress ensures this is not called in same cycle as the click!
				{
					float fPlanePosX, fPlanePosY, fPlanePosZ;
					float fActivePosX = ObjectPositionX(t.widget.activeObject) + t.widget.offsetx;
					float fActivePosY = ObjectPositionY(t.widget.activeObject) + t.widget.offsety;
					float fActivePosZ = ObjectPositionZ(t.widget.activeObject) + t.widget.offsetz;
					bool bPlanePosRegistered = widget_getplanepos(fActivePosX, fActivePosY, fActivePosZ, &fPlanePosX, &fPlanePosY, &fPlanePosZ );
					if (bPlanePosRegistered == true)
					{
						//LB: for some reason the plane 'shifted' on the second cycle, so update toriginalTranslateClickXYZ to account!
						if (t.toriginalTranslateClickDeferred == 1)
						{
							t.toriginalTranslateClickX_f = fPlanePosX - fActivePosX;
							t.toriginalTranslateClickY_f = fPlanePosY - fActivePosY;
							t.toriginalTranslateClickZ_f = fPlanePosZ - fActivePosZ;
							t.toriginalTranslateClickDeferred = 0;
						}

						// offset pos on plane with difference between widget click pos and active object
						fPlanePosX -= (t.toriginalTranslateClickX_f + t.widget.offsetx);
						fPlanePosY -= (t.toriginalTranslateClickY_f + t.widget.offsety);
						fPlanePosZ -= (t.toriginalTranslateClickZ_f + t.widget.offsetz);
						if (t.widget.pickedEntityIndex > 0 && (t.widget.pickedSection == t.widget.widgetXZObj || t.widget.pickedSection == t.widget.widgetXObj || t.widget.pickedSection == t.widget.widgetZObj ) )
						{
							int oldte = t.e;
							int oldtgridentity = t.gridentity;
							int oldtgridentityobj = t.gridentityobj;
							float oldtgridentityposx_f = t.gridentityposx_f;
							float oldtgridentityposy_f = t.gridentityposy_f;
							float oldtgridentityposz_f = t.gridentityposz_f;

							t.e = t.widget.pickedEntityIndex;
							t.gridentity = t.entityelement[t.e].bankindex;
							t.gridentityobj = t.widget.activeObject;
							t.gridentityposx_f = fPlanePosX;
							t.gridentityposy_f = fPlanePosY;
							t.gridentityposz_f = fPlanePosZ;


							#ifdef WICKEDENGINE
							//PE: Prevent user for placing objects outside playable area.
							bool bObjectOutSideEditArea = false;
							float fEditableSizeHalved = GGTerrain_GetEditableSize();
							t.terraineditableareasizeminx = -fEditableSizeHalved;
							t.terraineditableareasizeminz = -fEditableSizeHalved;
							t.terraineditableareasizemaxx = fEditableSizeHalved;
							t.terraineditableareasizemaxz = fEditableSizeHalved;
							if (t.gridentityposx_f < t.terraineditableareasizeminx) { t.gridentityposx_f = t.terraineditableareasizeminx; bObjectOutSideEditArea = true; }
							if (t.gridentityposx_f > t.terraineditableareasizemaxx) { t.gridentityposx_f = t.terraineditableareasizemaxx; bObjectOutSideEditArea = true; }
							if (t.gridentityposz_f < t.terraineditableareasizeminz) { t.gridentityposz_f = t.terraineditableareasizeminz; bObjectOutSideEditArea = true; }
							if (t.gridentityposz_f > t.terraineditableareasizemaxz) { t.gridentityposz_f = t.terraineditableareasizemaxz; bObjectOutSideEditArea = true; }

							if (bObjectOutSideEditArea)
							{
								//Trigger warning.
								extern char cSmallTriggerMessage[MAX_PATH];
								extern int iTriggerMessageFrames;
								extern bool bTriggerSmallMessage;
								sprintf(cSmallTriggerMessage, "Object is Outside Editable Area");
								iTriggerMessageFrames = 60;
								bTriggerSmallMessage = true;
							}
							#endif

							void Add_Grid_Snap_To_Position(void);
							Add_Grid_Snap_To_Position();

							fPlanePosX = t.gridentityposx_f;
							fPlanePosY = t.gridentityposy_f;
							fPlanePosZ = t.gridentityposz_f;

							t.e = oldte;
							t.gridentity = oldtgridentity;
							t.gridentityobj = oldtgridentityobj;
							t.gridentityposx_f = oldtgridentityposx_f;
							t.gridentityposy_f = oldtgridentityposy_f;
							t.gridentityposz_f = oldtgridentityposz_f;
						}

						// update active object position from planeposition detected 
						PositionObject(t.widget.activeObject, fPlanePosX, fPlanePosY, fPlanePosZ);

						// store in entity element
						t.te = t.widget.pickedEntityIndex;
						t.entityelement[t.te].x = ObjectPositionX(t.widget.activeObject);
						t.entityelement[t.te].y = ObjectPositionY(t.widget.activeObject);
						t.entityelement[t.te].z = ObjectPositionZ(t.widget.activeObject);

						// move zones and lights if in group
						widget_movezonesandlights ( t.te );

						bool bDisableRubberBandMoving = false;
						#ifdef WICKEDENGINE
						extern int current_selected_group;
						extern bool group_editing_on;
						if (current_selected_group >= 0 && group_editing_on)
						{
							bDisableRubberBandMoving = true;
						}
						#endif
						if (!bDisableRubberBandMoving)
						{
							// if we need to also move rubber band highlighted objects, do so now
							if (g.entityrubberbandlist.size() > 0)
							{
								float fMovedActiveObjectX = ObjectPositionX(t.widget.activeObject) - fOldActiveObjectX;
								float fMovedActiveObjectY = ObjectPositionY(t.widget.activeObject) - fOldActiveObjectY;
								float fMovedActiveObjectZ = ObjectPositionZ(t.widget.activeObject) - fOldActiveObjectZ;
								for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
								{
									int e = g.entityrubberbandlist[i].e;
									int tobj = t.entityelement[e].obj;
									if (tobj > 0 && t.entityelement[e].editorlock == 0 )
									{
										if (ObjectExist(tobj) == 1)
										{
											if (tobj != t.widget.activeObject)
											{
												// reposition this entity
												PositionObject(tobj, ObjectPositionX(tobj) + fMovedActiveObjectX, ObjectPositionY(tobj) + fMovedActiveObjectY, ObjectPositionZ(tobj) + fMovedActiveObjectZ);
												t.entityelement[e].x = ObjectPositionX(tobj);
												t.entityelement[e].y = ObjectPositionY(tobj);
												t.entityelement[e].z = ObjectPositionZ(tobj);
												if (t.entityelement[e].staticflag == 1) g.projectmodifiedstatic = 1;
												widget_movezonesandlights(e);
											}
										}
									}
								}
							}
						}
					}
				}
				#else
				// translate, rotation and scale
				//  translate
				if (  t.widget.mode == 0 && ObjectExist(t.widget.activeObject) == 1 ) 
				{
					if (  t.widget.pickedSection  ==  t.widget.widgetXObj ) 
					{
						//PE: Fixed
						PositionObject (  t.widget.widgetPlaneObj,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
						RotateObject (  t.widget.widgetPlaneObj, -90,0,0 );
						ShowObject (  t.widget.widgetPlaneObj );
						//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
						if (1) //(  t.a == 0 ) 
						{
							t.ttdx_f=CameraPositionX()-ObjectPositionX(t.widget.activeObject);
							t.ttdy_f=CameraPositionY()-ObjectPositionY(t.widget.activeObject);
							t.ttdz_f=CameraPositionZ()-ObjectPositionZ(t.widget.activeObject);
							t.ttdist_f=Sqrt(abs(t.ttdx_f*t.ttdx_f)+abs(t.ttdy_f*t.ttdy_f)+abs(t.ttdz_f*t.ttdz_f));
							PickScreen2D23D (  t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.ttdist_f );
						}

						//extern char cImGuiDebug[2048];
						//sprintf(cImGuiDebug, "HITOBJ: %d\ndebugobj: %f\nDIST: %f", t.a, debugobj, t.ttdist_f);
						
						HideObject (  t.widget.widgetPlaneObj );
						float fMaxMoveSpeed = GetPickVectorX();
						t.tx_f = CameraPositionX() + fMaxMoveSpeed;
						SlowPositionObject (  t.widget.activeObject,t.tx_f - t.toriginalTranslateClickX_f,ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
					}
					if (  t.widget.pickedSection  ==  t.widget.widgetYObj ) 
					{
						//PE: Fixed
						PositionObject (  t.widget.widgetPlaneObj,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
						RotateObject (  t.widget.widgetPlaneObj,0,0,0 );
						ShowObject (  t.widget.widgetPlaneObj );
						//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
						if (1) //(  t.a == 0 ) 
						{
							t.ttdx_f=CameraPositionX()-ObjectPositionX(t.widget.activeObject);
							t.ttdy_f=CameraPositionY()-ObjectPositionY(t.widget.activeObject);
							t.ttdz_f=CameraPositionZ()-ObjectPositionZ(t.widget.activeObject);
							t.ttdist_f=Sqrt(abs(t.ttdx_f*t.ttdx_f)+abs(t.ttdy_f*t.ttdy_f)+abs(t.ttdz_f*t.ttdz_f));
							PickScreen2D23D (  t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.ttdist_f );
						}
						HideObject (  t.widget.widgetPlaneObj );
						t.ty_f = CameraPositionY() + GetPickVectorY();
						SlowPositionObject (  t.widget.activeObject,ObjectPositionX(t.widget.activeObject),t.ty_f - t.toriginalTranslateClickY_f,ObjectPositionZ(t.widget.activeObject) );
					}
					if (  t.widget.pickedSection  ==  t.widget.widgetZObj ) 
					{
						//PE: Fixed
						PositionObject (  t.widget.widgetPlaneObj,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
						RotateObject (  t.widget.widgetPlaneObj,-90,0,0 );
						ShowObject (  t.widget.widgetPlaneObj );
						//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
						if (1) //(  t.a == 0 ) 
						{
							t.ttdx_f=CameraPositionX()-ObjectPositionX(t.widget.activeObject);
							t.ttdy_f=CameraPositionY()-ObjectPositionY(t.widget.activeObject);
							t.ttdz_f=CameraPositionZ()-ObjectPositionZ(t.widget.activeObject);
							t.ttdist_f=Sqrt(abs(t.ttdx_f*t.ttdx_f)+abs(t.ttdy_f*t.ttdy_f)+abs(t.ttdz_f*t.ttdz_f));
							PickScreen2D23D (  t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.ttdist_f );
						}
						HideObject (  t.widget.widgetPlaneObj );
						t.tz_f = CameraPositionZ() + GetPickVectorZ();
						SlowPositionObject(  t.widget.activeObject,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),t.tz_f - t.toriginalTranslateClickZ_f );
					}
					if (  t.widget.pickedSection  ==  t.widget.widgetXYObj ) 
					{
						//PE: Fixed
						PositionObject (  t.widget.widgetPlaneObj,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
						RotateObject (  t.widget.widgetPlaneObj,0,0,0 );
						ShowObject (  t.widget.widgetPlaneObj );
						//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
						if (1) //(  t.a == 0 ) 
						{
							t.ttdx_f=CameraPositionX()-ObjectPositionX(t.widget.activeObject);
							t.ttdy_f=CameraPositionY()-ObjectPositionY(t.widget.activeObject);
							t.ttdz_f=CameraPositionZ()-ObjectPositionZ(t.widget.activeObject);
							t.ttdist_f=Sqrt(abs(t.ttdx_f*t.ttdx_f)+abs(t.ttdy_f*t.ttdy_f)+abs(t.ttdz_f*t.ttdz_f));
							PickScreen2D23D (  t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.ttdist_f );
						}
						t.tx_f = CameraPositionX() + GetPickVectorX();
						//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
						if (1) //(  t.a == 0 )
						{
							t.ttdx_f=CameraPositionX()-ObjectPositionX(t.widget.activeObject);
							t.ttdy_f=CameraPositionY()-ObjectPositionY(t.widget.activeObject);
							t.ttdz_f=CameraPositionZ()-ObjectPositionZ(t.widget.activeObject);
							t.ttdist_f=Sqrt(abs(t.ttdx_f*t.ttdx_f)+abs(t.ttdy_f*t.ttdy_f)+abs(t.ttdz_f*t.ttdz_f));
							PickScreen2D23D (  t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.ttdist_f );
						}
						HideObject (  t.widget.widgetPlaneObj );
						t.ty_f = CameraPositionY() + GetPickVectorY();
						SlowPositionObject(  t.widget.activeObject,t.tx_f - t.toriginalTranslateClickX1_f,t.ty_f - t.toriginalTranslateClickY_f,ObjectPositionZ(t.widget.activeObject) );
					}
					if (  t.widget.pickedSection  ==  t.widget.widgetXZObj ) 
					{
						//PE: Fixed
						PositionObject (  t.widget.widgetPlaneObj,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
						RotateObject (  t.widget.widgetPlaneObj,-90,0,0 );
						ShowObject (  t.widget.widgetPlaneObj );
						//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
						if (1) //(  t.a == 0 ) 
						{
							//PE: This did not match with so gives huge jumps/moves.
							t.ttdx_f=CameraPositionX()-ObjectPositionX(t.widget.activeObject);
							t.ttdy_f=CameraPositionY()-ObjectPositionY(t.widget.activeObject);
							t.ttdz_f=CameraPositionZ()-ObjectPositionZ(t.widget.activeObject);
							t.ttdist_f=Sqrt(abs(t.ttdx_f*t.ttdx_f)+abs(t.ttdy_f*t.ttdy_f)+abs(t.ttdz_f*t.ttdz_f));
							PickScreen2D23D (  t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.ttdist_f );
						}
						HideObject (  t.widget.widgetPlaneObj );
						float fMaxMoveSpeedX = GetPickVectorX();
						float fMaxMoveSpeedZ = GetPickVectorZ();
						t.tx_f = CameraPositionX() + fMaxMoveSpeedX;
						t.tz_f = CameraPositionZ() + fMaxMoveSpeedZ;
						//extern char cImGuiDebug[2048];
						//sprintf(cImGuiDebug, "fMaxMoveSpeedX: %f\nfMaxMoveSpeedZ: %f", fMaxMoveSpeedX, fMaxMoveSpeedZ);
						SlowPositionObject(t.widget.activeObject, t.tx_f - t.toriginalTranslateClickX_f, ObjectPositionY(t.widget.activeObject), t.tz_f - t.toriginalTranslateClickZ_f);
					}
					if (  t.widget.pickedSection  ==  t.widget.widgetYZObj ) 
					{
						//PE: Fixed
						PositionObject (  t.widget.widgetPlaneObj,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
						RotateObject (  t.widget.widgetPlaneObj,0,90,0 );
						ShowObject (  t.widget.widgetPlaneObj );
						//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
						if (1) //(  t.a == 0 ) 
						{
							t.ttdx_f=CameraPositionX()-ObjectPositionX(t.widget.activeObject);
							t.ttdy_f=CameraPositionY()-ObjectPositionY(t.widget.activeObject);
							t.ttdz_f=CameraPositionZ()-ObjectPositionZ(t.widget.activeObject);
							t.ttdist_f=Sqrt(abs(t.ttdx_f*t.ttdx_f)+abs(t.ttdy_f*t.ttdy_f)+abs(t.ttdz_f*t.ttdz_f));
							PickScreen2D23D (  t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.ttdist_f );
						}
						t.tz_f = CameraPositionZ() + GetPickVectorZ();
						//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
						if (1) //(  t.a == 0 ) 
						{
							t.ttdx_f=CameraPositionX()-ObjectPositionX(t.widget.activeObject);
							t.ttdy_f=CameraPositionY()-ObjectPositionY(t.widget.activeObject);
							t.ttdz_f=CameraPositionZ()-ObjectPositionZ(t.widget.activeObject);
							t.ttdist_f=Sqrt(abs(t.ttdx_f*t.ttdx_f)+abs(t.ttdy_f*t.ttdy_f)+abs(t.ttdz_f*t.ttdz_f));
							PickScreen2D23D (  t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.ttdist_f );
						}
						HideObject (  t.widget.widgetPlaneObj );
						t.ty_f = CameraPositionY() + GetPickVectorY();
						SlowPositionObject(  t.widget.activeObject,ObjectPositionX(t.widget.activeObject),t.ty_f - t.toriginalTranslateClickYonZ_f,t.tz_f - t.toriginalTranslateClickZ2_f );
					}
					t.te=t.widget.pickedEntityIndex;
					t.entityelement[t.te].x=ObjectPositionX(t.widget.activeObject);
					t.entityelement[t.te].y=ObjectPositionY(t.widget.activeObject);
					t.entityelement[t.te].z=ObjectPositionZ(t.widget.activeObject);
					t.entityelement[t.te].beenmoved=1;
					// mark as static if it was
					if ( t.entityelement[t.te].staticflag == 1 ) g.projectmodifiedstatic = 1;
					//  update infinilight list with addition
					t.tttentid=t.entityelement[t.te].bankindex;
					if (  t.entityprofile[t.tttentid].ismarker == 2 || t.entityprofile[t.tttentid].ismarker == 5 ) 
					{
						lighting_refresh ( );
					}
				}

				// 201015 - if we need to also move rubber band highlighted objects, do so now
				if (g.entityrubberbandlist.size() > 0)
				{
					float fMovedActiveObjectX = ObjectPositionX(t.widget.activeObject) - fOldActiveObjectX;
					float fMovedActiveObjectY = ObjectPositionY(t.widget.activeObject) - fOldActiveObjectY;
					float fMovedActiveObjectZ = ObjectPositionZ(t.widget.activeObject) - fOldActiveObjectZ;
					for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
					{
						int e = g.entityrubberbandlist[i].e;
						int tobj = t.entityelement[e].obj;
						if (tobj > 0)
						{
							if (ObjectExist(tobj) == 1)
							{
								if (tobj != t.widget.activeObject)
								{
									// reposition this entity
									SlowPositionObject(tobj, ObjectPositionX(tobj) + fMovedActiveObjectX, ObjectPositionY(tobj) + fMovedActiveObjectY, ObjectPositionZ(tobj) + fMovedActiveObjectZ);
									t.entityelement[e].x = ObjectPositionX(tobj);
									t.entityelement[e].y = ObjectPositionY(tobj);
									t.entityelement[e].z = ObjectPositionZ(tobj);
									// mark as static if it was
									if (t.entityelement[e].staticflag == 1) g.projectmodifiedstatic = 1;
									// also update zones if part of group
									widget_movezonesandlights(e);
								}
							}
						}
					}
				}
				#endif
			}

			// only allow rot/scl for non markers
			t.te = t.widget.pickedEntityIndex;
			t.tttentid = t.entityelement[t.te].bankindex;

			// some markers and spotlight have Y rotation
			t.thaveyrot = 0; 
			if ( t.entityprofile[t.tttentid].ismarker == 1 || t.entityprofile[t.tttentid].ismarker == 6 || t.entityprofile[t.tttentid].ismarker == 7  )  t.thaveyrot = 1;
			if ( t.entityelement[t.te].eleprof.usespotlighting ) t.thaveyrot = 1;

			// allow rotation/scale
			if ( (t.entityprofile[t.tttentid].ismarker == 0 || t.thaveyrot == 1) && ObjectExist(t.widget.activeObject) == 1 ) 
			{
				#ifdef WICKEDENGINE
				// rotation
				if ( t.widget.mode == 1 && t.toriginalTranslateClickDeferred <= 1 )//|| g.fForceYRotationOfRubberBandFromKeyPress != 0.0f )
				{
					// calculate movement from start position to now
					float fMoveAngleX = 0.0f;
					float fMoveAngleY = 0.0f;
					float fMoveAngleZ = 0.0f;
					float fPlanePosX, fPlanePosY, fPlanePosZ;
					float fActivePosX = ObjectPositionX(t.widget.activeObject);
					float fActivePosY = ObjectPositionY(t.widget.activeObject);
					float fActivePosZ = ObjectPositionZ(t.widget.activeObject);
					bool bPlanePosRegistered = widget_getplanepos(fActivePosX, fActivePosY, fActivePosZ, &fPlanePosX, &fPlanePosY, &fPlanePosZ);
					if (bPlanePosRegistered == true)
					{
						// offset pos on plane with difference between widget click pos and active object
						fPlanePosX -= fActivePosX;
						fPlanePosY -= fActivePosY;
						fPlanePosZ -= fActivePosZ;

						// based on axis chosen, work out the absolute angle the user has indicated
						if (t.widget.pickedSection == t.widget.widgetXRotObj) fMoveAngleX = -GGToDegree(atan2(fPlanePosY, fPlanePosZ));
						if (t.widget.pickedSection == t.widget.widgetYRotObj) fMoveAngleY = -GGToDegree(atan2(fPlanePosZ, fPlanePosX));
						if (t.widget.pickedSection == t.widget.widgetZRotObj) fMoveAngleZ = GGToDegree(atan2(fPlanePosY, fPlanePosX));

						// initial click or actual adjustment
						if (t.toriginalTranslateClickDeferred == 1)
						{
							// for the initial click, detect the start of the change
							if (t.widget.pickedSection == t.widget.widgetXRotObj) t.toriginalRotationClickX_f = fMoveAngleX;
							if (t.widget.pickedSection == t.widget.widgetYRotObj) t.toriginalRotationClickY_f = fMoveAngleY;
							if (t.widget.pickedSection == t.widget.widgetZRotObj) t.toriginalRotationClickZ_f = fMoveAngleZ;
							fMoveAngleX = 0; fMoveAngleY = 0; fMoveAngleZ = 0;

							// if entity has no quat, create one (leelee, move to own function, may be using this a lot)
							if (t.entityelement[t.te].quatmode == 0)
							{
								// generate quat for first time
								GGQUATERNION QuatAroundX, QuatAroundY, QuatAroundZ;
								GGQuaternionRotationAxis(&QuatAroundX, &GGVECTOR3(1, 0, 0), GGToRadian(ObjectAngleX(t.widget.activeObject)));
								GGQuaternionRotationAxis(&QuatAroundY, &GGVECTOR3(0, 1, 0), GGToRadian(ObjectAngleY(t.widget.activeObject)));
								GGQuaternionRotationAxis(&QuatAroundZ, &GGVECTOR3(0, 0, 1), GGToRadian(ObjectAngleZ(t.widget.activeObject)));
								t.toriginalAngle = QuatAroundX * QuatAroundY * QuatAroundZ;
							}
							else
							{
								// use entities current quat
								t.toriginalAngle = GGQUATERNION(t.entityelement[t.te].quatx, t.entityelement[t.te].quaty, t.entityelement[t.te].quatz, t.entityelement[t.te].quatw);
							}

							// defer detect complete
							t.toriginalTranslateClickDeferred = 0;
						}
						else
						{
							// actual adjustment
							if (t.widget.pickedSection == t.widget.widgetXRotObj) fMoveAngleX -= t.toriginalRotationClickX_f;
							if (t.widget.pickedSection == t.widget.widgetYRotObj) fMoveAngleY -= t.toriginalRotationClickY_f;
							if (t.widget.pickedSection == t.widget.widgetZRotObj) fMoveAngleZ -= t.toriginalRotationClickZ_f;
						}
					}

					// quat rotation event
					GGQUATERNION quatRotationEvent = { 0,0,0,0 };
					if (t.entityprofile[t.tttentid].ragdoll == 1)
					{
						GGQuaternionRotationAxis(&quatRotationEvent, &GGVECTOR3(0, 1, 0), GGToRadian(fMoveAngleY));
					}
					else
					{
						GGQUATERNION QuatAroundX, QuatAroundY, QuatAroundZ;
						GGQuaternionRotationAxis(&QuatAroundX, &GGVECTOR3(1, 0, 0), GGToRadian(fMoveAngleX));
						GGQuaternionRotationAxis(&QuatAroundY, &GGVECTOR3(0, 1, 0), GGToRadian(fMoveAngleY));
						GGQuaternionRotationAxis(&QuatAroundZ, &GGVECTOR3(0, 0, 1), GGToRadian(fMoveAngleZ));
						quatRotationEvent = QuatAroundX * QuatAroundY * QuatAroundZ;
					}

					// apply the rotation event to the angle of the object
					GGQUATERNION quatNewOrientation;
					GGQuaternionMultiply(&quatNewOrientation, &t.toriginalAngle, &quatRotationEvent);

					// rotate this object with final quat and get new entity rotation eulers
					RotateObjectQuat(t.widget.activeObject, quatNewOrientation.x, quatNewOrientation.y, quatNewOrientation.z, quatNewOrientation.w);
					t.entityelement[t.te].rx = ObjectAngleX(t.widget.activeObject);
					t.entityelement[t.te].ry = ObjectAngleY(t.widget.activeObject);
					t.entityelement[t.te].rz = ObjectAngleZ(t.widget.activeObject);

					// update entity quat as the preferred source rotation
					t.entityelement[t.te].quatmode = 1;
					t.entityelement[t.te].quatx = quatNewOrientation.x;
					t.entityelement[t.te].quaty = quatNewOrientation.y;
					t.entityelement[t.te].quatz = quatNewOrientation.z;
					t.entityelement[t.te].quatw = quatNewOrientation.w;

					// mark as static if it was
					if ( t.entityelement[t.te].staticflag == 1 ) g.projectmodifiedstatic = 1;

					// move zones and lights if in group
					widget_movezonesandlights ( t.te );

					// if we need to also rotate rubber band highlighted objects, do so now
					bool bDisableRubberBandMoving = false;
					#ifdef WICKEDENGINE
					extern int current_selected_group;
					extern bool group_editing_on;
					if (current_selected_group >= 0 && group_editing_on)
					{
						bDisableRubberBandMoving = true;
					}
					#endif
					if (!bDisableRubberBandMoving)
					{
						if (g.entityrubberbandlist.size() > 0)
						{
							// rotate all the grouped entities and move around Y axis of widget as pivot (from their starting positions and angles)
							RotateAndMoveRubberBand(t.widget.activeObject, 0, 0, 0, quatRotationEvent);
						}
					}
				}

				// scale
				if ( t.widget.mode == 2 && t.toriginalTranslateClickDeferred <= 1)
				{
					// store old active object scales for rubber band
					float fOldActiveObjectSX = ObjectScaleX ( t.widget.activeObject );
					float fOldActiveObjectSY = ObjectScaleY ( t.widget.activeObject );
					float fOldActiveObjectSZ = ObjectScaleZ ( t.widget.activeObject );
					float fScaleX = ObjectScaleX(t.widget.activeObject);
					float fScaleY = ObjectScaleY(t.widget.activeObject);
					float fScaleZ = ObjectScaleZ(t.widget.activeObject);
					float fPlanePosX, fPlanePosY, fPlanePosZ;
					float fActivePosX = ObjectPositionX(t.widget.activeObject);
					float fActivePosY = ObjectPositionY(t.widget.activeObject);
					float fActivePosZ = ObjectPositionZ(t.widget.activeObject);
					bool bPlanePosRegistered = widget_getplanepos(fActivePosX, fActivePosY, fActivePosZ, &fPlanePosX, &fPlanePosY, &fPlanePosZ );
					if (bPlanePosRegistered == true)
					{
						// offset pos on plane with difference between widget click pos and active object
						fPlanePosX -= fActivePosX;
						fPlanePosY -= fActivePosY;
						fPlanePosZ -= fActivePosZ;

						// if scale mode, additional init stuff when first click
						if (t.toriginalTranslateClickDeferred == 1)
						{
							// record initial click plane pos
							t.toriginalTranslateClickX_f = fPlanePosX;
							t.toriginalTranslateClickY_f = fPlanePosY;
							t.toriginalTranslateClickZ_f = fPlanePosZ;
							
							// record original scale of active object
							t.toriginalScalingX_f = ObjectScaleX(t.widget.activeObject);
							t.toriginalScalingY_f = ObjectScaleY(t.widget.activeObject);
							t.toriginalScalingZ_f = ObjectScaleZ(t.widget.activeObject);

							// deferred detect complete for rotation
							t.toriginalTranslateClickDeferred = 0;
						}
						
						// ensure the scaling modified latest current scale from moment click the scale widget gadget
						fPlanePosX -= t.toriginalTranslateClickX_f;
						fPlanePosY -= t.toriginalTranslateClickY_f;
						fPlanePosZ -= t.toriginalTranslateClickZ_f;

						GGQUATERNION quatRot = GGQUATERNION(t.entityelement[t.te].quatx, t.entityelement[t.te].quaty, t.entityelement[t.te].quatz, t.entityelement[t.te].quatw);
						GGMATRIX matRot;
						GGMatrixRotationQuaternion(&matRot, &quatRot);
						GGVECTOR3 vecDir;
						if (t.widget.pickedSection == t.widget.widgetXScaleObj) vecDir = GGVECTOR3(1, 0, 0);
						if (t.widget.pickedSection == t.widget.widgetYScaleObj) vecDir = GGVECTOR3(0, 1, 0);
						if (t.widget.pickedSection == t.widget.widgetZScaleObj) vecDir = GGVECTOR3(0, 0, 1);
						if (t.widget.pickedSection == t.widget.widgetXYZScaleObj) vecDir = GGVECTOR3(0, 1, 0);
						GGVec3TransformCoord(&vecDir, &vecDir, &matRot);
						GGVECTOR3 vecPlanePos = GGVECTOR3(fPlanePosX, fPlanePosY, fPlanePosZ);
						float fShiftDelta = GGVec4Dot(&vecDir, &vecPlanePos);
						if (t.widget.pickedSection == t.widget.widgetXScaleObj) fScaleX = t.toriginalScalingX_f + fShiftDelta;
						if (t.widget.pickedSection == t.widget.widgetYScaleObj) fScaleY = t.toriginalScalingY_f + fShiftDelta;
						if (t.widget.pickedSection == t.widget.widgetZScaleObj) fScaleZ = t.toriginalScalingZ_f + fShiftDelta;
						if (t.widget.pickedSection == t.widget.widgetXYZScaleObj)
						{
							fScaleX = t.toriginalScalingX_f + fShiftDelta;
							fScaleY = t.toriginalScalingY_f + fShiftDelta;
							fScaleZ = t.toriginalScalingZ_f + fShiftDelta;
						}
					}

					// scale object and store in entity element item
					ScaleObject ( t.widget.activeObject, fScaleX, fScaleY, fScaleZ );
					t.entityelement[t.te].scalex = ObjectScaleX(t.widget.activeObject)-100.0;
					t.entityelement[t.te].scaley = ObjectScaleY(t.widget.activeObject)-100.0;
					t.entityelement[t.te].scalez = ObjectScaleZ(t.widget.activeObject)-100.0;

					// mark as static if it was
					if ( t.entityelement[t.te].staticflag == 1 ) g.projectmodifiedstatic = 1;

					// if we need to also scale rubber band highlighted objects, do so now
					bool bDisableRubberBandMoving = false;
					#ifdef WICKEDENGINE
					extern int current_selected_group;
					extern bool group_editing_on;
					if (current_selected_group >= 0 && group_editing_on)
					{
						bDisableRubberBandMoving = true;
					}
					#endif
					if (!bDisableRubberBandMoving)
					{

						if (g.entityrubberbandlist.size() > 0)
						{
							float fMovedActiveObjectSX = ObjectScaleX(t.widget.activeObject) - fOldActiveObjectSX;
							float fMovedActiveObjectSY = ObjectScaleY(t.widget.activeObject) - fOldActiveObjectSY;
							float fMovedActiveObjectSZ = ObjectScaleZ(t.widget.activeObject) - fOldActiveObjectSZ;
							for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
							{
								int e = g.entityrubberbandlist[i].e;
								int tobj = t.entityelement[e].obj;
								if (tobj > 0 && t.entityelement[e].editorlock == 0)
								{
									if (ObjectExist(tobj) == 1)
									{
										if (tobj != t.widget.activeObject)
										{
											ScaleObject(tobj, ObjectScaleX(tobj) + fMovedActiveObjectSX, ObjectScaleY(tobj) + fMovedActiveObjectSY, ObjectScaleZ(tobj) + fMovedActiveObjectSZ);
											t.entityelement[e].scalex = ObjectScaleX(tobj) - 100;
											t.entityelement[e].scaley = ObjectScaleY(tobj) - 100;
											t.entityelement[e].scalez = ObjectScaleZ(tobj) - 100;
											if (t.entityelement[e].staticflag == 1) g.projectmodifiedstatic = 1;
										}
									}
								}
							}
						}
					}
				}
				#else
				if ( t.widget.mode ==  1 ) //|| g.fForceYRotationOfRubberBandFromKeyPress != 0.0f
				{
					// 271015 - store old active object angles
					float fOldActiveObjectRX = ObjectAngleX ( t.widget.activeObject );
					float fOldActiveObjectRY = ObjectAngleY ( t.widget.activeObject );
					float fOldActiveObjectRZ = ObjectAngleZ ( t.widget.activeObject );

					//  RotateObject (  on all three axis )
					PickScreen2D23D (  t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,50000 );

					//  control distant mouse pointer object
					t.p0x_f=CameraPositionX();
					t.p0y_f=CameraPositionY();
					t.p0z_f=CameraPositionZ();
					t.px_f=t.p0x_f+GetPickVectorX();
					t.py_f=t.p0y_f+GetPickVectorY();
					t.pz_f=t.p0z_f+GetPickVectorZ();
					PositionObject (  t.twidgetRotStartObject+11,t.px_f,t.py_f,t.pz_f );
					t.p1x_f=ObjectPositionX(t.twidgetRotStartObject+11);
					t.p1y_f=ObjectPositionY(t.twidgetRotStartObject+11);
					t.p1z_f=ObjectPositionZ(t.twidgetRotStartObject+11);

					//PE: At some angles InterSectObject fails, so make sure we have valid values before processing.
					//PE: This prevent suttenly jumps where first click values is not valid.
					//PE: This could be improved more, so we never had invalid data. but fine for now.
					bool bValidRotation = false;

					//  handle rotation modes
					if (  t.widget.pickedSection  ==  t.widget.widgetXRotObj && (t.thaveyrot == 0 || t.entityelement[t.te].eleprof.usespotlighting ) )
					{
						if (  ObjectExist(t.twidgetRotStartObject+14) == 1  )  DeleteObject (  t.twidgetRotStartObject+14 );
						MakeObjectBox (  t.twidgetRotStartObject+14,1,5000000,5000000 );
						HideObject (  t.twidgetRotStartObject+14 );
						PositionObject (  t.twidgetRotStartObject+14,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
						RotateObject (  t.twidgetRotStartObject+14,ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject),ObjectAngleZ(t.widget.activeObject) );
						if (  IntersectObject(t.twidgetRotStartObject+14,t.p0x_f,t.p0y_f,t.p0z_f,t.p1x_f,t.p1y_f,t.p1z_f) != 0 ) 
						{
							t.pinterx_f=ChecklistFValueA(6); //PE: MegaCollisionFeedback.vecHitPoint.x
							t.pintery_f=ChecklistFValueB(6);
							t.pinterz_f=ChecklistFValueC(6);
							bValidRotation = true;
						}
						PositionObject (  t.twidgetRotStartObject+13,t.pinterx_f,t.pintery_f,t.pinterz_f );
					}
					if (  t.widget.pickedSection  ==  t.widget.widgetYRotObj ) 
					{
						if (  ObjectExist(t.twidgetRotStartObject+14) == 1  )  DeleteObject (  t.twidgetRotStartObject+14 );
						MakeObjectBox (  t.twidgetRotStartObject+14,5000000,1,5000000 );
						HideObject (  t.twidgetRotStartObject+14 );
						PositionObject (  t.twidgetRotStartObject+14,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
						RotateObject (  t.twidgetRotStartObject+14,ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject),ObjectAngleZ(t.widget.activeObject) );
						if (  IntersectObject(t.twidgetRotStartObject+14,t.p0x_f,t.p0y_f,t.p0z_f,t.p1x_f,t.p1y_f,t.p1z_f) != 0 ) 
						{
							t.pinterx_f=ChecklistFValueA(6);
							t.pintery_f=ChecklistFValueB(6);
							t.pinterz_f=ChecklistFValueC(6);
							bValidRotation = true;

						}
						PositionObject (  t.twidgetRotStartObject+13,t.pinterx_f,t.pintery_f,t.pinterz_f );
					}
					if (  t.widget.pickedSection  ==  t.widget.widgetZRotObj && ( t.thaveyrot == 0|| t.entityelement[t.te].eleprof.usespotlighting) )
					{
						if (  ObjectExist(t.twidgetRotStartObject+14) == 1  )  DeleteObject (  t.twidgetRotStartObject+14 );
						MakeObjectBox (  t.twidgetRotStartObject+14,5000000,5000000,1 );
						HideObject (  t.twidgetRotStartObject+14 );
						PositionObject (  t.twidgetRotStartObject+14,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
						RotateObject (  t.twidgetRotStartObject+14,ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject),ObjectAngleZ(t.widget.activeObject) );
						if (  IntersectObject(t.twidgetRotStartObject+14,t.p0x_f,t.p0y_f,t.p0z_f,t.p1x_f,t.p1y_f,t.p1z_f) != 0 ) 
						{
							t.pinterx_f=ChecklistFValueA(6);
							t.pintery_f=ChecklistFValueB(6);
							t.pinterz_f=ChecklistFValueC(6);
							bValidRotation = true;

						}
						PositionObject (  t.twidgetRotStartObject+13,t.pinterx_f,t.pintery_f,t.pinterz_f );
					}

					//  control rotations
					if (bValidRotation && t.widget.grabbed == 0 )
					{
						//  start drag
						t.gmx=t.widgetinputsysxmouse_f;
						t.gmy=t.widgetinputsysymouse_f;
						PositionObject (  t.twidgetRotStartObject+12,t.pinterx_f,t.pintery_f,t.pinterz_f );
						t.fAngleStoreX = ObjectAngleX ( t.widget.activeObject );
						t.fAngleStoreY = ObjectAngleY ( t.widget.activeObject );
						t.fAngleStoreZ = ObjectAngleZ ( t.widget.activeObject );
						t.widget.grabbed=1;
					}
					if (bValidRotation && t.widget.grabbed == 1 )
					{
						SetIdentityMatrix (  g.widgetStartMatrix+3 );
						RotateXMatrix (  g.widgetStartMatrix+4,ObjectAngleX(t.widget.activeObject)*0.017444 );
						MultiplyMatrix (  g.widgetStartMatrix+3,g.widgetStartMatrix+3,g.widgetStartMatrix+4 );
						RotateYMatrix (  g.widgetStartMatrix+4,ObjectAngleY(t.widget.activeObject)*0.017444 );
						MultiplyMatrix (  g.widgetStartMatrix+3,g.widgetStartMatrix+3,g.widgetStartMatrix+4 );
						RotateZMatrix (  g.widgetStartMatrix+4,ObjectAngleZ(t.widget.activeObject)*0.017444 );
						MultiplyMatrix (  g.widgetStartMatrix+3,g.widgetStartMatrix+3,g.widgetStartMatrix+4 );
						t.widget.grabbed=2;
					}
					if ( (bValidRotation && t.widget.grabbed == 2)  ) //|| g.fForceYRotationOfRubberBandFromKeyPress != 0.0f
					{
//						if ( g.fForceYRotationOfRubberBandFromKeyPress != 0.0f )
//						{
//							// comes from pressing the R key (and 3 and 4 key)
//							t.tanglediff_f = g.fForceYRotationOfRubberBandFromKeyPress;
//						}
//						else
						{
							//  dragging in world space
							//  x-axis oriented by object
							if (  t.widget.pickedSection == t.widget.widgetXRotObj  )  SetVector3 (  g.widgetvectorindex+1,1,0,0 );
							if (  t.widget.pickedSection == t.widget.widgetYRotObj  )  SetVector3 (  g.widgetvectorindex+1,0,1,0 );
							if (  t.widget.pickedSection == t.widget.widgetZRotObj  )  SetVector3 (  g.widgetvectorindex+1,0,0,1 );
							TransformVectorCoordinates3 (  g.widgetvectorindex+1,g.widgetvectorindex+1,g.widgetStartMatrix+3 );
							//  work out grab locations
							SetVector3 (  g.widgetvectorindex+7,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
							SetVector3 (  g.widgetvectorindex+8,ObjectPositionX(t.twidgetRotStartObject+12),ObjectPositionY(t.twidgetRotStartObject+12),ObjectPositionZ(t.twidgetRotStartObject+12) );
							SetVector3 (  g.widgetvectorindex+9,ObjectPositionX(t.twidgetRotStartObject+13),ObjectPositionY(t.twidgetRotStartObject+13),ObjectPositionZ(t.twidgetRotStartObject+13) );
							SubtractVector3 (  g.widgetvectorindex+8,g.widgetvectorindex+8,g.widgetvectorindex+7 );
							SubtractVector3 (  g.widgetvectorindex+9,g.widgetvectorindex+9,g.widgetvectorindex+7 );
							SetVector3 (  g.widgetvectorindex+7,0,0,0 );
							if (  t.tanglediff_f>360  )  t.tanglediff_f = t.tanglediff_f-360;
							if (  t.tanglediff_f<-360  )  t.tanglediff_f = t.tanglediff_f+360;
							//  transform vector 8 and 9 around new UP vector
							SetVector3 (  g.widgetvectorindex+12,0,1,0 );
							CopyMatrix (  g.widgetStartMatrix+4,g.widgetStartMatrix+3 );
							t.r=InverseMatrix(g.widgetStartMatrix+4,g.widgetStartMatrix+4);
							TransformVectorCoordinates3 (  g.widgetvectorindex+8,g.widgetvectorindex+8,g.widgetStartMatrix+4 );
							TransformVectorCoordinates3 (  g.widgetvectorindex+9,g.widgetvectorindex+9,g.widgetStartMatrix+4 );
							if (  t.widget.pickedSection == t.widget.widgetXRotObj ) 
							{
								t.tangle1_f=atan2deg(GetZVector3(g.widgetvectorindex+8),GetYVector3(g.widgetvectorindex+8)) ;
							}
							if (  t.widget.pickedSection == t.widget.widgetYRotObj ) 
							{
								t.tangle1_f=atan2deg(GetXVector3(g.widgetvectorindex+8),GetZVector3(g.widgetvectorindex+8));
							}
							if (  t.widget.pickedSection == t.widget.widgetZRotObj ) 
							{
								t.tangle1_f=atan2deg(GetYVector3(g.widgetvectorindex+8),GetXVector3(g.widgetvectorindex+8)) ;
							}
							if (  t.widget.pickedSection == t.widget.widgetXRotObj ) 
							{
								t.tangle2_f=atan2deg(GetZVector3(g.widgetvectorindex+9),GetYVector3(g.widgetvectorindex+9));
							}
							if (  t.widget.pickedSection == t.widget.widgetYRotObj ) 
							{
								t.tangle2_f=atan2deg(GetXVector3(g.widgetvectorindex+9),GetZVector3(g.widgetvectorindex+9));
							}
							if (  t.widget.pickedSection == t.widget.widgetZRotObj ) 
							{
								t.tangle2_f=atan2deg(GetYVector3(g.widgetvectorindex+9),GetXVector3(g.widgetvectorindex+9)) ;
							}
							t.tanglediff_f=t.tangle2_f-t.tangle1_f;
						}

						// if group rotation, use simpler Y only rotation
						if ( g.entityrubberbandlist.size() > 0 )
						{
							// avoids messy math for now
							t.pVecAnglesx_f = t.fAngleStoreX;
							t.pVecAnglesy_f = t.fAngleStoreY + t.tanglediff_f;
							t.pVecAnglesz_f = t.fAngleStoreZ;
						}
						else
						{
							//  apply rotation
							BuildRotationAxisMatrix (  g.widgetStartMatrix+4,g.widgetvectorindex+1,t.tanglediff_f*0.017444 ) ;
							MultiplyMatrix (  g.widgetStartMatrix+5,g.widgetStartMatrix+3,g.widgetStartMatrix+4 );
							//  convert matrix to euler angles
							t.m00_f = GetMatrixElement(g.widgetStartMatrix+5,0);
							t.m01_f = GetMatrixElement(g.widgetStartMatrix+5,1);
							t.m02_f = GetMatrixElement(g.widgetStartMatrix+5,2);
							t.m12_f = GetMatrixElement(g.widgetStartMatrix+5,6);
							t.m22_f = GetMatrixElement(g.widgetStartMatrix+5,10);
							t.heading_f = atan2deg(t.m01_f,t.m00_f);
							t.attitude_f = atan2deg(t.m12_f,t.m22_f);
							t.bank_f = Asin(-t.m02_f);
							if (  abs ( t.m02_f ) > 1.0 ) 
							{
								t.PI_f = 3.14159265f / 2.0f;
								t.pVecAnglesx_f = 0.0;
								t.pVecAnglesy_f = ( t.PI_f * t.m02_f ) / 0.017444;
								t.pVecAnglesz_f = 0.0;
							}
							else
							{
								t.pVecAnglesx_f = ( t.attitude_f );
								t.pVecAnglesy_f = ( t.bank_f );
								t.pVecAnglesz_f = ( t.heading_f );
							}
						}
						//  update object with regular euler
						RotateObject ( t.widget.activeObject,t.pVecAnglesx_f,t.pVecAnglesy_f,t.pVecAnglesz_f );

//						if (g.fForceYRotationOfRubberBandFromKeyPress != 0.0f)
//						{
//							//PE: From "R" key, also update t.fAngleStoreY.
//							t.fAngleStoreX = ObjectAngleX(t.widget.activeObject);
//							t.fAngleStoreY = ObjectAngleY(t.widget.activeObject);
//							t.fAngleStoreZ = ObjectAngleZ(t.widget.activeObject);
//						}
					}
					//  transfer final eulers to entity element setting
					if (  t.entityprofile[t.tttentid].ragdoll == 1 ) 
					{
						t.entityelement[t.te].rx=0;
						t.entityelement[t.te].ry=t.tlastgoody_f+t.tanglediff_f;
						t.entityelement[t.te].rz=0;
						RotateObject (  t.widget.activeObject,0,t.entityelement[t.te].ry,0 );
					}
					else
					{
						t.entityelement[t.te].rx=ObjectAngleX(t.widget.activeObject);
						t.entityelement[t.te].ry=ObjectAngleY(t.widget.activeObject);
						t.entityelement[t.te].rz=ObjectAngleZ(t.widget.activeObject);
					}
					// mark as static if it was
					if ( t.entityelement[t.te].staticflag == 1 ) g.projectmodifiedstatic = 1;

					// 271015 - if we need to also rotate rubber band highlighted objects, do so now
					if ( g.entityrubberbandlist.size() > 0 )
					{
						// rotate all the grouped entities and move around Y axis of widget as pivot
						float fMovedActiveObjectRX = ObjectAngleX ( t.widget.activeObject ) - fOldActiveObjectRX;
						float fMovedActiveObjectRY = ObjectAngleY ( t.widget.activeObject ) - fOldActiveObjectRY;
						float fMovedActiveObjectRZ = ObjectAngleZ ( t.widget.activeObject ) - fOldActiveObjectRZ;
						for ( int i = 0; i < (int)g.entityrubberbandlist.size(); i++ )
						{
							int e = g.entityrubberbandlist[i].e;
							int tobj = t.entityelement[e].obj;
							if ( tobj > 0 )
							{
								if ( ObjectExist(tobj) == 1 )
								{
									if ( tobj != t.widget.activeObject )
									{
										// 061115 - a fix for entities that have been inverted
										if ( t.entityelement[e].rx==180 && t.entityelement[e].rz==180 )
										{
											t.entityelement[e].rx = 0;
											t.entityelement[e].ry = t.entityelement[e].ry;
											t.entityelement[e].rz = 0;
											RotateObject ( tobj, t.entityelement[e].rx, t.entityelement[e].ry, t.entityelement[e].rz );
										}

										// rotate the entity
										RotateObject ( tobj, ObjectAngleX(tobj)+fMovedActiveObjectRX, ObjectAngleY(tobj)+fMovedActiveObjectRY, ObjectAngleZ(tobj)+fMovedActiveObjectRZ );
										t.entityelement[e].rx = ObjectAngleX(tobj);
										t.entityelement[e].ry = ObjectAngleY(tobj);
										t.entityelement[e].rz = ObjectAngleZ(tobj);
										// mark as static if it was
										if ( t.entityelement[e].staticflag == 1 ) g.projectmodifiedstatic = 1;

										// move the entity around a pivot point
										GGVECTOR3 VecPos;
										VecPos.x = g.entityrubberbandlist[i].x;
										VecPos.y = g.entityrubberbandlist[i].y;
										VecPos.z = g.entityrubberbandlist[i].z;
										sObject* pObject = GetObjectData(tobj);
										GGVec3TransformCoord ( &VecPos, &VecPos, &pObject->position.matObjectNoTran );
										t.entityelement[e].x = ObjectPositionX ( t.widget.activeObject ) + VecPos.x;
										t.entityelement[e].y = ObjectPositionY ( t.widget.activeObject ) + VecPos.y;
										t.entityelement[e].z = ObjectPositionZ ( t.widget.activeObject ) + VecPos.z;
										PositionObject ( tobj, t.entityelement[e].x, t.entityelement[e].y, t.entityelement[e].z );

										// move zones and lights if in group
										widget_movezonesandlights ( e );
									}
								}
							}
						}
					}
				}

				// scale
				if ( t.entityprofile[t.tttentid].ragdoll == 0 && t.thaveyrot == 0 ) 
				{
					// only if NOT using ragdoll
					if ( t.widget.mode  ==  2 ) 
					{
						// 271015 - store old active object scales
						float fOldActiveObjectSX = ObjectScaleX ( t.widget.activeObject );
						float fOldActiveObjectSY = ObjectScaleY ( t.widget.activeObject );
						float fOldActiveObjectSZ = ObjectScaleZ ( t.widget.activeObject );

						// 281015 - take scale of object inti account
						float fEntityTotalSize = ObjectSize ( t.widget.activeObject, 0 );
						float fEntityScalingRatio = 100.0f / fEntityTotalSize;

						// scale on X, Y and Z axis
						if (  t.widget.pickedSection  ==  t.widget.widgetXScaleObj ) 
						{
							ShowObject (  t.widget.widgetPlaneObj );
							PositionObject (  t.widget.widgetPlaneObj,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
							RotateObject (  t.widget.widgetPlaneObj,-90,0,0 );
							//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
							if (1) //(t.a == 0) //PE: Fixed
							{
								t.ttdx_f = CameraPositionX() - ObjectPositionX(t.widget.activeObject);
								t.ttdy_f = CameraPositionY() - ObjectPositionY(t.widget.activeObject);
								t.ttdz_f = CameraPositionZ() - ObjectPositionZ(t.widget.activeObject);
								t.ttdist_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdy_f*t.ttdy_f) + abs(t.ttdz_f*t.ttdz_f));
								PickScreen2D23D(t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.ttdist_f);
							}
							HideObject (  t.widget.widgetPlaneObj );
							t.tNewClickX_f = CameraPositionX() + GetPickVectorX();
							t.tNewClickY_f = CameraPositionY() + GetPickVectorY();
							t.tNewClickZ_f = CameraPositionZ() + GetPickVectorZ();
							GGVECTOR3 vecOrig = GGVECTOR3(t.toriginalClick2X_f,t.toriginalClick2Y_f,t.toriginalClick2Z_f);
							GGVECTOR3 vecNew = GGVECTOR3(t.tNewClickX_f,t.tNewClickY_f,t.tNewClickZ_f);
							sObject* pObject = GetObjectData ( t.widget.activeObject );
							float pDet;
							GGMATRIX matInverse;
							GGMatrixInverse ( &matInverse, &pDet, &pObject->position.matRotation );
							vecNew = vecNew - vecOrig;
							GGVec3TransformCoord ( &vecNew, &vecNew, &matInverse );
							t.tnewDistance_f = vecNew.x;
							t.tfactor_f=(t.toriginalScaleX_f/100.0)*2;
							if (  t.tfactor_f<1.0  )  t.tfactor_f = 1.0;
							if (  t.tfactor_f>5.0  )  t.tfactor_f = 5.0;
							t.tscale_f = (t.tnewDistance_f*t.tfactor_f*fEntityScalingRatio)+t.toriginalScaleX_f;
							if (  t.tscale_f < 5  )  t.tscale_f  =  5;
							if (  t.tscale_f > 10000  )  t.tscale_f  =  10000;
							ScaleObject (  t.widget.activeObject,t.tscale_f,ObjectScaleY(t.widget.activeObject),ObjectScaleZ(t.widget.activeObject) );
						}
						if (  t.widget.pickedSection  ==  t.widget.widgetYScaleObj ) 
						{
							ShowObject (  t.widget.widgetPlaneObj );
							PositionObject (  t.widget.widgetPlaneObj,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
							RotateObject (  t.widget.widgetPlaneObj,0,0,0 );
							//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
							if (1) //(t.a == 0) //PE: Fixed
							{
								t.ttdx_f = CameraPositionX() - ObjectPositionX(t.widget.activeObject);
								t.ttdy_f = CameraPositionY() - ObjectPositionY(t.widget.activeObject);
								t.ttdz_f = CameraPositionZ() - ObjectPositionZ(t.widget.activeObject);
								t.ttdist_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdy_f*t.ttdy_f) + abs(t.ttdz_f*t.ttdz_f));
								PickScreen2D23D(t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.ttdist_f);
							}
							HideObject (  t.widget.widgetPlaneObj );
							t.tNewClickX_f = CameraPositionX() + GetPickVectorX();
							t.tNewClickY_f = CameraPositionY() + GetPickVectorY();
							t.tNewClickZ_f = CameraPositionZ() + GetPickVectorZ();
							GGVECTOR3 vecOrig = GGVECTOR3(t.toriginalClick2X_f,t.toriginalClick2Y_f,t.toriginalClick2Z_f);
							GGVECTOR3 vecNew = GGVECTOR3(t.tNewClickX_f,t.tNewClickY_f,t.tNewClickZ_f);
							sObject* pObject = GetObjectData ( t.widget.activeObject );
							float pDet;
							GGMATRIX matInverse;
							GGMatrixInverse ( &matInverse, &pDet, &pObject->position.matRotation );
							vecNew = vecNew - vecOrig;
							GGVec3TransformCoord ( &vecNew, &vecNew, &matInverse );
							t.tnewDistance_f = vecNew.y;
							t.tfactor_f=(t.toriginalScaleY_f/100.0)*2;
							if (  t.tfactor_f<1.0  )  t.tfactor_f = 1.0;
							if (  t.tfactor_f>5.0  )  t.tfactor_f = 5.0;
							t.tscale_f = ((t.tnewDistance_f)*t.tfactor_f*fEntityScalingRatio)+t.toriginalScaleY_f;
							if (  t.tscale_f < 5  )  t.tscale_f  =  5;
							if (  t.tscale_f > 10000  )  t.tscale_f  =  10000;
							ScaleObject (  t.widget.activeObject,ObjectScaleX(t.widget.activeObject),t.tscale_f,ObjectScaleZ(t.widget.activeObject) );
						}
						if (  t.widget.pickedSection  ==  t.widget.widgetZScaleObj ) 
						{
							ShowObject (  t.widget.widgetPlaneObj );
							PositionObject (  t.widget.widgetPlaneObj,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
							RotateObject (  t.widget.widgetPlaneObj,-90,0,0 );
							//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj) ;
							if (1) //(t.a == 0) //PE: Fixed
							{
								t.ttdx_f = CameraPositionX() - ObjectPositionX(t.widget.activeObject);
								t.ttdy_f = CameraPositionY() - ObjectPositionY(t.widget.activeObject);
								t.ttdz_f = CameraPositionZ() - ObjectPositionZ(t.widget.activeObject);
								t.ttdist_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdy_f*t.ttdy_f) + abs(t.ttdz_f*t.ttdz_f));
								PickScreen2D23D(t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.ttdist_f);
							}
							HideObject (  t.widget.widgetPlaneObj );
							t.tNewClickX_f = CameraPositionX() + GetPickVectorX();
							t.tNewClickY_f = CameraPositionY() + GetPickVectorY();
							t.tNewClickZ_f = CameraPositionZ() + GetPickVectorZ();
							GGVECTOR3 vecOrig = GGVECTOR3(t.toriginalClick2X_f,t.toriginalClick2Y_f,t.toriginalClick2Z_f);
							GGVECTOR3 vecNew = GGVECTOR3(t.tNewClickX_f,t.tNewClickY_f,t.tNewClickZ_f);
							sObject* pObject = GetObjectData ( t.widget.activeObject );
							float pDet;
							GGMATRIX matInverse;
							GGMatrixInverse ( &matInverse, &pDet, &pObject->position.matRotation );
							vecNew = vecNew - vecOrig;
							GGVec3TransformCoord ( &vecNew, &vecNew, &matInverse );
							t.tnewDistance_f = vecNew.z;
							t.tfactor_f=(t.toriginalScaleZ_f/100.0)*2;
							if (  t.tfactor_f<1.0  )  t.tfactor_f = 1.0;
							if (  t.tfactor_f>5.0  )  t.tfactor_f = 5.0;
							t.tscale_f = ((t.tnewDistance_f)*t.tfactor_f*fEntityScalingRatio)+t.toriginalScaleZ_f;
							if (  t.tscale_f < 5  )  t.tscale_f  =  5;
							if (  t.tscale_f > 10000  )  t.tscale_f  =  10000;
							ScaleObject (  t.widget.activeObject,ObjectScaleX(t.widget.activeObject),ObjectScaleY(t.widget.activeObject),t.tscale_f );
						}

						// scale on XYZ together
						if (  t.widget.pickedSection  ==  t.widget.widgetXYZScaleObj ) 
						{
							//PE: Fixed.
							ShowObject (  t.widget.widgetPlaneObj );
							PositionObject (  t.widget.widgetPlaneObj,ObjectPositionX(t.widget.activeObject),ObjectPositionY(t.widget.activeObject),ObjectPositionZ(t.widget.activeObject) );
							RotateObject (  t.widget.widgetPlaneObj,-90,0,0 );
							//t.a=PickScreenObject (t.widgetinputsysxmouse_f,t.widgetinputsysymouse_f,t.widget.widgetPlaneObj,t.widget.widgetPlaneObj);
							if (1) //(t.a == 0)
							{
								t.ttdx_f = CameraPositionX() - ObjectPositionX(t.widget.activeObject);
								t.ttdy_f = CameraPositionY() - ObjectPositionY(t.widget.activeObject);
								t.ttdz_f = CameraPositionZ() - ObjectPositionZ(t.widget.activeObject);
								t.ttdist_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdy_f*t.ttdy_f) + abs(t.ttdz_f*t.ttdz_f));
								PickScreen2D23D(t.widgetinputsysxmouse_f, t.widgetinputsysymouse_f, t.ttdist_f);
							}
							HideObject (  t.widget.widgetPlaneObj );
							t.tNewClickX_f = CameraPositionX() + GetPickVectorX();
							t.tNewClickY_f = CameraPositionY() + GetPickVectorY();
							t.tNewClickZ_f = CameraPositionZ() + GetPickVectorZ();
							t.tdx_f = t.tNewClickX_f - ObjectPositionX(t.widget.activeObject);
							t.tdy_f = t.tNewClickY_f - ObjectPositionY(t.widget.activeObject);
							t.tdz_f = t.tNewClickZ_f - ObjectPositionZ(t.widget.activeObject);
							t.tnewDistance_f = Sqrt(t.tdx_f*t.tdx_f + t.tdy_f*t.tdy_f + t.tdz_f*t.tdz_f);
							t.tfactor_f=(t.toriginalScaleX_f/100.0)*2;
							if (  t.tfactor_f<1.0  )  t.tfactor_f = 1.0;
							if (  t.tfactor_f>5.0  )  t.tfactor_f = 5.0;
							t.tscale_f = ((t.tnewDistance_f-t.toriginalDistance_f)*t.tfactor_f*fEntityScalingRatio)+t.toriginalScaleX_f;
							t.tScaleY_f = (t.toriginalScaleY_f / t.toriginalScaleX_f) * t.tscale_f;
							t.tScaleZ_f = (t.toriginalScaleZ_f / t.toriginalScaleX_f) * t.tscale_f;
							if (  t.tscale_f < 5  )  t.tscale_f  =  5;
							if (  t.tscale_f > 10000  )  t.tscale_f  =  10000;
							if (  t.tScaleY_f < 5  )  t.tScaleY_f  =  5;
							if (  t.tScaleY_f > 10000  )  t.tScaleY_f  =  10000;
							if (  t.tScaleZ_f < 5  )  t.tScaleZ_f  =  5;
							if (  t.tScaleZ_f > 10000  )  t.tScaleZ_f  =  10000;
							ScaleObject (  t.widget.activeObject,t.tscale_f,t.tScaleY_f,t.tScaleZ_f );
						}

						//  transfer final scale to entity element setting
						t.entityelement[t.te].scalex = ObjectScaleX(t.widget.activeObject)-100.0;
						t.entityelement[t.te].scaley = ObjectScaleY(t.widget.activeObject)-100.0;
						t.entityelement[t.te].scalez = ObjectScaleZ(t.widget.activeObject)-100.0;
						// mark as static if it was
						if ( t.entityelement[t.te].staticflag == 1 ) g.projectmodifiedstatic = 1;

						// 271015 - if we need to also scale rubber band highlighted objects, do so now
						if ( g.entityrubberbandlist.size() > 0 )
						{
							float fMovedActiveObjectSX = ObjectScaleX ( t.widget.activeObject ) - fOldActiveObjectSX;
							float fMovedActiveObjectSY = ObjectScaleY ( t.widget.activeObject ) - fOldActiveObjectSY;
							float fMovedActiveObjectSZ = ObjectScaleZ ( t.widget.activeObject ) - fOldActiveObjectSZ;
							for ( int i = 0; i < (int)g.entityrubberbandlist.size(); i++ )
							{
								int e = g.entityrubberbandlist[i].e;
								int tobj = t.entityelement[e].obj;
								if ( tobj > 0 )
								{
									if ( ObjectExist(tobj) == 1 )
									{
										if ( tobj != t.widget.activeObject )
										{
											ScaleObject ( tobj, ObjectScaleX(tobj)+fMovedActiveObjectSX, ObjectScaleY(tobj)+fMovedActiveObjectSY, ObjectScaleZ(tobj)+fMovedActiveObjectSZ );
											t.entityelement[e].scalex = ObjectScaleX(tobj) - 100;
											t.entityelement[e].scaley = ObjectScaleY(tobj) - 100;
											t.entityelement[e].scalez = ObjectScaleZ(tobj) - 100;
											// mark as static if it was
											if ( t.entityelement[e].staticflag == 1 ) g.projectmodifiedstatic = 1;
										}
									}
								}
							}
						}
					}
				}
				#endif				
			}

			//  detect if widget panel off screen, and shift back in
			widget_correctwidgetpanel ( );

			// user has let go of the mouse, reset section chosen
			if ( t.inputsys.mclick == 0 ) 
			{
				t.widget.pickedSection = 0;
				t.widget.grabbed = 0;

				#ifdef WICKEDENGINE
				//PE: If we moved a light, also update probes.
				if (t.widget.pickedEntityIndex > 0)
				{
					int entid = t.entityelement[t.widget.pickedEntityIndex].bankindex;
					if (entid > 0)
					{
						if (t.entityprofile[entid].ismarker == 2)
						{
							float fLightProbeScale = t.entityelement[t.widget.pickedEntityIndex].eleprof.light.fLightHasProbe;
							if ( fLightProbeScale > 0 )
								entity_placeprobe(t.entityelement[t.widget.pickedEntityIndex].obj, fLightProbeScale);
							else
								entity_deleteprobe(t.entityelement[t.widget.pickedEntityIndex].obj);
						}
					}
				}

				// and finally trigger UI to update its euler values to sync with widget rotation!
				g_bRefreshRotationValuesFromObjectOnce = true;
				#endif
			}
		}

		// update waypoint object when widget moves entity
		t.ttte=t.widget.pickedEntityIndex;
		if ( t.ttte>0 ) 
		{
			t.waypointindex=t.entityelement[t.ttte].eleprof.trigger.waypointzoneindex;
			if ( t.waypointindex>0 ) 
			{
				t.thisx_f=t.entityelement[t.ttte].x;
				t.thisy_f=t.entityelement[t.ttte].y;
				t.thisz_f=t.entityelement[t.ttte].z;
				waypoint_movetothiscoordinate ( );
			}
		}
	}
	t.widget.oldMouseClick = t.inputsys.mclick;

	//  restore camera range
	editor_refreshcamerarange ( );

	// 061115 - ensure the R key press is cancelled
	//g.fForceYRotationOfRubberBandFromKeyPress = 0.0f;
}

void widget_correctwidgetpanel ( void )
{
	// reverse widget when facing away from user
	if ( WrapValue(CameraAngleY())>180 || WrapValue(CameraAngleY())<1  ) t.txflip = 0; else t.txflip = 1;
	if ( t.txflip == 1 ) 
	{
		YRotateObject ( t.widget.widgetXObj, 180 );
		YRotateObject ( t.widget.widgetXYObj, 180 );
	}
	else
	{
		YRotateObject ( t.widget.widgetXObj, 0 );
		YRotateObject ( t.widget.widgetXYObj, 0 );
	}
	if ( WrapValue(CameraAngleY())>270 || WrapValue(CameraAngleY())<90 ) 
	{
		YRotateObject ( t.widget.widgetZObj, 180 );
		if ( t.txflip == 1 ) 
			YRotateObject ( t.widget.widgetXZObj, 90 );
		else
			YRotateObject ( t.widget.widgetXZObj, 0 );
		YRotateObject ( t.widget.widgetYZObj, 0 );
	}
	else
	{
		YRotateObject ( t.widget.widgetZObj, 0 );
		if ( t.txflip == 1 ) 
		{
			YRotateObject ( t.widget.widgetXZObj, 180 );
		}
		else
		{
			YRotateObject ( t.widget.widgetXZObj, 270 );
		}
		YRotateObject ( t.widget.widgetYZObj, 180 );
	}

	// align panels better
	#ifdef WICKEDENGINE
	#else
	for ( t.a = t.widget.widgetPOSObj ; t.a <= t.widget.widgetLCKObj; t.a++ )
	{
		SetObjectToCameraOrientation ( t.a );
		MoveObjectRight (  t.a,5 );
		MoveObject (  t.a,-5 );
	}
	#endif

	// default location for widget popup is top right
	#ifdef WICKEDENGINE
	#else
	t.tshiftscrx_f=0 ; t.tshiftscry_f=0;
	t.tareawidth_f=(GetDisplayWidth()-66);
	t.tareaheight_f=(GetDisplayHeight()-18);
	t.tmousemodifierx_f=(GetChildWindowWidth()+0.0)/(GetWindowWidth()+0.0);
	t.tmousemodifiery_f=(GetChildWindowHeight()+0.0)/(GetWindowHeight()+0.0);
	t.tmaxwidthhere_f=t.tareawidth_f*t.tmousemodifierx_f;
	t.tmaxheighthere_f=t.tareaheight_f*t.tmousemodifiery_f;
	//  move menu when near right and top
	t.tmenux_f=GetScreenX(t.widget.widgetPOSObj);
	t.tmenuy_f=GetScreenY(t.widget.widgetPOSObj);
	if (  t.tmenux_f>t.tmaxwidthhere_f*0.9 ) 
	{
		t.tshiftscrx_f=t.tshiftscrx_f-9;
	}
	if (  t.tmenuy_f<190.0*t.tmousemodifierx_f ) 
	{
		t.tshiftscry_f=t.tshiftscry_f-10;
	}
	//  apply overall shift when in lower left quadrant
	if (  t.tmenux_f<(t.tareawidth_f/2.0) ) 
	{
		t.tshiftscrx_f=t.tshiftscrx_f+(((t.tareawidth_f/2.0)-t.tmenux_f)*0.007);
	}
	if (  t.tmenuy_f>(t.tareaheight_f/2.0) ) 
	{
		t.tshiftscry_f=t.tshiftscry_f+((t.tmenuy_f-(t.tareaheight_f/2.0))*0.007);
	}

	//  position final widget panel resting place
	for ( t.a = t.widget.widgetPOSObj ; t.a<=  t.widget.widgetLCKObj; t.a++ )
	{
		MoveObjectRight (  t.a,t.tshiftscrx_f );
		MoveObjectUp (  t.a,t.tshiftscry_f );
	}

	//  if widget core not in screen, hide widget menu altogether
	if (  GetInScreen(t.widget.widgetXZObj) == 1 ) 
	{
		for ( t.a = t.widget.widgetPOSObj ; t.a<=  t.widget.widgetLCKObj; t.a++ )
		{
			ShowLimb (  t.a,0 );
		}
	}
	else
	{
		for ( t.a = t.widget.widgetPOSObj ; t.a<=  t.widget.widgetLCKObj; t.a++ )
		{
			HideLimb (  t.a,0 );
		}
	}
	#endif
}

void widget_updatewidgetobject ( void )
{
	//  If no picked object and the widget has been in use, switch it off
	if (  t.widget.pickedObject == 0 ) 
	{
		if (  t.widget.activeObject>0 ) 
		{
			t.widget.pickedEntityIndex=0;
			t.widget.grabbed=0;
			t.widget.pickedSection=0;
			t.widget.mode=0;
			for ( t.a = 0 ; t.a <= t.widget.widgetMAXObj; t.a++ )
			{
				if (  ObjectExist(g.widgetobjectoffset+t.a) ) HideObject ( g.widgetobjectoffset+t.a );
			}
			t.widget.activeObject=0;
			extern bool bTriggerVisibleWidget;
			bTriggerVisibleWidget = false;
		}
	}
	else
	{
		if (  t.widget.pickedObject != t.widget.activeObject  )  t.widget.activeObject = 0;
	}
}

void widget_check_for_new_object_selection ( void )
{
	// check if an object has been chosen
	if (  t.widget.activeObject == 0 ) 
	{
		if (  t.widget.pickedObject != 0 ) 
		{
			t.widget.activeObject=t.widget.pickedObject;
			t.widget.pickedSection=0;
			t.widget.grabbed=0;
			t.widget.mode=0;
			widget_show_widget ( );
		}
	}
	t.widget.oldActiveObject=t.widget.activeObject;
}

void widget_show_widget ( void )
{
	//  Show the widget in all its glory
	if ( t.widget.activeObject == 0  )  return;
	if ( ObjectExist(t.widget.activeObject) == 0  )  return;
	#ifdef WICKEDENGINE
	if (widget_temp_disabled) return;
	#ifdef ALLOWSELECTINGLOCKEDOBJECTS
	//PE: Never enable if locked.
	if (t.widget.pickedEntityIndex > 0 && t.entityelement[t.widget.pickedEntityIndex].editorlock == 1)
		return;
	#endif
	#endif
	// what is shown varies on which mode is picked (translate, rotate or scale)
	t.oldry_f = ObjectAngleY(t.widget.activeObject);
	for (t.a = 0; t.a <= t.widget.widgetMAXObj; t.a++)
	{
		if (ObjectExist(g.widgetobjectoffset + t.a))
		{
			HideObject(g.widgetobjectoffset + t.a);
		}
		#ifdef WICKEDENGINE
		sObject* pObject = g_ObjectList[g.widgetobjectoffset + t.a];
		if (pObject)
		{
			WickedCall_SetObjectCastShadows(pObject, false);
		}
		extern bool bTriggerVisibleWidget;
		bTriggerVisibleWidget = false;
		#endif
	}
	// detect if selected in an EBE entity
	bool bIsEBEWidget = false;
	int iEntID = -1;
	t.ttte=t.widget.pickedEntityIndex;
	if (  t.ttte>0 ) 
	{
		iEntID = t.entityelement[t.ttte].bankindex;
		if ( iEntID > 0 ) 
			if ( t.entityprofile[iEntID].isebe != 0 )
				bIsEBEWidget = true;
	}

	// normally scale and property (but EBE can change this texture)
	#ifdef WICKEDENGINE
	#else
	TextureObject ( t.widget.widgetSCLObj, 0, t.widget.imagestart+3 );
	TextureObject ( t.widget.widgetPRPObj, 0, t.widget.imagestart+4 );
	#endif

	// rubber band or selected parent
	bool bRealRubberBand = false;
	if ( g.entityrubberbandlist.size() > 0 )
	{
		bRealRubberBand = true;
		if ( t.widget.pickedEntityIndex > 0 )
		{
			bool bPartOfParentChildGroup = false;
			editor_rec_checkifindexinparentchain ( t.widget.pickedEntityIndex, &bPartOfParentChildGroup );
			if ( bPartOfParentChildGroup == true )
				bRealRubberBand = false;
		}
	}

	// show all or just POS in widget popup
	#ifdef WICKEDENGINE
	#else
	if ( bRealRubberBand == true )
	{
		// Rubber band select POS, DELETE and LOCK only
		t.a = t.widget.widgetPOSObj; if ( ObjectExist(t.a) == 1  ) ShowObject ( t.a );
		t.a = t.widget.widgetROTObj; if ( ObjectExist(t.a) == 1  ) ShowObject ( t.a );
		t.a = t.widget.widgetSCLObj; if ( ObjectExist(t.a) == 1  ) ShowObject ( t.a );
		t.a = t.widget.widgetDELObj; if ( ObjectExist(t.a) == 1  ) ShowObject ( t.a );
		t.a = t.widget.widgetLCKObj; if ( ObjectExist(t.a) == 1  ) ShowObject ( t.a );

		// move POS button alongside DEL and EXT
		OffsetLimb ( t.widget.widgetPOSObj,0,-1.2f,3.0f+(t.tmovestep_f*3),t.tmovezup_f  ); RotateLimb (  t.widget.widgetPOSObj,0,0,0,0 );
		OffsetLimb ( t.widget.widgetROTObj,0,-1.2f,3.0f+(t.tmovestep_f*2),t.tmovezup_f  ); RotateLimb (  t.widget.widgetPOSObj,0,0,0,0 );
		OffsetLimb ( t.widget.widgetSCLObj,0,-1.2f,3.0f+(t.tmovestep_f*1),t.tmovezup_f  ); RotateLimb (  t.widget.widgetPOSObj,0,0,0,0 );
	}
	else
	{
		// POS, ROT, SCALE, etc
		for ( t.a = t.widget.widgetPOSObj ; t.a<=  t.widget.widgetLCKObj; t.a++ )
		{
			if (  ObjectExist(t.a) == 1  )  ShowObject (  t.a );
		}

		// hide if EBE widget
		if ( bIsEBEWidget == true ) 
		{
			OffsetLimb ( t.widget.widgetPOSObj,0,-1.2f,3.0f+(t.tmovestep_f*5),t.tmovezup_f  ); RotateLimb (  t.widget.widgetPOSObj,0,0,0,0 );
			OffsetLimb ( t.widget.widgetROTObj,0,-1.2f,3.0f+(t.tmovestep_f*4),t.tmovezup_f  ); RotateLimb (  t.widget.widgetROTObj,0,0,0,0 );
			OffsetLimb ( t.widget.widgetSCLObj,0,-1.2f,3.0f+(t.tmovestep_f*3),t.tmovezup_f  ); RotateLimb (  t.widget.widgetSCLObj,0,0,0,0 );
			TextureObject ( t.widget.widgetSCLObj, 0, t.widget.imagestart+8 ); // EDIT
			TextureObject ( t.widget.widgetPRPObj, 0, t.widget.imagestart+9 ); // SAVE
		}
		else
		{
			// correct POS position
			OffsetLimb ( t.widget.widgetPOSObj,0,-1.2f,3.0f+(t.tmovestep_f*5),t.tmovezup_f  ); RotateLimb (  t.widget.widgetPOSObj,0,0,0,0 );
			OffsetLimb ( t.widget.widgetROTObj,0,-1.2f,3.0f+(t.tmovestep_f*4),t.tmovezup_f  ); RotateLimb (  t.widget.widgetROTObj,0,0,0,0 );
			OffsetLimb ( t.widget.widgetSCLObj,0,-1.2f,3.0f+(t.tmovestep_f*3),t.tmovezup_f  ); RotateLimb (  t.widget.widgetSCLObj,0,0,0,0 );
		}
	}
	#endif

	if ( t.widget.mode == 0 ) 
	{
		for ( t.a = 0 ; t.a <= 5; t.a++ )
		{
			if ( ObjectExist(g.widgetobjectoffset+t.a) == 1 ) 
			{
				ShowObject ( g.widgetobjectoffset+t.a );
				PositionObject ( g.widgetobjectoffset+t.a, CameraPositionX(), CameraPositionY(), CameraPositionZ() );
				if (t.widget.mode == 0)
					PointObject ( g.widgetobjectoffset+t.a,ObjectPositionX(t.widget.activeObject)+t.widget.offsetx,ObjectPositionY(t.widget.activeObject)+t.widget.offsety,ObjectPositionZ(t.widget.activeObject)+t.widget.offsetz );
				else
					PointObject (g.widgetobjectoffset + t.a, ObjectPositionX(t.widget.activeObject) + 0, ObjectPositionY(t.widget.activeObject) + 0, ObjectPositionZ(t.widget.activeObject) + 0);
				MoveObject ( g.widgetobjectoffset+t.a,40 );
				RotateObject ( g.widgetobjectoffset+t.a,0,0,0 );
			}
		}
	}
	if ( t.widget.mode == 1 ) 
	{
		for ( t.a = 6 ; t.a <= 8; t.a++ )
		{
			if ( ObjectExist(g.widgetobjectoffset+t.a) == 1 ) 
			{
				#ifdef WICKEDENGINE
				if (iEntID > 0)
				{
					// Player start marker and characters can be rotated about the y-axis, so show the partial widget object
					if (t.entityprofile[iEntID].ismarker == 1 || t.entityprofile[iEntID].ischaracter)
					{
						if (t.a == 6 || t.a == 8)
						{
							HideObject(g.widgetobjectoffset + t.a);
							continue;
						}
					}
				}
				#endif
				ShowObject ( g.widgetobjectoffset+t.a );
				PositionObject ( g.widgetobjectoffset+t.a, CameraPositionX(), CameraPositionY(), CameraPositionZ() );
				if (t.widget.mode == 0)
					PointObject (g.widgetobjectoffset + t.a, ObjectPositionX(t.widget.activeObject) + t.widget.offsetx, ObjectPositionY(t.widget.activeObject) + t.widget.offsety, ObjectPositionZ(t.widget.activeObject) + t.widget.offsetz);
				else
					PointObject ( g.widgetobjectoffset+t.a,ObjectPositionX(t.widget.activeObject)+0,ObjectPositionY(t.widget.activeObject)+0,ObjectPositionZ(t.widget.activeObject)+0 );
				MoveObject ( g.widgetobjectoffset+t.a,40 );
			}
		}
		if ( ObjectExist( g.widgetobjectoffset+6) == 1 ) 
		{
			ScaleObject ( g.widgetobjectoffset+6,50,100,100 );
			ScaleObject ( g.widgetobjectoffset+7,100,50,100 );
			ScaleObject ( g.widgetobjectoffset+8,100,100,50 );
			//new widget rotation is global rotation!
			//RotateObject ( g.widgetobjectoffset+6, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
			//RotateObject ( g.widgetobjectoffset+7, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
			//RotateObject ( g.widgetobjectoffset+8, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
			RotateObject(g.widgetobjectoffset + 6, 0, 0, 0);
			RotateObject(g.widgetobjectoffset + 7, 0, 0, 0);
			RotateObject(g.widgetobjectoffset + 8, 0, 0, 0);
		}
		if ( g.entityrubberbandlist.size() > 0 || bIsEBEWidget == true )
		{
			// hide X and Z adjustment (for now)
			HideObject ( g.widgetobjectoffset + 6 );
			RotateObject ( g.widgetobjectoffset+7, 0, ObjectAngleY(t.widget.activeObject), 0 );
			HideObject ( g.widgetobjectoffset + 8 );
		}	
	}
	if ( t.widget.mode == 2 ) 
	{
		for ( t.a = 9 ; t.a <= 12; t.a++ )
		{
			if ( ObjectExist(g.widgetobjectoffset+t.a) == 1 ) 
			{
				ShowObject ( g.widgetobjectoffset+t.a );
				PositionObject ( g.widgetobjectoffset+t.a, CameraPositionX(), CameraPositionY(), CameraPositionZ() );
				if (t.widget.mode == 0)
					PointObject (g.widgetobjectoffset + t.a, ObjectPositionX(t.widget.activeObject) + t.widget.offsetx, ObjectPositionY(t.widget.activeObject) + t.widget.offsety, ObjectPositionZ(t.widget.activeObject) + t.widget.offsetz);
				else
					PointObject ( g.widgetobjectoffset+t.a,ObjectPositionX(t.widget.activeObject)+0,ObjectPositionY(t.widget.activeObject)+0,ObjectPositionZ(t.widget.activeObject)+0 );
				MoveObject ( g.widgetobjectoffset+t.a,40 );
				RotateObject ( g.widgetobjectoffset+t.a,0,0,0 );
			}
		}
		if ( ObjectExist(g.widgetobjectoffset+9) == 1 ) 
		{
			RotateObject ( g.widgetobjectoffset+9, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
			RotateObject ( g.widgetobjectoffset+10, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
			RotateObject ( g.widgetobjectoffset+11, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
			RotateObject ( g.widgetobjectoffset+12, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
		}
	}

	// hide any buttons and widgets if entity is a 'waypoint zone type'
	#ifdef WICKEDENGINE
	#else
	t.ttte=t.widget.pickedEntityIndex;
	if ( t.ttte>0 ) 
	{
		t.tttwi=t.entityelement[t.ttte].eleprof.trigger.waypointzoneindex;
		if ( t.tttwi>0 ) 
		{
			// hide top three buttons
			if ( ObjectExist(t.widget.widgetPOSObj) == 1 ) 
			{
				HideObject (  t.widget.widgetPOSObj );
				HideObject (  t.widget.widgetROTObj );
				HideObject (  t.widget.widgetSCLObj );
				// hide widget gadgets
				for ( t.a = 6 ; t.a<=  12; t.a++ )
				{
					HideObject ( g.widgetobjectoffset+t.a );
				}
			}
		}
	}
	#endif
}

void widget_switchoff ( void )
{
	t.widget.pickedSection=0;
	t.widget.activeObject=0;
	t.widget.pickedObject=0;
	t.widget.mode=0;
	widget_hide ( );
}

void widget_hide ( void )
{
	for ( t.a = 0 ; t.a<=  40; t.a++ )
	{
		if (  ObjectExist (g.widgetobjectoffset+t.a)  )  HideObject (  g.widgetobjectoffset+t.a );
	}
	t.widget.mode=0;
	extern bool bTriggerVisibleWidget;
	bTriggerVisibleWidget = false;
}

void widget_free ( void )
{
	//  clean up widget files (will need to call widget_init again before using the widget)
	for ( t.a = 0 ; t.a<=  40; t.a++ )
	{
		if (  ObjectExist (g.widgetobjectoffset+t.a)  )  DeleteObject (  g.widgetobjectoffset+t.a ) ;
	}
	for ( t.a = 1 ; t.a<=  12; t.a++ )
	{
		t.r=DeleteVector3(g.widgetvectorindex+t.a);
	}
	for ( t.a = 1 ; t.a<=  5; t.a++ )
	{
		if (  MatrixExist(g.widgetStartMatrix+t.a)  )  DeleteMatrix (  g.widgetStartMatrix+t.a );
	}
	t.widget.mode=0;
	return;
}
