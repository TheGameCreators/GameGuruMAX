//----------------------------------------------------
//--- GAMEGURU - M-DAINew
//----------------------------------------------------

#include "stdafx.h"
#include "gameguru.h"

#ifdef WICKEDENGINE
#include "GGRecastDetour.h"
extern GGRecastDetour g_RecastDetour;
#endif

// Globals 
bool g_bDormantCheckForThisCycle = true;

void darkai_init ( void )
{
	// Inits
	t.aisystem.on=1;
	t.aisystem.processlogic = 1;
	t.aisystem.processplayerlogic = 1;

	// create debug objects to visualize AI
	#ifdef NEWMAXAISYSTEM
	darkai_createinternaldebugvisuals();
	#endif
	
	// reset smoothanim array so no carryover of transitions to new test game
	darkai_resetsmoothanims();
}

void darkai_free ( void )
{
	// free A.I resources
	t.aisystem.on=0;

	// free debug objects
	#ifdef NEWMAXAISYSTEM
	darkai_destroyinternaldebugvisuals();
	#endif
}

void darkai_createinternaldebugvisuals_coneofsight (int iOuterViewObject, float fOuterViewArc, float fViewRange)
{
	// if no cone, assume character can see in all directions!
	if (fOuterViewArc == 0.0f) fOuterViewArc = 179.0f;
	
	// if existing cone of sight object, delete it
	if (ObjectExist(iOuterViewObject) == 1) DeleteObject(iOuterViewObject);

	// create a fan outline using Zak's prisms
	WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_CURSOROBJECT);
	WickedCall_PresetObjectPutInEmissive(1);

	// Find a free memblock
	int iMemblockToMakeConeOfSight = 0;
	for (int i = 1; i <= 257; i++) if (MemblockExist(i) == 0) { iMemblockToMakeConeOfSight = i; break; }
	if (iMemblockToMakeConeOfSight == 0) return;

	// fan outline shape
	int iSegmentCount = 36;
	int iLineCount = 2 + iSegmentCount;
	float fOuterViewArcRad = GGToRadian(fOuterViewArc);
	float fAngleStart = (fOuterViewArc / -2.0f);
	float fAngleStartRad = GGToRadian(fAngleStart);
	float fSegAngle = (fOuterViewArc / iSegmentCount);
	float fSegAngleRad = GGToRadian(fSegAngle);

	// create object memblock
	int vertsize = 32;
	int iSizeBytes = 0;
	int vertexCount = 36 * iLineCount;
	iSizeBytes = vertsize * vertexCount;
	iSizeBytes += 12;
	MakeMemblock(iMemblockToMakeConeOfSight, iSizeBytes);
	WriteMemblockDWord(iMemblockToMakeConeOfSight, 0, GGFVF_XYZ | GGFVF_NORMAL | GGFVF_TEX1);
	WriteMemblockDWord(iMemblockToMakeConeOfSight, 4, 32);
	WriteMemblockDWord(iMemblockToMakeConeOfSight, 8, vertexCount);

	// all lines needed
	int v = 0;
	for (int iLineIndex = 0; iLineIndex < iLineCount; iLineIndex++)
	{
		// create a set of prism lines to create the outline of a cone fan
		float fromx = 0, fromy = 0, fromz = 0;
		float tox = 0, toy = 0, toz = 0;
		if (iLineIndex == 0)
		{
			// left side
			tox = sin(fAngleStartRad) * fViewRange;
			toz = cos(fAngleStartRad) * fViewRange;
		}
		else if (iLineIndex == 1)
		{
			// right side
			tox = sin(fAngleStartRad + fOuterViewArcRad) * fViewRange;
			toz = cos(fAngleStartRad + fOuterViewArcRad) * fViewRange;
		}
		else
		{
			// line at end of fan
			fromx = sin(fAngleStartRad + ((iLineIndex - 2)*fSegAngleRad)) * fViewRange;
			fromz = cos(fAngleStartRad + ((iLineIndex - 2)*fSegAngleRad)) * fViewRange;
			tox   = sin(fAngleStartRad + ((iLineIndex - 1)*fSegAngleRad)) * fViewRange;
			toz   = cos(fAngleStartRad + ((iLineIndex - 1)*fSegAngleRad)) * fViewRange;
		}

		// line coordinates
		float p0[3];
		float p1[3];
		p0[0] = fromx; p0[1] = fromy; p0[2] = fromz;
		p1[0] = tox; p1[1] = toy; p1[2] = toz;

		// create points
		float points[18];
		physics_debug_make_prism_between_points(p0, p1, points, 0.25f);

		// Corners of the prism
		float x0, x1, x2, x3, x4, x5;
		float y0, y1, y2, y3, y4, y5;
		float z0, z1, z2, z3, z4, z5;
		x0 = points[0]; y0 = points[1]; z0 = points[2];
		x1 = points[3]; y1 = points[4]; z1 = points[5];
		x2 = points[6]; y2 = points[7]; z2 = points[8];
		x3 = points[9]; y3 = points[10]; z3 = points[11];
		x4 = points[12]; y4 = points[13]; z4 = points[14];
		x5 = points[15]; y5 = points[16]; z5 = points[17];

		// Midpoints
		float mx03, mx14, mx25;
		float my03, my14, my25;
		float mz03, mz14, mz25;
		mx03 = (x0 + x3) / 2.0f; my03 = (y0 + y3) / 2.0f; mz03 = (z0 + z3) / 2.0f;
		mx14 = (x1 + x4) / 2.0f; my14 = (y1 + y4) / 2.0f; mz14 = (z1 + z4) / 2.0f;
		mx25 = (x2 + x5) / 2.0f; my25 = (y2 + y5) / 2.0f; mz25 = (z2 + z5) / 2.0f;

		// setup UV for the prism colors
		float fRelationUVs[24];
		const float off = 0.16666667f;
		fRelationUVs[0] = 0; fRelationUVs[1] = 0;
		fRelationUVs[2] = 0; fRelationUVs[3] = 1;
		fRelationUVs[4] = 1; fRelationUVs[5] = 0;
		fRelationUVs[6] = 1; fRelationUVs[7] = 0;
		fRelationUVs[8] = 0; fRelationUVs[9] = 1;
		fRelationUVs[10] = 1; fRelationUVs[11] = 1;
		fRelationUVs[12] = 0 + off; fRelationUVs[13] = 0 + off;
		fRelationUVs[14] = 0 + off; fRelationUVs[15] = 1 + off;
		fRelationUVs[16] = 1 + off; fRelationUVs[17] = 0 + off;
		fRelationUVs[18] = 1 + off; fRelationUVs[19] = 0 + off;
		fRelationUVs[20] = 0 + off; fRelationUVs[21] = 1 + off;
		fRelationUVs[22] = 1 + off; fRelationUVs[23] = 1 + off;

		// Bottom, right and left face
		int iFound = iMemblockToMakeConeOfSight;
		extern void AddVertToObjectRelation(float x, float y, float z, float texU, float texV, int v, int memblock);
		AddVertToObjectRelation(x0, y0, z0, fRelationUVs[0], fRelationUVs[1], v++, iFound);
		AddVertToObjectRelation(x2, y2, z2, fRelationUVs[2], fRelationUVs[3], v++, iFound);
		AddVertToObjectRelation(mx03, my03, mz03, fRelationUVs[4], fRelationUVs[5], v++, iFound);
		AddVertToObjectRelation(mx03, my03, mz03, fRelationUVs[6], fRelationUVs[7], v++, iFound);
		AddVertToObjectRelation(x2, y2, z2, fRelationUVs[8], fRelationUVs[9], v++, iFound);
		AddVertToObjectRelation(mx25, my25, mz25, fRelationUVs[10], fRelationUVs[11], v++, iFound);
		AddVertToObjectRelation(mx03, my03, mz03, fRelationUVs[0], fRelationUVs[1], v++, iFound);
		AddVertToObjectRelation(mx25, my25, mz25, fRelationUVs[2], fRelationUVs[3], v++, iFound);
		AddVertToObjectRelation(x3, y3, z3, fRelationUVs[4], fRelationUVs[5], v++, iFound);
		AddVertToObjectRelation(x3, y3, z3, fRelationUVs[6], fRelationUVs[7], v++, iFound);
		AddVertToObjectRelation(mx25, my25, mz25, fRelationUVs[8], fRelationUVs[9], v++, iFound);
		AddVertToObjectRelation(x5, y5, z5, fRelationUVs[10], fRelationUVs[11], v++, iFound);
		AddVertToObjectRelation(x1, y1, z1, fRelationUVs[0], fRelationUVs[1], v++, iFound);
		AddVertToObjectRelation(x0, y0, z0, fRelationUVs[2], fRelationUVs[3], v++, iFound);
		AddVertToObjectRelation(mx14, my14, mz14, fRelationUVs[4], fRelationUVs[5], v++, iFound);
		AddVertToObjectRelation(mx14, my14, mz14, fRelationUVs[6], fRelationUVs[7], v++, iFound);
		AddVertToObjectRelation(x0, y0, z0, fRelationUVs[8], fRelationUVs[9], v++, iFound);
		AddVertToObjectRelation(mx03, my03, mz03, fRelationUVs[10], fRelationUVs[11], v++, iFound);
		AddVertToObjectRelation(mx14, my14, mz14, fRelationUVs[0], fRelationUVs[1], v++, iFound);
		AddVertToObjectRelation(mx03, my03, mz03, fRelationUVs[2], fRelationUVs[3], v++, iFound);
		AddVertToObjectRelation(x4, y4, z4, fRelationUVs[4], fRelationUVs[5], v++, iFound);
		AddVertToObjectRelation(x4, y4, z4, fRelationUVs[6], fRelationUVs[7], v++, iFound);
		AddVertToObjectRelation(mx03, my03, mz03, fRelationUVs[8], fRelationUVs[9], v++, iFound);
		AddVertToObjectRelation(x3, y3, z3, fRelationUVs[10], fRelationUVs[11], v++, iFound);
		AddVertToObjectRelation(x2, y2, z2, fRelationUVs[0], fRelationUVs[1], v++, iFound);
		AddVertToObjectRelation(x1, y1, z1, fRelationUVs[2], fRelationUVs[3], v++, iFound);
		AddVertToObjectRelation(mx25, my25, mz25, fRelationUVs[4], fRelationUVs[5], v++, iFound);
		AddVertToObjectRelation(mx25, my25, mz25, fRelationUVs[6], fRelationUVs[7], v++, iFound);
		AddVertToObjectRelation(x1, y1, z1, fRelationUVs[8], fRelationUVs[9], v++, iFound);
		AddVertToObjectRelation(mx14, my14, mz14, fRelationUVs[10], fRelationUVs[11], v++, iFound);
		AddVertToObjectRelation(mx25, my25, mz25, fRelationUVs[0], fRelationUVs[1], v++, iFound);
		AddVertToObjectRelation(mx14, my14, mz14, fRelationUVs[2], fRelationUVs[3], v++, iFound);
		AddVertToObjectRelation(x5, y5, z5, fRelationUVs[4], fRelationUVs[5], v++, iFound);
		AddVertToObjectRelation(x5, y5, z5, fRelationUVs[6], fRelationUVs[7], v++, iFound);
		AddVertToObjectRelation(mx14, my14, mz14, fRelationUVs[8], fRelationUVs[9], v++, iFound);
		AddVertToObjectRelation(x4, y4, z4, fRelationUVs[10], fRelationUVs[11], v++, iFound);
	}

	// create the object
	int iMeshID = g.meshgeneralwork;
	if (GetMeshExist(iMeshID) == 1) DeleteMesh(iMeshID);
	CreateMeshFromMemblock(iMeshID, iMemblockToMakeConeOfSight);
	MakeObject(iOuterViewObject, iMeshID, 0);
	SetObjectCull(iOuterViewObject, 0);
	ShowObject(iOuterViewObject);
	SetObjectMask (iOuterViewObject, 1);
	DisableObjectZDepth (iOuterViewObject);
	SetObjectCollisionOff (iOuterViewObject);

	// provide with a texture
	if (ImageExist(g.coneofsightimageoffset) == 0) LoadImage("editors\\uiv3\\nodeconeofsight.png", g.coneofsightimageoffset);
	TextureObject(iOuterViewObject, g.coneofsightimageoffset);

	// wicked settings
	sObject* pObject = GetObjectData(iOuterViewObject);
	WickedCall_SetObjectCastShadows(pObject, false);
	WickedCall_SetObjectLightToUnlit(pObject, (int)wiScene::MaterialComponent::SHADERTYPE::SHADERTYPE_UNLIT);

	// finished creating object
	WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_NORMAL);
	WickedCall_PresetObjectPutInEmissive(0);

	// free resources used
	DeleteMemblock(iMemblockToMakeConeOfSight);
	if (GetMeshExist(iMeshID) == 1) DeleteMesh(iMeshID);
}

void darkai_createinternaldebugvisuals (void)
{
	// created in 'darkai_setupcharacter' initially
}

void darkai_destroyinternaldebugvisuals (void)
{
	// Free all debug objects
	#ifdef NEWMAXAISYSTEM
	if (ObjectExist(g.debugraycastvisual) == 1) DeleteObject(g.debugraycastvisual);
	for (int ichardebugobj = g.debugconeofsightstart; ichardebugobj < g.debugconeofsightfinish; ichardebugobj++)
		if (ObjectExist(ichardebugobj) == 1)
			DeleteObject(ichardebugobj);
	#endif
}

void darkai_updatedebugobjects_forcharacter (bool bCharIsActive)
{
	// Control ONE raycaster line
	extern bool g_bShowRecastDetourDebugVisuals;
	if (ObjectExist(g.debugraycastvisual) == 0)
	{
		WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_CURSOROBJECT);
		WickedCall_PresetObjectPutInEmissive(1);
		MakeObjectBox(g.debugraycastvisual, 1, 1, 100);
		PositionObject(g.debugraycastvisual, -999999, -999999, -999999);
		if (ImageExist(g.coneofsightimageoffset) == 0) LoadImage("editors\\uiv3\\nodeconeofsight.png", g.coneofsightimageoffset);
		TextureObject(g.debugraycastvisual, g.coneofsightimageoffset);
		sObject* pObject = GetObjectData(g.debugraycastvisual);
		WickedCall_SetObjectLightToUnlit(pObject, (int)wiScene::MaterialComponent::SHADERTYPE::SHADERTYPE_UNLIT);
		WickedCall_SetObjectCastShadows(pObject, false);
		WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_NORMAL);
		WickedCall_PresetObjectPutInEmissive(0);
	}
	else
	{
		if (g_bShowRecastDetourDebugVisuals == true)
			ShowObject(g.debugraycastvisual);
		else
			HideObject(g.debugraycastvisual);
	}

	/*
	extern int g_debugraycastvisual;
	g_debugraycastvisual = g.debugraycastvisual;
	*/

	// Control cone of sight object
	#ifdef NEWMAXAISYSTEM
	int iOuterViewObject = g.debugconeofsightstart + g.charanimindex;
	if (ObjectExist(iOuterViewObject) == 1)
	{
		// takes t.charanimstate.e and t.charanimstate.obj
		float fAIObjX = ObjectPositionX(t.charanimstate.obj);
		float fAIObjY = ObjectPositionY(t.charanimstate.obj) + 5.0f;
		float fAIObjZ = ObjectPositionZ(t.charanimstate.obj);

		// get object facing angle, also apply any head turning activity
		bool bWithinDebugViewRange = false;
		float fAIObjAngleY = ObjectAngleY(t.charanimstate.obj);
		fAIObjAngleY += t.charanimstate.neckRightAndLeft;
		float fDX = fAIObjX - CameraPositionX(0);
		float fDZ = fAIObjZ - CameraPositionZ(0);
		float fDist = sqrt(fabs(fDX*fDX) + fabs(fDZ*fDZ));
		float fViewRange = t.entityelement[t.charanimstate.e].eleprof.conerange;
		if (fDist < fViewRange*1.25f) bWithinDebugViewRange = true;

		// position and show/hide
		PositionObject (iOuterViewObject, fAIObjX, fAIObjY, fAIObjZ);
		YRotateObject (iOuterViewObject, fAIObjAngleY);
		if (bCharIsActive == true && g_bShowRecastDetourDebugVisuals == true && bWithinDebugViewRange == true)
		{
			// cone of sight debug
			ShowObject(iOuterViewObject);

			// also show object entity ID (so can debug logic in behavior editor)
			if (fDist < 500)
			{
				t.entityelement[t.charanimstate.e].overprompt_s = cstr(t.charanimstate.e);
				t.entityelement[t.charanimstate.e].overprompttimer = Timer() + 1000;
			}
		}
		else
		{
			HideObject(iOuterViewObject);
		}
	}
	#endif
}

