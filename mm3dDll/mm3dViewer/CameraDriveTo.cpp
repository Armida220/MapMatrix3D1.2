#include "stdafx.h"
#include "CameraDriveTo.h"
#include <osg/Vec3d>
#include <osg/Matrix>
#include <osgViewer/Viewer>

using namespace mm3dView;
using namespace osg;
using namespace osgViewer;


CCameraDriveTo::CCameraDriveTo(ref_ptr<Viewer> viewer,
	osg::Vec3d center)
	: mViewer(viewer),
	mCenter(center)
{
}


CCameraDriveTo::~CCameraDriveTo()
{
}


void CCameraDriveTo::operator()()
{
	if (!mViewer)
	{
		return;
	}

	Matrix mat = mViewer->getCameraManipulator()->getMatrix();
	Vec3d xyz = mat.getTrans();
	// 创建矩阵，指定到视点的距离。
	Matrix trans;
	trans.makeTranslate(mCenter.x(), mCenter.y(), xyz.z());
	// 旋转一定角度（弧度值）。
	double angle(0);
	// 创建旋转矩阵。
	Matrix rot;
	rot.makeRotate(DegreesToRadians(angle), Vec3(1., 0., 0.));

	// 设置视口矩阵（旋转矩阵和平移矩阵连乘）。
	mViewer->getCameraManipulator()->setByMatrix(rot * trans);
}