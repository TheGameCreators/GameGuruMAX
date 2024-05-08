#include "BulletDebugDrawer.h"
#include "gameguru.h"
#include <mutex>

BulletDebugDrawer::BulletDebugDrawer()
{
	elementCount = 0; 
	mode = 0; 
	drawTransforms = false;
}

std::mutex lock;
void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	//PE: This is not threadsafe so.
	lock.lock();

	//PE: This can keep running even if debug not set, so make it safe.
	if (lineData.size() > 1000000)
	{
		BPhys_ClearDebugDrawData();
	}
	if (mode == 0)
	{
		lock.unlock();
		return;
	}
	if (elementCount + 6 <= lineData.size())
	{
		lineData[elementCount + 0] = from.x() * scale;
		lineData[elementCount + 1] = from.y()* scale;
		lineData[elementCount + 2] = from.z()* scale;
		lineData[elementCount + 3] = to.x()* scale;
		lineData[elementCount + 4] = to.y()* scale;
		lineData[elementCount + 5] = to.z()* scale;
		//lineData[elementCount + 6] = color.x();
		//lineData[elementCount + 7] = color.y();
		//lineData[elementCount + 8] = color.z();
		elementCount += 6;
		lock.unlock();
		return;
	}

	lineData.push_back(from.x()* scale);
	lineData.push_back(from.y()* scale);
	lineData.push_back(from.z()* scale);
	lineData.push_back(to.x()* scale);
	lineData.push_back(to.y()* scale);
	lineData.push_back(to.z()* scale);
	//lineData.push_back(color.x());
	//lineData.push_back(color.y());
	//lineData.push_back(color.z());
	elementCount += 6;
	lock.unlock();
}

void BulletDebugDrawer::drawTransform(const btTransform& transform, btScalar orthoLen)
{
#ifndef WICKEDENGINE
	btIDebugDraw::drawTransform(transform, orthoLen);
	return;
#endif

	if(drawTransforms) btIDebugDraw::drawTransform(transform, orthoLen);
}

void BulletDebugDrawer::drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color)
{
}

void BulletDebugDrawer::reportErrorWarning(const char * warningString)
{
}

void BulletDebugDrawer::draw3dText(const btVector3 & location, const char * textString)
{
}

void BulletDebugDrawer::setDebugMode(int debugMode)
{
	mode = debugMode;
}

int BulletDebugDrawer::getDebugMode() const
{
	return mode;
} 

void BulletDebugDrawer::SetWorldScale(float fScale)
{
	scale = fScale;
}
