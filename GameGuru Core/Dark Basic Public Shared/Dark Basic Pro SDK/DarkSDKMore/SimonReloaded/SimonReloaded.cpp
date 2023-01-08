#include "stdafx.h"
#include ".\globstruct.h"
#include "SimonReloaded.h"
#include "CObjectsC.h"
#include "CMemblocks.h"
#include "BlitzTerrain.h"

#ifdef WICKEDENGINE
#include ".\..\..\Guru-WickedMAX\wickedcalls.h"
#endif

#define GRASSALPHACLIP 0.41f

extern GlobStruct* g_pGlob;

int iGridSize,iVegAreaWidth,iVegHalfAreaWidth,iVegAreaRad,iGridDimension;
float fVegWidth,fVegHeight;
int iGrassClumpVerts,iVegPerMesh,iBuildMesh,iCameraMask;
float fOldViewPointX,fOldViewPointZ;
int iOldLeftEdge,iOldRightEdge,iOldFrontEdge,iOldBackEdge;
int iGridObjectStart, iGridObjectEnd, iGrassObj, iGrassImg, iShadowImg, iShader, iGrassMemBlock, iGrassMemBlockRes;
int iPBRAGEDImg, iPBRSpecImg, iPBRCubeImg, iPBRCurveImg;
bool bResourcesSet,bGridMade;
int iLeftVert,iRightVert,iFrontVert,iBackVert,iTopVert,iBottomVert;
bool **bGridExist;
int iGrassMemblockThreshhold;
float fWorldSize;
bool bDisplayBelowWater;

void InfiniteVegetationConstructor ( void )
{
	bResourcesSet = false;
	bGridMade = false;
	iGridObjectStart = 0;
	iGridObjectEnd = 0;
	iGrassObj = 0;
	iGrassImg = 0;
	iShadowImg = 0;
	iShader = 0;
	iGrassMemBlock = 0;
	iGrassMemBlockRes = 0;
	iBuildMesh = 0;
	iCameraMask = 0;
	iGridSize = 0;
	iVegAreaWidth = 0;
	iVegHalfAreaWidth = 0;
	iVegAreaRad = 0;
	iGrassClumpVerts = 0;
	iVegPerMesh = 0;	
	fVegWidth = 0;
	fVegHeight = 0;
	iGrassMemblockThreshhold = 74; // Grass not drawn when memblock value <= this value
	fWorldSize = 51200.0f;
}
 void InfiniteVegetationDestructor ( void )
{
	// Free memory here
}
void InfiniteVegetationReceiveCoreDataPtr ( LPVOID pCore )
{
}

float floatFromBits( DWORD const bits )
{
    return *reinterpret_cast< float const* >( &bits );
}

void SetPBRResourceValues ( int iPBRAGED, int iPBRSpec, int iPBRCube, int iPBRCurve )
{
	#ifdef PAULNEWGRASSSYSTEM
	return;
	#endif
	iPBRAGEDImg = iPBRAGED;
	iPBRSpecImg = iPBRSpec;
	iPBRCubeImg = iPBRCube;
	iPBRCurveImg = iPBRCurve;
}

void SetResourceValues(int iGrassObjIN, int iGridObjectStartIN, int iGrassImgIN, int iShadowImgIN, 
									 int iBuildMeshIN, int iShaderIN, int iGrassMemBlockIN, int iGrassMemBlockResIN,
									 int iCameraMaskIN)
{ 
	#ifdef PAULNEWGRASSSYSTEM
	return;
	#endif

	if (bResourcesSet) return;

	iGrassObj = iGrassObjIN;
	iGridObjectStart = iGridObjectStartIN;
	iGrassImg = iGrassImgIN;
	iShadowImg = iShadowImgIN;
	iShader = iShaderIN;
	iGrassMemBlock = iGrassMemBlockIN;
	iGrassMemBlockRes = iGrassMemBlockResIN;
	iBuildMesh = iBuildMeshIN;
	iCameraMask = iCameraMaskIN;

	bResourcesSet = true;
}

 void SetTerrainMask(int iMask)
{
	#ifdef PAULNEWGRASSSYSTEM
	 return;
	#endif

	iCameraMask = iMask;
	if (!bGridMade) return;
	
	for (int iObj = iGridObjectStart; iObj < iGridObjectEnd + 1; iObj++){
		if (ObjectExist(iObj) == 1) SetObjectMask(iObj,iCameraMask);	
	}
}
 
 int InfiniteVegetationRnd(int iMax)
{
	#ifdef PAULNEWGRASSSYSTEM
	 return 0;
	#endif

	return (int)(rand()*((float)iMax)/RAND_MAX);
}

 void DeleteVegetationGrid(void)
{
	#ifdef PAULNEWGRASSSYSTEM
	 return;
	#endif

	if (!bGridMade) return;

	for (int iObj = iGridObjectStart; iObj < iGridObjectEnd + 1; iObj++){
		if (ObjectExist(iObj) == 1) DeleteObject(iObj);
	}	

	// Delete old grid exist array
	for (int iG = 0; iG < iGridDimension; iG++){
		delete [] bGridExist[iG];		
	}
	delete [] bGridExist;

	bGridMade = false;
}

void SetVegetationGridVisible(bool bShow)
{
	#ifdef PAULNEWGRASSSYSTEM
	return;
	#endif

	if (!bGridMade) return;

	int iX,iZ;
	for (int iObj = iGridObjectStart; iObj < iGridObjectEnd + 1; iObj++){
		if (ObjectExist(iObj) == 1){
			if (bShow){
				iX = (int)(ObjectPositionX(iObj)/iVegAreaWidth);
				iZ = (int)(ObjectPositionZ(iObj)/iVegAreaWidth);
				if (bGridExist[iX][iZ])	ShowObject(iObj);
			}else{
				HideObject(iObj);
			}
		}
	}
}

void SetVegetationGridVisibleForce(bool bShow)
{
	#ifdef PAULNEWGRASSSYSTEM
	return;
	#endif

	int iX, iZ;
	for (int iObj = iGridObjectStart; iObj < iGridObjectEnd + 1; iObj++) {
		if (ObjectExist(iObj) == 1) {
			if (bShow) {
				iX = (int)(ObjectPositionX(iObj) / iVegAreaWidth);
				iZ = (int)(ObjectPositionZ(iObj) / iVegAreaWidth);
				if (bGridExist[iX][iZ])	ShowObject(iObj);
			}
			else {
				HideObject(iObj);
			}
		}
	}
}

