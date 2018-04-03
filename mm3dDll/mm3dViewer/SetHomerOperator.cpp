#include "stdafx.h"
#include "SetHomerOperator.h"
#include <osg/ComputeBoundsVisitor>
#include "OsgModel.h"
#include "osgGA/TerrainManipulator"


using namespace osg;
using namespace mm3dView;
using namespace osgGA;

CSetHomerOperator::CSetHomerOperator(ref_ptr<TerrainManipulator> terrain, 
	COsgModel* osgbModel) : mTerrain(terrain), mOsgbModel(osgbModel)
{
}


CSetHomerOperator::~CSetHomerOperator()
{
}

void CSetHomerOperator::operator()()
{
	BoundingBox bb = mOsgbModel->calBox();
	Vec3d homeCenter = bb.center() + 
		Vec3(0.0,-1.5f * bb.radius(), 2.f * bb.radius());
		//Vec3(1.5f * bb.radius(), 1.5f * bb.radius(), 1.5f * bb.radius());
	mTerrain->setHomePosition(homeCenter,
		bb.center(), Vec3(0.0f, 0.0f, 1.0f));
}
