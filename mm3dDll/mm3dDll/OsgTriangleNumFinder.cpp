#include "StdAfx.h"
#include "OsgTriangleNumFinder.h"
#include <osg/Geometry>

using namespace osg;

void COsgTriangleNumFinder::apply(Geode& geode)
{
	for (int i = 0; i < geode.getNumDrawables(); i++)
	{
		ref_ptr<Geometry> geom = dynamic_cast<Geometry*>(geode.getDrawable(i));

		if (!geom)
			continue;

		for (int k = 0; k < geom->getNumPrimitiveSets(); k++)
		{
			PrimitiveSet* priSet = geom->getPrimitiveSet(k);
			ref_ptr<DrawElements> drawElementTriangle = priSet->getDrawElements();
			ref_ptr<DrawElementsUInt> newDrawElementsTriangle = new DrawElementsUInt(PrimitiveSet::TRIANGLES);
			int cnt = drawElementTriangle->getNumIndices();
			int triCnt = cnt / 3;
			numTri += triCnt;
		}
	}
}
