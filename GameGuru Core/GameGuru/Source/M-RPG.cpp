//----------------------------------------------------
//--- GAMEGURU - M-RPG
//----------------------------------------------------

// Includes 
#include "stdafx.h"
#include "gameguru.h"
#include "M-RPG.h"

// Globals
std::vector<cstr> g_collectionLabels;
std::vector<collectionItemType> g_collectionMasterList;
std::vector<collectionItemType> g_collectionList;

// Functions
void init_rpg_system(void)
{
	// clear collection list
	g_collectionMasterList.clear();
	g_collectionList.clear();
}

bool load_rpg_system(char* name)
{
	// out of the box mandatory labels
	g_collectionLabels.clear();
	g_collectionLabels.push_back("title");
	g_collectionLabels.push_back("profile");
	g_collectionLabels.push_back("image");
	g_collectionLabels.push_back("description");
	g_collectionLabels.push_back("cost");
	g_collectionLabels.push_back("value");
	g_collectionLabels.push_back("container");
	g_collectionLabels.push_back("ingredients");
	g_collectionLabels.push_back("style");

	// load in collection file (contains all items in all game levels)
	std::vector<cstr> g_localCollectionLabels;
	char collectionfilename[MAX_PATH];
	strcpy(collectionfilename, "projectbank\\");
	strcat(collectionfilename, name);
	strcat(collectionfilename, "\\collection - items.tsv");
	FILE* collectionFile = GG_fopen(collectionfilename, "r");
	if (collectionFile)
	{
		// read all lines in TAB DELIMITED FILE
		bool bPopulateLabels = true;
		while (!feof(collectionFile))
		{
			// read a line
			char theline[MAX_PATH];
			fgets(theline, MAX_PATH - 1, collectionFile);
			if (strlen(theline) > 0 && theline[strlen(theline) - 1] == '\n')
				theline[strlen(theline) - 1] = 0;

			// determine which list to fill
			collectionItemType item;
			item.iEntityID = 0;
			item.iEntityElementE = 0;
			if (bPopulateLabels == true)
			{
				// first line are all the labels
				g_localCollectionLabels.clear();
			}
			else
			{
				// remaining lines are the collection, prepopulate with correct number of them
				item.collectionFields.clear();
				item.collectionFields.push_back("");
				item.collectionFields.push_back("default");
				item.collectionFields.push_back("default");
				item.collectionFields.push_back("");
				item.collectionFields.push_back("10");
				item.collectionFields.push_back("5");
				item.collectionFields.push_back("shop");
				item.collectionFields.push_back("none");
				item.collectionFields.push_back("none");
				int iLAIndex = item.collectionFields.size();
				while (iLAIndex < g_collectionLabels.size())
				{
					item.collectionFields.push_back("none");
					iLAIndex++;
				}
			}

			// go through tab delimited fields
			int iColumnIndex = 0;
			char pTab[2]; pTab[0] = 9; pTab[1] = 0;
			const char* delimiter = pTab;
			char* token = std::strtok(theline, delimiter);
			while (token) 
			{
				if (bPopulateLabels == true)
				{
					// record local order of the labels from the import
					g_localCollectionLabels.push_back(token);

					// add unique ones to end of labels list
					bool bFoundThisOne = false;
					for (int la = 0; la < g_collectionLabels.size(); la++)
					{
						if (stricmp(g_collectionLabels[la].Get(), token) == NULL)
						{
							bFoundThisOne = true;
							break;
						}
					}
					if (bFoundThisOne == false)
					{
						// add to end of main list of labels
						g_collectionLabels.push_back(token);
					}
				}
				else
				{
					// add to correct location in item collection fields (respect main labels list, not local import)
					if (iColumnIndex < g_localCollectionLabels.size())
					{
						LPSTR pLabelAssociated = g_localCollectionLabels[iColumnIndex].Get();
						iColumnIndex++;
						for (int la = 0; la < g_collectionLabels.size(); la++)
						{
							if (stricmp(g_collectionLabels[la].Get(), pLabelAssociated) == NULL)
							{
								item.collectionFields[la] = token;
								break;
							}
						}
					}
				}
				token = std::strtok(nullptr, delimiter);
			}

			// add populated item to collection list
			if (bPopulateLabels == false && item.collectionFields.size()>2)
			{
				if (stricmp(item.collectionFields[0].Get(), "title") == NULL && stricmp(item.collectionFields[2].Get(), "image") == NULL)
				{
					// seems we have duplicated the header row, so ignore (title, profile, image, etc)
				}
				else
				{
					// quick sanity check, reset any corrupt entries for image (might be FPE from old tabbed files)
					LPSTR pImageEntry = item.collectionFields[2].Get();
					if (strnicmp(pImageEntry + strlen(pImageEntry) - 4, ".fpe", 4) == NULL)
					{
						// restore to default, thank you!
						item.collectionFields[2] = "default";
					}

					// real entry, add it
					g_collectionMasterList.push_back(item);
				}
			}

			// first line over
			bPopulateLabels = false;
		}
		fclose(collectionFile);
	}

	// make a copy to regular gaming list
	g_collectionList = g_collectionMasterList;

	// trigger initial filling of global list (better location for this somewhere)
	extern bool g_bRefreshGlobalList;
	g_bRefreshGlobalList = true;

	// success
	return true;
}

