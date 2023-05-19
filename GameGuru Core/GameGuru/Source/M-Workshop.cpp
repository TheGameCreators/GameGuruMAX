//----------------------------------------------------
//--- GAMEGURU - M-Workshop
//----------------------------------------------------

#include "gameguru.h"
#include "M-Workshop.h"

// Globals
bool g_bWorkshopAvailable = false;
bool g_bUpdateWorkshopItemList = false;
cstr g_WorkshopUserPrompt = "";
std::vector<sWorkshopItem> g_workshopItemsList;
int g_iSelectedExistingWorkshopItem = -1;
sWorkshopItem g_currentWorkshopItem;
CSteamUserGeneratedWorkshopItem g_UserWorkShopItem;

// Functions
void workshop_init (bool bLoggedIn)
{
	// general inits
	g_bWorkshopAvailable = true;// bLoggedIn;
	g_bUpdateWorkshopItemList = true;
	g_WorkshopUserPrompt = "";
	g_workshopItemsList.clear();

	// current working item
	g_currentWorkshopItem.sImage = "";
	g_currentWorkshopItem.sName = "";
	g_currentWorkshopItem.sDesc = "";
	g_currentWorkshopItem.sMediaFolder = "";
}

void workshop_free (void)
{
	// any closing of resources needed
	g_bWorkshopAvailable = false;
}

void workshop_new_item (void)
{
	// when click Add New Item in HUB Workshop
	g_currentWorkshopItem.sImage = "";
	g_currentWorkshopItem.sName = "";
	g_currentWorkshopItem.sDesc = "";
	g_currentWorkshopItem.sMediaFolder = "";
}

bool workshop_submit_item_check (void)
{
	// first check have all fields filled
	if (g_currentWorkshopItem.sName.Len() > 0 && g_currentWorkshopItem.sDesc.Len() > 0 && g_currentWorkshopItem.sMediaFolder.Len() > 0)
	{
		// now check if workshop item already exists with this name for this user
		if (0)
		{
			g_WorkshopUserPrompt = "A workshop item already exists with that name, choose another name.";
			return false;
		}

		// this users steam ID for the folder
		uint32 uAccountID = SteamUser()->GetSteamID().GetAccountID();

		// check media folder created and valid
		char pMediaFolder[MAX_PATH];
		sprintf (pMediaFolder, "%s\\Files\\scriptbank\\Community\\%d\\%s", g.fpscrootdir_s.Get(), uAccountID, g_currentWorkshopItem.sMediaFolder.Get());
		GG_GetRealPath(pMediaFolder, true);
		if (PathExist(pMediaFolder) == 0)
		{
			g_WorkshopUserPrompt = "Workshop item media folder not found, create it in your community folder and add your files.";
			return false;
		}

		// check the specified preview image exists (warning only)
		if (g_currentWorkshopItem.sImage.Len() > 0)
		{
			char pPreviewImageFile[MAX_PATH];
			sprintf (pPreviewImageFile, "%s\\Files\\%s", g.fpscrootdir_s.Get(), g_currentWorkshopItem.sImage.Get());
			GG_GetRealPath(pPreviewImageFile, false);
			if (FileExist(pPreviewImageFile) == 0)
			{
				g_WorkshopUserPrompt = "Info: specified preview image does not exist.";
			}
		}

		// all checks complete, can submit
		return true;
	}
	else
	{
		// must fill in all fields
		g_WorkshopUserPrompt = "You must fill in all workshop item fields before you can submit.";
		return false;
	}
}

void workshop_submit_item_now (void)
{
	// create an item, and starts the submission process (uses callbacks to complete task)
	g_UserWorkShopItem.CreateOrUpdateWorkshopItem();
}

void workshop_update (void)
{
	// trigger workshop item list update
	if ( g_bUpdateWorkshopItemList == true )
	{
		g_UserWorkShopItem.RefreshItemsList();
		g_bUpdateWorkshopItemList = false;
	}

	// run Steam callbacks
	g_UserWorkShopItem.SteamRunCallbacks();
}

// Callback Functions for Steam Workshop
CSteamUserGeneratedWorkshopItem::CSteamUserGeneratedWorkshopItem()
{
}

CSteamUserGeneratedWorkshopItem::~CSteamUserGeneratedWorkshopItem()
{
}

void CSteamUserGeneratedWorkshopItem::SteamRunCallbacks()
{
	// only do steam update if logged into Steam
	ISteamUtils* pSteamUtilsPtr = SteamUtils();
	if (pSteamUtilsPtr)
	{
		SteamAPI_RunCallbacks();
	}
}