// Sets memblock values to 0 when grass exists in invalid locations (slopes and underwater)
 void DeleteInvalidGrass(int iTerrainID, float fWaterHeight, float fSlopeMax)
{
	#ifdef PAULNEWGRASSSYSTEM
	 return;
	#endif

	if (!bResourcesSet) return;
	if (MemblockExist(iGrassMemBlock) == 0) return; // Safety check

	float fStepRatio = fWorldSize/iGrassMemBlockRes;
	float fXPos,fZPos,fHeightLeft,fHeightRight,fHeightFront,fHeightBack;
	int iGrassMemPos = 4+4+4;
	float fSlopeDistFull = fSlopeMax * 10; // Distance between height sample points front/back and left/right
	float fSlopeDistDiag = (float)sqrt((fSlopeDistFull/2.0)*(fSlopeDistFull/2.0)*2); // Distance between diagonal sample points
	for (int iZ = 0; iZ < iGrassMemBlockRes; iZ ++)
	{
		fZPos = iZ * fStepRatio;
		for (int iX = 0; iX < iGrassMemBlockRes; iX ++)
		{
			// No point trying to delete grass if there's none there already!
			#ifdef WICKEDENGINE
			if (ReadMemblockByte(iGrassMemBlock,iGrassMemPos+2) > 0)
			#else
			if (ReadMemblockByte(iGrassMemBlock,iGrassMemPos+2) > iGrassMemblockThreshhold)
			#endif
			{
				fXPos = iX * fStepRatio;
				// Check water height and slopes. This tries to check as efficiently as possible with the
				// minimum number of getgroundheight calls, which is why the logic is somewhat unintuitive
				#ifndef NOSTEAMORVIDEO
				fHeightLeft = (BT_GetGroundHeight(iTerrainID,fXPos-5.0f,fZPos));
				#else
				fHeightLeft = 0;
				#endif
				// First check we're not underwater using our first height check
				if (fHeightLeft < fWaterHeight)
				{
					//WriteMemblockByte(iGrassMemBlock,iGrassMemPos,0);
					WriteMemblockByte(iGrassMemBlock,iGrassMemPos+0,0);
					WriteMemblockByte(iGrassMemBlock,iGrassMemPos+1,0);
					WriteMemblockByte(iGrassMemBlock,iGrassMemPos+2,0);
					WriteMemblockByte(iGrassMemBlock,iGrassMemPos+3,0);
				}
				else
				{
					// Now get the right height and check left/right
					#ifndef NOSTEAMORVIDEO
					fHeightRight = (BT_GetGroundHeight(iTerrainID,fXPos + 5.0f,fZPos));
					#else
					fHeightRight = 0;
					#endif
					if (abs(fHeightLeft - fHeightRight) > fSlopeDistFull)
					{
						WriteMemblockByte(iGrassMemBlock,iGrassMemPos+0,0);
						WriteMemblockByte(iGrassMemBlock,iGrassMemPos+1,0);
						WriteMemblockByte(iGrassMemBlock,iGrassMemPos+2,0);
						WriteMemblockByte(iGrassMemBlock,iGrassMemPos+3,0);
					}
					else
					{
						// Now get the front height and check two diagonals
						#ifndef NOSTEAMORVIDEO
						fHeightFront = (BT_GetGroundHeight(iTerrainID,fXPos,fZPos + 5.0f));
						#else
						fHeightFront = 0;
						#endif
						if (abs(fHeightLeft - fHeightFront) > fSlopeDistDiag || abs(fHeightFront - fHeightRight) > fSlopeDistDiag)
						{
							WriteMemblockByte(iGrassMemBlock,iGrassMemPos+0,0);
							WriteMemblockByte(iGrassMemBlock,iGrassMemPos+1,0);
							WriteMemblockByte(iGrassMemBlock,iGrassMemPos+2,0);
							WriteMemblockByte(iGrassMemBlock,iGrassMemPos+3,0);
						}
						else
						{
							// Now get the back height, and check two diagonals and front to back
							#ifndef NOSTEAMORVIDEO
							fHeightBack = (BT_GetGroundHeight(iTerrainID,fXPos,fZPos - 5.0f));
							#else
							fHeightBack = 0;
							#endif
							if (abs(fHeightFront - fHeightBack) > fSlopeDistFull || abs(fHeightLeft - fHeightBack) > fSlopeDistDiag || abs(fHeightFront - fHeightBack) > fSlopeDistDiag)
							{
								WriteMemblockByte(iGrassMemBlock,iGrassMemPos+0,0);
								WriteMemblockByte(iGrassMemBlock,iGrassMemPos+1,0);
								WriteMemblockByte(iGrassMemBlock,iGrassMemPos+2,0);
								WriteMemblockByte(iGrassMemBlock,iGrassMemPos+3,0);
							}
						}
					}			
				}
			}
			iGrassMemPos+=4;
		}
	}
}

// Store VEG mesh UV pattern, so can recreate for many grasses
bool g_bVegMeshPatternFilled = false;
int g_iNumOfVertsInVegMesh = 0;
float g_fUVPatternForVegMesh[128][5];

