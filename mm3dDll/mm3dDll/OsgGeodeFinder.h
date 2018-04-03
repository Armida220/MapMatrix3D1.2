#pragma once
#include <osg/NodeVisitor>
#include <osg/Geode>

class COsgGeodeFinder : public osg::NodeVisitor
{

public:
	COsgGeodeFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{
		bFindGeode = false;
	}

	virtual void apply(osg::Geode& geode);

public:
	bool bFindGeode;
};

