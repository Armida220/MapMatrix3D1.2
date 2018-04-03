#include "stdafx.h"
#include "NodeAddOperator.h"

using namespace mm3dView;

CNodeAddOperator::CNodeAddOperator(osg::ref_ptr<osg::Group> parent, 
	osg::ref_ptr<osg::Node> child, int id) :
	spParent(parent), spChild(child), mId(id)
{
}


CNodeAddOperator::~CNodeAddOperator()
{
}

void CNodeAddOperator::operator()()
{
	if (spParent && spChild)
	{
		spParent->addChild(spChild);
	}
}