// Makes the actual 3D model for the grid square of grass
void MakeVegPatch(int iVegObj, float fVegHeight, float fVegWidth, int iX, int iZ, int iOptionalSkipGrassMemblock)
{
	#ifdef PAULNEWGRASSSYSTEM
	return;
	#endif

	if (!bResourcesSet) return;
	float fGrassX,fGrassZ,fGrassHeight,fGrassWidth;
	int iVegBoom;
	int iBoomChance = (int)(iVegPerMesh * 0.6f);
	bool bWeHaveGrassHere = true;

	#ifdef WICKEDENGINE
	if (g_bVegMeshPatternFilled == false)
	{
		if (iGrassObj > 0 && GetMeshExist(iGrassObj) == 1)
		{
			// bear in mind at this point the grass mesh has UVs 0->1 not 0->0.25f
			LockVertexDataForMesh(iGrassObj);
			int iVerts = GetVertexDataVertexCount();
			g_iNumOfVertsInVegMesh = iVerts;
			if (g_iNumOfVertsInVegMesh > 127) g_iNumOfVertsInVegMesh = 127;
			for (int iV = 0; iV < g_iNumOfVertsInVegMesh; iV++)
			{
				g_fUVPatternForVegMesh[iV][0] = GetVertexDataU(iV) / 4.0f;
				g_fUVPatternForVegMesh[iV][1] = GetVertexDataV(iV) / 4.0f;
				g_fUVPatternForVegMesh[iV][2] = GetVertexDataPositionX(iV);
				g_fUVPatternForVegMesh[iV][3] = GetVertexDataPositionY(iV);
				g_fUVPatternForVegMesh[iV][4] = GetVertexDataPositionZ(iV);
			}
			UnlockVertexData();
			g_bVegMeshPatternFilled = true;
		}
	}
	#endif

	// now create veg object (full or dummy)
	if (ObjectExist(iVegObj)) DeleteObject(iVegObj);
	if ( bWeHaveGrassHere == true )
	{
		if ( iVegObj == iGridObjectStart )
		{
			// determine grass obj (mesh!), for wicked this is a 4x4 plate containing many grasses
			int iGrassMeshToUse = iGrassObj;

			// create original grass clump
			MakeObject(iVegObj,iGrassMeshToUse,0);
			for (int iGrass = 1; iGrass < iVegPerMesh + 1; iGrass++)
			{
				// Add and position grass clump in mesh
				fGrassX=(float)(InfiniteVegetationRnd(iVegAreaWidth)-iVegHalfAreaWidth);
				fGrassZ=(float)(InfiniteVegetationRnd(iVegAreaWidth)-iVegHalfAreaWidth);
				AddLimb(iVegObj,iGrass,iGrassMeshToUse);
				OffsetLimb(iVegObj,iGrass,fGrassX,0,fGrassZ);
				RotateLimb(iVegObj,iGrass,0,(float)InfiniteVegetationRnd(360),0);
				// Occassionally make a huge piece of grass
				if (InfiniteVegetationRnd(iBoomChance) == 1){iVegBoom=2;} else {iVegBoom=1;}
				fGrassHeight = (InfiniteVegetationRnd((int)(fVegHeight/1.5)) + fVegHeight) * iVegBoom;
				fGrassWidth = fVegWidth*2*iVegBoom;
				ScaleLimb(iVegObj,iGrass,fGrassWidth,fGrassHeight,fGrassWidth);
			}
			// Turn the multi limb object into a single limb object and setup
			if (GetMeshExist(iBuildMesh)) DeleteMesh(iBuildMesh);
			MakeMeshFromObject(iBuildMesh,iVegObj);
			DeleteObject(iVegObj);
			MakeObject(iVegObj,iBuildMesh,iGrassImg);
			DeleteMesh(iBuildMesh);
			SetObjectEffect(iVegObj,iShader);
		}
		else
		{
			// simply clone clump to speed up process for others
			CloneObject ( iVegObj, iGridObjectStart );
		}
		SetObjectMask(iVegObj,iCameraMask);
		#ifdef WICKEDENGINE
		sObject* pVegObject = GetObjectData(iVegObj);
		if (pVegObject)
		{
			WickedCall_SetObjectAlphaRef(pVegObject, GRASSALPHACLIP); //PE: Same as tree/veg values.
			WickedCall_SetObjectCastShadows(pVegObject, false); //PE: No shadows on veg for now.
		}
		#else
		if( bDisplayBelowWater )
			SetObjectTransparency(iVegObj,8);
		else
			SetObjectTransparency(iVegObj, 6);
		#endif
		SetObjectCull ( iVegObj, 0 );
	}
}

