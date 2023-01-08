#ifndef GGRECASTDETOUR_H
#define GGRECASTDETOUR_H

#include "InputGeom.h"
#include "Sample.h"
#include "Sample_SoloMesh.h"
#include "Sample_TileMesh.h"
#include "NavMeshTesterTool.h"

class GGRecastDetour
{
public:
	GGRecastDetour();
	void freeall (void);
	int buildall (float* pVertices, uint32_t numVertices);
	int findPath (float fStart[3], float fEnd[3]);
	int getPath (int* piPointCount, float** ppfPointData);
	bool isWithinNavMeshEx(float fX, float fY, float fZ, float* pvecNearestPt, bool bMustBeOverPoly);
	bool isWithinNavMesh (float fX, float fY, float fZ);
	float getYFromPos (float fX, float fY, float fZ);
	void handleDebugRender (void);
	void cleanupDebugRender(void);

	void TogglePolys( float x, float y, float z, float radius, bool enable );

private:
	// core classes
	BuildContext ctx;
	Sample* sample;
	InputGeom* geom;
	NavMeshTesterTool* tool;
};

#endif // GGRECASTDETOUR_H
