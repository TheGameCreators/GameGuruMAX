//----------------------------------------------------
//--- GAMEGURU - M-UndoSys Terrain
//----------------------------------------------------

#pragma once

// Includes
#include <stack>
#include <vector>
#include <deque>
#include "K3D_Vector3D.h"
#include "M-UndoSys.h"

// Externs
extern std::stack<sUndoStackItem> g_UndoSysTerrainStack[2];

void undosys_terrain_init(uint32_t sculptSize, int width);
void undosys_terrain_cleanup();
void undosys_terrain_preparefornewlevel();

// Bounding box points on the heightmap.
struct TerrainEditsBB
{
	int minX, minY;
	int maxX, maxY;
};
void undosys_terrain_editsize(int startX, int endX, int startY, int endY);

// Terrain Sculpt
struct sUndoSysEventTerrainSculpt
{
	int minX;
	int minY;
	int maxX;
	int maxY;
	uint8_t* typeData;
	float* editData;
};

// Terrain Paint
struct sUndoSysEventTerrainPaint
{
	int minX;
	int minY;
	int maxX;
	int maxY;
	uint8_t* materialData;
};
// Grass Event
struct sUndoSysEventGrass
{
	int minX;
	int minY;
	int maxX;
	int maxY;
	uint8_t* grassData;
};
// Tree events
struct sUndoSysEventTrees
{
	uint8_t* treeData;
	//uint8_t* chunkData;
};

// Data storage for events using a single tree, data matches one of the structs below.
struct sUndoSysEventTreeSingle 
{
	// Use pointer to pre-allocated memory so when we remove old events to make space for new ones, the treemove event will be known and the stack order can be preserved.
	uint8_t* data; 
};
struct TreeMoveData
{
	// Data for eUndoSys_Terrain_MoveTree for convenience.
	uint32_t treeIndex;
	float x;
	float z;
	bool userMoved;
};
struct TreeScaleData
{
	// Data for eUndoSys_Terrain_ScaleTree
	uint32_t treeIndex;
	bool userMoved;
	unsigned char scale;
};
struct TreeAddData
{
	// Data for eUndoSys_Terrain_AddTree
	uint32_t treeIndex;
	float x;
	float z;
	uint32_t type;
	bool userMoved;
	unsigned char scale;
};
typedef struct TreeAddData TreeRemoveData;

struct sUndoSysEventTerrainMoveAfterSculpt
{
	int entity;
	uint8_t* height; // Using pointer to height, as we need to use the terrain stack for this event. Memory will be known when stack is shrunk to fit. Can't mix multiple events from different master stacks.
};

void undosys_terrain_paint(const TerrainEditsBB& bb, uint8_t* snapshot, int eList);
void undosys_terrain_sculpt (const TerrainEditsBB& bb, uint8_t* snapshot, int eList);
void undosys_terrain_grass(const TerrainEditsBB& bb, uint8_t* snapshot, int eList);
void undosys_terrain_settreecount(int treeCount, uint32_t treeSize, int chunkCount, uint32_t chunkSize);
void undosys_terrain_trees(uint8_t* snapshot, int eList);
void undosys_terrain_treemove(const TreeMoveData& moveData, int eList);
void undosys_terrain_treescale(const TreeScaleData& scaleData, int eList);
void undosys_terrain_treeadd(const TreeAddData& addData, int eList);
void undosys_terrain_treeremove(const TreeRemoveData& removeData, int eList);
void undosys_terrain_objectmovedaftersculpt(int entity, const float height, int eList);
int undosys_terrain_copysculptdata(sUndoSysEventTerrainSculpt* event, uint8_t* snapshot, int eList);
int undosys_terrain_copypaintdata(sUndoSysEventTerrainPaint* event, uint8_t* snapshot, int eList);
int undosys_terrain_copygrassdata(sUndoSysEventGrass* event, uint8_t* snapshot, int eList);
int undosys_terrain_copytreedata(sUndoSysEventTrees* event, uint8_t* snapshot, int eList);
int undosys_terrain_copysingletreedata(sUndoSysEventTreeSingle* event, void* data, int eEventType, int eList);
int undosys_terrain_copymoveaftersculptdata(sUndoSysEventTerrainMoveAfterSculpt* event, const float height, int eList);
uint32_t undosys_terrain_treemove_gettreeindex(sUndoSysEventTreeSingle* event);


struct UndoRedoMemory
{
	uint8_t* pMin;
	uint8_t* pMax;
	uint8_t* pTop;
	uint8_t* pMemory;
};
uint8_t* undosys_terrain_getmemory(uint32_t bytes, UndoRedoMemory& memory);
void undosys_terrain_setmemoryunusedfrom(uint8_t* location, UndoRedoMemory& memory);
uint8_t* undosys_terrain_getfreedata(UndoRedoMemory& memory);
void undosys_terrain_cleardata(UndoRedoMemory& memory);
void undosys_terrain_makespaceforevent( uint32_t bytesNeeded);
int undosys_terrain_removestackitem(sUndoStackItem& item, uint32_t* bytesRemoved);


void undosys_terrain_viewboundingbox(float realMinX, float realMinZ, float realMaxX, float realMaxZ);