void darkai_calcplrvisible (void)
{
	// takes tcharanimindex - work out if entity A.I can see (stored until recalculated)
	t.entityelement[t.charanimstate.e].plrvisible = 0;
	t.entityelement[t.charanimstate.e].lua.flagschanged = 1;
	if (t.player[t.plrid].health > 0)
	{
		// work out distance between player and entity
		t.ttdx_f = ObjectPositionX(t.aisystem.objectstartindex) - ObjectPositionX(t.charanimstate.obj);
		t.ttdz_f = ObjectPositionZ(t.aisystem.objectstartindex) - ObjectPositionZ(t.charanimstate.obj);
		t.ttdd_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdz_f*t.ttdz_f));
		float fDistanceRangeToCheck = t.maximumnonefreezedistance;
		if (t.entityelement[t.charanimstate.e].eleprof.conerange > 0) fDistanceRangeToCheck = t.entityelement[t.charanimstate.e].eleprof.conerange;
		if (t.ttdd_f < fDistanceRangeToCheck)
		{
			// get object facing angle, also apply any head turning activity
			float fAIObjAngleY = ObjectAngleY(t.charanimstate.obj);
			fAIObjAngleY += t.charanimstate.neckRightAndLeft;

			// player within units, otherwise skip further vis checking
			t.ttda_f = atan2deg(t.ttdx_f, t.ttdz_f);
			t.ttdiff_f = WrapValue(t.ttda_f) - WrapValue(fAIObjAngleY);
			if (t.ttdiff_f < -180) t.ttdiff_f = t.ttdiff_f + 360;
			if (t.ttdiff_f > 180) t.ttdiff_f = t.ttdiff_f - 360;
			t.tconeangle = t.entityelement[t.charanimstate.e].eleprof.coneangle;
			if (t.tconeangle == 0) t.tconeangle = 179;
			if (abs(t.ttdiff_f) <= t.tconeangle)
			{
				// assume visible unless blocked (below)
				t.ttokay = 1;

				// match ray cast with masterinterpreter raycasting (baseY+65)
				t.brayx1_f = ObjectPositionX(t.charanimstate.obj);
				t.brayy1_f = ObjectPositionY(t.charanimstate.obj) + 35;// 65 - this fixes characters shooting from the eyeballs!
				t.brayz1_f = ObjectPositionZ(t.charanimstate.obj);

				// location of player (if player camera can see enemy, vice versa)
				t.tcamerapositionx_f = CameraPositionX(t.terrain.gameplaycamera);
				t.tcamerapositiony_f = CameraPositionY(t.terrain.gameplaycamera);
				t.tcamerapositionz_f = CameraPositionZ(t.terrain.gameplaycamera);
				t.brayx2_f = t.tcamerapositionx_f;
				t.brayy2_f = t.tcamerapositiony_f;
				t.brayz2_f = t.tcamerapositionz_f;

				// first ensure not going through physics terrain
				if (ODERayTerrain(t.brayx1_f, t.brayy1_f, t.brayz1_f, t.brayx2_f, t.brayy2_f, t.brayz2_f, true) == 1)
				{
					t.ttokay = 0;
				}
				if (t.ttokay == 1)
				{
					// actually move ray BACK a little in case enemy right up against something!
					t.ttdx_f = t.brayx2_f - t.brayx1_f;
					t.ttdy_f = t.brayy2_f - t.brayy1_f;
					t.ttdz_f = t.brayz2_f - t.brayz1_f;
					t.ttdd_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdy_f*t.ttdy_f) + abs(t.ttdz_f*t.ttdz_f));
					t.ttdx_f = t.ttdx_f / t.ttdd_f;
					t.ttdy_f = t.ttdy_f / t.ttdd_f;
					t.ttdz_f = t.ttdz_f / t.ttdd_f;
					t.brayx1_f = t.brayx1_f - (t.ttdx_f*10.0);
					t.brayy1_f = t.brayy1_f - (t.ttdy_f*10.0);
					t.brayz1_f = t.brayz1_f - (t.ttdz_f*10.0);

					// exclude VWEAP held by character
					sObject* pIgnoreVWEAPObject = NULL;
					if (t.tgunobj > 0) pIgnoreVWEAPObject = GetObjectData(t.tgunobj);
					if (pIgnoreVWEAPObject) WickedCall_SetObjectRenderLayer(pIgnoreVWEAPObject, GGRENDERLAYERS_CURSOROBJECT);
					t.tintersectvalue = IntersectAllEx(g.entityviewstartobj, g.entityviewendobj, t.brayx1_f, t.brayy1_f, t.brayz1_f, t.brayx2_f, t.brayy2_f, t.brayz2_f, t.charanimstate.obj, 0, t.charanimstate.e, 500, 1);
					if (t.tintersectvalue != 0)
					{
						t.ttokay = 0;
					}
					if (pIgnoreVWEAPObject) WickedCall_SetObjectRenderLayer(pIgnoreVWEAPObject, GGRENDERLAYERS_NORMAL);
				}
				if (t.ttokay == 1)
				{
					t.entityelement[t.charanimstate.e].plrvisible = 1;
					t.entityelement[t.charanimstate.e].lua.flagschanged = 1;
				}
			}
		}
	}
}

void darkai_mouthandheadtracking (void)
{
	// impose anim frame overrides on top of regular animation
	int iCharObj = t.charanimstate.obj;

	// mouth simulation
	float fTimeFromStartOfSpeak = 0;
	if (t.charanimstate.ccpo.speak.fMouthTimeStamp == 0.0f)
	{
		// waiting for mouth timer to be started (elsewhere)
		t.charanimstate.ccpo.speak.iMouthDataShape = 0;
	}
	else
	{
		// only if mouth data
		if (t.charanimstate.ccpo.speak.mouthData.size() > 0)
		{
			fTimeFromStartOfSpeak = ((float)Timer() / 1000.0f) - t.charanimstate.ccpo.speak.fMouthTimeStamp;
			int iMouthDataNextIndex = t.charanimstate.ccpo.speak.iMouthDataIndex;
			if (fTimeFromStartOfSpeak > t.charanimstate.ccpo.speak.mouthData[iMouthDataNextIndex].fTimeStamp)
			{
				int iMouthDataShape = t.charanimstate.ccpo.speak.mouthData[iMouthDataNextIndex].iMouthShape;
				t.charanimstate.ccpo.speak.iMouthDataShape = iMouthDataShape;
				iMouthDataNextIndex++;
				t.charanimstate.ccpo.speak.fSmouthDataSpeedToNextShape = 4.0f;
				t.charanimstate.ccpo.speak.iMouthDataIndex = iMouthDataNextIndex;
				if (t.charanimstate.ccpo.speak.iMouthDataIndex >= t.charanimstate.ccpo.speak.mouthData.size())
				{
					t.charanimstate.ccpo.speak.fMouthTimeStamp = 0;
					t.charanimstate.ccpo.speak.iMouthDataIndex = 0;
				}
			}
			else
			{
				// modulate speed to final mouth shape based on closeness to next shape
				int iMouthDataCurrentIndex = iMouthDataNextIndex - 1;
				if (iMouthDataCurrentIndex >= 0)
				{
					float fTimeDifference = t.charanimstate.ccpo.speak.mouthData[iMouthDataNextIndex].fTimeStamp - t.charanimstate.ccpo.speak.mouthData[iMouthDataCurrentIndex].fTimeStamp;
					float fTimeToNextShape = fTimeFromStartOfSpeak - t.charanimstate.ccpo.speak.mouthData[iMouthDataCurrentIndex].fTimeStamp;
					t.charanimstate.ccpo.speak.fSmouthDataSpeedToNextShape = 1.0f + ((1.0f - (fTimeToNextShape / fTimeDifference))*3.0f);
				}
			}
		}
	}
	sObject* pCharObject = GetObjectData (iCharObj);
	int iFinalFrameToUse = -1;
	if (t.charanimstate.ccpo.speak.mouthData.size() > 0)
	{
		iFinalFrameToUse = t.charanimstate.ccpo.speak.iMouthDataShape;
		if (iFinalFrameToUse == 0)
		{
			iFinalFrameToUse = 12;
			if (t.charanimstate.ccpo.speak.fNeedToBlink > 1.0f)
			{
				// randomise blink (maybe use blink logic in future)
				t.charanimstate.ccpo.speak.fNeedToBlink = -0.05f;
			}
			if (t.charanimstate.ccpo.speak.fNeedToBlink < 0.0f)
			{
				// allow blink for a few frames
				t.charanimstate.ccpo.speak.fSmouthDataSpeedToNextShape = 5.0f;
				iFinalFrameToUse = 13;
			}
		}
		if (t.charanimstate.ccpo.speak.fNeedToBlink > 0.0f)
		{
			double dPowerRandom = rand() % 10000;
			if (dPowerRandom > 9900.0)
				dPowerRandom = 5.0;
			else
				dPowerRandom = dPowerRandom / 100000.0;
			t.charanimstate.ccpo.speak.fNeedToBlink += 0.0001f + (float)(dPowerRandom / 10.0f);
		}
		else
			t.charanimstate.ccpo.speak.fNeedToBlink += 0.01f;
	}

	#ifdef WICKEDENGINE
	// Neck(head) tracking is smoother as called all the time now
	float fLookAtX = 0;
	float fLookAtY = 0;
	float fLookAtZ = 0;
	if (t.charanimstate.entityTarget > 0)
	{
		fLookAtX = t.entityelement[t.charanimstate.entityTarget].x;
		fLookAtY = t.entityelement[t.charanimstate.entityTarget].y + 65.0f; // so character looks at eye level from ground
		fLookAtZ = t.entityelement[t.charanimstate.entityTarget].z;
	}
	else
	{
		fLookAtX = CameraPositionX();
		fLookAtY = CameraPositionY();
		fLookAtZ = CameraPositionZ();
	}
	float fDX = fLookAtX - ObjectPositionX (t.charanimstate.obj);
	float fDZ = fLookAtZ - ObjectPositionZ (t.charanimstate.obj);
	float fDD = sqrt (fabs(fDX*fDX) + fabs(fDZ*fDZ));

	// work out how far to twist spine left and right
	float fNeckLimit = t.charanimstate.neckRightAndLeftLimit;
	float fDA = atan2deg(fDX, fDZ);
	float fAIObjAngleY = ObjectAngleY (t.charanimstate.obj);
	float fDiffA = WrapValue(fDA) - WrapValue(fAIObjAngleY);
	if (t.charanimstate.neckAiming < -5000.0f )
	{
		float fForcedAngle = t.charanimstate.neckAiming + 10000;
		fDiffA = fForcedAngle;
	}
	if (t.charanimstate.neckAiming == 0.0f) fDiffA = 0.0f;
	if (fDiffA < -180) fDiffA = fDiffA + 360;
	if (fDiffA > 180) fDiffA = fDiffA - 360;
	if (fabs(fDiffA) > fNeckLimit)
	{
		if (fDiffA < -fNeckLimit) fDiffA = -fNeckLimit;
		if (fDiffA > fNeckLimit) fDiffA = fNeckLimit;
	}
	float fRightAndLeft = fDiffA + t.charanimstate.neckRightAndLeftOffset;

	// and the neck up and down
	float fDY = fLookAtY - (ObjectPositionY (t.charanimstate.obj) + 70.0f);
	fDA = WrapValue (GGToDegree(atan2(fDY, fDD)));
	if (t.charanimstate.neckAiming == 0.0f) fDA = 0.0f;
	fNeckLimit = t.charanimstate.neckUpAndDownLimit;
	if (fDA < 360 - fNeckLimit && fDA > 180.0f) fDA = 360 - fNeckLimit;
	if (fDA > fNeckLimit && fDA < 180.0f) fDA = fNeckLimit;
	float fYLookUpAndDown = fDA + t.charanimstate.neckUpAndDownOffset;
	fYLookUpAndDown -= t.charanimstate.spineYAdjust; // adjust for any spine tilt too!

	// smooth transition to final angle
	float fTransitionSpeed = t.charanimstate.neckAiming;
	if (fTransitionSpeed <= 0.0f)
	{
		// make head reset graceful and smooth
		if (t.charanimstate.neckAiming < -5000.0f)
			fTransitionSpeed = 3;
		else
			fTransitionSpeed = 10;
	}
	float fSmoothSpeed = fTransitionSpeed / 100.0f;
	float fDiffX = fRightAndLeft; if (fDiffX >= 180.0f) fDiffX -= 360.0f;
	float fDiffY = fYLookUpAndDown; if (fDiffY >= 180.0f) fDiffY -= 360.0f;
	fDiffX -= t.charanimstate.neckRightAndLeft;
	fDiffY -= t.charanimstate.neckUpAndDown;
	t.charanimstate.neckRightAndLeft += fDiffX * fSmoothSpeed;
	t.charanimstate.neckUpAndDown += fDiffY * fSmoothSpeed;
	#endif

	// clever system to reset pose to use a specific frame, and then allow regular animation to transform on top of it
	if (t.charanimstate.ccpo.settings.iNeckBone > 0)
	{
		sFrame* pFrameOfLimb = pCharObject->ppFrameList[t.charanimstate.ccpo.settings.iNeckBone];
		if (pFrameOfLimb)
		{
			if (iFinalFrameToUse >= 0)
			{
				// only use mouth and blink if have mouth data (and the object supports mouthshapes)
				WickedCall_SetObjectPreFrames(pCharObject, "Bip01_Head", iFinalFrameToUse, t.charanimstate.ccpo.speak.fSmouthDataSpeedToNextShape, 2);
			}
			WickedCall_RotateLimb(pCharObject, pFrameOfLimb, t.charanimstate.neckRightAndLeft, t.charanimstate.neckUpAndDown, 0); 
		}
	}
}

