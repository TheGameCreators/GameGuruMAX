//----------------------------------------------------
//--- GAMEGURU - M-UndoSys
//----------------------------------------------------

#pragma once

// Includes
#include <stack>
#include <vector>
#include <deque>
#include "K3D_Vector3D.h"

// Master Stack Structure
enum eUndoMasterList
{
	eUndoSys_UndoList,
	eUndoSys_RedoList
};
enum eUndoMasterItemType
{
	eUndoSys_Object,
	eUndoSys_Terrain
};
struct sUndoMasterStackItem
{
	eUndoMasterItemType type;
	int count;
	int gluebatchcount;
};

// Stack Structure
enum eUndoEventType
{
	eUndoSys_None,
	eUndoSys_Object_Add,
	eUndoSys_Object_ChangePosRotScl,
	eUndoSys_Object_Delete,
	eUndoSys_Object_DeleteWaypoint,
	eUndoSys_Object_Group,
	eUndoSys_Object_UnGroup,
	eUndoSys_Terrain_Sculpt,
	eUndoSys_Terrain_Paint,
	eUndoSys_Terrain_Grass,
	eUndoSys_Terrain_AddTree,		// Must not change the order of these.
	eUndoSys_Terrain_MoveTree,		//
	eUndoSys_Terrain_RemoveTree,	//
	eUndoSys_Terrain_ScaleTree,		//
	eUndoSys_Terrain_PaintTree,		//
	eUndoSys_Terrain_SculptMovedObject
};
struct sUndoStackItem
{
	eUndoEventType event;
	void* pEventData;
};

// Externs
extern eUndoMasterList g_UndoList;
extern std::stack<sUndoMasterStackItem> g_UndoSysMasterStack[2];

// Core Functions
void undosys_multiplevents_start(void);
void undosys_multiplevents_finish(void);
void undosys_addevent (eUndoMasterItemType mastertype, eUndoEventType eventtype, void* pEventData);
void undosys_glue(int eList, int iGlueBatchCount);

// Actions for user
void undosys_clearredostack (void);
void undosys_undoevent (void);
void undosys_redoevent (void);
