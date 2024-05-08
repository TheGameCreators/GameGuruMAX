//----------------------------------------------------
//--- GAMEGURU - M-Bulletholes
//----------------------------------------------------

// includes
#include "stdafx.h"
#include "gameguru.h"

#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

// globals
#define BULLETHOLESMAX 1000
struct sBulletHole
{
	int iVertIndexStart;
	float fLifeCounter;
	float fRadius;
	GGVECTOR3 vecWorldPos;
};
std::vector<sBulletHole> g_bulletholes;
bool g_bulletholeavailable[BULLETHOLESMAX];
GGVECTOR3 g_vecBulletHoleQuad[4];
GGVECTOR3 g_vecBulletHoleQuadUV[4];

// functions

void bulletholes_init (void)
{
	if (ObjectExist(g.bulletholesobject) == 0)
	{
		// create bullethole memblock mesh
		int iMemblockIndex = 0;
		for (int i = 1; i <= 257; i++)
		{
			if (MemblockExist(i) == 0)
			{
				iMemblockIndex = i;
				break;
			}
		}
		if (iMemblockIndex == 0) return;
		int iPolygonCount = BULLETHOLESMAX*2;
		int vertsize = 12+12+8;
		int iVertexCount = iPolygonCount * 6;
		int iSizeBytes = 12; // header size
		iSizeBytes += vertsize * iVertexCount;
		MakeMemblock(iMemblockIndex, iSizeBytes);
		WriteMemblockDWord(iMemblockIndex, 0, GGFVF_XYZ | GGFVF_NORMAL | GGFVF_TEX1);
		WriteMemblockDWord(iMemblockIndex, 4, vertsize);
		WriteMemblockDWord(iMemblockIndex, 8, iVertexCount);

		// create bullethole master object
		int iWorkMeshID = g.meshgeneralwork2;
		if (GetMeshExist(iWorkMeshID) == 1) DeleteMesh(iWorkMeshID);
		CreateMeshFromMemblock(iWorkMeshID, iMemblockIndex);
		MakeObject(g.bulletholesobject, iWorkMeshID, 0);
		if (GetMeshExist(iWorkMeshID) == 1) DeleteMesh(iWorkMeshID);
		if (MemblockExist(iMemblockIndex) == 1) DeleteMemblock(iMemblockIndex);

		// set bulletholes object for rendering
		LoadImage("gamecore\\bulletholes\\bulletholes_color.dds", g.bulletholeimage);
		TextureObject(g.bulletholesobject, g.bulletholeimage);
		SetObjectTransparency(g.bulletholesobject, 1);
		SetObjectCull(g.bulletholesobject, 0);
	}

	//  clear all bulletholes
	bulletholes_clearall();
}

void bulletholes_clearall (void)
{
	// wipe out data in object
	if (ObjectExist(g.bulletholesobject) == 1)
	{
		int iPolygonCount = BULLETHOLESMAX * 2;
		int iVertexCount = iPolygonCount * 6;
		LockVertexDataForLimbCore(g.bulletholesobject, 0, 1);
		for (int iVertIndex = 0; iVertIndex < iVertexCount; iVertIndex++)
		{
			SetVertexDataPosition(iVertIndex, 0, 0, 0);
		}
		UnlockVertexData();
	}

	// clear the list
	g_bulletholes.clear();
	memset(g_bulletholeavailable, 0, sizeof(g_bulletholeavailable));
}

void bulletholes_changesinglehole (int iVertIndex, float fNX, float fNY, float fNZ)
{
	// needs g_vecBulletHoleQuad and g_vecBulletHoleQuadUV globals
	bool bBulletHoleValid = true; if (fNX == 0 && fNY == 0 & fNZ == 0) bBulletHoleValid = false;
	LockVertexDataForLimbCore(g.bulletholesobject, 0, 1);
	for (int v = 0; v < 6; v++)
	{
		int iWhichCornerOfQuad = 0;
		if (v == 1) iWhichCornerOfQuad = 1;
		if (v == 2) iWhichCornerOfQuad = 2;
		if (v == 3) iWhichCornerOfQuad = 1;
		if (v == 4) iWhichCornerOfQuad = 3;
		if (v == 5) iWhichCornerOfQuad = 2;
		if (bBulletHoleValid == true)
		{
			SetVertexDataPosition(iVertIndex + v, g_vecBulletHoleQuad[iWhichCornerOfQuad].x, g_vecBulletHoleQuad[iWhichCornerOfQuad].y, g_vecBulletHoleQuad[iWhichCornerOfQuad].z);
			SetVertexDataNormals(iVertIndex + v, fNX, fNY, fNZ);
			SetVertexDataUV(iVertIndex + v, g_vecBulletHoleQuadUV[iWhichCornerOfQuad].x, g_vecBulletHoleQuadUV[iWhichCornerOfQuad].y);
		}
		else
		{
			SetVertexDataPosition(iVertIndex + v, 0, 0, 0);
		}
	}
	UnlockVertexData();
}