void darkai_spinetracking (void)
{
	// difference between player and character
	float fLookAtX = CameraPositionX();
	float fLookAtY = CameraPositionY();
	float fLookAtZ = CameraPositionZ();
	if (t.charanimstate.entityTarget > 0)
	{
		int ee = t.charanimstate.entityTarget;
		fLookAtX = t.entityelement[ee].x;
		fLookAtY = t.entityelement[ee].y + 65.0f;
		fLookAtZ = t.entityelement[ee].z;
	}
	float fDX = fLookAtX - ObjectPositionX (t.charanimstate.obj);
	float fDZ = fLookAtZ - ObjectPositionZ (t.charanimstate.obj);
	float fDD = sqrt (fabs(fDX*fDX) + fabs(fDZ*fDZ));
	
	// work out how far to twist spine left and right
	float fSpineLimit = t.charanimstate.spineRightAndLeftLimit;
	float fDA = atan2deg(fDX, fDZ);
	float fAIObjAngleY = ObjectAngleY (t.charanimstate.obj);
	float fDiffA = WrapValue(fDA) - WrapValue(fAIObjAngleY);
	if (t.charanimstate.spineAiming == 0.0f ) fDiffA = 0.0f;
	if (fDiffA < -180) fDiffA = fDiffA + 360;
	if (fDiffA > 180) fDiffA = fDiffA - 360;
	if (fabs(fDiffA) > fSpineLimit)
	{
		if (fDiffA < -fSpineLimit) fDiffA = -fSpineLimit;
		if (fDiffA > fSpineLimit) fDiffA = fSpineLimit;
	}
	float fspineRightAndLeft = fDiffA + t.charanimstate.spineRightAndLeftOffset;

	// calculate adjustment based on primary twist angle (using idle pistol crouch as basis for adjustments!!)
	float fspineYAdjust = (fabs(fspineRightAndLeft) / fSpineLimit) * -15.0f;
	float fspineZAdjust = (fspineRightAndLeft / fSpineLimit) * -30.0f;

	// work out how far to aim up and down
	float fSpineVertLimit = t.charanimstate.spineUpAndDownLimit;
	float fDY = fLookAtY - (ObjectPositionY (t.charanimstate.obj)+65);
	float fVertAngle = atan2deg(fDY, fDD);
	if (t.charanimstate.spineAiming == 0.0f) fVertAngle = 0.0f;
	if (fVertAngle < -180) fVertAngle = fVertAngle + 360;
	if (fVertAngle > 180) fVertAngle = fVertAngle - 360;
	if (fabs(fVertAngle) > fSpineVertLimit)
	{
		if (fVertAngle < -fSpineVertLimit) fVertAngle = -fSpineVertLimit;
		if (fVertAngle > fSpineVertLimit) fVertAngle = fSpineVertLimit;
	}
	fspineYAdjust = fspineYAdjust + fVertAngle + t.charanimstate.spineUpAndDownOffset;

	// smooth transition to final angle
	float fTransitionSpeed = t.charanimstate.spineAiming;
	if (fTransitionSpeed == 0.0f)
	{
		// make aim reset graceful and smooth
		fTransitionSpeed = 10;
	}
	float fSmoothSpeed = fTransitionSpeed / 100.0f;
	float fDiffX = fspineRightAndLeft; if (fDiffX >= 180.0f) fDiffX -= 360.0f;
	float fDiffY = fspineYAdjust; if (fDiffY >= 180.0f) fDiffY -= 360.0f;
	float fDiffZ = fspineZAdjust; if (fDiffZ >= 180.0f) fDiffZ -= 360.0f;
	fDiffX -= t.charanimstate.spineRightAndLeft;
	fDiffY -= t.charanimstate.spineYAdjust;
	fDiffZ -= t.charanimstate.spineZAdjust;
	t.charanimstate.spineRightAndLeft += fDiffX * fSmoothSpeed;
	t.charanimstate.spineYAdjust += fDiffY * fSmoothSpeed;
	t.charanimstate.spineZAdjust += fDiffZ * fSmoothSpeed;

	// impose anim frame overrides on top of regular animation
	int iCharObj = t.charanimstate.obj;
	if (iCharObj > 0)
	{
		int iEntID = t.entityelement[t.charanimstate.e].bankindex;
		int iFrameIndex = t.entityprofile[iEntID].spine2;
		if (iFrameIndex > 0)
		{
			sObject* pCharObject = GetObjectData(iCharObj);
			sFrame* pFrameOfLimb = pCharObject->ppFrameList[iFrameIndex];
			if (pFrameOfLimb)
			{
				WickedCall_RotateLimb(pCharObject, pFrameOfLimb, t.charanimstate.spineRightAndLeft, t.charanimstate.spineYAdjust, t.charanimstate.spineZAdjust);
			}
		}
	}
}

void GetPositionFromAnimFrameLimb (GGVECTOR3* pvecPos, sAnimation* pAnim, float fTime)
{
	DWORD dwKey = 0;
	DWORD dwKeyMax = pAnim->dwNumPositionKeys;
	int keyMin = 0;
	int keyMax = (int)dwKeyMax;
	int keyDiff = keyMax - keyMin;
	int keyCentre = (int)(keyMin + ((keyDiff) / 2.0));
	for (; keyDiff > 2;)
	{
		if (pAnim->pPositionKeys[keyCentre + 1].dwTime > fTime)
		{
			// the correct key is in the first half of the divided section
			keyMax = keyCentre;
		}
		else
		{
			// the correct key is in the second half of the divided section
			keyMin = keyCentre;
		}

		// subdivide
		keyDiff = keyMax - keyMin;
		keyCentre = (int)(keyMin + ((keyDiff) / 2.0));
	}
	if (keyMax >= (int)dwKeyMax) keyMax = dwKeyMax - 1;
	for (int i = keyMin; i <= keyMax; i++)
	{
		if (pAnim->pPositionKeys[i].dwTime <= fTime)
			dwKey = i;
		else
			break;
	}
	if (dwKey == (pAnim->dwNumPositionKeys - 1))
	{
		// use final frame in animation data
		(*pvecPos) = pAnim->pPositionKeys[dwKey].vecPos;
	}
	else
	{
		// calculate the time difference and interpolate time
		float fIntTime = fTime - pAnim->pPositionKeys[dwKey].dwTime;
		(*pvecPos) = pAnim->pPositionKeys[dwKey].vecPos + pAnim->pPositionKeys[dwKey].vecPosInterpolation * (float)fIntTime;
	}
}

bool AdjustPositionSoNoOverlap (int iEntityIndex, float* pX, float* pZ, float fOldX, float fOldZ)
{
	// if character itself does not repell, no overlap adjust needed
	int iEntID = t.entityelement[iEntityIndex].bankindex;
	if (iEntID > 0 && t.entityprofile[iEntID].collisionmode == 22)
		return true;

	bool bCanSafelyShiftXYZ = true;
	float fGapNeeded = 12.0f * 2.0f; // two radius for both characters
	float fCurrentDirX = *pX - fOldX;
	float fCurrentDirZ = *pZ - fOldZ;
	float fCurrentDist = sqrt(fabs(fCurrentDirX*fCurrentDirX) + fabs(fCurrentDirZ*fCurrentDirZ));
	if (fCurrentDist > 0.0f)
	{
		// scan all characters and ensure returned XYZ does not overlap any of them
		for (int tcharanimindex = 1; tcharanimindex <= g.charanimindexmax; tcharanimindex++)
		{
			int ee = t.charanimstates[tcharanimindex].e;
			if (ee > 0 && ee != iEntityIndex && t.entityelement[ee].health > 1 && t.entityelement[ee].active != 0 && t.entityelement[ee].eleprof.disableascharacter == 0 )
			{
				int entid = t.entityelement[ee].bankindex;
				if (entid > 0 && t.entityprofile[entid].collisionmode != 22)
				{
					float fThisX, fThisY, fThisZ;
					fThisX = t.entityelement[ee].x;
					fThisY = t.entityelement[ee].y;
					fThisZ = t.entityelement[ee].z;
					float fEEX = *pX - fThisX;
					float fEEY = t.entityelement[iEntityIndex].y - fThisY;
					float fEEZ = *pZ - fThisZ;
					float fDist = sqrt(fabs(fEEX*fEEX) + fabs(fEEY*fEEY) + fabs(fEEZ*fEEZ));
					if (fDist < fGapNeeded)
					{
						// the XYZ overlaps another entity position - shift back to origin until no more overlaps
						float fOutsideAngle = GGToDegree(atan2(fEEX, fEEZ));
						float fOutsideX = NewXValue(fThisX, fOutsideAngle, fGapNeeded);
						float fOutsideZ = NewZValue(fThisZ, fOutsideAngle, fGapNeeded);
						*pX = fOutsideX;
						*pZ = fOutsideZ;

						// before use this new position, ensure it does not leave the navmesh
						//if (1) // do not need this now - we can go anywhere - g_RecastDetour.isWithinNavMesh (*pX, t.entityelement[iEntityIndex].y, *pZ) == true) // arg - not quite, this can push entities THROUGH WALLS!!!
						float vecNearestPt[3];
						bool bMustBeOverPoly = true;
						if (g_RecastDetour.isWithinNavMeshEx (*pX, t.entityelement[iEntityIndex].y, *pZ, (float*)&vecNearestPt, bMustBeOverPoly) == true) // new XYZ only valid inside nav mesh!
						{
							// also ensure it does NOT overlap another
							for (int tcharanimindex2 = 1; tcharanimindex2 <= g.charanimindexmax; tcharanimindex2++)
							{
								int ee2 = t.charanimstates[tcharanimindex2].e;
								if (ee != iEntityIndex && ee2 != iEntityIndex && t.entityelement[ee2].eleprof.disableascharacter == 0)
								{
									float fThisX, fThisZ;
									fThisX = t.entityelement[ee2].x;
									fThisZ = t.entityelement[ee2].z;
									float fEEX = *pX - fThisX;
									float fEEZ = *pZ - fThisZ;
									float fDist = sqrt(fabs(fEEX*fEEX) + fabs(fEEZ*fEEZ));
									if (fDist < fGapNeeded)
									{
										// shifted into a neighbor, this move should not take place!
										bCanSafelyShiftXYZ = false;
										break;
									}
								}
							}
						}
						else
						{
							// shifted out of navmesh, this move should not take place!
							bCanSafelyShiftXYZ = false;
						}
						if (bCanSafelyShiftXYZ == false)
						{
							*pX = fOldX;
							*pZ = fOldZ;
							break;
						}
					}
				}
			}
		}
	}
	return bCanSafelyShiftXYZ;
}

