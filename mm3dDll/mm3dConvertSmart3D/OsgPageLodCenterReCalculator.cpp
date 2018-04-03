#include "stdafx.h"
#include "OsgPageLodCenterReCalculator.h"


void CPageLodCenterReCalculator::apply(osg::PagedLOD& pagedLod)
{
	osg::Vec3d oldCenter = pagedLod.getCenter();

	pagedLod.setCenterMode(osg::LOD::CenterMode::USE_BOUNDING_SPHERE_CENTER);
	osg::BoundingSphere bs = pagedLod.computeBound();
	osg::Vec3d center = bs.center();

	pagedLod.setCenter(center);
	traverse(pagedLod);
}

void CPageLodCenterReCalculator::apply(osg::Node& node)
{
	//osg::Vec3d oldCenter = geode.getBound().center();

	//node.setCenterMode(osg::LOD::CenterMode::USE_BOUNDING_SPHERE_CENTER);
	
	osg::BoundingSphere bs = node.computeBound();
	osg::Vec3d center = bs.center();
	
	//geode.setInitialBound(bs);
	traverse(node);
}