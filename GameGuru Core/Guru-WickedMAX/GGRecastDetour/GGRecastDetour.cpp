// Includes
#include "GGRecastDetour.h"
#include "CFileC.h"
#include "DetourCommon.h"

// main functions
GGRecastDetour::GGRecastDetour()
{
	sample = 0;
	geom = 0;
	tool = 0;
}

void GGRecastDetour::freeall (void)
{
	if (tool)
	{
		delete tool;
		tool = 0;
	}
	if (sample)
	{
		delete sample;
		sample = 0;
	}
	if (geom)
	{
		delete geom;
		geom = 0;
	}
}

int GGRecastDetour::buildall (float* pVertices, uint32_t numVertices)
{
	// free any previous resources
	freeall();

	// create simple solo mesh sample
	//sample = new Sample_SoloMesh();
	sample = new Sample_TileMesh();
	if (sample)
	{
		// initialise debug objects in sample class
		sample->getDebugDraw().init();

		// set context for sample
		sample->setContext(&ctx);

		// create geom to hold level data
		geom = new InputGeom;
		sample->handleMeshChanged(geom);

		if ( pVertices )
		{
			if (!geom->loadData(&ctx, pVertices, numVertices))
			{
				// error
				delete geom;
				geom = 0;
				delete sample;
				sample = 0;
				return 0;
			}
		}
		else
		{
			// load data into geom
			char szRealFilename[2048];
			strcpy_s(szRealFilename, 2048, "levelbank\\testmap\\rawlevelgeometry.obj");
			GG_GetRealPath(szRealFilename, 1);
			std::string meshName = szRealFilename;
			if (!geom->load(&ctx, meshName))
			{
				// error
				delete geom;
				geom = 0;
				delete sample;
				sample = 0;
				return 0;
			}
		}

		// if have sample and geom, build nav mesh
		if (geom && sample)
		{
			sample->handleSettings();

			// build navmesh
			ctx.resetLog();
			if (!sample->handleBuild())
			{
				// failed
			}
			ctx.dumpLog("Build log");
		}
	}

	// create navmesh tool and init with build sample
	tool = new NavMeshTesterTool;
	if (tool) tool->init(sample);

	// success
	return 1;
}

int GGRecastDetour::findPath (float fStart[3], float fEnd[3])
{
	if (tool)
	{
		// find path through nav mesh
		tool->findAPath (fStart, fEnd);

		// success
		return 1;
	}

	// failure
	return 0;
}

int GGRecastDetour::getPath (int* piPointCount, float** ppfPointData)
{
	if (tool)
	{
		// resulting path
		tool->getPathResult(piPointCount, ppfPointData);
		if (*ppfPointData != NULL)
		{
			// success
			return 1;
		}
	}

	// failure
	return 0;
}

bool GGRecastDetour::isWithinNavMeshEx (float fX, float fY, float fZ, float* pvecNearestPt, bool bMustBeOverPoly)
{
	if (tool)
	{ 
		bool pIsOverPoly = false;
		if (tool->isWithinNavMeshEx(fX, fY, fZ, pvecNearestPt, &pIsOverPoly) == true)
		{
			if (bMustBeOverPoly == true)
			{
				if (pIsOverPoly == true)
				{
					return true;
				}
			}
			else
			{
				return true;
			}
		}
		return false;
	}
	else
		return false;
}

bool GGRecastDetour::isWithinNavMesh (float fX, float fY, float fZ)
{
	return isWithinNavMeshEx(fX, fY, fZ, 0, 0);
}

float GGRecastDetour::getYFromPos (float fX, float fY, float fZ)
{
	if (tool)
		return tool->getYFromPos(fX, fY, fZ);
	else
		return fY;
}

void GGRecastDetour::handleDebugRender(void)
{
	if (sample)
	{
		sample->handleRender();
	}
	if (tool)
	{
		tool->handleRender();
	}
}

void GGRecastDetour::cleanupDebugRender(void)
{
	if (sample)
	{
		sample->GetDD()->hideDebugObjects();
	}
}

// radius of 15 seems about right
void GGRecastDetour::TogglePolys( float x, float y, float z, float radius, bool enable )
{
	float p[ 3 ] = { x, y, z };
	
	if ( !sample ) return;
	dtNavMesh* nav = sample->getNavMesh();
	dtNavMeshQuery* navquery = sample->getNavMeshQuery();
	if (nav && navquery)
	{
		dtQueryFilter filter;
		const float halfExtents[ 3 ] = { radius, radius, radius };
			
		int numFound = 0;
		const int findMax = 100;
		dtPolyRef ref[ findMax ];
			
		navquery->queryPolygons(p, halfExtents, &filter, ref, &numFound, findMax);
		for( int i = 0; i < numFound; i++ )
		{
			const dtPoly* pPoly = 0;
			const dtMeshTile* pTile = 0;
			nav->getTileAndPolyByRefUnsafe( ref[i], &pTile, &pPoly );

			float closest[3];
			unsigned short vert1 = pPoly->verts[0];
			unsigned short vert2 = pPoly->verts[1];
			unsigned short vert3 = pPoly->verts[2];
			float* a = &pTile->verts[ vert1*3 ];
			float* b = &pTile->verts[ vert2*3 ];
			float* c = &pTile->verts[ vert3*3 ];
			dtClosestPtPointTriangle(closest, p, a, b, c );

			float dist = dtVdistSqr( closest, p );
			if ( dist > radius*radius ) continue;

			unsigned short flags = 0;
			if (dtStatusSucceed(nav->getPolyFlags(ref[i], &flags)))
			{
				if ( enable ) flags |= SAMPLE_POLYFLAGS_DISABLED;
				else flags &= ~SAMPLE_POLYFLAGS_DISABLED;
				nav->setPolyFlags(ref[i], flags);
			}
		}
	}
}