void darkai_handlegotomove (void)
{
	// if any movement required, switch character into a mover (if never need to move, it can animate inplace with good foot planting)
	if (t.charanimstate.requiremovementnow == 0)
	{
		int iPointCount = t.charanimstate.pathPointCount;
		int iPointIndex = t.charanimstate.moveToMode;
		if ((iPointIndex > 0 && iPointCount > 0) || t.charanimstate.remainingMoveDistanceOnPath_f > 0)
		{
			t.charanimstate.requiremovementnow = 1;
		}
	}

	// first run the animation simlation to see if spine tracking movement occurs
	bool bObjectIsSpineTracked = false;
	int iID = t.charanimstate.obj;
	sObject* pObject = GetObjectData(iID);
	if (pObject )
	{
		// first handle any movewithanimation functionality (to get to distance we need the second part of the code to MOVE the object)
		if (pObject->ppFrameList)
		{
			if (pObject->dwSpineCenterLimbIndex > 0)
			{
				// we use animation for movement
				bObjectIsSpineTracked = true;

				// work out position offset of Bip01
				GGVECTOR3 vecBip01PosOffset;
				float fLastAnimFrame = pObject->fAnimLastFrame;
				float fAnimFrame = WickedCall_GetObjectFrame(pObject);
				pObject->fAnimLastFrame = fAnimFrame;
				pObject->fAnimFrame = fAnimFrame;
				if (fLastAnimFrame != -1)
				{
					sFrame* pFrame = pObject->ppFrameList[pObject->dwSpineCenterLimbIndex];
					if (pFrame)
					{
						// work out BIP01 shift since last anim
						GGVECTOR3 vecLastBip01PosOffset;
						GetPositionFromAnimFrameLimb (&vecLastBip01PosOffset, pFrame->pAnimRef, fLastAnimFrame);
						GetPositionFromAnimFrameLimb (&vecBip01PosOffset, pFrame->pAnimRef, fAnimFrame);
						vecBip01PosOffset = vecBip01PosOffset - vecLastBip01PosOffset;
						float fShiftSinceLastAnimX = vecBip01PosOffset.x;
						float fShiftSinceLastAnimZ = vecBip01PosOffset.z;

						// if animation loops back, invalidate any shift (as animation loop is resetting)
						bool bSupressTransitionToHideGlitchDuringLoop = false;
						if (pObject->bAnimLooping == true && fLastAnimFrame > fAnimFrame)
						{
							// ensure to massive backward movement delta when animation Bip01 pos resets
							// just as we deducted the length of anim time, deduct the full anim time offset (so matching the displacement)
							//fShiftSinceLastAnimX = 0.0f;
							//fShiftSinceLastAnimZ = 0.0f;
							// still not perfect as I think there is time displacement between multithread anim and core thread movemement!
							GGVECTOR3 vecFirstBip01PosOffsetFrame;
							GetPositionFromAnimFrameLimb (&vecFirstBip01PosOffsetFrame, pFrame->pAnimRef, pObject->fAnimLoopStart);
							GGVECTOR3 vecLastBip01PosOffsetFrame;
							GetPositionFromAnimFrameLimb (&vecLastBip01PosOffsetFrame, pFrame->pAnimRef, pObject->fAnimFrameEnd);
							GGVECTOR3 vecFullBip01PosOffsetDisplacement;
							vecFullBip01PosOffsetDisplacement = vecLastBip01PosOffsetFrame - vecFirstBip01PosOffsetFrame;
							fShiftSinceLastAnimX -= vecFullBip01PosOffsetDisplacement.x;
							fShiftSinceLastAnimZ -= vecFullBip01PosOffsetDisplacement.z;
							bSupressTransitionToHideGlitchDuringLoop = true;
						}

						// also some play animations cause huge backward shifts, so detect and eliminate ones too large (unnatural)
						if (fabs(fShiftSinceLastAnimZ) > 10.0f || fabs(fShiftSinceLastAnimX) > 10.0f)
						{
							fShiftSinceLastAnimX = 0.0f;
							fShiftSinceLastAnimZ = 0.0f;
							bSupressTransitionToHideGlitchDuringLoop = true;
						}

						// delta Z takes more time as it uses physics to shift the object, and we damped it here too in an accumilator
						if (bSupressTransitionToHideGlitchDuringLoop == false)
						{
							t.smoothanim[iID].movedeltax *= 0.8f;
							t.smoothanim[iID].movedeltaz *= 0.8f;
							if (fabs(fShiftSinceLastAnimX) + fabs(fShiftSinceLastAnimZ) > 0.0f)
							{
								t.smoothanim[iID].movedeltacontrib += 0.05f;
								if (t.smoothanim[iID].movedeltacontrib > 1.0f) t.smoothanim[iID].movedeltacontrib = 1.0f;
								t.smoothanim[iID].movedeltax += fShiftSinceLastAnimX * t.smoothanim[iID].movedeltacontrib;
								if (fShiftSinceLastAnimX > 0.0f)
								{
									if (t.smoothanim[iID].movedeltax > fShiftSinceLastAnimX) t.smoothanim[iID].movedeltax = fShiftSinceLastAnimX;
								}
								else
								{
									if (t.smoothanim[iID].movedeltax < fShiftSinceLastAnimX) t.smoothanim[iID].movedeltax = fShiftSinceLastAnimX;
								}
								t.smoothanim[iID].movedeltaz += fShiftSinceLastAnimZ * t.smoothanim[iID].movedeltacontrib;
								if (fShiftSinceLastAnimZ > 0.0f)
								{
									if (t.smoothanim[iID].movedeltaz > fShiftSinceLastAnimZ) t.smoothanim[iID].movedeltaz = fShiftSinceLastAnimZ;
								}
								else
								{
									if (t.smoothanim[iID].movedeltaz < fShiftSinceLastAnimZ) t.smoothanim[iID].movedeltaz = fShiftSinceLastAnimZ;
								}
							}
							else
							{
								t.smoothanim[iID].movedeltacontrib -= 0.01f;
								if (t.smoothanim[iID].movedeltacontrib < 0.0f)
									t.smoothanim[iID].movedeltacontrib = 0.0f;
							}
							// and store good deltas
							t.smoothanim[iID].lastmovedeltax = t.smoothanim[iID].movedeltax;
							t.smoothanim[iID].lastmovedeltaz = t.smoothanim[iID].movedeltaz;
						}
						else
						{
							// use known move deltas to hide any transition glitches
							t.smoothanim[iID].movedeltax = t.smoothanim[iID].lastmovedeltax;
							t.smoothanim[iID].movedeltaz = t.smoothanim[iID].lastmovedeltaz;
						}

						// special mode wipes out BIP01 from animation calc
						if (t.charanimstate.requiremovementnow == 1)
						{
							// activate and keep active (non-moving characters can still retain their foot planting if they are just animating in place)
							WickedCall_SetBip01Position(pObject, pFrame, 3, 0, 0);
						}
					}
				}
			}
		}
	}

	// if have movement
	bool bReversingOrStrafing = false;
	float fAdvanceTheMovement = 0.0f;
	if (bObjectIsSpineTracked == true )
	{
		// movement controlled by animation (spinetracking)
		if (t.charanimstate.movingbackward == 1)
		{
			// we are reversing movement logic (so can face forward, animate a backward anim and move along projected backward path vector)
			bReversingOrStrafing = true;
			fAdvanceTheMovement = t.smoothanim[iID].movedeltaz;
		}
		else
		{
			// normal forward movement
			fAdvanceTheMovement = -t.smoothanim[iID].movedeltaz;
		}
		if (fabs(t.smoothanim[iID].movedeltax) > fAdvanceTheMovement) bReversingOrStrafing = true; // strafing
		if (t.charanimstate.iRotationAlongPathMode == 0 ) bReversingOrStrafing = true; // called by behavior when want animation to perform without rotating to follow projected path
	}
	else
	{
		// regular specified forward movement if spline tracking not used
		if (t.charanimstate.movespeed_f > 0.0f)
		{
			fAdvanceTheMovement = t.charanimstate.movespeed_f;
		}
	}
	// special mode to slowly re-introduce ability to rotate to face path direction (used when anims need to stay facing X for strafes/backoffs/etc)
	if (t.charanimstate.iRotationAlongPathMode > 0 && t.charanimstate.iRotationAlongPathMode < 100)
	{
		t.charanimstate.iRotationAlongPathMode += 2;
		if (t.charanimstate.iRotationAlongPathMode > 100)
			t.charanimstate.iRotationAlongPathMode = 100;
	}
	// record old position
	float fOldPosX = t.entityelement[t.charanimstate.e].x;
	float fOldPosZ = t.entityelement[t.charanimstate.e].z;
	// if we are advancing a move, do it here
	if ( fAdvanceTheMovement > 0.0f )
	{
		// we need to move the object
		float fCurrentMoveDistance = fAdvanceTheMovement;
		int iPointCount = t.charanimstate.pathPointCount;
		int iPointIndex = t.charanimstate.moveToMode;
		if (iPointIndex > 0 && iPointCount > 0)
		{
			// reset until need it (at end of path traversal)
			t.charanimstate.remainingMoveDistanceOnPath_f = 0;
			t.charanimstate.remainingOverallDistanceToDest_f = 0;
			
			// eat through points until above movement distance used up
			float fDiffA = t.charanimstate.moveangle_f; // redundant, but assighned here to show cyclic nature of fDiffA
			float fCurrentX = t.entityelement[t.charanimstate.e].x;
			float fCurrentZ = t.entityelement[t.charanimstate.e].z;
			bool bEatPointsInPath = true;
			while (bEatPointsInPath == true)
			{
				float thisPoint[3] = { -1, -1, -1 };
				while (iPointIndex > 0)
				{
					// work out angle and distance in direction of next point
					thisPoint[0] = t.charanimstate.pointx[iPointIndex];
					thisPoint[2] = t.charanimstate.pointz[iPointIndex];
					float fDiffX = thisPoint[0] - fCurrentX;
					float fDiffZ = thisPoint[2] - fCurrentZ;
					float tdisttopoint = sqrt(fabs(fDiffX*fDiffX) + fabs(fDiffZ*fDiffZ));
					fDiffA = GGToDegree(atan2(fDiffX, fDiffZ));
					if (fDiffA < -180) fDiffA = fDiffA + 360;
					if (fDiffA > 180) fDiffA = fDiffA - 360;
					// also work out distance to final point in current path
					float fFinalPointX = t.charanimstate.pointx[iPointCount - 1];
					float fFinalPointY = t.charanimstate.pointy[iPointCount - 1];
					float fFinalPointZ = t.charanimstate.pointz[iPointCount - 1];
					float fFinalDiffX = fFinalPointX - fCurrentX;
					float fFinalDiffZ = fFinalPointZ - fCurrentZ;
					float fFinalDist = sqrt(fabs(fFinalDiffX*fFinalDiffX) + fabs(fFinalDiffZ*fFinalDiffZ));
					t.charanimstate.remainingOverallDistanceToDest_f = fFinalDist;
					float fStopAheadOfFinishingPositionBy = t.charanimstate.iStopFromEnd;// 10.0f;// 75.0f; //LB: would be good to scale this by genral movement speed (anim or non-anim based)
					bool bEndThisPathTraversal = false;
					if (fFinalDist < fStopAheadOfFinishingPositionBy)
					{
						// special mode allows future animations to travel on the path (but never exceed it less they go through walls)
						t.charanimstate.remainingMoveDistanceOnPath_f = fStopAheadOfFinishingPositionBy;
						bEndThisPathTraversal = true;
					}
					else
					{
						if (tdisttopoint >= fCurrentMoveDistance)
						{
							// we found a point that is beyond our remaining distance to travel
							// no more distance to traverse - keep remaining fCurrentMoveDistance for below
							bEatPointsInPath = false;
							break;
						}
						else
						{
							// next point is within the overall remaining distance, so eat it
							fCurrentMoveDistance -= tdisttopoint;
							fCurrentX = thisPoint[0];
							fCurrentZ = thisPoint[2];
							iPointIndex++;
							if (iPointIndex >= iPointCount)
							{
								bEndThisPathTraversal = true;
							}
						}
					}
					if (bEndThisPathTraversal == true )
					{
						bEatPointsInPath = false;
						fCurrentMoveDistance = 0.0f;
						t.charanimstate.movingbackward = 0;
						t.charanimstate.movespeed_f = 0.0f;
						iPointIndex = 0;
						break;
					}
				}
			}
			// if reached end of path, will NOT call this last movement
			if (fCurrentMoveDistance > 0.0f)
			{
				// set the final position along the path of latest points
				t.entityelement[t.charanimstate.e].x = NewXValue(fCurrentX, fDiffA, fCurrentMoveDistance);
				t.entityelement[t.charanimstate.e].z = NewZValue(fCurrentZ, fDiffA, fCurrentMoveDistance);
				// hard angle set for the movement along this last part
				t.charanimstate.moveangle_f = fDiffA; // redundant, we have already moved the XZ
				// scan ahead to a point further than 10 units away, and make that the look angle (so never spinny spinny over own XZ position)
				float fDistNow = 0.0f;
				float fLookAheadAngle = 0.0f;
				int iScanPointIndex = iPointIndex;
				while (fDistNow < 10.0f && iScanPointIndex < iPointCount)
				{
					float fDX = t.charanimstate.pointx[iScanPointIndex] - t.entityelement[t.charanimstate.e].x;
					float fDZ = t.charanimstate.pointz[iScanPointIndex] - t.entityelement[t.charanimstate.e].z;
					fDistNow = sqrt(fabs(fDX*fDX) + fabs(fDZ*fDZ));
					fLookAheadAngle = GGToDegree(atan2(fDX, fDZ));
					if (fLookAheadAngle < -180) fLookAheadAngle = fLookAheadAngle + 360;
					if (fLookAheadAngle > 180) fLookAheadAngle = fLookAheadAngle - 360;
					iScanPointIndex++;
				}
				if (fDistNow >= 10.0f)
				{
					// this commands a smooth rotation using slowly system (separate from move angle)
					if (bReversingOrStrafing == false)
					{
						// calculate the object rotation from path movement
						t.charanimstate.currentangle_f = fLookAheadAngle;
						float fModulateRotSpeed = t.charanimstate.iRotationAlongPathMode / 100.0f;
						t.charanimstate.currentangleslowlyspeed_f = (t.charanimstate.turnspeed_f * fModulateRotSpeed);
					}
					else
					{
						// leave last angle along if reversing during path follow (back off)
					}
				}
			}
			// set the updated point index
			t.charanimstate.moveToMode = iPointIndex;
			// reset Z movement so not to affect footplant object reposition below
			if (t.charanimstate.remainingMoveDistanceOnPath_f == 0)
			{
				// only when complete stop, not for when we completed path early allowing anim to continue to use up remaining path move distance
				fAdvanceTheMovement = 0.0f;
			}
		}
		else
		{
			if (t.charanimstate.remainingMoveDistanceOnPath_f > 0)
			{
				t.charanimstate.remainingMoveDistanceOnPath_f -= fabs(fAdvanceTheMovement);
				if (t.charanimstate.remainingMoveDistanceOnPath_f < 0)
				{
					t.charanimstate.remainingMoveDistanceOnPath_f = 0;
					fAdvanceTheMovement = 0.0f;
				}
			}
			else
			{
				// need this otherwise char uses pure anim move to go through walls, etc!!
				fAdvanceTheMovement = 0.0f;
			}
		}
	}
	// always resolve animation movement deltas
	if (t.charanimstate.requiremovementnow == 1)
	{
		GGVECTOR3 vecShift = GGVECTOR3(0, 0, 0);
		vecShift = GGVECTOR3(-t.smoothanim[iID].movedeltax, 0, 0); // prevents subtle shifts in Z from moving away from navmesh good position (zombies through fences)
		GGVec3TransformCoord(&vecShift, &vecShift, &pObject->position.matRotation);
		t.entityelement[t.charanimstate.e].x -= vecShift.x;
		t.entityelement[t.charanimstate.e].z -= vecShift.z;
	}
	t.smoothanim[iID].movedeltax = 0;
	t.smoothanim[iID].movedeltaz = 0;
	// ensure free of other characters
	if (AdjustPositionSoNoOverlap (t.charanimstate.e, &t.entityelement[t.charanimstate.e].x, &t.entityelement[t.charanimstate.e].z, fOldPosX, fOldPosZ) == true)
	{
		// no overlap, or was allowed to shift around another character safely (still in nav mesh)
		t.entityelement[t.charanimstate.e].lua.dynamicavoidance = 0;
		t.entityelement[t.charanimstate.e].lua.dynamicavoidancestuckclock = Timer();
	}
	else
	{
		// shift did not take place, character heading THROUGH another one
		// signal to AI so behavior can do something about walking through other people!
		t.entityelement[t.charanimstate.e].lua.dynamicavoidance = Timer() - t.entityelement[t.charanimstate.e].lua.dynamicavoidancestuckclock;
	}
	// special flag which interupts any path in progress
	if (t.entityelement[t.charanimstate.e].lua.interuptpath > 0)
	{
		t.entityelement[t.charanimstate.e].lua.dynamicavoidance = 1000;
		t.entityelement[t.charanimstate.e].lua.interuptpath--;
	}
	// find surface for object at this XZ position, faster than capsule and ensures object is purely navmesh/movement driven (not softy physics driven)
	bool bSurfaceFound = false;
	float fStepUp = 35.0f;
	float fStepDown = 50000.0f;
	int iCollisionMode = (1 << (0)) | (1 << (1)) | (1 << (3)); //COL_TERRAIN | COL_OBJECT | COL_OBJECT_DYNAMIC;
	float fSurfaceYPosition = -10000.0f;
	for (int iBoxScan = 0; iBoxScan < 4; iBoxScan++)
	{
		float fOffsetX = -1.0f;
		float fOffsetZ = -1.0f;
		if (iBoxScan == 1) { fOffsetX =  1.0f; fOffsetZ = -1.0f; }
		if (iBoxScan == 2) { fOffsetX = -1.0f; fOffsetZ =  1.0f; }
		if (iBoxScan == 3) { fOffsetX =  1.0f; fOffsetZ =  1.0f; }
		if (ODERayTerrainEx (t.entityelement[t.charanimstate.e].x + fOffsetX, t.entityelement[t.charanimstate.e].y + fStepUp, t.entityelement[t.charanimstate.e].z + fOffsetZ, t.entityelement[t.charanimstate.e].x + fOffsetX, t.entityelement[t.charanimstate.e].y - fStepDown, t.entityelement[t.charanimstate.e].z + fOffsetZ, iCollisionMode, false) == 1)
		{
			float fThisSurfaceY = ODEGetRayCollisionY();
			if (fThisSurfaceY > fSurfaceYPosition) fSurfaceYPosition = fThisSurfaceY;
			bSurfaceFound = true;
		}
	}
	if ( bSurfaceFound == false )
	{
		// probably on steep terrain, step up of 15 at high speed cannot handle this, so ensure LOWEST point is terrain 
		float fThisSurfaceY = BT_GetGroundHeight(0, t.entityelement[t.charanimstate.e].x, t.entityelement[t.charanimstate.e].z);
		if (fThisSurfaceY > fSurfaceYPosition) fSurfaceYPosition = fThisSurfaceY;
	}
	if (fSurfaceYPosition > -10000.0f)
	{
		float fYPosition = t.entityelement[t.charanimstate.e].y;
		float fDifference = fSurfaceYPosition - t.entityelement[t.charanimstate.e].y;
		if (fDifference < 0.0f)
		{
			// a drop, make object fall using gravity of 1G
			t.entityelement[t.charanimstate.e].climbgravity -= g.timeelapsed_f * 0.2f;
			if (t.entityelement[t.charanimstate.e].climbgravity < -50.0f) t.entityelement[t.charanimstate.e].climbgravity = -50.0f;
			fYPosition += g.timeelapsed_f * 6 * t.entityelement[t.charanimstate.e].climbgravity;
			if (fYPosition < fSurfaceYPosition) fYPosition = fSurfaceYPosition;
		}
		else
		{
			// walkable surface raises or stays level, set fNewYPosition to surface using a lerp curve, and reset to no gravity fall
			fYPosition += g.timeelapsed_f * fDifference * 0.5f;
			t.entityelement[t.charanimstate.e].climbgravity = 0.0f;
		}
		t.entityelement[t.charanimstate.e].y = fYPosition;
	}
	// tilt object X axis if tilt mode active (for non-bipeds like rats, horses)
	if ( t.charanimstate.iTiltMode == 1 )
	{
		float fX = t.entityelement[t.charanimstate.e].x;
		float fY = t.entityelement[t.charanimstate.e].y;
		float fZ = t.entityelement[t.charanimstate.e].z;
		float fNewY = fY;
		if (g_RecastDetour.isWithinNavMesh(fX, fY, fZ) == true)
		{
			fY = g_RecastDetour.getYFromPos(fX, fY, fZ);
		}
		else
		{
			fY = BT_GetGroundHeight(0, fX, fZ);
		}
		float fNewX = NewXValue(fX, t.charanimstate.currentangle_f, 5.0f);
		float fNewZ = NewZValue(fZ, t.charanimstate.currentangle_f, 5.0f);
		if (g_RecastDetour.isWithinNavMesh(fNewX, fY, fNewZ) == true)
		{
			fNewY = g_RecastDetour.getYFromPos(fNewX, fY, fNewZ);
		}
		else
		{
			fNewY = BT_GetGroundHeight(0, fNewX, fNewZ);
		}
		float fDX = fNewX - fX;
		float fDZ = fNewZ - fZ;
		float fDistXZ = sqrt(fabs(fDX*fDX) + fabs(fDZ*fDZ));
		float fDistY = fNewY - fY;
		float fTiltXAxis = LimbAngleX(iID, 0);
		float fTiltXAxisDest = GGToDegree(atan2(fDistY, fDistXZ));
		fTiltXAxis += (fTiltXAxisDest - fTiltXAxis)*0.1f;
		RotateLimb (iID, 0, fTiltXAxis, 0, 0);
	}
	// run physics to ensure capsule keeps up with world position of object (includes PositionObject for final XYZ placement)
	int stte = t.te;
	int sttv = t.tv_f;
	int stcharanimindex = g.charanimindex;
	t.te = t.charanimstate.e; t.tv_f = 0; g.charanimindex = 0;
	entity_updatepos();
	t.te = stte; t.tv_f = sttv; g.charanimindex = stcharanimindex;
	// final placement of object
	PositionObject (iID, t.entityelement[t.charanimstate.e].x, t.entityelement[t.charanimstate.e].y, t.entityelement[t.charanimstate.e].z);
}

