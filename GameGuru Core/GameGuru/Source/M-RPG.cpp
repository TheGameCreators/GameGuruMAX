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
	// load in collection file (contains all items in all game levels)
	char collectionfilename[MAX_PATH];
	strcpy(collectionfilename, "projectbank\\");
	strcat(collectionfilename, name);
	strcat(collectionfilename, "\\collection - items.tsv");
	FILE* collectionFile = GG_fopen(collectionfilename, "r");
	if (collectionFile)
	{
		// read all lines in TAB DELIMITED FILE
		while (!feof(collectionFile))
		{
			// read a line
			char theline[MAX_PATH];
			fgets(theline, MAX_PATH - 1, collectionFile);
			if (strlen(theline) > 0 && theline[strlen(theline) - 1] == '\n')
				theline[strlen(theline) - 1] = 0;

			// determine which list to fill
			collectionItemType item;
			bool bPopulateLabels = false;
			if (g_collectionLabels.size() == 0)
			{
				// first line contains the collection labels (title,image,desc,cost)
				bPopulateLabels = true;
			}
			else
			{
				// remaining lines are the collection
				item.collectionFields.clear();
			}

			// go through tab delimited fields
			char pTab[2]; pTab[0] = 9; pTab[1] = 0;
			const char* delimiter = pTab;
			char* token = std::strtok(theline, delimiter);
			while (token) 
			{
				if (bPopulateLabels == true)
				{
					// forms the full list of attributes per item
					g_collectionLabels.push_back(token);
				}
				else
				{
					item.collectionFields.push_back(token);
				}
				token = std::strtok(nullptr, delimiter);
			}

			// add populated item to collection list
			if (bPopulateLabels == false)
			{
				if (item.collectionFields.size() > 2)
				{
					g_collectionMasterList.push_back(item);
				}
			}
		}
		fclose(collectionFile);
	}
	else
	{
		// if not user defined collection, out of the box fields
		g_collectionLabels.clear();
		g_collectionLabels.push_back("title");
		g_collectionLabels.push_back("image");
		g_collectionLabels.push_back("description");
		g_collectionLabels.push_back("cost");
		g_collectionLabels.push_back("value");
		g_collectionLabels.push_back("container");
		g_collectionLabels.push_back("ingredients");
		g_collectionLabels.push_back("style");
	}

	// make a copy to regular gaming list
	g_collectionList = g_collectionMasterList;

	// trigger initial filling of global list (better location for this somewhere)
	extern bool g_bRefreshGlobalList;
	g_bRefreshGlobalList = true;

	// success
	return true;
}

bool save_rpg_system(char* name)
{
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

		// then for each item a line is created with all attribs
		for (int i = 0; i < g_collectionMasterList.size(); i++)
		{
			strcpy(theline, "");
			for (int l = 0; l < g_collectionMasterList[i].collectionFields.size(); l++)
			{
				strcat(theline, g_collectionMasterList[i].collectionFields[l].Get());
				strcat(theline, pTab);
			}
			theline[strlen(theline) - 1] = 0;
			strcat(theline, pCR);
			fwrite (theline, strlen (theline) * sizeof (char), 1, collectionFile);
		}
		fclose(collectionFile);
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
		pItem->collectionFields.clear();
		for (int l = 0; l < g_collectionLabels.size(); l++)
		{
			int iKnownLabel = 0;
			LPSTR pLabel = g_collectionLabels[l].Get();
			if (stricmp(pLabel, "title") == NULL) iKnownLabel = 1;
			if (stricmp(pLabel, "image") == NULL) iKnownLabel = 2;
			if (stricmp(pLabel, "description") == NULL) iKnownLabel = 3;
			if (stricmp(pLabel, "cost") == NULL) iKnownLabel = 4;
			if (stricmp(pLabel, "value") == NULL) iKnownLabel = 5;
			if (stricmp(pLabel, "container") == NULL) iKnownLabel = 6;
			if (stricmp(pLabel, "ingredients") == NULL) iKnownLabel = 7;
			if (stricmp(pLabel, "style") == NULL) iKnownLabel = 8;
			if (iKnownLabel > 0)
			{
				if (iKnownLabel == 1)
				{
					LPSTR pTitle = "";
					if (iAddThisItem == 1) pTitle = t.entityelement[e].eleprof.name_s.Get();
					if (iAddThisItem == 2) pTitle = t.entityelement[e].eleprof.name_s.Get();
					if (iAddThisItem == 3) pTitle = pPassedInTitle;
					pItem->collectionFields.push_back(pTitle);
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
					if (iAddThisItem == 1) pDesc = t.entityelement[e].eleprof.name_s.Get();
					if (iAddThisItem == 2) pDesc = t.entityelement[e].eleprof.name_s.Get();
					if (iAddThisItem == 3) pDesc = pPassedInTitle;
					pItem->collectionFields.push_back(pDesc);
				}
				if (iKnownLabel == 4) pItem->collectionFields.push_back(10);
				if (iKnownLabel == 5) pItem->collectionFields.push_back(5);
				if (iKnownLabel == 6) pItem->collectionFields.push_back("shop");
				if (iKnownLabel == 7) pItem->collectionFields.push_back("none");
				if (iKnownLabel == 8) pItem->collectionFields.push_back("none");
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
			if (stricmp(g_collectionList[n].collectionFields[1].Get(), "default") == NULL)
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
				/* only finds original parent names, need renamed ones the level, i.e. recipe
				for (int entid = 1; entid < t.entitybank_s.size(); entid++)
				{
					LPSTR pThisEnt = t.entitybank_s[entid].Get();
					if (strnicmp (pThisEnt + strlen(pThisEnt) - strlen(pFind) - 4, pFind, strlen(pFind)) == NULL)
					{
						entityfile = pThisEnt;
						break;
					}
				}
				*/
				if (strlen(entityfile.Get()) > 0)
				{
					g_collectionList[n].collectionFields[1] = get_rpg_imagefinalfile(entityfile);
				}
			}
		}
	}

	// for init on each game project, where is the INGAME HUD screen (can change as working in storyboard after initial opening)
	extern int FindLuaScreenNode(char* name);
	t.game.ingameHUDScreen = FindLuaScreenNode("HUD0");

	// success
	return true;
}

