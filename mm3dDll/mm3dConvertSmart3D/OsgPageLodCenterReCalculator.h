#pragma once
#include <osg/NodeVisitor>
#include <osg/PagedLOD>
#include <osg/Geode>

class CPageLodCenterReCalculator : public osg::NodeVisitor
{
public:
	CPageLodCenterReCalculator() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{

	}

protected:
	virtual void apply(osg::PagedLOD& pagedLod);
	virtual void apply(osg::Node& node);

};

