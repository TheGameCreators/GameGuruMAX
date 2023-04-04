//----------------------------------------------------
//--- GAMEGURU - M-UndoSys Terrain
//----------------------------------------------------

// Includes
#include "gameguru.h"

// Globals
std::stack<sUndoStackItem> g_UndoSysTerrainStack[2];
int g_iCalculatingChangeBounds = 0;
TerrainEditsBB g_EditBounds = { 0 };
//TerrainEditsRealBB g_WorldEditBounds = { 0 };

// These are copies of pHeightmapEdit
uint8_t* g_pTerrainSnapshot = nullptr;
UndoRedoMemory g_TerrainUndoMem = { 0 };
UndoRedoMemory g_TerrainRedoMem = { 0 };

int g_iTerrainEditSize = 0;

int g_iTreeCount = 0;
uint32_t g_iTreeSize;
int g_iChunkCount = 0;
uint32_t g_iChunkSize = 0;

#define GGTERRAIN_HEIGHTMAP_EDIT_SIZE 4096
//GGTERRAIN_HEIGHTMAP_EDIT_SIZE* GGTERRAIN_HEIGHTMAP_EDIT_SIZE* (sizeof(float) + sizeof(uint8_t)) = 83886080
#define SCULPTSIZE 83886080
uint8_t OutOfHeapMem1[157286400];
uint8_t OutOfHeapMem2[157286400];
uint8_t OutOfHeapMem3[SCULPTSIZE];

void undosys_terrain_init(uint32_t sculptDataSize, int width)
{
	if (t.game.gameisexe == 0)
	{
		//83886080
		// Pre-allocate the memory to store the terrain snapshots in:
		g_pTerrainSnapshot = &OutOfHeapMem3[0]; // new uint8_t[sculptDataSize];
	
		// minX and minY set higher than they could possible be, so a < comparison is always correct.
		g_EditBounds.minX = 50000;
		g_EditBounds.minY = 50000;
		g_EditBounds.maxX = 0;
		g_EditBounds.maxY = 0;

		g_iTerrainEditSize = width;

		// For now, allocate 300MB of memory between undo and redo.
		// TODO: since when the redo stack grows, the undo stack shrinks, this can be changed to a double ended stack, 
		// where the undo and redo share the same memory.
		
		uint32_t size = 157286400;// 
		g_TerrainUndoMem.pMemory = &OutOfHeapMem1[0]; // new uint8_t[size];
		g_TerrainUndoMem.pMin = g_TerrainUndoMem.pMemory;
		g_TerrainUndoMem.pMax = g_TerrainUndoMem.pMin + (size);
		g_TerrainUndoMem.pTop = g_TerrainUndoMem.pMin;

		g_TerrainRedoMem.pMemory = &OutOfHeapMem2[0]; //new uint8_t[size];
		g_TerrainRedoMem.pMin = g_TerrainRedoMem.pMemory;
		g_TerrainRedoMem.pMax = g_TerrainRedoMem.pMin + (size);
		g_TerrainRedoMem.pTop = g_TerrainRedoMem.pMin;
	}
}

void undosys_terrain_cleanup()
{
	if (g_pTerrainSnapshot)
	{
		delete[] g_pTerrainSnapshot;
		g_pTerrainSnapshot = 0;
	}

	if (g_TerrainUndoMem.pMemory)
	{
		delete[] g_TerrainUndoMem.pMemory;
		g_TerrainUndoMem.pMemory = 0;
	}

	if (g_TerrainRedoMem.pMemory)
	{
		delete[] g_TerrainRedoMem.pMemory;
		g_TerrainRedoMem.pMemory = 0;
	}
}

void undosys_terrain_settreecount(int treeCount, uint32_t treeSize, int chunkCount, uint32_t chunkSize)
{
	g_iTreeCount = treeCount;
	g_iTreeSize = treeSize;
	g_iChunkCount = chunkCount;
	g_iChunkSize = chunkSize;
}
	

//
// Specific Calls for Terrain Events
//