/*
bool add_collection_internal(char* pTitle, char* pImage, char* pDesc)
{
	collectionItemType item;
	item.collectionFields.clear();
	for (int l = 0; l < g_collectionLabels.size(); l++)
	{
		int iKnownLabel = 0;
		LPSTR pLabel = g_collectionLabels[l].Get();
		if (stricmp(pLabel, "title") == NULL) iKnownLabel = 1;
		if (stricmp(pLabel, "image") == NULL) iKnownLabel = 2;
		if (stricmp(pLabel, "description") == NULL) iKnownLabel = 3;
		if (stricmp(pLabel, "cost") == NULL) iKnownLabel = 4;
		if (stricmp(pLabel, "value") == NULL) iKnownLabel = 5;
		if (stricmp(pLabel, "container") == NULL) iKnownLabel = 6;
		if (stricmp(pLabel, "ingredients") == NULL) iKnownLabel = 7;
		if (stricmp(pLabel, "style") == NULL) iKnownLabel = 8;
		if (iKnownLabel > 0)
		{
			// field we can populate automatically
			if (iKnownLabel == 1) item.collectionFields.push_back(pTitle);
			if (iKnownLabel == 2) item.collectionFields.push_back(pImage);
			if (iKnownLabel == 3) item.collectionFields.push_back(pDesc);
			if (iKnownLabel == 4) item.collectionFields.push_back("10");
			if (iKnownLabel == 5) item.collectionFields.push_back("5");
			if (iKnownLabel == 6) item.collectionFields.push_back("shop");
			if (iKnownLabel == 7) item.collectionFields.push_back("none");
			if (iKnownLabel == 8) item.collectionFields.push_back("none");
		}
		else
		{
			// empty field
			item.collectionFields.push_back("none");
		}
	}
	g_collectionList.push_back(item);
	return true;
}
*/

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


//##################
//##### OLD RPG ####
//##################

#ifdef RPG_GAMES
void ProcessRPGSetupWindow(void);
bool IsRPGActive(void);
bool save_rpg_system(char* name);
bool load_rpg_system(char* name);
void init_rpg_system(void);
bool bRPGSetup_Window = false;
#endif

#ifdef RPG_GAMES

//PE: Structures to be moved later.

//PE: Need limits to make sure we can save directly and are able to use IDs directly, no pointer to pointer to pointer coding nightmare.
//PE: We work with floats everywhere but only display integers. (always round up).
//PE: Each struct get saved into its own file, so its easy to expand each struct later. Saved with fpm level.

#define RPG_MAX_STATS 150
#define RPG_STATS_ACTION_NONE 0
#define RPG_STATS_ACTION_MONEY 1
#define RPG_STATS_ACTION_HEALTH 2
#define RPG_STATS_ACTION_ARMOR 3
#define RPG_STATS_ACTION_STRENGTH 4
#define RPG_STATS_ACTION_STAMINA 5
#define RPG_STATS_ACTION_MAGIC 6

struct RPGStats
{
	int StatID[RPG_MAX_STATS];
	int StatAction[RPG_MAX_STATS]; //Predefined action to change ingame settings , like play Health,Armor,Ammo...
	float StatMax[RPG_MAX_STATS]; //the max value for this stat.
	char StatName[RPG_MAX_STATS][256];
	float StatValue[RPG_MAX_STATS];
} RPG_Stats;

//PE: Used to group items into categories, like weapon,armor,ammo,consumable,resource,recipe,quest
//PE: Also used as filter names.
#define RPG_MAX_TYPES 50
struct RPGTypes
{
	int TypeID[RPG_MAX_TYPES];
	char TypeName[RPG_MAX_STATS][256];
} RPG_Types;