void darkai_loop (void)
{
	// all characters in game
	g_bDormantCheckForThisCycle = true;
	DWORD dwCurrentTime = timeGetTime();
	for (g.charanimindex = 1; g.charanimindex <= g.charanimindexmax; g.charanimindex++)
	{
		// This character
		t.charanimstate = t.charanimstates[g.charanimindex];
		t.te = t.charanimstate.e;

		// handle updating of debug objects (cone of sight for characters)
		bool bCharIsActive = true;
		if (t.entityelement[t.te].active == 0) bCharIsActive = false;
		darkai_updatedebugobjects_forcharacter (bCharIsActive);

		// no longer part of the system
		if (t.entityelement[t.charanimstate.e].ragdollplusactivate > 0 )
			continue;

		// Lua calls darkai_calcplrvisible 2 times per char per sync, very expensive, only do one check.
		t.entityelement[t.charanimstate.e].bPlrVisibleCheckDone = false;

		// Entity Element Index for this A.I character
		t.i = t.charanimstate.obj;

		// ensure can stop looping sound ANY time
		t.ttsnd = t.charanimstate.firesoundindex;
		if (t.ttsnd > 0)
		{
			if (SoundExist(t.ttsnd) == 1)
			{
				if (Timer() > (int)t.charanimstate.firesoundexpiry)
				{
					StopSound (t.ttsnd);
				}
				if (SoundPlaying(t.ttsnd) == 0)
				{
					t.charanimstate.firesoundindex = 0;
				}
			}
		}

		// is entiy active here (and not disabled [as a character])
		if (t.entityelement[t.te].active == 1 && t.entityelement[t.te].eleprof.disableascharacter == 0)
		{
			// For valid A.I entities
			if (t.entityelement[t.charanimstate.e].health > 0)
			{
				// If in range for activity
				if (t.entityelement[t.charanimstate.e].plrdist < t.maximumnonefreezedistance || t.entityelement[t.charanimstate.e].eleprof.phyalways != 0)
				{
					// character can be flagged as back in range
					if (t.charanimstate.outofrange == 1)
					{
						t.entityelement[t.charanimstate.e].lua.outofrangefreeze = 0;
						t.charanimstate.outofrange = 0;
					}

					// active characters can be dormant, and awoken when they pass a line of sight check
					if (t.charanimstate.dormant == 1)
					{
						// infrequent check to awaken dormant AI
						if (dwCurrentTime > t.charanimstate.dormanttimer)
						{
							// only one check per cycle
							if (g_bDormantCheckForThisCycle == true)
							{
								t.charanimstate.dormanttimer = dwCurrentTime + 1000;
								int tthitvalue = 0;
								float fX = CameraPositionX(0);
								float fY = CameraPositionY(0);
								float fZ = CameraPositionZ(0);
								float fTargetPosX = t.entityelement[t.charanimstate.e].x;
								float fTargetPosY = t.entityelement[t.charanimstate.e].y + 65;// head height was 40;
								float fTargetPosZ = t.entityelement[t.charanimstate.e].z;
								float fDX = fTargetPosX - fX;
								float fDY = fTargetPosY - fY;
								float fDZ = fTargetPosZ - fZ;
								float fDistance = sqrt(fabs(fDX*fDX) + fabs(fDY*fDY) + fabs(fDZ*fDZ));
								if (fDistance < 500.0f || t.entityelement[t.charanimstate.e].eleprof.phyalways != 0 )
								{
									// closer than this, and wake up the character, player getting very close!
								}
								else
								{
									if (ODERayTerrain(fX, fY, fZ, fTargetPosX, fTargetPosY, fTargetPosZ, false) == 1) tthitvalue = -1;
									if (tthitvalue == 0) tthitvalue = ODERay (fX, fY, fZ, fTargetPosX, fTargetPosY, fTargetPosZ, (1 << 1));//COL_OBJECT);
								}
								if (tthitvalue == 0)
								{
									// released into game action - can see it!
									t.charanimstate.dormant = 0;

									// additionally, agro needs to alert other characters who are in proximity
									for ( int charanimindex2 = 1; charanimindex2 <= g.charanimindexmax; charanimindex2++)
									{
										int ee = t.charanimstates[charanimindex2].e;
										if (ee > 0 && t.entityelement[ee].active == 1 )
										{
											float fDX = t.entityelement[ee].x - t.entityelement[t.charanimstate.e].x;
											float fDY = t.entityelement[ee].y - t.entityelement[t.charanimstate.e].y;
											float fDZ = t.entityelement[ee].z - t.entityelement[t.charanimstate.e].z;
											float fDist = sqrt(fabs(fDX*fDX) + fabs(fDY*fDY) + fabs(fDZ*fDZ));
											if (fDist < t.entityelement[ee].eleprof.conerange)
											{
												t.charanimstates[charanimindex2].dormant = 0;
											}
										}
									}
								}
								g_bDormantCheckForThisCycle = false;
							}
						}
					}
					else
					{
						// character is active, in range, process mouth and head tracking
						if (g_ObjectList[t.charanimstate.obj]->iFrameCount >= 37) // prevent errors on old models
						{
							// apply frames over main animation for mouth phoneme and blinking
							darkai_mouthandheadtracking();

							// and handle any spine tracking
							darkai_spinetracking();
						}

						// if character has mobility
						if (t.entityelement[t.charanimstate.e].eleprof.isimmobile == 0)
						{
							// replaces old 'rotateYslowly' with better 'on-target' path following system
							darkai_handlegotomove();
						}
					}
				}
				else
				{
					// Character out of range
					if (t.charanimstate.outofrange == 0)
					{
						// freeze activity until back in operating range
						t.tte = t.charanimstate.e;
						t.tobj = t.charanimstate.obj;
						t.tentid = t.entityelement[t.charanimstate.e].bankindex;
						entity_resettodefaultanimation();
						// restart behavior (too many unknowns when stop LUA logic mid-sequence!)
						LuaSetFunction("UpdateEntityDebugger", 2, 0);
						LuaPushInt(t.charanimstate.e);
						LuaPushInt(3);
						LuaCall();
						// we will ensure when t.charanimstate.outofrange is ONE, no further logic processed
						t.entityelement[t.charanimstate.e].lua.outofrangefreeze = 1;
						t.charanimstate.outofrange = 1;
					}
				}
			}
		}
		else
		{
			// if not active, stop any fire sound
			t.ttsnd = t.charanimstate.firesoundindex;
			t.charanimstate.firesoundindex = 0;
			if (t.ttsnd > 0)
			{
				if (SoundExist(t.ttsnd) == 1)
				{
					StopSound (t.ttsnd);
				}
			}
		}

		// Handle character Animation Speed (allows for machine indie speeds with timeelapsed_f)
		if (t.charanimstate.dormant == 1)
		{
			if (t.charanimstate.obj > 0 && ObjectExist(t.charanimstate.obj) == 1)
			{
				// some characters have 'moving anims', so freeze these while dormant (setting speed to zero avoids interfering with anim choice and frame)
				SetObjectSpeed (t.charanimstate.obj, 0);
			}
		}
		else
		{
			float fPolarity = 1.0f;
			if (t.charanimstate.animationspeed_f >= 0.0f)
			{
				// only use reverse polarity if animationspeed is NOT negative
				if (GetSpeed(t.charanimstate.obj) < 0) fPolarity = -1; else fPolarity = 1;
			}
			#ifdef WICKEDENGINE
			t.tfinalspeed_f = t.entityelement[t.charanimstate.e].speedmodulator_f * t.charanimstate.animationspeed_f * fPolarity * 2.5f;
			#else
			t.tfinalspeed_f = t.entityelement[t.charanimstate.e].speedmodulator_f * t.charanimstate.animationspeed_f * fPolarity * 2.5f * g.timeelapsed_f;
			#endif
			SetObjectSpeed (t.charanimstate.obj, t.tfinalspeed_f);
		}

		// smoothing animations for this character
		char_loop();

		#ifndef WICKEDENGINE
		// Handle character removal
		if (t.entityelement[t.charanimstate.e].health <= 0 && t.charanimstate.timetofadeout > 0)
		{
			if (Timer() > t.charanimstate.timetofadeout)
			{
				t.txDist_f = ObjectPositionX(t.charanimstate.obj) - CameraPositionX(0);
				t.tzDist_f = ObjectPositionZ(t.charanimstate.obj) - CameraPositionZ(0);
				if (t.txDist_f * t.txDist_f + t.tzDist_f * t.tzDist_f > 500000)
				{
					if (GetInScreen(t.charanimstate.obj) == 0)
					{
						// disable ability to remove character from system if ALWAYS ACTIVE has been set (allows characters to respawn)
						if (t.entityelement[t.charanimstate.e].eleprof.phyalways == 0)
						{
							//darkai_character_remove ();
						}
					}
				}
			}
		}
		#endif

		//  Store any changes
		t.charanimstates[g.charanimindex] = t.charanimstate;
	}

	// manage AI sound events
	darkai_managesound();
}

void darkai_update (void)
{
}

void darkai_setupcharacter (void)
{
	// Entity profile index
	t.ttentid = t.entityelement[t.charanimstates[g.charanimindex].e].bankindex;

	// Setup character defaults
	t.charanimstates[g.charanimindex].realheadangley_f = 0.0;
	t.charanimstates[g.charanimindex].animationspeed_f = (65.0 / 100.0)*t.entityelement[t.charanimstates[g.charanimindex].e].eleprof.animspeed;
	t.charanimstates[g.charanimindex].outofrange = 0;
	t.charanimstates[g.charanimindex].dormant = 1;
	t.charanimstates[g.charanimindex].dormanttimer = timeGetTime() + (rand() % 1000);
	t.charanimstates[g.charanimindex].currentangle_f = t.entityelement[t.charanimstates[g.charanimindex].e].ry;
	t.charanimstates[g.charanimindex].moveangle_f = t.charanimstates[g.charanimindex].currentangle_f;
	t.charanimstates[g.charanimindex].moveToMode = 0;
	t.charanimstates[g.charanimindex].movetox_f = -1;
	t.charanimstates[g.charanimindex].movetoy_f = -1;
	t.charanimstates[g.charanimindex].movetoz_f = -1;
	t.charanimstates[g.charanimindex].iTiltMode = 0;
	t.charanimstates[g.charanimindex].iStopFromEnd = 10;	
	t.charanimstates[g.charanimindex].entityTarget = 0;
	t.charanimstates[g.charanimindex].neckAiming = 0.0f;
	t.charanimstates[g.charanimindex].spineAiming = 0.0f;
	t.charanimstates[g.charanimindex].iRotationAlongPathMode = 100;

	// movement and turn speeds (taken from entity properties in scenarios where start/move path not controlling move and turn speeds)
	t.charanimstates[g.charanimindex].movingbackward = 0;
	t.charanimstates[g.charanimindex].requiremovementnow = 0;
	t.charanimstates[g.charanimindex].movespeed_f = 1.0f;
	t.charanimstates[g.charanimindex].turnspeed_f = 10.0f;
	#ifdef WICKEDENGINE
	t.charanimstates[g.charanimindex].movespeed_f = (float)t.entityelement[t.charanimstates[g.charanimindex].e].eleprof.iMoveSpeed / 100.0f;
	t.charanimstates[g.charanimindex].turnspeed_f = (float)t.entityelement[t.charanimstates[g.charanimindex].e].eleprof.iTurnSpeed / 100.0f;
	#endif

	// setup head and spine tracker details	
	t.charanimstates[g.charanimindex].neckRightAndLeftLimit = t.entityprofile[t.ttentid].headspinetracker.headhlimit;
	t.charanimstates[g.charanimindex].neckRightAndLeftOffset = t.entityprofile[t.ttentid].headspinetracker.headhoffset;
	t.charanimstates[g.charanimindex].neckUpAndDownLimit = t.entityprofile[t.ttentid].headspinetracker.headvlimit;
	t.charanimstates[g.charanimindex].neckUpAndDownOffset = t.entityprofile[t.ttentid].headspinetracker.headvoffset;
	t.charanimstates[g.charanimindex].spineRightAndLeftLimit = t.entityprofile[t.ttentid].headspinetracker.spinehlimit;
	t.charanimstates[g.charanimindex].spineRightAndLeftOffset = t.entityprofile[t.ttentid].headspinetracker.spinehoffset;
	t.charanimstates[g.charanimindex].spineUpAndDownLimit = t.entityprofile[t.ttentid].headspinetracker.spinevlimit;
	t.charanimstates[g.charanimindex].spineUpAndDownOffset = t.entityprofile[t.ttentid].headspinetracker.spinevoffset;

	// By default, characters have default PISTOL weapon style OR new WEAPSTYLE value
	t.tgunid = t.entityelement[t.charanimstates[g.charanimindex].e].eleprof.hasweapon;
	if (t.entityprofile[t.ttentid].usesweapstyleanims == 0)
	{
		if (t.tgunid > 0)
		{
			t.charanimstates[g.charanimindex].weapstyle = 1;
		}
		else
		{
			t.charanimstates[g.charanimindex].weapstyle = 0;
		}
	}
	else
	{
		//  1-pistol, 2-rocket, 3-shotgun, 4-uzi, 5-assault, 51-meleenoammo
		t.charanimstates[g.charanimindex].weapstyle = t.gun[t.tgunid].weapontype;
		if (t.charanimstates[g.charanimindex].weapstyle > 5) t.charanimstates[g.charanimindex].weapstyle = 1;
	}

	// populate character with weapon details
	t.tgunid = t.entityelement[t.charanimstates[g.charanimindex].e].eleprof.hasweapon;
	t.charanimstates[g.charanimindex].ammoinclipmax = g.firemodes[t.tgunid][0].settings.reloadqty;
	t.charanimstates[g.charanimindex].ammoinclip = t.charanimstates[g.charanimindex].ammoinclipmax;
	if (t.charanimstates[g.charanimindex].ammoinclip > 0)
	{
		// allows characters to reload at different times
		t.charanimstates[g.charanimindex].ammoinclip = 1 + Rnd(t.charanimstates[g.charanimindex].ammoinclip - 1);
	}

	// Set collision property
	SetObjectCollisionProperty (t.charanimstates[g.charanimindex].obj, 0);

	// determine if character holds 'gun' or 'rocket' style weapon
	if (t.charanimstates[g.charanimindex].weapstyle <= 1)
	{
		//  only if older legacy character (newer Uber characters use weapstyle=2)
		t.charanimstates[g.charanimindex].rocketstyle = 0;
		if (t.tgunid > 0)
		{
			if (g.firemodes[t.tgunid][0].settings.flakindex > 0)
			{
				t.charanimstates[g.charanimindex].rocketstyle = 1;
			}
		}
	}

	//  Hard-Code ENEMY or NEUTRAL in entity profile (can be changed via LUA scripting)
	t.charanimstates[g.charanimindex].aiobjectexists = 1;

	// character speaking settings reset
	t.charanimstates[g.charanimindex].ccpo.speak.mouthData.clear();
	t.charanimstates[g.charanimindex].ccpo.speak.fMouthTimeStamp = 0.0f;
	t.charanimstates[g.charanimindex].ccpo.speak.iMouthDataShape = 0;
	t.charanimstates[g.charanimindex].ccpo.speak.iMouthDataIndex = 0;
	t.charanimstates[g.charanimindex].ccpo.speak.fSmouthDataSpeedToNextShape = 4.0f;
	t.charanimstates[g.charanimindex].ccpo.speak.fNeedToBlink = 0.0f;

	// must be full object to be a character
	int iStoreOBJ = t.obj;
	int iStoreENTID = t.tentid;
	int iStoreTTE = t.tte;
	t.obj = t.charanimstates[g.charanimindex].obj;
	t.tentid = t.entityelement[t.charanimstates[g.charanimindex].e].bankindex;
	t.tte = t.charanimstates[g.charanimindex].e; entity_converttoclone ();
	t.obj = iStoreOBJ; t.tentid = iStoreENTID; t.tte = iStoreTTE;

	// find neck bone for this base body model (can shift index)
	int iNeckBone = 0;
	PerformCheckListForLimbs(t.charanimstates[g.charanimindex].obj);
	for (int c = 1; c <= ChecklistQuantity(); c++)
		if (iNeckBone == 0 && (strstr (ChecklistString (c), "_Head") != NULL || strstr (ChecklistString (c), "_head") != NULL))
			iNeckBone = c - 1;
	t.charanimstates[g.charanimindex].ccpo.settings.iNeckBone = iNeckBone;

	// Create cone of sight debug object for character
	int e = t.charanimstates[g.charanimindex].e;
	float fConeAngle = t.entityelement[e].eleprof.coneangle;
	float fViewRange = t.entityelement[e].eleprof.conerange;
	#ifdef NEWMAXAISYSTEM
	int iDebugConeOfSightObj = g.debugconeofsightstart + g.charanimindex;
	if (iDebugConeOfSightObj < g.debugconeofsightfinish)
	{
		darkai_createinternaldebugvisuals_coneofsight (iDebugConeOfSightObj, fConeAngle, fViewRange);
	}
	#endif

	// always recreated as a regular character
	t.entityelement[t.charanimstates[g.charanimindex].e].eleprof.disableascharacter = 0;
}