void undosys_terrain_objectmovedaftersculpt(int entity, const float height, int eList)
{
	sUndoSysEventTerrainMoveAfterSculpt* pEvent = new sUndoSysEventTerrainMoveAfterSculpt;
	pEvent->entity = entity;

	if (undosys_terrain_copymoveaftersculptdata(pEvent, height, eList))
	{
		undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_SculptMovedObject, pEvent);
	}
	else
	{
		// Could not create the event.
		if (pEvent)
		{
			// Free some memory and try again.

			assert(eList == eUndoSys_UndoList);

			
			// Assign the event sculpt data some memory.
			uint32_t iBytesNeeded = sizeof(float);

			// Make space for the new event (removes as many events from the terrain undo stack as needed, to fit within the given memory).
			undosys_terrain_makespaceforevent(iBytesNeeded);

			if (undosys_terrain_copymoveaftersculptdata(pEvent, height, eList))
			{
				undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_SculptMovedObject, pEvent);
			}
		}
	}
}

int undosys_terrain_copymoveaftersculptdata(sUndoSysEventTerrainMoveAfterSculpt* pEvent, const float height, int eList)
{
	// Assign the event data some memory.
	uint32_t iBytesNeeded = sizeof(float);
	UndoRedoMemory* pMemory = nullptr;
	if (eList == eUndoSys_UndoList)
		pMemory = &g_TerrainUndoMem;
	else
		pMemory = &g_TerrainRedoMem;

	pEvent->height = undosys_terrain_getmemory(iBytesNeeded, *pMemory);

	if (pEvent->height)
	{
		float y = height;
		float* pY = &y;
		memcpy(pEvent->height, (uint8_t*)pY, sizeof(float));
		return 1;
	}

	return 0;
}

void undosys_terrain_sculpt (const TerrainEditsBB& bb, uint8_t* snapshot, int eList)
{
	sUndoSysEventTerrainSculpt* pEvent = new sUndoSysEventTerrainSculpt;
	pEvent->minX = bb.minX;
	pEvent->minY = bb.minY;
	pEvent->maxX = bb.maxX;
	pEvent->maxY = bb.maxY;
	
	if (undosys_terrain_copysculptdata(pEvent, snapshot, eList))
	{
		undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_Sculpt, pEvent);
	}
	else
	{
		// Could not create the event.
		if (pEvent)
		{
			// Free some memory and try again.

			assert(eList == eUndoSys_UndoList);

			// if there was only enough memory available for the type data, mark it as unused and free some memory.
			if (pEvent->typeData)
				undosys_terrain_setmemoryunusedfrom(pEvent->typeData, g_TerrainUndoMem);

			int savedSizeX = pEvent->maxX - pEvent->minX;
			int savedSizeY = pEvent->maxY - pEvent->minY;

			if (savedSizeX < 0 || savedSizeY < 0)
			{
				// Invalid event.
				return;
			}
			// Assign the event sculpt data some memory.
			uint32_t iBytesNeeded = savedSizeX * savedSizeY * sizeof(uint8_t);
			iBytesNeeded += (iBytesNeeded * 4);

			// Make space for the new event (removes as many events from the terrain undo stack as needed, to fit within the given memory).
			undosys_terrain_makespaceforevent(iBytesNeeded);

			if (undosys_terrain_copysculptdata(pEvent, snapshot, eList))
			{
				undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_Sculpt, pEvent);
			}
		}
	}
}

int undosys_terrain_copysculptdata(sUndoSysEventTerrainSculpt* pEvent, uint8_t* snapshot, int eList)
{
	// First portion of data in the terrain snapshot are uint8_t's, second are floats.
	int iSnapshotTypeCount = g_iTerrainEditSize * g_iTerrainEditSize * sizeof(uint8_t);
	uint8_t* pEditsData = snapshot + iSnapshotTypeCount;
	float* pEdits = (float*)pEditsData;

	int savedSizeX = pEvent->maxX - pEvent->minX;
	int savedSizeY = pEvent->maxY - pEvent->minY;

	if (savedSizeX < 0 || savedSizeY < 0)
	{
		// Invalid event.
		delete pEvent;
		pEvent = 0;
		return 0;
	}

	// Assign the event sculpt data some memory.
	uint32_t iBytesNeeded = savedSizeX * savedSizeY * sizeof(uint8_t);
	UndoRedoMemory* pMemory = nullptr;
	if (eList == eUndoSys_UndoList)
		pMemory = &g_TerrainUndoMem;
	else
		pMemory = &g_TerrainRedoMem;

	pEvent->typeData = undosys_terrain_getmemory(iBytesNeeded, *pMemory);
	iBytesNeeded *= 4;
	pEvent->editData = (float*)undosys_terrain_getmemory(iBytesNeeded, *pMemory);

	if (pEvent->typeData && pEvent->editData)
	{
		// Store the previous heights of any changed vertices.
		for (int y = pEvent->minY; y < pEvent->maxY; y++)
		{
			for (int x = pEvent->minX; x < pEvent->maxX; x++)
			{
				int indexHeightMap = (g_iTerrainEditSize - 1 - y) * g_iTerrainEditSize + x;
				int indexSavedData = savedSizeX * (y - pEvent->minY) + (x - pEvent->minX);
				pEvent->typeData[indexSavedData] = snapshot[indexHeightMap];
				pEvent->editData[indexSavedData] = pEdits[indexHeightMap];
			}
		}

		return 1;
	}

	return 0;
}

