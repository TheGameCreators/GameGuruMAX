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

// For profiling
#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

// Globals
int g_iInitializationSequence = 0;
bool g_bNoGGUntilGameGuruMainCalled = false;
bool g_bInGameCPUFrameComplete = false;

// Extra thread to run clean logic code alongside main thread
bool g_bInitExtraThreadForGameLogic = true;
bool g_bTriggerSomeGameLogic = false;
int g_iCountNumberOfExtraThreadCalls = 0;
#include "GGThread.h"
using namespace GGThread;

class GuruLogicClass : public GGThread
{
protected:
	//static threadLock lock;
	static GuruLogicClass* pThreads;
	static uint32_t iNumThreads;
	static threadLock lock;
public:
	static bool AnyRunning()
	{
		for (uint32_t i = 0; i < iNumThreads; i++)
		{
			if (pThreads[i].IsRunning()) return true;
		}
		return false;
	}
	static void WaitForAll()
	{
		for (uint32_t i = 0; i < iNumThreads; i++) pThreads[i].Join();
	}
	static void StartThreads()
	{
		for (uint32_t i = 0; i < iNumThreads; i++) pThreads[i].Start();
	}
	static void StopThreads()
	{
		for (uint32_t i = 0; i < iNumThreads; i++) pThreads[i].Stop();		
	}
	static void SetThreads(uint32_t numThreads)
	{
		if (numThreads == iNumThreads) return;
		if (pThreads) delete[] pThreads;
		pThreads = new GuruLogicClass[numThreads];
		iNumThreads = numThreads;
	}
	uint32_t Run()
	{
		while (1)
		{
			if (bTerminate) return 0;
			while (!lock.Acquire());
			lock.Release();
			if (g_bTriggerSomeGameLogic == true)
			{
				// physics on thread causes crashes and need of thread locking, so commented out for now
				// savely moved out of main thread
				//auto range1 = wiProfiler::BeginRangeCPU("Extra - Logic - Physics");
				//physics_loop ();
				//wiProfiler::EndRange(range1);

				auto range2 = wiProfiler::BeginRangeCPU("Extra - Logic - Intersects");
				ProcessIntersectDatabaseExtraThreadItemList();
				wiProfiler::EndRange(range2);
				auto range3 = wiProfiler::BeginRangeCPU("Extra - Logic - Visibility");
				entity_lua_getentityplrvisible_processlist();
				wiProfiler::EndRange(range3);

				g_iCountNumberOfExtraThreadCalls++;
				g_bTriggerSomeGameLogic = false;
			}
			else
			{
				//PE: This thread is taken over 33% of total cpu time even if it has nothing to do so:
				Sleep(1);
			}
		}
		return 0;
	}
	GuruLogicClass() : GGThread()
	{
	}
	~GuruLogicClass()
	{
		if (pThreads) delete[] pThreads;
	}
};
GuruLogicClass* GuruLogicClass::pThreads = 0;
uint32_t GuruLogicClass::iNumThreads = 0;
threadLock GuruLogicClass::lock;

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
				extern int g_iDisableTerrainSystem;
				if (g_iDisableTerrainSystem == 0)
				{
					// terrain init
					timestampactivity(0, "GGTerrain::GGTerrain_Init();");
					wiGraphics::CommandList cmd = wiRenderer::GetDevice()->BeginCommandList();
					GGTerrain::GGTerrain_Init(cmd);
					timestampactivity(0, "GGTrees::GGTrees_Init();");
					GGTrees::GGTrees_Init();
					timestampactivity(0, "GGGrass::GGGrass_Init();");
					GGGrass::GGGrass_Init();
				}
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

		// as new wicked engine never uses Sync(), we still need regular update work for sound and animation
		ConstantNonDisplayUpdate();

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
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	// not until main has started 
	if (g_bNoGGUntilGameGuruMainCalled == false)
		return;

	common_loop_render();

	// call on an extra thread to do clean game logic work (that does not tap graphics devices, etc)
	if (g_bInGameCPUFrameComplete == true)
	{
		g_bInGameCPUFrameComplete = false;
		extern bool g_bInitExtraThreadForGameLogic;
		if (g_bInitExtraThreadForGameLogic == true)
		{
			GuruLogicClass::SetThreads(1);
			GuruLogicClass::StartThreads();
			g_bInitExtraThreadForGameLogic = false;
		}
		extern bool g_bTriggerSomeGameLogic;
		g_bTriggerSomeGameLogic = true;
	}
}

void GuruLoopStopExtraThread (void)
{
	GuruLogicClass::StopThreads();
	g_bInitExtraThreadForGameLogic = true;
	g_bTriggerSomeGameLogic = false;
	g_bInGameCPUFrameComplete = false;
	ResetIntersectDatabaseExtraThreadItemList();
	entity_lua_getentityplrvisible_clear();
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
