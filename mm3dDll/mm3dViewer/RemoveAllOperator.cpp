#include "stdafx.h"
#include "RemoveAllOperator.h"

using namespace mm3dView;
using namespace osg;

CRemoveAllOperator::CRemoveAllOperator(Group* group)
	: mGroup(group)
{
}


CRemoveAllOperator::~CRemoveAllOperator()
{
}


void CRemoveAllOperator::operator()()
{
	int numGroupChild = mGroup->getNumChildren();
	mGroup->removeChildren(0, numGroupChild);
}

