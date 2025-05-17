//
// Wicked Calls - place Wicked commands here so can compile (cannot call from old graphics engine modules, conflicts with its own data types)
//

//#pragma optimize("", off)
// Includes
#include "stdafx.h"
#include "wickedcalls.h"
#include <vector>
#include <unordered_map>

#include "..\Dark Basic Pro SDK\Shared\Objects\CObjectManagerWicked.h"
#include "M-Entity.h"
#include "CFileC.h"
#include "CImageC.h"
#include "CCameraC.h"

#include "master.h"
extern Master master;

#include "GGTerrain/GGTerrain.h"
#include "gameguru.h"

// OPTICK Performance
#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

#define ONLY_USE_OUTLINE_HIGHLIGHT
#define MATCHCLASSICROTATION

enum EDITORSTENCILREF
{
	EDITORSTENCILREF_CLEAR = 0x00,
	EDITORSTENCILREF_HIGHLIGHT_OBJECT = 0x01,
	EDITORSTENCILREF_HIGHLIGHT_MATERIAL = 0x02,
	EDITORSTENCILREF_HIGHLIGHT_OBJECT_RED = 0x03,
	EDITORSTENCILREF_HIGHLIGHT_OBJECT_BLUE = 0x04,
	EDITORSTENCILREF_LAST = 0x0F,
};

// Prototypes
void CameraAnglesFromMatrix(GGMATRIX* pmatMatrix, GGVECTOR3* pVecAngles);
void WickedCall_DrawObjctBox(sObject* pObject, XMFLOAT4 color, bool bThickLine = false, bool ForceBox = false);
bool bUseEditorOutlineSelection(void);
DARKSDK_DLL int ObjectExist(int iID);
void* GetObjectsInternalData(int iID);
int iGetgrideditselect(void);

// Name spaces
using namespace std;
using namespace wiGraphics;
using namespace wiScene;
using namespace wiECS;

// Object loading structure
struct WickedLoaderState
{
	Scene* scene;
	uint64_t storeMasterRootEntityIndex;
	unordered_map<int, Entity> entityMap;  // node/frame -> entity
	unordered_map<int, Entity> entityMeshMap;  // node/frame -> entity
};

// Globals
std::string g_pWickedTexturePath;
int g_iWickedLayerMaskPreference = GGRENDERLAYERS_NORMAL;
int g_iWickedLayerMaskOptionalLimb = -1;
float g_iWickedUScalePreference = 1.0f;
float g_iWickedVScalePreference = 1.0f;
bool g_bWickedCreateOnlyWhenUsed = false;
bool g_bWickedIgnoreTextureInfo = false;
bool g_bWickedUseImagePtrInsteadOfTexFile = false;
bool g_bDisplayWarnings = true;
int g_iWickedPutInEmissiveMode = 0;
XMFLOAT4 g_lastMousePos = { 0,0,0,0 };
uint64_t g_hovered_entity = 0;
uint64_t g_selected_entity = 0;
sObject* g_selected_pobject = NULL;
sObject* g_highlight_pobject = NULL;
sObject* g_hovered_pobject = NULL;
sObject* g_ray_pobject = NULL;

uint64_t g_hovered_dot_entity = 0;
sObject* g_hovered_dot_pobject = NULL;
bool g_bhovered_dot = false;
float fLastHitPosition[4] = { 0,0,0,0 };

int g_selected_editor_objectID = 0;
sObject* g_selected_editor_object = NULL;
XMFLOAT4 g_selected_editor_color = XMFLOAT4(0.25f, 1.0f, 0.25f, 0.5f);

std::vector<int> g_ObjectHighlightList;

// stores original resolution of editor when enter VR, as need to restore it after VR
int g_iStoreRenderResolutionWidth = -1;
int g_iStoreRenderResolutionHeight = -1;
float fLODMultiplier = 1.0f;


bool g_bLightShaftState = true;
bool g_bLensFlareState = true;
void SetLightShaftState(bool bState);
bool GetLightShaftState(void);
bool GetLensFlareState();
void SetLensFlareState(bool bState);

extern CObjectManager m_ObjectManager;
extern LPGGIMMEDIATECONTEXT m_pImmediateContext;
extern wiSprite* pboundbox[4];
extern Entity g_entityCameraLight;
extern Entity g_entityThumbLight;
extern Entity g_entityThumbLight2;
extern Entity g_entitySunLight;
extern Entity g_weatherEntityID;
extern bool bImGuiGotFocus;
extern bool bProceduralLevel;
extern bool bImGuiInTestGame;
bool bRenderTargetHasFocus = false;

float fWickedMaxCenterTest = 0.0f;
uint32_t iCulledPointShadows = 0;
uint32_t iCulledSpotShadows = 0;
uint32_t iCulledAnimations = 0;
uint32_t iRenderedPointShadows = 0;
uint32_t iRenderedSpotShadows = 0;

bool bEnable30FpsAnimations = false;
bool bEnableTerrainChunkCulling = true;
bool bEnablePointShadowCulling = true;
bool bEnableSpotShadowCulling = true;
bool bEnableObjectCulling = true;
bool bEnableAnimationCulling = true;
bool bShadowsInFrontTakesPriority = false;

bool bShadowsLowestLOD = false;
bool bProbesLowestLOD = false;
bool bRaycastLowestLOD = false;
bool bPhysicsLowestLOD = false;
bool bThreadedPhysics = false;
bool bHideWeapons = false;
bool bHideWeaponsMuzzle = false;
bool bHideWeaponsSmoke = false;
bool bReflectionsLowestLOD = false;
int iTracerPosition = 0;
int iEnterGodMode = 0;
bool bTmpTesting = false;

float fWickedCallShadowFarPlane = DEFAULT_FAR_PLANE;


// Image Management
std::vector<sImageList> g_imageList;
std::string g_rootFolder;

void WickedCall_InitImageManagement(LPSTR pRootFolder)
{
	// clear image list
	g_imageList.clear();
	g_rootFolder = pRootFolder;
}

void WickedCall_FreeImage(sImageList* pImage)
{
	if ( pImage )
	{
		if (pImage->pName)
		{
			// first release wicked resource (turns out it maintains a list referenced by name)
			char pRealNameToDeleteFromWickedResource[MAX_PATH];
			strcpy(pRealNameToDeleteFromWickedResource, pImage->pName);
			GG_GetRealPath(pRealNameToDeleteFromWickedResource, 0);
			wiResourceManager::FreeResource(pRealNameToDeleteFromWickedResource);

			// and then the string holding the name
			delete pImage->pName;
			pImage->pName = NULL;
		}
		if (pImage->image != NULL)
		{
			// free wicked resource
			//wiResourceManager::Clear() <-- clears everything!!
			//pImage->image.swap(); what frees all resources created with the wiResourceManager::Load call?
			pImage->image = NULL;
		}
	}
}

void WickedCall_FreeAllImagesOfType(eImageResType eType)
{
	// clear all image data from list
	for (int i = 0; i < g_imageList.size(); i++)
	{
		sImageList* pImage = &g_imageList[i];
		if ( pImage->eType == eType )
		{
			WickedCall_FreeImage(pImage);
		}
	}
}

void WickedCall_GetRelativeAfterRoot(std::string pFilename, LPSTR pFullRelativeLocationFilename)
{
	// account for current directory location (as foldera\lee.png is different than folderb\lee.png)
	char pCurrentDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, pCurrentDir);
	int iChopRootFolder = strlen(g_rootFolder.c_str()) + strlen("Files\\") + 1;
	char pRelativePathAfterRoot[MAX_PATH];
	strcpy(pRelativePathAfterRoot, "");
	int iCurrentDirLength = strlen(pCurrentDir);
	if (iChopRootFolder < iCurrentDirLength)
	{
		// create new filename reference that includes the full relative location
		strcpy(pRelativePathAfterRoot, pCurrentDir + iChopRootFolder);
		strcpy(pFullRelativeLocationFilename, pRelativePathAfterRoot);
		strcat(pFullRelativeLocationFilename, "\\");
		strcat(pFullRelativeLocationFilename, pFilename.c_str());
	}
	else
	{
		// create new filename, but no additional prefix from root to the path used here
		strcpy(pFullRelativeLocationFilename, pFilename.c_str());
	}
}

int WickedCall_FindImageIndexInList(std::string pFilenameToFind, LPSTR pFullRelativeLocationFilename)
{
	// resulting image index
	int iImageIndex = -1;

	// see if this image has already been loaded
	if (pFilenameToFind.length() > 0)
	{
		char pReturnedRelativeLocationFilename[MAX_PATH];
		WickedCall_GetRelativeAfterRoot(pFilenameToFind, pReturnedRelativeLocationFilename);
		if (pFullRelativeLocationFilename) strcpy(pFullRelativeLocationFilename, pReturnedRelativeLocationFilename);
		for (int i = 0; i < g_imageList.size(); i++)
		{
			LPSTR pThisImageFilename = g_imageList[i].pName;
			if (pThisImageFilename)
			{
				if (stricmp(pThisImageFilename, pReturnedRelativeLocationFilename) == NULL)
				{
					iImageIndex = i;
					break;
				}
			}
		}
	}

	// return result
	return iImageIndex;
}

void WickedCall_AddImageToList(std::shared_ptr<wiResource> image, eImageResType eType, std::string pFilenameRef, int iKbused)
{
	sImageList newImage;
	newImage.image = image;
	newImage.eType = eType;
	newImage.iMemUsedKB = iKbused;
	newImage.pName = new char[strlen(pFilenameRef.c_str()) + 1];
	strcpy(newImage.pName, pFilenameRef.c_str());

	// add loaded image to existing list slot, or add a new one
	int i = 0;
	for (; i < g_imageList.size(); i++)
	{
		if (g_imageList[i].pName == NULL)
		{
			g_imageList[i] = newImage;
			break;
		}
	}
	if (i == g_imageList.size())
		g_imageList.push_back(newImage);
}

int total_mem_from_load = 0;
std::shared_ptr<wiResource> WickedCall_LoadImage(std::string pFilenameToLoadIN, eImageResType eType)
{
	//PE: Prevent dublicate textures even if using different names.
	//PE: Scan all our images and make a text file including filename+CRC64 of the file.
	//PE: Load this text file into a vector.
	//PE: Find filname in vector and add the CRC64 to g_imageList when adding a image.
	//PE: Below also lookup filename CRC64 and check against the g_imageList CRC64 to also reused image.
	if (pFilenameToLoadIN.length() <= 0)
		return NULL; //PE: We get this alot from DISPLACEMENTMAP ... 

	// when gdividetexturesize is 0, we are not using textures, so use a dummy texture (tests performance against using too LARGE a texture set)
	std::string pFilenameToLoad = pFilenameToLoadIN;
	if (g.gdividetexturesize == 0)
	{
		pFilenameToLoad = "editors\\gfx\\notexture.dds";
	}

	std::shared_ptr<wiResource> image = NULL;
	char pFullRelativeLocationFilename[MAX_PATH];
	int iImageIndex = WickedCall_FindImageIndexInList(pFilenameToLoad,pFullRelativeLocationFilename);
	if (iImageIndex != -1)
	{
		// found image
		image = g_imageList[iImageIndex].image;
	}
	else
	{
		// quickly reject nonesense filenames
		char pRealFilenameToLoad[MAX_PATH];
		strcpy(pRealFilenameToLoad, pFilenameToLoad.c_str());
		if ( pRealFilenameToLoad[strlen(pRealFilenameToLoad)-1] == '\\'
		|| strnicmp ( pRealFilenameToLoad + strlen(pRealFilenameToLoad) - 5, "\\.dds", 5 ) == NULL 
		|| strnicmp ( pRealFilenameToLoad + strlen(pRealFilenameToLoad) - 5, "\\.png", 5 ) == NULL 
		|| strlen ( pRealFilenameToLoad ) <= 4 )
		{
			// is not a filename that makes sense, reject load
			return NULL;
		}

		// first get real filename
		GG_GetRealPath(pRealFilenameToLoad, 0);
		pFilenameToLoad = pRealFilenameToLoad;

		bool bDecrypted = false;
		char VirtualFilename[_MAX_PATH];
		strcpy(VirtualFilename, pFilenameToLoad.c_str());
		//if (strstr(VirtualFilename, "gamecore\\bulletholes") != NULL
		//|| strstr(VirtualFilename, "gamecore\\decals") != NULL)
		//{
		//	// ignore some folders that are not encrypted
		//}
		//else
		//{
		extern bool CheckForWorkshopFile(LPSTR);
		CheckForWorkshopFile (VirtualFilename);
		g_pGlob->Decrypt(VirtualFilename);
		bDecrypted = true;
		//}

		//PE: We are calling this very often trying to locate textures, no need to bother wicked if not exists.
		bool bFileExists = false;
		//HANDLE hfile = GG_CreateFile(pFilenameToLoad.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE hfile = GG_CreateFile(VirtualFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hfile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hfile);
			bFileExists = true;
		}
		if( bFileExists )
		{
			// if not, load it
			wiResourceManager::SetErrorCode(0);
			DARKSDK int SMEMAvailable(int iMode);
			int startmem = SMEMAvailable(1);

			// handle encrypted image files when loading
			//image = wiResourceManager::Load(pFilenameToLoad);
			std::vector<uint8_t> data;
			if (wiHelper::FileRead(VirtualFilename, data))
			{
				image = wiResourceManager::Load(pFilenameToLoad, 0, data.data(), data.size());
				data.clear();
			}
			if (image != NULL)
			{
				// add image list item
				int endmem = SMEMAvailable(1);
				total_mem_from_load += (endmem - startmem);
				WickedCall_AddImageToList(image, eType, pFullRelativeLocationFilename, (endmem - startmem));
			}
			else
			{
				// image failed to load - no need to add to list
				if (wiResourceManager::GetErrorCode() == 1)
				{
					if (g_bDisplayWarnings)
					{
						char pFriendlyMessage[MAX_PATH];
						sprintf(pFriendlyMessage, "The DDS texture (%s) could not be loaded into MAX, likely an unknown or 24-bit format. Re-export the DDS as a 32-bit format.", pFilenameToLoad.c_str());
						MessageBoxA(NULL, pFriendlyMessage, "TinyDDS Load Failure", MB_OK);
					}
				}
			}
		}

		// and re-encrypt before proceeding
		if (bDecrypted == true)
		{
			g_pGlob->Encrypt(VirtualFilename);
		}
	}

	// return new or existing image resource
	return image;
}

std::shared_ptr<wiResource> WickedCall_LoadImage(std::string pFilenameToLoad)
{
	return WickedCall_LoadImage(pFilenameToLoad, IMAGERES_LEVEL);
}

void WickedCall_DeleteImage(std::string pFilenameToDelete)
{
	int iImageIndex = WickedCall_FindImageIndexInList(pFilenameToDelete,NULL);
	if (iImageIndex != -1)
	{
		WickedCall_FreeImage(&g_imageList[iImageIndex]);
	}
}


// Functions
bool bNoHierarchySorting = false;
bool bUseInstancing = false;
int iUseMasterObjectID = 0;
bool bNextObjectMustBeClone = false;
void WickedCall_LoadNode(sFrame* pFrame, Entity parent, Entity root, WickedLoaderState& state)
{
	// only if have a frame
	if (pFrame==NULL )
		return;

	// setup vars
	//auto& node = state.gltfModel.nodes[nodeIndex];
	Scene& scene = *state.scene;
	Entity entity = INVALID_ENTITY;

	// the DBO mesh needs to have geometry, or not point creating object (just the entity further down)
	bool bUseFrameMatrix = true;
	sMesh* pDBOMesh = pFrame->pMesh;

	//PE: Make sure not to add more objects then needed.
	if (pDBOMesh)
	{
		pDBOMesh->pFrameAttachedTo = pFrame;
		if (pFrame->bIgnoreMesh)
		{
			pFrame->wickedobjindex = 0;
			// recurse through all frames
			if (pFrame->pChild) WickedCall_LoadNode(pFrame->pChild, entity, root, state);
			if (pFrame->pSibling) WickedCall_LoadNode(pFrame->pSibling, parent, root, state);
			return;
		}
	}

	if (pDBOMesh && pDBOMesh->dwVertexCount > 0 && pDBOMesh->iPrimitiveType == GGPT_TRIANGLELIST ) // pDBOMesh->dwIndexCount > 0 ) nned to support indexless models
	{
		// Create object to hold mesh
		entity = scene.Entity_CreateObject(pFrame->szName);
		ObjectComponent& object = *scene.objects.GetComponent(entity);

		// store object entity ID reference in frame (for later use by texture function amongst others)
		pFrame->wickedobjindex = entity;

		state.entityMeshMap[pFrame->iID] = entity;
		// assign layer to object
		wiScene::LayerComponent& layer = *scene.layers.GetComponent(entity);

		// leelee, find out why I cannot set this after object added to scene!
		if ( g_iWickedLayerMaskOptionalLimb == -1 || (g_iWickedLayerMaskOptionalLimb == pFrame->iID ) )
			layer.layerMask = g_iWickedLayerMaskPreference;

		// create mesh
		wiECS::Entity meshEntity;
		//PE: InstanceObject
		if (bUseInstancing && pDBOMesh->master_wickedmeshindex > 0 )
		{
			meshEntity = pDBOMesh->master_wickedmeshindex;
			pDBOMesh->bInstanced = true;
		}
		else
		{
			meshEntity = scene.Entity_CreateMesh("node_mesh");
			pDBOMesh->bInstanced = false;
		}
		wiScene::MeshComponent& mesh = *scene.meshes.GetComponent(meshEntity);
		pDBOMesh->wickedmeshindex = meshEntity;
		pDBOMesh->pFrameAttachedTo = pFrame;

		// associate mesh with object
		object.meshID = meshEntity;

		//#### PE: mesh setup start ####
		//PE: InstanceObject
		if (!(bUseInstancing && pDBOMesh->master_wickedmeshindex > 0))
		{
			// if model has no indices, assume triangles in list order
			if (pDBOMesh->dwIndexCount == 0)
			{
				// simple triangle list
				for (size_t i = 0; i < pDBOMesh->dwVertexCount; i += 3)
				{
					mesh.indices.push_back(i + 0);
					mesh.indices.push_back(i + 2);
					mesh.indices.push_back(i + 1);
				}
			}
			else
			{
				// go through all indices and copy to created mesh
				for (size_t i = 0; i < pDBOMesh->dwIndexCount; i += 3)
				{
					mesh.indices.push_back(pDBOMesh->pIndices[i + 0]);
					mesh.indices.push_back(pDBOMesh->pIndices[i + 2]);
					mesh.indices.push_back(pDBOMesh->pIndices[i + 1]);
				}
			}

			//TODO:
			//PE: IF (pDBOMesh->dwBoneCount > 0)
			//PE: And pMesh->dwFVF!=0 (dwFVF already set in dbo).
			//PE: Then we must support dwTU[3] and dwTU[4] 
			//PE: as wicked SkinVertex requere vertex_boneindices,vertex_boneweights to match vertex_positions .size.
			//PE: If not we get a crash in wicked "Pick" function.

			// if loaded as a DBO, the vertex format may not include weights and indices for the bone animation,
			// so apply those now to ensure all bone based meshes can be animated by wicked engine
			sOffsetMap offsetMap;
			GetFVFOffsetMapFixedForBones(pDBOMesh, &offsetMap);
			if (pDBOMesh->dwBoneCount > 0)
			{
				GetFVFOffsetMapFixedForBones(pDBOMesh, &offsetMap);
				if (offsetMap.dwTU[3] == 0 || offsetMap.dwTU[4] == 0)
				{
					// generate a new mesh that only includes pos, normal, textureUV0, bone indices and weights
					bool bGenerateNormals = true, bUsesTangents = false, bUsesBinormals = false, bUsesDiffuse = false, bUsesBoneData = true, bDoNotGenerateExtraData = false;
					GenerateExtraDataForMeshEx(pDBOMesh, bGenerateNormals, bUsesTangents, bUsesBinormals, bUsesDiffuse, bUsesBoneData, bDoNotGenerateExtraData);
					pDBOMesh->dwFVF = 0; // strangely this was not done in above function, but done here to avoid issues with legacy functionality
					GetFVFOffsetMapFixedForBones(pDBOMesh, &offsetMap);
				}
			}

			// flag to adjust mesh using transform provided (not all models require this)
			GGMATRIX* pmatMeshTransform = NULL;
			bool bTransformMeshByTransformProvided = false;
			//bool bOnlyTransformMeshIfHaveNoParent = false;
			//if (pFrame->pParent == NULL) bOnlyTransformMeshIfHaveNoParent = true; // the logic being all DBOs require this if the mesh has no parent frame?!?!
			//if ( pDBOMesh->dwBoneCount > 0 && bOnlyTransformMeshIfHaveNoParent == true )
			if (pDBOMesh->dwBoneCount > 0 && bTransformMeshByTransformProvided == true)
			{
				float fDet = 0.0f;
				GGMATRIX matInverseMeshTransform;
				GGMatrixInverse(&matInverseMeshTransform, &fDet, &pFrame->matOriginal);
				pmatMeshTransform = &matInverseMeshTransform;
			}
			else
			{
				GGMATRIX matIdentityTransform;
				GGMatrixIdentity(&matIdentityTransform);
				pmatMeshTransform = &matIdentityTransform;
			}

			// scan DBO for offsets to required mesh data
			for (size_t v = 0; v < pDBOMesh->dwVertexCount; v++)
			{
				if (offsetMap.dwZ > 0)
				{
					XMFLOAT3 pos = XMFLOAT3(0, 0, 0);
					GGVECTOR3 vecPos = *(GGVECTOR3*)((float*)pDBOMesh->pVertexData + offsetMap.dwX + (offsetMap.dwSize * v));
					GGVec3TransformCoord(&vecPos, &vecPos, pmatMeshTransform);
					pos.x = vecPos.x;
					pos.y = vecPos.y;
					pos.z = vecPos.z;
					mesh.vertex_positions.push_back(pos);
				}
				if (offsetMap.dwNZ > 0)
				{
					XMFLOAT3 nor = XMFLOAT3(0, 0, 0);
					GGVECTOR3 vecNorm = *(GGVECTOR3*)((float*)pDBOMesh->pVertexData + offsetMap.dwNX + (offsetMap.dwSize * v));
					GGVec3TransformNormal(&vecNorm, &vecNorm, pmatMeshTransform);
					nor.x = vecNorm.x;
					nor.y = vecNorm.y;
					nor.z = vecNorm.z;
					mesh.vertex_normals.push_back(nor);
				}
				if (offsetMap.dwDiffuse > 0)
				{
					//PE Wicked: Alpha , Blue , green , red
					#define GGCOLOR_ABGR(r,g,b,a) ((GGCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))
					DWORD color = *(DWORD*)((DWORD*)pDBOMesh->pVertexData + offsetMap.dwDiffuse + (offsetMap.dwSize * v));
					int r = (int)((color & 0x00FF0000) >> 16);
					int g = (int)((color & 0x0000FF00) >> 8);
					int b = (int)((color & 0x000000FF));
					int a = (int)((color & 0xFF000000) >> 24);
					color = GGCOLOR_ABGR(r, g, b, a);
					mesh.vertex_colors.push_back(color);
				}
				if (offsetMap.dwTU[2] > 0)
				{
					XMFLOAT4 tan = XMFLOAT4(0, 0, 0, 0);
					GGVECTOR3 vecTangent = *(GGVECTOR3*)((float*)pDBOMesh->pVertexData + offsetMap.dwTU[2] + (offsetMap.dwSize * v));
					GGVec3TransformNormal(&vecTangent, &vecTangent, pmatMeshTransform);
					tan.x = vecTangent.x;
					tan.y = vecTangent.y;
					tan.z = vecTangent.z;
					mesh.vertex_tangents.push_back(tan);
				}
				if (offsetMap.dwTU[0] > 0)
				{
					XMFLOAT2 tex = XMFLOAT2(0, 0);
					tex.x = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwTU[0] + (offsetMap.dwSize * v));
					tex.y = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwTV[0] + (offsetMap.dwSize * v));
					tex.x *= g_iWickedUScalePreference;
					tex.y *= g_iWickedVScalePreference;
					mesh.vertex_uvset_0.push_back(tex);
				}
				if (offsetMap.dwTU[3] > 0)
				{
					XMFLOAT4 boneweights = XMFLOAT4(0, 0, 0, 0);
					boneweights.x = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwTU[3] + (offsetMap.dwSize * v));
					boneweights.y = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwTV[3] + (offsetMap.dwSize * v));
					boneweights.z = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwTZ[3] + (offsetMap.dwSize * v));
					boneweights.w = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwTW[3] + (offsetMap.dwSize * v));
					mesh.vertex_boneweights.push_back(boneweights);
				}
				if (offsetMap.dwTU[4] > 0)
				{
					XMUINT4 boneindices = XMUINT4(0, 0, 0, 0);
					boneindices.x = (int)*(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwTU[4] + (offsetMap.dwSize * v));
					boneindices.y = (int)*(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwTV[4] + (offsetMap.dwSize * v));
					boneindices.z = (int)*(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwTZ[4] + (offsetMap.dwSize * v));
					boneindices.w = (int)*(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwTW[4] + (offsetMap.dwSize * v));
					mesh.vertex_boneindices.push_back(boneindices);
				}
			}

			// apply material to mesh
			mesh.subsets.push_back(wiScene::MeshComponent::MeshSubset());
			mesh.subsets.back().materialID = pDBOMesh->wickedmaterialindex;
			mesh.subsets.back().indexOffset = 0;
			mesh.subsets.back().indexCount = (uint32_t)mesh.indices.size();
			mesh.subsets.back().active = true;

			mesh.lodlevels = 0; //PE: NEWLOD
			if (pDBOMesh->dwIndexCountLOD1 > 0)
			{
				mesh.lodlevels = 1;
				mesh.subsets.push_back(wiScene::MeshComponent::MeshSubset());
				mesh.subsets.back().materialID = pDBOMesh->wickedmaterialindex;
				mesh.subsets.back().indexOffset = mesh.indices.size();
				mesh.subsets.back().indexCount = pDBOMesh->dwIndexCountLOD1;
				mesh.subsets.back().active = false;

				for (size_t i = 0; i < pDBOMesh->dwIndexCountLOD1; i += 3)
				{
					mesh.indices.push_back(pDBOMesh->pIndicesLOD1[i + 0]);
					mesh.indices.push_back(pDBOMesh->pIndicesLOD1[i + 2]);
					mesh.indices.push_back(pDBOMesh->pIndicesLOD1[i + 1]);
				}
			}
			if (pDBOMesh->dwIndexCountLOD2 > 0)
			{
				mesh.lodlevels = 2;
				mesh.subsets.push_back(wiScene::MeshComponent::MeshSubset());
				mesh.subsets.back().materialID = pDBOMesh->wickedmaterialindex;
				mesh.subsets.back().indexOffset = mesh.indices.size();
				mesh.subsets.back().indexCount = pDBOMesh->dwIndexCountLOD2;
				mesh.subsets.back().active = false;

				for (size_t i = 0; i < pDBOMesh->dwIndexCountLOD2; i += 3)
				{
					mesh.indices.push_back(pDBOMesh->pIndicesLOD2[i + 0]);
					mesh.indices.push_back(pDBOMesh->pIndicesLOD2[i + 2]);
					mesh.indices.push_back(pDBOMesh->pIndicesLOD2[i + 1]);
				}
			}
			if (pDBOMesh->dwIndexCountLOD3 > 0)
			{
				mesh.lodlevels = 3;
				mesh.subsets.push_back(wiScene::MeshComponent::MeshSubset());
				mesh.subsets.back().materialID = pDBOMesh->wickedmaterialindex;
				mesh.subsets.back().indexOffset = mesh.indices.size();
				mesh.subsets.back().indexCount = pDBOMesh->dwIndexCountLOD3;
				mesh.subsets.back().active = false;

				for (size_t i = 0; i < pDBOMesh->dwIndexCountLOD3; i += 3)
				{
					mesh.indices.push_back(pDBOMesh->pIndicesLOD3[i + 0]);
					mesh.indices.push_back(pDBOMesh->pIndicesLOD3[i + 2]);
					mesh.indices.push_back(pDBOMesh->pIndicesLOD3[i + 1]);
				}
			}
			
			//PE: Test Tessellation
			//mesh.tessellationFactor = 50.0; //PE: Many objects dont work with this and deform, disable for now
			

			// LB: ensure culling mode is set for mesh
			if (pDBOMesh->bCull == false)
				mesh.SetDoubleSided(true);
			else
				mesh.SetDoubleSided(false);

			// LB: default render distance bias is zero
			float fRenderOrderBias = 0.0f;
			WickedCall_SetRenderOrderBias(pDBOMesh, fRenderOrderBias);

			// if this mesh is associated with bones, must be added to Wicked armature
			// some bones exist in DBO that have no bine weights or indices, so ignore those (Uzi HUD)
			if (pDBOMesh->dwBoneCount > 0 && mesh.vertex_boneindices.size() > 0)
			{
				// we need one armature per 'mesh with bones' (as bone count differs for each mesh)
				if (pDBOMesh->wickedarmatureindex == 0)
				{
					// create armature to hold framework for bones and animation
					Entity armatureEntity = CreateEntity();
					scene.names.Create(armatureEntity) = "Armature";
					scene.layers.Create(armatureEntity);
					scene.transforms.Create(armatureEntity);
					ArmatureComponent& armature = scene.armatures.Create(armatureEntity);
					// calculate the inverse bone matrices for each bone (to transform the pose world matrix to the bone local space)
					armature.inverseBindMatrices.resize(pDBOMesh->dwBoneCount);
					for (int iB = 0; iB < pDBOMesh->dwBoneCount; iB++)
					{
						XMFLOAT4X4 inverseBindMatrixForFrame;
						sBone* pBone = &pDBOMesh->pBones[iB];
						GGMATRIX matBind;
						matBind = pBone->matTranslation;
						inverseBindMatrixForFrame._11 = matBind._11;
						inverseBindMatrixForFrame._12 = matBind._12;
						inverseBindMatrixForFrame._13 = matBind._13;
						inverseBindMatrixForFrame._14 = matBind._14;
						inverseBindMatrixForFrame._21 = matBind._21;
						inverseBindMatrixForFrame._22 = matBind._22;
						inverseBindMatrixForFrame._23 = matBind._23;
						inverseBindMatrixForFrame._24 = matBind._24;
						inverseBindMatrixForFrame._31 = matBind._31;
						inverseBindMatrixForFrame._32 = matBind._32;
						inverseBindMatrixForFrame._33 = matBind._33;
						inverseBindMatrixForFrame._34 = matBind._34;
						inverseBindMatrixForFrame._41 = matBind._41;
						inverseBindMatrixForFrame._42 = matBind._42;
						inverseBindMatrixForFrame._43 = matBind._43;
						inverseBindMatrixForFrame._44 = matBind._44;
						armature.inverseBindMatrices[iB] = inverseBindMatrixForFrame;
					}
					pFrame->pMesh->wickedarmatureindex = armatureEntity;
				}

				// This node is an armature (a frame which contains a bone animatable mesh)
				mesh.armatureID = pFrame->pMesh->wickedarmatureindex;

				// ensure animated mesh frame cannot interfere with transform of armature
				// The object component will use an identity transform but will be parented to the armature
				// so possible animating entity becomes child of armature
				if (1)
				{
					// LB: this was the original, and together with restoring the entity to parent attachment, fixes geometry corruptions!
					//PE: Test now always do no sorting, should speed up everything :)
					//PE: Still need sorting for gun to appear , even with DisableObjectZDepth ? need more testing before disable sorting for everything.
					//if(bNoHierarchySorting)
					//	scene.Component_Attach(mesh.armatureID, entity, true);
					//else
					//  scene.Component_Attach_Sort(mesh.armatureID, entity, true);
					//LB: Had to restore older system to correct disappearing weapons, switchs and many animating objects
					scene.Component_Attach(mesh.armatureID, entity, true);
				}
				if (0)
				{
					// LB: this one from Paul solves the ground mesh on the "linhi_character_preview.zip" model, but corrupts the geometry of DBO/CC characters
					//scene.Component_Attach(entity, mesh.armatureID, true);
					///if (root != INVALID_ENTITY) scene.Component_Attach(mesh.armatureID, root, true);
				}

				// for frames with a mesh, ignore the matOriginal transform (see above for mesh transform)
				bUseFrameMatrix = false;
			}
			else
			{
				if (0)
				{
					// LB: this needed to be removed for original system to work (and solve corrupt geometry issue)
					// attach entity to its parent
					///if (parent != INVALID_ENTITY) scene.Component_Attach(entity, parent, true);
				}
			}

			// now create the internals for this mesh
			mesh.CreateRenderData();
		}
		//#### PE: mesh setup end ####
	}
	else
	{
		// if no mesh, just frame, then create entity to hold frame name
		if (entity == INVALID_ENTITY)
		{
			entity = CreateEntity();
			scene.transforms.Create(entity);
			scene.names.Create(entity) = pFrame->szName;
			pFrame->wickedobjindex = entity;
		}

		// attach entity to its parent
		if (0)
		{
			// LB: this needed to be removed for original system to work (and solve corrupt geometry issue)
			///if (parent != INVALID_ENTITY) scene.Component_Attach(entity, parent, true);
		}
	}

	// store 'frame' entity ID
	state.entityMap[pFrame->iID] = entity;

	// copy the transform from the frame to the scene entity transform
	TransformComponent& transform = *scene.transforms.GetComponent(entity);
	transform.scale_local = XMFLOAT3(1, 1, 1);
	transform.rotation_local = XMFLOAT4(0, 0, 0, 0);
	transform.translation_local = XMFLOAT3(0,0,0);
	if (bUseFrameMatrix == true)
	{
		GGMATRIX nodematrix = pFrame->matOriginal;
		transform.world._11 = nodematrix._11;
		transform.world._12 = nodematrix._12;
		transform.world._13 = nodematrix._13;
		transform.world._14 = nodematrix._14;
		transform.world._21 = nodematrix._21;
		transform.world._22 = nodematrix._22;
		transform.world._23 = nodematrix._23;
		transform.world._24 = nodematrix._24;
		transform.world._31 = nodematrix._31;
		transform.world._32 = nodematrix._32;
		transform.world._33 = nodematrix._33;
		transform.world._34 = nodematrix._34;
		transform.world._41 = nodematrix._41;
		transform.world._42 = nodematrix._42;
		transform.world._43 = nodematrix._43;
		transform.world._44 = nodematrix._44;
		transform.ApplyTransform();
	}
	transform.UpdateTransform();
	
	// attach entity to its parent
	if (1)
	{
		// LB: this needed to be added to restore original system so DBO/CC characters are not geometry corrupted
		if (parent != INVALID_ENTITY) scene.Component_Attach(entity, parent, true);
	}

	// recurse through all frames
	if (pFrame->pChild) WickedCall_LoadNode(pFrame->pChild, entity, root, state);
	if (pFrame->pSibling) WickedCall_LoadNode(pFrame->pSibling, parent, root, state);
}


