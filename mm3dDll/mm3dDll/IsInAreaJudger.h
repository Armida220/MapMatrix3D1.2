#pragma once
#include <vector>
#include "Geometry.h"
#include "VertsTriangle.h"

class CIsInAreaJudger
{
public:
	CIsInAreaJudger(std::vector<CL::Vec2d> area);
	
	bool isPtInArea(osg::Vec3d pt);

	bool isTriangleInPolygon(osg::Vec3d pt1, osg::Vec3d pt2, osg::Vec3d pt3);

private:
	std::vector<CL::Vec2d> mArea;

};

