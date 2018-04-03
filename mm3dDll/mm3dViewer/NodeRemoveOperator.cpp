#include "stdafx.h"
#include "NodeRemoveOperator.h"

using namespace mm3dView;

CNodeRemoveOperator::CNodeRemoveOperator(osg::ref_ptr<osg::Group> parent, 
	osg::ref_ptr<osg::Node> child, int id) :
	spParent(parent), spChild(child), mId(id)
{
}


CNodeRemoveOperator::~CNodeRemoveOperator()
{
}


void CNodeRemoveOperator::operator()()
{
	spParent->removeChild(spChild);
}