bool save_rpg_system(char* name, bool bIncludeELEFile)
{
	// nothing to save if no collection to save
	if (g_collectionLabels.size() == 0)
		return false;

	// save master collection in file (contains all items in all game levels)
	char collectionfilename[MAX_PATH];
	strcpy(collectionfilename, "projectbank\\");
	strcat(collectionfilename, name);
	strcat(collectionfilename, "\\collection - items.tsv");
	DeleteFileA(collectionfilename);
	FILE* collectionFile = GG_fopen(collectionfilename, "w");
	if (collectionFile)
	{
		// write all lines in TAB DELIMITED FILE
		char pTab[2]; pTab[0] = 9; pTab[1] = 0;
		char pCR[2]; pCR[0] = 10; pCR[1] = 0;
		char theline[MAX_PATH];

		// first write collection labels
		strcpy(theline, "");
		for (int l = 0; l < g_collectionLabels.size(); l++)
		{
			strcat(theline, g_collectionLabels[l].Get());
			strcat(theline, pTab);
		}
		// Got a crash here when creating a new game project
		int lineLength = strlen(theline);
		if (lineLength > 0)
		{
			theline[strlen(theline) - 1] = 0;
		}
		strcat(theline, pCR);
		fwrite (theline, strlen (theline) * sizeof (char), 1, collectionFile);

		// then for each item a line is created with all attribs - was g_collectionMasterList
		for (int i = 0; i < g_collectionList.size(); i++)
		{
			strcpy(theline, "");
			for (int l = 0; l < g_collectionList[i].collectionFields.size(); l++)
			{
				strcat(theline, g_collectionList[i].collectionFields[l].Get());
				strcat(theline, pTab);
			}
			theline[strlen(theline) - 1] = 0;
			strcat(theline, pCR);
			fwrite (theline, strlen (theline) * sizeof (char), 1, collectionFile);
		}
		fclose(collectionFile);
	}

	// also save an up to date copy of the needed elements
	if (bIncludeELEFile == true)
	{
		cstr storeoldELEfile = t.elementsfilename_s;
		char collectionELEfilename[MAX_PATH];
		strcpy(collectionELEfilename, "projectbank\\");
		strcat(collectionELEfilename, name);
		strcat(collectionELEfilename, "\\collection - items.ele");
		GG_GetRealPath(collectionELEfilename, 1);
		if (FileExist(collectionELEfilename) == 1) DeleteFileA(collectionELEfilename);
		t.elementsfilename_s = collectionELEfilename;
		int iEntitiesToSaveCount = g_collectionList.size();
		if (iEntitiesToSaveCount > g.entityelementlist) iEntitiesToSaveCount = g.entityelementlist;
		if (iEntitiesToSaveCount > 0)
		{
			entitytype* pStoreEntEle = new entitytype[iEntitiesToSaveCount];
			entitytype* pTempEntEle = new entitytype[iEntitiesToSaveCount];
			for (int storee = 0; storee < iEntitiesToSaveCount; storee++)
			{
				pStoreEntEle[storee] = t.entityelement[1 + storee];
			}
			for (int c = 0; c < iEntitiesToSaveCount; c++)
			{
				int sourcee = g_collectionList[c].iEntityElementE;
				if (sourcee > 0)
					pTempEntEle[c] = t.entityelement[sourcee];
				else
					pTempEntEle[c] = t.entityelement[0];
			}
			for (int e = 0; e < iEntitiesToSaveCount; e++)
			{
				t.entityelement[1 + e] = pTempEntEle[e];
			}
			int iStoreEntEleCount = g.entityelementlist;
			g.entityelementlist = iEntitiesToSaveCount;
			entity_saveelementsdata();
			for (int storee = 0; storee < iEntitiesToSaveCount; storee++)
			{
				t.entityelement[1 + storee] = pStoreEntEle[storee];
			}
			g.entityelementlist = iStoreEntEleCount;
			t.elementsfilename_s = storeoldELEfile;
			delete[] pStoreEntEle;
			delete[] pTempEntEle;
		}
	}

	// success
	return true;
}

