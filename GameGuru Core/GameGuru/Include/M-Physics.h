//----------------------------------------------------
//--- GAMEGURU - M-Physics
//----------------------------------------------------

// includes
#include "cstr.h"

// defines
#define PHYSICS_VIRTUALTREE_MAX 32

// prototypes
void physics_inittweakables ( void );
void physics_init ( void );
void physics_finalize ( void );
float& getCombinedLevelProgress ( void );
void physics_createterraincollision ( void );
#ifdef WICKEDENGINE
void physics_createvirtualtreecylinders (void);
void physics_freevirtualtreecylinders (void);
void physics_managevirtualtreecylinders (void);
int physics_getmaterialindex(float, float);
int physics_rayintersecttree (float fX, float fY, float fZ, float fToX, float fToY, float fToZ);
#endif
void physics_prepareentityforphysics ( void );
void physics_setupplayernoreset ( void );
void physics_setupplayer ( void );
void physics_disableplayer ( void );
void physics_setupcharacter ( void );
void physics_setupebestructure ( void );
void physics_setupobject ( void );
void physics_setupimportershapes ( void );
void physics_setuptreecylinder ( void );
void physics_disableobject ( void );
void physics_beginsimulation ( void );
void physics_pausephysics ( void );
void physics_resumephysics ( void );
void physics_loop ( void );
void physics_free ( void );
void physics_explodesphere ( void );
void physics_player_init ( void );
void physics_player_free ( void );
void physics_player ( void );
void physics_player_gatherkeycontrols ( void );
void physics_no_gun_zoom ( void );
void physics_getcorrectjumpframes ( int entid, float* fStartFrame, float* fHoldFrame, float* fResumeFrame, float* fFinishFrame );
void physics_player_control ( void );
void physics_player_handledeath ( void );
void physics_player_listener ( void );
void physics_player_takedamage ( void );
void physics_player_reset_underwaterstate ( void );
void physics_player_gotolastcheckpoint ( void );
void physics_resetplayer_core ( void );
void physics_player_thirdpersonreset ( void );
void physics_player_addweapon ( void );
void physics_player_removeweapon ( void );
void physics_player_resetWeaponSlots( void );
void physics_player_refreshcount ( void );

// Physics debug drawing.
void physics_set_debug_draw(int iDraw);
void physics_create_debug_mesh(float* data, int count, bool bStatic, int offset);
void physics_update_debug_mesh(float* data, int count, int objectID, int offsetLower, int offsetUpper);
void physics_add_vert_to_debug_mesh(float x, float y, float z, int v, int memblock);
void physics_debug_make_prism_between_points(float* p0, float* p1, float* points, float thickness = 0.05f);
void physics_debug_add_object(int objectID);
void physics_debug_remove_object(int objectID);
void physics_debug_draw();
void physics_render_debug_meshes();
void physics_importer_create_temp();