void UpdateVegPatch(int iVegObj, int iTerrainID, float fVegX, float fVegZ)
{
	#ifdef PAULNEWGRASSSYSTEM
	return;
	#endif

	if (ObjectExist(iVegObj) == 0) return; // Safety check
	if (MemblockExist(iGrassMemBlock) == 0) return; // Safety check

	int iVertexCount,iGrassMemX,iGrassMemZ,iGrassMemPos;
	float fCenterHeight,fMiddleX,fMiddleZ,fLow,fHigh,fFloor,fNewHigh;
	float fVertX,fVertY,fVertZ;
	float fMemStepRatio = iGrassMemBlockRes/fWorldSize;

	#ifndef NOSTEAMORVIDEO
	fCenterHeight = (BT_GetGroundHeight(iTerrainID,fVegX,fVegZ));
	#else
	fCenterHeight = 0;
	#endif
	PositionObject(iVegObj,fVegX,fCenterHeight,fVegZ);

	// Now conform the grass pieces to the terrain height
	LockVertexDataForLimbCore(iVegObj,0,1);
	iVertexCount = 0;
	for (int iGrass=0; iGrass < iVegPerMesh + 1; iGrass++)
	{
		// Extract a central point, low point and high point for this piece of grass. 
		#ifdef WICKEDENGINE
		fMiddleX = fVegX + ((GetVertexDataPositionX(iVertexCount)) + (GetVertexDataPositionX(iVertexCount))) / 2.0f;
		fMiddleZ = fVegZ + ((GetVertexDataPositionZ(iVertexCount)) + (GetVertexDataPositionZ(iVertexCount))) / 2.0f;
		#else
		fMiddleX = fVegX + ((GetVertexDataPositionX(iVertexCount + iLeftVert)) + (GetVertexDataPositionX(iVertexCount + iRightVert))) / 2.0f;
		fMiddleZ = fVegZ + ((GetVertexDataPositionZ(iVertexCount + iFrontVert)) + (GetVertexDataPositionZ(iVertexCount + iBackVert))) / 2.0f;
		fLow = (GetVertexDataPositionY(iVertexCount + iBottomVert));
		fHigh = (GetVertexDataPositionY(iVertexCount + iTopVert));
		#endif
		
		// If the grass isn't painted for this area, we can simply set the verts to the magic high value of 200.
		// The vertex shader will do minimal processing on these verts and their pixels will be clipped in the
		// pixel shader
		iGrassMemX = (int)(fMiddleX * fMemStepRatio);
		if (iGrassMemX < 0) iGrassMemX = 0;
		if (iGrassMemX >= iGrassMemBlockRes) iGrassMemX = iGrassMemBlockRes - 1;
		iGrassMemZ = (int)(fMiddleZ * fMemStepRatio);
		if (iGrassMemZ < 0) iGrassMemZ = 0;
		if (iGrassMemZ >= iGrassMemBlockRes) iGrassMemZ = iGrassMemBlockRes - 1;
		iGrassMemPos = 4+4+4+((iGrassMemX + iGrassMemZ * iGrassMemBlockRes)*4);

		#ifdef WICKEDENGINE
		// green channel can control the per grass height modulation (0-31)
		int iGreenChannel = ReadMemblockByte(iGrassMemBlock, iGrassMemPos + 1);
		float fScalePerGrassSize = ((100.0f / 31.0f)*(float)iGreenChannel)/100.0f;
		#else
		float fScalePerGrassSize = 1.0f;
		#endif

		#ifdef WICKEDENGINE
		int iGrassType = 0;
		int iRedChannel = ReadMemblockByte(iGrassMemBlock, iGrassMemPos + 2);
		if ( iRedChannel == 0 || fScalePerGrassSize == 0.0f )
		{
			// hopefully these verts cull out before rendering takes a hit
			fFloor = 9999999;
		}
		else
		{
			// follow terrain height
			fFloor = (BT_GetGroundHeight(iTerrainID,fMiddleX,fMiddleZ)) - fCenterHeight;

			// also assign grass type from memblock
			iGrassType = iRedChannel-1;
			if (iGrassType < 0) iGrassType = 0;
			if (iGrassType > 15) iGrassType = 15;
		}
		#else
		if (ReadMemblockByte(iGrassMemBlock,iGrassMemPos+2) <= iGrassMemblockThreshhold)
		{
			fFloor=200;
		}
		else
		{
			#ifndef NOSTEAMORVIDEO
			fFloor = (BT_GetGroundHeight(iTerrainID,fMiddleX,fMiddleZ)) - fCenterHeight;
			#endif
		}
		#endif

		// final height of grass
		#ifdef WICKEDENGINE
		//fNewHigh = fFloor + (20.0f*fScalePerGrassSize);
		#else
		fNewHigh = fFloor + ((fHigh - fLow)*fScalePerGrassSize);
		#endif

		// Now reposition all of our vertices for this grass clump					
		int iLocalVertexCount = iVertexCount;
		for (int fGrassVert = 0; fGrassVert < iGrassClumpVerts; fGrassVert++)
		{
			// set grass quad position
			#ifdef WICKEDENGINE
			fVertX = (GetVertexDataPositionX(iLocalVertexCount));
			fVertZ = (GetVertexDataPositionZ(iLocalVertexCount));
			//fVertX = g_fUVPatternForVegMesh[fGrassVert][2];
			//fVertY = g_fUVPatternForVegMesh[fGrassVert][3];
			//fVertZ = g_fUVPatternForVegMesh[fGrassVert][4];
			fVertY = g_fUVPatternForVegMesh[fGrassVert][3] * fScalePerGrassSize;
			SetVertexDataPosition(iLocalVertexCount,fVertX,fFloor+fVertY,fVertZ);
			#else
			fVertX = (GetVertexDataPositionX(iLocalVertexCount));
			fVertY = (GetVertexDataPositionY(iLocalVertexCount));
			fVertZ = (GetVertexDataPositionZ(iLocalVertexCount));
			if (abs(fVertY - fLow) < 0.01f)
			{
				SetVertexDataPosition(iLocalVertexCount,fVertX,fFloor,fVertZ);
			}
			else
			{
				SetVertexDataPosition(iLocalVertexCount,fVertX,fNewHigh,fVertZ);
			}				
			#endif
			iLocalVertexCount++;
		}				

		// Now adjust UV data to reflect grass type
		#ifdef WICKEDENGINE
		if (g_bVegMeshPatternFilled == true)
		{
			// we control grass type by offsetting the UV data within the 4x4 plate
			if (iGrassType < 0) iGrassType = 0;
			if (iGrassType > 15) iGrassType = 15;
			int iGrassH = iGrassType / 4;
			int iGrassW = iGrassType - (iGrassH * 4);
			float fOffsetU = iGrassW * 0.25f;
			float fOffsetV = iGrassH * 0.25f;

			// we cycle through the pattern that stored the original veg mesh UVs
			iLocalVertexCount = iVertexCount;
			for (int fGrassVert = 0; fGrassVert < iGrassClumpVerts; fGrassVert++)
			{
				// set grass quad position
				float fVertU = g_fUVPatternForVegMesh[fGrassVert][0] + fOffsetU;
				float fVertV = g_fUVPatternForVegMesh[fGrassVert][1] + fOffsetV;

				// apply the final UVs for this grass mesh
				SetVertexDataUV(iLocalVertexCount,fVertU,fVertV);
				iLocalVertexCount++;
			}				
		}
		#endif	

		// ensure vertex count keeps pace with transition through data!
		iVertexCount = iLocalVertexCount;
	}
	UnlockVertexData();
	#ifdef WICKEDENGINE
	sObject* pObject = GetObjectData(iVegObj);
	if (pObject->ppMeshList)
	{
		sMesh* pMesh = pObject->ppMeshList[0];
		if (pMesh) WickedCall_UpdateMeshVertexData(pMesh);
	}
	#endif
}

// Takes a float and returns an integer clamped between 0 and grass memblock max width
 int ClampToMemblockRes(float fValue)
{
	#ifdef PAULNEWGRASSSYSTEM
	 return 0;
	#endif

	if (fValue < 0) return 0;
	if (fValue >= iGrassMemBlockRes) return iGrassMemBlockRes - 1;
	return (int)fValue;
}

 bool GridSquareContainsGrass(int iX, int iZ)
{
	#ifdef PAULNEWGRASSSYSTEM
	 return(false);
	#endif

	// Work out area in the grass existance memblock we're looking at
	float fMemStepRatio = iVegAreaWidth * iGrassMemBlockRes/fWorldSize;
	int iMemLeft = ClampToMemblockRes(iX * fMemStepRatio);	
	int iMemRight = ClampToMemblockRes(iMemLeft + fMemStepRatio);
	int iMemBack = ClampToMemblockRes(iZ * fMemStepRatio);
	int iMemFront = ClampToMemblockRes(iMemBack + fMemStepRatio);
		
	int iMemPos, iMemX, iMemZ;
	for (iMemX = iMemLeft; iMemX <= iMemRight; iMemX ++){
		for (iMemZ = iMemBack; iMemZ <= iMemFront; iMemZ ++)
		{
			iMemPos = 4+4+4+((iMemX + iMemZ * iGrassMemBlockRes)*4);
			#ifdef WICKEDENGINE
			if (ReadMemblockByte(iGrassMemBlock,iMemPos+2) > 0) 
			#else
			if (ReadMemblockByte(iGrassMemBlock,iMemPos+2) > iGrassMemblockThreshhold) 
			#endif
			{
				return true;
			}
		}
	}
	return false;
}

 void RefreshGridExistArray(void)
{	
	#ifdef PAULNEWGRASSSYSTEM
	 return;
	#endif

	 if (!bGridMade) return;

	for (int iX = 0; iX < iGridDimension; iX++){				
		for (int iZ = 0; iZ < iGridDimension; iZ++){			
			bGridExist[iX][iZ] = GridSquareContainsGrass(iX,iZ);
		}
	}
}