// Terrain Paint
void undosys_terrain_paint(const TerrainEditsBB& bb, uint8_t* snapshot, int eList)
{
	sUndoSysEventTerrainPaint* pEvent = new sUndoSysEventTerrainPaint;
	pEvent->minX = bb.minX;
	pEvent->minY = bb.minY;
	pEvent->maxX = bb.maxX;
	pEvent->maxY = bb.maxY;

	if (undosys_terrain_copypaintdata(pEvent, snapshot, eList))
	{
		undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_Paint, pEvent);
	}
	else
	{
		// Could not create the event.
		if (pEvent)
		{
			// Free some memory and try again.

			assert(eList == eUndoSys_UndoList);

			// if there was only enough memory available for the type data, mark it as unused and free some memory.
			if (pEvent->materialData)
				undosys_terrain_setmemoryunusedfrom(pEvent->materialData, g_TerrainUndoMem);

			int savedSizeX = pEvent->maxX - pEvent->minX;
			int savedSizeY = pEvent->maxY - pEvent->minY;
			if (savedSizeX < 0 || savedSizeY < 0)
			{
				// Invalid event.
				return;
			}
			// Assign the event sculpt data some memory.
			uint32_t iBytesNeeded = savedSizeX * savedSizeY * sizeof(uint8_t);

			// Make space for the new event (removes as many events from the terrain undo stack as needed, to fit within the given memory).
			undosys_terrain_makespaceforevent(iBytesNeeded);

			if (undosys_terrain_copypaintdata(pEvent, snapshot, eList))
			{
				undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_Paint, pEvent);
			}
		}
	}
}


int undosys_terrain_copypaintdata(sUndoSysEventTerrainPaint* pEvent, uint8_t* snapshot, int eList)
{
	int savedSizeX = pEvent->maxX - pEvent->minX;
	int savedSizeY = pEvent->maxY - pEvent->minY;

	if (savedSizeX < 0 || savedSizeY < 0)
	{
		// Invalid event.
		delete pEvent;
		pEvent = 0;
		return 0;
	}

	// Assign the event sculpt data some memory.
	uint32_t iBytesNeeded = savedSizeX * savedSizeY * sizeof(uint8_t);
	UndoRedoMemory* pMemory = nullptr;
	if (eList == eUndoSys_UndoList)
		pMemory = &g_TerrainUndoMem;
	else
		pMemory = &g_TerrainRedoMem;

	pEvent->materialData = undosys_terrain_getmemory(iBytesNeeded, *pMemory);
	
	if (pEvent->materialData)
	{
		// Store the previous heights of any changed vertices.
		for (int y = pEvent->minY; y < pEvent->maxY; y++)
		{
			for (int x = pEvent->minX; x < pEvent->maxX; x++)
			{
				int indexHeightMap = y * g_iTerrainEditSize + x;
				int indexSavedData = savedSizeX * (y - pEvent->minY) + (x - pEvent->minX);
				pEvent->materialData[indexSavedData] = snapshot[indexHeightMap];
			}
		}

		return 1;
	}

	return 0;
}

