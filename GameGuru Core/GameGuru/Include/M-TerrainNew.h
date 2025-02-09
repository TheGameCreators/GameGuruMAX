//----------------------------------------------------
//--- GAMEGURU - M-TerrainNew
//----------------------------------------------------

#include "cstr.h"

void terrain_initstyles ( void );
void terrain_initstyles_reset ( void );
void terrain_setupedit ( void );
int terrain_loadcustomtexture ( char* pDestPathAndFile, int iTextureSlot );
cstr terrain_getterrainfolder ( void );
void terrain_resetfornewlevel ( void );
void terrain_paintselector_hide ( void );
void terrain_deletesupertexturepalette ( void );
int terrain_createnewterraintexture ( char* pDestTerrainTextureFile, int iWhichTextureOver, char* pTexFileToLoad, int iSeamlessMode, int iCompressIt );
void terrain_loadlatesttexture ( void );
void terrain_changestyle ( void );
void terrain_getpaintmode ( void );
void terrain_loop ( void );
void terrain_terraintexturesystempainterentry ( void );
void terrain_detectendofterraintexturesystempainter ( void );
void terrain_editcontrol ( void );
void terrain_recordbuffer ( void );
void terrain_undo ( void );
void terrain_redo ( void );
void terrain_editcontrol_auxiliary ( void );
void terrain_paintterrain ( void );
void terrain_cursor ( void );
void terrain_cursor_nograsscolor ( void );
void terrain_cursor_off ( void );
void terrain_renderonly ( void );

void terrain_clearterraindirtyregion ( void );
void terrain_cleargrassdirtyregion ( void );
void terrain_cleardirtyregion ( void );
void terrain_waterineditor ( void );
void terrain_assignnewshader ( void );
void terrain_applyshader ( void );
void terrain_createactualterrain ( void );
void terrain_make ( void );
void terrain_make_image_only(void);
void terrain_load ( char* pLevelBankLocation );
void terrain_save ( char* pLevelBankLocation );
void terrain_savetextures ( void );
void terrain_generatevegandmask_grab ( void );
void terrain_generatevegandmaskfromterrain ( void );
void terrain_generateblanktextures ( void );
void terrain_loaddata ( void );
void terrain_delete ( void );
void terrain_updaterealheights ( void );
void terrain_randomiseorflattenterrain ( void );
void terrain_flattenterrain ( void );
void terrain_randomiseterrain ( void );
void terrain_refreshterrainmatrix ( void );
void terrain_skipifnowaterexposed ( void );
void terrain_updatewatermask ( void );
void terrain_updatewatermask_new ( void );
void terrain_whitewashwatermask ( void );
void terrain_createheightmapfromheightdata ( void );
void terrain_quickupdateheightmapfromheightdata ( void );
void terrain_generatetextureselect ( void );
void terrain_generatesupertexture ( bool bForceRecalcOfPalette );
void terrain_generateshadows ( void );
void generate_terrain ( int seed, int scale, int mchunk_size );
void DiamondSquare(unsigned x1, unsigned y1, unsigned x2, unsigned y2, float range, unsigned level) ;
void terrain_start_play ( void );
void terrain_stop_play ( void );
void terrain_setfog ( void );
void terrain_parsed_getstring ( void );
void terrain_parsed_getvalues ( void );
void terrain_water_init ( void );
void terrain_water_free ( void );
void terrain_updatewatermechanism ( void );
void terrain_updatewaterphysics ( void );
void terrain_water_setfog ( void );
void imgui_Customize_Terrain(int mode);
void imgui_Customize_Sky_V2(int mode);
void imgui_Customize_Vegetation(int mode);
void imgui_Customize_Weather(int mode);
void imgui_Customize_Water_V2(int mode);
void imgui_Customize_Weather_V2(int mode);
void imgui_Customize_Terrain_v3(int mode);
void imgui_Customize_Vegetation_v3(int mode);
void imgui_Customize_Tree_v3(int mode);

void ChooseTerrainTextureFolder(char* folder = nullptr);
void SaveTerrainTextureFolder(LPSTR pFile);
void LoadTerrainTextureFolder(LPSTR pFile);
void TerrainConvertPNGFiles(std::vector<std::string>* files);
void ResetTextureSettings();