void MakeVegetationGrid(int iVegPerMeshIN,  float fVegWidthIN, float fVegHeightIN, int iVegAreaWidthIN, int iGridSizeIN, int iTerrainID, int iOptionalSkipGrassMemblock, bool bBelowWater)
{
	#ifdef PAULNEWGRASSSYSTEM
	return;
	#endif

	if (!bResourcesSet) return;
	if (GetMeshExist(iGrassObj) == 0) return; // Safety check

	bDisplayBelowWater = bBelowWater;

	// Set our 'old' positions miles away so we force a position update on the first VEG UPDATE call
	fOldViewPointX = -10000;
	fOldViewPointZ = -10000;
    iOldLeftEdge = -10000;
	iOldRightEdge = -10000;
	iOldFrontEdge = -10000;
	iOldBackEdge = -10000;

	// If the grid is already made to the exact same spec (quantity and size) then there's no point remaking it
	if (bGridMade && iVegPerMesh == iVegPerMeshIN && iGridSize == iGridSizeIN && fVegWidth == fVegWidthIN && fVegHeight == fVegHeightIN){
		// Update the grid square visibility array as new grass may have been painted
		RefreshGridExistArray();
		SetVegetationGridVisible(true);
		// Let's refresh all veg objects already in place unless we're in superflat mode (iTerrainID = 0) as heights
		// may have changed
		#ifdef WICKEDENGINE
		// for wicked and flat terrain, we need to call UpdateVeg Patch and the condition below blocks it
		#else
		if (iTerrainID != 0)
		#endif
		{
			int iX,iZ;
			for (int iObj = iGridObjectStart; iObj < iGridObjectEnd + 1; iObj++)
			{
				if (ObjectExist(iObj) == 1)
				{
					iX = (int)(ObjectPositionX(iObj)/iVegAreaWidth);
					iZ = (int)(ObjectPositionZ(iObj)/iVegAreaWidth);
					if (bGridExist[iX][iZ]) UpdateVegPatch(iObj,iTerrainID,ObjectPositionX(iObj),ObjectPositionZ(iObj));
				}
			}
		}
		return;
	}

	DeleteVegetationGrid();

	if (iGridSizeIN < 1) return;	 // Can't make a grid if we have no dimensions
	if (iVegAreaWidthIN < 1) return; // Can't make a grid if each veg patch has no size
	if (iVegPerMeshIN < 1) return;   // Can't make a grid if we don't have a valid number of clumps per patch
	
	int iVegObj = iGridObjectStart;
    iVegPerMesh = iVegPerMeshIN;
	srand(12345);	

	// Set the veg area and width values
	iVegAreaWidth = iVegAreaWidthIN; 
	iGridSize = iGridSizeIN;
	fVegWidth = fVegWidthIN;
	fVegHeight = fVegHeightIN;
	iVegHalfAreaWidth = iVegAreaWidth/2;
	iVegAreaRad = iVegHalfAreaWidth + iVegAreaWidth * (iGridSize/2 - 1);
		
	LockVertexDataForMesh(iGrassObj);
	// Extract number of verts in this mesh for later use
	iGrassClumpVerts = GetVertexDataVertexCount();
	// Figure out which verts are on the extremities
	float fLeft = 10000;
	float fRight = -10000;
	float fFront = -10000;
	float fBack = 10000;
	float fTop = -10000;
	float fBottom = 10000;
	float fVertX,fVertY,fVertZ;
	for (int iV = 0; iV < iGrassClumpVerts; iV++)
	{
		fVertX = (GetVertexDataPositionX(iV));
		fVertY = (GetVertexDataPositionY(iV));
		fVertZ = (GetVertexDataPositionZ(iV));
		if (fVertX < fLeft)  { fLeft = fVertX;   iLeftVert = iV;}
		if (fVertX > fRight) { fRight = fVertX;  iRightVert = iV;}
		if (fVertY < fBottom){ fBottom = fVertY; iBottomVert = iV;}
		if (fVertY > fTop)   { fTop = fVertY;    iTopVert = iV;}
		if (fVertZ < fBack)  { fBack = fVertZ;   iBackVert = iV;}
		if (fVertZ > fFront) { fFront = fVertZ;  iFrontVert = iV;}
	}
	UnlockVertexData();

	// Now make our veg grid patches
#ifdef WICKEDENGINE
	WickedCall_PresetObjectCreateOnDemand(true);
#endif

	for (int iX = 0; iX < iGridSize; iX++)
	{
		for (int iZ = 0; iZ < iGridSize; iZ++)
		{
			MakeVegPatch(iVegObj,fVegHeight,fVegWidth,iX,iZ,iOptionalSkipGrassMemblock);
			iVegObj++;
		}
	}
#ifdef WICKEDENGINE
	WickedCall_PresetObjectCreateOnDemand(false);
#endif
	iGridObjectEnd = iVegObj - 1;

	// Define an array of bools which specify whether a grid block is populated or not
	iGridDimension = (int)(fWorldSize/iVegAreaWidth);
	//bGridExist = new bool*[iGridDimension]; // lee - 170614 - crashed when put grass on lower right edge
	bGridExist = new bool*[iGridDimension+1];
	//for (int iG = 0; iG < iGridDimension; iG++)
	for (int iG = 0; iG <= iGridDimension; iG++)
	{
		bGridExist[iG] = new bool[iGridDimension+1];
	}

	// lee - 170614 - ensure its completely clear
	for (int iX = 0; iX <= iGridDimension; iX++){				
		for (int iZ = 0; iZ <= iGridDimension; iZ++){			
			bGridExist[iX][iZ] = false;
		}
	}

	bGridMade = true; //PE: Moved here so grid exists are updated.

	RefreshGridExistArray();
		
}

