//
// GameGuru Main Engine (for Wicked Engine)
//

// Includes 
#include "stdafx.h"
#include "GameGuruMain.h"
#include <stdio.h>
#include <string.h>
#include "gameguru.h"
#include "GPUParticles.h"
#include "GGTerrain/GGTerrain.h"
#include "GGTerrain/GGTrees.h"
#include "GGTerrain/GGGrass.h"

// Globals
int g_iInitializationSequence = 0;
bool g_bNoGGUntilGameGuruMainCalled = false;

// extern into DBDLLCore module
extern void ConstantNonDisplayUpdate(void);

bool GuruLoopLogic ( void )
{
	// not until main has started
	if (g_bNoGGUntilGameGuruMainCalled == false)
	{
		switch (g_iInitializationSequence)
		{
			case 0: 
			{
				// start init sequence
				g_iInitializationSequence = 1;
				break;
			}
			case 1:
			{
				// GPU particles init
				timestampactivity(0, "GPUParticles::gpup_init();");
				GPUParticles::gpup_init();
				g_iInitializationSequence = 2;
				break;
			}
			case 2:
			{
				#ifdef GGTERRAIN_USE_NEW_TERRAIN
				
				//PE: Start early , while setting up terrain.
				int iUpdateCheckRetValue = ExecuteFile("..\\..\\GameGuru MAX Update Check.exe", "", "", 0, 1);

				// terrain init
				timestampactivity(0, "GGTerrain::GGTerrain_Init();");
				wiGraphics::CommandList cmd = wiRenderer::GetDevice()->BeginCommandList();
				GGTerrain::GGTerrain_Init( cmd );
				timestampactivity(0, "GGTrees::GGTrees_Init();");
				GGTrees::GGTrees_Init();
				timestampactivity(0, "GGGrass::GGGrass_Init();");
				GGGrass::GGGrass_Init();
				#endif
				g_iInitializationSequence = 3;
				break;
			}
			case 3:
			{
				// separated common() into common_init() and common_loop() (no internal loop for Wicked Engine variant)
				timestampactivity(0, "GuruMain();");
				GuruMain();

				// can resume normal operations, everything initialised
				g_bNoGGUntilGameGuruMainCalled = true;
				break;
			}
		}

		// still in init stages
		return false;
	}
	else
	{
		// regular common loop logic call
		common_loop_logic();

		#ifdef WICKEDENGINE
		// as new wicked engine never uses Sync(), we still need regular update work for sound and animation
		ConstantNonDisplayUpdate();
		#endif

		// normal operations
		return true;
	}
}

void GuruMain (void)
{
	// Launch GameGuru (but no internal loop)
	common_init();
}

void GuruLoopRender ( void )
{
	// not until main has started
	if (g_bNoGGUntilGameGuruMainCalled == false)
		return;

	common_loop_render();
}

bool GuruUpdate (void)
{
	// return false if guru not yet ready
	return g_bNoGGUntilGameGuruMainCalled;
}

void GuruFinish ( void )
{
	// not until main has started
	if (g_bNoGGUntilGameGuruMainCalled == false)
		return;

	common_finish();
}