// Create an undo event for grass.
void undosys_terrain_grass(const TerrainEditsBB& bb, uint8_t* snapshot, int eList)
{
	sUndoSysEventGrass* pEvent = new sUndoSysEventGrass;
	pEvent->minX = bb.minX;
	pEvent->minY = bb.minY;
	pEvent->maxX = bb.maxX;
	pEvent->maxY = bb.maxY;

	if (undosys_terrain_copygrassdata(pEvent, snapshot, eList))
	{
		undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_Grass, pEvent);
	}
	else
	{
		// Could not create the event.
		if (pEvent)
		{
			// Free some memory and try again.

			assert(eList == eUndoSys_UndoList);

			// if there was only enough memory available for the type data, mark it as unused and free some memory.
			if (pEvent->grassData)
				undosys_terrain_setmemoryunusedfrom(pEvent->grassData, g_TerrainUndoMem);

			int savedSizeX = pEvent->maxX - pEvent->minX;
			int savedSizeY = pEvent->maxY - pEvent->minY;
			if (savedSizeX < 0 || savedSizeY < 0)
			{
				// Invalid event.
				return;
			}
			// Assign the event sculpt data some memory.
			uint32_t iBytesNeeded = savedSizeX * savedSizeY * sizeof(uint8_t);

			// Make space for the new event (removes as many events from the terrain undo stack as needed, to fit within the given memory).
			undosys_terrain_makespaceforevent(iBytesNeeded);

			if (undosys_terrain_copygrassdata(pEvent, snapshot, eList))
			{
				undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_Grass, pEvent);
			}
		}
	}
}

int undosys_terrain_copygrassdata(sUndoSysEventGrass* pEvent, uint8_t* snapshot, int eList)
{
	int savedSizeX = pEvent->maxX - pEvent->minX;
	int savedSizeY = pEvent->maxY - pEvent->minY;

	if (savedSizeX < 0 || savedSizeY < 0)
	{
		// Invalid event.
		delete pEvent;
		pEvent = 0;
		return 0;
	}

	// Assign the event grass data some memory.
	uint32_t iBytesNeeded = savedSizeX * savedSizeY * sizeof(uint8_t);
	UndoRedoMemory* pMemory = nullptr;
	if (eList == eUndoSys_UndoList)
		pMemory = &g_TerrainUndoMem;
	else
		pMemory = &g_TerrainRedoMem;

	pEvent->grassData = undosys_terrain_getmemory(iBytesNeeded, *pMemory);

	if (pEvent->grassData)
	{
		// Store the previous heights of any changed vertices.
		for (int y = pEvent->minY; y < pEvent->maxY; y++)
		{
			for (int x = pEvent->minX; x < pEvent->maxX; x++)
			{
				int indexSnapshot = y * g_iTerrainEditSize + x;
				int indexSavedData = savedSizeX * (y - pEvent->minY) + (x - pEvent->minX);
				pEvent->grassData[indexSavedData] = snapshot[indexSnapshot];
			}
		}

		return 1;
	}

	return 0;
}

void undosys_terrain_trees(uint8_t* snapshot, int eList)
{
	sUndoSysEventTrees* pEvent = new sUndoSysEventTrees;

	if (undosys_terrain_copytreedata(pEvent, snapshot, eList))
	{
		undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_PaintTree, pEvent);
	}
	else
	{
		// Could not create the event.
		if (pEvent)
		{
			// Free some memory and try again.
		
			assert(eList == eUndoSys_UndoList);
		
			// Assign the event sculpt data some memory.
			uint32_t iTreeBytes = g_iTreeCount * g_iTreeSize;
			uint32_t iBytesNeeded = iTreeBytes;
		
			// Make space for the new event (removes as many events from the terrain undo stack as needed, to fit within the given memory).
			undosys_terrain_makespaceforevent(iBytesNeeded);
		
			if (undosys_terrain_copytreedata(pEvent, snapshot, eList))
			{
				undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_PaintTree, pEvent);
			}
		}
	}
}

int undosys_terrain_copytreedata(sUndoSysEventTrees* pEvent, uint8_t* snapshot, int eList)
{
	// Assign the event sculpt data some memory.
	uint32_t iTreeBytes = g_iTreeCount * g_iTreeSize;
	UndoRedoMemory* pMemory = nullptr;
	if (eList == eUndoSys_UndoList)
		pMemory = &g_TerrainUndoMem;
	else
		pMemory = &g_TerrainRedoMem;

	pEvent->treeData = undosys_terrain_getmemory(iTreeBytes, *pMemory);

	if (pEvent->treeData)
	{
		memcpy(pEvent->treeData, snapshot, iTreeBytes);

		return 1;
	}

	return 0;
}

