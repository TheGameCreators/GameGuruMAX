//----------------------------------------------------
//--- GAMEGURU - M-UndoSys Object
//----------------------------------------------------

// Includes
#include <stack>
#include <vector>
#include <deque>
#include "K3D_Vector3D.h"
#include "M-UndoSys.h"

// Externs
extern std::stack<sUndoStackItem> g_UndoSysObjectStack[2];
extern bool g_UndoSysObjectIsBeingMoved;
extern bool g_UndoSysObjectRememberBeforeMove;
extern int g_UndoSysObjectRememberBeforeMoveE;
extern float g_UndoSysObjectRememberBeforeMovePX;
extern float g_UndoSysObjectRememberBeforeMovePY;
extern float g_UndoSysObjectRememberBeforeMovePZ;
extern float g_UndoSysObjectRememberBeforeMoveRX;
extern float g_UndoSysObjectRememberBeforeMoveRY;
extern float g_UndoSysObjectRememberBeforeMoveRZ;
extern float g_UndoSysObjectRememberBeforeMoveQuatMode;
extern float g_UndoSysObjectRememberBeforeMoveQuatX;
extern float g_UndoSysObjectRememberBeforeMoveQuatY;
extern float g_UndoSysObjectRememberBeforeMoveQuatZ;
extern float g_UndoSysObjectRememberBeforeMoveQuatW;
extern float g_UndoSysObjectRememberBeforeMoveSX;
extern float g_UndoSysObjectRememberBeforeMoveSY;
extern float g_UndoSysObjectRememberBeforeMoveSZ;
struct sRubberBandType;

// Object Add
struct sUndoSysEventObjectAdd
{
	int iEntityElementIndex;
};
void undosys_object_add (int iEntityElementIndex);

// Object Delete
struct sUndoSysEventObjectDelete
{
	int e;
	int gridentity;
	int grideleprof_trigger_waypointzoneindex;
	int gridentityeditorfixed;
	int entitymaintype;
	int entitybankindex;
	int gridentitystaticmode;
	int gridentityhasparent;
	float gridentityposx_f;
	float gridentityposz_f;
	float gridentityposy_f;
	float gridentityrotatex_f;
	float gridentityrotatey_f;
	float gridentityrotatez_f;
	int gridentityrotatequatmode;
	float gridentityrotatequatx_f;
	float gridentityrotatequaty_f;
	float gridentityrotatequatz_f;
	float gridentityrotatequatw_f;
	float gridentityscalex_f;
	float gridentityscaley_f;
	float gridentityscalez_f;
};
void undosys_object_delete (int e, int waypointzoneindex, int editorfixed, int maintype, int bankindex, int staticflag, int iHasParentIndex, float x, float y, float z, float rx, float ry, float rz, int quatmode, float quatx, float quaty, float quatz, float quatw, float scalex, float scaley, float scalez);
void undosys_object_deletewaypoint(int waypointindex, int e);

// Object Change PosRotScl
struct sUndoSysEventObjectChangePosRotScl
{
	int e;
	float posx_f;
	float posz_f;
	float posy_f;
	float rotatex_f;
	float rotatey_f;
	float rotatez_f;
	int rotatequatmode;
	float rotatequatx_f;
	float rotatequaty_f;
	float rotatequatz_f;
	float rotatequatw_f;
	float scalex_f;
	float scaley_f;
	float scalez_f;
};
void undosys_object_changeposrotscl (int e, float x, float y, float z, float rx, float ry, float rz, int quatmode, float quatx, float quaty, float quatz, float quatw, float scalex, float scaley, float scalez);
//bool undosys_object_changeposrotscl_erasefromstackifsame (int e, float x, float y, float z, float rx, float ry, float rz, float quatx, float quaty, float quatz, float quatw, float scalex, float scaley, float scalez);

struct sUndoSysEventObjectDeleteWaypoint
{
	waypointtype waypoint;
	waypointcoordtype waypointcoords[6] = {};
	int e;
};

struct sUndoSysEventObjectGroup
{
	int groupindex;
	/*std::vector<sRubberBandType> groupData;*/
};
void undosys_object_group(int index);

struct sUndoSysEventObjectUnGroup
{
	int groupindex;
	std::vector<sRubberBandType> groupData;

};
void undosys_object_ungroup(int index, std::vector<sRubberBandType> data);