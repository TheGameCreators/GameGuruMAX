//----------------------------------------------------
//--- GAMEGURU - M-UndoSys
//----------------------------------------------------

// Includes
#include "gameguru.h"

// Globals
eUndoMasterList g_UndoList;
std::stack<sUndoMasterStackItem> g_UndoSysMasterStack[2];
bool g_UndoSysMasterCollectingMultipleEvents = false;
int g_UndoSysMasterMultipleEventsCount = 0;

// 
// UNDO SYSTEM CORE
// 

void undosys_setlist(eUndoMasterList eList)
{
	g_UndoList = eList;
}

void undosys_multiplevents_start(void)
{
	// called before multiple events to be added as one action
	if (g_UndoSysMasterCollectingMultipleEvents == false)
	{
		g_UndoSysMasterCollectingMultipleEvents = true;
		g_UndoSysMasterMultipleEventsCount = 0;
	}
}

void undosys_multiplevents_finish(void)
{
	// called when finished all events to go under the last master stack item added
	if (g_UndoSysMasterCollectingMultipleEvents == true)
	{
		//PE: This crash if g_UndoSysMasterStack[g_UndoList].size = 0;
		//sUndoMasterStackItem lastmasteritem = g_UndoSysMasterStack[g_UndoList].top();
		g_UndoSysMasterCollectingMultipleEvents = false;
		g_UndoSysMasterMultipleEventsCount = 0;
	}
}

void undosys_addevent ( eUndoMasterItemType mastertype, eUndoEventType eventtype, void* pEventData )
{
	// add item to master list stack
	sUndoMasterStackItem masteritem;
	if (g_UndoSysMasterCollectingMultipleEvents == true)
	{
		// can have multiple events in type stacks represented by one item in master stack (for multiple events as one user action)
		g_UndoSysMasterMultipleEventsCount++;
		if (g_UndoSysMasterMultipleEventsCount == 1)
		{
			// first one of multiple events
			masteritem.type = mastertype;
			masteritem.count = 1;
			masteritem.gluebatchcount = 0;
			g_UndoSysMasterStack[g_UndoList].push(masteritem);
		}
		else
		{
			// ensure master type does not change INSIDE the multiple events collection
			masteritem = g_UndoSysMasterStack[g_UndoList].top();
			if (masteritem.type != mastertype)
			{
				// cannot mix types inside a single grouped undo, but CAN join two events together when user performs an undo/redo
				MessageBoxA (NULL, "Cannot mix undo system master types in multiple events capture", "", MB_OK);
			}
			else
			{
				// if another event added, replace last master item with new count
				g_UndoSysMasterStack[g_UndoList].pop();
				masteritem.type = mastertype;
				masteritem.count = g_UndoSysMasterMultipleEventsCount;
				masteritem.gluebatchcount = 0;
				g_UndoSysMasterStack[g_UndoList].push(masteritem);
			}
		}
	}
	else
	{
		// regular single master item on stack per event
		masteritem.type = mastertype;
		masteritem.count = 1;
		masteritem.gluebatchcount = 0;
		g_UndoSysMasterStack[g_UndoList].push(masteritem);
	}

	// create item for this event
	sUndoStackItem UndoStackItem;
	UndoStackItem.event = eventtype;
	UndoStackItem.pEventData = pEventData;

	// add item to relevant type stack
	switch (masteritem.type)
	{
		case eUndoSys_Object:	g_UndoSysObjectStack[g_UndoList].push(UndoStackItem); break;
		case eUndoSys_Terrain:  g_UndoSysTerrainStack[g_UndoList].push(UndoStackItem); break;
	}
}

void undosys_glue(int eList, int iGlueBatchCount)
{
	// used to connect two events together (i.e. terrain sculpt and objectposgroup events)
	sUndoMasterStackItem lastitem = g_UndoSysMasterStack[eList].top();
	g_UndoSysMasterStack[eList].pop();
	lastitem.gluebatchcount = iGlueBatchCount;
	g_UndoSysMasterStack[eList].push(lastitem);
}

// user functions

int g_iStoreIterationCountForLastRecurse = 0;

