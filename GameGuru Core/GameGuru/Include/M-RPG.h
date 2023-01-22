//----------------------------------------------------
//--- GAMEGURU - M-RPG
//----------------------------------------------------

// Includes
#include "cstr.h"

// Data structures
struct collectionItemType
{
	std::vector<cstr> collectionFields;
};

// Externs
extern std::vector<cstr> g_collectionLabels;
extern std::vector<collectionItemType> g_collectionList;

// Prototypes
void init_rpg_system(void);
bool load_rpg_system(char* name);
bool save_rpg_system(char* name);