void undosys_terrain_treemove(const TreeMoveData& moveData, int eList)
{
	sUndoSysEventTreeSingle* pEvent = new sUndoSysEventTreeSingle;
	TreeMoveData data = moveData;

	if (undosys_terrain_copysingletreedata(pEvent, &data, eUndoSys_Terrain_MoveTree, eList))
	{
		undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_MoveTree, pEvent);
	}
	else
	{
		// Could not create the event.
		if (pEvent)
		{
			// Free some memory and try again.
			assert(eList == eUndoSys_UndoList);
			
			// Assign the event sculpt data some memory.
			uint32_t iBytesNeeded = sizeof(TreeMoveData);

			// Make space for the new event (removes as many events from the terrain undo stack as needed, to fit within the given memory).
			undosys_terrain_makespaceforevent(iBytesNeeded);

			if (undosys_terrain_copysingletreedata(pEvent, &data, eUndoSys_Terrain_MoveTree, eList))
			{
				undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_MoveTree, pEvent);
			}
		}
	}
}

void undosys_terrain_treescale(const TreeScaleData& scaleData, int eList)
{
	sUndoSysEventTreeSingle* pEvent = new sUndoSysEventTreeSingle;
	TreeScaleData data = scaleData;

	if (undosys_terrain_copysingletreedata(pEvent, &data, eUndoSys_Terrain_ScaleTree, eList))
	{
		undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_ScaleTree, pEvent);
	}
	else
	{
		// Could not create the event.
		if (pEvent)
		{
			// Free some memory and try again.
			assert(eList == eUndoSys_UndoList);

			// Assign the event data some memory.
			uint32_t iBytesNeeded = sizeof(TreeScaleData);

			// Make space for the new event (removes as many events from the terrain undo stack as needed, to fit within the given memory).
			undosys_terrain_makespaceforevent(iBytesNeeded);

			if (undosys_terrain_copysingletreedata(pEvent, &data, eUndoSys_Terrain_ScaleTree, eList))
			{
				undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_ScaleTree, pEvent);
			}
		}
	}
}

void undosys_terrain_treeadd(const TreeAddData& addData, int eList)
{
	sUndoSysEventTreeSingle* pEvent = new sUndoSysEventTreeSingle;
	TreeAddData data = addData;

	if (undosys_terrain_copysingletreedata(pEvent, &data, eUndoSys_Terrain_AddTree, eList))
	{
		undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_AddTree, pEvent);
	}
	else
	{
		// Could not create the event.
		if (pEvent)
		{
			// Free some memory and try again.
			assert(eList == eUndoSys_UndoList);

			// Assign the event data some memory.
			uint32_t iBytesNeeded = sizeof(TreeAddData);

			// Make space for the new event (removes as many events from the terrain undo stack as needed, to fit within the given memory).
			undosys_terrain_makespaceforevent(iBytesNeeded);

			if (undosys_terrain_copysingletreedata(pEvent, &data, eUndoSys_Terrain_AddTree, eList))
			{
				undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_AddTree, pEvent);
			}
		}
	}
}

void undosys_terrain_treeremove(const TreeRemoveData& removeData, int eList)
{
	sUndoSysEventTreeSingle* pEvent = new sUndoSysEventTreeSingle;
	TreeRemoveData data = removeData;

	if (undosys_terrain_copysingletreedata(pEvent, &data, eUndoSys_Terrain_RemoveTree, eList))
	{
		undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_RemoveTree, pEvent);
	}
	else
	{
		// Could not create the event.
		if (pEvent)
		{
			// Free some memory and try again.
			assert(eList == eUndoSys_UndoList);

			// Assign the event data some memory.
			uint32_t iBytesNeeded = sizeof(TreeRemoveData);

			// Make space for the new event (removes as many events from the terrain undo stack as needed, to fit within the given memory).
			undosys_terrain_makespaceforevent(iBytesNeeded);

			if (undosys_terrain_copysingletreedata(pEvent, &data, eUndoSys_Terrain_RemoveTree, eList))
			{
				undosys_addevent(eUndoSys_Terrain, eUndoSys_Terrain_RemoveTree, pEvent);
			}
		}
	}
}