void WickedCall_RefreshObjectAnimations(sObject* pObject, void* pstateptr)
{
	// get true pointer to loader state
	WickedLoaderState* pstate = (WickedLoaderState*)pstateptr;

	// current scene and root frame
	wiScene::Scene* pScene = &wiScene::GetScene();
	sFrame* pRootFrame = pObject->pFrame;

	// for objects that have animation data, create equivilant for wicked engine
	if (pObject->pAnimationSet)
	{
		// go through [first] animation set[s]
		sAnimationSet* pAnimSet = pObject->pAnimationSet;
		//while (pAnimSet != NULL) // LB: only use first animset core zero :) See Lee!
		{
			// clear any old wicked animation components (in case of a refresh)
			if (pAnimSet->wickedanimentityindex > 0)
			{
				AnimationComponent* animationcomponent = pScene->animations.GetComponent( pAnimSet->wickedanimentityindex );
				if (animationcomponent)
				{
					for (int i = 0; i < animationcomponent->samplers.size(); i++)
					{
						wiScene::GetScene().Entity_Remove(animationcomponent->samplers[i].data);
					}
				}
				wiScene::GetScene().Entity_Remove(pAnimSet->wickedanimentityindex);
				pAnimSet->wickedanimentityindex = 0;
			}

			// for each animation, create wicked animation component
			Entity animentity = CreateEntity();
			pScene->names.Create(animentity) = pAnimSet->szName;
			AnimationComponent& animationcomponent = pScene->animations.Create(animentity);
			pAnimSet->wickedanimentityindex = animentity;

			animationcomponent.objectIndex = 0;
			if (pstate->entityMeshMap.size() > 0)
			{
				//PE: TODO better way to indentify supported animation culling objects needed ?
				DWORD objid = pObject->dwObjectNumber;
				int masterId = -1;
				if (t.tupdatee > 0)
				{
					if(t.tupdatee < g.entityelementlist)
						masterId = t.entityelement[t.tupdatee].bankindex;
				}
				if (masterId < 1 && objid > 70000)
				{
					//PE: Locate object.
					for (int i = 0; i < g.entityelementlist; i++)
					{
						if (t.entityelement[i].obj == objid)
						{
							masterId = t.entityelement[i].bankindex;
							break;
						}
					}
				}
				else if (masterId < 1 && objid > 50000 && objid < 60000)
				{
					masterId = objid - 50000;
				}
				if (masterId > 0)
				{
					if (masterId > 0 && masterId < t.entityprofile.size())
					{
						if (t.entityprofile[masterId].ischaracter && (pstate->entityMeshMap.size() >= 4 && pstate->entityMeshMap.size() <= 6))
						{
							//PE: CCP Map all animations to first mesh for culling.
							animationcomponent.objectIndex = pstate->entityMeshMap.begin()->second;
						}
					}
				}
				if (animationcomponent.objectIndex == 0 && objid != 50000)
				{
					//PE: Only 1 mesh for culling will work.
					if (pstate->entityMeshMap.size() == 1)
					{
						animationcomponent.objectIndex = pstate->entityMeshMap.begin()->second;
					}
					else
					{
						//PE: Check for LOD and hidden meshes.
						int iTotalVisible = 0;
						int iVisibleIndex = 0;
						for (unordered_map<int, Entity>::iterator it = pstate->entityMeshMap.begin(); it != pstate->entityMeshMap.end(); ++it)
						{
							ObjectComponent* object = wiScene::GetScene().objects.GetComponent(it->second);
							NameComponent* name = wiScene::GetScene().names.GetComponent(it->second);
							bool bThisLODWillBeHidden = false;
							if(name && pestrcasestr(name->name.c_str(), "LOD_") && !pestrcasestr(name->name.c_str(), "LOD_0"))
								bThisLODWillBeHidden = true;
							if (object)
							{
								if (!bThisLODWillBeHidden && object->IsRenderable())
								{
									iTotalVisible++;
									iVisibleIndex = it->second;
								}
							}
						}
						if (iTotalVisible == 1 && iVisibleIndex > 0)
						{
							animationcomponent.objectIndex = iVisibleIndex;
						}
					}
				}
			}
			// increases as add more anim data (all goes into the above animationcomponent)
			int iSamplerAndChannelCount = 0;

			// go through all animations in this set
			sAnimation* pAnim = pAnimSet->pAnimation;
			while (pAnim != NULL)
			{
				//PE: TODO - To support more culled animation put objectindex into animationcomponent.channels, if object found.
				/*
				if (animationcomponent.objectIndex == 0)
				{
					//PE: Try to find bone mesh.
					sFrame* pFrame = pAnim->pFrame;
					if (pFrame)
					{
						uint64_t objindex = pFrame->wickedobjindex;
						ObjectComponent* object = wiScene::GetScene().objects.GetComponent(objindex);
						if (object)
						{
							animationcomponent.objectIndex = objindex;
						}
					}
				}
				*/
				//PE: Wicked - Support Matrix animations. convert to pPositionKeys,pRotateKeys,pScaleKeys
				if (pAnim->dwNumMatrixKeys > 0) //&& pAnim->dwNumPositionKeys == 0 && pAnim->dwNumRotateKeys == 0 && pAnim->dwNumScaleKeys == 0)
				{
					// supports appending matrix data to blank or populated pos/rot/scl keys
					DWORD dwNumKeys = pAnim->dwNumMatrixKeys;
					sPositionKey* pPosKeys = new sPositionKey[dwNumKeys];
					sRotateKey* pRotKeys = new sRotateKey[dwNumKeys];
					sScaleKey* pSclKeys = new sScaleKey[dwNumKeys];

					// unpack matrix date.
					for (size_t j = 0; j < dwNumKeys; ++j)
					{
						float fTime = pAnim->pMatrixKeys[j].dwTime;
						GGMATRIX mMat = pAnim->pMatrixKeys[j].matMatrix;
						XMFLOAT4X4 tmatrix;
						tmatrix._11 = mMat._11;	tmatrix._12 = mMat._12;	tmatrix._13 = mMat._13;	tmatrix._14 = mMat._14;
						tmatrix._21 = mMat._21;	tmatrix._22 = mMat._22;	tmatrix._23 = mMat._23;	tmatrix._24 = mMat._24;
						tmatrix._31 = mMat._31;	tmatrix._32 = mMat._32;	tmatrix._33 = mMat._33;	tmatrix._34 = mMat._34;
						tmatrix._41 = mMat._41;	tmatrix._42 = mMat._42;	tmatrix._43 = mMat._43;	tmatrix._44 = mMat._44;
						XMVECTOR S, R, T;
						XMMatrixDecompose(&S, &R, &T, XMLoadFloat4x4(&tmatrix));
						XMFLOAT3 vpos;
						XMStoreFloat3(&vpos, T);
						XMFLOAT4 vrot;
						XMStoreFloat4(&vrot, R);
						XMFLOAT3 vscale;
						XMStoreFloat3(&vscale, S);
						pPosKeys[j].dwTime = fTime;
						pRotKeys[j].dwTime = fTime;
						pSclKeys[j].dwTime = fTime;
						pPosKeys[j].vecPos.x = vpos.x;
						pPosKeys[j].vecPos.y = vpos.y;
						pPosKeys[j].vecPos.z = vpos.z;
						pRotKeys[j].Quaternion.x = vrot.x;
						pRotKeys[j].Quaternion.y = vrot.y;
						pRotKeys[j].Quaternion.z = vrot.z;
						pRotKeys[j].Quaternion.w = vrot.w;
						pSclKeys[j].vecScale.x = vscale.x;
						pSclKeys[j].vecScale.y = vscale.y;
						pSclKeys[j].vecScale.z = vscale.z;
					}

					// remove matrix keys
					SAFE_DELETE(pAnim->pMatrixKeys);
					pAnim->dwNumMatrixKeys = 0;

					// set the anim ptrs
					sPositionKey* pNewPosKeys = new sPositionKey[pAnim->dwNumPositionKeys+dwNumKeys];
					sRotateKey* pNewRotKeys = new sRotateKey[pAnim->dwNumRotateKeys+dwNumKeys];
					sScaleKey* pNewSclKeys = new sScaleKey[pAnim->dwNumScaleKeys+dwNumKeys];

					// append new keys to whole
					if ( pAnim->dwNumPositionKeys > 0 ) memcpy (pNewPosKeys, pAnim->pPositionKeys, sizeof(sPositionKey)*pAnim->dwNumPositionKeys);
					memcpy ((char*)pNewPosKeys+(sizeof(sPositionKey)*pAnim->dwNumPositionKeys), pPosKeys, sizeof(sPositionKey)*dwNumKeys);
					if ( pAnim->dwNumRotateKeys > 0 ) memcpy (pNewRotKeys, pAnim->pRotateKeys, sizeof(sRotateKey)*pAnim->dwNumRotateKeys);
					memcpy ((char*)pNewRotKeys+(sizeof(sRotateKey)*pAnim->dwNumRotateKeys), pRotKeys, sizeof(sRotateKey)*dwNumKeys);
					if ( pAnim->dwNumScaleKeys > 0 ) memcpy (pNewSclKeys, pAnim->pScaleKeys, sizeof(sScaleKey)*pAnim->dwNumScaleKeys);
					memcpy ((char*)pNewSclKeys+(sizeof(sScaleKey)*pAnim->dwNumScaleKeys), pSclKeys, sizeof(sScaleKey)*dwNumKeys);

					// set the new anim ptrs
					SAFE_DELETE(pAnim->pPositionKeys);
					SAFE_DELETE(pAnim->pRotateKeys);
					SAFE_DELETE(pAnim->pScaleKeys);
					pAnim->pPositionKeys = pNewPosKeys;
					pAnim->pRotateKeys = pNewRotKeys;
					pAnim->pScaleKeys = pNewSclKeys;
					pAnim->dwNumPositionKeys += dwNumKeys;
					pAnim->dwNumRotateKeys += dwNumKeys;
					pAnim->dwNumScaleKeys += dwNumKeys;

					// free resources
					SAFE_DELETE(pPosKeys);
					SAFE_DELETE(pRotKeys);
					SAFE_DELETE(pSclKeys);
				}

				// new anim means new frame with anim data to consider)
				int iOffset = iSamplerAndChannelCount;

				// work out how many samplers/channels are needed

				//PE: OPTIMIZING No need to process scale keys if not used. (RunAnimationUpdateSystem is slow). most anim dont use scale.
				bool bGotScale = false;
				if (pAnim->dwNumScaleKeys > 0)
				{
					for (size_t j = 0; j < pAnim->dwNumScaleKeys; ++j)
					{
						XMFLOAT3 vec3;
						vec3.x = pAnim->pScaleKeys[j].vecScale.x;
						vec3.y = pAnim->pScaleKeys[j].vecScale.y;
						vec3.z = pAnim->pScaleKeys[j].vecScale.z;
						if (vec3.x != 1.0f || vec3.y != 1.0f || vec3.z != 1.0f)
						{
							bGotScale = true;
							break;
						}
					}
				}

				bool bSamplerChannelsMask[3];
				bSamplerChannelsMask[0] = false;
				bSamplerChannelsMask[1] = false;
				bSamplerChannelsMask[2] = false;
				int iSamplerChannelsNeeded = 0;
				if (pAnim->dwNumPositionKeys > 0) { iSamplerChannelsNeeded++; bSamplerChannelsMask[0] = true; }
				if (pAnim->dwNumRotateKeys > 0) { iSamplerChannelsNeeded++; bSamplerChannelsMask[1] = true; }
				if (bGotScale && pAnim->dwNumScaleKeys > 0) { iSamplerChannelsNeeded++; bSamplerChannelsMask[2] = true; }
				iSamplerAndChannelCount += iSamplerChannelsNeeded;

				// calculate size of samplers (to hold raw data)
				animationcomponent.samplers.resize(iSamplerAndChannelCount);
				int iSamplerOffset = iOffset;
				for (int i = 0; i < 3; i++)
				{
					if (bSamplerChannelsMask[i] == true)
					{
						animationcomponent.samplers[iSamplerOffset].mode = AnimationComponent::AnimationSampler::Mode::LINEAR;
						int count = 0;
						if (i == 0) count = pAnim->dwNumPositionKeys;
						if (i == 1) count = pAnim->dwNumRotateKeys;
						if (i == 2) count = pAnim->dwNumScaleKeys;
						animationcomponent.samplers[iSamplerOffset].backwards_compatibility_data.keyframe_times.resize(count);
						for (size_t j = 0; j < count; ++j)
						{
							float time = 0.0f;
							if (i == 0) time = (float)pAnim->pPositionKeys[j].dwTime;
							if (i == 1) time = (float)pAnim->pRotateKeys[j].dwTime;
							if (i == 2) time = (float)pAnim->pScaleKeys[j].dwTime;
							animationcomponent.samplers[iSamplerOffset].backwards_compatibility_data.keyframe_times[j] = time;
							animationcomponent.start = min(animationcomponent.start, time);
							animationcomponent.end = max(animationcomponent.end, time);
						}
						if (i == 1)
						{
							// rotation
							animationcomponent.samplers[iSamplerOffset].backwards_compatibility_data.keyframe_data.resize(count * 4);
							for (size_t j = 0; j < count; ++j)
							{
								XMFLOAT4 rot;
								rot.x = pAnim->pRotateKeys[j].Quaternion.x;
								rot.y = pAnim->pRotateKeys[j].Quaternion.y;
								rot.z = pAnim->pRotateKeys[j].Quaternion.z;
								rot.w = pAnim->pRotateKeys[j].Quaternion.w;
								((XMFLOAT4*)animationcomponent.samplers[iSamplerOffset].backwards_compatibility_data.keyframe_data.data())[j] = rot;
							}
						}
						else
						{
							// position or scale
							animationcomponent.samplers[iSamplerOffset].backwards_compatibility_data.keyframe_data.resize(count * 3);
							for (size_t j = 0; j < count; ++j)
							{
								XMFLOAT3 vec3;
								if (i == 0)
								{
									vec3.x = pAnim->pPositionKeys[j].vecPos.x;
									vec3.y = pAnim->pPositionKeys[j].vecPos.y;
									vec3.z = pAnim->pPositionKeys[j].vecPos.z;
								}
								else
								{
									vec3.x = pAnim->pScaleKeys[j].vecScale.x;
									vec3.y = pAnim->pScaleKeys[j].vecScale.y;
									vec3.z = pAnim->pScaleKeys[j].vecScale.z;
								}
								((XMFLOAT3*)animationcomponent.samplers[iSamplerOffset].backwards_compatibility_data.keyframe_data.data())[j] = vec3;
							}
						}
						iSamplerOffset++;
					}
				}

				// calculate size of channels (to direct anim data to target)
				animationcomponent.channels.resize(iSamplerAndChannelCount);
				int iChannelOffset = iOffset;
				for (size_t i = 0; i < 3; ++i)
				{
					if (bSamplerChannelsMask[i] == true)
					{
						//PE: All channels MUST have a target , or wicked will crash.
						//PE: Target is used to get the transform
						sFrame*	pFrameMustSet = pAnim->pFrame;
						if (!pFrameMustSet)
						{
							// if pframe not found assume animation is for pRootFrame
							pFrameMustSet = pRootFrame;
						}
						if (pFrameMustSet)
						{
							// set the target, samplerindex and type for this channel item
							int iFrameIndexForThisAnim = pFrameMustSet->iID;
							wiECS::Entity thisTarget = wiECS::INVALID_ENTITY;
							if (pstate) thisTarget = pstate->entityMap[iFrameIndexForThisAnim];
							animationcomponent.channels[iChannelOffset].target = thisTarget;
							animationcomponent.channels[iChannelOffset].samplerIndex = (uint32_t)iChannelOffset;
							if (i == 0) animationcomponent.channels[iChannelOffset].path = AnimationComponent::AnimationChannel::Path::TRANSLATION;
							if (i == 1) animationcomponent.channels[iChannelOffset].path = AnimationComponent::AnimationChannel::Path::ROTATION;
							if (i == 2) animationcomponent.channels[iChannelOffset].path = AnimationComponent::AnimationChannel::Path::SCALE;

							// new features of the wicked animation system
							animationcomponent.channels[iChannelOffset].iUsePreFrame = 0;
							animationcomponent.channels[iChannelOffset].vPreFrameScale = XMVectorSet(1, 1, 1, 0);
							animationcomponent.channels[iChannelOffset].qPreFrameRotation = XMQuaternionRotationRollPitchYaw(0, 0, 0);
							animationcomponent.channels[iChannelOffset].vPreFrameTranslation = XMVectorSet(0, 0, 0, 0);
							int iThisSamplerOffset = animationcomponent.channels[iChannelOffset].samplerIndex;

							// finally store channel and sampler offsets for this animation item
							pAnim->wickedanimationchannel[i] = iChannelOffset;
							pAnim->wickedanimationsampler[i] = iThisSamplerOffset;

							// next channel
							iChannelOffset++;
						}
					}
				}

				// next animation
				pAnim = pAnim->pNext;
			}

			// next animation set - wicked uses first animset to store all animations for object
			//pAnimSet = pAnimSet->pNext;
		}
	}
}

void WickedCall_AddObject ( sObject* pObject )
{
	// delibeately not create the wicked object, speeds up everything in wicked engine
	// until the object actually needed (only used for decals/explosions/particles which require 1000's of objects)
	if (g_bWickedCreateOnlyWhenUsed == true)
	{
		// soon to be replaced with nicer GPU particles
		pObject->wickedrootentityindex = 0;
		return;
	}

	if (pObject->iMeshCount >= 590)
	{
		//PE: Get stack overflow when object have more then 600 meshes.
		// Walled Garden Pack\Mausoleum 05.fpe
		return;
	}

	// ensure wickedloaderstateptr deleted first
	if (pObject->wickedloaderstateptr)
	{
		delete pObject->wickedloaderstateptr;
		pObject->wickedloaderstateptr = NULL;
	}

	// reject if no mesh
	if (pObject->ppMeshList == NULL)
		return;

	// for structures, allow to pass if object has at least one mesh with geometry in it
	bool bhasGeometry = false;
	//for (int m = 0; m < pObject->iMeshCount; m++) if (pObject->ppMeshList[m]->dwIndexCount > 0) bhasGeometry = true; // need to support meshes with NO indices (32-bit large poly models)
	for (int m = 0; m < pObject->iMeshCount; m++) if (pObject->ppMeshList[m]->dwVertexCount > 0) bhasGeometry = true;
	if (bhasGeometry == false) return;

	// instead of creating a scene and merging, just add direct to main scene (faster?)
	wiScene::Scene* pScene = &wiScene::GetScene();

	// create scene object (will contain created object we are adding)
	Entity rootEntity = CreateEntity();
	pScene->transforms.Create(rootEntity);

	// create materials, but do not load textures, done later when load textures
	for ( int iM = 0; iM < pObject->iMeshCount; iM++ )
	{
		sMesh* pMesh = pObject->ppMeshList[iM];
		if (pMesh)
		{
			if (pMesh->pTextures)
			{
				char* pTextureFilename = pMesh->pTextures[0].pName;
				if ( pTextureFilename )
				{
					// create material, texture them later on
					if (!(pMesh->bInstanced && pMesh->master_wickedmeshindex > 0))
					{
						//PE: No need to create material for Instanced objects.
						wiECS::Entity materialEntity = pScene->Entity_CreateMaterial(pTextureFilename);
						wiScene::MaterialComponent& material = *pScene->materials.GetComponent(materialEntity);
						material.baseColor = XMFLOAT4(1, 1, 1, 1);
						pMesh->wickedmaterialindex = materialEntity;

						// LB: ensure transparency is respected in wicked
						if (pMesh->bTransparency == true)
							material.userBlendMode = BLENDMODE_ALPHA;
						else
							material.userBlendMode = BLENDMODE_OPAQUE;

						// LB: assume that transparent meshes cast no shadows
						if (pMesh->bTransparency == true)
							material.SetCastShadow(false);
						else
							material.SetCastShadow(true);
					}
					else
					{
						pMesh->wickedmaterialindex = 0; //PE: We use master material index.
					}
				}
			}
		}
	}
	if (pScene->materials.GetCount() == 0)
	{
		// if no material created above, add default material for mesh
		wiECS::Entity materialEntity = pScene->Entity_CreateMaterial("defaultMaterial");
		wiScene::MaterialComponent& material = *pScene->materials.GetComponent(materialEntity);
		material.baseColor = XMFLOAT4(1, 1, 1, 1);

		// apply default material to all meshes in this object
		for (int iM = 0; iM < pObject->iMeshCount; iM++)
			if (pObject->ppMeshList[iM])
				pObject->ppMeshList[iM]->wickedmaterialindex = materialEntity;
	}

	// if object has bones (animation), then need an armature - done inside LoadNode
	// but need to reset wickedarmatureindex IDs as this scene is a temporarly one, to be merged at the end
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
		if (pObject->ppMeshList[iM]->dwBoneCount > 0)
			pObject->ppMeshList[iM]->wickedarmatureindex = 0;

	// Create transform hierarchy, assign objects, meshes, armatures, cameras
	WickedLoaderState state;
	state.scene = pScene;
	state.storeMasterRootEntityIndex = rootEntity;
	sFrame* pRootFrame = pObject->pFrame;
	WickedCall_LoadNode ( pRootFrame, rootEntity, rootEntity, state );
	

	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		sMesh* pDBOMesh = pObject->ppMeshList[iM];
		if (pDBOMesh)
		{
			uint64_t meshindex = pDBOMesh->wickedmeshindex;
			if (meshindex > 0)
			{
				wiScene::MeshComponent* mesh = pScene->meshes.GetComponent(meshindex);
				if (mesh)
				{
					if (mesh->vertex_colors.size() > 0)
					{
						uint64_t wickedmaterialindex = pDBOMesh->wickedmaterialindex;
						if(wickedmaterialindex == 0)
							wickedmaterialindex = pDBOMesh->master_wickedmaterialindex;
						if (wickedmaterialindex != 0)
						{
							wiScene::MaterialComponent* material = pScene->materials.GetComponent(wickedmaterialindex);
							if (material)
							{
								//PE: defaults.
								material->roughness = 1;
								material->metalness = 0;
								material->reflectance = 0.04f;// 0.002f;
								if (pDBOMesh->iReservedForFuture > 10)
								{
									//PE: activate as lod object. with vertexcolors off by default.
									if (pDBOMesh->pFrameAttachedTo)
									{
										ObjectComponent* object = wiScene::GetScene().objects.GetComponent(pDBOMesh->pFrameAttachedTo->wickedobjindex);
										if (object)
										{
											//pDBOMesh->pFrameAttachedTo->wickedobjindex
											object->SetLOD(true);
											object->SetLodDistance(pDBOMesh->iReservedForFuture);
										}
									}
								}
								else
									material->SetUseVertexColors(true);
								material->SetDirty();
							}
						}
					}
				}
			}
		}
	}


	// Create armature-bone mappings (connect armature bone collection to frame entities created in LoadNode)
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		sMesh* pDBOMesh = pObject->ppMeshList[iM];
		if (pDBOMesh->dwBoneCount > 0)
		{
			if (pDBOMesh->wickedarmatureindex != 0)
			{
				ArmatureComponent& armature = *pScene->armatures.GetComponent(pDBOMesh->wickedarmatureindex);
				armature.boneCollection.resize(pDBOMesh->dwBoneCount);
				for (size_t i = 0; i < pDBOMesh->dwBoneCount; ++i)
				{
					// for each bone within this mesh, find the frame that matches the name
					sFrame* pFoundFrame = NULL;
					LPSTR pSearchFor = pDBOMesh->pBones[i].szName;
					for (int iFindFrame = 0; iFindFrame < pObject->iFrameCount; iFindFrame++)
					{
						if (stricmp(pObject->ppFrameList[iFindFrame]->szName, pSearchFor) == NULL)
						{
							// found frame that matches this bone name
							pFoundFrame = pObject->ppFrameList[iFindFrame];
							break;
						}
					}
					if (pFoundFrame)
					{
						// get the bone entity from the hierarchy
						Entity boneEntity = state.entityMap[pFoundFrame->iID];

						// assign the bone entity to this armature mesh's bone collection
						armature.boneCollection[i] = boneEntity;
					}
				}

				//PE: Make sure all boneCollection has a transform , or we get a crash.
				for (Entity boneEntity : armature.boneCollection)
				{
					const TransformComponent& bone = *pScene->transforms.GetComponent(boneEntity);
					if (!&bone)
					{
						pScene->armatures.Remove(pDBOMesh->wickedarmatureindex);
						pDBOMesh->wickedarmatureindex = 0;
						break;
					}
				}
			}
		}
	}

	// Create animations (from animation data stored in DBO)
	WickedLoaderState* pCopyLoaderState = new WickedLoaderState;
	pCopyLoaderState->scene = state.scene;
	pCopyLoaderState->storeMasterRootEntityIndex = state.storeMasterRootEntityIndex;
	pCopyLoaderState->entityMap = state.entityMap;
	pCopyLoaderState->entityMeshMap = state.entityMeshMap;

	//int objectindex = state.entityMap[pObject->pFrame->iID];
	WickedCall_RefreshObjectAnimations(pObject, (void*)pCopyLoaderState);

	// trigger an update to the root entity transform
	wiScene::TransformComponent* pATransform = pScene->transforms.GetComponent(rootEntity);
	pATransform->SetDirty(true);

	// set a transform for the object
	//wiScene::TransformComponent* pRootTransform = scene.transforms.GetComponent(rootEntity);
	wiScene::TransformComponent* pRootTransform = pScene->transforms.GetComponent(rootEntity);
	pRootTransform->Translate(XMFLOAT3(0, 0, 0));
	pRootTransform->UpdateTransform();

	// stores wicked's object entity 'ID' so object can reference by sObject ptr
	pObject->wickedrootentityindex = rootEntity;
	pObject->wickedloaderstateptr = (void*)pCopyLoaderState;
}

void WickedCall_SetObjectSpeed(sObject* pObject, float fSpeed)
{
	// set the newly added speed modifier in wicked
	if ( pObject->pAnimationSet )
	{
		sAnimationSet* pAnimSet = pObject->pAnimationSet;
		//while ( pAnimSet != NULL ) - wicked uses first animset to store all animations for object
		{
			Entity animentity = pAnimSet->wickedanimentityindex;
			AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
			if (animationcomponent)
			{
				// not all animation entries have data (FBX imports can have empty animation sets!)
				animationcomponent->SetSpeed(fSpeed*50); //PE: (ORG:50) Need to adjust this to fit old speed.
			}
			//pAnimSet = pAnimSet->pNext;
		}
	}
}

//PE: this works for doors and other animation controlls by lua.
//PE: but dont work for guns , as they rely on the playing status.
//PE: so we cant do this at the moment.
//PE: Perhaps its better to control this directly in the lua scripts ?
void WickedCall_CheckAnimationDone(sObject* pObject)
{
	// set the newly added speed modifier in wicked
	if (pObject->pAnimationSet)
	{
		float fEndFrame = -1;
		sAnimationSet* pAnimSet = pObject->pAnimationSet;
		//while (pAnimSet != NULL) - wicked uses first animset to store all animations for object
		{
			Entity animentity = pAnimSet->wickedanimentityindex;
			AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
			if(animationcomponent)
			{
				//PE: Wicked dont stop animations by itself, WickedCall_SetObjectSpeed is called all the time.
				//PE: So check here if we need to stop the animation.
				if (animationcomponent->IsPlaying())
				{
					//PE: pObject->bAnimPlaying is not set anywhere.
					pObject->bAnimPlaying = true;
					//float timer = animationcomponent->timer;
					//float length = animationcomponent->GetLength();
					bool isended = animationcomponent->IsEnded();
					bool islooped = animationcomponent->IsLooped();
					if (isended && !islooped)
					{
						fEndFrame = animationcomponent->end;
						animationcomponent->SetLooped(false);
						animationcomponent->Stop();
						pObject->bAnimPlaying = false;
						//PE: Must make sure we are set at the last frame.
						//PE: Fix - https://thegamecreators.teamwork.com/index.cfm#/tasks/21003817?c=10406263 ,
						animationcomponent->timer = fEndFrame;
						animationcomponent->SetUpdateOnce();
					}
				}
			}
			//pAnimSet = pAnimSet->pNext;
		}
	}
}

bool WickedCall_GetAnimationPlayingState (sObject* pObject)
{
	sAnimationSet* pAnimSet = pObject->pAnimationSet;
	if (pAnimSet)
	{
		Entity animentity = pAnimSet->wickedanimentityindex;
		AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
		if(animationcomponent)
			if (animationcomponent->IsPlaying())
				return true;
	}
	return false;
}

void WickedCall_SetAnimationLerpFactor (sObject* pObject)
{
	sAnimationSet* pAnimSet = pObject->pAnimationSet;
	if (pAnimSet)
	{
		Entity animentity = pAnimSet->wickedanimentityindex;
		AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
		if (animationcomponent)
		{
			if (animationcomponent->updateonce == false)
			{
				animationcomponent->amount = pObject->fAnimInterp;
			}
		}
	}
}

void WickedCall_PlayObject(sObject* pObject, float fStart, float fEnd, bool bLooped)
{
	if ( pObject->pAnimationSet )
	{
		sAnimationSet* pAnimSet = pObject->pAnimationSet;
		if (!pAnimSet)
			return;
		//while ( pAnimSet != NULL )
		{
			Entity animentity = pAnimSet->wickedanimentityindex;
			AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
			if (animationcomponent)
			{
				animationcomponent->start = fStart;
				if (fEnd != -1)
				{
					animationcomponent->end = fEnd;
				}
				animationcomponent->timer = fStart;
				if (animationcomponent->updateonce == false)
				{
					animationcomponent->amount = pObject->fAnimInterp;
				}
				animationcomponent->SetLooped(bLooped);
				animationcomponent->Play();
			}
			//pAnimSet = pAnimSet->pNext;
		}
	}
}

void WickedCall_InstantObjectFrameUpdate(sObject* pObject)
{
	if (pObject->pAnimationSet)
	{
		sAnimationSet* pAnimSet = pObject->pAnimationSet;
		Entity animentity = pAnimSet->wickedanimentityindex;
		AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
		if (animationcomponent)
		{
			animationcomponent->updateonce = true;
			animationcomponent->amount = 1;
		}
	}
}

void WickedCall_GetObjectAnimationData(sObject* pObject, float* pStart, float* pFinish )
{
	if ( pObject->pAnimationSet )
	{
		sAnimationSet* pAnimSet = pObject->pAnimationSet;
		Entity animentity = pAnimSet->wickedanimentityindex;
		AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
		if (animationcomponent)
		{
			*pStart = animationcomponent->start;
			*pFinish = animationcomponent->end;
		}
	}
}

void WickedCall_StopObject(sObject* pObject)
{
	if (pObject)
	{
		sAnimationSet* pAnimSet = pObject->pAnimationSet;
		if (pAnimSet)
		{
			Entity animentity = pAnimSet->wickedanimentityindex;
			AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
			if (animationcomponent )
				animationcomponent->Stop();
		}
	}
}

void WickedCall_SetObjectFrame(sObject* pObject, float fFrame)
{
	if ( pObject->pAnimationSet )
	{
		sAnimationSet* pAnimSet = pObject->pAnimationSet;
		//while ( pAnimSet != NULL ) - wicked uses first animset to store all animations for object
		{
			Entity animentity = pAnimSet->wickedanimentityindex;
			AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
			if (animationcomponent)
			{
				animationcomponent->SetLooped(false);
				animationcomponent->Stop();
				animationcomponent->timer = fFrame;
				animationcomponent->SetUpdateOnce();
			}
			//pAnimSet = pAnimSet->pNext;
		}
	}
}

void WickedCall_SetObjectFrameEx(sObject* pObject, float fFrame)
{
	// used when want to set the framr even if looping or playing
	if (pObject->pAnimationSet)
	{
		sAnimationSet* pAnimSet = pObject->pAnimationSet;
		{
			Entity animentity = pAnimSet->wickedanimentityindex;
			AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
			if (animationcomponent)
			{
				animationcomponent->timer = fFrame;
				animationcomponent->SetUpdateOnce();
			}
		}
	}
}


float WickedCall_GetObjectFrame(sObject* pObject)
{
	float fFrame = 0.0f;
	if (pObject)
	{
		if (pObject->pAnimationSet)
		{
			// first animset only for frame return
			sAnimationSet* pAnimSet = pObject->pAnimationSet;
			Entity animentity = pAnimSet->wickedanimentityindex;
			AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
			if (animationcomponent)
			{
				fFrame = animationcomponent->timer;
			}
		}
	}
	return fFrame;
}