void darkai_refresh_characters ( bool bScanForNewlySpawned )
{
	// required during level init and when new entities are spawned live
	for (t.e = 1; t.e <= g.entityelementlist; t.e++)
	{
		t.entid = t.entityelement[t.e].bankindex;
		if (t.entid > 0)
		{
			if (t.entityprofile[t.entid].ischaracter == 1 && t.entityelement[t.e].ragdollified == 0)
			{
				t.tobj = t.entityelement[t.e].obj;
				if (t.tobj > 0)
				{
					if (ObjectExist(t.tobj) == 1)
					{
						// but only if not already part of the char anim list
						bool bFound = false;
						for (int n = 1; n <= g.charanimindexmax; n++)
						{
							if (t.charanimstates[n].e == t.e)
							{
								bFound = true;
							}
						}
						if (bFound == false)
						{
							// Set up object one as character
							++g.charanimindexmax;
							g.charanimindex = g.charanimindexmax;
							Dim (t.charanimcontrols, g.charanimindexmax);
							Dim (t.charanimstates, g.charanimindexmax);
							Dim2(t.charactergunpose, g.charanimindexmax, 36);
							t.charanimstates[g.charanimindex].obj = t.tobj;
							t.charanimstates[g.charanimindex].e = t.e;
							t.charanimstates[g.charanimindex].originale = t.e;
							t.charanimstates[g.charanimindex].currentangle_f = t.entityelement[t.e].ry;
							t.entityelement[t.e].eleprof.disableascharacter = 0;
							darkai_setupcharacter ();
							for (t.i = 0; t.i <= 36; t.i++)
							{
								t.charactergunpose[g.charanimindex][t.i].x = 0;
								t.charactergunpose[g.charanimindex][t.i].y = 0;
								t.charactergunpose[g.charanimindex][t.i].z = 0;
							}
						}
						else
						{
							// if found, existing characters can be left along during a newly spawned scam
							if (bScanForNewlySpawned == true)
							{
								continue;
							}
						}

						// swap in animation override
						char pWeaponAnimFile[MAX_PATH];
						strcpy(pWeaponAnimFile, "");
						LPSTR pOverrideAnimSet = t.entityelement[t.e].eleprof.overrideanimset_s.Get();
						if (strlen(pOverrideAnimSet) > 1) // "" = default to weapon type, "-" = default to object anim
						{
							if (FileExist(pOverrideAnimSet))
								strcpy(pWeaponAnimFile, pOverrideAnimSet);
							else
								pOverrideAnimSet = NULL;
						}
						else
						{
							pOverrideAnimSet = NULL;
						}

						// force animation to weapon type if NOT "-" = default to object anim
						if (pOverrideAnimSet == NULL && strlen(t.entityelement[t.e].eleprof.overrideanimset_s.Get()) == 0)
						{
							// swap in animation for character base types if needed
							LPSTR animsystem_getweapontype (LPSTR, LPSTR);
							int gunid = t.entityelement[t.e].eleprof.hasweapon;
							LPSTR pWeaponHeld = animsystem_getweapontype(t.entityelement[t.e].eleprof.hasweapon_s.Get(), t.gun[gunid].animsetoverride.Get());
							LPSTR pGender = NULL;
							if (t.entityprofile[t.entid].characterbasetype == 0) pGender = "adult male";
							if (t.entityprofile[t.entid].characterbasetype == 1) pGender = "adult female";
							if (t.entityprofile[t.entid].characterbasetype == 2) pGender = "zombie male";
							if (t.entityprofile[t.entid].characterbasetype == 3) pGender = "zombie female";
							if (pGender != NULL)
							{
								if (t.entityprofile[t.entid].characterbasetype >= 0 && t.entityprofile[t.entid].characterbasetype <= 1)
								{
									sprintf(pWeaponAnimFile, "charactercreatorplus\\parts\\%s\\default animations%s.dbo", pGender, pWeaponHeld);
								}
							}
						}
						if (FileExist(pWeaponAnimFile))
						{
							// replace actual object animations
							sObject* pObject = GetObjectData(t.tobj);
							AppendObject(pWeaponAnimFile, t.tobj, 0);
							WickedCall_RefreshObjectAnimations(pObject, pObject->wickedloaderstateptr);
						}
					}
				}
			}
		}
	}
}

void darkai_setup_characters (void)
{
	// for MAX, we fully reset any previous character states so this can be fresh
	for (int n = 1; n <= g.charanimindexmax; n++)
	{
		t.charanimstates[n].e = 0;
	}
	g.charanimindexmax = 0;

	// Create A.I entities for all characters
	darkai_refresh_characters(false);
}

void darkai_killai (void)
{
	if (t.charanimstates[t.tcharanimindex].aiobjectexists == 1)
	{
		// not supporting this method in MAX - stay in the main script!!
		// Attempt to call the _exit function for the characters script
		//if (t.entityelement[t.charanimstates[t.tcharanimindex].e].eleprof.aimain == 1)
		//{
		//	t.strwork = Lower(t.entityelement[t.charanimstates[t.tcharanimindex].e].eleprof.aimainname_s.Get());
		//	t.strwork += "_exit";
		//	LuaSetFunction (t.strwork.Get(), 1, 0);
		//	LuaPushInt (t.charanimstates[t.tcharanimindex].e); LuaCallSilent ();
		//}

		// free this AI from the game loop
		t.charanimstates[t.tcharanimindex].aiobjectexists = 0;
		if (t.entityelement[t.charanimstates[t.tcharanimindex].e].usingphysicsnow != 0)
		{
			t.tphyobj = t.charanimstates[t.tcharanimindex].obj; physics_disableobject ();
			t.entityelement[t.charanimstates[t.tcharanimindex].e].usingphysicsnow = 0;
		}
		SetObjectCollisionProperty (t.charanimstates[t.tcharanimindex].obj, 1);
	}

	// reset any limbs of character
	if (t.entityelement[t.charanimstates[t.tcharanimindex].e].health > 0)
	{
		t.headlimbofcharacter = t.entityprofile[t.entityelement[t.charanimstates[t.tcharanimindex].e].bankindex].headlimb;
		if (t.headlimbofcharacter > 0)
		{
			if (LimbExist(t.charanimstates[t.tcharanimindex].obj, t.headlimbofcharacter) == 1)
			{
				RotateLimb (t.charanimstates[t.tcharanimindex].obj, t.headlimbofcharacter, 0, 0, 0);
			}
		}
		t.spinelimbofcharacter = t.entityprofile[t.entityelement[t.charanimstates[t.tcharanimindex].e].bankindex].spine;
		if (t.spinelimbofcharacter > 0)
		{
			if (LimbExist(t.charanimstates[t.tcharanimindex].obj, t.spinelimbofcharacter) == 1)
			{
				RotateLimb (t.charanimstates[t.tcharanimindex].obj, t.spinelimbofcharacter, 0, 0, 0);
			}
		}
	}

	// stop any animations (in case we need to ragdoll)
	StopObject (t.charanimstates[t.tcharanimindex].obj);

	// reset any looping/sounds
	t.ttsnd = t.charanimstates[t.tcharanimindex].firesoundindex;
	t.charanimstates[t.tcharanimindex].firesoundindex = 0;
	if (t.ttsnd > 0)
	{
		if (SoundExist(t.ttsnd) == 1)
		{
			StopSound (t.ttsnd);
		}
	}
	#ifdef WICKEDENGINE
	// additionally any sounds triggered by this entity
	int e = t.charanimstates[t.tcharanimindex].e;
	for (int s = 0; s < 6; s++)
	{
		int ttsnd = 0;
		if (s == 0) ttsnd = t.entityelement[e].soundset;
		if (s == 1) ttsnd = t.entityelement[e].soundset2;
		if (s == 2) ttsnd = t.entityelement[e].soundset3;
		//if (s == 3) ttsnd = t.entityelement[e].soundset4;
		if (s == 4) ttsnd = t.entityelement[e].soundset5;
		if (s == 5) ttsnd = t.entityelement[e].soundset6;
		if (ttsnd > 0)
		{
			if (SoundExist(ttsnd) == 1)
			{
				StopSound (ttsnd);
			}
		}
	}
	#endif

}

// Smooth Anim System

void char_init (void)
{
	// Create array to hold transition information for per-object
	t.tmaxobjectnumber = 90000;
	Dim (t.smoothanim, t.tmaxobjectnumber);
}

void char_loop (void)
{
	// Update anim system for smoothing (or transitions if wicked)
	smoothanimupdate (t.charanimstate.obj);

	// Ensure object can be smoothly rotated from LUA instruction
	if (t.charanimstate.currentangleslowlyspeed_f != 0.0f)
	{
		// preserve t.e and t.obj
		int ste = t.e, stobj = t.obj;
		t.e = t.charanimstate.e;

		// get dest angle
		float fDestAngle = t.charanimstate.currentangle_f;

		// need to factor in entity speed for characters
		t.tsmooth_f = (100.0 / (t.charanimstate.currentangleslowlyspeed_f)) / g.timeelapsed_f;
		t.tsmooth_f /= (t.entityelement[t.charanimstate.e].eleprof.speed / 100.0f);

		// smoothly rotate to destination angle and update object
		t.entityelement[t.e].ry = CurveAngle(fDestAngle, t.entityelement[t.e].ry, t.tsmooth_f);
		entity_lua_rotateupdate ();

		// when reach destination can close this system down
		if (fabs(fDestAngle - t.entityelement[t.e].ry) < 0.1f) t.charanimstate.currentangleslowlyspeed_f = 0.0f;

		// restore t.e
		t.e = ste; t.obj = stobj;
	}
}

void darkai_resetsmoothanims (void)
{
	for (int n = 0; n < t.tmaxobjectnumber; n++)
	{
		t.smoothanim[n].fn = 0;
		t.smoothanim[n].playflag = 0;
		t.smoothanim[n].playstarted = 0;
		t.smoothanim[n].rev = 0;
		t.smoothanim[n].st = 0;
		t.smoothanim[n].transition = 0;
		t.smoothanim[n].movedeltacontrib = 0;
		t.smoothanim[n].movedeltax = 0;
		t.smoothanim[n].movedeltaz = 0;
		t.smoothanim[n].startat = 0;
		t.smoothanim[n].usefulTimer = 0;
	}
}

void smoothanimtriggerrev (int obj, float st, float fn, int speedoftransition, int rev, int playflag, float fStartFromPercentage)
{
	// transition to the start of the loop frame
	if (t.smoothanim[obj].st != st)
	{
		StopObject (obj);
		#ifdef WICKEDENGINE
		SetObjectInterpolation (obj, speedoftransition);
		#else
		SetObjectInterpolation (obj, 100.0 / speedoftransition);
		#endif
		if (rev == 1)
		{
			SetObjectFrame (obj, fn);
		}
		else
		{
			SetObjectFrame (obj, st);
		}
		t.smoothanim[obj].st = st;
		t.smoothanim[obj].fn = fn;
		t.smoothanim[obj].rev = rev;
		t.smoothanim[obj].playflag = playflag;
		t.smoothanim[obj].playstarted = 0;
		#ifdef WICKEDENGINE
		// transitions handled differently with MAX, we control a lerp factor that handles transitions
		// nicely within the Wicked animation system
		t.smoothanim[obj].transition = 1;
		#else
		t.smoothanim[obj].transition = speedoftransition;
		#endif

		#ifdef WICKEDENGINE
		// affect starting frame if specified
		float fThisAnimLength = fn - st;
		t.smoothanim[obj].startat = 0;
		if (fStartFromPercentage > 0.0f)
		{
			float fStartFrame = st;
			fStartFrame += (fThisAnimLength / 100.0f)*fStartFromPercentage;
			t.smoothanim[obj].startat = fStartFrame;
		}
		#endif
	}
}

