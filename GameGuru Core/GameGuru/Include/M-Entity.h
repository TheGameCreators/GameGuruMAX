//----------------------------------------------------
//--- GAMEGURU - M-Entity
//----------------------------------------------------

#pragma once
#include "cstr.h"
#include "types.h"

void entity_addtoselection_core ( void );
void entity_addtoselection ( void );
void entity_adduniqueentity ( bool bAllowDuplicates );
void entity_validatearraysize ( void );
bool entity_load_thread_prepare(LPSTR pFpeFile);
bool entity_load(bool bCalledFromLibrary = false );
void entity_loaddata ( void );
void entity_loadvideoid ( void );
void entity_loadactivesoundsandvideo ( void );
void entity_fillgrideleproffromprofile ( void );
void entity_updatetextureandeffectfromeleprof ( void );
void entity_updatetextureandeffectfromgrideleprof ( void );
void entity_getgunidandflakid ( void );
void entity_loadtexturesandeffect ( void );
void entity_saveelementsdata ( void );
void entity_savebank ( void );
void entity_savebank_ebe ( void );
void entity_loadbank ( void );
void entity_loadelementsdata ( void );
void entity_loadentitiesnow ( void );
void entity_deletebank ( void );
void entity_deleteelementsdata ( void );
void entity_deleteelements ( void );
void entity_assignentityparticletodecalelement ( void );
void entity_addentitytomap_core ( void );
void entity_addentitytomap ( void );
void entity_deleteentityfrommap ( void );
#ifdef WICKEDENGINE
// new improved system using master and event stacks
void entity_createundoaction (int eventtype, int te, bool bUserAction = true);
#else
void entity_recordbuffer_add ( void );
void entity_recordbuffer_delete ( void );
void entity_recordbuffer_move ( void );
#endif
void entity_undo ( void );
void entity_redo ( void );
void entity_updateparticleemitterbyID(entityeleproftype* pEleprof, int iObj, float fScale, float fX, float fY, float fZ, float fRX, float fRY, float fRZ);
void entity_updateparticleemitter(int e);
void entity_updateautoflatten(int e, int obj = 0);
void entity_autoFlattenWhenAdded(int e, int obj = 0);

bool ObjectIsEntity(void* pTestObject);
bool IsWickedMaterialActive(void* pvMesh);
void WickedSetMeshNumber(int iMNumber);
cStr WickedGetBaseColorName(void);
cStr WickedGetNormalName(void);
cStr WickedGetSurfaceName(void);
cStr WickedGetDisplacementName(void);
cStr WickedGetEmissiveName(void);
cStr WickedGetOcclusionName(void);
void WickedSetEntityId(int ent_id);
int WickedGetEntityId(void);
float WickedGetTreeAlphaRef(void);
void WickedSetElementId(int ele_id);
void WickedSetUseEditorGrideleprof(bool bUse);
float WickedGetNormalStrength(void);
float WickedGetRoughnessStrength(void);
float WickedGetMetallnessStrength(void);
float WickedGetEmissiveStrength(void);
float WickedGetAlphaRef(void);
bool WickedDoubleSided(void);
float WickedRenderOrderBias(void);
bool WickedGetTransparent(void);
bool WickedGetCastShadows(void);
bool WickedPlanerReflection(void);
DWORD WickedGetEmmisiveColor(void);
DWORD WickedGetBaseColor(void);
float WickedGetReflectance(void);
void Wicked_Highlight_ClearAllObjects(void);
void Wicked_Highlight_AllLogicObjects(void);
void Wicked_Highlight_Rubberband(void);
void Wicked_Highlight_Selection(void);
void Wicked_Highlight_LockedList(void);
void Wicked_Hide_Lower_Lod_Meshes(int obj);
int GetLodLevels(int obj);

void entity_placeprobe(int obj, float fLightProbeScale);
void entity_deleteprobe(int obj);

void entity_updatequatfromeuler (int e);
void entity_updatequat (int e, float quatx, float quaty, float quatz, float quatw);
void entity_calculateeuleryfromquat (int e);