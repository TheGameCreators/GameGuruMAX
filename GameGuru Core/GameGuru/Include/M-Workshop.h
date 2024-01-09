//----------------------------------------------------
//--- GAMEGURU - M-Workshop
//----------------------------------------------------
#ifndef MAXSTEAMWORKSHOP
#define MAXSTEAMWORKSHOP

// Include
#include "cstr.h"

// Not for Educational Version
#ifndef GGMAXEDU

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
	int iNumberOfFilesInWorkshopItem;
	cstr sLatestDateOfItem;
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
extern std::vector<PublishedFileId_t> g_workshopTrustedItems;
extern sWorkshopItem g_currentWorkshopItem;
extern int g_iSelectedExistingWorkshopItem;
extern int g_iCurrentMediaTypeForWorkshopItem;
extern bool g_bStillDownloadingThings;
extern bool g_bStillDownloadingThingsWithDelay;
extern int g_iStillDownloadingThingsWithDelayTimer;
extern int g_iUnsubscribeByForce;

// Functions
void workshop_init (bool bLoggedIn);
void workshop_free (void);
void workshop_new_item (void);
LPSTR workshop_getmediatypepath (int mediatypevalue);
int workshop_getvaluefromtype (LPSTR mediatypestring);
bool workshop_submit_item_check (void);
void workshop_submit_item_now (void);
void workshop_update_steamusernames (void);
void workshop_subscribetoalltrusteditems (void);
cstr workshop_findtrustedreplacement(LPSTR pCoreScriptFile);
bool workshop_verifyandorreplacescript(int e, int entid);

// Callback Class for Steam Workshop
class CSteamUserGeneratedWorkshopItem
{
public:
	CSteamUserGeneratedWorkshopItem();
	~CSteamUserGeneratedWorkshopItem();

public:
	void UnsubscribeTrustedItems();
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

#endif //GGMAXEDU

#endif //MAXSTEAMWORKSHOP