float WickedCall_GetObjectRealFrame(sObject* pObject)
{
	// special 
	float fFrame = 0.0f;
	if (pObject->pAnimationSet)
	{
		// first animset only for frame return
		sAnimationSet* pAnimSet = pObject->pAnimationSet;
		Entity animentity = pAnimSet->wickedanimentityindex;
		AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
		if (animationcomponent)
		{
			fFrame = animationcomponent->timer;
			if (animationcomponent->IsLooped())
			{
				// special situation where looped animations have an extra 1.0f after the loop end to interp back to first frame
				// and this frame returned represents the real frame in the anim data, not the animation.timer frame which tracks the raw position in the seqwence (timer)
				if (fFrame >= animationcomponent->end)
				{
					// the assumption is that the _timers are all aligned as 1.0f timings so they match key frame subscripts
					float fInterpT = fFrame - animationcomponent->end;
					fFrame = animationcomponent->start + fInterpT;
				}
			}
		}
	}
	return fFrame;
}

bool bBlockSceneUpdate = false;
void WickedCall_RemoveObject( sObject* pObject )
{
	// when removing assets from the wicked engine scene
	uint64_t rootEntity = pObject->wickedrootentityindex;
	if (rootEntity > 0)
	{
		WickedCall_SetObjectVisible(pObject, false);
		wiJobSystem::context ctx;
		wiJobSystem::Wait(ctx);

		// stop any animation playing
		WickedCall_StopObject(pObject);

		// remove any animcomponents
		if (pObject->pAnimationSet)
		{
			sAnimationSet* pAnimSet = pObject->pAnimationSet;
			//while (pAnimSet != NULL) - wicked uses first animset to store all animations for object
			{
				if (pAnimSet->wickedanimentityindex > 0)
				{				
					AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent( pAnimSet->wickedanimentityindex );
					if (animationcomponent)
					{
						for (int i = 0; i < animationcomponent->samplers.size(); i++)
						{
							wiScene::GetScene().Entity_Remove(animationcomponent->samplers[i].data);
						}
					}
					wiScene::GetScene().Entity_Remove(pAnimSet->wickedanimentityindex);
					pAnimSet->wickedanimentityindex = 0;
				}
				//pAnimSet = pAnimSet->pNext;
			}
		}

		// remove all materials and armature used by any mesh
		for (int iM = 0; iM < pObject->iMeshCount; iM++)
		{
			sMesh* pMesh = pObject->ppMeshList[iM];
			if (pMesh)
			{		
				if (pMesh->wickedmeshindex > 0)
				{
					if(!pMesh->bInstanced)
						wiScene::GetScene().Entity_Remove(pMesh->wickedmeshindex);
					pMesh->wickedmeshindex = 0;
				}
				if (pMesh->wickedmaterialindex > 0)
				{				
					if (!pMesh->bInstanced)
						wiScene::GetScene().Entity_Remove(pMesh->wickedmaterialindex);
					pMesh->wickedmaterialindex = 0;
				}
				if (pMesh->wickedarmatureindex > 0)
				{
					if (!pMesh->bInstanced)
						wiScene::GetScene().Entity_Remove(pMesh->wickedarmatureindex);
					pMesh->wickedarmatureindex = 0;
				}
			}
		}

		// remove all entities associated with all frames of this object
		for (int iF = 0; iF < pObject->iFrameCount; iF++)
		{
			sFrame* pFrame = pObject->ppFrameList[iF];
			if (pFrame)
			{
				wiScene::GetScene().Entity_Remove(pFrame->wickedobjindex);
				pFrame->wickedobjindex = 0;
			}
		}
		// look at editor on how objects are thoroughly deleted
		wiScene::GetScene().Entity_Remove(rootEntity);
		pObject->wickedrootentityindex = 0;

		// ensure wickedloaderstateptr deleted first
		if (pObject->wickedloaderstateptr)
		{
			delete pObject->wickedloaderstateptr;
			pObject->wickedloaderstateptr = NULL;
		}

		// finally update scene with removals
		if(!bBlockSceneUpdate)
			wiScene::GetScene().Update(0);
	}
}

void WickedCall_SetTexturePath(LPSTR pPath)
{
	g_pWickedTexturePath = pPath;
}

void WickedCall_TextureMesh(sMesh* pMesh)
{
	if (pMesh)
	{
		if (g_bWickedUseImagePtrInsteadOfTexFile == true)
		{
			// uses special way to texture a wicked object, using the old DX11 texture image ptr
			WickedCall_TextureMeshWithImagePtr(pMesh,g_iWickedPutInEmissiveMode);
		}
		else
		{
			// get wicked meshID
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
			if (mesh)
			{
				// special flag which will wipe out texture info
				if (g_bWickedIgnoreTextureInfo == true)
				{
					// used when we do NOT want textures from loaded objects, we have a wicked new approach for that object :)
					for( int i = 0; i < pMesh->dwTextureCount; i++ ) strcpy(pMesh->pTextures[i].pName, "");
				}

				// get mesh texture filename
				char* pTextureFilename = pMesh->pTextures[0].pName;

				//PE: check if we can find a multimaterial textures.
				if (pTextureFilename && strlen(pTextureFilename) < 1)
				{
					if (pMesh && pMesh->bUseMultiMaterial)
					{
						if (pMesh->pMultiMaterial && pMesh->pMultiMaterial[0].pName)
						{
							pTextureFilename = pMesh->pMultiMaterial[0].pName;
						}
					}
				}
				std::string sTextureFilenameBase = pTextureFilename;

				// get material from mesh
				uint64_t materialEntity = mesh->subsets[0].materialID;
				wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);

				// set DEFAULT RAW material values
				pObjectMaterial->baseColor = XMFLOAT4(1, 1, 1, 1);

				// if surface texture specified, but ROUGH,METAL,AO empty, fill them as no reference to preserve at this point
				if (pMesh->dwTextureCount >= GG_MESH_TEXTURE_SURFACE && strlen(pMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName) != 0)
				{
					if (strlen(pMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName) == 0)
					{
						strcpy (pMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName, pMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName);
						pMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].channelMask = (15)+(1 << 4);
					}
					if (strlen(pMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName) == 0)
					{
						strcpy (pMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName, pMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName);
						pMesh->pTextures[GG_MESH_TEXTURE_METALNESS].channelMask = (15)+(2 << 4);
					}
					if (strlen(pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].pName) == 0)
					{
						strcpy (pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].pName, pMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName);
						pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].channelMask = (15)+(0 << 4);
					}
				}

				// set roughness strength if have texture
				pObjectMaterial->roughness = 0.0f;
				if ( pMesh->dwTextureCount > GG_MESH_TEXTURE_ROUGHNESS && strlen(pMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName)!=0 )
					pObjectMaterial->roughness = 1.0f;

				// set metalness strength if have texture
				pObjectMaterial->metalness = 0.0f;
				if ( pMesh->dwTextureCount > GG_MESH_TEXTURE_METALNESS && strlen(pMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName)!=0 )
					pObjectMaterial->metalness = 1.0f;

				// set roughness strength if have texture
				pObjectMaterial->emissiveColor.w = 0.0f;
				if (pMesh->dwTextureCount > GG_MESH_TEXTURE_EMISSIVE && strlen(pMesh->pTextures[GG_MESH_TEXTURE_EMISSIVE].pName) != 0)
					pObjectMaterial->emissiveColor.w = 0.0f; // 64.0f; corrected default as it was before, no emissive strength until set
				
				// default reflectance
				pObjectMaterial->reflectance = 0.04f;// 0.002f;

				//Make sure Trees And Veg get alpharef.
				float fAlphaRef = WickedGetTreeAlphaRef();
				pObjectMaterial->alphaRef = fAlphaRef;

				// split passed in texture path/name
				std::string sFoundTexturePath = "";
				std::string sFoundTextureFilename = sTextureFilenameBase;
				std::string sFoundTextureType = "";
				char pSplitTextureFilenameBase[MAX_PATH];
				strcpy(pSplitTextureFilenameBase, sTextureFilenameBase.c_str());
				for (int n = strlen(pSplitTextureFilenameBase); n > 0; n--)
				{
					if (pSplitTextureFilenameBase[n] == '\\' || pSplitTextureFilenameBase[n] == '/')
					{
						sFoundTextureFilename = pSplitTextureFilenameBase + n + 1;
						pSplitTextureFilenameBase[n + 1] = 0;
						sFoundTexturePath = pSplitTextureFilenameBase;
						break;
					}
				}
				strcpy(pSplitTextureFilenameBase, sFoundTextureFilename.c_str());
				for (int n = strlen(pSplitTextureFilenameBase); n > 0; n--)
				{
					if (pSplitTextureFilenameBase[n] == '.')
					{
						sFoundTextureType = pSplitTextureFilenameBase + n;
						pSplitTextureFilenameBase[n] = 0;
						sFoundTextureFilename = pSplitTextureFilenameBase;
						break;
					}
				}

				bool bFoundTextureToLoad = false;
				std::string sFoundFinalPathAndFilename = "";
				std::string sTextureFileName;
				bool bWickedMaterialActive = IsWickedMaterialActive(pMesh);
				if (bWickedMaterialActive)
				{
					// Use wicked material from fpe.
					bool bFound = false;
					//PE: Optimize by hit rate.
					cstr sBaseColor = WickedGetBaseColorName();
					if (sFoundTexturePath.size() <= 0)
					{
						//PE: Better hit rate.
						sFoundFinalPathAndFilename = g_pWickedTexturePath + sBaseColor.Get();
						if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
						{
							sFoundFinalPathAndFilename = sFoundTexturePath + sBaseColor.Get();
							if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
								sFoundFinalPathAndFilename = sBaseColor.Get();
							else
								bFound = true;
						}
						else
							bFound = true;
					}
					else
					{
						sFoundFinalPathAndFilename = sFoundTexturePath + sBaseColor.Get();
						if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
						{
							sFoundFinalPathAndFilename = g_pWickedTexturePath + sBaseColor.Get();
							if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
								sFoundFinalPathAndFilename = sBaseColor.Get();
							else
								bFound = true;
						}
						else
							bFound = true;
					}
					if (bFound || FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 1)
					{
						if (pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource) //PE: Delete first if already active.
						{
							pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource = nullptr;
							pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].name = "";
							pObjectMaterial->SetDirty();
							wiJobSystem::context ctx;
							wiJobSystem::Wait(ctx);
						}

						pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].name = sFoundFinalPathAndFilename;
						pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].name);
						if (pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource)
						{
							//PE: save full path as g_pWickedTexturePath is lost later.
							strcpy(pMesh->pTextures[0].pName, sFoundFinalPathAndFilename.c_str());

							//TODO: Get AlphaRef.
							float fAlphaRef = WickedGetAlphaRef();
							if (fAlphaRef < 0.0) fAlphaRef = 1.0f;
							pObjectMaterial->SetAlphaRef(fAlphaRef);

							//Normalmap.
							if (sFoundTexturePath.size() <= 0)
							{
								//PE: Best hit rate.
								sFoundFinalPathAndFilename = g_pWickedTexturePath + WickedGetNormalName().Get();
								if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
								{
									sFoundFinalPathAndFilename = WickedGetNormalName().Get();
								}
							}
							else
							{
								sFoundFinalPathAndFilename = sFoundTexturePath + WickedGetNormalName().Get();
								if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
								{
									sFoundFinalPathAndFilename = g_pWickedTexturePath + WickedGetNormalName().Get();
									if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
										sFoundFinalPathAndFilename = WickedGetNormalName().Get();
								}
							}
							if (pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource) //PE: Delete first if already active.
							{
								pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource = nullptr;
								pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = "";
								pObjectMaterial->SetDirty();
								wiJobSystem::context ctx;
								wiJobSystem::Wait(ctx);
							}

							pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = sFoundFinalPathAndFilename;
							pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::NORMALMAP].name);
							if (pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource)
							{
								//Set normal intensity.
								pObjectMaterial->SetNormalMapStrength(WickedGetNormalStrength());
							}
							else
							{
								pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = ""; //Prevent wicked from reloading image.
							}

							//Surface.
							if (sFoundTexturePath.size() <= 0)
							{
								//PE: Best hit rate.
								sFoundFinalPathAndFilename = g_pWickedTexturePath + WickedGetSurfaceName().Get();
								if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
								{
									sFoundFinalPathAndFilename = WickedGetSurfaceName().Get();
								}
							}
							else
							{
								sFoundFinalPathAndFilename = sFoundTexturePath + WickedGetSurfaceName().Get();
								if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
								{
									sFoundFinalPathAndFilename = g_pWickedTexturePath + WickedGetSurfaceName().Get();
									if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
										sFoundFinalPathAndFilename = WickedGetSurfaceName().Get();
								}
							}

							if (pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource) //PE: Delete first if already active.
							{
								pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource = nullptr;
								pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = "";
								pObjectMaterial->SetDirty();
								wiJobSystem::context ctx;
								wiJobSystem::Wait(ctx);
							}

							pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = sFoundFinalPathAndFilename;
							pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name);
							if (pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource)
							{
								//Set roughness,metalness intensity.
								pObjectMaterial->SetRoughness(WickedGetRoughnessStrength());
								pObjectMaterial->SetMetalness(WickedGetMetallnessStrength());

								// also enable AO from surface map
								pObjectMaterial->SetOcclusionEnabled_Primary(true);
								pObjectMaterial->SetOcclusionEnabled_Secondary(false);
							}
							else
							{
								pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = "";
							}

							// Parallax Occlusion Mapping (if HEIGHT TEXTURE used)
							bool bPOMShaderRequired = false;
							sFoundFinalPathAndFilename = sFoundTexturePath + WickedGetDisplacementName().Get();
							if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
							{
								sFoundFinalPathAndFilename = g_pWickedTexturePath + WickedGetDisplacementName().Get();
								if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
									sFoundFinalPathAndFilename = WickedGetDisplacementName().Get();
							}
							if (pObjectMaterial->textures[MaterialComponent::DISPLACEMENTMAP].resource)
							{
								pObjectMaterial->textures[MaterialComponent::DISPLACEMENTMAP].resource = nullptr;
								pObjectMaterial->textures[MaterialComponent::DISPLACEMENTMAP].name = "";
								pObjectMaterial->SetDirty();
								wiJobSystem::context ctx;
								wiJobSystem::Wait(ctx);
							}
							pObjectMaterial->textures[MaterialComponent::DISPLACEMENTMAP].name = sFoundFinalPathAndFilename;
							pObjectMaterial->textures[MaterialComponent::DISPLACEMENTMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::DISPLACEMENTMAP].name);
							if (pObjectMaterial->textures[MaterialComponent::DISPLACEMENTMAP].resource)
							{
								pObjectMaterial->parallaxOcclusionMapping = 0.05f;
								bPOMShaderRequired = true;
							}
							else
							{
								pObjectMaterial->textures[MaterialComponent::DISPLACEMENTMAP].name = "";
							}

							//Set emissive colors before map.
							DWORD dwEmmisiveColor = WickedGetEmmisiveColor();
							DWORD dwBaseColor = WickedGetBaseColor();
							if (dwBaseColor != -1)
							{
								pMesh->mMaterial.Diffuse.r = ((dwBaseColor & 0xff000000) >> 24) / 255.0f;;
								pMesh->mMaterial.Diffuse.g = ((dwBaseColor & 0x00ff0000) >> 16) / 255.0f;
								pMesh->mMaterial.Diffuse.b = ((dwBaseColor & 0x0000ff00) >> 8) / 255.0f;
								pMesh->mMaterial.Diffuse.a = (dwBaseColor & 0x000000ff) / 255.0f;
								WickedCall_SetMeshMaterial(pMesh,true);
							}

							//Emissive.
							if (sFoundTexturePath.size() <= 0)
							{
								//PE: Best hit rate.
								sFoundFinalPathAndFilename = g_pWickedTexturePath + WickedGetEmissiveName().Get();
								if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
								{
									//PE: Check not needed sFoundTexturePath empty.
									//sFoundFinalPathAndFilename = sFoundTexturePath + WickedGetEmissiveName().Get();
									sFoundFinalPathAndFilename = WickedGetEmissiveName().Get();
								}
							}
							else
							{
								sFoundFinalPathAndFilename = sFoundTexturePath + WickedGetEmissiveName().Get();
								if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
								{
									sFoundFinalPathAndFilename = g_pWickedTexturePath + WickedGetEmissiveName().Get();
									if (FileExist((LPSTR)sFoundFinalPathAndFilename.c_str()) == 0)
										sFoundFinalPathAndFilename = WickedGetEmissiveName().Get();
								}
							}
							if (pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource) //PE: Delete first if already active.
							{
								pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = nullptr;
								pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = "";
								pObjectMaterial->SetDirty();
								wiJobSystem::context ctx;
								wiJobSystem::Wait(ctx);
							}

							pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = sFoundFinalPathAndFilename;
							pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name);
							float fEmissive = WickedGetEmissiveStrength();
							if (pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource)
							{
								//Set Emissive intensity.
								pObjectMaterial->SetEmissiveStrength(fEmissive);

								//Change default emmisive color if needed.
								if (dwEmmisiveColor == -1)
								{
									pMesh->mMaterial.Emissive.r = 1.0f;
									pMesh->mMaterial.Emissive.g = 1.0f;
									pMesh->mMaterial.Emissive.b = 1.0f;
									WickedCall_SetMeshMaterial(pMesh, false);
								}

							}
							else
							{
								pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = "";
							}

							//PE: Moved here , We cant setup Emissive color before Emissive texture.
							if (dwEmmisiveColor != -1)
							{
								pMesh->mMaterial.Emissive.r = ((dwEmmisiveColor & 0xff000000) >> 24) / 255.0f;;
								pMesh->mMaterial.Emissive.g = ((dwEmmisiveColor & 0x00ff0000) >> 16) / 255.0f;
								pMesh->mMaterial.Emissive.b = ((dwEmmisiveColor & 0x0000ff00) >> 8) / 255.0f;
								pMesh->mMaterial.Emissive.a = (dwEmmisiveColor & 0x000000ff) / 255.0f;

								WickedCall_SetMeshMaterial(pMesh, false);
							}

							//PE: Special object settings.
							bool bTransparent = WickedGetTransparent();
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

							bool bDoubleSided = WickedDoubleSided();
							if (bDoubleSided)
								mesh->SetDoubleSided(true);
							else
								mesh->SetDoubleSided(false);

							float fRenderOrderBias = WickedRenderOrderBias();
							WickedCall_SetRenderOrderBias(pMesh, fRenderOrderBias);

							int iCustomShaderID = WickedCustomShaderID();
							float iCustomShaderParam1 = WickedCustomShaderParam1();
							float iCustomShaderParam2 = WickedCustomShaderParam2();
							float iCustomShaderParam3 = WickedCustomShaderParam3();
							float iCustomShaderParam4 = WickedCustomShaderParam4();
							float iCustomShaderParam5 = WickedCustomShaderParam5();
							float iCustomShaderParam6 = WickedCustomShaderParam6();
							float iCustomShaderParam7 = WickedCustomShaderParam7();
							pObjectMaterial->customShaderID = iCustomShaderID;
							pObjectMaterial->customShaderParam1 = iCustomShaderParam1;
							pObjectMaterial->customShaderParam2 = iCustomShaderParam2;
							pObjectMaterial->customShaderParam3 = iCustomShaderParam3;
							pObjectMaterial->customShaderParam4 = iCustomShaderParam4;
							pObjectMaterial->customShaderParam5 = iCustomShaderParam5;
							pObjectMaterial->customShaderParam6 = iCustomShaderParam6;
							pObjectMaterial->customShaderParam7 = iCustomShaderParam7;

							bool bPlanerReflection = WickedPlanerReflection();
							if (bPlanerReflection)
							{
								pObjectMaterial->shaderType = MaterialComponent::SHADERTYPE_PBR_PLANARREFLECTION;
							}
							else
							{
								if (pObjectMaterial->parallaxOcclusionMapping > 0.0f)
									pObjectMaterial->shaderType = MaterialComponent::SHADERTYPE_PBR_PARALLAXOCCLUSIONMAPPING;
								else
									pObjectMaterial->shaderType = MaterialComponent::SHADERTYPE_PBR;
							}

							bool bCastShadows = WickedGetCastShadows();
							if (bCastShadows)
							{
								pObjectMaterial->SetCastShadow(true);
							}
							else
							{
								pObjectMaterial->SetCastShadow(false);
							}

							float fReflectance = WickedGetReflectance();
							pObjectMaterial->SetReflectance(fReflectance);

							pObjectMaterial->SetDirty();

						}
						else
						{
							//PE: Use alpha ref from material.
							float fAlphaRef = WickedGetAlphaRef();
							if (fAlphaRef < 0.0) fAlphaRef = 1.0f;
							pObjectMaterial->SetAlphaRef(fAlphaRef);

							//Wicked material failed  , use old way.
							pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].name = ""; //Prevent wicked form trying to reload image.
							bWickedMaterialActive = false;

							//PE: Apply special settings even if not wicked material texture is set.
							bool bTransparent = WickedGetTransparent();
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

							bool bDoubleSided = WickedDoubleSided();
							if (bDoubleSided)
								mesh->SetDoubleSided(true);
							else
								mesh->SetDoubleSided(false);

							float fRenderOrderBias = WickedRenderOrderBias();
							WickedCall_SetRenderOrderBias(pMesh, fRenderOrderBias);

							int iCustomShaderID = WickedCustomShaderID();
							float iCustomShaderParam1 = WickedCustomShaderParam1();
							float iCustomShaderParam2 = WickedCustomShaderParam2();
							float iCustomShaderParam3 = WickedCustomShaderParam3();
							float iCustomShaderParam4 = WickedCustomShaderParam4();
							float iCustomShaderParam5 = WickedCustomShaderParam5();
							float iCustomShaderParam6 = WickedCustomShaderParam6();
							float iCustomShaderParam7 = WickedCustomShaderParam7();
							pObjectMaterial->customShaderID = iCustomShaderID;
							pObjectMaterial->customShaderParam1 = iCustomShaderParam1;
							pObjectMaterial->customShaderParam2 = iCustomShaderParam2;
							pObjectMaterial->customShaderParam3 = iCustomShaderParam3;
							pObjectMaterial->customShaderParam4 = iCustomShaderParam4;
							pObjectMaterial->customShaderParam5 = iCustomShaderParam5;
							pObjectMaterial->customShaderParam6 = iCustomShaderParam6;
							pObjectMaterial->customShaderParam7 = iCustomShaderParam7;

							bool bPlanerReflection = WickedPlanerReflection();
							if (bPlanerReflection)
							{
								pObjectMaterial->shaderType = MaterialComponent::SHADERTYPE_PBR_PLANARREFLECTION;
							}
							else
							{
								if (pObjectMaterial->parallaxOcclusionMapping > 0.0f)
									pObjectMaterial->shaderType = MaterialComponent::SHADERTYPE_PBR_PARALLAXOCCLUSIONMAPPING;
								else
									pObjectMaterial->shaderType = MaterialComponent::SHADERTYPE_PBR;
							}

							bool bCastShadows = WickedGetCastShadows();
							if (bCastShadows)
							{
								pObjectMaterial->SetCastShadow(true);
							}
							else
							{
								pObjectMaterial->SetCastShadow(false);
							}

							float fReflectance = WickedGetReflectance();
							pObjectMaterial->SetReflectance(fReflectance);

							pObjectMaterial->SetDirty();

						}
					}
					else
					{
						//PE: Use alpha ref from material.
						float fAlphaRef = WickedGetAlphaRef();
						if (fAlphaRef < 0.0) fAlphaRef = 1.0f;
						pObjectMaterial->SetAlphaRef(fAlphaRef);

						//Wicked material failed  , use old way.
						bWickedMaterialActive = false;

						//PE: Apply special settings even if not wicked material texture is set.
						bool bTransparent = WickedGetTransparent();
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

						bool bDoubleSided = WickedDoubleSided();
						if (bDoubleSided)
							mesh->SetDoubleSided(true);
						else
							mesh->SetDoubleSided(false);

						float fRenderOrderBias = WickedRenderOrderBias();
						WickedCall_SetRenderOrderBias(pMesh, fRenderOrderBias);

						int iCustomShaderID = WickedCustomShaderID();
						float iCustomShaderParam1 = WickedCustomShaderParam1();
						float iCustomShaderParam2 = WickedCustomShaderParam2();
						float iCustomShaderParam3 = WickedCustomShaderParam3();
						float iCustomShaderParam4 = WickedCustomShaderParam4();
						float iCustomShaderParam5 = WickedCustomShaderParam5();
						float iCustomShaderParam6 = WickedCustomShaderParam6();
						float iCustomShaderParam7 = WickedCustomShaderParam7();
						pObjectMaterial->customShaderID = iCustomShaderID;
						pObjectMaterial->customShaderParam1 = iCustomShaderParam1;
						pObjectMaterial->customShaderParam2 = iCustomShaderParam2;
						pObjectMaterial->customShaderParam3 = iCustomShaderParam3;
						pObjectMaterial->customShaderParam4 = iCustomShaderParam4;
						pObjectMaterial->customShaderParam5 = iCustomShaderParam5;
						pObjectMaterial->customShaderParam6 = iCustomShaderParam6;
						pObjectMaterial->customShaderParam7 = iCustomShaderParam7;

						bool bPlanerReflection = WickedPlanerReflection();
						if (bPlanerReflection)
						{
							pObjectMaterial->shaderType = MaterialComponent::SHADERTYPE_PBR_PLANARREFLECTION;
						}
						else
						{
							if (pObjectMaterial->parallaxOcclusionMapping > 0.0f)
								pObjectMaterial->shaderType = MaterialComponent::SHADERTYPE_PBR_PARALLAXOCCLUSIONMAPPING;
							else
								pObjectMaterial->shaderType = MaterialComponent::SHADERTYPE_PBR;
						}

						bool bCastShadows = WickedGetCastShadows();
						if (bCastShadows)
						{
							pObjectMaterial->SetCastShadow(true);
						}
						else
						{
							pObjectMaterial->SetCastShadow(false);
						}

						float fReflectance = WickedGetReflectance();
						pObjectMaterial->SetReflectance(fReflectance);

						pObjectMaterial->SetDirty();

					}
				}

				if (!bWickedMaterialActive)
				{
					// skip normal, surface and emissive PBR Ready Constructions if already loaded in
					bool bGotNormalTexture = false;
					bool bGotSurfaceTexture = false;
					bool bGotEmissiveTexture = false;

					if (pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource) //PE: Delete first if already active.
					{
						pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource = nullptr;
						pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].name = "";
						pObjectMaterial->SetDirty();
						wiJobSystem::context ctx;
						wiJobSystem::Wait(ctx);
					}

					// go through all permutations until we find the texture
					for (int findit = 0; findit <= 5; findit++)
					{
						if (findit == 0) { sFoundFinalPathAndFilename = sFoundTexturePath + sFoundTextureFilename + sFoundTextureType; }
						if (findit == 1) { sFoundFinalPathAndFilename = g_pWickedTexturePath + sFoundTextureFilename + sFoundTextureType; }
						if (findit == 2) { sFoundTextureType = ".dds"; sFoundFinalPathAndFilename = sFoundTexturePath + sFoundTextureFilename + sFoundTextureType; }
						if (findit == 3) { sFoundTextureType = ".dds"; sFoundFinalPathAndFilename = g_pWickedTexturePath + sFoundTextureFilename + sFoundTextureType; }
						if (findit == 4) { sFoundTextureType = ".png"; sFoundFinalPathAndFilename = sFoundTexturePath + sFoundTextureFilename + sFoundTextureType; }
						if (findit == 5) { sFoundTextureType = ".png"; sFoundFinalPathAndFilename = g_pWickedTexturePath + sFoundTextureFilename + sFoundTextureType; }

						pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].name = sFoundFinalPathAndFilename;
						pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].name);
						if (pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource)
						{
							// found the texture file location and type
							// carried in sFoundFinalPathAndFilename
							bFoundTextureToLoad = true;

							//PE: For "CloneObject" we need to update. pMesh->pTextures[0].pName
							//PE: To the full path as g_pWickedTexturePath is lost later.
							strcpy(pMesh->pTextures[0].pName, sFoundFinalPathAndFilename.c_str());

							break;
						}
						else
						{
							pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].name = "";
						}
					}

					if ( pMesh->dwTextureCount > GG_MESH_TEXTURE_NORMAL && *(pMesh->pTextures[GG_MESH_TEXTURE_NORMAL].pName) )
					{
						// Normal texture
						if (pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource) // Delete first if already active.
						{
							pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource = nullptr;
							pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = "";
							pObjectMaterial->SetDirty();
							wiJobSystem::context ctx;
							wiJobSystem::Wait(ctx);
						}
						pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = pMesh->pTextures[GG_MESH_TEXTURE_NORMAL].pName;
						if (!FileExist((char*)pObjectMaterial->textures[MaterialComponent::NORMALMAP].name.c_str())) pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = "";
						else
						{
							pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::NORMALMAP].name);
							bGotNormalTexture = true;
						}
					}

					if ( pMesh->dwTextureCount > GG_MESH_TEXTURE_SURFACE && *(pMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName) )
					{
						// Ambient occlusion texture
						if (pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource) // Delete first if already active.
						{
							pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource = nullptr;
							pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = "";
							pObjectMaterial->SetDirty();
							wiJobSystem::context ctx;
							wiJobSystem::Wait(ctx);
						}
						pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = pMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName;
						if (!FileExist((char*)pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name.c_str())) 
							pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = "";
						else
						{
							pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name);
							pObjectMaterial->SetOcclusionEnabled_Primary(true);
							pObjectMaterial->SetOcclusionEnabled_Secondary(false);
							bGotSurfaceTexture = true;
						}
					}
					if ( pMesh->dwTextureCount > GG_MESH_TEXTURE_EMISSIVE && *(pMesh->pTextures[GG_MESH_TEXTURE_EMISSIVE].pName) )
					{
						// Emissive texture
						if (pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource) // Delete first if already active.
						{
							pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = nullptr;
							pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = "";
							pObjectMaterial->SetDirty();
							wiJobSystem::context ctx;
							wiJobSystem::Wait(ctx);
						}
						pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = pMesh->pTextures[GG_MESH_TEXTURE_EMISSIVE].pName;
						if (!FileExist((char*)pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name.c_str())) pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = "";
						else
						{
							pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name);
							bGotEmissiveTexture = true;
						}
					}

					if ( pMesh->bTransparency )
						pObjectMaterial->userBlendMode = BLENDMODE_ALPHA;

					// default emissive is off (when not custom material)
					pMesh->mMaterial.Emissive.r = 0.0f;
					pMesh->mMaterial.Emissive.g = 0.0f;
					pMesh->mMaterial.Emissive.b = 0.0f;
					WickedCall_SetMeshMaterial(pMesh, false);

					// if found and loaded base texture, continue with rest
					if (bFoundTextureToLoad == true)
					{
						// default texture format is DDS (if not already determined)
						if (sFoundTextureType == "") sFoundTextureType = ".dds";

						// have a backup format in case PNG/DDS twizzles happening in source asset!
						std::string sAltTextureType = ".dds";
						if (sFoundTextureType == ".dds") sAltTextureType = ".png";

						// determine if filename is ready for PBR additions, or is regular 'nonPBR' full filename
						bool bPBRReady = false;
						char pDetectUnderscoreColor[MAX_PATH];
						strcpy(pDetectUnderscoreColor, sFoundFinalPathAndFilename.c_str());
						//pDetectUnderscoreColor[strlen(pDetectUnderscoreColor) - 4] = 0; // extension might be more than 4 characters (.jpeg)
						char* pExt = strrchr( pDetectUnderscoreColor, '.' );
						if ( pExt ) *pExt = 0;
						if (strnicmp(pDetectUnderscoreColor + strlen(pDetectUnderscoreColor) - 6, "_color", 6) == NULL) bPBRReady = true;

						// apply various PBR textures to mesh material
						if (bPBRReady == true)
						{
							// Remove the _color.xxx part
							char pTrimFinalTextureFilenameBase[MAX_PATH];
							strcpy(pTrimFinalTextureFilenameBase, sFoundFinalPathAndFilename.c_str());
							pTrimFinalTextureFilenameBase[strlen(pTrimFinalTextureFilenameBase) - 10] = 0;
							sTextureFilenameBase = pTrimFinalTextureFilenameBase;

							// PBR Color already loaded, what about normal
							if (bGotNormalTexture == false)
							{
								// PBR Normal texture
								if (pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource) //PE: Delete first if already active.
								{
									pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource = nullptr;
									pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = "";
									pObjectMaterial->SetDirty();
									wiJobSystem::context ctx;
									wiJobSystem::Wait(ctx);
								}
								pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = sTextureFilenameBase + "_normal" + sFoundTextureType;
								if (!FileExist((char*)pObjectMaterial->textures[MaterialComponent::NORMALMAP].name.c_str())) pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = sTextureFilenameBase + "_normal" + sAltTextureType;
								pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::NORMALMAP].name);
								if (!pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource)
								{
									pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = "";
								}
							}

							// If SURFACE texture not present, create it
							if (bGotSurfaceTexture == false && (!pObjectMaterial->IsUsingVertexColors() || pMesh->iReservedForFuture > 10 ))
							{
								// PBR Surface texture
								if (pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource) //PE: Delete first if already active.
								{
									pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource = nullptr;
									pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = "";
									pObjectMaterial->SetDirty();
									wiJobSystem::context ctx;
									wiJobSystem::Wait(ctx);
								}
								//LB: breaks when importer model has a PNG for the surface texture when importing
								//PE: Surface must be dds, So make sure we always save as .dds, we can still merge from .png ...
								//std::string surfaceTexFile = sTextureFilenameBase + "_surface" + ".dds";
								//pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = surfaceTexFile;
								pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = sTextureFilenameBase + "_surface" + sFoundTextureType;
								if (!FileExist((char*)pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name.c_str())) pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = sTextureFilenameBase + "_surface" + sAltTextureType;
								pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name);
								std::string surfaceTexFile = "";
								if (!pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource)
								{
									// could not load DDS or PNG surface during the import, fall back and make one
									pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = "";
									surfaceTexFile = sTextureFilenameBase + "_surface" + ".dds";
								}
								LPSTR pSurfaceTexFile = (char*)surfaceTexFile.c_str();
								if (pSurfaceTexFile && strlen(pSurfaceTexFile)>0 && !FileExist(pSurfaceTexFile))
								{
									//PE: It should save to the docwrite folder in the correct location ?.
									//PE: We have relative path here, so the newSurfaceFileTemp check dont work, without resolving it first.
									char resolve[MAX_PATH];
									strcpy(resolve, pSurfaceTexFile);
									GG_GetRealPath(resolve, 1);
									pSurfaceTexFile = resolve;

									// surface file location must be temp if not in MAX folder or GameGuruApps folder
									char newSurfaceFileTemp[MAX_PATH];
									GG_SetWritablesToRoot(true);
									strcpy(newSurfaceFileTemp, GG_GetWritePath());
									GG_SetWritablesToRoot(false);
									if (strnicmp (pSurfaceTexFile, g_rootFolder.c_str(), strlen(g_rootFolder.c_str())) != NULL
										&& strnicmp (pSurfaceTexFile, newSurfaceFileTemp, strlen(newSurfaceFileTemp)) != NULL)
									{
										// surface should not be created outside MAX folders (unwelcome)
										// so use our temporary file area (importer would then copy these when saving object)
										char fullTextureName[MAX_PATH];
										char stripTextureName[MAX_PATH];
										strcpy(stripTextureName, "import_generate");
										strcpy(fullTextureName, (char*)sTextureFilenameBase.c_str());
										for (int n = strlen(fullTextureName) - 1; n > 0; n--)
										{
											if (fullTextureName[n] == '\\' || fullTextureName[n] == '/')
											{
												strcpy(stripTextureName, fullTextureName + n + 1);
												break;
											}
										}
										strcat(newSurfaceFileTemp, "imported_models\\");
										strcat(newSurfaceFileTemp, stripTextureName);
										strcat(newSurfaceFileTemp, "_surface.dds");
										surfaceTexFile = newSurfaceFileTemp;
										pSurfaceTexFile = (char*)surfaceTexFile.c_str();
										pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = surfaceTexFile;
									}

									// create surface texture from PBR texture set provided
									std::string sTextureAO = sTextureFilenameBase + "_ao" + sFoundTextureType;
									std::string sTextureGloss = sTextureFilenameBase + "_gloss" + sFoundTextureType;
									std::string sTextureMetalness = sTextureFilenameBase + "_metalness" + sFoundTextureType;
									if (!FileExist((char*)sTextureAO.c_str())) sTextureAO = sTextureFilenameBase + "_ao" + sAltTextureType;
									if (!FileExist((char*)sTextureGloss.c_str())) sTextureGloss = sTextureFilenameBase + "_gloss" + sAltTextureType;
									if (!FileExist((char*)sTextureMetalness.c_str())) sTextureMetalness = sTextureFilenameBase + "_metalness" + sAltTextureType;
									ImageCreateSurfaceTexture(pSurfaceTexFile, (char*)sTextureAO.c_str(), (char*)sTextureGloss.c_str(), (char*)sTextureMetalness.c_str());

									// and assign this surface to DBO mesh along with channel info (used by importer and other code)
									if (pMesh->dwTextureCount < GG_MESH_TEXTURE_SURFACE)
									{
										//PE: Realloc we get a heap errors here.
										//PE: And are generating random crashes from everywhere.
										extern bool EnsureTextureStageValid(sMesh* pMesh, int iTextureStage);
										EnsureTextureStageValid(pMesh, GG_MESH_TEXTURE_SURFACE);
									}

									strcpy (pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].pName, pSurfaceTexFile);
									pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].channelMask = (0 << 4) + (15);
									strcpy (pMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName, pSurfaceTexFile);
									pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].channelMask = (1 << 4) + (15);
									strcpy (pMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName, pSurfaceTexFile);
									pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].channelMask = (2 << 4) + (15);
								}

								// PBR Surface texture
								pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name);
								if (pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource)
								{
									pObjectMaterial->roughness = 1;
									pObjectMaterial->metalness = 1;
									pObjectMaterial->SetOcclusionEnabled_Primary(true);
									pObjectMaterial->SetOcclusionEnabled_Secondary(false);
								}
							}

							// PBR emissive
							if ( bGotEmissiveTexture == false )
							{
								if (pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource) //PE: Delete first if already active.
								{
									pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = nullptr;
									pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = "";
									pObjectMaterial->SetDirty();
									wiJobSystem::context ctx;
									wiJobSystem::Wait(ctx);
								}
								pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = sTextureFilenameBase + "_emissive" + sFoundTextureType;
								if (!FileExist((char*)pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name.c_str())) pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = sTextureFilenameBase + "_emissive" + sAltTextureType;
								pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name);
								if (pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource)
								{
									//PE: We need a default emissiveColor for anything to illume.
									pMesh->mMaterial.Emissive.r = 1.0f;
									pMesh->mMaterial.Emissive.g = 1.0f;
									pMesh->mMaterial.Emissive.b = 1.0f;
									pObjectMaterial->emissiveColor.x = pMesh->mMaterial.Emissive.r;
									pObjectMaterial->emissiveColor.y = pMesh->mMaterial.Emissive.g;
									pObjectMaterial->emissiveColor.z = pMesh->mMaterial.Emissive.b;
									pObjectMaterial->emissiveColor.w = 1.0f;
								}
								else
								{
									pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = sTextureFilenameBase + "_illum" + sFoundTextureType;
									if (!FileExist((char*)pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name.c_str())) pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = sTextureFilenameBase + "_illum" + sAltTextureType;
									pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name);
									if (pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource)
									{
										//PE: We need a default emissiveColor for anything to illume.
										pMesh->mMaterial.Emissive.r = 1.0f;
										pMesh->mMaterial.Emissive.g = 1.0f;
										pMesh->mMaterial.Emissive.b = 1.0f;
										pObjectMaterial->emissiveColor.x = pMesh->mMaterial.Emissive.r;
										pObjectMaterial->emissiveColor.y = pMesh->mMaterial.Emissive.g;
										pObjectMaterial->emissiveColor.z = pMesh->mMaterial.Emissive.b;
										pObjectMaterial->emissiveColor.w = 1.0f;
									}
									else
									{
										pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = sTextureFilenameBase + "_illumination" + sFoundTextureType;
										if (!FileExist((char*)pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name.c_str())) pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = sTextureFilenameBase + "_illumination" + sAltTextureType;
										pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name);
										if (pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource)
										{
											//PE: We need a default emissiveColor for anything to illume.
											pMesh->mMaterial.Emissive.r = 1.0f;
											pMesh->mMaterial.Emissive.g = 1.0f;
											pMesh->mMaterial.Emissive.b = 1.0f;
											pObjectMaterial->emissiveColor.x = pMesh->mMaterial.Emissive.r;
											pObjectMaterial->emissiveColor.y = pMesh->mMaterial.Emissive.g;
											pObjectMaterial->emissiveColor.z = pMesh->mMaterial.Emissive.b;
											pObjectMaterial->emissiveColor.w = 1.0f;
										}
										else
										{
											//Try old DNS I
											pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = sTextureFilenameBase + "_i" + sFoundTextureType;
											if (!FileExist((char*)pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name.c_str())) pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = sTextureFilenameBase + "_i" + sAltTextureType;
											pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name);
											if (pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource)
											{
												//PE: We need a default emissiveColor for anything to illume.
												pMesh->mMaterial.Emissive.r = 1.0f;
												pMesh->mMaterial.Emissive.g = 1.0f;
												pMesh->mMaterial.Emissive.b = 1.0f;
												pObjectMaterial->emissiveColor.x = pMesh->mMaterial.Emissive.r;
												pObjectMaterial->emissiveColor.y = pMesh->mMaterial.Emissive.g;
												pObjectMaterial->emissiveColor.z = pMesh->mMaterial.Emissive.b;
												pObjectMaterial->emissiveColor.w = 1.0f;
											}
											else
											{
												//PE: Wicked will retry loading images if the name is set and got no resource, so clear it.
												pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = ""; //Prevent wicked from trying to reload.
											}
										}
									}
								}
							}
						}
						else
						{
							// very old DNS texture support - Diffuse sorted, but need normal and [what to do with old specular]
							bool bDNSReady = false;
							if (strnicmp(pDetectUnderscoreColor + strlen(pDetectUnderscoreColor) - 2, "_d", 2) == NULL) bDNSReady = true;
							if (bDNSReady == true)
							{
								// Remove the _D.xxx part
								char pTrimFinalTextureFilenameBase[MAX_PATH];
								strcpy(pTrimFinalTextureFilenameBase, sFoundFinalPathAndFilename.c_str());
								pTrimFinalTextureFilenameBase[strlen(pTrimFinalTextureFilenameBase) - 6] = 0;
								sTextureFilenameBase = pTrimFinalTextureFilenameBase;

								// Normal texture
								if (pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource) //PE: Delete first if already active.
								{
									pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource = nullptr;
									pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = "";
									pObjectMaterial->SetDirty();
									wiJobSystem::context ctx;
									wiJobSystem::Wait(ctx);
								}
								pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = sTextureFilenameBase + "_n" + sFoundTextureType;
								if (!FileExist((char*)pObjectMaterial->textures[MaterialComponent::NORMALMAP].name.c_str())) pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = sTextureFilenameBase + "_n" + sAltTextureType;
								pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::NORMALMAP].name);
								if (!pObjectMaterial->textures[MaterialComponent::NORMALMAP].resource)
								{
									pObjectMaterial->textures[MaterialComponent::NORMALMAP].name = "";
								}

								// Surface texture
								if (pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource) //PE: Delete first if already active.
								{
									pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource = nullptr;
									pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = "";
									pObjectMaterial->SetDirty();
									wiJobSystem::context ctx;
									wiJobSystem::Wait(ctx);
								}
								pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = sTextureFilenameBase + "_surface" + ".dds";
								if (!FileExist((char*)pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name.c_str()))
								{
									std::string sTextureGloss = sTextureFilenameBase + "_s" + sFoundTextureType;
									std::string sTextureMetalness = sTextureFilenameBase + "_s" + sFoundTextureType;
									if (!FileExist((char*)sTextureGloss.c_str())) sTextureGloss = sTextureFilenameBase + "_s" + sAltTextureType;
									if (!FileExist((char*)sTextureMetalness.c_str())) sTextureMetalness = sTextureFilenameBase + "_s" + sAltTextureType;
									LPSTR pSurfaceTexFile = (char*)pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name.c_str();
									ImageCreateSurfaceTexture(pSurfaceTexFile, NULL, (char*)sTextureGloss.c_str(), (char*)sTextureMetalness.c_str());

									if (pMesh->dwTextureCount < GG_MESH_TEXTURE_SURFACE)
									{
										//PE: Realloc we get a heap errors here.
										//PE: And are generating random crashes from everywhere.
										extern bool EnsureTextureStageValid(sMesh* pMesh, int iTextureStage);
										EnsureTextureStageValid(pMesh, GG_MESH_TEXTURE_SURFACE);

									}

									// and assign this surface to DBO mesh along with channel info (used by importer and other code)
									strcpy ( pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].pName, pSurfaceTexFile);
									pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].channelMask = (0 << 4) + (15);
									strcpy ( pMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName, pSurfaceTexFile);
									pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].channelMask = (1 << 4) + (15);
									strcpy ( pMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName, pSurfaceTexFile);
									pMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].channelMask = (2 << 4) + (15);
								}
								pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name);
								if (pObjectMaterial->textures[MaterialComponent::SURFACEMAP].resource)
								{
									pObjectMaterial->roughness = 1;
									pObjectMaterial->metalness = 1;
									pObjectMaterial->SetOcclusionEnabled_Primary(true);
									pObjectMaterial->SetOcclusionEnabled_Secondary(false);
								}
								else
								{
									pObjectMaterial->textures[MaterialComponent::SURFACEMAP].name = "";
								}

								//Try old DNS I
								pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = sTextureFilenameBase + "_i" + sFoundTextureType;
								if (!FileExist((char*)pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name.c_str())) pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = sTextureFilenameBase + "_i" + sAltTextureType;
								pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name);
								if (pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource)
								{
									pObjectMaterial->SetEmissiveStrength(1.0f);
								}
								else
								{
									pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = "";
								}
							}
						}

						// special flag which moves base color to emissive so surface will be the same
						// no matter which angle it is viewed at or what lights are nearby (smoke, muzzle flashes)
						if (g_iWickedPutInEmissiveMode > 0)
						{
							// emissive override
							if (pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource)
							{
								pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = nullptr;
								pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = "";
								pObjectMaterial->SetDirty();
								wiJobSystem::context ctx;
								wiJobSystem::Wait(ctx);
							}
							pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].name;
							pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource;
							if (!pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource)
							{
								pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = "";
							}
							pObjectMaterial->SetEmissiveStrength(1.0f);
						}

						// ensure material refs updated for rendering
						pObjectMaterial->SetDirty();
					}
				}
			}
		}
	}
}