#define RPG_MAX_ITEMS 1000
struct RPGItems
{
	int ItemID[RPG_MAX_ITEMS]; //ID
	char ItemName[RPG_MAX_ITEMS][256]; //Name
	char ItemDesc[RPG_MAX_ITEMS][256]; //Description
	char ItemIconLarge[RPG_MAX_ITEMS][256]; //Large icon path.
	int ItemIconLargeID[RPG_MAX_ITEMS]; //Large icon ID
	char ItemIconSmall[RPG_MAX_ITEMS][256]; //Small icon path.
	int ItemIconSmallID[RPG_MAX_ITEMS]; //Small icon ID

	float ItemWeight[RPG_MAX_ITEMS]; // Weight.

	//Prices.
	float ItemBuyPrice[RPG_MAX_ITEMS]; //Fixed to stat money.
	float ItemSellPrice[RPG_MAX_ITEMS];
	float ItemTradePrice[RPG_MAX_ITEMS];

	//Relation to objects on map. To easy the setup, all objects on map will get this item added to them.
	int ItemMasterObjectID[RPG_MAX_ITEMS]; //entityprofile[] - All object of this type in the level will trigger this item.
	char ItemMasterObjectFPE[RPG_MAX_ITEMS][256]; //We need the master object fpe, if we change level we need to match ItemMasterObjectID to the FPE on that level.

	int ItemTypeID[RPG_MAX_ITEMS]; //Lookup in other struct (weapon,armor,ammo,consumable,resource,recipe,quest,"Stat"/"none") ("none" for item that dont go into inv. but count stat... or other actions ).
	float ItemTypeValue[RPG_MAX_ITEMS]; // Like Armor 23, Ammo 50 ...

	//Item can make stat changes. also minus.
	int ItemStatID[RPG_MAX_ITEMS][RPG_MAX_STATS]; //Stat ID to change.
	float ItemStatValue[RPG_MAX_ITEMS][RPG_MAX_STATS]; // Amount to change stat by. 0.0=random change.
	ImVec2 ItemStatRandomLowHigh[RPG_MAX_ITEMS][RPG_MAX_STATS];

	//If not selling but "USE" this item , trigger these stat changes.
	int ItemUseAction[RPG_MAX_ITEMS]; //Trigger action when using this item, Start Quest, PlayerItemInGameCarryMaxWeight (bag size), Fire a weapon in ALL directions,...
	int ItemUseStatID[RPG_MAX_ITEMS][RPG_MAX_STATS]; //Stat ID to change.
	float ItemUseStatValue[RPG_MAX_ITEMS][RPG_MAX_STATS]; // Amount to change stat by.

	//Pickup item.
	int ItemAvailableOnlyAfterDead[RPG_MAX_ITEMS]; //
	char ItemPickupText[RPG_MAX_ITEMS][256];
	int ItemPickupKey[RPG_MAX_ITEMS]; //Asc("E");
	int ItemPickupRange[RPG_MAX_ITEMS];
	int ItemAutoCollect[RPG_MAX_ITEMS]; //Just collect no prompt.
	int ItemAutoUse[RPG_MAX_ITEMS]; //Use this item instantly when pickup, add health, larger bag size ...

	//Effects.
	char ItemPickupDisplayText[RPG_MAX_ITEMS][256]; // value+text like "+5 Bag size increased" , fast scroll text up and out.
	char ItemPickupSound[RPG_MAX_ITEMS][256];
	char ItemPickupParticle[RPG_MAX_ITEMS][256];
	int ItemPickupDisplayValue[RPG_MAX_ITEMS];

} RPG_Items;

#define RPG_MAX_LEVELS 15
#define RPG_MAX_PLAYERS 200
//PE: Fixed level 0 playerid 0 always start marker.
//PE: Use storyboard unique nodeid as level number here ?
//PE: Somehow we need to remap PlayerID when changing level ?, or include a "level ID" here ?
//PE: Parse from Storyboard.Nodes[node].levelnumber and use this level.
struct RPGPlayer
{
	int RPGActive = -1;

	int PlayerID[RPG_MAX_PLAYERS];
	float PlayerItemCarryMaxWeight[RPG_MAX_PLAYERS]; //For Store set this high.
	float PlayerItemInGameCarryMaxWeight[RPG_MAX_PLAYERS]; //Copy from PlayerItemCarryMaxWeight on new game, and save/load this one.
	int PlayerAllowSelling[RPG_MAX_PLAYERS];
	int PlayerAllowTrading[RPG_MAX_PLAYERS];
	int PlayerAllowLoot[RPG_MAX_PLAYERS];

	//What items do this player carry.
	int PlayerItemID[RPG_MAX_PLAYERS][RPG_MAX_ITEMS];
	int PlayerItemStartTotal[RPG_MAX_PLAYERS][RPG_MAX_ITEMS];
	int PlayerItemInGameTotal[RPG_MAX_PLAYERS][RPG_MAX_ITEMS]; //Copy from PlayerItemStartAmounts on new game, and save/load this one.

