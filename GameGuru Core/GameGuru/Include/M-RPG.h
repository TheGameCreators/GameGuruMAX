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
	std::vector<cstr> collectionFields;
};
#endif

// Externs
extern std::vector<cstr> g_collectionLabels;
extern std::vector<collectionItemType> g_collectionMasterList;
extern std::vector<collectionItemType> g_collectionList;

// Prototypes
void init_rpg_system(void);
bool load_rpg_system(char* name);
bool refresh_collection_from_entities(void);
bool add_collection_internal(char* pTitle, char* pImage, char* pDesc, char* pCost);
bool save_rpg_system(char* name);
int find_rpg_collectionindex (char* pName);
