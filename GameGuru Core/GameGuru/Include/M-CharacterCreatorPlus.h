//----------------------------------------------------
//--- GAMEGURU - M-CharacterCreatorPlus
//----------------------------------------------------

// Includes
#include "cstr.h"

// Data type
struct sCharacterCreatorPlusMouthData
{
	float fTimeStamp;
	int iMouthShape;
};
struct sCharacterCreatorPlusSpeak
{
	std::vector <sCharacterCreatorPlusMouthData> mouthData;
	float fMouthTimeStamp;
	int iMouthDataIndex;
	int iMouthDataShape;
	float fSmouthDataSpeedToNextShape;
	float fNeedToBlink;
};
struct sCharacterCreatorPlusSettings
{
	cstr script_s;
	cstr voice_s;
	int iSpeakRate;
	int iGender;
	int iNeckBone;
};
struct sCharacterCreatorPlusObject
{
	sCharacterCreatorPlusSettings settings;
	sCharacterCreatorPlusSpeak speak;
};
struct sCharacterCreatorPlus
{
	bool bInitialised;
	float fTimerTrigger;
	sCharacterCreatorPlusObject obj;
};

#ifdef WICKEDENGINE
// Some parts (e.g. head gear) require specific head shapes or hair styles to ensure no meshes overlap.
//	This struct stores parts that require other parts to ensure no overlap occurs.
struct AutoSwapData
{
	int iPartType;
	std::string sPartName;
	std::vector<int> requiredSwapCategories;
	std::vector<std::string> requiredSwapNames;
	std::vector<std::string> swappedPartNames;
};

struct CameraTransition
{
	float from[3];
	float to[3];
	float t;
	float angFrom[2];
	float angTo[2];
};

#endif

void charactercreatorplus_preloadinitialcharacter ( void );
void charactercreatorplus_preloadallcharacterpartchoices ( void );

void charactercreatorplus_refreshtype(void);
void charactercreatorplus_init ( void );
void charactercreatorplus_free ( void );
bool charactercreatorplus_savecharacterentity ( int iCharObj, LPSTR pOptionalDBOSaveFile, int iThumbnailImage );
void charactercreatorplus_loop ( void );

void charactercreatorplus_imgui(void);
#ifdef WICKEDENGINE
void charactercreatorplus_imgui_v3(void);
#endif
void charactercreatorplus_preparechange(char *path, int part, char* tag);
void charactercreatorplus_waitforpreptofinish(void);
void charactercreatorplus_change(char *path, int part, char* tag);
void charactercreatorplus_loadannotationlist ( void );
LPSTR charactercreatorplus_findannotation ( LPSTR pSearchStr );
LPSTR charactercreatorplus_findannotationtag ( LPSTR pSearchStr );

void charactercreatorplus_initautoswaps();
void charactercreatorplus_getautoswapdata(char* filename);
int charactercreatorplus_getcategoryindex(char* category);
#ifdef WICKEDENGINE
void charactercreatorplus_storeautoswapdata(AutoSwapData* pData);
#endif
void charactercreatorplus_performautoswap(int part);
void charactercreatorplus_restoreswappedparts();
void charactercreatorplus_extracthaircolour(const char* source, char* destination);
void charactercreatorplus_restrictpart(int part);

#ifdef WICKEDENGINE
void charactercreatorplus_initcameratransitions();
void charactercreatorplus_changecameratransition(int part);
void charactercreatorplus_performcameratransition(bool bIsZooming = false);
#endif

void characterkitplus_chooseOnlineAvatar(void);
void characterkitplus_saveAvatarInfo(void);
void characterkitplus_checkAvatarExists(void);
void characterkitplus_loadMyAvatarInfo(void);
void characterkitplus_makeMultiplayerCharacterCreatorAvatar(void);
void characterkitplus_removeMultiplayerCharacterCreatorAvatar(void);