cstr get_rpg_imagefinalfile(cstr entityfile)
{
	extern cstr BackBufferCacheName;
	bool CreateBackBufferCacheName(char* file, int width, int height);
	CreateBackBufferCacheName(entityfile.Get(), 512, 288);
	LPSTR pAbsPathToFile = BackBufferCacheName.Get();
	cstr pRootDir = g.fpscrootdir_s + "\\Files\\";
	char pIconFile[MAX_PATH];
	strcpy(pIconFile, pAbsPathToFile + strlen(pRootDir.Get()));
	pIconFile[strlen(pIconFile) - 4] = 0;
	strcat(pIconFile, ".png");
	cstr pFinalImgFile = pIconFile;
	if (FileExist(pFinalImgFile.Get()) == 0) pFinalImgFile = "imagebank\\HUD Library\\MAX\\object.png";
	return pFinalImgFile;
}

bool fill_rpg_item_defaults_passedin(collectionItemType* pItem, int entid, int e, LPSTR pPassedInTitle, LPSTR pPassedInImageFile)
{
	// only some entities can make an item
	int iAddThisItem = false;
	if (entid > 0 && e > 0)
	{
		if (t.entityprofile[entid].isweapon > 0) iAddThisItem = 1;
		if (t.entityelement[e].eleprof.iscollectable != 0) iAddThisItem = 2;
	}
	else
	{
		if (pPassedInTitle && pPassedInImageFile) iAddThisItem = 3;
	}
	if (iAddThisItem > 0)
	{
		pItem->iEntityID = entid;
		pItem->iEntityElementE = e;
		pItem->collectionFields.clear();
		for (int l = 0; l < g_collectionLabels.size(); l++)
		{
			int iKnownLabel = -1;
			LPSTR pLabel = g_collectionLabels[l].Get();
			if (stricmp(pLabel, "title") == NULL) iKnownLabel = 0;
			if (stricmp(pLabel, "profile") == NULL) iKnownLabel = 1;
			if (stricmp(pLabel, "image") == NULL) iKnownLabel = 2;
			if (stricmp(pLabel, "description") == NULL) iKnownLabel = 3;
			if (stricmp(pLabel, "cost") == NULL) iKnownLabel = 4;
			if (stricmp(pLabel, "value") == NULL) iKnownLabel = 5;
			if (stricmp(pLabel, "container") == NULL) iKnownLabel = 6;
			if (stricmp(pLabel, "ingredients") == NULL) iKnownLabel = 7;
			if (stricmp(pLabel, "style") == NULL) iKnownLabel = 8;
			if (iKnownLabel >= 0)
			{
				if (iKnownLabel == 0)
				{
					LPSTR pTitle = "";
					if (iAddThisItem == 1) pTitle = t.entityelement[e].eleprof.name_s.Get();
					if (iAddThisItem == 2) pTitle = t.entityelement[e].eleprof.name_s.Get();
					if (iAddThisItem == 3) pTitle = pPassedInTitle;
					pItem->collectionFields.push_back(pTitle);
				}
				if (iKnownLabel == 1)
				{
					pItem->collectionFields.push_back(t.entitybank_s[entid].Get());
				}
				if (iKnownLabel == 2)
				{
					cstr pFinalImgFile = "";
					cstr localiconfile = "";
					if (iAddThisItem == 1) localiconfile = cstr("gamecore\\guns\\") + t.entityprofile[entid].isweapon_s + cstr("\\item.png");
					if (iAddThisItem == 3) localiconfile = pPassedInImageFile;
					if (FileExist(localiconfile.Get()) == 1)
					{
						// use locally specified icon
						pFinalImgFile = localiconfile;
					}
					else
					{
						// use default out of the box icon
						cstr entityfile = t.entitybank_s[entid];
						pFinalImgFile = get_rpg_imagefinalfile(entityfile);
					}
					pItem->collectionFields.push_back(pFinalImgFile);
				}
				if (iKnownLabel == 3)
				{
					LPSTR pDesc = "";
					if (iAddThisItem == 1) pDesc = t.entityprofile[entid].collectable.description.Get();
					if (iAddThisItem == 2) pDesc = t.entityprofile[entid].collectable.description.Get();
					if (iAddThisItem == 3) pDesc = pPassedInTitle;
					pItem->collectionFields.push_back(pDesc);
				}
				if (iKnownLabel == 4) pItem->collectionFields.push_back(t.entityprofile[entid].collectable.cost);
				if (iKnownLabel == 5) pItem->collectionFields.push_back(t.entityprofile[entid].collectable.value);
				if (iKnownLabel == 6) pItem->collectionFields.push_back(t.entityprofile[entid].collectable.container.Get());
				if (iKnownLabel == 7) pItem->collectionFields.push_back(t.entityprofile[entid].collectable.ingredients.Get());
				if (iKnownLabel == 8) pItem->collectionFields.push_back(t.entityprofile[entid].collectable.style.Get());
			}
			else
			{
				// empty field
				pItem->collectionFields.push_back("none");
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool fill_rpg_item_defaults(collectionItemType* pItem, int entid, int e)
{
	return fill_rpg_item_defaults_passedin(pItem, entid, e, NULL, NULL);
}

bool refresh_collection_from_entities(void)
{
	// start with game project master list
	g_collectionList = g_collectionMasterList;

	// go through all entities and add per-level items to collection (weapons, objects marked as collectable but not in master list)
	for ( int e = 1; e <= g.entityelementlist; e++)
	{
		int entid = t.entityelement[e].bankindex;
		if (entid > 0)
		{
			// find things to add to collection
			collectionItemType item;
			bool bAddThisItem = fill_rpg_item_defaults(&item, entid, e);
			if (bAddThisItem == true)
			{
				// before we add, confirm this does not already exist
				bool bNewItemIsUnqiue = true;
				for (int n = 0; n < g_collectionList.size(); n++)
				{
					if (g_collectionList[n].collectionFields[0] == item.collectionFields[0])
					{
						bNewItemIsUnqiue = false;
						break;
					}
				}
				if (bNewItemIsUnqiue == true)
				{
					g_collectionList.push_back(item);
				}
			}
		}
	}

	// replace any default images with correct paths
	for (int n = 0; n < g_collectionList.size(); n++)
	{
		if (g_collectionList[n].collectionFields.size() > 0)
		{
			if (stricmp(g_collectionList[n].collectionFields[2].Get(), "default") == NULL)
			{
				cstr entityfile = "";
				LPSTR pFind = g_collectionList[n].collectionFields[0].Get();
				for (int ee = 1; ee < g.entityelementmax; ee++)
				{
					LPSTR pThisEnt = t.entityelement[ee].eleprof.name_s.Get();
					if (stricmp (pThisEnt, pFind) == NULL)
					{
						int entid = t.entityelement[ee].bankindex;
						entityfile = t.entitybank_s[entid].Get();
						break;
					}
				}
				if (strlen(entityfile.Get()) > 0)
				{
					g_collectionList[n].collectionFields[2] = get_rpg_imagefinalfile(entityfile);
				}
			}
		}
	}

	// associate all collection items with present entity profiles and element eleprof copy
	bool bNeedMoreEntitiesLoading = false;
	for (int n = 0; n < g_collectionList.size(); n++)
	{
		if (g_collectionList[n].collectionFields.size() > 1)
		{
			LPSTR pCollectionItemTitle = g_collectionList[n].collectionFields[0].Get();
			if (strlen(pCollectionItemTitle) > 0 )
			{
				bool bFoundAndAssignedE = false;
				for (int ee = 1; ee < g.entityelementmax; ee++)
				{
					if (stricmp(t.entityelement[ee].eleprof.name_s.Get(), pCollectionItemTitle) == NULL)
					{
						int entid = t.entityelement[ee].bankindex;
						if (stricmp(g_collectionList[n].collectionFields[1].Get(), "default") == NULL)
						{
							g_collectionList[n].collectionFields[1] = t.entitybank_s[entid];
						}
						g_collectionList[n].iEntityID = entid;
						g_collectionList[n].iEntityElementE = ee;
						bFoundAndAssignedE = true;
						break;
					}
				}
				if ( bFoundAndAssignedE == false ) bNeedMoreEntitiesLoading = true;
			}
		}
	}

	// for init on each game project, where is the INGAME HUD screen (can change as working in storyboard after initial opening)
	extern int FindLuaScreenNode(char* name);
	t.game.ingameHUDScreen = FindLuaScreenNode("HUD0");

	// return true if some entities missing during refresh (causes a load elsewhere)
	return bNeedMoreEntitiesLoading;
}

int find_rpg_collectionindex (char* pName)
{
	int collectionID = 0;
	for (int n = 0; n < g_collectionList.size(); n++)
	{
		if (g_collectionList[n].collectionFields.size() > 0)
		{
			if (stricmp(pName, g_collectionList[n].collectionFields[0].Get()) == NULL)
			{
				collectionID = 1 + n;
				break;
			}
		}
	}
	return collectionID;
}
