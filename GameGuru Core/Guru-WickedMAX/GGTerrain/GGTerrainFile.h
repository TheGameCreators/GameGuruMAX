#ifndef _H_GGTERRAINFILE
#define _H_GGTERRAINFILE

void GGTerrainFile_LoadTerrainData(LPSTR pTerrainDataFile, bool bRestoreWater);
void GGTerrainFile_SaveTerrainData(LPSTR pTerrainDataFile, int water_height);

#endif // _H_GGTERRAINFILE