	//What are the player stat, Health,money...
	int PlayerStatID[RPG_MAX_PLAYERS][RPG_MAX_STATS];
	float PlayerStatStartValue[RPG_MAX_PLAYERS][RPG_MAX_ITEMS];
	//PC,NPC's should all react on these values in game.
	float PlayerStatInGameValue[RPG_MAX_PLAYERS][RPG_MAX_ITEMS]; //Copy from PlayerStatStartValue on new game , and save/load this one.

} RPG_Player[RPG_MAX_LEVELS];

//void *RPG_System;

void init_rpg_system_stats(void)
{
	//Stats.
	for (int i = 0; i < RPG_MAX_STATS; i++)
	{
		RPG_Stats.StatID[i] = -1;
		RPG_Stats.StatAction[i] = -1;
		RPG_Stats.StatMax[i] = 10000;
		strcpy(RPG_Stats.StatName[i], "");
		RPG_Stats.StatValue[i] = 0.0;
	}
	int id = 0;
	//Stats predefined.
	RPG_Stats.StatID[id] = 0;
	RPG_Stats.StatAction[id] = RPG_STATS_ACTION_MONEY;
	RPG_Stats.StatMax[id] = 100000000;
	strcpy(RPG_Stats.StatName[id], "GOLD");
	RPG_Stats.StatValue[id] = 10;
	id++;
	RPG_Stats.StatID[id] = 0;
	RPG_Stats.StatAction[id] = RPG_STATS_ACTION_HEALTH;
	RPG_Stats.StatMax[id] = 1000;
	strcpy(RPG_Stats.StatName[id], "HEALTH");
	RPG_Stats.StatValue[id] = 500;
	id++;
	RPG_Stats.StatID[id] = 0;
	RPG_Stats.StatAction[id] = RPG_STATS_ACTION_ARMOR;
	RPG_Stats.StatMax[id] = 9999;
	strcpy(RPG_Stats.StatName[id], "ARMOR");
	RPG_Stats.StatValue[id] = 10;
	id++;
	RPG_Stats.StatID[id] = 0;
	RPG_Stats.StatAction[id] = RPG_STATS_ACTION_STRENGTH;
	RPG_Stats.StatMax[id] = 9999;
	strcpy(RPG_Stats.StatName[id], "STRENGTH");
	RPG_Stats.StatValue[id] = 10;
	id++;
	RPG_Stats.StatID[id] = 0;
	RPG_Stats.StatAction[id] = RPG_STATS_ACTION_STAMINA;
	RPG_Stats.StatMax[id] = 9999;
	strcpy(RPG_Stats.StatName[id], "STAMINA");
	RPG_Stats.StatValue[id] = 0;
	id++;
	RPG_Stats.StatID[id] = 0;
	RPG_Stats.StatAction[id] = RPG_STATS_ACTION_MAGIC;
	RPG_Stats.StatMax[id] = 9999;
	strcpy(RPG_Stats.StatName[id], "MAGIC");
	RPG_Stats.StatValue[id] = 0;
	id++;
}

void init_rpg_system_types(void)
{

	for (int i = 0; i < RPG_MAX_TYPES; i++)
	{
		RPG_Types.TypeID[i] = -1;
		strcpy(RPG_Types.TypeName[i], "");
	}
	int id = 0;
	RPG_Types.TypeID[id] = 0;
	strcpy(RPG_Types.TypeName[id], "OTHER");
	id++;
	RPG_Types.TypeID[id] = 0;
	strcpy(RPG_Types.TypeName[id], "WEAPON");
	id++;
	RPG_Types.TypeID[id] = 0;
	strcpy(RPG_Types.TypeName[id], "ARMOR");
	id++;
	RPG_Types.TypeID[id] = 0;
	strcpy(RPG_Types.TypeName[id], "AMMO");
	id++;
	RPG_Types.TypeID[id] = 0;
	strcpy(RPG_Types.TypeName[id], "CONSUMABLE");
	id++;
	RPG_Types.TypeID[id] = 0;
	strcpy(RPG_Types.TypeName[id], "RESOURCE");
	id++;
	RPG_Types.TypeID[id] = 0;
	strcpy(RPG_Types.TypeName[id], "RECIPE");
	id++;
	RPG_Types.TypeID[id] = 0;
	strcpy(RPG_Types.TypeName[id], "QUEST");
	id++;

}

