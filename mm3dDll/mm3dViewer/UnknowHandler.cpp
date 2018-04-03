#include "stdafx.h"
#include "UnknowHandler.h"
#include <osg/Group>
#include <osg/Vec3>
#include <osg/Matrix>

using namespace osgEvent;
using namespace osg;


CUnknowHandler::CUnknowHandler(Group *root, Group* mapGroup, Group* editGroup,
	Group* tempGroup, COsgUnknowEvent* parent)
	: mRoot(root),
	mEditGroup(editGroup),
	mTempGroup(tempGroup),
	mMapGroup(mapGroup),	
	mParent(parent)
{
}


CUnknowHandler::~CUnknowHandler()
{
}


void CUnknowHandler::lfClk(const Vec3d &clkPt)
{

}

void CUnknowHandler::mouseMv(const Vec3d &clkPt)
{
	return;
}

void CUnknowHandler::dbClk(const Vec3d &clkPt)
{

}

bool CUnknowHandler::rightClk(const Vec3d &clkPt)
{

	return true;
}
