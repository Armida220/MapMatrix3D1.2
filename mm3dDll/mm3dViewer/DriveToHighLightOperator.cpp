#include "stdafx.h"
#include "DriveToHighLightOperator.h"
#include <osg/Matrix>
#include <osg/Vec3d>
#include "qcomm.h"

using namespace mm3dView;
using namespace osg;
using namespace osgViewer;

CDriveToHighLightOperator::CDriveToHighLightOperator(ref_ptr<Viewer> viewer,
	ref_ptr<Group> highGroup)
	: mViewer(viewer), mHighGroup(highGroup)
{
}


CDriveToHighLightOperator::~CDriveToHighLightOperator()
{
}

void CDriveToHighLightOperator::operator()()
{
	if (!mViewer)
	{
		return;
	}

	BoundingSphere bs = mHighGroup->getBound();
	Vec3d center = bs.center();

	if (center.x() == 0 && center.y() == 0)
	{
		return;
	}

	//进行垂直碰撞检测
	Matrix mat = mViewer->getCameraManipulator()->getMatrix();
	Vec3d xyz = mat.getTrans();
	// 创建矩阵，指定到视点的距离。
	Matrix trans;
	trans.makeTranslate(center.x(), center.y(), xyz.z());
	// 旋转一定角度（弧度值）。
	double angle(0);
	// 创建旋转矩阵。
	Matrix rot;
	rot.makeRotate(DegreesToRadians(angle), Vec3(1., 0., 0.));

	// 设置视口矩阵（旋转矩阵和平移矩阵连乘）。
	mViewer->getCameraManipulator()->setByMatrix(rot * trans);
}
