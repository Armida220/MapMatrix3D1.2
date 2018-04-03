#include "stdafx.h"
#include "CornerPtCatcher.h"
#include "qcomm.h"

using namespace osgEvent;
using namespace osg;
using namespace osgGA;

CCornerPtCatcher::CCornerPtCatcher()
	: bIsCatch(false)
{
	vecClkPt.reserve(10);
}


CCornerPtCatcher::~CCornerPtCatcher()
{
}

void CCornerPtCatcher::lfClk(Vec3d &clkPt)
{
	//如果内角点捕捉开启， 进行碰撞点替换
	catchPt(clkPt);

	//push(clkPt);
}

void CCornerPtCatcher::mouseMv(Vec3d &clkPt)
{

}

void CCornerPtCatcher::dbClk(const Vec3d &clkPt)
{

}

bool CCornerPtCatcher::rightClk(const Vec3d &clkPt)
{
	//clear();

	return true;
}

bool CCornerPtCatcher::keyDown(const GUIEventAdapter &ea)
{
	return true;
}

void CCornerPtCatcher::push(Vec3d clkPt)
{
	vecClkPt.push_back(clkPt);
}

void CCornerPtCatcher::pop()
{
	if (!vecClkPt.empty())
		vecClkPt.pop_back();
}

void CCornerPtCatcher::clear()
{
	vecClkPt.clear();
}

bool CCornerPtCatcher::shift()
{
	bIsCatch = !bIsCatch;
	//clear();
	return bIsCatch;
}

void CCornerPtCatcher::catchPt(Vec3d &clkPt)
{
	//如果捕捉设置
	if (bIsCatch)
	{
		int size = vecClkPt.size();

		if (size >= 2)
		{
			Vec3d prePt1 = vecClkPt[size - 1];
			Vec3d prePt2 = vecClkPt[size - 2];

			//重新计算clkPt
			Vec3d cornerPt = calCornerPt(prePt1, prePt2, clkPt);
			clkPt.x() = cornerPt.x();
			clkPt.y() = cornerPt.y();
		}
	}
}


Vec3d CCornerPtCatcher::calCornerPt(const Vec3d pt1, const Vec3d pt2, const Vec3d pt3)
{
	double k1 = (pt2.y() - pt1.y()) / (pt2.x() - pt1.x());
	double b1 = pt3.y() - k1 * pt3.x();
	double k2 = (-1) / k1;
	double b2 = pt1.y() - k2 * pt1.x();

	double x = (b1 - b2) / (k2 - k1);
	double y = k1 * x + b1;
	double z = pt1.z();

	return Vec3d(x, y, z);
}

void CCornerPtCatcher::setLastConstDragLine(osg::Vec3d pt1, osg::Vec3d pt2)
{
	clear();
	vecClkPt.push_back(pt1);
	vecClkPt.push_back(pt2);
}

void CCornerPtCatcher::clearConstDragLine()
{
	clear();
}