bool bFullVegUpdate = true;

void MakeVegetationGridQuick(int iVegPerMeshIN, float fVegWidthIN, float fVegHeightIN, int iVegAreaWidthIN, int iGridSizeIN, int iTerrainID, int iOptionalSkipGrassMemblock, bool bBelowWater)
{
	#ifdef PAULNEWGRASSSYSTEM
	return;
	#endif

	if (!bResourcesSet) return;
	if (GetMeshExist(iGrassObj) == 0) return; // Safety check

	bDisplayBelowWater = bBelowWater;

	// Set our 'old' positions miles away so we force a position update on the first VEG UPDATE call
	fOldViewPointX = -10000;
	fOldViewPointZ = -10000;
	iOldLeftEdge = -10000;
	iOldRightEdge = -10000;
	iOldFrontEdge = -10000;
	iOldBackEdge = -10000;

	// If the grid is already made to the exact same spec (quantity and size) then there's no point remaking it
	if (bGridMade && iVegPerMesh == iVegPerMeshIN && iGridSize == iGridSizeIN && fVegWidth == fVegWidthIN && fVegHeight == fVegHeightIN) 
	{
		// Update the grid square visibility array as new grass may have been painted
		RefreshGridExistArray();
		SetVegetationGridVisible(true);
		// Let's refresh all veg objects already in place unless we're in superflat mode (iTerrainID = 0) as heights may have changed
		#ifdef WICKEDENGINE
		// for wicked and flat terrain, we need to call UpdateVeg Patch and the condition below blocks it
		#else
		if (iTerrainID != 0)
		#endif
		if(bFullVegUpdate)
		{
			bFullVegUpdate = false;
			int iX, iZ;
			for (int iObj = iGridObjectStart; iObj < iGridObjectEnd + 1; iObj++)
			{
				if (ObjectExist(iObj) == 1)
				{
					iX = (int)(ObjectPositionX(iObj) / iVegAreaWidth);
					iZ = (int)(ObjectPositionZ(iObj) / iVegAreaWidth);
					if (bGridExist[iX][iZ]) UpdateVegPatch(iObj, iTerrainID, ObjectPositionX(iObj), ObjectPositionZ(iObj));
				}
			}
		}
		return;
	}

	// create new veg from scratch (happens when terrain is loaded, height data availabke and need height corrected grass)
	DeleteVegetationGrid();

	if (iGridSizeIN < 1) return;	 // Can't make a grid if we have no dimensions
	if (iVegAreaWidthIN < 1) return; // Can't make a grid if each veg patch has no size
	if (iVegPerMeshIN < 1) return;   // Can't make a grid if we don't have a valid number of clumps per patch

	int iVegObj = iGridObjectStart;
	iVegPerMesh = iVegPerMeshIN;
	srand(12345);

	// Set the veg area and width values
	iVegAreaWidth = iVegAreaWidthIN;
	iGridSize = iGridSizeIN;
	fVegWidth = fVegWidthIN;
	fVegHeight = fVegHeightIN;
	iVegHalfAreaWidth = iVegAreaWidth / 2;
	iVegAreaRad = iVegHalfAreaWidth + iVegAreaWidth * (iGridSize / 2 - 1);

	LockVertexDataForMesh(iGrassObj);
	// Extract number of verts in this mesh for later use
	iGrassClumpVerts = GetVertexDataVertexCount();
	// Figure out which verts are on the extremities
	float fLeft = 10000;
	float fRight = -10000;
	float fFront = -10000;
	float fBack = 10000;
	float fTop = -10000;
	float fBottom = 10000;
	float fVertX, fVertY, fVertZ;
	for (int iV = 0; iV < iGrassClumpVerts; iV++)
	{
		fVertX = (GetVertexDataPositionX(iV));
		fVertY = (GetVertexDataPositionY(iV));
		fVertZ = (GetVertexDataPositionZ(iV));
		if (fVertX < fLeft) { fLeft = fVertX;   iLeftVert = iV; }
		if (fVertX > fRight) { fRight = fVertX;  iRightVert = iV; }
		if (fVertY < fBottom) { fBottom = fVertY; iBottomVert = iV; }
		if (fVertY > fTop) { fTop = fVertY;    iTopVert = iV; }
		if (fVertZ < fBack) { fBack = fVertZ;   iBackVert = iV; }
		if (fVertZ > fFront) { fFront = fVertZ;  iFrontVert = iV; }
	}
	UnlockVertexData();

	// Now make our veg grid patches
#ifdef WICKEDENGINE
	WickedCall_PresetObjectCreateOnDemand(true);
#endif
	for (int iX = 0; iX < iGridSize; iX++)
	{
		for (int iZ = 0; iZ < iGridSize; iZ++)
		{
			MakeVegPatch(iVegObj, fVegHeight, fVegWidth, iX, iZ, iOptionalSkipGrassMemblock);
			iVegObj++;
		}
	}
#ifdef WICKEDENGINE
	WickedCall_PresetObjectCreateOnDemand(false);
#endif
	iGridObjectEnd = iVegObj - 1;

	// Define an array of bools which specify whether a grid block is populated or not
	iGridDimension = (int)(fWorldSize / iVegAreaWidth);
	//bGridExist = new bool*[iGridDimension]; // lee - 170614 - crashed when put grass on lower right edge
	bGridExist = new bool*[iGridDimension + 1];
	//for (int iG = 0; iG < iGridDimension; iG++)
	for (int iG = 0; iG <= iGridDimension; iG++)
	{
		bGridExist[iG] = new bool[iGridDimension + 1];
	}

	// lee - 170614 - ensure its completely clear
	for (int iX = 0; iX <= iGridDimension; iX++) {
		for (int iZ = 0; iZ <= iGridDimension; iZ++) {
			bGridExist[iX][iZ] = false;
		}
	}

	bGridMade = true; //PE: Moved here to make sure grass gets visible, and gridexists update.
	RefreshGridExistArray();

	int iX, iZ;
	for (int iObj = iGridObjectStart; iObj < iGridObjectEnd + 1; iObj++)
	{
		if (ObjectExist(iObj) == 1)
		{
			iX = (int)(ObjectPositionX(iObj) / iVegAreaWidth);
			iZ = (int)(ObjectPositionZ(iObj) / iVegAreaWidth);
			if (bGridExist[iX][iZ]) UpdateVegPatch(iObj, iTerrainID, ObjectPositionX(iObj), ObjectPositionZ(iObj));
		}
	}
	SetVegetationGridVisible(true);

}

 void UpdateVegZone(bool bSuperFlat, float fX1, float fZ1, float fX2, float fZ2, int iTerrainID, float fSuperFlatHeight)
{
	#ifdef PAULNEWGRASSSYSTEM
	 return;
	#endif

	if (!bGridMade) return; // Can't update if grid doesn't exist

	// Work out the update zone grid squares
	int iLeftEdge = (int)fX1 - iVegHalfAreaWidth; 	
	int iRightEdge = (int)fX2 + iVegHalfAreaWidth; 
	int iBackEdge = (int)fZ1 - iVegHalfAreaWidth;
	int iFrontEdge = (int)fZ2 + iVegHalfAreaWidth;
	if (iLeftEdge < iVegHalfAreaWidth) iLeftEdge = iVegHalfAreaWidth;
	if (iRightEdge > fWorldSize - iVegHalfAreaWidth) iRightEdge = (int)(fWorldSize - iVegHalfAreaWidth);
	if (iBackEdge < iVegHalfAreaWidth) iBackEdge = iVegHalfAreaWidth;
	if (iFrontEdge > fWorldSize - iVegHalfAreaWidth) iFrontEdge = (int)(fWorldSize - iVegHalfAreaWidth);
 
	// Now round this off to veggie grid positions
	iLeftEdge /= iVegAreaWidth;
	iRightEdge /= iVegAreaWidth;
	iBackEdge /= iVegAreaWidth;
	iFrontEdge /= iVegAreaWidth;

	// Cycle through our grid and process each veg obj
	int iVegObj;
	float fVegX,fVegZ;
	for (int iX = iLeftEdge; iX < iRightEdge + 1; iX++)
	{		
		for (int iZ = iBackEdge; iZ < iFrontEdge + 1; iZ++)
		{			
			// Because this function is called when grass is being newly painted in F9 mode, we'll also need to
			// update the grid existance array for this updated zone			
			bGridExist[iX][iZ] = GridSquareContainsGrass(iX,iZ);		
			if (bGridExist[iX][iZ])
			{
				// Work out which veg obj should be at this location and update it
				iVegObj = iGridObjectStart + (iZ % iGridSize) + iGridSize * (iX % iGridSize);
				if (ObjectExist(iVegObj) == 1)
				{	
					// Safety check
					fVegX = (float)(iX * iVegAreaWidth + iVegHalfAreaWidth);
					fVegZ = (float)(iZ * iVegAreaWidth + iVegHalfAreaWidth);
					#ifdef WICKEDENGINE
					// or wicked and flat terrain, we need to call UpdateVeg Patch - old comment
					// leelee, needed the veg object in the right position so it can be detected in its XZ location ?!? When loading a new level with grass!
					PositionObject ( iVegObj, fVegX, fSuperFlatHeight, fVegZ);
					UpdateVegPatch ( iVegObj, iTerrainID, fVegX, fVegZ);
					#else
					if (bSuperFlat)
					{
						PositionObject(iVegObj,fVegX,fSuperFlatHeight,fVegZ);
					}
					else
					{
						UpdateVegPatch(iVegObj,iTerrainID,fVegX,fVegZ);
					}
					#endif
					ShowObject(iVegObj);
				}
			}
		}
	}
}

