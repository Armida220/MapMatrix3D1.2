#include "stdafx.h"
#include "CameraPanTo.h"
#include <osg/Vec3d>
#include <osg/Matrix>
#include "SelfDefTerrainManipulator.h"

using namespace mm3dView;
using namespace osg;



CCameraPanTo::CCameraPanTo(CSelfDefTerrainManipulator* manipulator,
	double x, double y) 
	: mManipulator(manipulator),
	mX(x), mY(y)
{
}


CCameraPanTo::~CCameraPanTo()
{
}


void CCameraPanTo::operator()()
{
	mManipulator->panToXY(mX, mY);
}
