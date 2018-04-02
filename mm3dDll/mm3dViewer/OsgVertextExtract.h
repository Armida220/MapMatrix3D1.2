#pragma once
#include <osg/NodeVisitor>
#include "osg/Node"
#include "osg/Vec3"

class COsgVertextExtract : public osg::NodeVisitor
{
public: //���еĶ��� 
	osg::ref_ptr<osg::Vec3Array> extracted_verts; 
	osg::Matrix matrix;

	void ExtractVerts(osg::ref_ptr<osg::Node> node, osg::ref_ptr<osg::Vec3Array> &extracted_verts);

public:
	COsgVertextExtract();
	void apply(osg::Geode& geode);
	~COsgVertextExtract(void);
};

