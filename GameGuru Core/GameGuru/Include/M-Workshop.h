//----------------------------------------------------
//--- GAMEGURU - M-Workshop
//----------------------------------------------------
#ifndef MAXSTEAMWORKSHOP
#define MAXSTEAMWORKSHOP

// Include
#include "cstr.h"
 
// For Steam Workshop Usage
#include "steam/steam_api.h"

// Data structures
struct sWorkshopItem
{
	cstr sImage;
	cstr sName;
	cstr sDesc;
	cstr sMediaType;
	cstr sMediaFolder;
	PublishedFileId_t nPublishedFileId;
	cstr sSteamUserAccountID;
	cstr sSteamUsersPersonaName;
	bool bDownloadItemTriggered;
};
struct sWorkshopSteamUserName
{
	cstr sSteamUserAccountID;
	cstr sSteamUsersPersonaName;
};

// Externs
extern bool g_bWorkshopAvailable;
extern bool g_bUpdateWorkshopItemList;
extern cstr g_WorkshopUserPrompt;
extern std::vector<sWorkshopItem> g_workshopItemsList;
extern std::vector<sWorkshopSteamUserName> g_workshopSteamUserNames;
extern sWorkshopItem g_currentWorkshopItem;
extern int g_iSelectedExistingWorkshopItem;
extern int g_iCurrentMediaTypeForWorkshopItem;

// Functions
void workshop_init (bool bLoggedIn);
void workshop_free (void);
void workshop_new_item (void);
LPSTR workshop_getmediatypepath (int mediatypevalue);
int workshop_getvaluefromtype (LPSTR mediatypestring);
bool workshop_submit_item_check (void);
void workshop_submit_item_now (void);
void workshop_update_steamusernames (void);

// Callback Class for Steam Workshop
class CSteamUserGeneratedWorkshopItem
{
public:
	CSteamUserGeneratedWorkshopItem();
	~CSteamUserGeneratedWorkshopItem();

public:
	void SteamRunCallbacks();
	void CreateOrUpdateWorkshopItem();
	void OnWorkshopItemCreated(CreateItemResult_t* pCallback, bool bIOFailure);
	void OnWorkshopItemStartUpdate(PublishedFileId_t m_nPublishedFileId);
	void OnWorkshopItemUpdated(SubmitItemUpdateResult_t* pCallback, bool bIOFailure);
	CCallResult<CSteamUserGeneratedWorkshopItem, CreateItemResult_t> m_SteamCallResultWorkshopItemCreated;
	CCallResult<CSteamUserGeneratedWorkshopItem, SubmitItemUpdateResult_t> m_SteamCallResultWorkshopItemUpdated;
	void RefreshItemsList();
	void onWorkshopItemQueried(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure);
	CCallResult<CSteamUserGeneratedWorkshopItem, SteamUGCQueryCompleted_t> m_SteamCallResultWorkshopItemQueried;
};

#endif //MAXSTEAMWORKSHOP
