#include "stdafx.h"
#include "CameraZoomTo.h"
#include "SelfDefTerrainManipulator.h"

using namespace mm3dView;

CCameraZoomTo::CCameraZoomTo(CSelfDefTerrainManipulator* manipulator,
	double scale) 
	: mManipulator(manipulator), 
	  mScale(scale)
{
}


CCameraZoomTo::~CCameraZoomTo()
{
}


void CCameraZoomTo::operator()()
{
	mManipulator->zoomTo(mScale);
}
