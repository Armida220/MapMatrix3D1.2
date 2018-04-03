#pragma once
#include <osg/MatrixTransform>  
#include <osg/Projection>  
#include <osg/Vec4>  

#include <osg/NodeVisitor>  
#include <osg/ClipPlane>  
#include <osg/Viewport>  
class Compass :
	public osg::Projection
{
public:
	Compass(osg::Viewport *vp);

protected:

	virtual void traverse(osg::NodeVisitor&);
	//static const osg::Vec4 color;  

private:
	osg::ref_ptr<osg::MatrixTransform> _tx;
	osg::ref_ptr<osg::Geode> _makeGeode();
};