void init_rpg_system_items(void)
{
	for (int i = 0; i < RPG_MAX_ITEMS; i++)
	{
		RPG_Items.ItemID[i] = -1;
		strcpy(RPG_Items.ItemName[i], "");
		strcpy(RPG_Items.ItemDesc[i], "");
		strcpy(RPG_Items.ItemIconLarge[i], "");
		RPG_Items.ItemIconLargeID[i] = -1;
		strcpy(RPG_Items.ItemIconSmall[i], "");
		RPG_Items.ItemIconSmallID[i] = -1;

		RPG_Items.ItemWeight[i] = 5; // Weight.

		//Prices.
		RPG_Items.ItemBuyPrice[i] = 2;
		RPG_Items.ItemSellPrice[i] = 1;
		RPG_Items.ItemTradePrice[i] = 1.5;

		//Relation to objects on map.
		RPG_Items.ItemMasterObjectID[i] = -1;
		strcpy(RPG_Items.ItemMasterObjectFPE[i], "");

		RPG_Items.ItemTypeID[i] = 0; //default to OTHER
		RPG_Items.ItemTypeValue[i] = 1; //How strong is this item, Like Armor 23, Ammo 50 ...

		//Item can make stat changes. also minus.
		for (int a = 0; a < RPG_MAX_STATS; a++)
		{
			RPG_Items.ItemStatID[i][a] = -1;
			RPG_Items.ItemStatValue[i][a] = 1; //0 = random.
			RPG_Items.ItemStatRandomLowHigh[i][a] = ImVec2(1, 4);
		}

		//If not selling but "USE" this item , trigger these stat changes.
		RPG_Items.ItemUseAction[i] = 0; //Trigger action when using this item, Start Quest, PlayerItemInGameCarryMaxWeight (bag size), Fire a weapon in ALL directions,...

		for (int a = 0; a < RPG_MAX_STATS; a++)
		{
			RPG_Items.ItemUseStatID[i][a] = -1;
			RPG_Items.ItemUseStatValue[i][a] = 0; // Amount to change stat by.
		}

		//Pickup item.
		RPG_Items.ItemAvailableOnlyAfterDead[i] = 0; //1=Item only available after object health <= 0.
		strcpy(RPG_Items.ItemPickupText[i], "");
		RPG_Items.ItemPickupKey[i] = Asc("E");
		RPG_Items.ItemPickupRange[i] = 100;
		RPG_Items.ItemAutoCollect[i] = 0; //1=Just collect no prompt.
		RPG_Items.ItemAutoUse[i] = 0; //1=Use this item instantly when pickup, add health, larger bag size , start quest ...

		//Pickup Effects.
		strcpy(RPG_Items.ItemPickupDisplayText[i], "");
		strcpy(RPG_Items.ItemPickupSound[i], "");
		strcpy(RPG_Items.ItemPickupParticle[i], "");
		RPG_Items.ItemPickupDisplayValue[i] = 0; //1=value+ItemPickupDisplayText like "+5 Bag size increased" , fast scroll text up and out.

	}

	//PE: default Inventory ?
}

void init_rpg_system_players(void)
{
	//PE: Fixed level 0 playerid 0 always start marker.
	//PE: Use storyboard unique nodeid as level number here ?
	//PE: Somehow we need to remap PlayerID when changing level ?, or include a "level ID" here ?
	//PE: Parse from Storyboard.Nodes[node].levelnumber and use this level.
	for (int level = 0; level < RPG_MAX_LEVELS; level++)
	{
		RPG_Player[level].RPGActive = -1;

		for (int i = 0; i < RPG_MAX_PLAYERS; i++)
		{
			RPG_Player[level].PlayerID[i] = -1;
			RPG_Player[level].PlayerItemCarryMaxWeight[i] = 1000;
			RPG_Player[level].PlayerItemInGameCarryMaxWeight[i] = 1000; //InGame.

			RPG_Player[level].PlayerAllowSelling[i] = 0;
			RPG_Player[level].PlayerAllowTrading[i] = 0;
			RPG_Player[level].PlayerAllowLoot[i] = 0; //Will turn enemy if looted. have loot time ? so he can perhaps attach us.

			//What items do this player carry.
			for (int a = 0; a < RPG_MAX_ITEMS; a++)
			{
				RPG_Player[level].PlayerItemID[i][a] = -1;
				RPG_Player[level].PlayerItemStartTotal[i][a] = 0;
				RPG_Player[level].PlayerItemInGameTotal[i][a] = 0; //Ingame Copy from RPG_Player[level].PlayerItemStartTotal on new game, and save/load this one.
			}

			//What are the player stat, Health,money...
			for (int a = 0; a < RPG_MAX_STATS; a++)
			{
				RPG_Player[level].PlayerStatID[i][a] = -1;
				RPG_Player[level].PlayerStatStartValue[i][a] = 0;
				RPG_Player[level].PlayerStatInGameValue[i][a] = 0; //Ingame: copy from RPG_Player[level].PlayerStatStartValue on new game, and save/load this one.
			}
		}

		//PE: Defaults ?
	}

}

void init_rpg_system(void)
{
	init_rpg_system_stats();
	init_rpg_system_types();
	init_rpg_system_items();
	init_rpg_system_players();
}

