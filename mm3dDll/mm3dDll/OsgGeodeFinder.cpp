#include "StdAfx.h"
#include "OsgGeodeFinder.h"

void COsgGeodeFinder::apply(osg::Geode& geode)
{
	bFindGeode = true;                                            /////ÐÞ¸Ä³É±£´æ
	traverse(geode);
}