void bulletholes_add (int iMaterialIndex, float fX, float fY, float fZ, float fNX, float fNY, float fNZ)
{
	// no holes for silent materials
	if (iMaterialIndex <= 0)
		return;

	// map materialindex to atlas (could make this a settings file to update via text file easily in the future)
	float fBulletHoleRadius = 1.25f;
	int iAtlasIndex = iMaterialIndex;
	switch (iMaterialIndex)
	{
		case 0: iAtlasIndex = 1; fBulletHoleRadius = 0.50f; break; // generic
		case 1: iAtlasIndex = 0; fBulletHoleRadius = 1.00f; break; // stone
		case 2: iAtlasIndex = 3; fBulletHoleRadius = 0.35f; break; // metal
		case 3: iAtlasIndex = 4; fBulletHoleRadius = 0.75f; break; // wood
		case 4: iAtlasIndex = 2; fBulletHoleRadius = 0.75f; break; // glass
	}

	// instant reject if near an existing bullethole (avoid zclash)
	GGVECTOR3 vecPointInWorldSpace = GGVECTOR3(fX, fY, fZ);
	for (int n = 0; n < g_bulletholes.size(); n++)
	{
		float fDX = g_bulletholes[n].vecWorldPos.x - vecPointInWorldSpace.x;
		float fDY = g_bulletholes[n].vecWorldPos.y - vecPointInWorldSpace.y;
		float fDZ = g_bulletholes[n].vecWorldPos.z - vecPointInWorldSpace.z;
		float fDist = sqrt(fabs(fDX*fDX) + fabs(fDY*fDY) + fabs(fDZ*fDZ));
		if (fDist < g_bulletholes[n].fRadius+fBulletHoleRadius)
		{
			// bullethole too near an existing one
			return;
		}
	}

	// find free vertindex to place bullethole
	int iBulletSlot;
	for (iBulletSlot = 0; iBulletSlot < BULLETHOLESMAX; iBulletSlot++)
		if (g_bulletholeavailable[iBulletSlot] == false)
			break;

	// add bullethole if found a free space
	if (iBulletSlot < BULLETHOLESMAX)
	{
		// make coordinates for the bullethole quad
		g_vecBulletHoleQuad[0] = GGVECTOR3(-2, -2, 0);
		g_vecBulletHoleQuad[1] = GGVECTOR3( 2, -2, 0);
		g_vecBulletHoleQuad[2] = GGVECTOR3(-2,  2, 0);
		g_vecBulletHoleQuad[3] = GGVECTOR3( 2,  2, 0);
		GGMATRIX matFinalOrientation;
		float fRandomSpin = rand() % 360; // spin hole around 360 degrees randomly
		GGMatrixRotationZ(&matFinalOrientation, GGToRadian(fRandomSpin)); 
		float fAngleX, fAngleY, fAngleZ;
		GetAngleFromPoint (0, 0, 0, fNX, fNY, fNZ, &fAngleX, &fAngleY, &fAngleZ);
		GGMATRIX matRotX; GGMatrixRotationX(&matRotX, GGToRadian(fAngleX));
		GGMATRIX matRotY; GGMatrixRotationY(&matRotY, GGToRadian(fAngleY));
		GGMATRIX matRotZ; GGMatrixRotationZ(&matRotZ, GGToRadian(fAngleZ));
		GGMATRIX matNormalDirectionRot;
		GGMatrixIdentity(&matNormalDirectionRot);
		GGMatrixMultiply(&matNormalDirectionRot, &matNormalDirectionRot, &matRotX);
		GGMatrixMultiply(&matNormalDirectionRot, &matNormalDirectionRot, &matRotY);
		GGMatrixMultiply(&matNormalDirectionRot, &matNormalDirectionRot, &matRotZ);
		GGMatrixMultiply(&matFinalOrientation, &matFinalOrientation, &matNormalDirectionRot);
		GGVECTOR3 vecPointInWorldSpace = GGVECTOR3(fX, fY, fZ);
		GGVECTOR3 vecNormalizedDir = GGVECTOR3(fNX, fNY, fNZ);
		GGVec3Normalize(&vecNormalizedDir, &vecNormalizedDir);
		// bring actual point slightly out of surface so as not to clash with it
		// LB: not happy with this for terrain, the physics geometry does not match the visual one, so bulletholes float, or sink under the terrain floor!
		// vecPointInWorldSpace += vecNormalizedDir * 2.0f;
		// every so slightly for regular things like walls, flat static objects, etc (again if the physics shape is very different, float or sink again)
		GGVECTOR3 vecShiftedPointInWorldSpace = vecPointInWorldSpace + (vecNormalizedDir * 0.1f);
		for (int p = 0; p < 4; p++)
		{
			GGVec3TransformCoord(&g_vecBulletHoleQuad[p], &g_vecBulletHoleQuad[p], &matFinalOrientation);
			g_vecBulletHoleQuad[p] += vecShiftedPointInWorldSpace;
		}

		// work out UV based on material index
		float U_f = 0.0f;
		float V_f = 0.0f;
		float USize_f = 1.0f / 4.0f;
		float VSize_f = 1.0f / 4.0f;
		if (iAtlasIndex != 0)
		{
			int across = int(iAtlasIndex / 4.0f);
			V_f = VSize_f * across;
			U_f = iAtlasIndex * USize_f ;
		}

		//MD: Bullet holes had white atrefacts due to float inaccuracy, shaving off the edges fixed this
		g_vecBulletHoleQuadUV[0] = GGVECTOR3(U_f + 0.01f, V_f + 0.01f, 0);
		g_vecBulletHoleQuadUV[1] = GGVECTOR3(U_f + USize_f - 0.01f, V_f + 0.01f, 0);
		g_vecBulletHoleQuadUV[2] = GGVECTOR3(U_f + 0.01f, V_f + VSize_f - 0.01f, 0);
		g_vecBulletHoleQuadUV[3] = GGVECTOR3(U_f + USize_f - 0.01f, V_f + VSize_f - 0.01f, 0);

		// add bullet hole to list
		sBulletHole bullethole;
		bullethole.iVertIndexStart = iBulletSlot * 6;
		bullethole.fLifeCounter = 2000.0f;
		bullethole.fRadius = fBulletHoleRadius;
		bullethole.vecWorldPos = vecPointInWorldSpace;
		g_bulletholes.push_back(bullethole);
		g_bulletholeavailable[iBulletSlot] = true;

		// add vert data to represent this bullet hole
		bulletholes_changesinglehole (bullethole.iVertIndexStart, fNX, fNY, fNZ);

		// update object to see change
		sObject* pObject = GetObjectData(g.bulletholesobject);
		WickedCall_RemoveObject(pObject);
		WickedCall_AddObject(pObject);
		WickedCall_TextureObject(pObject, NULL);
		WickedCall_SetObjectCastShadows(pObject, false);
		WickedCall_SetObjectTransparent(pObject);
		WickedCall_SetObjectRenderLayer(pObject, GGRENDERLAYERS_CURSOROBJECT);
	}
}