void CSteamUserGeneratedWorkshopItem::CreateOrUpdateWorkshopItem()
{
	// only start if logged into Steam
	ISteamUtils* pSteamUtilsPtr = SteamUtils();
	if (pSteamUtilsPtr)
	{
		// get AppID
		uint32 consumerAppID = SteamUtils()->GetAppID();
		#ifdef TESTSTEAMFREETRIAL
		consumerAppID = 1247289; // // dummy app, forces the free trial mode
		#endif

		// check if already have item
		int iItemExistsIndex = -1;
		for (int i = 0; i < g_workshopItemsList.size(); i++)
		{
			if (stricmp(g_workshopItemsList[i].sName.Get(), g_currentWorkshopItem.sName.Get()) == NULL)
			{
				iItemExistsIndex = i;
			}
		}
		if (iItemExistsIndex==-1)
		{
			// start creating an item
			SteamAPICall_t ret;
			ret = SteamUGC()->CreateItem (consumerAppID, k_EWorkshopFileTypeCommunity);
			m_SteamCallResultWorkshopItemCreated.Set(ret, this, &CSteamUserGeneratedWorkshopItem::OnWorkshopItemCreated);
			g_WorkshopUserPrompt = "Started the item creation process for this item...";
		}
		else
		{
			// update an existing item
			OnWorkshopItemStartUpdate ( g_currentWorkshopItem.nPublishedFileId );
			g_WorkshopUserPrompt = "Started the item update process for this item...";
		}
	}
	else
	{
		// not logged in
		g_WorkshopUserPrompt = "Cannot upload a workshop item when not logged into your Steam client account!";
	}
}

void CSteamUserGeneratedWorkshopItem::OnWorkshopItemCreated(CreateItemResult_t* pCallback, bool bIOFailure)
{
	if (pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement)
	{
		ShellExecuteW(NULL, L"open", L"http://steamcommunity.com/sharedfiles/workshoplegalagreement", NULL, NULL, SW_SHOWMAXIMIZED);
	}
	if (!bIOFailure)
	{
		OnWorkshopItemStartUpdate(pCallback->m_nPublishedFileId);
	}
}

void CSteamUserGeneratedWorkshopItem::OnWorkshopItemStartUpdate(PublishedFileId_t m_nPublishedFileId)
{
	// start the upload
	UGCUpdateHandle_t WorkShopItemUpdateHandle = NULL;
	WorkShopItemUpdateHandle = SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), m_nPublishedFileId);

	// specify details for the upload
	bool result;
	result = SteamUGC()->SetItemTitle(WorkShopItemUpdateHandle, g_currentWorkshopItem.sName.Get());
	result = SteamUGC()->SetItemDescription(WorkShopItemUpdateHandle, g_currentWorkshopItem.sDesc.Get());
	result = SteamUGC()->SetItemVisibility(WorkShopItemUpdateHandle, k_ERemoteStoragePublishedFileVisibilityPublic);

	// meta data (k_cchDeveloperMetadataMax)
	LPSTR pNoMetaData = "nometadata";
	result = SteamUGC()->SetItemMetadata(WorkShopItemUpdateHandle, pNoMetaData);

	// extra data we need when retrieving item details
	result = SteamUGC()->AddItemKeyValueTag(WorkShopItemUpdateHandle, "imagefile", g_currentWorkshopItem.sImage.Get());
	result = SteamUGC()->AddItemKeyValueTag(WorkShopItemUpdateHandle, "mediafolder", g_currentWorkshopItem.sMediaFolder.Get());

	// one tag
	char** pMyTags = new char* [1];
	for (int c = 0; c < 1; c++) pMyTags[c] = new char[128];
	strcpy (pMyTags[0], "Game Guru MAX");
	SteamParamStringArray_t tags;
	tags.m_nNumStrings = 1;
	tags.m_ppStrings = (const char**)pMyTags;
	result = SteamUGC()->SetItemTags(WorkShopItemUpdateHandle, &tags);
	for (int c = 0; c < 1; c++) delete[] pMyTags[c];
	delete[] pMyTags;

	// this users steam ID for the folder
	uint32 uAccountID = SteamUser()->GetSteamID().GetAccountID();

	// media folder and preview image
	char pMediaFolder[MAX_PATH];
	sprintf (pMediaFolder, "%s\\Files\\scriptbank\\Community\\%d\\%s", g.fpscrootdir_s.Get(), uAccountID, g_currentWorkshopItem.sMediaFolder.Get());
	GG_GetRealPath(pMediaFolder, true);
	result = SteamUGC()->SetItemContent(WorkShopItemUpdateHandle, pMediaFolder);
	char pPreviewImageFile[MAX_PATH];
	sprintf (pPreviewImageFile, "%s\\Files\\%s", g.fpscrootdir_s.Get(), g_currentWorkshopItem.sImage.Get());
	GG_GetRealPath(pPreviewImageFile, false);
	if (FileExist(pPreviewImageFile) == 0)
	{
		sprintf (pPreviewImageFile, "%s\\Files\\editors\\uiv3\\hub-workshopitem.png", g.fpscrootdir_s.Get());
	}
	result = SteamUGC()->SetItemPreview(WorkShopItemUpdateHandle, pPreviewImageFile);

	// and submit to Steam now
	g_WorkshopUserPrompt = "Started the submission upload process for this item...";
	SteamAPICall_t ret = SteamUGC()->SubmitItemUpdate(WorkShopItemUpdateHandle, "Latest Version");
	m_SteamCallResultWorkshopItemUpdated.Set(ret, this, &CSteamUserGeneratedWorkshopItem::OnWorkshopItemUpdated);
}