void WickedCall_TextureObjectAsEmissive(sObject* pObject)
{
	for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
	{
		sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
		if (pMesh)
		{
			WickedSetMeshNumber(iMeshIndex);
			WickedCall_SetAsEmissiveMaterial(pMesh);
		}
	}
}

void WickedCall_SetAsEmissiveMaterial(sMesh* pMesh)
{
	if (pMesh)
	{
		// get wicked meshID
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
		if (mesh)
		{
			// get material from mesh
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
			if (pObjectMaterial)
			{
				// steal the base texture and give it to the emissive
				pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].name;
				pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = WickedCall_LoadImage(pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name);

				// set base color to black
				pMesh->mMaterial.Diffuse.r = 0;
				pMesh->mMaterial.Diffuse.g = 0;
				pMesh->mMaterial.Diffuse.b = 0;
				pMesh->mMaterial.Diffuse.a = 0;
				pObjectMaterial->SetBaseColor(XMFLOAT4(pMesh->mMaterial.Diffuse.r, pMesh->mMaterial.Diffuse.g, pMesh->mMaterial.Diffuse.b, pMesh->mMaterial.Diffuse.a));

				// set emissve to white
				pMesh->mMaterial.Emissive.r = 1;
				pMesh->mMaterial.Emissive.g = 1;
				pMesh->mMaterial.Emissive.b = 1;
				pMesh->mMaterial.Emissive.a = 1;
				pObjectMaterial->SetEmissiveColor(XMFLOAT4(pMesh->mMaterial.Emissive.r, pMesh->mMaterial.Emissive.g, pMesh->mMaterial.Emissive.b, pMesh->mMaterial.Emissive.a));

				// ensure material refs updated for rendering
				pObjectMaterial->SetDirty();
			}
		}
	}
}

void WickedCall_SetReflectance(sMesh* pMesh, float fReflectance)
{
	if (pMesh)
	{
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
		if (mesh)
		{
			// get material from mesh
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
			if (pObjectMaterial)
			{
				pObjectMaterial->reflectance = fReflectance;
			}
		}
	}
}

void WickedCall_SetObjectReflectance(sObject* pObject, float fReflectance)
{
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
		if (pObject->ppMeshList[iM])
			WickedCall_SetReflectance(pObject->ppMeshList[iM], fReflectance);
}

float WickedCall_GetObjectReflectance(sObject* pObject)
{
	float fReflectance = 0.0f;
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		sMesh* pMesh = pObject->ppMeshList[iM];
		if (pMesh)
		{
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
			if (mesh)
			{
				uint64_t materialEntity = mesh->subsets[0].materialID;
				wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
				if (pObjectMaterial)
				{
					fReflectance = pObjectMaterial->reflectance;
					break;
				}
			}
		}
	}
	return fReflectance;
}

void WickedCall_SetMeshCullmode(sMesh* pMesh)
{
	if (pMesh)
	{
		// get wicked meshID
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
		if (mesh)
		{
			if(pMesh->iCullMode > 0)
				mesh->SetDoubleSided(false);
			else
				mesh->SetDoubleSided(true);
		}
	}
}

void WickedCall_SetObjectCullmode(sObject* pObject)
{
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
		WickedCall_SetMeshCullmode(pObject->ppMeshList[iM]);
}

void WickedCall_SetObjectDoubleSided(sObject* pObject, bool bDoubleSided)
{
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		sMesh* pMesh = pObject->ppMeshList[iM];
		if (pMesh)
		{
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
			if (mesh)
			{
				mesh->SetDoubleSided(bDoubleSided);
			}
		}
	}
}

bool WickedCall_GetObjectDoubleSided(sObject* pObject)
{
	bool bDoubleSided = false;
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		sMesh* pMesh = pObject->ppMeshList[iM];
		if (pMesh)
		{
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
			if (mesh)
			{
				bDoubleSided = mesh->IsDoubleSided();
				break;
			}
		}
	}
	return bDoubleSided;
}

void WickedCall_SetMeshTransparent(sMesh* pMesh)
{
	if (pMesh)
	{
		// get wicked meshID
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
		if (mesh)
		{
			// get material from mesh
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
			if (pObjectMaterial)
			{
				if (pMesh->bTransparency)
					pObjectMaterial->userBlendMode = BLENDMODE_ALPHA;
				else
					pObjectMaterial->userBlendMode = BLENDMODE_OPAQUE;
				pObjectMaterial->IsDirty();
			}
		}
	}
}

void WickedCall_SetObjectTransparent(sObject* pObject)
{
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		//Respect per mesh transparancy.
		if (pObject->ppMeshList[iM])
		{
			WickedSetMeshNumber(iM);
			bool bWickedMaterialActive = IsWickedMaterialActive(pObject->ppMeshList[iM]);
			if (bWickedMaterialActive)
			{
				bool bTransparent = WickedGetTransparent();
				if (bTransparent)
				{
					pObject->ppMeshList[iM]->bTransparency = true;
				}
				else
				{
					//if (g_bSpecialCaseWeMustForceTransparencyChoicePerMesh == true)
					//{
					//	// this flag is set when called from 'recreateentitycursor' when user modifies object then clicks it in editor
					//	// and need all transparency states to be properly restored!
					//	pObject->ppMeshList[iM]->bTransparency = false;
					//}
					//else
					//{
					//PE: To respect fpe transparency=6 , we dont disable, but use fpe settings in mesh.
					//PE: So we only support enabling transparent per mesh, (importer).
					//}
				}
			}
			WickedCall_SetMeshTransparent(pObject->ppMeshList[iM]);
		}
	}
}

void WickedCall_SetMeshDisableDepth(sMesh* pMesh, bool bDisable)
{
	if (pMesh)
	{
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
		if (mesh)
		{
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
			if (pObjectMaterial)
			{
				if (bDisable == true)
				{
					pObjectMaterial->userBlendMode = BLENDMODE_FORCEDEPTH;
					pObjectMaterial->shaderType = MaterialComponent::SHADERTYPE_WEAPON;
					pObjectMaterial->SetDoubleSided(true);
				}
				else
				{
					pObjectMaterial->userBlendMode = BLENDMODE_OPAQUE;
					pObjectMaterial->SetDoubleSided(false);
				}
				pObjectMaterial->IsDirty();
			}
		}
	}
}

void WickedCall_SetObjectDisableDepth(sObject* pObject, bool bDisable)
{
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		if (pObject->ppMeshList[iM])
		{
			WickedCall_SetMeshDisableDepth(pObject->ppMeshList[iM], bDisable);
		}
	}
	if (bDisable == false)
	{
		// additionally restore if object transparent or opaque, and doublesided(cull mode)
		WickedCall_SetObjectTransparent(pObject);
		WickedCall_SetObjectCullmode(pObject);
	}
}

std::string WickedCall_GetAllTexturesUsed(sObject* pObject)
{
	std::string sTmp = "";
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		sMesh* pMesh = pObject->ppMeshList[iM];
		if (pMesh)
		{
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
			if (mesh)
			{
				// get material from mesh
				uint64_t materialEntity = mesh->subsets[0].materialID;
				wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
				if (pObjectMaterial)
				{
					for (int i = 0; i < MaterialComponent::TEXTURESLOT_COUNT; i++)
					{
						//Extract textures.
						if (pObjectMaterial->textures[i].resource)
						{
							if (pObjectMaterial->textures[i].name.length() > 0)
							{
								const char *pestrcasestr(const char *arg1, const char *arg2);
								if( !pestrcasestr(sTmp.c_str(), pObjectMaterial->textures[i].name.c_str()) )
									sTmp += pObjectMaterial->textures[i].name + "|";
							}
						}
					}
				}
			}
		}
	}
	return sTmp;
}

void WickedCall_SetMeshAlpha(sMesh* pMesh, float fPercentage)
{
	if (pMesh)
	{
		// get wicked meshID
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
		if (mesh)
		{
			// get material from mesh
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
			if (pObjectMaterial)
			{
				pObjectMaterial->SetOpacity(fPercentage/100.0);
				pObjectMaterial->IsDirty();
			}
		}
	}
}

LPSTR WickedCall_GetMeshMaterialName(sMesh* pMesh)
{
	LPSTR pName = NULL;
	if (pMesh)
	{
		// get wicked meshID
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
		if (mesh)
		{
			// get material from mesh
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
			if (pObjectMaterial)
			{
				// return a pointer to the material basecolor name (used to determine if a successful texture was loaded)
				// in place of the old method of checking the iImageID which now may be zero
				if ( pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource )
					pName = (LPSTR)pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].name.c_str();
			}
		}
	}
	return pName;
}

void WickedCall_GetFrameWorldPos(sFrame* pFrame, float* pfX, float* pfY, float* pfZ)
{
	if (pFrame)
	{
		uint64_t wickedobjindex = pFrame->wickedobjindex;
		wiScene::TransformComponent* pFrameTransform = wiScene::GetScene().transforms.GetComponent(wickedobjindex);
		if (pFrameTransform)
		{
			*pfX = pFrameTransform->GetPosition().x;
			*pfY = pFrameTransform->GetPosition().y;
			*pfZ = pFrameTransform->GetPosition().z;
		}
	}
}

void WickedCall_GetGluedLimbWorldPos(sObject* pObject, int iLimbID, float* pfX, float* pfY, float* pfZ)
{
	int iObjectParent = pObject->position.iGluedToObj;
	sObject* GetObjectData(int iID);
	sObject* pGluedTo = GetObjectData(iObjectParent);
	if (pGluedTo)
	{
		sFrame* pFrame = pObject->ppFrameList[0];
		if (iLimbID < pObject->iFrameCount) pFrame = pObject->ppFrameList[iLimbID];
		uint64_t wickedobjindex = pFrame->wickedobjindex;
		wiScene::TransformComponent* pFrameTransform = wiScene::GetScene().transforms.GetComponent(wickedobjindex);
		if (pFrameTransform)
		{
			wiScene::TransformComponent transform;
			transform.ClearTransform();
			transform.Translate(XMFLOAT3(0, 0, 0));
			transform.RotateRollPitchYaw(XMFLOAT3(0, 0, 0));
			transform.UpdateTransform_Parented(*pFrameTransform);
			*pfX = transform.GetPosition().x;
			*pfY = transform.GetPosition().y;
			*pfZ = transform.GetPosition().z;
		}
	}
}

void WickedCall_GetLimbDataEx(sObject* pObject, int iLimbID, bool bAdjustLimb, float fX, float fY, float fZ, float fAX, float fAY, float fAZ, float* pX, float* pY, float* pZ, float* pQAX, float* pQAY, float* pQAZ, float* pQAW)
{
	if ( pObject )
	{
		sFrame* pFrame = pObject->ppFrameList[iLimbID];
		if ( pFrame )
		{
			uint64_t iFrameWickedObjectNumber = pFrame->wickedobjindex;
			if (iFrameWickedObjectNumber > 0)
			{
				// set a transform for the object
				wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(iFrameWickedObjectNumber);
				if (pObject->position.bCustomWorldMatrix == false)
				{
					if (bAdjustLimb)
					{
						wiScene::TransformComponent transform;
						transform.ClearTransform();
						transform.Translate(XMFLOAT3(fX, fY, fZ));
						transform.RotateRollPitchYaw(XMFLOAT3(fAX, fAY, fAZ));
						//PE: Optimizing this is hitting TransformComponent::GetLocalMatrix() heavy.
						transform.UpdateTransform_Parented(*pTransform);
						*pX = transform.GetPosition().x;
						*pY = transform.GetPosition().y;
						*pZ = transform.GetPosition().z;
						if (pQAX)
						{
							*pQAX = transform.GetRotation().x;
							*pQAY = transform.GetRotation().y;
							*pQAZ = transform.GetRotation().z;
							*pQAW = transform.GetRotation().w;
						}
					}
					else
					{
						*pX = pTransform->GetPosition().x;
						*pY = pTransform->GetPosition().y;
						*pZ = pTransform->GetPosition().z;
						if (pQAX)
						{
							*pQAX = pTransform->GetRotation().x;
							*pQAY = pTransform->GetRotation().y;
							*pQAZ = pTransform->GetRotation().z;
							*pQAW = pTransform->GetRotation().w;
						}
					}
				}
			}
		}
	}
}

void WickedCall_GetLimbLocalPosAndRot(sObject* pObject, int iLimbID, float* pX, float* pY, float* pZ, float* pQAX, float* pQAY, float* pQAZ, float* pQAW)
{
	if (pObject)
	{
		sFrame* pFrame = pObject->ppFrameList[iLimbID];
		if (pFrame)
		{
			uint64_t iFrameWickedObjectNumber = pFrame->wickedobjindex;
			if (iFrameWickedObjectNumber > 0)
			{
				// set a transform for the object
				wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(iFrameWickedObjectNumber);
				*pX = pTransform->translation_local.x;
				*pY = pTransform->translation_local.y;
				*pZ = pTransform->translation_local.z;
				*pQAX = pTransform->rotation_local.x;
				*pQAY = pTransform->rotation_local.y;
				*pQAZ = pTransform->rotation_local.z;
				*pQAW = pTransform->rotation_local.w;
			}
		}
	}
}

void WickedCall_GetLimbData(sObject* pObject, int iLimbID, float* pX, float* pY, float* pZ, float* pQAX, float* pQAY, float* pQAZ, float* pQAW)
{
	WickedCall_GetLimbDataEx(pObject, iLimbID, false, 0, 0, 0, 0, 0, 0, pX, pY, pZ, pQAX, pQAY, pQAZ, pQAW);
}

void WickedCall_UpdateMeshVertexData(sMesh* pDBOMesh)
{
	if (pDBOMesh)
	{
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pDBOMesh->wickedmeshindex);
		if (mesh)
		{
			// for now, we are only interested in updating the UV data from the original to the wicked mesh
			sOffsetMap offsetMap;
			GetFVFOffsetMapFixedForBones(pDBOMesh, &offsetMap);

			//PE: The size can change in phyics debug drawer so:
			if(offsetMap.dwZ > 0 && mesh->vertex_positions.size() != pDBOMesh->dwVertexCount)
				mesh->vertex_positions.resize(pDBOMesh->dwVertexCount);
			if (offsetMap.dwTU[0] > 0 && mesh->vertex_uvset_0.size() != pDBOMesh->dwVertexCount)
				mesh->vertex_uvset_0.resize(pDBOMesh->dwVertexCount);
			if (offsetMap.dwNZ > 0 && mesh->vertex_normals.size() != pDBOMesh->dwVertexCount)
				mesh->vertex_normals.resize(pDBOMesh->dwVertexCount);

			for (size_t v = 0; v < pDBOMesh->dwVertexCount; v++)
			{
				if (offsetMap.dwZ > 0)
				{
					XMFLOAT3 pos = XMFLOAT3(0, 0, 0);
					pos.x = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwX + (offsetMap.dwSize * v));
					pos.y = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwY + (offsetMap.dwSize * v));
					pos.z = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwZ + (offsetMap.dwSize * v));
					mesh->vertex_positions[v] = pos;
				}
				//PE: To remove light from object (particles) we also need to update normals.
				//PE: Used CloneObject instead , leave it here if needed else where.
				//if (offsetMap.dwNZ > 0)
				//{
				//	XMFLOAT3 nor = XMFLOAT3(0, 0, 0);
				//	nor.x = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwNX + (offsetMap.dwSize * v));
				//	nor.y = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwNY + (offsetMap.dwSize * v));
				//	nor.z = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwNZ + (offsetMap.dwSize * v));
				//	mesh->vertex_normals[v] = nor;
				//}
				if (offsetMap.dwTU[0] > 0)
				{
					XMFLOAT2 tex = XMFLOAT2(0, 0);
					tex.x = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwTU[0] + (offsetMap.dwSize * v));
					tex.y = *(float*)((float*)pDBOMesh->pVertexData + offsetMap.dwTV[0] + (offsetMap.dwSize * v));
					mesh->vertex_uvset_0[v] = tex;
				}
			}
			mesh->CreateRenderData();
		}
	}
}

void WickedCall_SetObjectAlpha(sObject* pObject, float fPercentage)
{
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		WickedSetMeshNumber(iM);
		WickedCall_SetMeshAlpha(pObject->ppMeshList[iM], fPercentage);
	}
}

float WickedCall_GetObjectAlpha(sObject* pObject)
{
	float fPercentage = 1.0f;
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		sMesh* pMesh = pObject->ppMeshList[iM];
		if (pMesh)
		{
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
			if (mesh)
			{
				uint64_t materialEntity = mesh->subsets[0].materialID;
				wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
				if (pObjectMaterial)
				{
					fPercentage = pObjectMaterial->GetOpacity() * 100.0f;
					break;
				}
			}
		}
	}
	return fPercentage;
}

void WickedCall_SetObjectTransparentDirect(sObject* pObject, bool bTransparent)
{
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		pObject->ppMeshList[iM]->bTransparency = bTransparent;
		WickedCall_SetMeshTransparent(pObject->ppMeshList[iM]);
	}
}
bool WickedCall_GetObjectTransparentDirect(sObject* pObject)
{
	bool bTransparent = false;
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		bTransparent = pObject->ppMeshList[iM]->bTransparency;
		break;
	}
	return bTransparent;
}

void WickedCall_SetObjectBlendMode(sObject* pObject, int iBlendmode)
{
	if (!pObject) return;

	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		if (pObject->ppMeshList[iM])
		{
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pObject->ppMeshList[iM]->wickedmeshindex);
			if (mesh)
			{
				uint64_t materialEntity = mesh->subsets[0].materialID;
				wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
				if (pObjectMaterial)
				{
					pObjectMaterial->userBlendMode = (BLENDMODE) iBlendmode;
					pObjectMaterial->SetDirty(true);
				}
			}
		}
	}
}


void WickedCall_SetObjectAlphaRef(sObject* pObject, float fAlphaRef)
{
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
		WickedCall_SetMeshAlphaRef(pObject->ppMeshList[iM], fAlphaRef);
}

float WickedCall_GetObjectAlphaRef(sObject* pObject)
{
	float fAlphaRef = 1.0f;
	for (int iM = 0; iM < pObject->iMeshCount; iM++)
	{
		sMesh* pMesh = pObject->ppMeshList[iM];
		if (pMesh)
		{
			// get wicked meshID
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
			if (mesh)
			{
				// get material from mesh
				uint64_t materialEntity = mesh->subsets[0].materialID;
				wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
				if (pObjectMaterial)
				{
					fAlphaRef = pObjectMaterial->alphaRef;// GetAlphaRef();
					break;
				}
			}
		}
	}
	return fAlphaRef;
}

void WickedCall_SetMeshAlphaRef(sMesh* pMesh, float fAlphaRef)
{
	if (pMesh)
	{
		// get wicked meshID
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
		if (mesh)
		{
			// get material from mesh
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
			if (pObjectMaterial)
			{
				pObjectMaterial->SetAlphaRef(fAlphaRef);
			}
		}
	}
}

