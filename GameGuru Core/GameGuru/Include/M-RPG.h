//----------------------------------------------------
//--- GAMEGURU - M-RPG
//----------------------------------------------------

// Includes
#include "cstr.h"

// Data structures
#ifndef COLLECTIONDEFINED
#define COLLECTIONDEFINED
struct collectionItemType
{
	int iEntityID; // points to present levels ENTID
	int iEntityElementE; // holds the cloned identical details of this modified collectable object in ELEPROF
	std::vector<cstr> collectionFields; // all attribute fields
};
#endif

// Externs
extern std::vector<cstr> g_collectionLabels;
extern std::vector<collectionItemType> g_collectionMasterList;
extern std::vector<collectionItemType> g_collectionList;

// Prototypes
void init_rpg_system(void);
bool load_rpg_system(char* name);
bool save_rpg_system(char* name);
cstr get_rpg_imagefinalfile(cstr entityfile);
bool fill_rpg_item_defaults_passedin(collectionItemType* item, int entid, int e, LPSTR pTitle, LPSTR pImageFile);
bool fill_rpg_item_defaults(collectionItemType* item, int entid, int e);
bool refresh_collection_from_entities(void);
int find_rpg_collectionindex (char* pName);
