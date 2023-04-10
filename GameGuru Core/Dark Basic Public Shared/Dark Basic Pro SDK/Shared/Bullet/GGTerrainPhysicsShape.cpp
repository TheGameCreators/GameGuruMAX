/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "GGTerrainPhysicsShape.h"
#include "LinearMath/btTransformUtil.h"
#include "..\..\..\..\GameGuru\Include\preprocessor-flags.h"

#ifdef PRODUCTCONVERTER
#else
namespace GGTerrain
{
	int GGTerrain_Physics_GetBounds( float* minX, float* minY, float* minZ, float* maxX, float* maxY, float* maxZ );
	void GGTerrain_Physics_ProcessVertices( void* callback, float worldToPhysScale, float minX, float minY, float minZ, float maxX, float maxY, float maxZ );
	void GGTerrain_Physics_RayCast( void* callback, float worldToPhysScale, float srcX, float srcY, float srcZ, float dstX, float dstY, float dstZ );
}
#endif

GGTerrainPhysicsShape::GGTerrainPhysicsShape( float scale )
{
	m_shapeType = GGTERRAIN_SHAPE_PROXYTYPE;
	m_physToWorldScale = scale;
	m_worldToPhysScale = 1.0f / m_physToWorldScale;
}

GGTerrainPhysicsShape::~GGTerrainPhysicsShape()
{
	
}

void GGTerrainPhysicsShape::getAabb(const btTransform& t, btVector3& aabbMin, btVector3& aabbMax) const
{
	/*
	float minX, minY, minZ, maxX, maxY, maxZ;
	GGTerrain::GGTerrain_Physics_GetBounds( &minX, &minY, &minZ, &maxX, &maxY, &maxZ );

	btVector3 minExtents, maxExtents;
	
	minExtents.setValue(minX, minY, minZ);
	maxExtents.setValue(maxX, maxY, maxZ);

	minExtents *= m_worldToPhysScale;
	maxExtents *= m_worldToPhysScale;
		
	minExtents -= btVector3(getMargin(), getMargin(), getMargin());
	maxExtents += btVector3(getMargin(), getMargin(), getMargin());

	btMatrix3x3 matRot = t.getBasis();
	btVector3 center = t.getOrigin();
	
	minExtents = minExtents.dot3( matRot[0], matRot[1], matRot[2] );
	maxExtents = maxExtents.dot3( matRot[0], matRot[1], matRot[2] );
	
	aabbMin = minExtents;
	aabbMin.setMin( maxExtents );
	aabbMin += center;

	aabbMax = maxExtents;
	aabbMax.setMax( minExtents );
	aabbMax += center;
	*/

	btVector3 center = t.getOrigin();

	aabbMin.setValue( -1000000.0f, -1000000.0f, -1000000.0f );
	aabbMin += center;

	aabbMax.setValue( 1000000.0f, 1000000.0f, 1000000.0f );
	aabbMax += center;
}

/// process all triangles within the provided axis-aligned bounding box
/**
  basic algorithm:
    - convert input aabb to local coordinates (scale down and shift for local origin)
    - convert input aabb to a range of heightfield grid points (quantize)
    - iterate over all triangles in that subset of the grid
 */
void GGTerrainPhysicsShape::processAllTriangles(btTriangleCallback* callback, const btVector3& aabbMin, const btVector3& aabbMax) const
{
	btVector3 localAabbMin = aabbMin;
	btVector3 localAabbMax = aabbMax;

	localAabbMin *= m_physToWorldScale;
	localAabbMax *= m_physToWorldScale;

	#ifdef PRODUCTCONVERTER
	#else
	GGTerrain::GGTerrain_Physics_ProcessVertices( callback, m_worldToPhysScale, localAabbMin.getX(), localAabbMin.getY(), localAabbMin.getZ(),
												  localAabbMax.getX(), localAabbMax.getY(), localAabbMax.getZ() );
	#endif
}

void GGTerrainPhysicsShape::calculateLocalInertia(btScalar, btVector3& inertia) const
{
	//moving concave objects not supported
	inertia.setValue( btScalar(0.0), btScalar(0.0), btScalar(0.0) );
}

void GGTerrainPhysicsShape::setLocalScaling(const btVector3& scaling)
{
	// not supported
}
const btVector3& GGTerrainPhysicsShape::getLocalScaling() const
{
	return btVector3( 1.0f, 1.0f, 1.0f );
}

namespace
{
	struct GridRaycastState
	{
		int x;  // Next quad coords
		int z;
		int prev_x;  // Previous quad coords
		int prev_z;
		btScalar param;      // Exit param for previous quad
		btScalar prevParam;  // Enter param for previous quad
		btScalar maxDistanceFlat;
		btScalar maxDistance3d;
	};
}