void CSteamUserGeneratedWorkshopItem::OnWorkshopItemUpdated(SubmitItemUpdateResult_t* pCallback, bool bIOFailure)
{
	if (pCallback->m_eResult == k_EResultOK)
	{
		g_WorkshopUserPrompt = "Workshop item submitted successfully!";
		g_bUpdateWorkshopItemList = true;
	}
	else
	{
		// NOTE: a failed submission cannot be auto deleted, user needs to edit to complete submission or delete from steam client
		//SteamUGC()->DeleteItem() //To delete a Workshop item, you can call ISteamUGC::DeleteItem. Please note that this does not prompt the user and cannot be undone. (not exist?)

		// prompt the error and code
		char pErrorPrompt[MAX_PATH];
		sprintf(pErrorPrompt, "Workshop item failed to submit. Steam Fail Code: %d", pCallback->m_eResult);
		g_WorkshopUserPrompt = pErrorPrompt;
	}
}

void CSteamUserGeneratedWorkshopItem::RefreshItemsList()
{
	// clear last list
	g_workshopItemsList.clear();

	// create a new query (if MORE THAN 50, need to go through ALL PAGES!!)
	UGCQueryHandle_t handle;
	handle = SteamUGC()->CreateQueryUserUGCRequest(SteamUser()->GetSteamID().GetAccountID(), k_EUserUGCList_Published, k_EUGCMatchingUGCType_Items,
		k_EUserUGCListSortOrder_LastUpdatedDesc, SteamUtils()->GetAppID(), SteamUtils()->GetAppID(), 1);

	// also get other data too
	SteamUGC()->SetReturnKeyValueTags(handle, true);
	SteamUGC()->SetReturnMetadata(handle, true);

	// start the query
	SteamAPICall_t ret = SteamUGC()->SendQueryUGCRequest (handle);
	m_SteamCallResultWorkshopItemQueried.Set(ret, this, &CSteamUserGeneratedWorkshopItem::onWorkshopItemQueried);
}

void CSteamUserGeneratedWorkshopItem::onWorkshopItemQueried(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{
	if (pCallback->m_unNumResultsReturned > 0)
	{
		SteamUGCDetails_t details;
		bool found = false;
		for (unsigned int c = 0; c < pCallback->m_unNumResultsReturned; c++)
		{
			bool res = SteamUGC()->GetQueryUGCResult(pCallback->m_handle, c, &details);
			if (res == true && details.m_eResult == k_EResultOK)
			{
				if (details.m_eVisibility == k_ERemoteStoragePublishedFileVisibilityPublic && !details.m_bBanned)
				{
					// need extra data from this item
					char pImageFile[256];
					char pMediaFolder[256];
					strcpy(pImageFile, "");
					strcpy(pMediaFolder, "");
					int iKeyValueCount = SteamUGC()->GetQueryUGCNumKeyValueTags(pCallback->m_handle, c);
					for (int i = 0; i < iKeyValueCount; i++)
					{
						char pKeyName[256];
						char pKeyValue[256];
						SteamUGC()->GetQueryUGCKeyValueTag(pCallback->m_handle, c, i, pKeyName, 256, pKeyValue, 256);
						if (stricmp(pKeyName, "imagefile") == NULL) strcpy(pImageFile, pKeyValue);
						if (stricmp(pKeyName, "mediafolder") == NULL) strcpy(pMediaFolder, pKeyValue);
					}

					// no meta data
					char pNoMetaData[256];
					SteamUGC()->GetQueryUGCMetadata(pCallback->m_handle, c, pNoMetaData, 256);

					// add item into list for later use
					sWorkshopItem newitem;
					newitem.sImage = pImageFile;
					newitem.sName = details.m_rgchTitle;
					newitem.sDesc = details.m_rgchDescription;
					newitem.sMediaFolder = pMediaFolder;
					newitem.nPublishedFileId = details.m_nPublishedFileId;
					g_workshopItemsList.push_back(newitem);
				}
			}
		}
	}
}