void UpdateVegetation(bool bSuperFlat, float fViewPointX, float fViewPointZ, int iTerrainID, float fSuperFlatHeight, int iDynamicShadowImage)
{
	#ifdef PAULNEWGRASSSYSTEM
	return;
	#endif

	if (!bGridMade) return; // Can't update if grid doesn't exist

	// Exit if we haven't moved
	if (fOldViewPointX == fViewPointX && fOldViewPointZ == fViewPointZ) return;
	fOldViewPointX = fViewPointX;
	fOldViewPointZ = fViewPointZ;

	// Work out the grid square around our player to populate with veggies
	int iLeftEdge = (int)fViewPointX - iVegAreaRad; 	
	int iRightEdge = (int)fViewPointX + iVegAreaRad; 
	int iBackEdge = (int)fViewPointZ - iVegAreaRad;
	int iFrontEdge = (int)fViewPointZ + iVegAreaRad;
	if (iLeftEdge < iVegHalfAreaWidth) iLeftEdge = iVegHalfAreaWidth;
	if (iRightEdge > fWorldSize - iVegHalfAreaWidth) iRightEdge = (int)(fWorldSize - iVegHalfAreaWidth);
	if (iBackEdge < iVegHalfAreaWidth) iBackEdge = iVegHalfAreaWidth;
	if (iFrontEdge > fWorldSize - iVegHalfAreaWidth) iFrontEdge = (int)(fWorldSize - iVegHalfAreaWidth);
 
	// Now round this off to veggie grid positions
	iLeftEdge /= iVegAreaWidth;
	iRightEdge /= iVegAreaWidth;
	iBackEdge /= iVegAreaWidth;
	iFrontEdge /= iVegAreaWidth;

	// Exit if our grid extremities haven't changed
	if (iLeftEdge == iOldLeftEdge && iRightEdge == iOldRightEdge && iBackEdge == iOldBackEdge && iFrontEdge == iOldFrontEdge) return;
	iOldLeftEdge = iLeftEdge;
	iOldRightEdge = iRightEdge;
	iOldBackEdge = iBackEdge;
	iOldFrontEdge = iFrontEdge;

	// Cycle through our grid and process each veg obj
	int iVegObj;
	float fVegX,fVegZ;
	for (int iX = iLeftEdge; iX < iRightEdge + 1; iX++)
	{
		for (int iZ = iBackEdge; iZ < iFrontEdge + 1; iZ++)
		{	
			// Work out which veg obj should be at this location
			iVegObj = iGridObjectStart + (iZ % iGridSize) + iGridSize * (iX % iGridSize);
			if (ObjectExist(iVegObj) == 1)
			{
				// Does this grid square have any grass? If not, hide the object now, otherwise show it
				if (bGridExist[iX][iZ])
				{
					// if determined for use, create it now
					#ifdef WICKEDENGINE
					WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_CURSOROBJECT);
					sObject* pVegObject = GetObjectData(iVegObj);
					if ( pVegObject )
					{
						if (pVegObject->wickedrootentityindex == 0)
						{
							WickedCall_AddObject(pVegObject);
							WickedCall_TextureObject(pVegObject,NULL);
							WickedCall_SetObjectAlphaRef(pVegObject, GRASSALPHACLIP); //PE: Same as tree/veg values.
							WickedCall_SetObjectCastShadows(pVegObject, false);
							SetObjectCull(iVegObj, 0);
							WickedCall_SetObjectCullmode(pVegObject);
						}
					}
					WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_NORMAL);
					#endif

					// show veg chunk
					ShowObject(iVegObj);

					// veg position and updating step
					fVegX = (float)(iX * iVegAreaWidth + iVegHalfAreaWidth);
					fVegZ = (float)(iZ * iVegAreaWidth + iVegHalfAreaWidth);
					#ifdef WICKEDENGINE
					// we certainly need the veg object to be at the world position
					if (abs(ObjectPositionX(iVegObj) - fVegX) > 1 || abs(ObjectPositionZ(iVegObj) - fVegZ) > 1)
					{
						// we joggle the veg object position to force a veg update (speeds up editing!)
						PositionObject(iVegObj, fVegX, fSuperFlatHeight, fVegZ);
						UpdateVegPatch(iVegObj, iTerrainID, fVegX, fVegZ);
					}
					#else
					// Work out the position for this veg obj. If it's already at this position, we don't need to process it.
					// If it isn't, then we'll move it there and setup
					if (abs(ObjectPositionX(iVegObj) - fVegX) > 1 || abs(ObjectPositionZ(iVegObj) - fVegZ) > 1)
					{ 
						if (bSuperFlat)
						{
							PositionObject(iVegObj,fVegX,fSuperFlatHeight,fVegZ);
						}
						else
						{
							UpdateVegPatch(iVegObj,iTerrainID,fVegX,fVegZ);
						}
					}							
					#endif
				}
				else
				{
					HideObject(iVegObj);
				}
			}
		}
	}
}

 void UpdateBlitzTerrain(float fViewPointX, float fViewPointZ, int iTerrainID, int iDynamicShadowImage)
{
	#ifdef PAULNEWGRASSSYSTEM
	 return;
	#endif

	UpdateVegetation(false, fViewPointX, fViewPointZ, iTerrainID, 0, iDynamicShadowImage);
}

 void UpdateSuperFlat(float fViewPointX, float fViewPointZ, float fHeight, int iDynamicShadowImage )
{
	#ifdef PAULNEWGRASSSYSTEM
	 return;
	#endif

	 UpdateVegetation(true, fViewPointX, fViewPointZ, 0, fHeight, iDynamicShadowImage);
}

 void UpdateVegZoneBlitzTerrain(float fX1, float fZ1, float fX2, float fZ2, int iTerrainID)
{
	#ifdef PAULNEWGRASSSYSTEM
	 return;
	#endif

	UpdateVegZone(false,fX1,fZ1,fX2,fZ2,iTerrainID,0);

	// LB: fixed a bug 'white grass when upgraded wicked' but probably not the best way!
	UpdateGrassTexture(0); // ensure newly added veg object has its texture
}

 void UpdateVegZoneSuperFlat(float fX1, float fZ1, float fX2, float fZ2, float fSuperFlatHeight)
{
	#ifdef PAULNEWGRASSSYSTEM
	return;
	#endif

	UpdateVegZone(true,fX1,fZ1,fX2,fZ2,0,fSuperFlatHeight);
}

 void UpdateGrassTexture ( int iPBRMode )
{
	#ifdef PAULNEWGRASSSYSTEM
	 return;
	#endif
	if (!bGridMade) return; // Can't update if grid doesn't exist
	for (int iObj = iGridObjectStart; iObj < iGridObjectEnd + 1; iObj++)
	{
		if (ObjectExist(iObj) == 1)
		{
			#ifdef WICKEDENGINE
			TextureObject ( iObj, 0, iGrassImg );
			sObject* pVegObject = GetObjectData ( iObj );
			if ( pVegObject )
			{
				WickedCall_SetObjectAlphaRef ( pVegObject, GRASSALPHACLIP ); //PE: Same as tree/veg values.
				WickedCall_SetObjectCastShadows ( pVegObject, false ); //PE: No shadows on veg for now.
			}
			#else
			if ( iPBRMode == 0 )
			{
				// non-PBR
				TextureObject(iObj,0,iGrassImg);
				//TextureObject(iObj,1,iShadowImg); // not used
				// texture stage 2 reserved for passed in shadow (not used)
			}
			else
			{
				// PBR
				TextureObject ( iObj, 0, iGrassImg ); //Diffuse
				TextureObject ( iObj, 6, iPBRCubeImg );//EnvironmentMap
			}
			#endif
		}
	}

	#ifdef WICKEDENGINE
	// no logic for this, editor creates grass, in-game does not affect it any different than editor normal activity in wicked grass
	#else

	#ifndef PRODUCTCLASSIC
	//PE: Cant do this in classic , as grass will show in editor, and F9 grass paint dont work.
	#ifndef PRODUCTV3
	// LB: also do not do in VRQ as it wipes out grass flag which we need to see grass in editor!
	bGridMade = false;
	#endif
	#endif

	#endif
}

void UpdateGrassShader ( int iNewShaderIndex )
{
	#ifdef PAULNEWGRASSSYSTEM
	return;
	#endif

	if (!bGridMade) return; // Can't update if grid doesn't exist
	for (int iObj = iGridObjectStart; iObj < iGridObjectEnd + 1; iObj++)
	{
		if (ObjectExist(iObj) == 1)
		{
			SetObjectEffect ( iObj, iNewShaderIndex );
		}
	}	
}

void HideVegetationGrid(void)
{
	#ifdef PAULNEWGRASSSYSTEM
	return;
	#endif

	SetVegetationGridVisible(false);
}

 void ShowVegetationGrid(void)
{
	#ifdef PAULNEWGRASSSYSTEM
	 return;
	#endif

	SetVegetationGridVisible(true);
}

 int GetFirstGrassObject(void)
{
	return iGridObjectStart;
}

 int GetLastGrassObject(void)
{
	return iGridObjectEnd;
}