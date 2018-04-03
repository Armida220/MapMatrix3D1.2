#include "stdafx.h"
#include "IsInAreaJudger.h"
#include <fstream>

using namespace std;
using namespace osg;

CIsInAreaJudger::CIsInAreaJudger(vector<CL::Vec2d> area)
	:mArea(area)
{
}


bool CIsInAreaJudger::isPtInArea(Vec3d pt)
{
	bool bIsInPolygon = false;

	/*ofstream s("e:\\1.txt", ios::app);
	s << "pt: " << pt.x() << " " << pt.y() << endl;

	for (int i = 0; i < mArea.size(); i++)
	{
		s << "Matrix: " << mArea[i].x << " " << mArea[i].y << endl;
	}
*/
	if (Point_In_Polygon_2D(pt.x(), pt.y(), mArea))
	{
		bIsInPolygon = true;
	}

	return bIsInPolygon;
}


bool CIsInAreaJudger::isTriangleInPolygon(Vec3d pt1, Vec3d pt2, Vec3d pt3)
{
	bool isInPolygon = false;

	if (Point_In_Polygon_2D(pt1.x(), pt1.y(), mArea) ||
		Point_In_Polygon_2D(pt2.x(), pt2.y(), mArea) ||
		Point_In_Polygon_2D(pt3.x(), pt3.y(), mArea))
	{
		isInPolygon = true;
	}

	return isInPolygon;
}
