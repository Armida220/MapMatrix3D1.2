#include "StdAfx.h"
#include "OsgGeodeFinder.h"

void COsgGeodeFinder::apply(osg::Geode& geode)
{
	bFindGeode = true;                                            /////�޸ĳɱ���
	traverse(geode);
}