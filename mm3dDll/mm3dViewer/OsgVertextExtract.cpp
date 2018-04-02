#include "StdAfx.h"
#include <osg/Texture2D>
#include "OsgVertextExtract.h"
#include <osg/Geode>
#include <osg/Geometry>

COsgVertextExtract::COsgVertextExtract():osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
	extracted_verts = new osg::Vec3Array; 
}

COsgVertextExtract::~COsgVertextExtract(void)
{
}

void COsgVertextExtract::ExtractVerts(osg::ref_ptr<osg::Node> node, osg::ref_ptr<osg::Vec3Array> &extracted_verts)
{
	COsgVertextExtract ive;
	node->accept(ive);
	extracted_verts = ive.extracted_verts;
}

//apply 
void COsgVertextExtract::apply(osg::Geode& geode)
{ 
	//�����ж��ٿɻ��ƽ�� 
	int numDrawables = geode.getNumDrawables();

	for( unsigned int i=0; i < geode.getNumDrawables(); ++i ) 
	{ 
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i)); 

		if( !geom ) 
			continue; //�õ��ɻ��ƽ��Ķ������� 
		
		osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray()); 

		if( !verts ) 
			continue; //�Ѷ������в��뵽���㼯���Ա���� 
		extracted_verts->insert(extracted_verts->end(), verts->begin(), verts->end()); 
	} 
}