void smoothanimtrigger (int obj, float st, float fn, int speedoftransition)
{
	smoothanimtriggerrev(obj, st, fn, speedoftransition, 0, 0, 0);
}

void smoothanimupdate (int obj)
{
	if (t.smoothanim[obj].transition > 0)
	{
		t.smoothanim[obj].transition = t.smoothanim[obj].transition - 1;
		if (t.smoothanim[obj].transition == 0)
		{
			#ifdef WICKEDENGINE
			// for MAX we operate a smooth transition system
			SetObjectInterpolation (obj, 1.0);
			#else
			SetObjectInterpolation (obj, 100.0);
			#endif
			if (t.smoothanim[obj].playflag == 1)
			{
				if (t.smoothanim[obj].playstarted == 0)
				{
					PlayObject (obj, t.smoothanim[obj].st, t.smoothanim[obj].fn);
					if (t.smoothanim[obj].rev == 0)
					{
						if (t.smoothanim[obj].startat > 0)
						{
							#ifdef WICKEDENGINE
							sObject* pObject = GetObjectData(obj);
							WickedCall_SetObjectFrameEx(pObject, t.smoothanim[obj].startat);
							#endif
							t.smoothanim[obj].startat = 0;
						}
						SetObjectSpeed (obj, abs(GetSpeed(obj)));
					}
					else
					{
						SetObjectSpeed (obj, abs(GetSpeed(obj))*-1);
						SetObjectFrame (obj, t.smoothanim[obj].fn);
					}
					t.smoothanim[obj].playstarted = 1;
				}
			}
			else
			{
				LoopObject ( obj, t.smoothanim[obj].st, t.smoothanim[obj].fn);
				if (t.smoothanim[obj].startat > 0)
				{
					#ifdef WICKEDENGINE
					sObject* pObject = GetObjectData(obj);
					WickedCall_SetObjectFrameEx(pObject, t.smoothanim[obj].startat);
					#endif
					t.smoothanim[obj].startat = 0;
				}
				if (t.smoothanim[obj].rev == 0)
				{
					SetObjectSpeed (obj, abs(GetSpeed(obj)));
				}
				else
				{
					SetObjectSpeed (obj, abs(GetSpeed(obj))*-1);
				}
			}
		}
	}
	else
	{
		#ifdef WICKEDENGINE
		if (t.smoothanim[obj].playflag == 1)
		{
			if (t.smoothanim[obj].playstarted == 1)
			{
				sObject* pObject = GetObjectData(obj);
				float fCurrentFrame = WickedCall_GetObjectFrame(pObject);
				if (fCurrentFrame >= t.smoothanim[obj].fn)
				{
					StopObject(obj);
					fCurrentFrame = t.smoothanim[obj].fn;
					WickedCall_SetObjectFrameEx(pObject, fCurrentFrame);
				}
			}
		}
		#endif
	}
}

int darkai_canshoot (void)
{
	// takes tcharanimindex, takes charanimstate
	int iCanShootNow = 0;

	// if target player, consider
	if (t.charanimstate.entityTarget == 0)
	{
		// recalc PLRVISIBLE to ensure the enemy can truly STILL see PLAYER
		if (t.entityelement[t.charanimstate.e].bPlrVisibleCheckDone == false)
		{
			darkai_calcplrvisible();
			t.entityelement[t.charanimstate.e].bPlrVisibleCheckDone = true;
		}
	}

	// if want to shoot, can override firesound in use (otherwise can wait 7 seconds while sound fades)
	t.tpermitanoverride = 0;
	if (t.charanimstate.firesoundindex > 0 && Timer() > (int)t.charanimstate.firesoundstarted + 50)  t.tpermitanoverride = 1;
	bool bProceed = false;
	if (t.charanimstate.entityTarget == 0 && ((t.charanimstate.firesoundindex == 0 || t.tpermitanoverride == 1) && t.entityelement[t.charanimstate.e].plrvisible == 1)) bProceed = true;
	if (t.charanimstate.entityTarget > 0  && ((t.charanimstate.firesoundindex == 0 || t.tpermitanoverride == 1)) ) bProceed = true;
	if (bProceed == true)
	{
		// handle player being shot at
		t.te = t.charanimstate.e;
		if (t.te > 0)
		{
			t.tentid = t.entityelement[t.te].bankindex;
			t.tgunid = t.entityelement[t.te].eleprof.hasweapon;
			t.tcannotfirenow = 0;
			t.tattachedobj = t.entityelement[t.te].attachmentobj;
			if (t.tattachedobj > 0)
			{
				// cannot fire if weapon not pointing at player
				t.tattachmentobjfirespotlimb = t.entityelement[t.te].attachmentobjfirespotlimb;
				if (t.tgunid > 0 && t.tattachmentobjfirespotlimb > 0)
				{
					float fTargetX = ObjectPositionX(t.aisystem.objectstartindex);
					float fTargetY = ObjectPositionY(t.aisystem.objectstartindex);
					float fTargetZ = ObjectPositionZ(t.aisystem.objectstartindex);
					int ee = t.charanimstate.entityTarget;
					if (ee > 0)
					{
						fTargetX = t.entityelement[ee].x;
						fTargetY = t.entityelement[ee].y;
						fTargetZ = t.entityelement[ee].z;
					}
					t.tx_f = LimbPositionX(t.tattachedobj, t.tattachmentobjfirespotlimb) - fTargetX;
					t.ty_f = LimbPositionY(t.tattachedobj, t.tattachmentobjfirespotlimb) - fTargetY;
					t.tz_f = LimbPositionZ(t.tattachedobj, t.tattachmentobjfirespotlimb) - fTargetZ;
					t.tdist_f = Sqrt(abs(t.tx_f*t.tx_f) + abs(t.ty_f*t.ty_f) + abs(t.tz_f*t.tz_f));
					if (ObjectExist(g.projectorsphereobjectoffset) == 0)
					{
						MakeObjectSphere (g.projectorsphereobjectoffset, 10);
						HideObject (g.projectorsphereobjectoffset);
					}
					PositionObject (g.projectorsphereobjectoffset, LimbPositionX(t.tattachedobj, t.tattachmentobjfirespotlimb), LimbPositionY(t.tattachedobj, t.tattachmentobjfirespotlimb), LimbPositionZ(t.tattachedobj, t.tattachmentobjfirespotlimb));
					RotateObject (g.projectorsphereobjectoffset, LimbDirectionX(t.tattachedobj, t.tattachmentobjfirespotlimb), LimbDirectionY(t.tattachedobj, t.tattachmentobjfirespotlimb), LimbDirectionZ(t.tattachedobj, t.tattachmentobjfirespotlimb));
					MoveObject (g.projectorsphereobjectoffset, t.tdist_f*-1);
					t.tx_f = ObjectPositionX(g.projectorsphereobjectoffset) - fTargetX;
					t.ty_f = ObjectPositionY(g.projectorsphereobjectoffset) - fTargetY;
					t.tz_f = ObjectPositionZ(g.projectorsphereobjectoffset) - fTargetZ;
					t.tdist2_f = Sqrt(abs(t.tx_f*t.tx_f) + abs(t.ty_f*t.ty_f) + abs(t.tz_f*t.tz_f));
					t.tactualdistance_f = t.tdist2_f;
					t.tdist2_f = t.tdist2_f / t.tdist_f;
					t.tdist2_f = int(t.tdist2_f * 100);
					if (t.tdist2_f > 50 && t.tactualdistance_f > 100.0)  t.tcannotfirenow = 1;
				}
			}
			if (t.tgunid > 0 && t.tcannotfirenow == 0)
			{
				// frequenty of fire
				t.ttratecalc_f = (1.0 / (1.0 + g.firemodes[t.tgunid][0].settings.firerate))*g.timeelapsed_f*2.0;
				t.ttratecalc_f *= 30.0f;// 10.0f; // increase rate of fire for MAX
				t.charanimstate.firerateaccumilator = t.charanimstate.firerateaccumilator - t.ttratecalc_f;
				if (t.charanimstate.firerateaccumilator < 0.0)
				{
					t.charanimstate.firerateaccumilator = 0.5 + (Rnd(100) / 100.0);
					iCanShootNow = 1;
				}
			}
		}
	}

	// can we fire now
	return iCanShootNow;
}

void darkai_shoottarget (int targete)
{
	// targete not userd as such, reads from t.charanimstate.entityTarget in darkai_shooteffect
	t.te = t.charanimstate.e;
	t.tentid = t.entityelement[t.te].bankindex;
	if (t.tentid > 0)
	{
		t.tgunid = t.entityelement[t.te].eleprof.hasweapon;
		if (t.tgunid > 0)
		{
			t.ttrr = Rnd(1);
			for (t.tt = t.ttrr + 0; t.tt <= t.ttrr + 1; t.tt++)
			{
				t.ttsnd = t.gunsoundcompanion[t.tgunid][1][t.tt].soundid;
				if (t.ttsnd > 0)
				{
					if (SoundExist(t.ttsnd) == 1)
					{
						if (SoundPlaying(t.ttsnd) == 0 || t.tt == t.ttrr + 1)
						{
							t.toldsndid = t.charanimstate.firesoundindex;
							if (t.toldsndid > 0)
							{
								if (SoundExist(t.toldsndid) == 1)
								{
									StopSound (t.toldsndid);
								}
							}

							#ifdef WICKEDENGINE
							t.charanimstate.firesoundindex = t.ttsnd; t.tt = 3;
							t.tfireloopend = g.firemodes[t.tgunid][0].sound.fireloopend;
							t.charanimstate.firesoundstarted = Timer();
							if (t.tfireloopend > 0)
							{
								// sound loops (need to cap it off)
								t.charanimstate.firesoundexpiry = Timer() + 200 + Rnd(200);
							}
							else
							{
								if (t.tfireloopend < 0)
								{
									// when fireloop is negative, we use 'single instance' shots
									// and use negative value as MS time between instance plays
									// need to simulate how player weapon works for EA for now
									t.charanimstate.firesoundexpiry = Timer() + 500;// (fabs(t.tfireloopend));
								}
								else
								{
									// can let sound fade out slowly naturally
									t.charanimstate.firesoundexpiry = Timer() + 5000;
								}
							}
							#else
							t.charanimstate.firesoundindex = t.ttsnd; t.tt = 3;
							t.tfireloopend = g.firemodes[t.tgunid][0].sound.fireloopend;
							t.charanimstate.firesoundstarted = Timer();
							if (t.tfireloopend > 0)
							{
								// sound loops (need to cap it off)
								t.charanimstate.firesoundexpiry = Timer() + 200 + Rnd(200);
							}
							else
							{
								// can let sound fade out slowly naturally
								t.charanimstate.firesoundexpiry = Timer() + 5000;
							}
							#endif
						}
					}
				}
			}
			if (t.charanimstate.firesoundindex > 0)
			{
				// shoot effects
				t.tattachedobj = t.entityelement[t.te].attachmentobj;
				darkai_shooteffect ();
			}
		}
	}
}

void darkai_shootplayer (void)
{
	darkai_shoottarget(0);
}