void undosys_undoredoevent_core ( eUndoMasterList eList, eUndoMasterList eListForRedo, int iIterations )
{
	// if something on master stack
	if (g_UndoSysMasterStack[eList].size() == 0)
		return;

	// pull from master list stack
	sUndoMasterStackItem masteritem;
	masteritem = g_UndoSysMasterStack[eList].top();
	g_UndoSysMasterStack[eList].pop();

	// system to glue master events together and undo/redo as a batch
	int iSetGlueValueOnLastEvent = 0;
	if (iIterations == -1)
	{
		iIterations = masteritem.gluebatchcount;
		g_iStoreIterationCountForLastRecurse = iIterations;
	}
	else
	{
		if (iIterations == 0 && g_iStoreIterationCountForLastRecurse > 0 )
		{
			iSetGlueValueOnLastEvent = g_iStoreIterationCountForLastRecurse;
			g_iStoreIterationCountForLastRecurse = 0;
		}
	}

	// start the redo stack entry
	if (masteritem.count > 1)
	{
		undosys_setlist(eListForRedo);
		undosys_multiplevents_start();
		undosys_setlist(eList);
	}

	// master stack item can specify multiple events
	for (int n = 0; n < masteritem.count; n++)
	{
		// pull event from specific undo stack
		sUndoStackItem item;
		switch (masteritem.type)
		{
		case eUndoSys_Object:
			item = g_UndoSysObjectStack[eList].top();
			g_UndoSysObjectStack[eList].pop();
			break;

		case eUndoSys_Terrain:
			item = g_UndoSysTerrainStack[eList].top();
			g_UndoSysTerrainStack[eList].pop();
			break;
		}

		// execute undo code for this specific event
		if (item.pEventData)
		{
			switch (masteritem.type)
			{
			case eUndoSys_Object:
			{
				// create a redo action before actually do the undo
				undosys_setlist(eListForRedo);
				extern void entity_createtheredoaction(eUndoEventType eventtype, void* pEventData);
				entity_createtheredoaction(item.event, item.pEventData);
				undosys_setlist(eList);

				// perform the undo action for this event
				extern void entity_performtheundoaction(eUndoEventType eventtype, void* pEventData);
				entity_performtheundoaction(item.event, item.pEventData);
				break;
			}
			case eUndoSys_Terrain:
			{
				#ifdef GGTERRAIN_UNDOREDO
				if (item.event == eUndoSys_Terrain_Grass)
				{
					// create a redo action before actually doing the undo 
					undosys_setlist(eListForRedo);
					extern void GGGrass_CreateUndoRedoAction(int eventtype, int eList, bool bUserAction = true, void* pEventData = nullptr);
					GGGrass_CreateUndoRedoAction(item.event, eListForRedo, false, item.pEventData);
					undosys_setlist(eList);

					// perform the undo action for this event
					extern void GGGrass_PerformUndoRedoAction(int type, void* pEventData, int eList);
					GGGrass_PerformUndoRedoAction(item.event, item.pEventData, eList);
				}
				else if (item.event >= eUndoSys_Terrain_AddTree && item.event <= eUndoSys_Terrain_PaintTree)
				{
					// create a redo action before actually doing the undo 
					undosys_setlist(eListForRedo);
					extern void GGTrees_CreateUndoRedoAction(int eventtype, int eList, bool bUserAction = true, void* pEventData = nullptr);
					GGTrees_CreateUndoRedoAction(item.event, eListForRedo, false, item.pEventData);
					undosys_setlist(eList);

					// perform the undo action for this event
					extern void GGTrees_PerformUndoRedoAction(int type, void* pEventData, int eList);
					GGTrees_PerformUndoRedoAction(item.event, item.pEventData, eList);
				}
				else if (item.event == eUndoSys_Terrain_SculptMovedObject) //special case of undoing objects that were moved automatically after sculpting.
				{
					sUndoSysEventTerrainMoveAfterSculpt* pEvent = (sUndoSysEventTerrainMoveAfterSculpt*)item.pEventData;
					if (pEvent)
					{
						// create a redo action before actually doing the undo 
						undosys_setlist(eListForRedo);
						int e = pEvent->entity;
						float y = t.entityelement[e].y;
						undosys_terrain_objectmovedaftersculpt(e, y, eListForRedo);

						// Perform the undo action.
						undosys_setlist(eList);

						float height = 0;
						memcpy(&height, pEvent->height, sizeof(float));
						t.entityelement[pEvent->entity].y = height;
						t.tte = pEvent->entity;
						t.tobj = t.entityelement[t.tte].obj;
						entity_positionandscale();

						uint8_t* pAddressOfEventData = pEvent->height;

						delete pEvent;
						pEvent = 0;

						//Mark the sculpt data memory for this event as unused (0 = undo, 1 = redo)
						extern UndoRedoMemory g_TerrainUndoMem;
						extern UndoRedoMemory g_TerrainRedoMem;
						if (eList == eUndoSys_UndoList)
							undosys_terrain_setmemoryunusedfrom(pAddressOfEventData, g_TerrainUndoMem);
						else
							undosys_terrain_setmemoryunusedfrom(pAddressOfEventData, g_TerrainRedoMem);
					}
				}
				else
				{
					// create a redo action before actually doing the undo 
					undosys_setlist(eListForRedo);
					extern void GGTerrain_CreateUndoRedoAction(int eventtype, int eList, bool bUserAction = true, void* pEventData = nullptr);
					GGTerrain_CreateUndoRedoAction(item.event, eListForRedo, false, item.pEventData);
					undosys_setlist(eList);

					// perform the undo action for this event
					extern void GGTerrain_PerformUndoRedoAction(int type, void* pEventData, int eList);
					GGTerrain_PerformUndoRedoAction(item.event, item.pEventData, eList);
				}
				
				#endif
				break;
			}
			}
		}
	}

	// finish the redo stack entry
	if (masteritem.count > 1)
	{
		undosys_setlist(eListForRedo);
		undosys_multiplevents_finish();
		undosys_setlist(eList);
	}

	// when building the redo list, ensure we preserve the glue batch count
	if (iSetGlueValueOnLastEvent > 0)
	{
		undosys_glue(eListForRedo, iSetGlueValueOnLastEvent);
	}

	// if glued to neightboring master event, undo that too
	if (iIterations > 0)
	{
		undosys_undoredoevent_core (eList, eListForRedo, iIterations-1);
	}
}