bool load_rpg_system(char* name)
{
	bool bValid = true;
	char project[MAX_PATH];

	//RPG_Stats
	strcpy(project, "projectbank\\");
	strcat(project, name);
	strcat(project, "\\RPG_Stats.dat");
	FILE* projectfile = GG_fopen(project, "rb");
	if (projectfile)
	{
		size_t size = fread(&RPG_Stats, 1, sizeof(RPG_Stats), projectfile);
	}
	else
	{
		bValid = false;
	}

	//RPG_Types
	strcpy(project, "projectbank\\");
	strcat(project, name);
	strcat(project, "\\RPG_Types.dat");
	projectfile = GG_fopen(project, "rb");
	if (projectfile)
	{
		size_t size = fread(&RPG_Types, 1, sizeof(RPG_Types), projectfile);
	}
	else
	{
		bValid = false;
	}


	//RPG_Items
	strcpy(project, "projectbank\\");
	strcat(project, name);
	strcat(project, "\\RPG_Items.dat");
	projectfile = GG_fopen(project, "rb");
	if (projectfile)
	{
		size_t size = fread(&RPG_Items, 1, sizeof(RPG_Items), projectfile);
	}
	else
	{
		bValid = false;
	}


	//RPG_Player
	for (int level = 0; level < RPG_MAX_LEVELS; level++)
	{
		char tmp[MAX_PATH];
		sprintf(tmp, "projectbank\\%s\\%dRPG_Player.dat", name, level);
		strcpy(project, tmp);
		FILE* projectfile = GG_fopen(project, "rb");
		if (projectfile)
		{
			size_t size = fread(&RPG_Player[level], 1, sizeof(RPG_Player[level]), projectfile);
			RPG_Player[level].RPGActive = 1; //PE: Mark as ready to be used. RPG need a storyboard.
		}
	}
	RPG_Player[0].RPGActive = 1; //0 always active.

	return bValid;
}

bool save_rpg_system(char* name)
{
	bool bValid = true;
	char project[MAX_PATH];

	//RPG_Stats
	strcpy(project, "projectbank\\");
	strcat(project, name);
	strcat(project, "\\RPG_Stats.dat");
	FILE* projectfile = GG_fopen(project, "wb+");
	if (projectfile) {
		fwrite(&RPG_Stats, 1, sizeof(RPG_Stats), projectfile);
		fclose(projectfile);
	}
	else
	{
		bValid = false; //Keep saving other structures even if one failed.
	}

	//RPG_Types
	strcpy(project, "projectbank\\");
	strcat(project, name);
	strcat(project, "\\RPG_Types.dat");
	projectfile = GG_fopen(project, "wb+");
	if (projectfile) {
		fwrite(&RPG_Types, 1, sizeof(RPG_Types), projectfile);
		fclose(projectfile);
	}
	else
	{
		bValid = false; //Keep saving other structures even if one failed.
	}

	//RPG_Items
	strcpy(project, "projectbank\\");
	strcat(project, name);
	strcat(project, "\\RPG_Items.dat");
	projectfile = GG_fopen(project, "wb+");
	if (projectfile) {
		fwrite(&RPG_Items, 1, sizeof(RPG_Items), projectfile);
		fclose(projectfile);
	}
	else
	{
		bValid = false; //Keep saving other structures even if one failed.
	}


	//RPG_Player
	for (int level = 0; level < RPG_MAX_LEVELS; level++)
	{
		if (RPG_Player[level].PlayerID[0] != -1 || RPG_Player[level].PlayerID[1] != -1) //Do we got anything ?
		{
			char tmp[MAX_PATH];
			sprintf(tmp, "projectbank\\%s\\%dRPG_Player.dat", name, level);
			strcpy(project, tmp);
			FILE* projectfile = GG_fopen(project, "wb+");
			if (projectfile) {
				fwrite(&RPG_Player[level], 1, sizeof(RPG_Player[level]), projectfile);
				fclose(projectfile);
			}
			else
			{
				bValid = false; //Keep saving other structures even if one failed.
			}
		}
	}
	return bValid;
}

