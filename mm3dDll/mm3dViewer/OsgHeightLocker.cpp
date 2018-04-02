#include "stdafx.h"
#include "OsgHeightLocker.h"
#include <osg/Vec3d>
#include <osgGA/GUIEventAdapter>
#include "OsgPicker.h"

using namespace osg;
using namespace osgEvent;
using namespace osgGA;
using namespace std;

COsgHeightLocker::COsgHeightLocker():
	bIsLockHeight(false),
	lockHeight(0)
{
}


COsgHeightLocker::~COsgHeightLocker()
{
}


void COsgHeightLocker::lfClk(Vec3d &clkPt)
{
	replace(clkPt);
}

void COsgHeightLocker::mouseMv(Vec3d &clkPt)
{
	replace(clkPt);
	return;
}

void COsgHeightLocker::dbClk(const Vec3d &clkPt)
{
	return;
}

bool COsgHeightLocker::rightClk(const Vec3d &clkPt)
{
	return true;
}

bool COsgHeightLocker::keyDown(const GUIEventAdapter &ea)
{
	return true;
}

void COsgHeightLocker::setHeight(double h)
{
	if (bIsLockHeight)
	{
		lockHeight = h;
	}
	
}

void COsgHeightLocker::replace(Vec3d &clkPt)
{
	if (bIsLockHeight)
	{
		clkPt.z() = lockHeight;
	}
}


bool COsgHeightLocker::shift()
{
	bIsLockHeight = !bIsLockHeight;
	return bIsLockHeight;
}