int undosys_terrain_copysingletreedata(sUndoSysEventTreeSingle* pEvent, void* data, int eEventType, int eList)
{
	// Calculate number of bytes needed to store the event data.
	uint32_t iBytesNeeded = 0;
	if (eEventType == eUndoSys_Terrain_ScaleTree)
		iBytesNeeded = sizeof(TreeScaleData);
	else if (eEventType == eUndoSys_Terrain_MoveTree)
		iBytesNeeded = sizeof(TreeMoveData);
	else if (eEventType == eUndoSys_Terrain_AddTree)
		iBytesNeeded = sizeof(TreeAddData);
	else if (eEventType == eUndoSys_Terrain_RemoveTree)
		iBytesNeeded = sizeof(TreeRemoveData);

	// Assign the event some memory.
	UndoRedoMemory* pMemory = nullptr;
	if (eList == eUndoSys_UndoList)
		pMemory = &g_TerrainUndoMem;
	else
		pMemory = &g_TerrainRedoMem;

	pEvent->data = undosys_terrain_getmemory(iBytesNeeded, *pMemory);

	if (pEvent->data)
	{
		if(eEventType == eUndoSys_Terrain_ScaleTree)
			memcpy(pEvent->data, data, sizeof(TreeScaleData));
		else if (eEventType == eUndoSys_Terrain_MoveTree)
			memcpy(pEvent->data, data, sizeof(TreeMoveData));
		else if (eEventType == eUndoSys_Terrain_AddTree)
			memcpy(pEvent->data, data, sizeof(TreeAddData));
		else if (eEventType == eUndoSys_Terrain_RemoveTree)
			memcpy(pEvent->data, data, sizeof(TreeRemoveData));

		return 1;
	}

	return 0;
}

uint32_t undosys_terrain_treemove_gettreeindex(sUndoSysEventTreeSingle* pEvent)
{
	if (pEvent->data)
	{
		// This relies of treeIndex always being the first variable stored inside the different event data structs.
		uint32_t* pTreeIndex = (uint32_t*)pEvent->data;
		return *pTreeIndex;
	}
	
	return 0;
}

// Calculate a rectangle that surrounds all vertices that have been changed when editing terrain.
void undosys_terrain_editsize(int startX, int endX, int startY, int endY)
{
	if (startX < g_EditBounds.minX)
		g_EditBounds.minX = startX;
	if (startY < g_EditBounds.minY)
		g_EditBounds.minY = startY;
	if (endX > g_EditBounds.maxX)
		g_EditBounds.maxX = endX;
	if (endY > g_EditBounds.maxY)
		g_EditBounds.maxY = endY;
}

void undosys_terrain_viewboundingbox(float realMinX, float realMinZ, float realMaxX, float realMaxZ)
{
	int id = 1000000;
	
	if (ObjectExist(id))
		DeleteObject(id);
	float width = realMaxX - realMinX;
	float depth = realMaxZ - realMinZ;
	MakeObjectBox(id, width, 10000, depth );
	PositionObject(id, (realMinX + realMaxX) / 2.0f, 100, (realMinZ + realMaxZ) / 2.0f);
	ShowObject(id);
}

// Mark bytes as used and return the start location in memory.
uint8_t* undosys_terrain_getmemory(uint32_t bytes, UndoRedoMemory& memory)
{
	uint8_t* pPrevTop = memory.pTop;
	uint8_t* pNewTop = pPrevTop + bytes;
	if (pNewTop <= memory.pMax)
	{
		memory.pTop = pNewTop;
		return pPrevTop;
	}

	return nullptr;
}

// Mark the data after location as unused.
void undosys_terrain_setmemoryunusedfrom(uint8_t* location, UndoRedoMemory& memory)
{
	if(location >= memory.pMin)
		memory.pTop = location;
}

// Gets the location of the top of the sculpt data stack - everything after this is not in use.
uint8_t* undosys_terrain_getfreedata(UndoRedoMemory& memory)
{
	return memory.pTop;
}

// Resets the sculpt data back to its initial state (does not delete, that's handled in undosys_terrain_cleanup)
void undosys_terrain_cleardata(UndoRedoMemory& memory)
{
	// Mark all the sculpt memory as unused.
	memory.pTop = memory.pMin;
}

