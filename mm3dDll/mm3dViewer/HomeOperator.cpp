#include "stdafx.h"
#include "HomeOperator.h"
#include <osgGA/TerrainManipulator>

using namespace mm3dView;
using namespace osgGA;
using namespace osg;

CHomeOperator::CHomeOperator(ref_ptr<TerrainManipulator> terrain)
	: mTerrain(terrain)
{
}


CHomeOperator::~CHomeOperator()
{
}

void CHomeOperator::operator()()
{
	mTerrain->home(1);
}
