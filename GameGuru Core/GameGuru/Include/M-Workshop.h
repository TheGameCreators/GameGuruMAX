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
	cstr sMediaFolder;
	PublishedFileId_t nPublishedFileId;
};

// Externs
extern bool g_bWorkshopAvailable;
extern bool g_bUpdateWorkshopItemList;
extern cstr g_WorkshopUserPrompt;
extern std::vector<sWorkshopItem> g_workshopItemsList;
extern sWorkshopItem g_currentWorkshopItem;
extern int g_iSelectedExistingWorkshopItem;

// Functions
void workshop_init (bool bLoggedIn);
void workshop_free (void);
void workshop_new_item (void);
bool workshop_submit_item_check (void);
void workshop_submit_item_now (void);

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

	//void OnWorkshopItemSubscribed(RemoteStorageSubscribePublishedFileResult_t* pCallback, bool bIOFailure);
	//void OnWorkshopItemDownloaded(ItemInstalled_t* pCallback, bool bIOFailure);
	//void OnWorkshopItemDownloadDone(DownloadItemResult_t* pCallback);
	//CCallResult<CSteamUserGeneratedWorkshopItem, RemoteStorageSubscribePublishedFileResult_t> m_SteamCallResultWorkshopItemSubscribed;
	//CCallResult<CSteamUserGeneratedWorkshopItem, ItemInstalled_t> m_SteamCallResultWorkshopItemDownloaded;
	//CCallback<CSteamUserGeneratedWorkshopItem, DownloadItemResult_t > m_SteamCallResultWorkshopItemDownloadDone;
};

#endif //MAXSTEAMWORKSHOP