// Shifts all of the memory pointers back towards pMin, and copies the data to the new location, removing the first event.
void undosys_terrain_makespaceforevent(uint32_t bytesNeeded)
{
	std::stack<sUndoStackItem> stackCopyReversed;
	// Make a copy of the stack in reversed order, to access the other end of the stack, and pop as many as necessary to fit the new item in.
	while (!g_UndoSysTerrainStack[eUndoSys_UndoList].empty())
	{
		stackCopyReversed.push(g_UndoSysTerrainStack[eUndoSys_UndoList].top());
		g_UndoSysTerrainStack[eUndoSys_UndoList].pop();
	}

	// The byte count of any removed events. This value will be used for the offset of data in the sculpt memory.
	uint32_t iBytesToRemove = 0;
	
	// Remove as many events as needed to fit the new event.
	std::vector<sUndoStackItem> itemsToAddBackIn;
	for (int i = 0; i < stackCopyReversed.size(); i++)
	{
		sUndoStackItem item = stackCopyReversed.top();
		if (undosys_terrain_removestackitem(item, &iBytesToRemove))
		{
			stackCopyReversed.pop();
			// To avoid the terrain stack becoming unsynchronised with the master stack, we will reconstruct the stack with empty events later.
			itemsToAddBackIn.push_back(item);
		}

		if (iBytesToRemove >= bytesNeeded)
			break;
	}

	// Shift all of the sculpt data by the offset.
	uint8_t* sourceLocation = g_TerrainUndoMem.pMemory + iBytesToRemove;
	uint32_t bytesRemaining = g_TerrainUndoMem.pMax - sourceLocation;
	memmove(g_TerrainUndoMem.pMemory, sourceLocation, sizeof(uint8_t) * bytesRemaining);

	// Reconstruct the terrain undo stack, starting with the removed items.
	for (int i = 0; i < itemsToAddBackIn.size(); i++)
	{
		g_UndoSysTerrainStack[eUndoSys_UndoList].push(itemsToAddBackIn[i]);
	}
	// Fill in the remaining items.
	while (!stackCopyReversed.empty())
	{
		// Update all of the event pointers to point to the correct data again.
		sUndoStackItem item = stackCopyReversed.top();
		if (item.event == eUndoSys_Terrain_Sculpt)
		{
			sUndoSysEventTerrainSculpt* pEvent = (sUndoSysEventTerrainSculpt*)item.pEventData;
			if (pEvent)
			{
				pEvent->typeData -= iBytesToRemove;
				// editData is a float*, which deals in increments of 4 bytes.
				pEvent->editData -= (iBytesToRemove / 4);
			}
		}
		else if (item.event == eUndoSys_Terrain_Paint)
		{
			sUndoSysEventTerrainPaint* pEvent = (sUndoSysEventTerrainPaint*)item.pEventData;
			if (pEvent)
			{
				pEvent->materialData -= iBytesToRemove;
			}
		}
		else if (item.event == eUndoSys_Terrain_Grass)
		{
			sUndoSysEventGrass* pEvent = (sUndoSysEventGrass*)item.pEventData;
			if (pEvent)
			{
				pEvent->grassData -= iBytesToRemove;
			}
		}
		else if (item.event >= eUndoSys_Terrain_AddTree && item.event <= eUndoSys_Terrain_ScaleTree)
		{
			sUndoSysEventTreeSingle* pEvent = (sUndoSysEventTreeSingle*)item.pEventData;
			if (pEvent)
			{
				pEvent->data -= iBytesToRemove;
			}
		}
		else if (item.event == eUndoSys_Terrain_PaintTree)
		{
			sUndoSysEventTrees* pEvent = (sUndoSysEventTrees*)item.pEventData;
			if (pEvent)
			{
				pEvent->treeData -= iBytesToRemove;
			}
		}
		else if (item.event == eUndoSys_Terrain_SculptMovedObject)
		{
			sUndoSysEventTerrainMoveAfterSculpt* pEvent = (sUndoSysEventTerrainMoveAfterSculpt*)item.pEventData;
			if (pEvent)
			{
				pEvent->height -= iBytesToRemove;
			}
		}

		g_UndoSysTerrainStack[eUndoSys_UndoList].push(item);
		stackCopyReversed.pop();
	}

	uint8_t* newTopOfStackMem = g_TerrainUndoMem.pTop - iBytesToRemove;
	undosys_terrain_setmemoryunusedfrom(newTopOfStackMem, g_TerrainUndoMem);
}