void undosys_clearredostack (void)
{
	// this is called when user makes a new manual action that invalidates all past 'future events'
	int redostacksize = g_UndoSysMasterStack[eUndoSys_RedoList].size();
	for (int n = 0; n < redostacksize; n++) g_UndoSysMasterStack[eUndoSys_RedoList].pop();

	// also need to clear type specific stacks and remove event data allocations
	redostacksize = g_UndoSysObjectStack[eUndoSys_RedoList].size();
	for (int n = 0; n < redostacksize; n++)
	{
		sUndoStackItem eventitem = g_UndoSysObjectStack[eUndoSys_RedoList].top();
		if (eventitem.pEventData) delete eventitem.pEventData;
		g_UndoSysObjectStack[eUndoSys_RedoList].pop();
	}
	redostacksize = g_UndoSysTerrainStack[eUndoSys_RedoList].size();
	for (int n = 0; n < redostacksize; n++)
	{
		sUndoStackItem eventitem = g_UndoSysTerrainStack[eUndoSys_RedoList].top();
		if (eventitem.pEventData) delete eventitem.pEventData;
		g_UndoSysTerrainStack[eUndoSys_RedoList].pop();
	}
}

void undosys_undoevent (void)
{
	// undo from undo stack, and adds to redo stack
	undosys_undoredoevent_core (eUndoSys_UndoList, eUndoSys_RedoList, -1);
	undosys_setlist(eUndoSys_UndoList);
}

void undosys_redoevent (void)
{
	// undo from redo stack, and adds to undo stack
	undosys_undoredoevent_core (eUndoSys_RedoList, eUndoSys_UndoList, -1);
	undosys_setlist(eUndoSys_UndoList);
}