#include "stdafx.h"
#include "OsgPicker.h"
#include <osgUtil/LineSegmentIntersector>

using namespace osgEvent;
using namespace osgViewer;
using namespace osgUtil;
using namespace osg;
using namespace std;

bool isInvalidLS(const LineSegmentIntersector::Intersections::iterator & hitr)
{
	return (strcmp(hitr->drawable->className(), "Widget") == 0 ||
		strcmp(hitr->drawable->className(), "EmbeddedWindow") == 0 ||
		strcmp(hitr->drawable->className(), "Corner") == 0 ||
		strcmp(hitr->drawable->className(), "Border") == 0 ||
		strcmp(hitr->drawable->className(), "Text") == 0
		);
}

COsgPicker::COsgPicker(Viewer *view)
	: mView(view)
{
}


COsgPicker::~COsgPicker()
{
}


const CPickResult COsgPicker::pickResult(const float x, const float y) const
{
	LineSegmentIntersector::Intersections intersections;
	mView->computeIntersections(x, y, intersections);
	int totalNum = intersections.size();
	LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();

	while (hitr != intersections.end() && isInvalidLS(hitr))
	{
		totalNum -= 1;
		++hitr;
	}

	CPickResult pickResult;

	if (totalNum > 0)
	{
		ref_ptr<Node> parent = ((Node*)(hitr->nodePath.back()))->getParent(0);

		pickResult.localPt = hitr->localIntersectionPoint;
		pickResult.nodePath = hitr->nodePath;
		pickResult.worldPt = hitr->getWorldIntersectPoint();
		pickResult.worldNm = hitr->getWorldIntersectNormal();
		pickResult.modelIndex = -1;
		pickResult.parent = parent;
		pickResult.matrix = computeLocalToWorld(hitr->nodePath);

		for (int i = 0; i < pickResult.nodePath.size(); i++)
		{
			ref_ptr<Node> node = dynamic_cast<Node*> (pickResult.nodePath[i]);
			string name = node->getName();
			int pos = name.find("model");

			if (pos != -1)
			{
				int posL = name.find("l");
				string subStr = name.substr(posL + 1, name.length());
				int count = atoi(subStr.c_str());
				pickResult.modelIndex = count - 1;
				break;
			}
		}
	}
	else
	{
		Matrix matrix;
		NodePath np;
		pickResult.localPt = Vec3d(0, 0, 0);
		pickResult.nodePath = np;
		pickResult.worldPt = Vec3d(0, 0, 0);
		pickResult.modelIndex = -1;
		pickResult.matrix = matrix;
	}

	return pickResult;
}


const CPickResult COsgPicker::linePick(const Vec3d &pUp, const Vec3d &pDown, Group *scene) const
{
	if (!scene)
	{
		CPickResult pickResult;
		pickResult.worldPt = Vec3d(0, 0, 0);
		return pickResult;
	}

	//进行垂直碰撞检测获得三维点
	//碰撞检测
	ref_ptr<LineSegmentIntersector> ls = new LineSegmentIntersector(pDown, pUp);
	// 创建一个IV
	IntersectionVisitor iv(ls);

	scene->accept(iv);

	CPickResult pickResult;

	//碰撞有效
	if (!ls.valid() || !(ls->containsIntersections()))
	{
		return pickResult;
	}

	LineSegmentIntersector::Intersections intersections = ls->getIntersections();
	int totalNum = intersections.size();
	LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
	while (hitr != intersections.end() && isInvalidLS(hitr))
	{
		totalNum -= 1;
		++hitr;
	}

	if (totalNum > 0)
	{
		pickResult.nodePath = hitr->nodePath;
		pickResult.worldPt = hitr->getWorldIntersectPoint();
		pickResult.worldNm = hitr->getWorldIntersectNormal();
		pickResult.matrix = computeLocalToWorld(hitr->nodePath);
	}

	return pickResult;
}

vector<Vec3d> osgEvent::COsgPicker::DefaultlinePickAll(double x, double y, osg::Group *scene) const
{
	Vec3d pUp(x, y, 5000);
	Vec3d pDown(x, y, -1000);
	vector<Vec3d> res;
	if (!scene)
	{
		return res;
	}

	//进行垂直碰撞检测获得三维点
	//碰撞检测
	ref_ptr<LineSegmentIntersector> ls = new LineSegmentIntersector(pDown, pUp);
	// 创建一个IV
	IntersectionVisitor iv(ls);

	scene->accept(iv);

	//碰撞有效
	if (!ls.valid() || !(ls->containsIntersections()))
	{
		return res;
	}

	LineSegmentIntersector::Intersections intersections = ls->getIntersections();
	int totalNum = intersections.size();
// 	LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
// 	while (hitr != intersections.end() && isInvalidLS(hitr))
// 	{
// 		totalNum -= 1;
// 		++hitr;
// 	}
	for (LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
		hitr != intersections.end(); ++hitr)
	{
		if (isInvalidLS(hitr))
			continue;
		else
		{
			res.push_back(hitr->getWorldIntersectPoint());
		}
	}
	return res;
}

const CPickResult COsgPicker::DefaultlinePick(double x, double y, osg::Group *scene) const
{
	//进行垂直碰撞检测
	Vec3d pUp(x, y, 5000);
	Vec3d pDown(x, y, -1000);

	const CPickResult pResult = linePick(pUp, pDown, scene);
	return pResult;
}