int undosys_terrain_removestackitem(sUndoStackItem& item, uint32_t* bytesRemoved)
{
	uint32_t iEventSize = 0;

	// If trees and vegetation don't use this memory for undo/redo, then this will need changed to compensate for that.
	if (item.event == eUndoSys_Terrain_Sculpt)
	{
		sUndoSysEventTerrainSculpt* pEvent = nullptr;
		pEvent = (sUndoSysEventTerrainSculpt*)item.pEventData;
		if (pEvent)
		{
			int savedSizeX = pEvent->maxX - pEvent->minX;
			int savedSizeY = pEvent->maxY - pEvent->minY;

			// Calculate the total size (bytes) of the event.
			iEventSize = savedSizeX * savedSizeY * sizeof(uint8_t);
			iEventSize += (iEventSize * 4);
		}
	}
	else if (item.event == eUndoSys_Terrain_Paint)
	{
		sUndoSysEventTerrainPaint* pEvent = nullptr;
		pEvent = (sUndoSysEventTerrainPaint*)item.pEventData;
		if (pEvent)
		{
			int savedSizeX = pEvent->maxX - pEvent->minX;
			int savedSizeY = pEvent->maxY - pEvent->minY;

			// Calculate the total size (bytes) of the event.
			iEventSize = savedSizeX * savedSizeY * sizeof(uint8_t);
		}
	}
	else if (item.event == eUndoSys_Terrain_Grass)
	{
		sUndoSysEventGrass* pEvent = nullptr;
		pEvent = (sUndoSysEventGrass*)item.pEventData;
		if (pEvent)
		{
			int savedSizeX = pEvent->maxX - pEvent->minX;
			int savedSizeY = pEvent->maxY - pEvent->minY;

			// Calculate the total size (bytes) of the event.
			iEventSize = savedSizeX * savedSizeY * sizeof(uint8_t);
		}
	}
	else if (item.event == eUndoSys_Terrain_MoveTree)
	{
		sUndoSysEventTreeSingle* pEvent = nullptr;
		pEvent = (sUndoSysEventTreeSingle*)item.pEventData;
		if (pEvent)
		{
			iEventSize = sizeof(TreeMoveData);
		}
	}
	else if (item.event == eUndoSys_Terrain_AddTree)
	{
		sUndoSysEventTreeSingle* pEvent = nullptr;
		pEvent = (sUndoSysEventTreeSingle*)item.pEventData;
		if (pEvent)
		{
			iEventSize = sizeof(TreeAddData);
		}
	}
	else if (item.event == eUndoSys_Terrain_RemoveTree)
	{
		sUndoSysEventTreeSingle* pEvent = nullptr;
		pEvent = (sUndoSysEventTreeSingle*)item.pEventData;
		if (pEvent)
		{
			iEventSize = sizeof(TreeRemoveData);
		}
	}
	else if (item.event == eUndoSys_Terrain_PaintTree)
	{
		sUndoSysEventTrees* pEvent = nullptr;
		pEvent = (sUndoSysEventTrees*)item.pEventData;
		if (pEvent)
		{
			iEventSize += g_iTreeCount * g_iTreeSize;
		}
	}
	else if (item.event == eUndoSys_Terrain_SculptMovedObject)
	{
		sUndoSysEventTerrainMoveAfterSculpt* pEvent = nullptr;
		pEvent = (sUndoSysEventTerrainMoveAfterSculpt*)item.pEventData;
		if (pEvent)
		{
			iEventSize += sizeof(float);
		}
	}

	// Remove the event from the stack and add its byte count for use in moving the sculpt data.
	*bytesRemoved += iEventSize;
	free(item.pEventData);
	item.pEventData = nullptr;

	if (iEventSize > 0)
		return 1;

	return 0;
}

void undosys_terrain_preparefornewlevel()
{
	undosys_terrain_cleardata(g_TerrainUndoMem);
	undosys_terrain_cleardata(g_TerrainRedoMem);
}