void ProcessRPGSetupWindow(void)
{
	if (!bRPGSetup_Window) return;

	if (refresh_gui_docking == 1)
	{
		ImGui::SetNextWindowSize(ImVec2(64 * ImGui::GetFontSize(), 38 * ImGui::GetFontSize()), ImGuiCond_Always);
		ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
	}
	else
	{
		ImGui::SetNextWindowSize(ImVec2(64 * ImGui::GetFontSize(), 38 * ImGui::GetFontSize()), ImGuiCond_Once);
		ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
	}

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings;
	ImGui::Begin("RPG setup##MustBeUnique", &bRPGSetup_Window, window_flags);

	//Tabs
	if (ImGui::BeginTabBar("rpgsetuptabbar"))
	{
		int flags = 0; // flags = ImGuiTabItemFlags_SetSelected;
		//RPG_Types
		if (ImGui::BeginTabItem(" Groups ", NULL, flags))
		{
			ImGui::Indent(10);

			ImGui::SetWindowFontScale(1.5);
			ImGui::TextCenter("Groups is used to order inventory into groups, and to filter lists.");
			ImGui::SetWindowFontScale(1.0);

			ImGui::Columns(2, "rpggroupscolumns2", false);  //false no border
			ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x * 0.25);

			ImGui::PushItemWidth(-10);
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

			int padding_bottom_y = 10;
			static int current_group_selected = -1;

			if (ImGui::StyleButton("Add New Group", ImVec2(0, 0)))
			{
				for (int i = 0; i < RPG_MAX_TYPES; i++)
				{
					if (RPG_Types.TypeID[i] == -1)
					{
						RPG_Types.TypeID[i] = i;
						sprintf(RPG_Types.TypeName[i], "Empty%d", i);
						current_group_selected = i;
						break;
					}
				}
			}

			float ListBoxHeight = ImGui::GetContentRegionAvail().y - padding_bottom_y; //ImGui::GetFontSize()*16.0f;
			if (ImGui::ListBoxHeader("##rpggroupsBox", ImVec2(0, ListBoxHeight)) == true)
			{
				for (int i = 0; i < RPG_MAX_TYPES; i++)
				{
					if (RPG_Types.TypeID[i] != -1)
					{
						if (current_group_selected == -1) current_group_selected = i; //PE: Always have a selection.
						bool bIsSelected = false;
						if (current_group_selected == i) bIsSelected = true;
						if (ImGui::Selectable(RPG_Types.TypeName[i], bIsSelected))
						{
							current_group_selected = i;
						}
					}
				}
				ImGui::ListBoxFooter();
			}

			//#### Split ####
			ImGui::NextColumn();

			if (current_group_selected != -1)
			{
				ImGui::Text("");
				ImGui::Text("(%s ID=%d)", RPG_Types.TypeName[current_group_selected], current_group_selected);
				ImGui::Text("");
				ImGui::Text("Enter the name of this group.");
				ImGui::InputText("##RPGEnterGroupText", RPG_Types.TypeName[current_group_selected], 250);
				ImGui::Text("");
				if (ImGui::StyleButton("Delete This Group", ImVec2(0, 0)))
				{
					if (current_group_selected != 0) //We cant delete first group, always need atleast one group.
					{
						for (int i = 0; i < RPG_MAX_TYPES; i++)
						{
							if (RPG_Types.TypeID[i] == current_group_selected)
							{
								RPG_Types.TypeID[i] = -1;
								//Loop into all structs and delete references to this group.
								for (int a = 0; a < RPG_MAX_ITEMS; a++)
								{
									if (RPG_Items.ItemTypeID[a] == current_group_selected)
									{
										RPG_Items.ItemTypeID[i] = 0; //default to OTHER
									}
								}
								current_group_selected = 0;
								break;
							}
						}
					}
				}

			}

			ImGui::Columns(1);

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
			ImGui::PopItemWidth();
			ImGui::Indent(-10);

			if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
				ImGui::Text(""); //Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
				ImGui::Text("");
			}

			ImGui::EndTabItem();
		}

		//RPG_Stats
		if (ImGui::BeginTabItem(" Stats ", NULL, flags))
		{
			ImGui::Indent(10);

			ImGui::SetWindowFontScale(1.5);
			ImGui::TextCenter("Stats is used to add custom variables to your games.");
			ImGui::TextCenter("Stats variables can be set to directly impact players in game, like Health,Armor,Money,Strength...");
			ImGui::SetWindowFontScale(1.0);

			ImGui::Columns(2, "rpgstatscolumns2", false);  //false no border
			ImGui::SetColumnWidth(0, ImGui::GetWindowSize().x * 0.25);

			ImGui::PushItemWidth(-10);
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

			int padding_bottom_y = 10;
			static int current_stat_selected = -1;

			if (ImGui::StyleButton("Add New Stat", ImVec2(0, 0)))
			{
				for (int i = 0; i < RPG_MAX_TYPES; i++)
				{
					if (RPG_Stats.StatID[i] == -1)
					{
						RPG_Stats.StatID[i] = i;
						RPG_Stats.StatAction[i] = 0;
						RPG_Stats.StatMax[i] = 9999;
						RPG_Stats.StatValue[i] = 1;
						sprintf(RPG_Stats.StatName[i], "Empty%d", i);
						current_stat_selected = i;
						break;
					}
				}
			}

			float ListBoxHeight = ImGui::GetContentRegionAvail().y - padding_bottom_y; //ImGui::GetFontSize()*16.0f;
			if (ImGui::ListBoxHeader("##rpgstatsBox", ImVec2(0, ListBoxHeight)) == true)
			{
				for (int i = 0; i < RPG_MAX_TYPES; i++)
				{
					if (RPG_Stats.StatID[i] != -1)
					{
						if (current_stat_selected == -1) current_stat_selected = i; //PE: Always have a selection.
						bool bIsSelected = false;
						if (current_stat_selected == i) bIsSelected = true;
						if (ImGui::Selectable(RPG_Stats.StatName[i], bIsSelected))
						{
							current_stat_selected = i;
						}
					}
				}
				ImGui::ListBoxFooter();
			}

			//#### Split ####
			ImGui::NextColumn();

			if (current_stat_selected != -1)
			{
				ImGui::Text("");
				ImGui::Text("(%s ID=%d)", RPG_Stats.StatName[current_stat_selected], current_stat_selected);
				ImGui::Text("");
				ImGui::Text("Enter the name of this stat.");
				ImGui::InputText("##RPGEnterstatText", RPG_Stats.StatName[current_stat_selected], 250);
				ImGui::Text("");

				ImGui::Text("Select in game action this stat will have, if its only a visible stat like XP,Level... set it to none.");
				const char* stat_action_items[] = { "None", "Add Money", "Add Health", "Add Armor", "Add Strength", "Add Stamina", "Add Magic" };
				if (ImGui::Combo("##GraphicsQualityTest", &RPG_Stats.StatAction[current_stat_selected], stat_action_items, IM_ARRAYSIZE(stat_action_items))) {
					//
				}

				ImGui::Text("");
				ImGui::Text("Enter the max value this stat can have. it will never go above this.");
				int iInt = RPG_Stats.StatMax[current_stat_selected]; //We use int but count using floats.
				if (ImGui::InputInt("##StatsMaxValue", &iInt, 0, 100000000))
				{
					RPG_Stats.StatMax[current_stat_selected] = iInt;
				}

				ImGui::Text("");
				ImGui::Text("Enter the start value this stat have. This can be overwritten when you later setup players and objects if needed.");
				iInt = RPG_Stats.StatValue[current_stat_selected]; //We use int but count using floats.
				if (ImGui::InputInt("##StatsStartValue", &iInt, 0, 100000000))
				{
					RPG_Stats.StatValue[current_stat_selected] = iInt;
				}

				ImGui::Text("");

				if (ImGui::StyleButton("Delete This Stat", ImVec2(0, 0)))
				{
					if (current_stat_selected > 5) //First 5 is needed , cant be deleted just changed.
					{
						for (int i = 0; i < RPG_MAX_TYPES; i++)
						{
							if (RPG_Stats.StatID[i] == current_stat_selected)
							{
								RPG_Stats.StatID[i] = -1;
								//Loop into all structs and delete references to this stat.
								for (int a = 0; a < RPG_MAX_ITEMS; a++)
								{
									for (int b = 0; b < RPG_MAX_STATS; b++)
									{
										if (RPG_Items.ItemStatID[a][b] == current_stat_selected)
											RPG_Items.ItemStatID[a][b] = -1;
										if (RPG_Items.ItemUseStatID[a][b] == current_stat_selected)
											RPG_Items.ItemUseStatID[a][b] = -1;
									}
								}
								break;
							}
						}

						for (int level = 0; level < RPG_MAX_LEVELS; level++)
						{
							for (int a = 0; a < RPG_MAX_PLAYERS; a++)
							{
								for (int b = 0; b < RPG_MAX_STATS; b++)
								{
									if (RPG_Player[level].PlayerStatID[a][b] == current_stat_selected)
										RPG_Player[level].PlayerStatID[a][b] = -1;
								}
							}
						}

						current_stat_selected = 0;
					}
				}

			}

			ImGui::Columns(1);

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
			ImGui::PopItemWidth();
			ImGui::Indent(-10);

			if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
				ImGui::Text(""); //Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
				ImGui::Text("");
			}

			ImGui::EndTabItem();
		}

		//RPG_Items TODO
		if (ImGui::BeginTabItem(" Items ", NULL, flags))
		{
			ImGui::Indent(10);
			ImGui::PushItemWidth(-10);
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

			ImGui::TextCenter("Items is used to add/setup available inventory for all players and objects.");

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
			ImGui::PopItemWidth();
			ImGui::Indent(-10);

			if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
				//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
				ImGui::Text("");
				ImGui::Text("");
			}

			ImGui::EndTabItem();
		}

		//RPG_Player TODO
		if (ImGui::BeginTabItem(" Players ", NULL, flags))
		{
			ImGui::Indent(10);
			ImGui::PushItemWidth(-10);
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

			ImGui::TextCenter("Players is used to add startup/default inventory/settings for all players and objects.");

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
			ImGui::PopItemWidth();
			ImGui::Indent(-10);

			if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
				//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
				ImGui::Text("");
				ImGui::Text("");
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
		//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
		ImGui::Text("");
		ImGui::Text("");
	}

	ImRect bbwin(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize());
	if (ImGui::IsMouseHoveringRect(bbwin.Min, bbwin.Max))
	{
		bImGuiGotFocus = true; //Must set bImGuiGotFocus so if you type "w" in a imgui widget , editor dont move forward.
	}
	if (ImGui::IsAnyItemFocused()) {
		bImGuiGotFocus = true;
	}

	ImGui::End();
}
bool IsRPGActive(void)
{
	return true;
}
#endif

