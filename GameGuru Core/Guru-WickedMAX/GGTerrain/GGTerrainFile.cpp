//
// GGTerrain File Functions
//

// Includes
#include "stdafx.h"
#include "gameguru.h"
#include "GGTerrain.h"
#include "GGTerrainFile.h"
using namespace GGTerrain;

// Functions
void GGTerrainFile_LoadTerrainData ( LPSTR pTerrainDataFile , bool bRestoreWater)
{
	// Reset to default values
	extern void procedural_set_empty_level(bool bWaterReset);
	procedural_set_empty_level(true);

	// if file exists, load it
	if (FileExist(pTerrainDataFile) == 1)
	{
		OpenToRead(1, pTerrainDataFile);
		if (FileOpen(1) == 1)
		{
			ReadMemblock(1, 5);
			int iLength = GetMemblockSize(5);
			LPSTR pStringJSON = new char[iLength];
			for (int b = 0; b < iLength; b++)
			{
				pStringJSON[b] = ReadMemblockByte(5, b);
			}
			GGTerrain_LoadSettings(pStringJSON, bRestoreWater);
			delete pStringJSON;
			DeleteMemblock(5);
			CloseFile(1);

			// transfer water settings to rest of engine on load
			if (ggterrain_extra_params.iProceduralTerrainType == 0)
			{
				ggterrain_global_params.fractal_initial_amplitude = 0.0f;
				t.visuals.bWaterEnable = false;
				t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
			}
			else
			{
				//PE: Know fractal_initial_amplitude get saved, just for old levels for now.
				//PE: procedural_set_empty_level will set it to 0.0f.
				if(ggterrain_global_params.fractal_initial_amplitude == 0.0f)
					ggterrain_global_params.fractal_initial_amplitude = 1.0f;
				t.visuals.bWaterEnable = true;
				t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;
			}
			//g.gdefaultwaterheight = (int)GGTerrain_MetersToUnits(ggterrain_global_render_params2.waterHeight);
			//t.terrain.waterliney_f = (float)g.gdefaultwaterheight;
		}
	}
	else
	{
		// failed to load (rely on defaults above)
	}
}

void GGTerrainFile_SaveTerrainData (LPSTR pTerrainDataFile, int water_height)
{
	// save new terrain data file
	if (FileExist(pTerrainDataFile) == 1) DeleteAFile (pTerrainDataFile);
	if (FileOpen(1) == 1) CloseFile(1);
	OpenToWrite ( 1, pTerrainDataFile);
	if (FileOpen(1) == 1)
	{
		LPSTR pStringJSON = GGTerrain_SaveSettings(water_height);
		int iLength = (int) strlen(pStringJSON);
		if (MemblockExist(5) == 1) DeleteMemblock(5);
		MakeMemblock(5, iLength);
		for (int b = 0; b < iLength; b++)
		{
			WriteMemblockByte(5, b, pStringJSON[b]);
		}
		WriteMemblock(1, 5);
		CloseFile(1);
		DeleteMemblock(5);
		delete pStringJSON;
	}
}