void bulletholes_update (void)
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	// go through all bulletholes in list and remove those that have expired
	bool bUpdateTheObject = false;
	for (int b = 0; b < g_bulletholes.size(); b++)
	{
		sBulletHole* pBulletHole = &g_bulletholes[b];
		pBulletHole->fLifeCounter -= g.timeelapsed_f;
		if ( pBulletHole->fLifeCounter < 0.0f ) //LB: could also condition this based on camera distance/view so user NEVER sees the hole disappear
		{
			// remove bullethole from object
			bulletholes_changesinglehole (pBulletHole->iVertIndexStart, 0, 0, 0);

			// remove bullet hoel from list
			int iBulletSlot = pBulletHole->iVertIndexStart / 6;
			g_bulletholeavailable[iBulletSlot] = false;
			g_bulletholes.erase(g_bulletholes.begin() + b);

			// see the change
			bUpdateTheObject = true;

			// break from loop, only delete one from list per cycle
			break;
		}
	}
	if (bUpdateTheObject == true)
	{
		// update object to see change
		sObject* pObject = GetObjectData(g.bulletholesobject);
		WickedCall_RemoveObject(pObject);
		WickedCall_AddObject(pObject);
		WickedCall_TextureObject(pObject, NULL);
		WickedCall_SetObjectCastShadows(pObject, false);
		WickedCall_SetObjectTransparent(pObject);
	}
}

void bulletholes_free (void)
{
	// blank out all verts in the object
	bulletholes_clearall();

	// hide bullethole master object
	if (ObjectExist(g.bulletholesobject) == 1)
	{
		// hide the actual object
		HideObject(g.bulletholesobject);
	}
}
