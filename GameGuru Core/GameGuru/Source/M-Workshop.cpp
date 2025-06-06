//----------------------------------------------------
//--- GAMEGURU - M-Workshop
//----------------------------------------------------

#include "gameguru.h"
#include "M-Workshop.h"
#include <ctime>

// Not For Educational Version
#ifndef GGMAXEDU

// Globals
bool g_bWorkshopAvailable = false;
bool g_bWorkshopTabOpen = false;
bool g_bUpdateWorkshopItemList = false;
bool g_bUpdateWorkshopDownloads = false;
bool g_bUpdateWorkshopDownloadsAlwaysPerformOnce = true;
bool g_bUpdateWorkshopDownloadsAlwaysPerformOnceAtLaunch = true;
cstr g_WorkshopUserPrompt = "";
std::vector<sWorkshopItem> g_workshopItemsList;
std::vector<sWorkshopSteamUserName> g_workshopSteamUserNames;
std::vector<PublishedFileId_t> g_workshopTrustedItems;
int g_iSelectedExistingWorkshopItem = -1;
sWorkshopItem g_currentWorkshopItem;
CSteamUserGeneratedWorkshopItem g_UserWorkShopItem;
int g_iCurrentMediaTypeForWorkshopItem = 1;
bool g_bStillDownloadingThings = true;
bool g_bStillDownloadingThingsWithDelay = true;
int g_iStillDownloadingThingsWithDelayTimer = 0;
std::vector<cstr> g_sStillDownloadingLog;
std::vector<cstr> g_sStillDownloadingLogTitle;
int g_iStillDownloadingLogCount = 0;
int g_iUnsubscribeByForce = 0;
bool g_bRequireRestartAfterUnsubByForce = false;

// Functions
void workshop_init (bool bLoggedIn)
{
	// general inits
	g_sStillDownloadingLog.clear();
	g_sStillDownloadingLogTitle.clear();
	g_iStillDownloadingLogCount = 0;

	// no workshop in free version!
	extern bool g_bFreeTrialVersion;
	if (g_bFreeTrialVersion == false)
	{
		g_bWorkshopAvailable = true;
	}

	g_bUpdateWorkshopItemList = true;
	g_WorkshopUserPrompt = "";
	g_workshopItemsList.clear();
	g_workshopSteamUserNames.clear();

	// current working item
	g_currentWorkshopItem.sImage = "";
	g_currentWorkshopItem.sName = "";
	g_currentWorkshopItem.sDesc = "";
	g_currentWorkshopItem.sMediaType = "";
	g_currentWorkshopItem.sMediaFolder = "";
	g_currentWorkshopItem.nPublishedFileId = 0;
	g_currentWorkshopItem.sSteamUserAccountID = "";
	g_currentWorkshopItem.bDownloadItemTriggered = false;
	g_currentWorkshopItem.iNumberOfFilesInWorkshopItem = 0;
	g_currentWorkshopItem.sLatestDateOfItem = "";

	// start the process of ensuring trusted items are subscribed to
	workshop_subscribetoalltrusteditems();
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
	g_currentWorkshopItem.sMediaType = "";
	g_currentWorkshopItem.sMediaFolder = "";
	g_currentWorkshopItem.nPublishedFileId = 0;
	g_currentWorkshopItem.sSteamUserAccountID = "";
	g_currentWorkshopItem.bDownloadItemTriggered = false;
	g_currentWorkshopItem.iNumberOfFilesInWorkshopItem = 0;	
	g_currentWorkshopItem.sLatestDateOfItem = "";
	g_WorkshopUserPrompt = "";
}

LPSTR workshop_getmediatypepath ( int mediatypevalue )
{
	if (mediatypevalue == 1) return "audiobank";
	if (mediatypevalue == 2) return "entitybank";
	if (mediatypevalue == 3) return "imagebank";
	if (mediatypevalue == 4) return "particlesbank";
	if (mediatypevalue == 5) return "scriptbank";
	if (mediatypevalue == 6) return "root";
	return "";
}

int workshop_getvaluefromtype (LPSTR mediatypestring)
{
	if (stricmp(mediatypestring, "audiobank") == NULL) return 1;
	if (stricmp(mediatypestring, "entitybank") == NULL) return 2;
	if (stricmp(mediatypestring, "imagebank") == NULL) return 3;
	if (stricmp(mediatypestring, "particlesbank") == NULL) return 4;
	if (stricmp(mediatypestring, "scriptbank") == NULL) return 5;
	if (stricmp(mediatypestring, "root") == NULL) return 6;
	return 0;
}

