//----------------------------------------------------
//--- GAMEGURU - M-Importer
//----------------------------------------------------

#include "cstr.h"

// animation slot structure
struct sAnimSlotStruct
{
	char pName[32];
	float fStart;
	float fFinish;
	float fStep1;
	float fStep2;
	float fStep3;
	bool bLooped;
};
bool animsystem_buildanimslots(int objectnumber);

struct sImportedObjectData
{
	char cImportPath[MAX_PATH];
	char cName[MAX_PATH];
	int iScaleMode;
	int iCentreMeshData;
	int iScale;
	int iRotationOffset[3];
	int iPositionOffset[3];
	int iFindFloor;
	int iCollisionShape;
	int iStatic;
	int iMaterialType;
	std::vector<std::array<float, 4>> baseColours;
	std::vector<std::array<char, MAX_PATH>> albedoFiles;
	std::vector<std::array<char, MAX_PATH>> normalFiles;
	std::vector<std::array<char, MAX_PATH>> surfaceFiles;
	std::vector<std::array<char, MAX_PATH>> emissiveFiles;
	std::vector<std::array<float, 4>> emissiveColours;
	std::vector<float> reflectance;
	std::vector<float> renderBias;
	std::vector<int> transparent;
	std::vector<int> doubleSided;
	std::vector<int> planarReflection;
	std::vector<int> castShadows;
	std::vector<sAnimSlotStruct> animSlots;
};

void importer_init ( void );
void set_temp_visuals(visualstype& currentVisuals, visualsdatastoragetype& storage, const visualsdatastoragetype& desiredVisuals);
void restore_visuals(visualstype& currentVisuals, visualsdatastoragetype& storage);
void importer_free ( void );
void importer_changeshader ( LPSTR pNewShaderFilename );
void importer_loadmodel ( void );
void importer_loadmodel( int objnumber );
void importer_load_scenery();
void importer_loop ( void );
void imgui_importer_loop(void);
void importer_update_selection_markers ( void );
void importer_extract_collision ( void );
void importer_ShowCollisionOnly ( void );
void importer_ShowCollisionOnlyOff ( void );
void importer_snapLeft ( void );
void importer_snapUp ( void );
void importer_snapforward ( void );
void importer_check_for_physics_changes ( void );
void importer_update_textures ( void );
void importer_texture_all_meshes(int iTexSlot);
void importer_set_all_material_settings(int slot, float value);
void importer_set_all_material_colour(int slot, float values[4]);
void importer_set_all_material_transparent(bool bIsTransparent);
void importer_set_all_mesh_double_sided(bool bIsDoubleSided);
void importer_set_all_material_planar_reflection(bool planarReflection);
void importer_set_all_material_cast_shadow(bool bCastShadow);
void importer_load_textures_finish ( int tCount, bool bCubeMapOnly );
void importer_load_textures ( void );
void importer_load_fpe ( void );
void importer_apply_fpe ( void );
void importer_save_fpe ( void );
void importer_handleScale ( void );
void importer_draw ( void );
void importer_quit ( void );
void importer_save_entity ( char *filename = NULL );
void importer_tabs_update ( void );
void import_generate_thumb(void);
void importer_tabs_draw ( void );
void importer_add_collision_box ( void );
void importer_dupe_collision_box ( void );
void importer_add_collision_box_loaded ( void );
void importer_delete_collision_box ( void );
void importer_checkForShaderFiles ( void );
void importer_checkForScriptFiles ( void );
void importer_help ( void );
void importer_screenSwitch ( void );
int findFreeDll ( void );
int findFreeMemblock ( void );
char* openFileBox ( char* filter, char* initdir, char* dtitle, char* defext, unsigned char open );
LPSTR _get_str_ptr ( char* pstr );
char* _get_str ( LPSTR strptr, int strsize );
char* importerPadString ( char* tString );
int findFreeObject ( void );
float GetDistance ( float x1, float  y1, float  z1, float  x2, float  y2, float  z2 );
int importer_check_if_protected ( char* timporterfilecheck_s );
void importer_sort_names ( void );
void importer_find_object_name_from_fpe ( void );
void importer_hide_mouse ( void );
void importer_show_mouse ( void );
void importer_fade_out ( void );
void importer_fade_in ( void );
void importer_check_script_token_exists ( void );
void importer_update_scale ( void );
void importer_find_floor(void);
void importer_delete_old_surface_files();
float GetScaleMultiply(int iObj);
void Wicked_Change_Object_Material(void* pObject, int mode, entityeleproftype *edit_grideleprof = NULL, bool bFromCustomMaterials = true);
void Wicked_Set_Material_Defaults(void* pVObject, int mode);
void Wicked_Copy_Material_To_Grideleprof(void* pVObject, int mode, entityeleproftype *edit_grideleprof = NULL);
void Wicked_Copy_JustTextureNames_To_Grideleprof(void* pVObject, int mode);
void Wicked_Update_All_Materials(void* pVObject, int mode);
void Wicked_Set_Material_From_grideleprof_ThisMesh(void* pVObject, int mode, entityeleproftype *edit_grideleprof = NULL, int iSpecificMeshIndex = -1);
void Wicked_Set_Material_From_grideleprof(void* pVObject, int mode, entityeleproftype *edit_grideleprof = NULL);
void importer_storeobjectdata();
void importer_restoreobjectdata();
void importer_clearobjectdata();
char* importer_selectfile(int texslot, std::string currentfilename, bool bPresetExplorer = true);
void importer_collectmeshname(char* meshName);
bool importer_havevalidobject();
