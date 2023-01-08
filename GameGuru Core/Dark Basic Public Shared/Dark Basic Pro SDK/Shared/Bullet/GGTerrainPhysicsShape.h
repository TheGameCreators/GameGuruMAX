#ifndef _H_GGTERRAIN_PHYSICS_SHAPE
#define _H_GGTERRAIN_PHYSICS_SHAPE

#include "BulletCollision/CollisionShapes/btConcaveShape.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

ATTRIBUTE_ALIGNED16(class)
GGTerrainPhysicsShape : public btConcaveShape
{
public:
	struct Range
	{
		Range() {}
		Range(btScalar minin, btScalar maxin) { min = minin; max = maxin; }

		bool overlaps(const Range& other) const
		{
			return !(min > other.max || max < other.min);
		}

		btScalar min;
		btScalar max;
	};

protected:
	float m_physToWorldScale;
	float m_worldToPhysScale;

public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	GGTerrainPhysicsShape( float scale );

	virtual ~GGTerrainPhysicsShape();

	virtual void getAabb(const btTransform& t, btVector3& aabbMin, btVector3& aabbMax) const;

	virtual void processAllTriangles(btTriangleCallback * callback, const btVector3& aabbMin, const btVector3& aabbMax) const;

	virtual void calculateLocalInertia(btScalar mass, btVector3 & inertia) const;

	virtual void setLocalScaling(const btVector3& scaling);

	virtual const btVector3& getLocalScaling() const;

	void performRaycast(btTriangleRaycastCallback * callback, const btVector3& raySource, const btVector3& rayTarget) const;

	//debugging
	virtual const char* getName() const { return "GGTERRAIN"; }

	
	
};

#endif  //BT_HEIGHTFIELD_TERRAIN_SHAPE_H