void WickedCall_SetMeshMaterial ( sMesh* pMesh, bool bForce)
{
	if (pMesh)
	{
		// get wicked meshID
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
		if (mesh)
		{
			// get material from mesh
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
			if (pObjectMaterial)
			{
				// base color
				bool bWickedMaterialActive = IsWickedMaterialActive(pMesh);
				if (!bForce && bWickedMaterialActive)
				{
					DWORD dwBaseColor = WickedGetBaseColor();
					if (dwBaseColor != -1)
					{
						pMesh->mMaterial.Diffuse.r = ((dwBaseColor & 0xff000000) >> 24) / 255.0f;;
						pMesh->mMaterial.Diffuse.g = ((dwBaseColor & 0x00ff0000) >> 16) / 255.0f;
						pMesh->mMaterial.Diffuse.b = ((dwBaseColor & 0x0000ff00) >> 8) / 255.0f;
						pMesh->mMaterial.Diffuse.a = (dwBaseColor & 0x000000ff) / 255.0f;
					}
					pObjectMaterial->SetBaseColor(XMFLOAT4(pMesh->mMaterial.Diffuse.r, pMesh->mMaterial.Diffuse.g, pMesh->mMaterial.Diffuse.b, pMesh->mMaterial.Diffuse.a));
				}
				else
				{
					pObjectMaterial->SetBaseColor(XMFLOAT4(pMesh->mMaterial.Diffuse.r, pMesh->mMaterial.Diffuse.g, pMesh->mMaterial.Diffuse.b, pMesh->mMaterial.Diffuse.a));
				}

				// emissive color
				//PE: Prevent us for setting emissive color to black ? another way is needed.
				//PE: Only do trick if we dont have any custom material settings.
				if (!bWickedMaterialActive && pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource && (pMesh->mMaterial.Emissive.r + pMesh->mMaterial.Emissive.g + pMesh->mMaterial.Emissive.b) == 0)
				{
					// a trick so that old mesh materials set to black can be seen in new wicked engine
					pObjectMaterial->emissiveColor.x = 1.0f;
					pObjectMaterial->emissiveColor.y = 1.0f;
					pObjectMaterial->emissiveColor.z = 1.0f;
				}
				else 
				{
					// otherwise normally have values set in pMesh->mMaterial
					pObjectMaterial->emissiveColor.x = pMesh->mMaterial.Emissive.r;
					pObjectMaterial->emissiveColor.y = pMesh->mMaterial.Emissive.g;
					pObjectMaterial->emissiveColor.z = pMesh->mMaterial.Emissive.b;
					if (!pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource)
					{
						//PE: emissiveColor.w is used for emissive strength , so dont touch if we have a texture :)
						pObjectMaterial->emissiveColor.w = pMesh->mMaterial.Emissive.a;
					}
				}

				// ensure material refs updated for rendering
				pObjectMaterial->SetDirty();
			}
		}
	}
}

void WickedCall_SetObjectRenderOrderBias(sObject* pObject, float fRenderOrderBias)
{
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				// get wicked meshID
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					WickedCall_SetRenderOrderBias(pMesh, fRenderOrderBias);
				}
			}
		}
	}
}

float WickedCall_GetObjectRenderOrderBias(sObject* pObject)
{
	float fRenderOrderBias = 0.0f;
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		sFrame* pFrame = pObject->ppFrameList[i];
		if(pFrame)
		{
			ObjectComponent* object = wiScene::GetScene().objects.GetComponent(pFrame->wickedobjindex);
			if (object)
			{
				fRenderOrderBias = object->GetRenderOrderBiasDistance();
				break;
			}
		}
	}
	return fRenderOrderBias;
}

void WickedCall_SetObjectPlanerReflection(sObject* pObject, bool bPlanerReflection)
{
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				// get wicked meshID
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					// get material from mesh
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						//bool bPlanerReflection = WickedPlanerReflection();
						if (bPlanerReflection)
						{
							pObjectMaterial->shaderType = MaterialComponent::SHADERTYPE_PBR_PLANARREFLECTION;
						}
						else
						{
							if(pObjectMaterial->parallaxOcclusionMapping > 0.0f )
								pObjectMaterial->shaderType = MaterialComponent::SHADERTYPE_PBR_PARALLAXOCCLUSIONMAPPING;
							else
								pObjectMaterial->shaderType = MaterialComponent::SHADERTYPE_PBR;
						}
					}
				}
			}
		}
	}
}

bool WickedCall_GetObjectPlanerReflection(sObject* pObject)
{
	bool bPlanerReflection = false;
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				// get wicked meshID
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					// get material from mesh
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						if (pObjectMaterial->shaderType == MaterialComponent::SHADERTYPE_PBR_PLANARREFLECTION)
						{
							bPlanerReflection = true;
							break;
						}
					}
				}
			}
		}
	}
	return bPlanerReflection;
}

bool bActivateStandaloneOutline = false;

void WickedCall_SetObjectOutline(sObject* pObject, float fHighlight)
{
	extern std::vector<int> g_StandaloneObjectHighlightList;
	if (pObject)
	{
		if (ObjectExist(pObject->dwObjectNumber))
		{
			if (fHighlight > 1.1)
			{
				//PE: Keep a list and keep updating them.
				g_StandaloneObjectHighlightList.push_back(pObject->dwObjectNumber);
				bActivateStandaloneOutline = true;
			}
			else if (fHighlight > 0.1)
			{
				//PE: Fire only one time. effect will be lost on next frame.
				//void WickedCall_DrawObjctBox(sObject * pObject, XMFLOAT4 color, bool bThickLine, bool ForceBox);
				WickedCall_DrawObjctBox(pObject, XMFLOAT4(0.8f, 0.8f, 0.8f, 0.8f), false, false);
				bActivateStandaloneOutline = true;
			}
			else
			{
				for (int i = 0; i < g_StandaloneObjectHighlightList.size(); i++)
				{
					if (g_StandaloneObjectHighlightList[i] == pObject->dwObjectNumber)
					{
						g_StandaloneObjectHighlightList.erase(g_StandaloneObjectHighlightList.begin() + i);
						break;
					}
				}
			}
		}
	}
}

bool WickedCall_GetObjectOutline(sObject* pObject)
{
	extern std::vector<int> g_StandaloneObjectHighlightList;
	if (pObject)
	{
		if (ObjectExist(pObject->dwObjectNumber))
		{
			for (int i = 0; i < g_StandaloneObjectHighlightList.size(); i++)
			{
				if (g_StandaloneObjectHighlightList[i] == pObject->dwObjectNumber)
				{
					g_StandaloneObjectHighlightList.erase(g_StandaloneObjectHighlightList.begin() + i);
					return(true);
				}
			}
		}
	}
	return(false);
}

void WickedCall_SetObjectCastShadows(sObject* pObject, bool bCastShadow)
{
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				// get wicked meshID
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					// get material from mesh
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						if (bCastShadow)
							pObjectMaterial->SetCastShadow(true);
						else
							pObjectMaterial->SetCastShadow(false);
					}
				}
			}
		}
	}
}

bool WickedCall_GetObjectCastShadows(sObject* pObject)
{
	bool bCastShadow = true;
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				// get wicked meshID
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					// get material from mesh
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						bCastShadow = pObjectMaterial->IsCastingShadow();// > IsCastingShadow;// IsCastShadow();
						break;
					}
				}
			}
		}
	}
	return bCastShadow;
}

void WickedCall_SetObjectTextureUV(sObject* pObject, float x, float y, float z, float w)
{
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						pObjectMaterial->texMulAdd.x = x;
						pObjectMaterial->texMulAdd.y = y;
						pObjectMaterial->texMulAdd.z = z;
						pObjectMaterial->texMulAdd.w = w;
						pObjectMaterial->SetDirty(true);
					}
				}
			}
		}
	}
}

void WickedCall_GetObjectTextureUV(sObject* pObject, float* x, float* y, float* z, float* w)
{
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						*x = pObjectMaterial->texMulAdd.x;
						*y = pObjectMaterial->texMulAdd.y;
						*z = pObjectMaterial->texMulAdd.z;
						*w = pObjectMaterial->texMulAdd.w;
					}
				}
			}
		}
	}
}

void WickedCall_SetObjectLightToUnlit(sObject* pObject, int shaderType)
{
	for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
	{
		sMesh* pMesh = pObject->ppMeshList[iMesh];
		if (pMesh)
		{
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
			if (mesh)
			{
				uint64_t materialEntity = mesh->subsets[0].materialID;
				wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
				if (pObjectMaterial)
				{
					pObjectMaterial->SetReflectance(0.0f);
					pObjectMaterial->shaderType = (wiScene::MaterialComponent::SHADERTYPE)shaderType;
					pObjectMaterial->SetDirty(true);
				}
			}
			WickedCall_SetMeshMaterial(pMesh, false);
		}
	}
}

void WickedCall_SetObjectBaseColor(sObject* pObject, int r, int g, int b)
{
	for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
	{
		sMesh* pMesh = pObject->ppMeshList[iMesh];
		if (pMesh)
		{
			pMesh->mMaterial.Diffuse.r = r / 255.0f;
			pMesh->mMaterial.Diffuse.g = g / 255.0f;
			pMesh->mMaterial.Diffuse.b = b / 255.0f;
			WickedCall_SetMeshMaterial(pMesh,true);
		}
	}
}

void WickedCall_GetObjectBaseColor(sObject* pObject, int* r, int* g, int* b)
{
	for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
	{
		sMesh* pMesh = pObject->ppMeshList[iMesh];
		if (pMesh)
		{
			*r = pMesh->mMaterial.Diffuse.r * 255.0f;
			*g = pMesh->mMaterial.Diffuse.g * 255.0f;
			*b = pMesh->mMaterial.Diffuse.b * 255.0f;
			break;
		}
	}
}

void WickedCall_SetObjectEmissiveColor(sObject* pObject, int r, int g, int b)
{
	for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
	{
		sMesh* pMesh = pObject->ppMeshList[iMesh];
		if (pMesh)
		{
			pMesh->mMaterial.Emissive.r = r / 255.0f;
			pMesh->mMaterial.Emissive.g = g / 255.0f;
			pMesh->mMaterial.Emissive.b = b / 255.0f;
			WickedCall_SetMeshMaterial(pMesh, false);
		}
	}
}

void WickedCall_GetObjectEmissiveColor(sObject* pObject, int* r, int* g, int* b)
{
	for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
	{
		sMesh* pMesh = pObject->ppMeshList[iMesh];
		if (pMesh)
		{
			*r = pMesh->mMaterial.Emissive.r * 255.0f;
			*g = pMesh->mMaterial.Emissive.g * 255.0f;
			*b = pMesh->mMaterial.Emissive.b * 255.0f;
			break;
		}
	}
}

void WickedCall_SetObjectNormalness(sObject* pObject, float normalness)
{
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				// get wicked meshID
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					// get material from mesh
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						pObjectMaterial->SetNormalMapStrength(normalness);
					}
				}
			}
		}
	}
}
float WickedCall_GetObjectNormalness(sObject* pObject)
{
	float normalness = 1.0f;
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				// get wicked meshID
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					// get material from mesh
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						normalness = pObjectMaterial->normalMapStrength;// GetNormalMapStrength();
						break;
					}
				}
			}
		}
	}
	return normalness;
}
void WickedCall_SetObjectRoughness(sObject* pObject, float roughness)
{
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						pObjectMaterial->SetRoughness(roughness);
					}
				}
			}
		}
	}
}
float WickedCall_GetObjectRoughness(sObject* pObject)
{
	float roughness = 0.0f;
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						roughness = pObjectMaterial->roughness;// ->GetRoughness();
						break;
					}
				}
			}
		}
	}
	return roughness;
}
void WickedCall_SetObjectMetalness(sObject* pObject, float metalness)
{
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						pObjectMaterial->SetMetalness(metalness);
					}
				}
			}
		}
	}
}
float WickedCall_GetObjectMetalness(sObject* pObject)
{
	float metalness = 0.0f;
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						metalness = pObjectMaterial->metalness;// GetMetalness();
					}
				}
			}
		}
	}
	return metalness;
}

void WickedCall_SetObjectEmissiveStrength(sObject* pObject, float strength)
{
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						pObjectMaterial->SetEmissiveStrength(strength);
					}
				}
			}
		}
	}
}

float WickedCall_GetObjectEmissiveStrength(sObject* pObject)
{
	float strength = 0.0f;
	for (int i = 0; i < pObject->iFrameCount; i++)
	{
		if (pObject->ppFrameList[i]->pMesh)
		{
			sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
			if (pMesh)
			{
				wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
				if (mesh)
				{
					uint64_t materialEntity = mesh->subsets[0].materialID;
					wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					if (pObjectMaterial)
					{
						strength = pObjectMaterial->GetEmissiveStrength();
						break;
					}
				}
			}
		}
	}
	return strength;
}

void WickedCall_TextureObject(sObject* pObject,sMesh* pJustThisMesh)
{
	for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
	{
		sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
		if (pMesh)
		{
			if (pJustThisMesh == NULL || (pJustThisMesh == pMesh)) 
			{
				WickedSetMeshNumber(iMeshIndex);
				WickedCall_TextureMesh(pMesh);
			}
		}
	}
}

void WickedCall_TextureMeshWithImagePtr(sMesh* pMesh, int iPutInEmissivemode)
{
	//PE: This system can only be used for prompt and other FORMAT_R8G8B8A8_UNORM textures.
	//PE: If you try it with anything else it will crash.
	if (pMesh)
	{
		// get wicked meshID
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
		if (mesh)
		{
			// get material from mesh
			uint64_t materialEntity = mesh->subsets[0].materialID;
			wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
			if (pObjectMaterial && pMesh->pTextures)
			{
				// get old DX11 image ptr
				sTexture* pMeshTexture = &pMesh->pTextures[0];
				if (pMeshTexture)
				{
					ID3D11Resource* pTexturePtr = pMeshTexture->pTexturesRef;
					if (pTexturePtr)
					{
						// set default material values
						pObjectMaterial->baseColor = XMFLOAT4(1, 1, 1, 1);
						pObjectMaterial->roughness = 0;
						pObjectMaterial->metalness = 0;
						pObjectMaterial->reflectance = 0.04f;// 0.002f;

						// first delete old textures
						if (pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource)
						{
							pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource = nullptr;
							pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].name = "";
							pObjectMaterial->SetDirty();
							wiJobSystem::context ctx;
							wiJobSystem::Wait(ctx);
						}
						if (pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource)
						{
							pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = nullptr;
							pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = "";
							pObjectMaterial->SetDirty();
							wiJobSystem::context ctx;
							wiJobSystem::Wait(ctx);
						}

						// set texture resource description
						TextureDesc desc;
						desc.BindFlags = BIND_SHADER_RESOURCE;
						desc.CPUAccessFlags = 0;
						desc.Width = ImageWidth(pMeshTexture->iImageID);
						desc.Height = ImageHeight(pMeshTexture->iImageID);
						desc.Depth = 32;
						desc.MipLevels = 1;
						desc.ArraySize = 1;
						desc.MiscFlags = 0;
						desc.Usage = USAGE_IMMUTABLE;
						desc.Format = FORMAT_R8G8B8A8_UNORM;
						desc.type = TextureDesc::TEXTURE_2D;

						// get initdata ready
						std::vector<SubresourceData> InitData;

						// get access to dx11 texture desc
						DWORD bitdepth = 4;
						LPGGSURFACE pTextureInterface = NULL;
						pTexturePtr->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
						D3D11_TEXTURE2D_DESC dx11desc;
						pTextureInterface->GetDesc(&dx11desc);

						// create system memory version
						ID3D11Texture2D* pTempSysMemTexture = NULL;
						D3D11_TEXTURE2D_DESC StagedDesc = { dx11desc.Width, dx11desc.Height, 1, 1, dx11desc.Format, 1, 0, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ, 0 };
						m_pD3D->CreateTexture2D( &StagedDesc, NULL, &pTempSysMemTexture );
						if (pTempSysMemTexture)
						{
							// and copy texture image to it
							D3D11_BOX rc = { 0, 0, 0, (LONG)dx11desc.Width, (LONG)dx11desc.Height, 1 };
							m_pImmediateContext->CopySubresourceRegion(pTempSysMemTexture, 0, 0, 0, 0, pTextureInterface, 0, &rc);

							// lock for reading staging texture
							GGLOCKED_RECT d3dlock;
							if (SUCCEEDED(m_pImmediateContext->Map(pTempSysMemTexture, 0, D3D11_MAP_READ, 0, &d3dlock)))
							{
								// work out size of all data for this texture
								int iSizeOfBitmapData = dx11desc.Width*dx11desc.Height*bitdepth;

								// copy dx11 system staged texture data into initdata
								SubresourceData subresourceData;
								LPSTR pTextureMem = new char[iSizeOfBitmapData];
								memset(pTextureMem, 0, iSizeOfBitmapData);
								subresourceData.pSysMem = pTextureMem;
								subresourceData.SysMemPitch = dx11desc.Width * 4;
								subresourceData.SysMemSlicePitch = iSizeOfBitmapData * 4;
								InitData.push_back(subresourceData);

								// copy from surface to newly created texture mem data
								LPSTR pSrc = (LPSTR)d3dlock.pData;
								LPSTR pPtr = pTextureMem;
								DWORD dwDataWidth = dx11desc.Width*bitdepth;
								for (DWORD y = 0; y < dx11desc.Height; y++)
								{
									//PE: Get a crash here on BC3 textures. , Also got a crash on a BC1 using 12 mip levels ? (source read access)
									memcpy(pPtr, pSrc, dwDataWidth);
									pPtr += dwDataWidth;
									pSrc += d3dlock.RowPitch;
								}
								m_pImmediateContext->Unmap(pTempSysMemTexture, 0);
							}

							// free temp system surface
							SAFE_RELEASE(pTempSysMemTexture);
						}

						// release interface to original texture
						SAFE_RELEASE ( pTextureInterface );

						// give new texture a name (tests if we are freeing these registered resources)

						// PREBEN, it would seem this random name thing proves that the old named and registered images need deleting or we !!MEMORY LEAK!!
						// and also it retrieves the first image with the used name, so new images created with the same name are ignored.
						// needs closer looking at this one (used by CCP, EBE and LUA Prompt to create unique textures)

						char pMassivelyRandomTexName[MAX_PATH];
						sprintf(pMassivelyRandomTexName, "TotallyRandom%d", (int)(rand() % 99999));
						std::string sTextureName = pMassivelyRandomTexName;// "OldImagePtrTexture";
						//WickedCall_DeleteImage(sTextureName);	// leelee, this ensures only ONE copy of this created texture exists, but the engine
																// will likely require many uniquely created, so need more unique names instead of this hack!

						int found = 0;
						std::shared_ptr<wiResource> resource = wiResourceManager::GetResource( sTextureName, 1, &found );

						if ( !found )
						{
							// use desc and initdata to create the texture
							wiRenderer::GetDevice()->CreateTexture(&desc, InitData.data(), &resource->texture);
							wiRenderer::GetDevice()->SetName(&resource->texture, sTextureName.c_str());
						}

						// create a new resource manually
						pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].name = sTextureName;
						pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource = resource;
						//WickedCall_AddImageToList(pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource, IMAGERES_LEVEL, sTextureName);
						if (iPutInEmissivemode == 1)
						{
							// create second emissive texture so it is lit same from any angle, and reduce baseColor influence (except its alpha mask)
							sprintf(pMassivelyRandomTexName, "TotallyRandom%d", (int)(rand() % 99999));
							sTextureName = pMassivelyRandomTexName;// "OldImagePtrTextureEmssive";
							//WickedCall_DeleteImage(sTextureName); // and the same for this, unique names needed for created textures for management purposes

							std::shared_ptr<wiResource> resource2 = wiResourceManager::GetResource( sTextureName, 1, &found );

							if ( !found )
							{
								wiRenderer::GetDevice()->CreateTexture(&desc, InitData.data(), &resource2->texture);
								wiRenderer::GetDevice()->SetName(&resource2->texture, sTextureName.c_str());
							}

							pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].name = sTextureName;
							pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource = resource2;
							//WickedCall_AddImageToList(pObjectMaterial->textures[MaterialComponent::EMISSIVEMAP].resource, IMAGERES_LEVEL, sTextureName);
							pObjectMaterial->SetEmissiveStrength(20.0f);
							pObjectMaterial->baseColor = XMFLOAT4(0, 0, 0, 1);
						}

						// inform engine of texture change here
						pObjectMaterial->SetDirty();
					}
				}
			}
		}
	}
}

void WickedCall_TextureObjectWithImagePtr(sObject* pObject, int iPutInEmissivemode)
{
	for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
	{
		sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
		if (pMesh)
		{
			WickedSetMeshNumber(iMeshIndex);
			WickedCall_TextureMeshWithImagePtr(pMesh,iPutInEmissivemode);
		}
	}
}

void WickedCall_UpdateObject(sObject* pObject)
{
	// called when pos, rot or scale updated
	uint64_t rootEntity = pObject->wickedrootentityindex;
	if ( rootEntity > 0 )
	{
		// set a transform for the object
		wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(rootEntity);
		if (pObject->position.bCustomWorldMatrix == false)
		{
			// start with fres transform
			pTransform->ClearTransform();

			// work out offsets of object before applying world position/rot/etc
			sFrame* pFrame = pObject->pFrame;
			if (pFrame)
			{
				// using first frame orientation to control an object wide adjustment (from importer (or from DBO))
				// rotation
				XMFLOAT3 rotinrads;
				rotinrads.x = GGToRadian(pFrame->vecRotation.x);
				rotinrads.y = GGToRadian(pFrame->vecRotation.y);
				rotinrads.z = GGToRadian(pFrame->vecRotation.z);
				XMMATRIX rot;
				rot = XMMatrixRotationX(rotinrads.x);
				rot = rot * XMMatrixRotationY(rotinrads.y);
				rot = rot * XMMatrixRotationZ(rotinrads.z);
				XMVECTOR S;
				XMVECTOR R;
				XMVECTOR T;
				XMMatrixDecompose(&S, &R, &T, rot);
				pTransform->Rotate(R);

				// work out MAX rotation matrix
				GGMATRIX matRotation;
				GGMATRIX matRotateX, matRotateY, matRotateZ;
				GGMatrixRotationX(&matRotateX, GGToRadian(pFrame->vecRotation.x));	// x rotation
				GGMatrixRotationY(&matRotateY, GGToRadian(pFrame->vecRotation.y));	// y rotation
				GGMatrixRotationZ(&matRotateZ, GGToRadian(pFrame->vecRotation.z));	// z rotation
				matRotation = matRotateX * matRotateY * matRotateZ;

				// translation (using above rotation on the offset)
				GGVECTOR3 vecPosOffset = GGVECTOR3(pFrame->vecOffset.x, pFrame->vecOffset.y, pFrame->vecOffset.z);
				GGVec3TransformCoord(&vecPosOffset, &vecPosOffset, &matRotation);
				XMFLOAT3 applyPosOffset = { 0,0,0 };
				applyPosOffset.x = vecPosOffset.x;
				applyPosOffset.y = vecPosOffset.y;
				applyPosOffset.z = vecPosOffset.z;
				pTransform->Translate(applyPosOffset);
			}
			// apply world
			pTransform->Translate(XMFLOAT3(pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z));
			//pObject->position.vecScale.x = 0.20
			if (pObject->bUseFixedSize)
				pTransform->Scale(XMFLOAT3(pObject->vecFixedSize.x, pObject->vecFixedSize.y, pObject->vecFixedSize.z));
			else
				pTransform->Scale(XMFLOAT3(pObject->position.vecScale.x, pObject->position.vecScale.y, pObject->position.vecScale.z));

			XMFLOAT3 rotationinrads;
			rotationinrads.x = GGToRadian(pObject->position.vecRotate.x);
			rotationinrads.y = GGToRadian(pObject->position.vecRotate.y);
			rotationinrads.z = GGToRadian(pObject->position.vecRotate.z);
			#ifndef MATCHCLASSICROTATION
			pTransform->RotateRollPitchYaw(rotationinrads);
			#else
			//PE: This will match how classic rotation work.
			XMMATRIX rot;
			rot = XMMatrixRotationX(rotationinrads.x);
			rot = rot * XMMatrixRotationY(rotationinrads.y);
			rot = rot * XMMatrixRotationZ(rotationinrads.z);
			XMVECTOR S;
			XMVECTOR R;
			XMVECTOR T;
			XMMatrixDecompose(&S, &R, &T, rot);
			pTransform->Rotate(R);
			#endif
			if (pObject->position.bApplyPivot == true)
			{
				GGMATRIX matPivot = pObject->position.matPivot;
				XMFLOAT4X4 pivotmatrix;
				pivotmatrix._11 = matPivot._11;
				pivotmatrix._12 = matPivot._12;
				pivotmatrix._13 = matPivot._13;
				pivotmatrix._14 = matPivot._14;
				pivotmatrix._21 = matPivot._21;
				pivotmatrix._22 = matPivot._22;
				pivotmatrix._23 = matPivot._23;
				pivotmatrix._24 = matPivot._24;
				pivotmatrix._31 = matPivot._31;
				pivotmatrix._32 = matPivot._32;
				pivotmatrix._33 = matPivot._33;
				pivotmatrix._34 = matPivot._34;
				pivotmatrix._41 = matPivot._41;
				pivotmatrix._42 = matPivot._42;
				pivotmatrix._43 = matPivot._43;
				pivotmatrix._44 = matPivot._44;
				XMVECTOR S, R, T;
				XMMatrixDecompose(&S, &R, &T, XMLoadFloat4x4(&pivotmatrix));
				XMFLOAT4 rotation_local;
				XMStoreFloat4(&rotation_local, R);
				pTransform->Rotate(rotation_local);
			}
		}
		else
		{
			GGMATRIX worldmatrix = pObject->position.matWorld;
			pTransform->world._11 = worldmatrix._11;
			pTransform->world._12 = worldmatrix._12;
			pTransform->world._13 = worldmatrix._13;
			pTransform->world._14 = worldmatrix._14;
			pTransform->world._21 = worldmatrix._21;
			pTransform->world._22 = worldmatrix._22;
			pTransform->world._23 = worldmatrix._23;
			pTransform->world._24 = worldmatrix._24;
			pTransform->world._31 = worldmatrix._31;
			pTransform->world._32 = worldmatrix._32;
			pTransform->world._33 = worldmatrix._33;
			pTransform->world._34 = worldmatrix._34;
			pTransform->world._41 = worldmatrix._41;
			pTransform->world._42 = worldmatrix._42;
			pTransform->world._43 = worldmatrix._43;
			pTransform->world._44 = worldmatrix._44;
			pTransform->ApplyTransform();
		}
		pTransform->UpdateTransform();
		pTransform->SetDirty();
	}
}

void WickedCall_UpdateLimbsOfObject(sObject* pObject)
{
	// called when we 'know' that we have moved limbs/frame within the object and need wicked to reflect this
	for (int iF = 0; iF < pObject->iFrameCount; iF++)
	{
		sFrame* pFrame = pObject->ppFrameList[iF];
		if (pFrame)
		{
			Entity framewickedentity = pFrame->wickedobjindex;
			TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(framewickedentity);
			if (pTransform)
			{
				pTransform->ClearTransform();
				pTransform->Translate(XMFLOAT3(pFrame->vecOffset.x, pFrame->vecOffset.y, pFrame->vecOffset.z));
				pTransform->Scale(XMFLOAT3(pFrame->vecScale.x, pFrame->vecScale.y, pFrame->vecScale.z));
				XMFLOAT3 rotationinrads;
				rotationinrads.x = GGToRadian(pFrame->vecRotation.x);
				rotationinrads.y = GGToRadian(pFrame->vecRotation.y);
				rotationinrads.z = GGToRadian(pFrame->vecRotation.z);
				pTransform->RotateRollPitchYaw(rotationinrads);
				pTransform->UpdateTransform();
				pTransform->SetDirty();
			}
		}
	}
}

void WickedCall_UpdateSceneForPick(void)
{
	// when position/rot/etc an object, and then need to instantly
	// cast a ray to pick, need to update scene

	//wiScene::GetScene().Update(0); //PE: Very expensive, only update transforms.

	//PE: Only transform update needed.
	wiScene::GetScene().UpdateSceneTransform(0);
}

void WickedCall_SetRenderOrderBias(sMesh* pMesh, float fDistanceToAdd)
{
	sFrame* pFrame = pMesh->pFrameAttachedTo;
	if (pFrame)
	{
		ObjectComponent* object = wiScene::GetScene().objects.GetComponent(pFrame->wickedobjindex);
		if (object)
		{
			object->SetRenderOrderBiasDistance(fDistanceToAdd);
		}
	}
}

float WickedCall_GetRenderOrderBias(sMesh* pMesh)
{
	sFrame* pFrame = pMesh->pFrameAttachedTo;
	if (pFrame)
	{
		ObjectComponent* object = wiScene::GetScene().objects.GetComponent(pFrame->wickedobjindex);
		if (object)
		{
			return object->GetRenderOrderBiasDistance();
		}
	}
	return 0; // compiler complains if this is missing, what's a suitable default value?
}

void WickedCall_SetLimbVisible(sFrame* pFrame, bool bVisible)
{
	if (pFrame)
	{
		ObjectComponent* object = wiScene::GetScene().objects.GetComponent(pFrame->wickedobjindex);
		if (object)
		{
			object->SetRenderable(bVisible);
		}
		//PE: Hide the mesh , or it will have no effect.
		sMesh* pMesh = pFrame->pMesh;
		if (pMesh)
		{
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
			if (mesh)
			{
				mesh->SetRenderable(bVisible);
			}
		}
	}
}

void WickedCall_SetObjectPreventAnyApparentOcclusion (sObject* pObject, bool bPreventAnyApparentOcclusion)
{
	for (int iF = 0; iF < pObject->iFrameCount; iF++)
	{
		sFrame* pFrame = pObject->ppFrameList[iF];
		if(pFrame)
		{
			if (pFrame->wickedobjindex > 0)
			{
				ObjectComponent* object = wiScene::GetScene().objects.GetComponent(pFrame->wickedobjindex);
				if (object)
				{
					object->SetRenderPreventAnyKindOfCulling(bPreventAnyApparentOcclusion);
				}
			}
		}
	}
}

void WickedCall_SetObjectVisible ( sObject* pObject, bool bVisible )
{
	for (int iF = 0; iF < pObject->iFrameCount; iF++)
	{
		bool bVisForThisFrame = bVisible;
		sFrame* pFrame = pObject->ppFrameList[iF];
		if (pFrame->pMesh && pFrame->pMesh->bVisible == false ) bVisForThisFrame = false; // if limb was hiddem keep hidden until otherwise
		WickedCall_SetLimbVisible(pFrame, bVisForThisFrame);
	}
}

void WickedCall_GlueObjectToObject(sObject* pObjectToGlue, sObject* pParentObject, int iLimb, int iObjIDToSyncAnimTo, int iWorldToLocal)
{
	// attaches this entity object to a parent entity object
	if ( pObjectToGlue && pParentObject )
	{
		uint64_t rootToGlueEntity = pObjectToGlue->wickedrootentityindex;
		if (rootToGlueEntity > 0)
		{
			if (iLimb < pParentObject->iFrameCount)
			{
				sFrame* pFrame = pParentObject->ppFrameList[iLimb];
				uint64_t objectParentToAttachTo = pFrame->wickedobjindex;
				if (rootToGlueEntity > 0)
				{
					// attach child to parent
					bool bAlreadyInChildPosition = false; // i.e. offset to this parent, not world position
					if (iWorldToLocal == 1) bAlreadyInChildPosition = true;
					wiScene::GetScene().Component_Attach(rootToGlueEntity, objectParentToAttachTo, bAlreadyInChildPosition);

					// additionally assign child an ability to perfectly sync anim with parent
					if (pObjectToGlue->pAnimationSet)
					{
						// first animset only for frame return
						sAnimationSet* pAnimSet = pObjectToGlue->pAnimationSet;
						if (pAnimSet)
						{
							Entity animentity = pAnimSet->wickedanimentityindex;
							AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
							if (animationcomponent)
							{
								if (iObjIDToSyncAnimTo != -1)
								{
									// find the animation component of the parent to assign to this child animation 
									sObject* GetObjectData(int iID);
									sObject* pParentObjWithAnim = GetObjectData(iObjIDToSyncAnimTo);
									if (pParentObjWithAnim->pAnimationSet)
									{
										// first animset only for frame return
										sAnimationSet* pParentAnimSet = pParentObjWithAnim->pAnimationSet;
										if (pParentAnimSet)
										{
											Entity parentanimentity = pParentAnimSet->wickedanimentityindex;
											AnimationComponent* parentanimationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
											if (parentanimationcomponent)
											{
												parentanimationcomponent->UsePrimaryAnimTimer(0, parentanimentity);
												animationcomponent->UsePrimaryAnimTimer(parentanimentity, 0);
											}
										}
									}
								}
								else
								{
									// unsync child from parent (probably not used in favor of unglue/glue operation as one presently used)
									animationcomponent->UsePrimaryAnimTimer(0,0);
								}
							}
						}
					}
				}
			}
		}
	}
}

void WickedCall_UnGlueObjectToObject(sObject* pObjectToUnGlue)
{
	// deattaches entity object from any parent
	if ( pObjectToUnGlue )
	{
		uint64_t rootToUnGlueEntity = pObjectToUnGlue->wickedrootentityindex;
		if (rootToUnGlueEntity > 0)
		{
			wiScene::GetScene().Component_Detach(rootToUnGlueEntity);
			if (pObjectToUnGlue->pAnimationSet)
			{
				sAnimationSet* pAnimSet = pObjectToUnGlue->pAnimationSet;
				if (pAnimSet)
				{
					Entity animentity = pAnimSet->wickedanimentityindex;
					AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(animentity);
					if (animationcomponent)
					{
						animationcomponent->UsePrimaryAnimTimer(0,0);
					}
				}
			}
		}
	}
}

void WickedCall_PresetObjectRenderLayer(int iLayerMask)
{
	g_iWickedLayerMaskPreference = iLayerMask;
	g_iWickedLayerMaskOptionalLimb = -1;
}

void WickedCall_PresetObjectLimbRenderLayer(int iLayerMask, int iLimb)
{
	g_iWickedLayerMaskPreference = iLayerMask;
	g_iWickedLayerMaskOptionalLimb = iLimb;
}

void WickedCall_PresetObjectUVScale(float fUScale, float fVScale)
{
	g_iWickedUScalePreference = fUScale;
	g_iWickedVScalePreference = fVScale;
}

void WickedCall_PresetObjectCreateOnDemand(bool bCreateOnlyWhenUsed)
{
	// for some objects, like explosion planes, we have avoided creating
	// a wicked object for it to save on engine management, and will accept
	// the small hit of the objects creation until we entirely replace
	// all decal/particle systems with a GPU particle system
	g_bWickedCreateOnlyWhenUsed = bCreateOnlyWhenUsed;
}