void darkai_shooteffect (void)
{
	// needs tgunid, example; tgunid=entityprofile(tentid).hasweapon
	// needs tattachedobj, example; tattachedobj=entityelement(te).attachmentobj
	// needs te (entityelement index), example; te = e
	// charanimstate.firesoundindex needs to be set, examle; charanimstate.firesoundindex=ttsnd
	// t.charanimstate.entityTarget also used

	// target position
	bool bMuzzleFlashIfPlrTarget = true;
	int ee = t.charanimstate.entityTarget;
	if (ee  > 0)
	{
		t.tplayerx_f = t.entityelement[ee].x;
		t.tplayery_f = t.entityelement[ee].y;
		t.tplayerz_f = t.entityelement[ee].z;
		//bMuzzleFlashIfPlrTarget = false; //MD: Users were complaining that AI doesn't emit muzzle flash while shooting
	}
	else
	{
		t.tplayerx_f = ObjectPositionX(t.aisystem.objectstartindex);
		t.tplayery_f = ObjectPositionY(t.aisystem.objectstartindex);
		t.tplayerz_f = ObjectPositionZ(t.aisystem.objectstartindex);
	}

	// emit spot flash
	if (t.tattachedobj > 0)
	{
		// best coordinate is firespot on attached gun
		t.tokay = 0;
		t.tattachmentobjfirespotlimb = t.entityelement[t.te].attachmentobjfirespotlimb;
		if (t.tgunid > 0 && t.tattachmentobjfirespotlimb > 0)
		{
			t.tx_f = LimbPositionX(t.tattachedobj, t.tattachmentobjfirespotlimb);
			t.ty_f = LimbPositionY(t.tattachedobj, t.tattachmentobjfirespotlimb);
			t.tz_f = LimbPositionZ(t.tattachedobj, t.tattachmentobjfirespotlimb);
			t.tokay = 1;
		}
		if (t.tokay == 0)
		{
			//  actual gun position is better source coordinate
			t.tx_f = ObjectPositionX(t.tattachedobj);
			t.ty_f = ObjectPositionY(t.tattachedobj);
			t.tz_f = ObjectPositionZ(t.tattachedobj);
		}
	}
	else
	{
		// fallback is entity center
		t.tobj = t.entityelement[t.te].obj;
		t.tx_f = ObjectPositionX(t.tobj);
		t.ty_f = ObjectPositionY(t.tobj) + 50.0;
		t.tz_f = ObjectPositionZ(t.tobj);
	}
	t.tcolr = g.firemodes[t.entityelement[t.te].eleprof.hasweapon][0].settings.muzzlecolorr / 5;
	t.tcolg = g.firemodes[t.entityelement[t.te].eleprof.hasweapon][0].settings.muzzlecolorg / 5;
	t.tcolb = g.firemodes[t.entityelement[t.te].eleprof.hasweapon][0].settings.muzzlecolorb / 5;
	lighting_spotflash_forenemies ();

	// initiate decal
	if (bMuzzleFlashIfPlrTarget == true)
	{
		t.decalid = g.firemodes[t.tgunid][0].decalid;
		g.decalx = t.tx_f; g.decaly = t.ty_f; g.decalz = t.tz_f;
		t.decalscalemodx = 0; t.decalorient = 11;
		t.originatore = -1;
		t.originatorobj = t.tattachedobj;
		#ifdef WICKEDENGINE
		// if gunspec does not specify decal forward, apply some so we can see the muzzle flash for characters!
		t.decalforward = g.firemodes[t.tgunid][0].settings.decalforward;
		if (t.decalforward == 0) t.decalforward = 100.0f;
		t.decalforward = t.decalforward * 2.0f;
		#else
		t.decalforward = g.firemodes[t.tgunid][0].settings.decalforward;
		#endif
		if (g.firemodes[t.tgunid][0].action.automatic.s > 0)
		{
			// special instruction for decal to loop X times
			#ifdef WICKEDENGINE
			t.decalburstloop = 0;
			#else
			t.decalburstloop = 4;
			#endif // WICKEDENGINE
		}
		else
		{
			t.decalburstloop = 0;
		}
		decalelement_create ();
		t.decalburstloop = 0;
	}

	// emit sound
	#ifdef WICKEDENGINE
	// a better system is to create the event between shooter and target, bringing alert position closer to combat
	t.tsx_f = t.entityelement[t.te].x;
	t.tsy_f = t.entityelement[t.te].y;
	t.tsz_f = t.entityelement[t.te].z;
	float fDX = t.tplayerx_f - t.tsx_f;
	float fDY = t.tplayery_f - t.tsy_f;
	float fDZ = t.tplayerz_f - t.tsz_f;
	float fDIst = sqrt(fabs(fDX*fDX) + fabs(fDY*fDY) + fabs(fDZ*fDZ));
	fDX /= 2;
	fDY /= 2;
	fDZ /= 2;
	t.tsx_f += fDX;
	t.tsy_f += fDY;
	t.tsz_f += fDZ;
	t.tradius_f = fDIst;
	if (t.tradius_f < 500) t.tradius_f = 500;
	#else
	t.tsx_f = t.entityelement[t.te].x;
	t.tsy_f = t.entityelement[t.te].y;
	t.tsz_f = t.entityelement[t.te].z;
	t.tradius_f = 200;
	#endif
	darkai_makesound ();
	t.ttsnd = t.charanimstate.firesoundindex;
	if (t.ttsnd > 0)
	{
		if (SoundExist(t.ttsnd) == 1)
		{
			#ifdef WICKEDENGINE
			t.tfireloopend = 0;
			#else
			t.tfireloopend = g.firemodes[t.tgunid][0].sound.fireloopend;
			#endif // WICKEDENGINE

			if (t.tfireloopend > 0)
			{
				PlaySoundOffset (t.ttsnd, t.tfireloopend); 
				LoopSound (t.ttsnd, 0, t.tfireloopend);
			}
			else
			{
				if (t.tfireloopend < 0)
				{
					// need to simulate how player rifle works for EA for now
					LoopSound (t.ttsnd, 0, 5000);
				}
				else
				{
					PlaySound (t.ttsnd);
				}
			}
			PositionSound (t.ttsnd, t.entityelement[t.te].x, t.entityelement[t.te].y, t.entityelement[t.te].z);
			t.tvolume_f = soundtruevolume(95.0);
			SetSoundVolume (t.ttsnd, t.tvolume_f);
			SetSoundSpeed (t.ttsnd, 43000 + Rnd(2000));
		}
	}

	// is bullet or flak
	t.tflakid = g.firemodes[t.tgunid][0].settings.flakindex;
	if (t.tflakid == 0)
	{
		// BULLET - determine if bullet hit based on distance (ttdistanceaccuracy# lower is better)
		t.ttdx_f = t.tplayerx_f - t.tx_f;
		t.ttdy_f = t.tplayery_f - t.ty_f;
		t.ttdz_f = t.tplayerz_f - t.tz_f;
		t.ttdd_f = Sqrt(abs(t.ttdx_f*t.ttdx_f) + abs(t.ttdy_f*t.ttdy_f) + abs(t.ttdz_f*t.ttdz_f));
		t.ttdistanceaccuracy_f = t.ttdd_f / 800.0;
		if (t.ttentid > 0)  t.tisnotmpchar = t.entityprofile[t.ttentid].ismultiplayercharacter; else t.tisnotmpchar = 0;
		if (t.aisystem.playerducking == 1)
		{
			if (t.playercontrol.movement == 0)
			{
				t.tchancetohit_f = 4.0;
			}
			else
			{
				t.tchancetohit_f = 12.0;
			}
		}
		else
		{
			if (t.playercontrol.movement == 0)
			{
				t.tchancetohit_f = 2.0;
			}
			else
			{
				t.tchancetohit_f = 6.0;
			}
		}
		// amount of damage to player
		t.tdamage = g.firemodes[t.tgunid][0].settings.damage;
		if (ee > 0)
		{
			// another character is target
			t.ttte = ee;
			t.tdamageforce = 0;
			t.tdamagesource = 1;
			entity_applydamage ();

			// create either material decal specified in FPE or blood decal
			if (t.entityelement[ee].health > 10)
			{
				int entid = t.entityelement[ee].bankindex;
				t.tttriggerdecalimpact = 2;
				if (t.entityprofile[entid].bloodscorch == 0)
				{
					if (t.entityprofile[entid].decalmax > 0 && t.entitydecal[entid][0] > 0)
					{
						t.tttriggerdecalimpact = 1;
						t.decalglobal.impactid = t.entitydecal[entid][0];
					}
				}
				float fTorseAreaX = t.entityelement[ee].x;
				float fTorseAreaY = t.entityelement[ee].y+50;
				float fTorseAreaZ = t.entityelement[ee].z;
				int torselimbindex = t.entityprofile[entid].spine2;
				if (torselimbindex > 0)
				{
					fTorseAreaX = LimbPositionX(t.entityelement[ee].obj, torselimbindex);
					fTorseAreaY = LimbPositionY(t.entityelement[ee].obj, torselimbindex);
					fTorseAreaZ = LimbPositionZ(t.entityelement[ee].obj, torselimbindex);
				}
				entity_triggerdecalatimpact (fTorseAreaX, fTorseAreaY, fTorseAreaZ);
			}
		}
		else
		{
			if (t.ttdistanceaccuracy_f < 0.3 || Rnd(t.tchancetohit_f*t.ttdistanceaccuracy_f) == 0)
			{
				// player is target
				physics_player_takedamage ();
			}
			else
			{
				// play bullet whiz sound because the AI missed
				//t.tSndID = t.playercontrol.soundstartindex + 25 + Rnd(3);
				//if (SoundExist(t.tSndID) == 1)
				//{
				//	t.tSndX_f = t.tplayerx_f - 100 + Rnd(200);
				//	t.tSndY_f = t.tplayery_f - 100 + Rnd(200);
				//	t.tSndZ_f = t.tplayerz_f - 100 + Rnd(200);
				//	PositionSound(t.tSndID, t.tSndX_f, t.tSndY_f, t.tSndZ_f);
				//	SetSoundVolume(t.tSndID, soundtruevolume(100));
				//	SetSoundSpeed(t.tSndID, 36000 + Rnd(10000));
				//	PlaySound(t.tSndID);
				//}
			}
		}
	}
	else
	{
		// FLAK (projectile) - find starting GetPoint ( for projectile )
		t.tobj = t.entityelement[t.te].attachmentobj;
		if (t.tobj > 0)
		{
			t.flakx_f = LimbPositionX(t.tobj, t.entityelement[t.te].attachmentobjfirespotlimb);
			t.flaky_f = LimbPositionY(t.tobj, t.entityelement[t.te].attachmentobjfirespotlimb);
			t.flakz_f = LimbPositionZ(t.tobj, t.entityelement[t.te].attachmentobjfirespotlimb);
			t.tdx_f = t.tplayerx_f - t.flakx_f;
			t.tdy_f = t.tplayery_f - t.flaky_f;
			t.tdz_f = t.tplayerz_f - t.flakz_f;
			t.tdd_f = Sqrt(abs(t.tdx_f*t.tdx_f) + abs(t.tdz_f*t.tdz_f));
			t.flakangle_f = atan2deg(t.tdx_f, t.tdz_f) + (Rnd(4) - 2);
			t.flakpitch_f = ((t.tdy_f / t.tdd_f)*-35.0) + Rnd(4) - 2;
			t.ttzentid = t.entityelement[t.te].bankindex;

			// create and launch projectile
			int iStoreGunID = t.gunid;
			t.gunid = t.tgunid;
			t.tProjectileType_s = t.gun[t.gunid].projectile_s; weapon_getprojectileid ();
			t.tSourceEntity = t.te; t.tTracerFlag = 0;
			t.tStartX_f = t.flakx_f; t.tStartY_f = t.flaky_f; t.tStartZ_f = t.flakz_f;
			t.tAngX_f = t.flakpitch_f; t.tAngY_f = t.flakangle_f; t.tAngZ_f = 0;
			weapon_projectile_make (false);
			t.gunid = iStoreGunID;
		}
		t.tolde = t.e;
		t.e = t.te;
		entity_lua_findcharanimstate ();
		t.e = t.tolde;

		// deduct one unit of ammo (only if npc oes NOT ignore need to reload)
		if (g.firemodes[t.tgunid][0].settings.npcignorereload == 0)
		{
			t.charanimstate.ammoinclip = t.charanimstate.ammoinclip - 1;
			if (t.charanimstate.ammoinclip < 0)  t.charanimstate.ammoinclip = 0;
		}
	}
}

void darkai_ischaracterhit (void)
{
	// takes; px#,py#,pz#,tobj
	t.darkaifirerayhitcharacter = 0;
	for (g.charanimindex = 1; g.charanimindex <= g.charanimindexmax; g.charanimindex++)
	{
		if (t.entityelement[t.charanimstates[g.charanimindex].e].health > 0 && t.entityelement[t.charanimstates[g.charanimindex].e].eleprof.disableascharacter == 0 )
		{
			if (t.tobj == t.charanimstates[g.charanimindex].obj)
			{
				// if melee attack on character, half force for better organic response
				if (t.gun[t.gunid].settings.ismelee == 2)  t.tforce_f = t.tforce_f / 2.0;
				t.twhox_f = t.px_f; 
				t.twhoy_f = t.py_f; 
				t.twhoz_f = t.pz_f;
				darkai_shootcharacter ();
				t.darkaifirerayhitcharacter = 1;
			}
		}
	}
}

void darkai_shootcharacter (void)
{
	// receives charanimindex tobj tdamage twhox# twhoy# twhoz#
	if (t.entityelement[t.charanimstates[g.charanimindex].e].health > 0)
	{
		// handle shooting of character
		t.ttte = t.charanimstates[g.charanimindex].e;
		#ifdef WICKEDENGINE
		t.tdamage = 0; t.tdamageforce = t.tforce_f;
		if (g.firemodes[t.gunid][g.firemode].settings.damage > 0)
		{
			// ensure DamageMultiplier does not wipe out minimum damage
			t.tdamage = (float)g.firemodes[t.gunid][g.firemode].settings.damage * t.playercontrol.fWeaponDamageMultiplier;
			if (t.playercontrol.fWeaponDamageMultiplier > 0 && t.tdamage < 1) t.tdamage = 1;
		}
		if (t.gun[t.gunid].settings.ismelee == 2 || g.firemodes[t.gunid][g.firemode].settings.usemeleedamageonly > 0)
		{
			if (g.firemodes[t.gunid][0].settings.meleedamage > 0)
			{
				t.tdamage = g.firemodes[t.gunid][0].settings.meleedamage * t.playercontrol.fMeleeDamageMultiplier;
				if (t.playercontrol.fWeaponDamageMultiplier > 0 && t.tdamage < 1) t.tdamage = 1;
			}
		}
		#else
		t.tdamage = g.firemodes[t.gunid][g.firemode].settings.damage; t.tdamageforce = t.tforce_f;
		if (t.gun[t.gunid].settings.ismelee == 2) t.tdamage = g.firemodes[t.gunid][0].settings.meleedamage;
		#endif
		t.tdamagesource = 1;
		entity_applydamage ();
	}
}

// AI Sound Events

struct sSoundEvent
{
	float fX;
	float fY;
	float fZ;
	float fRadius;
	int iCategory;
	int iWhoE;
	DWORD dwTimeToFinish;
};
std::vector<sSoundEvent> g_soundEvent;

float getClosestSoundWithinRange (float fX, float fY, float fZ, int iCategory, int* piWhoE)
{
	int iWhoE = 0;
	int iResult = -1;
	float fBestDist = 99999.9f;
	for (int i = 0; i < g_soundEvent.size(); i++)
	{
		sSoundEvent* pSoundEvent = &g_soundEvent[i];
		float fDX = fX - pSoundEvent->fX;
		float fDY = fY - pSoundEvent->fY;
		float fDZ = fZ - pSoundEvent->fZ;
		float fDist = sqrt(fabs(fDX*fDX) + fabs(fDY*fDY) + fabs(fDZ*fDZ));
		if (fDist < pSoundEvent->fRadius && fDist < fBestDist && ( iCategory==0 || iCategory == pSoundEvent->iCategory) )
		{
			iWhoE = pSoundEvent->iWhoE;
			fBestDist = fDist;
			iResult = i;
		}
	}
	*piWhoE = iWhoE;
	if (iResult != -1)
	{
		return fBestDist;
	}
	else
	{
		return 0;
	}
}

void darkai_makesound_ex ( int iCategory, int iWhoE )
{
	// t.tsx_f, t.tsz_f, t.tradius_f
	// iCategory : 1-player, 2-nonplayer, 0-generic(explosion)
	sSoundEvent soundEvent;
	soundEvent.fX = t.tsx_f;
	soundEvent.fY = t.tsy_f;
	soundEvent.fZ = t.tsz_f;
	soundEvent.fRadius = t.tradius_f;
	soundEvent.iCategory = iCategory;
	soundEvent.iWhoE = iWhoE;
	soundEvent.dwTimeToFinish = timeGetTime() + 1000;
	g_soundEvent.push_back(soundEvent);
}

void darkai_makesound_byplayer (void)
{
	darkai_makesound_ex ( 1, -1 );
}

void darkai_makesound (void)
{
	darkai_makesound_ex ( 2, t.te );
}

void darkai_makeexplosionsound (void)
{
	t.tradius_f = 1000.0f;
	darkai_makesound_ex ( 0, 0 );
}

void darkai_managesound (void)
{
	// kill sound events after X time after creation
	DWORD dwCurrentTime = timeGetTime();
	for (int i = 0; i < g_soundEvent.size(); i++)
	{
		sSoundEvent* pSoundEvent = &g_soundEvent[i];
		if (dwCurrentTime > pSoundEvent->dwTimeToFinish)
		{
			g_soundEvent.erase(g_soundEvent.begin() + i);
			i = -1;
		}
	}
}

// empty functions not used

void darkai_preparedata ( void )
{
}

void darkai_completeobstacles ( void )
{
}

void darkai_invalidateobstacles ( void )
{
}

void darkai_saveobstacles ( void )
{
}

void darkai_loadobstacles ( void )
{
}

int darkai_finddoorcontainer ( int iObj )
{
	return 0;
}

void darkai_adddoor ( void )
{
}

void darkai_removedoor ( void )
{
}

void darkai_obstacles_terrain ( void )
{
}

void darkai_obstacles_terrain_refresh ( void )
{
}

void darkai_destroy_all_characterdata ( void )
{
}

void darkai_release_characters ( void )
{
}

void darkai_setup_tree ( void )
{
}

void darkai_setup_entity ( void )
{
}

void darkai_addobstoallneededcontainers ( int iType, int iObj, int iFullHeight, float fMinHeight, float fSliceHeight, float fSliceMinSize )
{
}

void darkai_staggerAIprocessing ( void )
{
}

void darkai_staywithzone ( int iAIObj, float fLastX, float fLastZ, float* pX, float* pZ )
{
}

void darkai_updatedebugobjects (void)
{
}

void darkai_character_remove_charpart ( void )
{
}

void darkai_character_remove ( void )
{
}

void darkai_character_loop ( void )
{
}

void darkai_finalsettingofcharacterobjects ( void )
{
}

void darkai_character_freezeall ( void )
{
}

void darkai_assignanimtofield ( void )
{
}

void char_createseqdata ( void )
{
}

void char_getcharseqcsifromplaycsi ( void )
{
}