// TODO Does it really need to take 3D vectors?
/// Iterates through a virtual 2D grid of unit-sized square cells,
/// and executes an action on each cell intersecting the given segment, ordered from begin to end.
/// Initially inspired by http://www.cse.yorku.ca/~amana/research/grid.pdf
template <typename Action_T>
void gridRaycast(Action_T& quadAction, const btVector3& beginPos, const btVector3& endPos)
{
	GridRaycastState rs;
	rs.maxDistance3d = beginPos.distance(endPos);
	if (rs.maxDistance3d < 0.0001)
	{
		// Consider the ray is too small to hit anything
		return;
	}
	

	btScalar rayDirectionFlatX = endPos[0] - beginPos[0];
	btScalar rayDirectionFlatZ = endPos[2] - beginPos[2];
	rs.maxDistanceFlat = btSqrt(rayDirectionFlatX * rayDirectionFlatX + rayDirectionFlatZ * rayDirectionFlatZ);

	if (rs.maxDistanceFlat < 0.0001)
	{
		// Consider the ray vertical
		rayDirectionFlatX = 0;
		rayDirectionFlatZ = 0;
	}
	else
	{
		rayDirectionFlatX /= rs.maxDistanceFlat;
		rayDirectionFlatZ /= rs.maxDistanceFlat;
	}

	const int xiStep = rayDirectionFlatX > 0 ? 1 : rayDirectionFlatX < 0 ? -1 : 0;
	const int ziStep = rayDirectionFlatZ > 0 ? 1 : rayDirectionFlatZ < 0 ? -1 : 0;

	const float infinite = 9999999;
	const btScalar paramDeltaX = xiStep != 0 ? 1.f / btFabs(rayDirectionFlatX) : infinite;
	const btScalar paramDeltaZ = ziStep != 0 ? 1.f / btFabs(rayDirectionFlatZ) : infinite;

	// pos = param * dir
	btScalar paramCrossX;  // At which value of `param` we will cross a x-axis lane?
	btScalar paramCrossZ;  // At which value of `param` we will cross a z-axis lane?

	// paramCrossX and paramCrossZ are initialized as being the first cross
	// X initialization
	if (xiStep != 0)
	{
		if (xiStep == 1)
		{
			paramCrossX = ((float)ceil(beginPos[0]) - beginPos[0]) * paramDeltaX;
		}
		else
		{
			paramCrossX = (beginPos[0] - (float)floor(beginPos[0])) * paramDeltaX;
		}
	}
	else
	{
		paramCrossX = infinite;  // Will never cross on X
	}

	// Z initialization
	if (ziStep != 0)
	{
		if (ziStep == 1)
		{
			paramCrossZ = ((float)ceil(beginPos[2]) - beginPos[2]) * paramDeltaZ;
		}
		else
		{
			paramCrossZ = (beginPos[2] - (float)floor(beginPos[2])) * paramDeltaZ;
		}
	}
	else
	{
		paramCrossZ = infinite;  // Will never cross on Z
	}

	rs.x = static_cast<int>(floor(beginPos[0]));
	rs.z = static_cast<int>(floor(beginPos[2]));

	// Workaround cases where the ray starts at an integer position
	if (paramCrossX == 0.0)
	{
		paramCrossX += paramDeltaX;
		// If going backwards, we should ignore the position we would get by the above flooring,
		// because the ray is not heading in that direction
		if (xiStep == -1)
		{
			rs.x -= 1;
		}
	}

	if (paramCrossZ == 0.0)
	{
		paramCrossZ += paramDeltaZ;
		if (ziStep == -1)
			rs.z -= 1;
	}

	rs.prev_x = rs.x;
	rs.prev_z = rs.z;
	rs.param = 0;

	while (true)
	{
		rs.prev_x = rs.x;
		rs.prev_z = rs.z;
		rs.prevParam = rs.param;

		if (paramCrossX < paramCrossZ)
		{
			// X lane
			rs.x += xiStep;
			// Assign before advancing the param,
			// to be in sync with the initialization step
			rs.param = paramCrossX;
			paramCrossX += paramDeltaX;
		}
		else
		{
			// Z lane
			rs.z += ziStep;
			rs.param = paramCrossZ;
			paramCrossZ += paramDeltaZ;
		}

		if (rs.param > rs.maxDistanceFlat)
		{
			rs.param = rs.maxDistanceFlat;
			quadAction(rs);
			break;
		}
		else
		{
			quadAction(rs);
		}
	}
}

void GGTerrainPhysicsShape::performRaycast(btTriangleRaycastCallback* callback, const btVector3& raySource, const btVector3& rayTarget) const
{
	// Transform to cell-local
	btVector3 beginPos = raySource * m_physToWorldScale;
	btVector3 endPos = rayTarget * m_physToWorldScale;
	
	#ifdef PRODUCTCONVERTER
	#else
	btVector3 meshScaling = getLocalScaling();
	GGTerrain::GGTerrain_Physics_RayCast( callback, m_worldToPhysScale, beginPos.getX(), beginPos.getY(), beginPos.getZ(), endPos.getX(), endPos.getY(), endPos.getZ() );
	#endif
}