void WickedCall_PresetObjectIgnoreTextures(bool bIgnoreTextureInfo)
{
	// used when we know DBOs have textures but we do not want to try and load any
	// such as the widget DBO objects which carry some rogue old material names
	g_bWickedIgnoreTextureInfo = bIgnoreTextureInfo;
}

void WickedCall_PresetObjectTextureFromImagePtr(bool bUseImagePtrInsteadOfTexFile, int iPutInEmissiveMode)
{
	// use this flag when you want to force a TextureObject function to use the ImagePtr to source the texture
	// rather than the texture filename stored in the texture structure of the mesh (the default behavior)
	g_bWickedUseImagePtrInsteadOfTexFile = bUseImagePtrInsteadOfTexFile;
	g_iWickedPutInEmissiveMode = iPutInEmissiveMode;
}

void WickedCall_PresetObjectPutInEmissive(int iPutInEmissiveMode)
{
	g_iWickedPutInEmissiveMode = iPutInEmissiveMode;
}

void WickedCall_RotateLimb(sObject* pObject, sFrame* pFrame, float fAX, float fAY, float fAZ)
{
	if (pFrame->pAnimRef)
	{
		if (pObject->pAnimationSet)
		{
			uint64_t wickedanimationindex = pObject->pAnimationSet->wickedanimentityindex;
			AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(wickedanimationindex);
			if (animationcomponent)
			{
				int iIndex = pFrame->pAnimRef->wickedanimationchannel[1];
				if (iIndex >= 0)//&& iIndex < animationcomponent->channels.size())
				{
					AnimationComponent::AnimationChannel* pAnimationChannel = &animationcomponent->channels[iIndex];
					if (pAnimationChannel)
					{
						pAnimationChannel->iUsePreFrame = 1;
						pAnimationChannel->qPreFrameRotation = XMQuaternionRotationRollPitchYaw(GGToRadian(fAX), GGToRadian(fAY), GGToRadian(fAZ));
					}
				}
			}
		}
	}
}

void WickedCall_CalculateWorldQuat(GGVECTOR4* pQuatA, GGVECTOR4* pQuatB, GGVECTOR4* pQuatResult)
{
	XMVECTOR qQuatA = XMLoadFloat4(&XMFLOAT4(pQuatA->x, pQuatA->y, pQuatA->z, pQuatA->w));
	XMVECTOR qQuatB = XMLoadFloat4(&XMFLOAT4(pQuatB->x, pQuatB->y, pQuatB->z, pQuatB->w));
	XMVECTOR qResult = XMQuaternionMultiply(qQuatA, qQuatB);
	pQuatResult->x = qResult.m128_f32[0];
	pQuatResult->y = qResult.m128_f32[1];
	pQuatResult->z = qResult.m128_f32[2];
	pQuatResult->w = qResult.m128_f32[3];
}

void WickedCall_CalculateQuatDiff(GGVECTOR4* pQuatA, GGVECTOR4* pQuatB, GGVECTOR4* pQuatDiff)
{
	XMVECTOR qQuatA = XMLoadFloat4(&XMFLOAT4(pQuatA->x, pQuatA->y, pQuatA->z, pQuatA->w));
	XMVECTOR qQuatB = XMLoadFloat4(&XMFLOAT4(pQuatB->x, pQuatB->y, pQuatB->z, pQuatB->w));
	qQuatA = XMQuaternionInverse(qQuatA);
	XMVECTOR qRotationDiff = XMQuaternionMultiply(qQuatA, qQuatB);
	pQuatDiff->x = qRotationDiff.m128_f32[0];
	pQuatDiff->y = qRotationDiff.m128_f32[1];
	pQuatDiff->z = qRotationDiff.m128_f32[2];
	pQuatDiff->w = qRotationDiff.m128_f32[3];
}

void WickedCall_CalculateQuatFromCombined(sObject* pObject, sFrame* pFrame, GGVECTOR4* pQuat)
{
	GGMATRIX matCombined = pFrame->matCombined;
	XMFLOAT4X4 matLimbRot;
	matLimbRot._11 = matCombined._11;
	matLimbRot._12 = matCombined._12;
	matLimbRot._13 = matCombined._13;
	matLimbRot._14 = matCombined._14;
	matLimbRot._21 = matCombined._21;
	matLimbRot._22 = matCombined._22;
	matLimbRot._23 = matCombined._23;
	matLimbRot._24 = matCombined._24;
	matLimbRot._31 = matCombined._31;
	matLimbRot._32 = matCombined._32;
	matLimbRot._33 = matCombined._33;
	matLimbRot._34 = matCombined._34;
	matLimbRot._41 = 0;
	matLimbRot._42 = 0;
	matLimbRot._43 = 0;
	matLimbRot._44 = 0;
	XMMATRIX finalMat = XMLoadFloat4x4(&matLimbRot);
	XMVECTOR qRotation = XMQuaternionRotationMatrix(finalMat);
	pQuat->x = qRotation.m128_f32[0];
	pQuat->y = qRotation.m128_f32[1];
	pQuat->z = qRotation.m128_f32[2];
	pQuat->w = qRotation.m128_f32[3];
}

void WickedCall_OverrideLimbWithCombined(sObject* pObject, sFrame* pFrame, bool bIncludeTranslation)
{
	if (pFrame->pAnimRef)
	{
		if (pObject->pAnimationSet)
		{
			uint64_t wickedanimationindex = pObject->pAnimationSet->wickedanimentityindex;
			AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(wickedanimationindex);
			if (animationcomponent)
			{
				int iIndexPos = pFrame->pAnimRef->wickedanimationchannel[0];
				int iIndexRot = pFrame->pAnimRef->wickedanimationchannel[1];
				if (iIndexPos >= 0)//&& iIndexPos < animationcomponent->channels.size() && iIndexRot >= 0 && iIndexRot < animationcomponent->channels.size())
				{
					AnimationComponent::AnimationChannel* pAnimationChannelPos = &animationcomponent->channels[iIndexPos];
					AnimationComponent::AnimationChannel* pAnimationChannelRot = &animationcomponent->channels[iIndexRot];
					if (pAnimationChannelPos && pAnimationChannelRot)
					{
						// translation
						pAnimationChannelPos->iUsePreFrame = 2;
						pAnimationChannelPos->vPreFrameTranslation = XMVectorSet(pFrame->matCombined._41, pFrame->matCombined._42, pFrame->matCombined._43, 0);

						// rotation
						GGMATRIX matCombined = pFrame->matCombined;
						XMFLOAT4X4 matLimbRot;
						matLimbRot._11 = matCombined._11;
						matLimbRot._12 = matCombined._12;
						matLimbRot._13 = matCombined._13;
						matLimbRot._14 = matCombined._14;
						matLimbRot._21 = matCombined._21;
						matLimbRot._22 = matCombined._22;
						matLimbRot._23 = matCombined._23;
						matLimbRot._24 = matCombined._24;
						matLimbRot._31 = matCombined._31;
						matLimbRot._32 = matCombined._32;
						matLimbRot._33 = matCombined._33;
						matLimbRot._34 = matCombined._34;
						matLimbRot._41 = 0;
						matLimbRot._42 = 0;
						matLimbRot._43 = 0;
						matLimbRot._44 = 0;
						XMMATRIX finalMat = XMLoadFloat4x4(&matLimbRot);
						XMVECTOR currentRot = XMQuaternionRotationMatrix(finalMat);
						pAnimationChannelRot->iUsePreFrame = 2;
						pAnimationChannelRot->qPreFrameRotation = currentRot;
					}
				}
			}
		}
	}
}

void WickedCall_OverrideLimbOff(sObject* pObject, sFrame* pFrame)
{
	if (pFrame->pAnimRef)
	{
		if (pObject->pAnimationSet)
		{
			uint64_t wickedanimationindex = pObject->pAnimationSet->wickedanimentityindex;
			AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(wickedanimationindex);
			if (animationcomponent)
			{
				int iIndexPos = pFrame->pAnimRef->wickedanimationchannel[0];
				int iIndexRot = pFrame->pAnimRef->wickedanimationchannel[1];
				if (iIndexPos >= 0)//&& iIndexPos < animationcomponent->channels.size() && iIndexRot >= 0 && iIndexRot < animationcomponent->channels.size())
				{
					AnimationComponent::AnimationChannel* pAnimationChannelPos = &animationcomponent->channels[iIndexPos];
					AnimationComponent::AnimationChannel* pAnimationChannelRot = &animationcomponent->channels[iIndexRot];
					if (pAnimationChannelPos) pAnimationChannelPos->iUsePreFrame = 0;
					if (pAnimationChannelRot) pAnimationChannelRot->iUsePreFrame = 0;
				}
			}
		}
	}
}

void WickedCall_SetBip01Position(sObject* pObject, sFrame* pFrame, int iUseMode, float fX, float fZ)
{
	if (pFrame->pAnimRef)
	{
		if (pObject->pAnimationSet)
		{
			uint64_t wickedanimationindex = pObject->pAnimationSet->wickedanimentityindex;
			AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(wickedanimationindex);
			if (animationcomponent)
			{
				int iIndexPos = pFrame->pAnimRef->wickedanimationchannel[0];
				if (iIndexPos >= 0)//&& iIndexPos < animationcomponent->channels.size() )
				{
					AnimationComponent::AnimationChannel* pAnimationChannel = &animationcomponent->channels[iIndexPos];
					if (pAnimationChannel)
					{
						if (iUseMode == 3)
						{
							pAnimationChannel->iUsePreFrame = 3;
							pAnimationChannel->vPreFrameTranslation = XMVectorSet(0, 0, 0, 0);
						}
						else
						{
							pAnimationChannel->iUsePreFrame = 0;
						}
					}
				}
			}
		}
	}
}

void WickedCall_SetBip01Rotation(sObject* pObject, sFrame* pFrame, int iUseMode, float fY)
{
	if (pFrame->pAnimRef)
	{
		if (pObject->pAnimationSet)
		{
			uint64_t wickedanimationindex = pObject->pAnimationSet->wickedanimentityindex;
			AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(wickedanimationindex);
			if (animationcomponent)
			{
				int iIndex = pFrame->pAnimRef->wickedanimationchannel[1];
				if (iIndex >= 0)//&& iIndex < animationcomponent->channels.size())
				{
					AnimationComponent::AnimationChannel* pAnimationChannel = &animationcomponent->channels[iIndex];
					if (pAnimationChannel)
					{
						if (iUseMode > 0)
						{
							pAnimationChannel->iUsePreFrame = iUseMode;
							pAnimationChannel->qPreFrameRotation = XMQuaternionRotationRollPitchYaw(0, 0, 0);
						}
						else
						{
							pAnimationChannel->iUsePreFrame = 0;
						}
					}
				}
			}
		}
	}
}

std::vector<sFrame*> g_pFramesToAffect;
void AddToFramesToAffect(sFrame* pFrame)
{
	if (pFrame)
	{
		g_pFramesToAffect.push_back(pFrame);
		AddToFramesToAffect(pFrame->pChild);
		AddToFramesToAffect(pFrame->pSibling);
	}
}

void WickedCall_SetObjectPreFrames(sObject* pObject, LPSTR pParentFrameName, float fFrameToUse, float fSmoothSlerpToNextShape, int iPreFrameMode)
{
	// seems a better way is to 'reuse' the animation system and specify the frame time there instead
	// of here as do not always get the exact frame due to different ways to calculate the final frame
	// so perhaps change the SRT vectors stored with keyframe and add more to wicked animation system
	fFrameToUse += 1.0f;

	// scan all frames in object, but only affect those after the parent specified (i.e. all frames after the head frame)
	sFrame* pTargetParentFrame = NULL;
	for (int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++)
	{
		sFrame* pFrame = pObject->ppFrameList[iFrame];
		if (pFrame)
		{
			if (stricmp(pFrame->szName, pParentFrameName) == NULL)
			{
				pTargetParentFrame = pFrame; break;
			}
		}
	}
	if (pTargetParentFrame)
	{
		// collect all frames affected
		g_pFramesToAffect.clear();
		AddToFramesToAffect(pTargetParentFrame->pChild);

		// get animation component
		uint64_t wickedanimationindex = pObject->pAnimationSet->wickedanimentityindex;
		AnimationComponent* animationcomponent = wiScene::GetScene().animations.GetComponent(wickedanimationindex);
		if (animationcomponent)
		{
			// for each frame, copy the specified animation frame to the preframe
			for (int iF = 0; iF < g_pFramesToAffect.size(); iF++)
			{
				sFrame* pFrame = g_pFramesToAffect[iF];
				sAnimation* pAnim = pFrame->pAnimRef;
				if (pAnim)
				{
					// must loop through three channels and samplers for this frame (T then R then S)
					for (int i = 0; i < 3; i++)
					{
						// LB: new animation system in wicked wipes out "backwards_compatibility_data" so need to use
						// the new location for "keyframe_times, etc"
						// prevent crash to ensure pAnim->wickedanimationchannel[i] not -1
						if (pAnim->wickedanimationchannel[i] >= 0 && pAnim->wickedanimationsampler[i] >= 0)
						{
							// for this animation, locate the channels and samplers for the frame
							AnimationComponent::AnimationChannel* pAnimationChannel = &animationcomponent->channels[pAnim->wickedanimationchannel[i]];
							AnimationComponent::AnimationSampler* pAnimationSampler = &animationcomponent->samplers[pAnim->wickedanimationsampler[i]];
							if (pAnimationChannel && pAnimationSampler)
							{
								AnimationDataComponent* animationdata = wiScene::GetScene().animation_datas.GetComponent(pAnimationSampler->data);
								if (animationdata == nullptr)
									continue;

								// find true keyframe slot using frame time passed in (using fFrameToUse)
								int keyLeft = 0;
								int keyRight = 0;
								if (animationdata->keyframe_times.back() < fFrameToUse)
								{
									// Rightmost keyframe is already outside animation, so just snap to last keyframe:
									keyLeft = keyRight = (int)animationdata->keyframe_times.size() - 1;
								}
								else
								{
									// Search for the right keyframe (greater/equal to anim time):
									while (animationdata->keyframe_times[keyRight++] < fFrameToUse) {}
									keyRight--;
									// Left keyframe is just near right:
									keyLeft = std::max(0, keyRight - 1);
								}

								// copy the animation data as the preframe so it will be imposed on the animation 
								pAnimationChannel->fSmoothAmount = 1.0f / fSmoothSlerpToNextShape;
								if (i == 0)
								{
									pAnimationChannel->iUsePreFrame = iPreFrameMode;
									pAnimationChannel->vPreFrameTranslation = XMLoadFloat3(&((const XMFLOAT3*)animationdata->keyframe_data.data())[keyLeft]);
								}
								if (i == 1)
								{
									pAnimationChannel->iUsePreFrame = iPreFrameMode;
									pAnimationChannel->qPreFrameRotation = XMLoadFloat4(&((const XMFLOAT4*)animationdata->keyframe_data.data())[keyLeft]);
								}
								if (i == 2)
								{
									pAnimationChannel->iUsePreFrame = iPreFrameMode;
									pAnimationChannel->vPreFrameScale = XMLoadFloat3(&((const XMFLOAT3*)animationdata->keyframe_data.data())[keyLeft]);
								}
							}
						}
					}
				}
			}
		}
	}
}

void WickedCall_SetObjectRenderLayer(sObject* pObject,int iLayerMask)
{
	// only for objects that are NOT glued
	//if (pObject->position.bGlued == false && pObject->position.iBeenGluedToBy == 0)
	if ( pObject->position.iBeenGluedToBy == 0)
	{
		// this does not work, cannot seem to set the layermask AFTER you have created the object and merged it with the scene!
		for (int iF = 0; iF < pObject->iFrameCount; iF++)
		{
			uint64_t objectEntity = pObject->ppFrameList[iF]->wickedobjindex;
			if (objectEntity > 0)
			{
				wiScene::LayerComponent* pWickedLayer = wiScene::GetScene().layers.GetComponent(objectEntity);
				pWickedLayer->layerMask = iLayerMask;

				//PE: layermask is taken from the parent in the hierarchy
				HierarchyComponent* parent = wiScene::GetScene().hierarchy.GetComponent(objectEntity);
				if (parent != nullptr)
				{
					parent->layerMask_bind = iLayerMask;
				}
			}
		}
	}
}


bool Convert2Dto3D(long x , long y, float* pOutX, float* pOutY, float* pOutZ, float* pDirX, float* pDirY, float* pDirZ)
{
	//PE: Wicked Mouse is relative to windows pos. ImGui is relative to screen.
	RAY pickRay = wiRenderer::GetPickRay((long)x, (long)y, master.masterrenderer);
	*pOutX = pickRay.origin.x;
	*pOutY = pickRay.origin.y;
	*pOutZ = pickRay.origin.z;
	*pDirX = pickRay.direction.x;
	*pDirY = pickRay.direction.y;
	*pDirZ = pickRay.direction.z;
	return true;
}

float fLastTerrainHitX = 0, fLastTerrainHitY = 0, fLastTerrainHitZ = 0;

bool WickedCall_GetPick2(float fMouseX, float fMouseY, float* pOutX, float* pOutY, float* pOutZ, float* pNormX, float* pNormY, float* pNormZ, uint64_t* pHitEntity, int iLayerMask)
{
	// Use wicked mouse pointer to determine intersection with solid geometry (for terrain and entity detection)
	// PE: We got hits from hidden objects like "widgets" , now ignore hidden objects in WICKEDREPO.
	bool bHitSuccess = false;

	//PE: Do not do anything if mouse is not over level editor.
	if (bImGuiGotFocus) return bHitSuccess;

	// do not wipe out hover object detection when widget plane pass happens
	if (iLayerMask & GGRENDERLAYERS_NORMAL)
	{
		g_hovered_pobject = NULL;
		g_hovered_entity = 0;
	}

	//PE: Wicked Mouse is relative to windows pos. ImGui is relative to screen.
	RAY pickRay = wiRenderer::GetPickRay((long)fMouseX, (long)fMouseY, master.masterrenderer);

	// check scene first if flagged, then terrain which is naturally underneath
	if ((iLayerMask & GGRENDERLAYERS_NORMAL) != 0 || (iLayerMask & GGRENDERLAYERS_CURSOROBJECT) != 0 || (iLayerMask & GGRENDERLAYERS_WIDGETPLANE) != 0)
	{
		wiScene::PickResult hovered = wiScene::Pick(pickRay, RENDERTYPE_ALL, iLayerMask);
		if (hovered.entity > 0)
		{
			if ((iLayerMask & GGRENDERLAYERS_NORMAL) != 0)
			{
				sObject* pHitObject = m_ObjectManager.FindObjectFromWickedObjectEntityID(hovered.entity);
				if (pHitObject)
				{
					bool ObjectIsEntity(void* pTestObject);

					//PE: Only highlight if this is a gg entity.
					bool bIsEntity = ObjectIsEntity(pHitObject);
					if (bIsEntity)
					{
						if (iLayerMask & GGRENDERLAYERS_NORMAL)
						{
							g_hovered_pobject = pHitObject;
							g_hovered_entity = hovered.entity;
							g_hovered_dot_pobject = NULL;
							g_hovered_dot_entity = 0;
						}
					}
					else
					{
						//#define DOTARCSOBJECTID 70001+40000+21001
						if (pHitObject->dwObjectNumber > 110000 && pHitObject->dwObjectNumber < 131002)
						{
							if (iLayerMask & GGRENDERLAYERS_NORMAL)
							{
								g_hovered_dot_pobject = pHitObject;
								g_hovered_dot_entity = hovered.entity;
								g_bhovered_dot = true;
							}
						}
					}
				}
				else
				{
					if (iLayerMask & GGRENDERLAYERS_NORMAL)
					{
						g_hovered_dot_pobject = NULL;
						g_hovered_dot_entity = 0;
					}
				}
			}

			// return hit position
			*pOutX = hovered.position.x;
			*pOutY = hovered.position.y;
			*pOutZ = hovered.position.z;

			if (iLayerMask & GGRENDERLAYERS_NORMAL)
			{
				fLastHitPosition[0] = hovered.position.x;
				fLastHitPosition[1] = hovered.position.y;
				fLastHitPosition[2] = hovered.position.z;
			}

			// if normals needed
			if (pNormX)
			{
				*pNormX = hovered.normal.x;
				*pNormY = hovered.normal.y;
				*pNormZ = hovered.normal.z;
			}

			// optionally return actual object the cursor hovered over
			if (pHitEntity) *pHitEntity = hovered.entity;

			// report a hit
			bHitSuccess = true;
		}
	}
	if ((iLayerMask & GGRENDERLAYERS_TERRAIN) != 0)
	{
		float fDistToObjectHit = -1;
		if (bHitSuccess == true)
		{
			float fDX = *pOutX - CameraPositionX(0);
			float fDY = *pOutY - CameraPositionY(0);
			float fDZ = *pOutZ - CameraPositionZ(0);
			fDistToObjectHit = sqrt(fabs(fDX*fDX) + fabs(fDY*fDY) + fabs(fDZ*fDZ));
		}
		float pTerrOutX, pTerrOutY, pTerrOutZ, pTerrNormX, pTerrNormY, pTerrNormZ;
		if (GGTerrain::GGTerrain_RayCast(pickRay, &pTerrOutX, &pTerrOutY, &pTerrOutZ, &pTerrNormX, &pTerrNormY, &pTerrNormZ, 0))
		{
			fLastTerrainHitX = pTerrOutX, fLastTerrainHitY = pTerrOutY, fLastTerrainHitZ = pTerrOutZ;

			float fDX = pTerrOutX - CameraPositionX(0);
			float fDY = pTerrOutY - CameraPositionY(0);
			float fDZ = pTerrOutZ - CameraPositionZ(0);
			float fDist = sqrt(fabs(fDX*fDX) + fabs(fDY*fDY) + fabs(fDZ*fDZ));
			if (fDist < fDistToObjectHit || fDistToObjectHit == -1)
			{
				// if terrain closer than object hit, we register a terrain detection instead
				if (pHitEntity) *pHitEntity = 0;
				*pOutX = pTerrOutX;
				*pOutY = pTerrOutY;
				*pOutZ = pTerrOutZ;
				if (pNormX)
				{
					*pNormX = pTerrNormX;
					*pNormY = pTerrNormY;
					*pNormZ = pTerrNormZ;
				}
				bHitSuccess = true;
			}
		}
		else
		{
			fLastTerrainHitX = 0, fLastTerrainHitY = 0, fLastTerrainHitZ = 0;
		}
	}

	// no entity hovering over, but still want to move the cursor line in visual logic system
	if (g_hovered_entity == 0 )
	{
		if (iLayerMask & GGRENDERLAYERS_NORMAL)
		{
			fLastHitPosition[0] = *pOutX;
			fLastHitPosition[1] = *pOutY;
			fLastHitPosition[2] = *pOutZ;
		}
	}

	// return success flag
	return bHitSuccess;
}

bool WickedCall_GetPick(float* pOutX, float* pOutY, float* pOutZ, float* pNormX, float* pNormY, float* pNormZ, uint64_t* pHitEntity, int iLayerMask)
{
	XMFLOAT4 currentMouse = wiInput::GetPointer();
	return WickedCall_GetPick2(currentMouse.x, currentMouse.y, pOutX, pOutY, pOutZ, pNormX, pNormY, pNormZ, pHitEntity, iLayerMask);
}

bool WickedCall_SentRay(float originx, float originy, float originz, float directionx, float directiony, float directionz,float* pOutX, float* pOutY, float* pOutZ, float* pNormX, float* pNormY, float* pNormZ, uint64_t* pHitEntity, int iLayerMask)
{
	//PE: Sent ray using wicked.
	XMFLOAT4 currentMouse = wiInput::GetPointer();
	RAY pickRay;
	XMFLOAT3 direction_inverse;
	pickRay.origin.x = originx;
	pickRay.origin.y = originy;
	pickRay.origin.z = originz;
	pickRay.direction.x = directionx;
	pickRay.direction.y = directiony;
	pickRay.direction.z = directionz;
	XMStoreFloat3(&direction_inverse, XMVectorDivide(XMVectorReplicate(1.0f), XMVectorSet(directionx, directiony, directionz,1.0f)));
	pickRay.direction_inverse = direction_inverse;
	wiScene::PickResult hovered = wiScene::Pick(pickRay, RENDERTYPE_ALL, iLayerMask);

	if (hovered.entity > 0)
	{
		sObject* pHitObject = m_ObjectManager.FindObjectFromWickedObjectEntityID(hovered.entity);

		if (pHitObject)
		{
			bool ObjectIsEntity(void* pTestObject);

			//PE: Only highlight if this is a gg entity.
			bool bIsEntity = ObjectIsEntity(pHitObject);
			if (bIsEntity) {
				g_ray_pobject = pHitObject;
			}
		}
		else {
			g_ray_pobject = NULL;
		}

		// return hit position
		*pOutX = hovered.position.x;
		*pOutY = hovered.position.y;
		*pOutZ = hovered.position.z;

		fLastHitPosition[0] = hovered.position.x;
		fLastHitPosition[1] = hovered.position.y;
		fLastHitPosition[2] = hovered.position.z;

		// if normals needed
		if (pNormX)
		{
			*pNormX = hovered.normal.x;
			*pNormY = hovered.normal.y;
			*pNormZ = hovered.normal.z;
		}

		// optionally return actual object the cursor hovered over
		if (pHitEntity) *pHitEntity = hovered.entity;

		// report a hit
		return true;
	}
	else
	{
		// report a miss
		g_ray_pobject = NULL;
		return false;
	}
}

bool WickedCall_SentRay2(float originx, float originy, float originz, float directionx, float directiony, float directionz, float* pOutX, float* pOutY, float* pOutZ, float* pNormX, float* pNormY, float* pNormZ, uint64_t* pHitEntity, int iLayerMask)
{
	if (iLayerMask & GGRENDERLAYERS_NORMAL)
	{
		g_hovered_pobject = NULL;
		g_hovered_entity = 0;
	}

	RAY pickRay;
	XMFLOAT3 direction_inverse;
	pickRay.origin.x = originx;
	pickRay.origin.y = originy;
	pickRay.origin.z = originz;
	pickRay.direction.x = directionx;
	pickRay.direction.y = directiony;
	pickRay.direction.z = directionz;
	XMStoreFloat3(&direction_inverse, XMVectorDivide(XMVectorReplicate(1.0f), XMVectorSet(directionx, directiony, directionz, 1.0f)));
	pickRay.direction_inverse = direction_inverse;

	// check scene first if flagged, then terrain which is naturally underneath
	if ((iLayerMask & GGRENDERLAYERS_NORMAL) != 0 || (iLayerMask & GGRENDERLAYERS_CURSOROBJECT) != 0 || (iLayerMask & GGRENDERLAYERS_WIDGETPLANE) != 0)
	{
		wiScene::PickResult hovered = wiScene::Pick(pickRay, RENDERTYPE_ALL, iLayerMask);
		if (hovered.entity > 0)
		{
			if ((iLayerMask & GGRENDERLAYERS_NORMAL) != 0)
			{
				sObject* pHitObject = m_ObjectManager.FindObjectFromWickedObjectEntityID(hovered.entity);
				if (pHitObject)
				{
					bool ObjectIsEntity(void* pTestObject);

					//PE: Only highlight if this is a gg entity.
					bool bIsEntity = ObjectIsEntity(pHitObject);
					if (bIsEntity)
					{
						if (iLayerMask & GGRENDERLAYERS_NORMAL)
						{
							g_hovered_pobject = pHitObject;
							g_hovered_entity = hovered.entity;
							g_hovered_dot_pobject = NULL;
							g_hovered_dot_entity = 0;
						}
					}
					else
					{
						//#define DOTARCSOBJECTID 70001+40000+21001
						if (pHitObject->dwObjectNumber > 110000 && pHitObject->dwObjectNumber < 131002)
						{
							if (iLayerMask & GGRENDERLAYERS_NORMAL)
							{
								g_hovered_dot_pobject = pHitObject;
								g_hovered_dot_entity = hovered.entity;
							}
						}
					}
				}
				else
				{
					if (iLayerMask & GGRENDERLAYERS_NORMAL)
					{
						g_hovered_dot_pobject = NULL;
						g_hovered_dot_entity = 0;
					}
				}
			}

			// return hit position
			*pOutX = hovered.position.x;
			*pOutY = hovered.position.y;
			*pOutZ = hovered.position.z;

			if (iLayerMask & GGRENDERLAYERS_NORMAL)
			{
				fLastHitPosition[0] = hovered.position.x;
				fLastHitPosition[1] = hovered.position.y;
				fLastHitPosition[2] = hovered.position.z;
			}

			// if normals needed
			if (pNormX)
			{
				*pNormX = hovered.normal.x;
				*pNormY = hovered.normal.y;
				*pNormZ = hovered.normal.z;
			}

			// optionally return actual object the cursor hovered over
			if (pHitEntity) *pHitEntity = hovered.entity;

			// report a hit
			return true;
		}
	}
	if ((iLayerMask & GGRENDERLAYERS_TERRAIN) != 0)
	{
		if (GGTerrain::GGTerrain_RayCast(pickRay, pOutX, pOutY, pOutZ, pNormX, pNormY, pNormZ, 0))
		{
			fLastTerrainHitX = *pOutX, fLastTerrainHitY = *pOutY, fLastTerrainHitZ = *pOutZ;
			return true;
		}
		else
		{
			fLastTerrainHitX = 0, fLastTerrainHitY = 0, fLastTerrainHitZ = 0;
		}

	}
	return false;
}

bool WickedCall_SentRay4(float originx, float originy, float originz, float directionx, float directiony, float directionz, float fDistanceOfRay, float* pOutX, float* pOutY, float* pOutZ, float* pNormX, float* pNormY, float* pNormZ, DWORD* pdwObjectNumberHit, bool bOpaqueOnly)
{
	// ray cast specifically used by game loop to find accurate position of animating objects (performant?)
	RAY pickRay;
	XMFLOAT3 direction_inverse;
	pickRay.origin.x = originx;
	pickRay.origin.y = originy;
	pickRay.origin.z = originz;
	pickRay.direction.x = directionx;
	pickRay.direction.y = directiony;
	pickRay.direction.z = directionz;
	XMStoreFloat3(&direction_inverse, XMVectorDivide(XMVectorReplicate(1.0f), XMVectorSet(directionx, directiony, directionz, 1.0f)));
	pickRay.direction_inverse = direction_inverse;
	RENDERTYPE checkType = RENDERTYPE_ALL;
	if (bOpaqueOnly == true) checkType = RENDERTYPE_OPAQUE;
	wiScene::PickResult hit = wiScene::Pick(pickRay, checkType, GGRENDERLAYERS_NORMAL);
	if (hit.entity > 0)
	{
		float fDX = hit.position.x - originx;
		float fDY = hit.position.y - originy;
		float fDZ = hit.position.z - originz;
		float fDistOfHit = sqrt(fabs(fDX*fDX)+fabs(fDY*fDY)+fabs(fDZ*fDZ));
		if (fDistOfHit <= fDistanceOfRay)
		{
			sObject* pHitObject = m_ObjectManager.FindObjectFromWickedObjectEntityID(hit.entity);
			if (pHitObject) *pdwObjectNumberHit = pHitObject->dwObjectNumber;
			*pOutX = hit.position.x;
			*pOutY = hit.position.y;
			*pOutZ = hit.position.z;
			if (pNormX)
			{
				*pNormX = hit.normal.x;
				*pNormY = hit.normal.y;
				*pNormZ = hit.normal.z;
			}
			return true;
		}
	}
	return false;
}

bool WickedCall_SentRay3(float originx, float originy, float originz, float directionx, float directiony, float directionz, float fDistanceOfRay, float* pOutX, float* pOutY, float* pOutZ, float* pNormX, float* pNormY, float* pNormZ, DWORD* pdwObjectNumberHit)
{
	return WickedCall_SentRay4(originx, originy, originz, directionx, directiony, directionz, fDistanceOfRay, pOutX, pOutY, pOutZ, pNormX, pNormY, pNormZ, pdwObjectNumberHit, false);
}

void WickedCall_GetMouseDeltas(float* pfX, float* pfY)
{
	XMFLOAT4 currentMouse = wiInput::GetPointer();
	*pfX = currentMouse.x - g_lastMousePos.x;
	*pfY = currentMouse.y - g_lastMousePos.y;
	g_lastMousePos = currentMouse;
}

uint32_t WickedCall_GetTextureWidth(void* ptex)
{
	Texture* tex = (Texture*)ptex;
	return( tex->GetDesc().Width);
}
uint32_t WickedCall_GetTextureHeight(void* ptex)
{
	Texture* tex = (Texture*)ptex;
	return(tex->GetDesc().Height);
}

uint64_t WickedCall_GetFirstRootEntityID(sObject* pObject)
{
	//PE: Each frame has its own master object id.
	for (int iFrameIndex = 0; iFrameIndex < pObject->iFrameCount; iFrameIndex++)
	{
		if (pObject->ppFrameList && pObject->ppFrameList[iFrameIndex])
		{
			sFrame* pFrame = pObject->ppFrameList[iFrameIndex];
			if (pFrame && pFrame->wickedobjindex > 0)
			{
				ObjectComponent* object = wiScene::GetScene().objects.GetComponent(pFrame->wickedobjindex);
				if (object) {
					return(pFrame->wickedobjindex);
				}
			}
		}
	}
	return(0);
}

int WickedCall_GetNumberOfRootEntityIDs(sObject* pObject)
{
	int iRootNumber = 0;
	for (int iFrameIndex = 0; iFrameIndex < pObject->iFrameCount; iFrameIndex++)
	{
		sFrame* pFrame = pObject->ppFrameList[iFrameIndex];
		if (pFrame && pFrame->wickedobjindex > 0)
		{
			ObjectComponent* object = wiScene::GetScene().objects.GetComponent(pFrame->wickedobjindex);
			if (object) {
				iRootNumber++;
			}
		}
	}
	return(iRootNumber);
}