bool workshop_submit_item_check (void)
{
	// first check have all fields filled
	if (g_currentWorkshopItem.sName.Len() > 0 && g_currentWorkshopItem.sDesc.Len() > 0 && g_currentWorkshopItem.sMediaFolder.Len() > 0)
	{
		if (SteamUser())
		{
			// now check if workshop item already exists with this name for this user, if a new entry
			if (g_currentWorkshopItem.nPublishedFileId == 0)
			{
				bool bFoundMatch = false;
				for (int i = 0; i < g_workshopItemsList.size(); i++)
				{
					if (stricmp(g_workshopItemsList[i].sName.Get(), g_currentWorkshopItem.sName.Get()) == NULL)
					{
						bFoundMatch = true;
						break;
					}
				}
				if (bFoundMatch == true)
				{
					g_WorkshopUserPrompt = "A workshop item already exists with that name, choose another name.";
					return false;
				}
			}

			// this users steam ID for the folder
			uint32 uAccountID = SteamUser()->GetSteamID().GetAccountID();

			// check media folder created and valid
			char pMediaFolder[MAX_PATH];
			LPSTR pMediaTypePath = workshop_getmediatypepath(g_iCurrentMediaTypeForWorkshopItem);
			if (stricmp(g_currentWorkshopItem.sMediaType.Get(), "root") == NULL)
			{
				sprintf (pMediaFolder, "%s\\%s", g.fpscrootdir_s.Get(), g_currentWorkshopItem.sMediaFolder.Get());
				GG_GetRealPath(pMediaFolder, true); // done here as when workshiop author updates, they do so from their writables area!
			}
			else
			{
				sprintf (pMediaFolder, "%s\\Files\\%s\\Community\\%d\\%s", g.fpscrootdir_s.Get(), pMediaTypePath, uAccountID, g_currentWorkshopItem.sMediaFolder.Get());
				GG_GetRealPath(pMediaFolder, true);
			}
			if (PathExist(pMediaFolder) == 0)
			{
				if (stricmp(g_currentWorkshopItem.sMediaType.Get(), "root") == NULL)
					g_WorkshopUserPrompt = "Workshop item media folder not found, create it in your GameGuruMAX writables folder and add your files.";
				else
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
			g_WorkshopUserPrompt = "Log into your Steam Client Account in order to submit a workshop item!";
			return false;
		}
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

void workshop_update ( bool bRefreshIfFlagged )
{
	// called from main game loop and from workshop tabs in HUB (false=game loop, true=HUB workshop pages)
	if (bRefreshIfFlagged == true || g_bUpdateWorkshopDownloadsAlwaysPerformOnce==true )
	{
		// trigger workshop item list update
		if (g_bUpdateWorkshopItemList == true)
		{
			workshop_update_steamusernames(); // populate before refresh (in case offline and need stored names)
			g_UserWorkShopItem.RefreshItemsList();
			g_bUpdateWorkshopItemList = false;
		}

		// when workshop item update refreshed, do a check to see if anything needs downloading
		if (g_bUpdateWorkshopDownloads == true)
		{
			// go through all the user has subscribed to
			uint32 numSubscribed = SteamUGC()->GetNumSubscribedItems();
			PublishedFileId_t* pEntries = new PublishedFileId_t[numSubscribed];
			SteamUGC()->GetSubscribedItems(pEntries, numSubscribed);
			for (int i = 0; i < numSubscribed; i++)
			{
				// for each item found
				PublishedFileId_t thisItem = pEntries[i];
				uint32 unItemState = SteamUGC()->GetItemState(thisItem);
				if (unItemState == k_EItemStateInstalled | k_EItemStateSubscribed)
				{
					// item is marked as installed
					uint64 punSizeOnDisk = 0;
					char pchFolder[MAX_PATH];
					uint32 cchFolderSize = 0;
					uint32 punTimeStamp = 0;
					bool bInstalledFlag = SteamUGC()->GetItemInstallInfo(thisItem, &punSizeOnDisk, pchFolder, MAX_PATH, &punTimeStamp);
					if (bInstalledFlag == true)
					{
						// store orig folder
						char pOldDir[MAX_PATH];
						strcpy(pOldDir, GetDir());

						// count files in workshop item
						extern bool g_bNormalOperations;
						int iFilesInThisWorkshopItem = -1;
						char pItemFolderNameOnly[MAX_PATH];
						strcpy(pItemFolderNameOnly, "");
						for (int n = strlen(pchFolder) - 1; n > 0; n--)
						{
							if (pchFolder[n] == '\\' || pchFolder[n] == '/')
							{
								strcpy(pItemFolderNameOnly, pchFolder + n + 1);
								break;
							}
						}
						if (strlen(pItemFolderNameOnly) > 0)
						{
							SetDir(pchFolder);
							SetDir("..");
							g.filecollectionmax = 0;
							Dim (t.filecollection_s, 500);
							g_bNormalOperations = false;
							addallinfoldertocollection(pItemFolderNameOnly, pItemFolderNameOnly);
							g_bNormalOperations = true;
							iFilesInThisWorkshopItem = g.filecollectionmax;
						}

						// TEST downloading item to OWN steam user account (typically you already have your own there)
						bool bTestCopyOwnWorkshopItemsToo = false;// false; // true;

						// except if tbat is the current user who owns the item as that would overwrite the latest versions not yet submitted
						// we do not want to copy in this case so the author can continue working on the latest unbpublished media
						uint32 uAccountID = SteamUser()->GetSteamID().GetAccountID();

						// if final dest is however empty, trigger latest workshop content to be copied over
						// find same entry in workshop item list that contains the true path needed
						char pWorkshopItemTitle[MAX_PATH];
						strcpy(pWorkshopItemTitle, "");
						char pMediaFolder[MAX_PATH];
						bool bFindMatchInItemList = false;
						bool bTheFinalDestFolderIsEmpty = false;
						uint32 uFoundAccountIDAssociated = 0;
						INT iNumberOfFilesInLocalItem = -1;
						for (int j = 0; j < g_workshopItemsList.size(); j++)
						{
							if (g_workshopItemsList[j].nPublishedFileId == thisItem)
							{
								uFoundAccountIDAssociated = atoi(g_workshopItemsList[j].sSteamUserAccountID.Get());
								if (bTestCopyOwnWorkshopItemsToo == true || uAccountID != uFoundAccountIDAssociated)
								{
									LPSTR pAllMediaFolder = g_workshopItemsList[j].sMediaFolder.Get();
									char pLastFolder[MAX_PATH];
									strcpy(pLastFolder, pAllMediaFolder);
									for (int nn = strlen(pAllMediaFolder) - 2; nn > 0; nn--)
									{
										if (pAllMediaFolder[nn] == '\\' || pAllMediaFolder[nn] == '/')
										{
											strcpy(pLastFolder, pAllMediaFolder + nn + 1);
											break;
										}
									}
									if (stricmp(g_workshopItemsList[j].sMediaType.Get(), "root") == NULL)
									{
										// stay in root parent folder
										sprintf (pMediaFolder, "%s\\%s\\", g.fpscrootdir_s.Get(), g_workshopItemsList[j].sMediaFolder.Get());
									}
									else
									{
										// go to writables community folder
										sprintf (pMediaFolder, "%s\\Files\\%s\\Community\\%s\\%s\\", g.fpscrootdir_s.Get(), g_workshopItemsList[j].sMediaType.Get(), g_workshopItemsList[j].sSteamUserAccountID.Get(), g_workshopItemsList[j].sMediaFolder.Get());
										GG_GetRealPath(pMediaFolder, true);
									}
									if (PathExist(pMediaFolder) == 0)
									{
										char pReportThisError[MAX_PATH];
										sprintf(pReportThisError, "Tried to copy workshop content into '%s'", pMediaFolder);
										timestampactivity (0, pReportThisError);
										break;
									}
									else
									{
										SetDir(pMediaFolder);
										SetDir("..");
										g.filecollectionmax = 0;
										Dim (t.filecollection_s, 500);
										g_bNormalOperations = false;
										addallinfoldertocollection(pLastFolder, pLastFolder);
										g_bNormalOperations = true;
										bool bResetLocalContents = false;
										iNumberOfFilesInLocalItem = g.filecollectionmax;
										if (g_workshopItemsList[j].bDownloadItemTriggered == true) { bResetLocalContents = true; g_workshopItemsList[j].bDownloadItemTriggered = false; }
										if (iFilesInThisWorkshopItem != iNumberOfFilesInLocalItem)
										{
											// can trigger if EXTRA files not part of workshop item are present!
											if (stricmp(g_workshopItemsList[j].sMediaType.Get(), "root") != NULL)
											{
												// so only do if NOT a ROOT folder (which allows such things)
												bResetLocalContents = true;
											}
										}
										if (bResetLocalContents == true)
										{
											SetDir(pLastFolder);// g_workshopItemsList[j].sMediaFolder.Get());
											for (int fileindex = 1; fileindex <= g.filecollectionmax; fileindex++)
											{
												cstr name_s = t.filecollection_s[fileindex];
												if (FileExist(name_s.Get()) == 1)
												{
													DeleteFileA(name_s.Get());
												}
											}
											bTheFinalDestFolderIsEmpty = true;
										}
										else
										{
											if (g.filecollectionmax == 0)
											{
												bTheFinalDestFolderIsEmpty = true;
											}
										}
										g_workshopItemsList[j].iNumberOfFilesInWorkshopItem = iFilesInThisWorkshopItem;
										struct tm* ptm;
										time_t timetvalue = (time_t)punTimeStamp;
										ptm = gmtime(&timetvalue);
										char pFormatDate[MAX_PATH];
										cstr day_s = cstr(100 + ptm->tm_mday);
										cstr month_s = cstr(101 + ptm->tm_mon);
										cstr year_s = cstr(1900 + ptm->tm_year);
										sprintf(pFormatDate, "%s-%s-%s", day_s.Get() + 1, month_s.Get() + 1, year_s.Get());
										g_workshopItemsList[j].sLatestDateOfItem = pFormatDate;
										UnDim (t.filecollection_s);
										bFindMatchInItemList = true;
										strcpy(pWorkshopItemTitle, g_workshopItemsList[j].sName.Get());
										break;
									}
								}
							}
						}

						// very first time running MAX, will refresh from workshop storage
						if (g_bUpdateWorkshopDownloadsAlwaysPerformOnce == true) bTheFinalDestFolderIsEmpty = true;

						// if valid final destination
						if (bFindMatchInItemList == true && PathExist(pMediaFolder) == 1 && bTheFinalDestFolderIsEmpty == true)
						{
							// copy the files in the folder to the correct subfolder of the community users location
							// another user created this, or doing a test, so copy over to writables final destination
							if (strlen(pItemFolderNameOnly) > 0)
							{
								SetDir(pchFolder);
								SetDir("..");
								g.filecollectionmax = 0;
								Dim (t.filecollection_s, 500);
								g_bNormalOperations = false;
								addallinfoldertocollection(pItemFolderNameOnly, pItemFolderNameOnly);
								g_bNormalOperations = true;
								if (g.filecollectionmax > 0)
								{
									SetDir(pItemFolderNameOnly);
									for (int fileindex = 1; fileindex <= g.filecollectionmax; fileindex++)
									{
										cstr name_s = t.filecollection_s[fileindex];
										LPSTR pFilename = name_s.Get() + strlen(pItemFolderNameOnly) + 1;
										if (FileExist(pFilename) == 1)
										{
											if (g_bUpdateWorkshopDownloadsAlwaysPerformOnceAtLaunch == false)
											{
												g_sStillDownloadingLogTitle.push_back(cstr(pWorkshopItemTitle));
												g_sStillDownloadingLog.push_back(cstr(pFilename));
											}
											char pExist[MAX_PATH];
											char pNew[MAX_PATH];
											sprintf(pExist, "%s\\%s", pchFolder, pFilename);
											sprintf(pNew, "%s%s", pMediaFolder, pFilename);
											char pDestPath[MAX_PATH];
											strcpy(pDestPath, pNew);
											for (int n = strlen(pDestPath) - 1; n > 0; n--)
											{
												if (pDestPath[n] == '\\' || pDestPath[n] == '/')
												{
													pDestPath[n + 1] = 0;
													break;
												}
											}
											GG_CreatePath(pDestPath);
											//PE: OPTIMIZE Only copy if changed. 2-3 sec faster this way. its around 689 files 28mb currently.
											struct stat sb;
											time_t tSource = 0;
											time_t tDest = 1;
											if (stat(pExist, &sb) == 0)
											{
												tSource = sb.st_mtime;
												if (stat(pNew, &sb) == 0)
												{
													tDest = sb.st_mtime;
												}
											}
											if(tSource != tDest)
												CopyFileA(pExist, pNew, FALSE);
										}
									}
								}
								UnDim (t.filecollection_s);
							}
						}

						// restore orig dir
						SetDir(pOldDir);
					}
				}
				else if (unItemState & k_EItemStateDownloading)
				{
					// indicates the item is currently downloading to the client - do nothing for now - may report progress at some point
				}
			}
			delete pEntries;
			g_bUpdateWorkshopDownloads = false;
			g_bStillDownloadingThingsWithDelay = true;
			g_iStillDownloadingThingsWithDelayTimer = Timer();
		}

		// ensure once flag reset
		g_bUpdateWorkshopDownloadsAlwaysPerformOnce = false;
		g_bUpdateWorkshopDownloadsAlwaysPerformOnceAtLaunch = false;
	}

	// special unsubscribe forcer to hopefully overcome Steams inability to update to latest workship item content
	if ( g_iUnsubscribeByForce > 0 )
	{
		g_iUnsubscribeByForce++;
		if (g_iUnsubscribeByForce == 2)
		{
			timestampactivity(0, "g_UserWorkShopItem.UnsubscribeTrustedItems()");
			g_UserWorkShopItem.UnsubscribeTrustedItems();
		}
		else
		{
			if (g_iUnsubscribeByForce == 200)
			{
				// after a small pause, resubscribe to trusted items
				workshop_subscribetoalltrusteditems();
			}
			else
			{
				if (g_iUnsubscribeByForce > 1000)
				{
					g_iUnsubscribeByForce = 0;
				}
			}
		}
	}

	// run Steam callbacks (and also check subscription items and download if not installed/notupdated)
	g_UserWorkShopItem.SteamRunCallbacks();
}

void workshop_update_steamusernames (void)
{
	// scans latest workshop items and maintains latest steam user name
	// database for when user is not logged in but still needs persona names

	// initially init
	g_workshopSteamUserNames.clear();

	// file storing steam user name database
	char pWorkshopSteamUserDatabase[MAX_PATH];
	strcpy(pWorkshopSteamUserDatabase, "WorkshopSteamUserDatabase.ini");
	GG_GetRealPath(pWorkshopSteamUserDatabase, 1);

	// load any previous database
	if (FileExist(pWorkshopSteamUserDatabase) == 1)
	{
		OpenToRead(1, pWorkshopSteamUserDatabase);
		LPSTR pLine = "";
		int iVersionID = 0; pLine = ReadString (1); iVersionID = atoi(pLine);
		int numEntries = 0; pLine = ReadString (1); numEntries = atoi(pLine);
		for (int iUser = 0; iUser < numEntries; iUser++)
		{
			LPSTR pAccountID = ""; pLine = ReadString (1); pAccountID = pLine;
			LPSTR pAccountSteamUserName = ""; pLine = ReadString (1); pAccountSteamUserName = pLine;
			if (strlen(pAccountID) > 0 && strlen(pAccountSteamUserName) > 0)
			{
				sWorkshopSteamUserName newuser;
				newuser.sSteamUserAccountID = pAccountID;
				newuser.sSteamUsersPersonaName = pAccountSteamUserName;
				g_workshopSteamUserNames.push_back(newuser);
			}
		}
		CloseFile(1);
	}

	// scan workshop items for any new additions or changes
	for (int i = 0; i < g_workshopItemsList.size(); i++)
	{
		bool bFoundAcccountIDInDatabase = false;
		for (int j = 0; j < g_workshopSteamUserNames.size(); j++)
		{
			if (g_workshopSteamUserNames[j].sSteamUserAccountID == g_workshopItemsList[i].sSteamUserAccountID)
			{
				// change to latest one
				g_workshopSteamUserNames[j].sSteamUsersPersonaName = g_workshopItemsList[i].sSteamUsersPersonaName;
				bFoundAcccountIDInDatabase = true;
				break;
			}
		}
		if (bFoundAcccountIDInDatabase == false)
		{
			// add new steam user to database
			sWorkshopSteamUserName newuser;
			newuser.sSteamUserAccountID = g_workshopItemsList[i].sSteamUserAccountID;
			newuser.sSteamUsersPersonaName = g_workshopItemsList[i].sSteamUsersPersonaName;
			g_workshopSteamUserNames.push_back(newuser);
		}
	}

	// save latest database out
	if (FileExist(pWorkshopSteamUserDatabase) == 1) DeleteFileA(pWorkshopSteamUserDatabase);
	if (FileExist(pWorkshopSteamUserDatabase) == 0)
	{
		OpenToWrite(1, pWorkshopSteamUserDatabase);
		LPSTR pLine = "";
		WriteString(1, "1");
		char pNumUsers[256]; sprintf(pNumUsers, "%d", g_workshopSteamUserNames.size()); WriteString(1, pNumUsers);
		for (int iUser = 0; iUser < g_workshopSteamUserNames.size(); iUser++)
		{
			WriteString(1, g_workshopSteamUserNames[iUser].sSteamUserAccountID.Get());
			WriteString(1, g_workshopSteamUserNames[iUser].sSteamUsersPersonaName.Get());
		}
		CloseFile(1);
	}
}

void workshop_subscribetoalltrusteditems (void)
{
	// init trusted item list
	timestampactivity(0, "workshop_subscribetoalltrusteditems");
	g_workshopTrustedItems.clear();
	
	// file storing steam user name database
	char pWorkshopItemsTrustedSources[MAX_PATH];
	strcpy(pWorkshopItemsTrustedSources, "WorkshopTrustedItems.ini");

	// load trusted items IDs
	timestampactivity(0, pWorkshopItemsTrustedSources);
	if (FileExist(pWorkshopItemsTrustedSources) == 1)
	{
		OpenToRead(1, pWorkshopItemsTrustedSources);
		bool bKeepReadingLines = true;
		while(bKeepReadingLines)
		{
			LPSTR pLineTrustedItem = ReadString (1);
			if (strlen(pLineTrustedItem) > 0)
			{
				timestampactivity(0, pLineTrustedItem);
				g_workshopTrustedItems.push_back(_atoi64(pLineTrustedItem));
			}
			else
				bKeepReadingLines = false;
		}
		CloseFile(1);
	}
}

cstr workshop_findtrustedreplacement(LPSTR pCoreScriptFile)
{
	cstr result_s = "";
	for (int i = 0; i < g_workshopTrustedItems.size(); i++)
	{
		PublishedFileId_t thisItem = g_workshopTrustedItems[i];
		if (thisItem > 0)
		{
			for (int j = 0; j < g_workshopItemsList.size(); j++)
			{
				if (g_workshopItemsList[j].nPublishedFileId == thisItem)
				{
					result_s = cstr("Community\\") + g_workshopItemsList[j].sSteamUserAccountID + cstr("\\") + g_workshopItemsList[j].sMediaFolder + cstr("\\") + cstr(pCoreScriptFile);
					return result_s;
				}
			}
		}
	}
	return result_s;
}

//PE: Im getting a strange crash here witout OPTICK_ENABLE ? , moved this to m-entity.cpp fixed it ?
bool workshop_verifyandorreplacescript2 ( int e, int entid )
{
	return false;
	/* no longer support duplicate scripts in core and workshop, was confusing in the end!
	#ifndef OPTICK_ENABLE
	bool bReplacedScript = false;
	char pScriptFile[MAX_PATH];
	strcpy(pScriptFile, "scriptbank\\");
	if (entid > 0)
		strcat(pScriptFile, t.entityprofile[entid].aimain_s.Get());
	else
		strcat(pScriptFile, t.entityelement[e].eleprof.aimain_s.Get());
	GG_GetRealPath(pScriptFile, false);
	if (FileExist(pScriptFile) == 0)
	{
		// was missing script a core file
		if (strnicmp(pScriptFile, g.fpscrootdir_s.Get(), strlen(g.fpscrootdir_s.Get())) == NULL)
		{
			// can only do verify replace if Steam Client active and have the list to hand
			if(g_workshopItemsList.size() == 0 )
			{
				// No Steam Client - warn user the object they dropped in has an outdated script
				extern bool bTriggerMessage;
				extern int iTriggerMessageDelay;
				extern char cTriggerMessage[MAX_PATH];
				strcpy(cTriggerMessage, "The behaviour for this object is out of date, you need to log into Steam Client to obtain the latest version.");
				iTriggerMessageDelay = 10;
				bTriggerMessage = true;
			}
			else
			{
				// yes, now check for tristed replacement
				cstr trustedReplacement_s = "";
				if (entid > 0)
					trustedReplacement_s = workshop_findtrustedreplacement(t.entityprofile[entid].aimain_s.Get());
				else
					trustedReplacement_s = workshop_findtrustedreplacement(t.entityelement[e].eleprof.aimain_s.Get());
				if (trustedReplacement_s.Len() > 0)
				{
					strcpy(pScriptFile, "scriptbank\\");
					strcat(pScriptFile, trustedReplacement_s.Get());
					GG_GetRealPath(pScriptFile, false);
					if (FileExist(pScriptFile) == 1)
					{
						if (entid > 0)
							t.entityprofile[entid].aimain_s = trustedReplacement_s;
						else
							t.entityelement[e].eleprof.aimain_s = trustedReplacement_s;

						bReplacedScript = true;
					}
				}
			}
		}
	}
	return bReplacedScript;
	#else
	return false;
	#endif
	*/
}

// Callback Functions for Steam Workshop
CSteamUserGeneratedWorkshopItem::CSteamUserGeneratedWorkshopItem()
{
}

CSteamUserGeneratedWorkshopItem::~CSteamUserGeneratedWorkshopItem()
{
}

void CSteamUserGeneratedWorkshopItem::UnsubscribeTrustedItems()
{
	// we do this because it seems Steam sometimes does not auto update workshop item content
	// (users are resorting to VERIFY FILES and unsubbing and resubbing to resolve this!!)
	// so need to force it to unsubscribe when the update button is clicked!
	for (int i = 0; i < g_workshopTrustedItems.size(); i++)
	{
		PublishedFileId_t thisItem = g_workshopTrustedItems[i];
		if (thisItem > 0)
		{

			SteamUGC()->UnsubscribeItem(thisItem);
		}
	}
}

int g_iFiveOnOffTimer = 0;
int g_iFiveOnOffTimerState = 0;
int g_iOnlyOneSubscripe = 0;
int g_iLastSubscripe = 0;
void CSteamUserGeneratedWorkshopItem::SteamRunCallbacks()
{
	// only do steam update if logged into Steam
	ISteamUtils* pSteamUtilsPtr = SteamUtils();
	if (pSteamUtilsPtr)
	{
		SteamAPI_RunCallbacks();
	}

	//PE: Got huge stutter from this, mainly from microsoft "defender" that run each time you download.
	//PE: While runnings it lower fps from 60 -> 18 fps, this keep on in 5 sec. intervals, but never stop again.
	//PE: Steam problem ? , anyway make it timout if it keep failing.
	if (g_bWorkshopTabOpen)
	{
		//PE: Keep running when workshop tab is open.
		g_iOnlyOneSubscripe = 0;
	}
	if (g_iOnlyOneSubscripe > 5)
	{
		//PE: Max 6 tries , then ignore. 30 sec.
		return;
	}

	// add a time out so callbacks can do there thing (five seconds on five off)
	extern bool g_bFreeTrialVersion;
	if (g_bFreeTrialVersion == false)
	{
		if (g_iFiveOnOffTimer < Timer())
		{
			g_iFiveOnOffTimerState = 1 - g_iFiveOnOffTimerState;
			g_iFiveOnOffTimer = Timer() + 5000;

			if (g_iLastSubscripe == 1)
			{
				g_iOnlyOneSubscripe++;
			}
		}
		if (g_iFiveOnOffTimerState == 0)
		{
			// ensure workshop items not installed or in need of updating are given the download item command
			if (SteamUGC())
			{
				g_bStillDownloadingThings = false;
				uint32 numSubscribed = SteamUGC()->GetNumSubscribedItems();
				//PE: Sometimes 1 is returned here, so something wrong, and it keep downloading items.
				g_iLastSubscripe = numSubscribed;
				PublishedFileId_t* pEntries = new PublishedFileId_t[numSubscribed];
				SteamUGC()->GetSubscribedItems(pEntries, numSubscribed);
				for (int i = 0; i < numSubscribed; i++)
				{
					// for each item found
					PublishedFileId_t thisItem = pEntries[i];
					uint32 unItemState = SteamUGC()->GetItemState(thisItem);
					if (!(unItemState & k_EItemStateInstalled) || unItemState & k_EItemStateNeedsUpdate)
					{
						if (SteamUGC()->DownloadItem (thisItem, true) == true)
						{
							// downloaddone callback not in API, instead wait until item status reads as installed
							for (int i = 0; i < g_workshopItemsList.size(); i++)
							{
								if (g_workshopItemsList[i].nPublishedFileId == thisItem)
								{
									g_workshopItemsList[i].bDownloadItemTriggered = true;
									break;
								}
							}
						}
					}
					if (unItemState == k_EItemStateInstalled | k_EItemStateSubscribed)
					{
						// this item is ready to use
					}
					else
					{
						// this item is not ready to use, we are still busy downloading things
						g_bStillDownloadingThings = true;
						g_bStillDownloadingThingsWithDelay = true;
						g_iStillDownloadingThingsWithDelayTimer = Timer();
					}
				}

				// check for any items not subcribed, but need to be
				for (int i = 0; i < g_workshopTrustedItems.size(); i++)
				{
					PublishedFileId_t thisItem = g_workshopTrustedItems[i];
					if (thisItem > 0)
					{
						bool bAlreadySubscribed = false;
						for (int i = 0; i < numSubscribed; i++)
						{
							if (pEntries[i] == thisItem)
							{
								bAlreadySubscribed = true;
								break;
							}
						}
						if (bAlreadySubscribed == false)
						{
							uint32 unItemState = SteamUGC()->GetItemState(thisItem);
							char pSubscribeAttempt[MAX_PATH];
							sprintf(pSubscribeAttempt, "Subscribing to trusted item unItemState=%d", (int)unItemState);
							timestampactivity(0, pSubscribeAttempt);
							if (unItemState == k_EItemStateNone || unItemState == k_EItemStateInstalled ) // k_EItemStateInstalled in case of stale item state!
							{
								// item is not subscribed to, so subscrube to trusted item
								SteamUGC()->SubscribeItem(thisItem);
								timestampactivity(0, ">SteamUGC()->SubscribeItem");
								if (SteamUGC()->DownloadItem (thisItem, true) == true)
								{
									timestampactivity(0, ">SteamUGC()->DownloadItem");
									g_bStillDownloadingThings = true;
									g_bStillDownloadingThingsWithDelay = true;
									g_iStillDownloadingThingsWithDelayTimer = Timer();
								}
							}
						}
					}
				}

				// free resource
				if (pEntries)
				{
					delete pEntries;
					pEntries = 0;
				}
			}
		}
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
		// start populating item once created
		OnWorkshopItemStartUpdate(pCallback->m_nPublishedFileId);

		// always subscribe to items you create 
		SteamUGC()->SubscribeItem(pCallback->m_nPublishedFileId);
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
	char pMetaDataSteamUsersCreatorsName[256];
	strcpy (pMetaDataSteamUsersCreatorsName, SteamFriends()->GetPersonaName());
	result = SteamUGC()->SetItemMetadata(WorkShopItemUpdateHandle, pMetaDataSteamUsersCreatorsName);

	// extra data we need when retrieving item details
	result = SteamUGC()->RemoveItemKeyValueTags(WorkShopItemUpdateHandle, "imagefile");
	result = SteamUGC()->RemoveItemKeyValueTags(WorkShopItemUpdateHandle, "mediatype");
	result = SteamUGC()->RemoveItemKeyValueTags(WorkShopItemUpdateHandle, "mediafolder");
	result = SteamUGC()->AddItemKeyValueTag(WorkShopItemUpdateHandle, "imagefile", g_currentWorkshopItem.sImage.Get());
	result = SteamUGC()->AddItemKeyValueTag(WorkShopItemUpdateHandle, "mediatype", g_currentWorkshopItem.sMediaType.Get());
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

	// media folder
	char pMediaFolder[MAX_PATH];
	if (stricmp(g_currentWorkshopItem.sMediaType.Get(), "root") == NULL)
		sprintf (pMediaFolder, "%s\\%s", g.fpscrootdir_s.Get(), g_currentWorkshopItem.sMediaFolder.Get());
	else
		sprintf (pMediaFolder, "%s\\Files\\%s\\Community\\%d\\%s", g.fpscrootdir_s.Get(), g_currentWorkshopItem.sMediaType.Get(), uAccountID, g_currentWorkshopItem.sMediaFolder.Get());

	GG_GetRealPath(pMediaFolder, true); // done here as when workshiop author updates, they do so from their writables area!
	result = SteamUGC()->SetItemContent(WorkShopItemUpdateHandle, pMediaFolder);

	// preview image
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

UGCQueryHandle_t g_UGCQueryHandle = NULL;

void CSteamUserGeneratedWorkshopItem::RefreshItemsList()
{
	// clear last list
	timestampactivity(0, "CSteamUserGeneratedWorkshopItem::RefreshItemsList()");
	g_workshopItemsList.clear();

	// create a new query (if MORE THAN 50, need to go through ALL PAGES!!)
	if (SteamUGC())
	{
		if (g_UGCQueryHandle == NULL)
		{
			g_UGCQueryHandle = SteamUGC()->CreateQueryUserUGCRequest(SteamUser()->GetSteamID().GetAccountID(), k_EUserUGCList_Subscribed, k_EUGCMatchingUGCType_Items,
				k_EUserUGCListSortOrder_LastUpdatedDesc, SteamUtils()->GetAppID(), SteamUtils()->GetAppID(), 1);
		}

		// also get other data too
		SteamUGC()->SetReturnKeyValueTags(g_UGCQueryHandle, true);
		SteamUGC()->SetReturnMetadata(g_UGCQueryHandle, true);

		// start the query
		SteamAPICall_t ret = SteamUGC()->SendQueryUGCRequest (g_UGCQueryHandle);
		m_SteamCallResultWorkshopItemQueried.Set(ret, this, &CSteamUserGeneratedWorkshopItem::onWorkshopItemQueried);
	}
	else
	{
		g_WorkshopUserPrompt = "Log into your Steam Client Account in order to upload to the workshop!";
	}
}

void CSteamUserGeneratedWorkshopItem::onWorkshopItemQueried(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{
	timestampactivity(0, "CSteamUserGeneratedWorkshopItem::onWorkshopItemQueried");
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
					char pMediaType[256];
					char pMediaFolder[256];
					strcpy(pImageFile, "");
					strcpy(pMediaType, "");
					strcpy(pMediaFolder, "");
					int iKeyValueCount = SteamUGC()->GetQueryUGCNumKeyValueTags(pCallback->m_handle, c);
					for (int i = 0; i < iKeyValueCount; i++)
					{
						char pKeyName[256];
						char pKeyValue[256];
						SteamUGC()->GetQueryUGCKeyValueTag(pCallback->m_handle, c, i, pKeyName, 256, pKeyValue, 256);
						if (stricmp(pKeyName, "imagefile") == NULL) strcpy(pImageFile, pKeyValue);
						if (stricmp(pKeyName, "mediatype") == NULL) strcpy(pMediaType, pKeyValue);
						if (stricmp(pKeyName, "mediafolder") == NULL) strcpy(pMediaFolder, pKeyValue);
					}

					// no meta data
					char pMetaDataSteamUsersCreatorsName[256];
					SteamUGC()->GetQueryUGCMetadata(pCallback->m_handle, c, pMetaDataSteamUsersCreatorsName, 256);

					// add item into list for later use
					sWorkshopItem newitem;
					newitem.sImage = pImageFile;
					newitem.sName = details.m_rgchTitle;
					newitem.sDesc = details.m_rgchDescription;
					newitem.sMediaType = pMediaType;
					newitem.sMediaFolder = pMediaFolder;
					newitem.nPublishedFileId = details.m_nPublishedFileId;
					newitem.sSteamUsersPersonaName = pMetaDataSteamUsersCreatorsName;
					char pSteamUserAccountID[256];
					CSteamID pSteamUserID = details.m_ulSteamIDOwner;
					sprintf(pSteamUserAccountID, "%d", pSteamUserID.GetAccountID());
					newitem.sSteamUserAccountID = pSteamUserAccountID;
					newitem.bDownloadItemTriggered = false;
					newitem.iNumberOfFilesInWorkshopItem = 0;
					newitem.sLatestDateOfItem = "";
					timestampactivity(0, newitem.sName.Get());
					g_workshopItemsList.push_back(newitem);
				}
			}
		}

		// free query handle now done
		if (g_UGCQueryHandle != NULL)
		{
			SteamUGC()->ReleaseQueryUGCRequest(g_UGCQueryHandle);
			g_UGCQueryHandle = NULL;
		}
	}

	// and update steam user database once all workship items obtained
	workshop_update_steamusernames();

	// once we know all items refreshed, see if we need to download any
	g_bUpdateWorkshopDownloads = true;
}

#endif // GGMAXEDU