void WickedCall_SetSelectedObject(sObject* pObject)
{
	g_selected_entity = 0;
	g_selected_pobject = pObject;

	if (!pObject) {
		return;
	}
	uint64_t rootEntity = WickedCall_GetFirstRootEntityID(pObject);
	if (rootEntity > 0)
	{
		g_selected_entity = rootEntity;
		g_selected_pobject = pObject;
	}
	return;
}

void WickedCall_SetObjectHighlightColor(sObject* pObject, bool bHighlight, int highlightColorType)
{
	uint64_t rootEntity = WickedCall_GetFirstRootEntityID(pObject);
	if (rootEntity > 0) 
	{
		wiScene::ObjectComponent* pWickedObject = wiScene::GetScene().objects.GetComponent(rootEntity);
		if (bHighlight) 
		{
			if (pWickedObject) pWickedObject->SetUserStencilRef((EDITORSTENCILREF)highlightColorType);
		}
		else 
		{
			if (pWickedObject) pWickedObject->SetUserStencilRef(EDITORSTENCILREF_CLEAR);
		}
	}
	for (int iF = 0; iF < pObject->iFrameCount; iF++)
	{
		if (pObject->ppFrameList && pObject->ppFrameList[iF])
		{
			uint64_t objectEntity = pObject->ppFrameList[iF]->wickedobjindex;
			if (objectEntity > 0 && rootEntity != objectEntity)
			{
				wiScene::ObjectComponent* pWickedObject = wiScene::GetScene().objects.GetComponent(objectEntity);
				if (bHighlight) 
				{
					if (pWickedObject) pWickedObject->SetUserStencilRef((EDITORSTENCILREF)highlightColorType);
				}
				else 
				{
					if (pWickedObject) pWickedObject->SetUserStencilRef(EDITORSTENCILREF_CLEAR);
				}
			}
		}
	}
}

void WickedCall_SetObjectHighlightRed(sObject* pObject, bool bHighlight)
{
	return WickedCall_SetObjectHighlightColor (pObject, bHighlight, (int)EDITORSTENCILREF_HIGHLIGHT_OBJECT_RED);
}

void WickedCall_SetObjectHighlightBlue(sObject* pObject, bool bHighlight)
{
	return WickedCall_SetObjectHighlightColor (pObject, bHighlight, (int)EDITORSTENCILREF_HIGHLIGHT_OBJECT_BLUE);
}

void WickedCall_SetObjectHighlight(sObject* pObject, bool bHighlight)
{
	uint64_t rootEntity = WickedCall_GetFirstRootEntityID(pObject);
	if (rootEntity > 0) 
	{
		wiScene::ObjectComponent* pWickedObject = wiScene::GetScene().objects.GetComponent(rootEntity);
		if (bHighlight) 
		{
			if (pWickedObject) pWickedObject->SetUserStencilRef(EDITORSTENCILREF_HIGHLIGHT_OBJECT);
		}
		else 
		{
			if (pWickedObject)
				pWickedObject->SetUserStencilRef(EDITORSTENCILREF_CLEAR);
		}
	}
	
	for (int iF = 0; iF < pObject->iFrameCount; iF++)
	{
		if (pObject->ppFrameList && pObject->ppFrameList[iF])
		{
			uint64_t objectEntity = pObject->ppFrameList[iF]->wickedobjindex;
			if (objectEntity > 0 && rootEntity != objectEntity)
			{
				wiScene::ObjectComponent* pWickedObject = wiScene::GetScene().objects.GetComponent(objectEntity);
				if (bHighlight) {
					if (pWickedObject) pWickedObject->SetUserStencilRef(EDITORSTENCILREF_HIGHLIGHT_OBJECT);
				}
				else {
					if (pWickedObject) pWickedObject->SetUserStencilRef(EDITORSTENCILREF_CLEAR);
				}
			}
		}
	}

}


void WickedCall_DrawObjctBox_CHECK_IF_WE_HAVE_A_GG_COLLISION_PROBLEM(sObject* pObject, XMFLOAT4 color)
{
	if (!pObject) return;

	GGMATRIX matARotation;
	GGVECTOR3 box1;
	GGMATRIX matRotateX, matRotateY, matRotateZ;
	if (pObject->position.bFreeFlightRotation)
	{
		matARotation = pObject->position.matFreeFlightRotate;
	}
	else
	{
		GGMatrixRotationX(&matRotateX, GGToRadian(pObject->position.vecRotate.x));	// x rotation
		GGMatrixRotationY(&matRotateY, GGToRadian(pObject->position.vecRotate.y));	// y rotation
		GGMatrixRotationZ(&matRotateZ, GGToRadian(pObject->position.vecRotate.z));	// z rotation
		matARotation = matRotateX * matRotateY * matRotateZ;
	}
	if (pObject->position.bApplyPivot)
	{
		matARotation *= pObject->position.matPivot;
	}

	AABB aabb;
	aabb._min.x = pObject->collision.vecMin.x;
	aabb._min.y = pObject->collision.vecMin.y;
	aabb._min.z = pObject->collision.vecMin.z;
	aabb._max.x = pObject->collision.vecMax.x;
	aabb._max.y = pObject->collision.vecMax.y;
	aabb._max.z = pObject->collision.vecMax.z;

	box1.x = aabb._min.x; box1.y = aabb._min.y; box1.z = aabb._min.z;
	GGVec3TransformCoord(&box1, &box1, &matARotation);
	aabb._min.x = box1.x; aabb._min.y = box1.y; aabb._min.z = box1.z;
	box1.x = aabb._max.x; box1.y = aabb._max.y; box1.z = aabb._max.z;
	GGVec3TransformCoord(&box1, &box1, &matARotation);
	aabb._max.x = box1.x; aabb._max.y = box1.y; aabb._max.z = box1.z;
	
	aabb._min.x = (aabb._min.x * pObject->position.vecScale.x) + pObject->position.vecPosition.x;
	aabb._min.y = (aabb._min.y * pObject->position.vecScale.y) + pObject->position.vecPosition.y;
	aabb._min.z = (aabb._min.z * pObject->position.vecScale.z) + pObject->position.vecPosition.z;
	aabb._max.x = (aabb._max.x * pObject->position.vecScale.x) + pObject->position.vecPosition.x;
	aabb._max.y = (aabb._max.y * pObject->position.vecScale.y) + pObject->position.vecPosition.y;
	aabb._max.z = (aabb._max.z * pObject->position.vecScale.z) + pObject->position.vecPosition.z;

	XMFLOAT4X4 hoverBox;
	XMStoreFloat4x4(&hoverBox, aabb.getAsBoxMatrix());
	wiRenderer::DrawBox(hoverBox, color);
}


void WickedCall_DrawObjctBox(sObject* pObject, XMFLOAT4 color, bool bThickLine, bool ForceBox)
{
	if (!pObject) return;

	if(!bUseEditorOutlineSelection())
		ForceBox = true;
	if (!ForceBox || bImGuiInTestGame)
	{
		if (ObjectExist(pObject->dwObjectNumber))
		{
			g_ObjectHighlightList.push_back(pObject->dwObjectNumber);
			if(color.x==1.0 && color.y==0.0 && color.z==0.0)
				WickedCall_SetObjectHighlightRed(pObject, true);
			else
				WickedCall_SetObjectHighlight(pObject, true);
		}
		return;
	}
	
	if (color.w == 0.0) return; //PE: Disable box if no color.

	AABB aabb;
	aabb._min.x = pObject->collision.vecMin.x;
	aabb._min.y = pObject->collision.vecMin.y;
	aabb._min.z = pObject->collision.vecMin.z;
	aabb._max.x = pObject->collision.vecMax.x;
	aabb._max.y = pObject->collision.vecMax.y;
	aabb._max.z = pObject->collision.vecMax.z;

	XMFLOAT4X4 hoverBox;

	XMMATRIX rot;
#ifndef MATCHCLASSICROTATION
	//PE: For some reaon we need ZXY rotation ?
	rot = XMMatrixRotationZ(GGToRadian(pObject->position.vecRotate.z));
	rot = rot * XMMatrixRotationX(GGToRadian(pObject->position.vecRotate.x));
	rot = rot * XMMatrixRotationY(GGToRadian(pObject->position.vecRotate.y));
#else
	rot = XMMatrixRotationX(GGToRadian(pObject->position.vecRotate.x));
	rot = rot * XMMatrixRotationY(GGToRadian(pObject->position.vecRotate.y));
	rot = rot * XMMatrixRotationZ(GGToRadian(pObject->position.vecRotate.z));
#endif

	GGVECTOR3 vObjectCenter = pObject->collision.vecCentre;

	XMFLOAT3 ext = aabb.getHalfWidth();
	XMMATRIX sca = XMMatrixScaling(ext.x*pObject->position.vecScale.x, ext.y*pObject->position.vecScale.y, ext.z*pObject->position.vecScale.z);
	XMMATRIX tra = XMMatrixTranslation(pObject->collision.vecCentre.x*pObject->position.vecScale.x, pObject->collision.vecCentre.y*pObject->position.vecScale.y, pObject->collision.vecCentre.z*pObject->position.vecScale.z) * rot;

	XMStoreFloat4x4(&hoverBox, sca * tra);

	hoverBox._41 += pObject->position.vecPosition.x;
	hoverBox._42 += pObject->position.vecPosition.y;
	hoverBox._43 += pObject->position.vecPosition.z;

	wiRenderer::DrawBox(hoverBox, color);

	if (bThickLine)
	{
		float seperate_line = 0.05; //0.1;
		hoverBox._41 += seperate_line;
		hoverBox._42 += seperate_line;
		hoverBox._43 += seperate_line;
		wiRenderer::DrawBox(hoverBox, color);
		hoverBox._41 -= seperate_line * 2.0;
		hoverBox._42 -= seperate_line * 2.0;
		hoverBox._43 -= seperate_line * 2.0;
		wiRenderer::DrawBox(hoverBox, color);
		hoverBox._42 += seperate_line;
		wiRenderer::DrawBox(hoverBox, color);
		hoverBox._41 += seperate_line * 2.0;
		hoverBox._43 += seperate_line * 2.0;
		wiRenderer::DrawBox(hoverBox, color);
	}

}


void WickedCall_DrawPoint(float fx, float fy, float fz, float size, XMFLOAT4 color, bool bThickLine)
{

	AABB aabb;
	aabb._min.x = fx- size;
	aabb._min.y = fy- size;
	aabb._min.z = fz- size;
	aabb._max.x = fx+ size;
	aabb._max.y = fy+ size;
	aabb._max.z = fz+ size;

	XMFLOAT4X4 hoverBox;

	XMFLOAT3 ext = aabb.getHalfWidth();
	XMMATRIX sca = XMMatrixScaling(ext.x*1.0, ext.y*1.0, ext.z*1.0);
	XMMATRIX tra = XMMatrixTranslation(0.0, 0.0, 0.0);

	XMStoreFloat4x4(&hoverBox, sca * tra);

	hoverBox._41 += fx;
	hoverBox._42 += fy;
	hoverBox._43 += fz;

	wiRenderer::DrawBox(hoverBox, color);

	if (bThickLine)
	{
		float seperate_line = 0.1;
		hoverBox._41 += seperate_line;
		hoverBox._42 += seperate_line;
		hoverBox._43 += seperate_line;
		wiRenderer::DrawBox(hoverBox, color);
		hoverBox._41 -= seperate_line * 2.0;
		hoverBox._42 -= seperate_line * 2.0;
		hoverBox._43 -= seperate_line * 2.0;
		wiRenderer::DrawBox(hoverBox, color);
		hoverBox._42 += seperate_line;
		wiRenderer::DrawBox(hoverBox, color);
		hoverBox._41 += seperate_line * 2.0;
		hoverBox._43 += seperate_line * 2.0;
		wiRenderer::DrawBox(hoverBox, color);
	}

}

void WickedCall_DrawObjctBox_Color(sObject* pObject, float r, float g, float b, float a)
{
	XMFLOAT4 color = { r,g,b,a };
	WickedCall_DrawObjctBox(pObject, color);
}

void WickedCall_RenderEditorFunctions( void )
{
	// if shooter genre mode active, show all logic objects
	extern bool Shooter_Tools_Window;
	if (Shooter_Tools_Window == true)
	{
		// only logic highlights
		Wicked_Highlight_ClearAllObjects();
		Wicked_Highlight_AllLogicObjects();
		return;
	}

	// Remove all highlights
	if (g_ObjectHighlightList.size() > 0)
	{
		for (int i = 0; i < (int)g_ObjectHighlightList.size(); i++)
		{
			int obj = g_ObjectHighlightList[i];
			if (obj > 0)
			{
				if (ObjectExist(obj))
				{
					sObject* pObject = (sObject*)GetObjectsInternalData(obj);
					if (pObject)
						WickedCall_SetObjectHighlight(pObject, false);
				}
			}
		}
		g_ObjectHighlightList.clear();
	}

	// exit early if not in gridedit selection mode, in procedural level creation mode or in test level mode
	if (iGetgrideditselect() != 5)
		return;
	if (bProceduralLevel)
		return;
	if (bImGuiInTestGame)
		return;

	// highlight selected object too
	if (g_selected_pobject) 
	{
		WickedCall_DrawObjctBox(g_selected_pobject, XMFLOAT4(0.25f, 1.0f, 0.25f, 0.5f));
	}

	// highlight editor selection too
	if (g_selected_editor_object) 
	{
		try
		{
			if (ObjectExist(g_selected_editor_objectID))
			{
				int iObjNo = g_selected_editor_object->dwObjectNumber;
				if (iObjNo > 0 && iObjNo <= 300000)
				{
					WickedCall_DrawObjctBox(g_selected_editor_object, g_selected_editor_color);
					WickedCall_DrawObjctBox(g_selected_editor_object, g_selected_editor_color, true, true);
				}
			}
		}
		catch(...)
		{
			// error trapping in case object is deleted elsewhere!
			g_selected_editor_object = NULL;
			g_selected_editor_objectID = 0;
		}
	}

	// highlight highlighted object too
	if (g_highlight_pobject) 
	{
		#ifdef ONLY_USE_OUTLINE_HIGHLIGHT
		WickedCall_DrawObjctBox(g_highlight_pobject, XMFLOAT4(1.0f, 0.75f, 0.0f, 0.85f), true, false);
		#else
		WickedCall_DrawObjctBox(g_highlight_pobject, XMFLOAT4(1.0f, 0.75f, 0.0f, 0.85f), true, true);
		#endif
	}

	// process highlights for rubber banded objects, selections and locked objects
	Wicked_Highlight_Rubberband();
	Wicked_Highlight_Selection();
	Wicked_Highlight_LockedList();
}

void Wicked_Update_Shadows(void *voidvisual);
uint64_t WickedCall_AddLight(int iLightType)
{
	Entity light = wiScene::GetScene ( ).Entity_CreateLight ( "light", XMFLOAT3 ( 0, 0, 0 ), XMFLOAT3 ( 1, 1, 1 ), 30, 500 );
	LightComponent* lightComponent = wiScene::GetScene ( ).lights.GetComponent ( light );
	lightComponent->_flags = 0;
	lightComponent->SetType ( (wiScene::LightComponent::LightType)iLightType );
	Wicked_Update_Shadows(NULL);
	return light;
}

void WickedCall_DeleteLight(uint64_t wickedlightindex)
{
	wiScene::GetScene().Entity_Remove(wickedlightindex);
	//PE: When creating light also update probes.
	WickedCall_UpdateProbes();

	
}

//PE: Calculate needed light textures.
int WickedCall_Get2DShadowLights(void)
{
	wiScene::Scene* pScene = &wiScene::GetScene();
	int lights = pScene->lights.GetCount();
	int shadows = 0;
	for (int i = 0; i < lights; i++)
	{
		if (pScene->lights[i].GetType() == ENTITY_TYPE_SPOTLIGHT)
		{
			if (pScene->lights[i].IsCastingShadow()) shadows++;
		}
	}
	return(shadows);
}

int WickedCall_GetCubeShadowLights(bool bDebug)
{
	wiScene::Scene* pScene = &wiScene::GetScene();
	int lights = pScene->lights.GetCount();
	int shadows = 0;
	for (int i = 0; i < lights; i++)
	{
		if (pScene->lights[i].GetType() == ENTITY_TYPE_POINTLIGHT)
		{
			if (pScene->lights[i].IsCastingShadow())
			{
				shadows++;
				if (bDebug)
				{
					XMFLOAT4X4 hoverBox;
					AABB aabb = pScene->aabb_lights[i];
					XMStoreFloat4x4(&hoverBox, aabb.getAsBoxMatrix());
					XMFLOAT4 color = { 1,0,0,1 };
					wiRenderer::DrawBox(hoverBox, color);
				}
			}
		}
	}
	return(shadows);
}

int WickedCall_GetSpotShadowLights(bool bDebug)
{
	wiScene::Scene* pScene = &wiScene::GetScene();
	int lights = pScene->lights.GetCount();
	int shadows = 0;
	for (int i = 0; i < lights; i++)
	{
		if (pScene->lights[i].GetType() == ENTITY_TYPE_SPOTLIGHT)
		{
			if (pScene->lights[i].IsCastingShadow())
			{
				shadows++;
				if (bDebug)
				{
					XMFLOAT4X4 hoverBox;
					AABB aabb = pScene->aabb_lights[i];
					XMStoreFloat4x4(&hoverBox, aabb.getAsBoxMatrix());
					XMFLOAT4 color = { 1,0,0,1 };
					wiRenderer::DrawBox(hoverBox, color);
				}
			}
		}
	}
	return(shadows);
}


void WickedCall_UpdateLight(uint64_t wickedlightindex, float fX, float fY, float fZ, float fAX, float fAY, float fAZ, float fRange, float fSpotRadius, int iColR, int iColG, int iColB, bool bCastShadow)
{
	bool bLightScapeHasChangedEnoughForEnvProbeUpdate = false;

	LightComponent* lightComponent = wiScene::GetScene ( ).lights.GetComponent ( wickedlightindex );
	lightComponent->SetCastShadow ( bCastShadow );
	lightComponent->fov = GGToRadian(fSpotRadius);
	lightComponent->color = XMFLOAT3((float)iColR / 255.0f, (float)iColG / 255.0f, (float)iColB / 255.0f);

	// for now, only change env probe if light on/off
	TransformComponent* transformLight = wiScene::GetScene ().transforms.GetComponent (wickedlightindex);
	bool bDetectOnOffNotRangeChange = false;
	if (lightComponent->range_local != fRange)
	{
		if (lightComponent->range_local > 1 && fRange <= 1) bDetectOnOffNotRangeChange = true;
		if (lightComponent->range_local <= 1 && fRange > 1) bDetectOnOffNotRangeChange = true;
		lightComponent->range_local = fRange;
	}
	float fCurrentX = transformLight->GetPosition().x;
	if (fCurrentX <= -999999.0 && fX > -999999.0) bDetectOnOffNotRangeChange = true;
	if (fCurrentX > -999999.0 && fX <= -999999.0) bDetectOnOffNotRangeChange = true;
	if (bDetectOnOffNotRangeChange == true)
	{
		bLightScapeHasChangedEnoughForEnvProbeUpdate = true;
	}

	transformLight->ClearTransform();
	transformLight->Translate(XMFLOAT3(fX, fY, fZ));

	if (lightComponent->GetType() == LightComponent::SPOT)
	{
		// fAX, fAY, fAZ is a directional normal pointing in direction of spot light
		XMMATRIX rot;
		rot = XMMatrixRotationX(GGToRadian(fAX-90.0)); //Match the spot light object.
		rot = rot * XMMatrixRotationY(GGToRadian(fAY));
		rot = rot * XMMatrixRotationZ(GGToRadian(fAZ));
		XMVECTOR S;
		XMVECTOR R;
		XMVECTOR T;
		XMMatrixDecompose(&S, &R, &T, rot);
		transformLight->Rotate(R);
	}
	transformLight->SetDirty();

	// when light-scape changes, refresh env probes (performance hit here)
	if (bLightScapeHasChangedEnoughForEnvProbeUpdate == true)
	{
		extern bool g_bLightProbeInstantChange;
		g_bLightProbeInstantChange = true;
	}
}

void WickedCall_UpdateProbes(void)
{
	GGTerrain::ggterrain_extra_params.bUpdateProbes = true;
}

void WickedCall_EnableCameraLight(bool On)
{
	if (On)
	{
		if (!g_entityCameraLight)
		{
			g_entityCameraLight = WickedCall_AddLight(2);
			g_entityCameraLight = wiScene::GetScene().Entity_CreateLight("cameraLight", XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), 8, 2900);
			LightComponent* lightCamera = wiScene::GetScene().lights.GetComponent(g_entityCameraLight);
			lightCamera->SetCastShadow(false); //PE: No need for shadows its a editor light.
			lightCamera->color = XMFLOAT3(0.65, 0.65, 0.65); //PE: Dont make it to bright.
		}
	}
	else {
		if (g_entityCameraLight)
		{
			WickedCall_DeleteLight(g_entityCameraLight);
			g_entityCameraLight = NULL;
		}
	}
}

void WickedCall_EnableThumbLight(bool On)
{
	if (On)
	{
		if (!g_entityThumbLight)
		{
			//g_entityThumbLight = WickedCall_AddLight(2);
			g_entityThumbLight = wiScene::GetScene().Entity_CreateLight("thumbLight", XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), 8, 2900);
			LightComponent* lightCamera = wiScene::GetScene().lights.GetComponent(g_entityThumbLight);
			lightCamera->SetCastShadow(false); //PE: No need for shadows its a editor light.
			lightCamera->color = XMFLOAT3(0.65, 0.65, 0.65); //PE: Dont make it to bright.
			g_bLightShaftState = GetLightShaftState();
			g_bLensFlareState = GetLensFlareState();
		}
		if (!g_entityThumbLight2)
		{
			//g_entityThumbLight2 = WickedCall_AddLight(2);
			g_entityThumbLight2 = wiScene::GetScene().Entity_CreateLight("thumbLight2", XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), 8, 2900);
			LightComponent* lightCamera = wiScene::GetScene().lights.GetComponent(g_entityThumbLight2);
			lightCamera->SetCastShadow(false); //PE: No need for shadows its a editor light.
			lightCamera->color = XMFLOAT3(0.65, 0.65, 0.65); //PE: Dont make it to bright.
			g_bLightShaftState = GetLightShaftState();
			g_bLensFlareState = GetLensFlareState();
		}
		SetLightShaftState(false);
		SetLensFlareState(false);
	}
	else 
	{
		if (g_entityThumbLight)
		{
			WickedCall_DeleteLight(g_entityThumbLight);
			g_entityThumbLight = NULL;
		}
		if (g_entityThumbLight2)
		{
			WickedCall_DeleteLight(g_entityThumbLight2);
			g_entityThumbLight2 = NULL;
		}
		SetLightShaftState(g_bLightShaftState);
		SetLensFlareState(g_bLensFlareState);
	}
}

void WickedCall_SetEditorCameraLight(bool bSwitchOn)
{
	LightComponent* lightCamera = wiScene::GetScene ( ).lights.GetComponent ( g_entityCameraLight );
	if (lightCamera)
	{
		if (bSwitchOn == true)
			lightCamera->color = XMFLOAT3(1, 1, 1);
		else
			lightCamera->color = XMFLOAT3(0, 0, 0);
	}
}

void WickedCall_SetSpriteBoundBox(bool bShow,float fX1, float fY1,float fX2, float fY2)
{
	//PE: Wicked sprite do not support DPI and placement is wrong. like if you set windows DPI to 150%.
	//PE: Use imgui to draw it.
	void DrawRubberBand(float fx, float fy, float fx2, float fy2);
	if (bShow == true)
	{
		float fMouseCenterOffset = 6.0f;
		fX1 -= fMouseCenterOffset;
		fY1 -= fMouseCenterOffset;
		fX2 -= fMouseCenterOffset;
		fY2 -= fMouseCenterOffset;
		DrawRubberBand(fX1, fY1, fX2, fY2);
	}
/*
	if (bShow == true)
	{
		fY1 -= 20; fY2 -= 20; // seems to help mouse vs screen accuracy (if change at source, free flight messes up)
		pboundbox[0]->params.pos.x = fX1;
		pboundbox[0]->params.pos.y = fY1;
		pboundbox[0]->params.siz.x = fX2 - fX1;
		pboundbox[0]->params.siz.y = 2;
		pboundbox[1]->params.pos.x = fX1;
		pboundbox[1]->params.pos.y = fY1;
		pboundbox[1]->params.siz.x = 2;
		pboundbox[1]->params.siz.y = fY2 - fY1;
		pboundbox[2]->params.pos.x = fX1;
		pboundbox[2]->params.pos.y = fY2-2;
		pboundbox[2]->params.siz.x = fX2 - fX1;
		pboundbox[2]->params.siz.y = 2;
		pboundbox[3]->params.pos.x = fX2-2;
		pboundbox[3]->params.pos.y = fY1;
		pboundbox[3]->params.siz.x = 2;
		pboundbox[3]->params.siz.y = fY2 - fY1;
	}
	else
	{
		pboundbox[0]->params.pos.x = -999999;
		pboundbox[1]->params.pos.x = -999999;
		pboundbox[2]->params.pos.x = -999999;
		pboundbox[3]->params.pos.x = -999999;
	}
*/
}

void WickedCall_SetSunDirection(float fAx, float fAy, float fAz)
{
	TransformComponent* transformSunLight = wiScene::GetScene().transforms.GetComponent(g_entitySunLight);
	transformSunLight->ClearTransform();
	XMFLOAT3 rotationinrads;
	rotationinrads.x = fAx * (3.141592654f / 180.0f);
	rotationinrads.y = fAy * (3.141592654f / 180.0f);
	rotationinrads.z = fAz * (3.141592654f / 180.0f);
	transformSunLight->RotateRollPitchYaw(rotationinrads);
}

void WickedCall_SetSunColors(float fRed, float fGreen, float fBlue,float fEnergy,float fFov, float fShadowBias)
{
	LightComponent* lightSun = wiScene::GetScene().lights.GetComponent(g_entitySunLight);
	lightSun->color.x = fRed;
	lightSun->color.y = fGreen;
	lightSun->color.z = fBlue;
	lightSun->energy = fEnergy;
	lightSun->fov = fFov;
	//lightSun->shadowBias = fShadowBias; no longer exists

	//SetVolumetricsEnabled
}
#include "master.h"

void WickedCall_SetVisualizerEnabled(bool bVisualizer)
{
	LightComponent* lightSun = wiScene::GetScene().lights.GetComponent(g_entitySunLight);
	lightSun->SetVisualizerEnabled(bVisualizer);

}

void WickedCall_SunSetVolumetricsEnabled(bool bVolumetrics)
{
	LightComponent* lightSun = wiScene::GetScene().lights.GetComponent(g_entitySunLight);
	lightSun->SetVolumetricsEnabled(bVolumetrics);
}

void WickedCall_SunSetSetStatic(bool bSetStatic)
{
	LightComponent* lightSun = wiScene::GetScene().lights.GetComponent(g_entitySunLight);
	lightSun->SetStatic(bSetStatic);
}
void WickedCall_SunSetRange(float fRange)
{
	LightComponent* lightSun = wiScene::GetScene().lights.GetComponent(g_entitySunLight);
	lightSun->range_global = fRange;
}

void WickedCall_SetTextureName(int obj, char *texturename)
{
	sObject* GetObjectData(int iID);
	sObject* pObject = GetObjectData(obj);
	if (pObject)
	{
		for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
		{
			sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
			if (pMesh && pMesh->pTextures)
			{
				strcpy(pMesh->pTextures[0].pName, texturename);
			}
		}
	}
}


void WickedCall_DrawImguiNow(void)
{
	void ImGuiHook_RenderCall(void* ctxptr);
	void ImGuiHook_RenderCall_Direct(void* ctxptr, void* d3dptr);
	extern LPGGDEVICE				m_pD3D;
	extern LPGGIMMEDIATECONTEXT		m_pImmediateContext;
	//GraphicsDevice* device = wiRenderer::GetDevice();
	//void* pContext = device->GetDeviceContext(0);
	//ImGuiHook_RenderCall((void*)pContext);
	//ImGuiHook_RenderCall((void*)m_pImmediateContext);
	ImGuiHook_RenderCall_Direct((void*)m_pImmediateContext, (void*)m_pD3D);
}

//PE: Changing the far plane in indoor levels can really boost the FPS.
void WickedCall_SetCameraFarPlanes(float farplane)
{
	float fNear = wiScene::GetCamera().zNearP;
	float fFar = farplane;
	float fCameraFov = wiScene::GetCamera().fov;
	wiScene::GetCamera().CreatePerspective((float)master.masterrenderer.GetLogicalWidth(), (float)master.masterrenderer.GetLogicalHeight(), fNear, fFar, fCameraFov);
}

void WickedCall_SetCameraFOV ( float fFOV )
{
	// from wicked camera
	float fNear = wiScene::GetCamera().zNearP;
	float fFar = wiScene::GetCamera().zFarP;

	// directly set camera FOV (ingame weapon FOV changes)
	float fCameraFov;
	extern bool bImGuiInTestGame;
	float fUsedFOV = fFOV;
	if (bImGuiInTestGame == false) fUsedFOV = 45;
	if (bImGuiInTestGame == true)
	{
		// when in game, weapon FOV correction
		fCameraFov = GGToRadian(fUsedFOV); // Oops - backwards logic, lower FOV needs lower angle passed in
	}
	else
	{
		// keep consistency with editor, even though reversed
		fCameraFov = XM_PI / (fUsedFOV / 15.0f); //Fit GG settings.
	}
	wiScene::GetCamera().CreatePerspective((float)master.masterrenderer.GetLogicalWidth(), (float)master.masterrenderer.GetLogicalHeight(), fNear, fFar, fCameraFov);
}

int WickedCall_GetSkinable(void)
{
	int iSkinable = 0;
	wiScene::Scene* pScene = &wiScene::GetScene();
	for (size_t i = 0; i < pScene->meshes.GetCount(); ++i)
	{
		Entity entity = pScene->meshes.GetEntity(i);
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(entity);
		//MeshComponent * mesh = pScene->meshes[i];
		if (mesh)
		{
			if (mesh->IsSkinned() && pScene->armatures.Contains(mesh->armatureID))
			{
				iSkinable++;
			}
		}
	}
	return(iSkinable);
}

int WickedCall_GetSkinableVisible(void)
{
	int iSkinable = 0;
	wiScene::Scene* pScene = &wiScene::GetScene();
	for (size_t i = 0; i < pScene->meshes.GetCount(); ++i)
	{
		Entity entity = pScene->meshes.GetEntity(i);
		wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(entity);
		//MeshComponent * mesh = pScene->meshes[i];
		if (mesh)
		{
			if (mesh->IsRenderable() && mesh->IsSkinned() && pScene->armatures.Contains(mesh->armatureID))
			{
				iSkinable++;
			}
		}
	}
	return(iSkinable);
}

void WickedCall_SetShadowRange(float ShadowFar)
{
	fWickedCallShadowFarPlane = ShadowFar;
}


bool bCubesVisible = false; //PE: Default off.
void WickedCall_DisplayCubes(bool Visible)
{
	if (Visible) {
		bCubesVisible = Visible;
		return;
	}
	if (bCubesVisible && !Visible)
	{
		bCubesVisible = Visible;
		void set_terrain_sculpt_mode(int mode);
		void set_terrain_edit_mode(int mode);
		void clear_highlighted_tree(void);
		int get_terrain_sculpt_mode(void);
		extern int iLastTerrainSculptMode;
		if(iLastTerrainSculptMode == -1)
			iLastTerrainSculptMode = get_terrain_sculpt_mode();
		set_terrain_sculpt_mode(0); // GGTERRAIN_SCULPT_NONE; Disable terrain sculpt circle.
		set_terrain_edit_mode(0); // GGTERRAIN_EDIT_NONE; Disable terrain paint circle.
		clear_highlighted_tree();

		//PE: Not sure if we are going to use the below for grass ? so keep it here.

		Scene& scene = wiScene::GetScene();
		for (unsigned int i = 0; i < 32; i++)
		{
			Entity entity;
			TransformComponent* transform = NULL;

			entity = scene.Entity_FindByName("CubeDisplay_" + std::to_string(i));
			if (entity)
			{
				transform = scene.transforms.GetComponent(entity);
				if (transform)
				{
					transform->ClearTransform();
					transform->Translate(XMFLOAT3(-100000, -100000, -100000));
				}
			}
		}
	}
}

void WickedCall_CreateReflectionProbe(float x, float y, float z,char *name,float size)
{
	/*
	Entity entityProbe;
	XMFLOAT3 posProbe = XMFLOAT3(x, y, z);
	cstr sname = cstr("envprobe") + cstr(name);
	Scene& scene = wiScene::GetScene();
	for (size_t i = 0; i < scene.probes.GetCount(); ++i)
	{
		entityProbe = wiScene::GetScene().Entity_FindByName(sname.Get());
		if (entityProbe)
		{
			// Just update.
			EnvironmentProbeComponent* probe = wiScene::GetScene().probes.GetComponent(entityProbe);
			probe->position = posProbe;
			probe->range = size;
			probe->IsDirty();

			wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(entityProbe);
			pTransform->ClearTransform();
			pTransform->Translate(posProbe);
			pTransform->Scale(XMFLOAT3(size, size, size));
			pTransform->UpdateTransform();
			pTransform->SetDirty();
			WickedCall_UpdateProbes(); //refresh all.

			// updated this existing probe successfully
			return;
		}
	}

	// cannot have more than 16 light probes it seems (lighting messes up big style!)
	if (scene.probes.GetCount() >= 16)
		return;

	// Create a new probe
	entityProbe = wiScene::GetScene().Entity_CreateEnvironmentProbe(sname.Get(), posProbe);
	EnvironmentProbeComponent* probe = wiScene::GetScene().probes.GetComponent(entityProbe);
	probe->range = size;
	wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(entityProbe);
	pTransform->ClearTransform();
	pTransform->Translate(posProbe);
	pTransform->Scale(XMFLOAT3(size, size, size));
	pTransform->UpdateTransform();
	pTransform->SetDirty();

	// refresh all probes
	WickedCall_UpdateProbes();
	*/
}


void WickedCall_MoveReflectionProbe(float x, float y, float z, char *name, float size)
{
	/*
	Entity entityProbe;
	XMFLOAT3 posProbe = XMFLOAT3(x, y, z);
	cstr sname = cstr(name);
	Scene& scene = wiScene::GetScene();
	entityProbe = wiScene::GetScene().Entity_FindByName(sname.Get());
	if (entityProbe)
	{
		// Just update.
		EnvironmentProbeComponent* probe = wiScene::GetScene().probes.GetComponent(entityProbe);
		probe->position = posProbe;
		probe->range = size;
		probe->IsDirty();

		wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(entityProbe);
		pTransform->ClearTransform();
		pTransform->Translate(posProbe);
		pTransform->Scale(XMFLOAT3(size, size, size));
		pTransform->UpdateTransform();
		pTransform->SetDirty();
		WickedCall_UpdateProbes(); //refresh all.

		// updated this existing probe successfully
		return;
	}
	return;
	*/
}

void WickedCall_DeleteReflectionProbe(char *name)
{
	/*
	Entity entityProbe;
	cstr sname = cstr("envprobe") + cstr(name);
	Scene& scene = wiScene::GetScene();
	for (size_t i = 0; i < scene.probes.GetCount(); ++i)
	{
		entityProbe = wiScene::GetScene().Entity_FindByName(sname.Get());
		if (entityProbe)
		{
			// delete probe entity
			wiScene::GetScene().Entity_Remove(entityProbe);
		}
	}
	WickedCall_UpdateProbes(); //refresh all.
	*/
}


//PE: Capsule need more work. just some test code.
void WickedCall_DrawObjctCapsule(sObject* pObject, XMFLOAT4 color)
{
	if (!pObject) return;

	AABB aabb;
	aabb._min.x = pObject->collision.vecMin.x;
	aabb._min.y = pObject->collision.vecMin.y;
	aabb._min.z = pObject->collision.vecMin.z;
	aabb._max.x = pObject->collision.vecMax.x;
	aabb._max.y = pObject->collision.vecMax.y;
	aabb._max.z = pObject->collision.vecMax.z;

	XMFLOAT4X4 hoverBox;

	XMMATRIX rot;
#ifndef MATCHCLASSICROTATION
	//PE: For some reaon we need ZXY rotation ?
	rot = XMMatrixRotationZ(GGToRadian(pObject->position.vecRotate.z));
	rot = rot * XMMatrixRotationX(GGToRadian(pObject->position.vecRotate.x));
	rot = rot * XMMatrixRotationY(GGToRadian(pObject->position.vecRotate.y));
#else
	rot = XMMatrixRotationX(GGToRadian(pObject->position.vecRotate.x));
	rot = rot * XMMatrixRotationY(GGToRadian(pObject->position.vecRotate.y));
	rot = rot * XMMatrixRotationZ(GGToRadian(pObject->position.vecRotate.z));
#endif
	GGVECTOR3 vObjectCenter = pObject->collision.vecCentre;

	XMFLOAT3 ext = aabb.getHalfWidth();
	XMMATRIX sca = XMMatrixScaling(ext.x*pObject->position.vecScale.x, ext.y*pObject->position.vecScale.y, ext.z*pObject->position.vecScale.z);
	XMMATRIX tra = XMMatrixTranslation(pObject->collision.vecCentre.x*pObject->position.vecScale.x, pObject->collision.vecCentre.y*pObject->position.vecScale.y, pObject->collision.vecCentre.z*pObject->position.vecScale.z) * rot;

	XMStoreFloat4x4(&hoverBox, sca * tra);
	XMFLOAT3 pos = XMFLOAT3(pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z);
	XMFLOAT3 tip = pos;
	//tip.x += 10.0f;
	tip.y += 1.0f;
	//tip.z += 10.0f;

	CAPSULE capsule = CAPSULE(pos, tip, 40.0f);
	wiRenderer::DrawCapsule(capsule, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) );
}
void CapsuleTest(void)
{
	if (g_selected_editor_object)
	{
		if (ObjectExist(g_selected_editor_objectID) == 1)
		{
			WickedCall_DrawObjctCapsule(g_selected_editor_object, g_selected_editor_color);
		}
	}
}

void WickedCall_SetLDSSkinningEnabled(bool enabled)
{
	wiRenderer::SetLDSSkinningEnabled(enabled);
}

void WickedCall_SetLightShaftParameters(float density, float weight, float decay, float exposure)
{
	//LB: master.masterrenderer.setLightShaftValues(density, weight, decay, exposure);
}

void WickedCall_GetLightShaftParameters(float& density, float& weight, float& decay, float& exposure)
{
	//LB: density = master.masterrenderer.GetLightShaftDensity();
	//LB: weight = master.masterrenderer.GetLightShaftWeight();
	//LB: decay = master.masterrenderer.GetLightShaftDecay();
	//LB: exposure = master.masterrenderer.GetLightShaftExposure();
}


constexpr DXGI_FORMAT _ConvertFormat(FORMAT value)
{
	switch (value)
	{
	case FORMAT_UNKNOWN:
		return DXGI_FORMAT_UNKNOWN;
		break;
	case FORMAT_R32G32B32A32_FLOAT:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case FORMAT_R32G32B32A32_UINT:
		return DXGI_FORMAT_R32G32B32A32_UINT;
		break;
	case FORMAT_R32G32B32A32_SINT:
		return DXGI_FORMAT_R32G32B32A32_SINT;
		break;
	case FORMAT_R32G32B32_FLOAT:
		return DXGI_FORMAT_R32G32B32_FLOAT;
		break;
	case FORMAT_R32G32B32_UINT:
		return DXGI_FORMAT_R32G32B32_UINT;
		break;
	case FORMAT_R32G32B32_SINT:
		return DXGI_FORMAT_R32G32B32_SINT;
		break;
	case FORMAT_R16G16B16A16_FLOAT:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
		break;
	case FORMAT_R16G16B16A16_UNORM:
		return DXGI_FORMAT_R16G16B16A16_UNORM;
		break;
	case FORMAT_R16G16B16A16_UINT:
		return DXGI_FORMAT_R16G16B16A16_UINT;
		break;
	case FORMAT_R16G16B16A16_SNORM:
		return DXGI_FORMAT_R16G16B16A16_SNORM;
		break;
	case FORMAT_R16G16B16A16_SINT:
		return DXGI_FORMAT_R16G16B16A16_SINT;
		break;
	case FORMAT_R32G32_FLOAT:
		return DXGI_FORMAT_R32G32_FLOAT;
		break;
	case FORMAT_R32G32_UINT:
		return DXGI_FORMAT_R32G32_UINT;
		break;
	case FORMAT_R32G32_SINT:
		return DXGI_FORMAT_R32G32_SINT;
		break;
	case FORMAT_R32G8X24_TYPELESS:
		return DXGI_FORMAT_R32G8X24_TYPELESS;
		break;
	case FORMAT_D32_FLOAT_S8X24_UINT:
		return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		break;
	case FORMAT_R10G10B10A2_UNORM:
		return DXGI_FORMAT_R10G10B10A2_UNORM;
		break;
	case FORMAT_R10G10B10A2_UINT:
		return DXGI_FORMAT_R10G10B10A2_UINT;
		break;
	case FORMAT_R11G11B10_FLOAT:
		return DXGI_FORMAT_R11G11B10_FLOAT;
		break;
	case FORMAT_R8G8B8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case FORMAT_R8G8B8A8_UNORM_SRGB:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		break;
	case FORMAT_R8G8B8A8_UINT:
		return DXGI_FORMAT_R8G8B8A8_UINT;
		break;
	case FORMAT_R8G8B8A8_SNORM:
		return DXGI_FORMAT_R8G8B8A8_SNORM;
		break;
	case FORMAT_R8G8B8A8_SINT:
		return DXGI_FORMAT_R8G8B8A8_SINT;
		break;
	case FORMAT_R16G16_FLOAT:
		return DXGI_FORMAT_R16G16_FLOAT;
		break;
	case FORMAT_R16G16_UNORM:
		return DXGI_FORMAT_R16G16_UNORM;
		break;
	case FORMAT_R16G16_UINT:
		return DXGI_FORMAT_R16G16_UINT;
		break;
	case FORMAT_R16G16_SNORM:
		return DXGI_FORMAT_R16G16_SNORM;
		break;
	case FORMAT_R16G16_SINT:
		return DXGI_FORMAT_R16G16_SINT;
		break;
	case FORMAT_R32_TYPELESS:
		return DXGI_FORMAT_R32_TYPELESS;
		break;
	case FORMAT_D32_FLOAT:
		return DXGI_FORMAT_D32_FLOAT;
		break;
	case FORMAT_R32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
		break;
	case FORMAT_R32_UINT:
		return DXGI_FORMAT_R32_UINT;
		break;
	case FORMAT_R32_SINT:
		return DXGI_FORMAT_R32_SINT;
		break;
	case FORMAT_R24G8_TYPELESS:
		return DXGI_FORMAT_R24G8_TYPELESS;
		break;
	case FORMAT_D24_UNORM_S8_UINT:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	case FORMAT_R8G8_UNORM:
		return DXGI_FORMAT_R8G8_UNORM;
		break;
	case FORMAT_R8G8_UINT:
		return DXGI_FORMAT_R8G8_UINT;
		break;
	case FORMAT_R8G8_SNORM:
		return DXGI_FORMAT_R8G8_SNORM;
		break;
	case FORMAT_R8G8_SINT:
		return DXGI_FORMAT_R8G8_SINT;
		break;
	case FORMAT_R16_TYPELESS:
		return DXGI_FORMAT_R16_TYPELESS;
		break;
	case FORMAT_R16_FLOAT:
		return DXGI_FORMAT_R16_FLOAT;
		break;
	case FORMAT_D16_UNORM:
		return DXGI_FORMAT_D16_UNORM;
		break;
	case FORMAT_R16_UNORM:
		return DXGI_FORMAT_R16_UNORM;
		break;
	case FORMAT_R16_UINT:
		return DXGI_FORMAT_R16_UINT;
		break;
	case FORMAT_R16_SNORM:
		return DXGI_FORMAT_R16_SNORM;
		break;
	case FORMAT_R16_SINT:
		return DXGI_FORMAT_R16_SINT;
		break;
	case FORMAT_R8_UNORM:
		return DXGI_FORMAT_R8_UNORM;
		break;
	case FORMAT_R8_UINT:
		return DXGI_FORMAT_R8_UINT;
		break;
	case FORMAT_R8_SNORM:
		return DXGI_FORMAT_R8_SNORM;
		break;
	case FORMAT_R8_SINT:
		return DXGI_FORMAT_R8_SINT;
		break;
	case FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM;
		break;
	case FORMAT_BC1_UNORM_SRGB:
		return DXGI_FORMAT_BC1_UNORM_SRGB;
		break;
	case FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM;
		break;
	case FORMAT_BC2_UNORM_SRGB:
		return DXGI_FORMAT_BC2_UNORM_SRGB;
		break;
	case FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM;
		break;
	case FORMAT_BC3_UNORM_SRGB:
		return DXGI_FORMAT_BC3_UNORM_SRGB;
		break;
	case FORMAT_BC4_UNORM:
		return DXGI_FORMAT_BC4_UNORM;
		break;
	case FORMAT_BC4_SNORM:
		return DXGI_FORMAT_BC4_SNORM;
		break;
	case FORMAT_BC5_UNORM:
		return DXGI_FORMAT_BC5_UNORM;
		break;
	case FORMAT_BC5_SNORM:
		return DXGI_FORMAT_BC5_SNORM;
		break;
	case FORMAT_B8G8R8A8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
		break;
	case FORMAT_B8G8R8A8_UNORM_SRGB:
		return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		break;
	case FORMAT_BC6H_UF16:
		return DXGI_FORMAT_BC6H_UF16;
		break;
	case FORMAT_BC6H_SF16:
		return DXGI_FORMAT_BC6H_SF16;
		break;
	case FORMAT_BC7_UNORM:
		return DXGI_FORMAT_BC7_UNORM;
		break;
	case FORMAT_BC7_UNORM_SRGB:
		return DXGI_FORMAT_BC7_UNORM_SRGB;
		break;
	}
	return DXGI_FORMAT_UNKNOWN;
}

void Wicked_Memory_Use_Textures(void)
{
	void timestampactivity(int i, char* desc_s);
	char timestampMsg[1024];
	sprintf(timestampMsg, "WICKED TEXTURES:");
	timestampactivity(0, timestampMsg);

	//PE: Displaying everything directly is way better.
	wiScene::Scene* pScene = &wiScene::GetScene();
	//wiScene::MaterialComponent& material = *pScene->materials.GetComponent(materialEntity);
	long usedsize = 0;
	long usedFilesize = 0;
	auto size = pScene->materials.GetCount();
	std::vector<void*> already_registred; //PE: We reuse textures, so ignore when reused.
	for (int i = 0; i < size; i++)
	{
		for (int a = 0; a < 13; a++) //TEXTURESLOT_COUNT = 13
		{
			if (!pScene->materials[i].textures[a].name.empty())
			{
				std::shared_ptr<wiResource> image = pScene->materials[i].textures[a].resource;
				if (image)
				{
					auto filedata = image->filedata.data();
					auto filesize = image->filedata.size();

					void *pmat = (void *)pScene->materials[i].textures[a].GetGPUResource();
					ID3D11ShaderResourceView* lpTexture = (ID3D11ShaderResourceView*)wiRenderer::GetDevice()->MaterialGetSRV((void*)pmat);
					bool bAlreadyDisplayed = false;
					for(int b=0;b< already_registred.size();b++)
						if (already_registred[b] == (void*)lpTexture) { bAlreadyDisplayed = true; break; }
					if (!bAlreadyDisplayed)
					{
						already_registred.push_back((void*)lpTexture);

						wiGraphics::Texture texture = image->texture;
						auto Height = texture.desc.Height;
						auto Width = texture.desc.Width;
						auto Format = texture.desc.Format;
						TextureDesc imgdesc = texture.GetDesc();
						DXGI_FORMAT DxgiFormat = _ConvertFormat(Format);

						int getBitsPerPixel(int fmt);
						float bperpixel = 32;
						if (DxgiFormat != DXGI_FORMAT_UNKNOWN)
						{
							bperpixel = (float)getBitsPerPixel(DxgiFormat) / 8;
							if (bperpixel == 0.5) {
								bperpixel -= 0.125; // remove alpha count from BC1
							}
						}
						int addmipmapssize;

						if (imgdesc.MipLevels > 1) {
							addmipmapssize = (int)((float)(Width*Height) * bperpixel) / 1024 * imgdesc.ArraySize - 1; // Full mipmaps always give size -1.
							if (addmipmapssize <= 0) addmipmapssize = 0;
						}
						else {
							addmipmapssize = 0;
						}

						usedFilesize += filesize;
						usedsize += ((int)(((float)(Width*Height) * bperpixel) / 1024) * imgdesc.ArraySize) + addmipmapssize;

						std::string getImageformat(int fmt);
						sprintf(timestampMsg, "WList%d: (%ld,%ld) (%ld kb.+ mipm %ld kb.) filesize %ldkb mipmaps %d array %d format %s \"%s\" (*%ld)", i, Width, Height, (int)((float)(Width*Height) * bperpixel) / 1024 * imgdesc.ArraySize, addmipmapssize, (long)filesize / 1024.0, imgdesc.MipLevels, imgdesc.ArraySize, getImageformat(DxgiFormat).c_str(), pScene->materials[i].textures[a].name.c_str(), lpTexture);
						timestampactivity(0, timestampMsg);
					}
				}
			}
		}
	}
	sprintf(timestampMsg, "Total WICKED texture mem used: %ld (%.2fmb) (%.2fgb)", usedsize, (float)usedsize / 1024.0, (float)usedsize / 1024.0 / 1024.0);
	timestampactivity(0, timestampMsg);
	sprintf(timestampMsg, "Total WICKED filedata allocated used: %ld (%.2fmb) (%.2fgb)", usedFilesize, (float)usedFilesize / 1024.0, (float)usedFilesize / 1024.0 / 1024.0);
	timestampactivity(0, timestampMsg);
	sprintf(timestampMsg, "Total: %ld (%.2fmb) (%.2fgb)", usedsize+usedFilesize, (float)(usedsize+usedFilesize) / 1024.0, (float)(usedsize+usedFilesize) / 1024.0 / 1024.0);
	timestampactivity(0, timestampMsg);


}

void WickedCall_SetRenderTargetMouseFocus(bool focus)
{
	bRenderTargetHasFocus = focus;
}



void WickedCall_UpdateWaterColor(float red, float green, float blue)
{
	wiScene::WeatherComponent* weather = wiScene::GetScene().weathers.GetComponent(g_weatherEntityID);
	if (weather)
	{
		XMFLOAT4 oldColor = weather->oceanParameters.waterColor;
		XMFLOAT4 waterColor = XMFLOAT4(red / 255.0f, green / 255.0f, blue / 255.0f, oldColor.w);
		weather->oceanParameters.waterColor = waterColor;
	}
}


void WickedCall_UpdateTreeWind(float wind)
{
	wiScene::WeatherComponent* weather = wiScene::GetScene().weathers.GetComponent(g_weatherEntityID);
	if (weather)
	{
		weather->tree_wind = wind;
	}
}

void WickedCall_UpdateWaterHeight(float height)
{
	wiScene::WeatherComponent* weather = wiScene::GetScene().weathers.GetComponent(g_weatherEntityID);
	if (weather)
	{
		weather->oceanParameters.waterHeight = height;
	}
}

void WickedCall_RemoveObjectTextures(sObject* pObject)
{
	for (int i = 0; i < pObject->iMeshCount; i++)
	{
		sMesh* pMesh = pObject->ppMeshList[i];
		if (pMesh)
		{
			// Remove mesh texures
			for (int slot = 0; slot < pMesh->dwTextureCount; slot++)
			{
				pMesh->pTextures[slot].pName[0] = 0;
			}

			// Remove Wicked material textures
			wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
			if (mesh)
			{
				// get material from mesh
				uint64_t materialEntity = mesh->subsets[0].materialID;
				wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
				if (pObjectMaterial)
				{
					for (int slot = 0; slot < wiScene::MaterialComponent::TEXTURESLOT_COUNT; slot++)
					{
						pObjectMaterial->textures[slot].name = "";
						pObjectMaterial->textures[slot].resource = nullptr;
						pObjectMaterial->SetDirty();
					}
				}
			}
		}
	}
}

void WickedCall_SetExposure(float exposure)
{
	master.masterrenderer.setExposure(exposure);
}

/* Experiment to stain surfaces, should not use decal projection
bool bDoJustOne = true;

void WickedCall_CreateDecal(sObject* pObject)
{
	// get object to attach it to
	//for (int i = 0; i < pObject->iMeshCount; i++)
	//{
		//sMesh* pMesh = pObject->ppMeshList[i];
		//if (pMesh)
		//{
	if (bDoJustOne == true)
	{
			Scene& scene = wiScene::GetScene();
			//Entity entity = scene.Entity_CreateDecal("DecalProjection", "gamecore/extras/enhanced/EAI Enhanced Weapon_Arms_UV_Template.png");
			Entity entity = scene.Entity_CreateDecal("DecalProjection", "databank/bloodsplash1.png");
			TransformComponent& transform = *scene.transforms.GetComponent(entity);
			//transform.MatrixTransform(pObject->orientation);
			//transform.RotateRollPitchYaw(XMFLOAT3(0, 0, 0)); //XMFLOAT3(XM_PIDIV2, 0, 0));
			//transform.Scale(XMFLOAT3(200, 200, 200));
			//scene.Component_Attach(entity, pMesh->wickedmeshindex);

			transform.ClearTransform();
			float x = 5000;// 5000 + (rand() % 500);
			float z = 0;// rand() % 500;
			transform.Translate(XMFLOAT3(x, 60, z));
			transform.RotateRollPitchYaw(XMFLOAT3(XMFLOAT3(XM_PIDIV2, 0, 0)));
			transform.Scale(XMFLOAT3(40, 40, 40));
			transform.UpdateTransform();
			transform.SetDirty();

			bDoJustOne = false;
	}

			//break;
		//}
	//}
}
*/

#ifdef WICKEDPARTICLESYSTEM
uint32_t WickedCall_LoadWiSceneDirect(Scene& scene2,char* filename, bool attached, char* changename, char* changenameto)
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	Entity root = 0;

	XMMATRIX& transformMatrix = XMMatrixIdentity();
	
	wiArchive archive(filename, true);
	if (archive.IsOpen())
	{
		if (archive.IsReadMode())
		{
			uint32_t reserved;
			archive >> reserved;
		}
		else
		{
			uint32_t reserved = 0;
			archive << reserved;
		}

		//PE: Keeping this alive to keep serialized resources alive until entity serialization ends:
		wiResourceManager::ResourceSerializer resource_seri;
		if (archive.GetVersion() >= 63)
		{
			wiResourceManager::Serialize(archive, resource_seri);
		}

		EntitySerializer seri;

		scene2.names.Serialize(archive, seri);
		scene2.layers.Serialize(archive, seri);
		scene2.transforms.Serialize(archive, seri);
		scene2.prev_transforms.Serialize(archive, seri);
		scene2.hierarchy.Serialize(archive, seri);
		scene2.materials.Serialize(archive, seri);
		scene2.meshes.Serialize(archive, seri);
		scene2.impostors.Serialize(archive, seri);
		scene2.objects.Serialize(archive, seri);
		scene2.aabb_objects.Serialize(archive, seri);
		scene2.rigidbodies.Serialize(archive, seri);

		scene2.softbodies.Serialize(archive, seri);

		scene2.armatures.Serialize(archive, seri);
		scene2.lights.Serialize(archive, seri);
		scene2.aabb_lights.Serialize(archive, seri);
		scene2.cameras.Serialize(archive, seri);
		scene2.probes.Serialize(archive, seri);
		scene2.aabb_probes.Serialize(archive, seri);
		scene2.forces.Serialize(archive, seri);
		scene2.decals.Serialize(archive, seri);
		scene2.aabb_decals.Serialize(archive, seri);
		scene2.animations.Serialize(archive, seri);
		scene2.emitters.Serialize(archive, seri);
		scene2.hairs.Serialize(archive, seri);
		scene2.weathers.Serialize(archive, seri);
		if (archive.GetVersion() >= 30)
		{
			scene2.sounds.Serialize(archive, seri);
		}
		if (archive.GetVersion() >= 37)
		{
			scene2.inverse_kinematics.Serialize(archive, seri);
		}
		if (archive.GetVersion() >= 38)
		{
			scene2.springs.Serialize(archive, seri);
		}
		if (archive.GetVersion() >= 46)
		{
			scene2.animation_datas.Serialize(archive, seri);
		}

		//PE: create new root:
		root = CreateEntity();
		scene2.transforms.Create(root);
		scene2.layers.Create(root).layerMask = ~0;

		//PE: Parent all unparented transforms to new root entity
		for (size_t i = 0; i < scene2.transforms.GetCount() - 1; ++i) // GetCount() - 1 because the last added was the "root"
		{
			Entity entity = scene2.transforms.GetEntity(i);
			if (!scene2.hierarchy.Contains(entity))
			{
				scene2.Component_Attach(entity, root);
			}
		}
		//PE: The root component is transformed, scene is updated:
		scene2.transforms.GetComponent(root)->MatrixTransform(transformMatrix);
		scene2.Update(0);

		if (!attached)
		{
			//PE: In this case, we don't care about the root anymore, so delete it. This will simplify overall hierarchy
			scene2.Component_DetachChildren(root);
			scene2.Entity_Remove(root);
			root = INVALID_ENTITY;
		}

		//PE: Support _e_ here for all materials.
		for (int i = 0; i < scene2.materials.GetCount(); i++)
		{
			for (int a = 0; a < MaterialComponent::EMISSIVEMAP; a++)
			{
				if (scene2.materials[i].textures[a].name.size() > 0)
				{
					if (!scene2.materials[i].textures[a].resource)
					{
						scene2.materials[i].textures[a].resource = WickedCall_LoadImage(scene2.materials[i].textures[a].name);
					}
				}
			}
		}
	}

	//PE: Fix for GG moved enums
	if (pestrcasestr(filename, ".wiscene"))
	{
		for (int i = 0; i < scene2.materials.GetCount(); i++)
		{
			if (scene2.materials[i].userBlendMode == BLENDMODE_PREMULTIPLIED)
				scene2.materials[i].userBlendMode = BLENDMODE_MULTIPLY;
			if (scene2.materials[i].userBlendMode == BLENDMODE_ALPHA)
				scene2.materials[i].userBlendMode = BLENDMODE_ADDITIVE;
			if (scene2.materials[i].userBlendMode == BLENDMODE_FORCEDEPTH)
				scene2.materials[i].userBlendMode = BLENDMODE_PREMULTIPLIED;
			if (scene2.materials[i].userBlendMode == BLENDMODE_ALPHANOZ)
				scene2.materials[i].userBlendMode = BLENDMODE_ALPHA;
			scene2.materials[i].SetDirty();
		}
	}

	if (changename && changenameto)
	{
		for (int i = 0; i < scene2.names.GetCount(); i++)
		{
			if (stricmp(scene2.names[i].name.c_str(), changename) == 0)
			{
				scene2.names[i].name = changenameto;
			}
		}
	}
	
	return root;
}
uint32_t WickedCall_LoadWiScene(char* filename, bool attached, char* changename, char* changenameto)
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	Scene scene2;
	Entity root = WickedCall_LoadWiSceneDirect(scene2, filename, attached, changename, changenameto);
	GetScene().Merge(scene2);
	return root;
}

//iAction = 1 Burst all. 2 = Pause. - 3 = Resume. - 4 = Restart
void WickedCall_PerformEmitterAction(int iAction, uint32_t emitter_root)
{

	Scene& scene = wiScene::GetScene();

	//PE: Scan emitters.
	for (int i = 0; i < scene.emitters.GetCount(); i++)
	{
		Entity emitter = scene.emitters.GetEntity(i);
		HierarchyComponent* hier = scene.hierarchy.GetComponent(emitter);
		if (hier)
		{
			if (hier->parentID == emitter_root)
			{
				wiEmittedParticle* ec = scene.emitters.GetComponent(emitter);
				switch (iAction)
				{
					case 1:
					{
						ec->Burst(0);
						break;
					}
					case 2:
					{
						ec->SetPaused(true);
						break;
					}
					case 3:
					{
						ec->SetPaused(false);
						break;
					}
					case 4:
					{
						ec->Restart();
						break;
					}
				}
			}
		}
	}

}

//#define WPEDebug
void WickedCall_UpdateEmitters(void)
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	//PE: Scan emitters.
	std::vector< uint32_t> parent_used;
	parent_used.clear();
	Scene& scene = wiScene::GetScene();
	for (int i = 0; i < scene.emitters.GetCount(); i++)
	{
		Entity emitter = scene.emitters.GetEntity(i);
		wiEmittedParticle* ec = scene.emitters.GetComponent(emitter);

#ifdef WPEDebug
		if (ec && ec->IsVolumeEnabled())
		{
			XMFLOAT4X4 hoverBox;
			wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(emitter);
			if (pTransform)
			{
				if (1)
				{
					AABB aabb;
					XMFLOAT3 pos = pTransform->GetPosition();
					//if (bIsUsingWidget && emitter_root != transform_entity)
					//{
					//	TransformComponent* root_tranform = scene.transforms.GetComponent(emitter_root);
					//	//PE: Need to add root here , as its not updated before next frame in wicked.
					//	if (root_tranform)
					//	{
					//		pos.x += root_tranform->GetPosition().x;
					//		pos.y += root_tranform->GetPosition().y;
					//		pos.z += root_tranform->GetPosition().z;
					//	}
					//}

					XMFLOAT3 sca = pTransform->GetScale();
					aabb._min = XMFLOAT3(pos.x - sca.x, pos.y, pos.z - sca.z);
					aabb._max = XMFLOAT3(pos.x + sca.x, pos.y + sca.y, pos.z + sca.z);

					XMStoreFloat4x4(&hoverBox, aabb.getAsBoxMatrix()); // *pTransform->GetLocalMatrix());
					XMVECTOR S, R, T;
					XMMatrixDecompose(&S, &R, &T, XMLoadFloat4x4(&hoverBox));

					//XMVECTOR R_local = XMLoadFloat4(&root_tranform->rotation_local);
					XMVECTOR R_local = XMLoadFloat4(&pTransform->rotation_local);
					XMStoreFloat4x4(&hoverBox,
						XMMatrixScalingFromVector(S) *
						XMMatrixRotationQuaternion(R_local) *
						XMMatrixTranslationFromVector(T));

					wiRenderer::DrawBox(hoverBox, XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
				}
			}
		}
#endif

		//PE: If bFollowCamera , find InDoor , OutDoor , UnderWater.
		//PE: bFindFloor ONLY if ec->bFollowCamera
		if (ec && (ec->bFindFloor || ec->bFollowCamera))
		{
			HierarchyComponent* hier = scene.hierarchy.GetComponent(emitter);
			if (hier)
			{
				if (hier->parentID != wiECS::INVALID_ENTITY)
				{
					bool bAlreadySet = false;
					for (int a = 0; a > parent_used.size(); a++)
					{
						if (parent_used[a] == hier->parentID)
						{
							bAlreadySet = true;
							break;
						}
					}
					if (!bAlreadySet)
					{
						parent_used.push_back(hier->parentID);
						TransformComponent* root_tranform = scene.transforms.GetComponent(hier->parentID);
						if (root_tranform)
						{

							if (ec->bFollowCamera)
							{
								float fX, fY, fZ;
								//float PlayerHeight = 62.0f;
								fX = CameraPositionX();
								//PE: PlayerHeight might have to be removed in GGM
								fY = CameraPositionY(); // -PlayerHeight;
								fZ = CameraPositionZ();
								root_tranform->ClearTransform();
								root_tranform->Translate(XMFLOAT3(fX, fY, fZ));
								root_tranform->UpdateTransform();
							}
							if (ec->bFindFloor && ec->bFollowCamera)
							{
								float fX = root_tranform->GetPosition().x;
								float fZ = root_tranform->GetPosition().z;
								float height = BT_GetGroundHeight(t.terrain.TerrainID, CameraPositionX(), CameraPositionZ());
								root_tranform->ClearTransform();
								root_tranform->Translate(XMFLOAT3(fX, height, fZ));
								root_tranform->UpdateTransform();
							}
						}
					}
				}
			}
		}
	}
}

uint32_t WickedCall_CreateEmitter(std::string& name, float posX, float posY, float posZ, uint32_t proot)
{
	XMFLOAT3 position = { posX , posY, posZ };

	//wiScene::Scene& scene = wiScene::GetScene();
	Scene scene;
	XMMATRIX& transformMatrix = XMMatrixIdentity();

	//PE: Create emitter.
	Entity entity = CreateEntity();

	scene.names.Create(entity) = name;
	scene.emitters.Create(entity).count = 10;

	wiEmittedParticle* ec;
	ec = scene.emitters.GetComponent(entity);
	ec->count = 40;
	ec->life = 2.5f;
	ec->size = 2;
	//ec->random_color = 1.0f;
	ec->gravity = XMFLOAT3(0, 9, 0);

	TransformComponent& transform = scene.transforms.Create(entity);
	transform.ClearTransform();
	transform.Translate(position);
	transform.Scale(XMFLOAT3(3, 1, 3));
	transform.UpdateTransform();

	scene.materials.Create(entity).userBlendMode = BLENDMODE_ADDITIVE; // BLENDMODE_ALPHA;

	//PE: Create root.
	Entity root;
	bool bUsePrevRoot = false;
	if (proot > 0)
	{
		root = proot;
		bUsePrevRoot = true;
	}
	else
	{
		root = CreateEntity();
		scene.transforms.Create(root);
		scene.layers.Create(root).layerMask = ~0;
		//emitter_root = root;
	}

	if (!bUsePrevRoot)
	{
		//PE: Parent all unparented transforms to new root entity
		for (size_t i = 0; i < scene.transforms.GetCount() - 1; ++i) // GetCount() - 1 because the last added was the "root"
		{
			Entity entity = scene.transforms.GetEntity(i);
			if (!scene.hierarchy.Contains(entity))
			{
				scene.Component_Attach(entity, root);
			}
		}
		//PE: The root component is transformed, scene is updated:
		scene.transforms.GetComponent(root)->MatrixTransform(transformMatrix);
		scene.Update(0);
	}
	GetScene().Merge(scene);

	//PE: Find name;
	wiScene::Scene& sceneR = wiScene::GetScene();

	for (int i = 0; i < sceneR.emitters.GetCount(); i++)
	{

		Entity emitter = sceneR.emitters.GetEntity(i);
		Entity text = sceneR.names.GetIndex(emitter);
		if (text > 0)
		{
			if (sceneR.names[text].name == name)
			{
				entity = emitter;
				break;
			}
		}
	}

	if (bUsePrevRoot)
	{
		sceneR.Component_Attach(entity, root);
		wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(entity);
		pTransform->ClearTransform();
		pTransform->Translate(position);
		pTransform->UpdateTransform();
	}

	return entity;
}

#endif

