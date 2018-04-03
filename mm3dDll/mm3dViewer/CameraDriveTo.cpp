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
	// ��������ָ�����ӵ�ľ��롣
	Matrix trans;
	trans.makeTranslate(mCenter.x(), mCenter.y(), xyz.z());
	// ��תһ���Ƕȣ�����ֵ����
	double angle(0);
	// ������ת����
	Matrix rot;
	rot.makeRotate(DegreesToRadians(angle), Vec3(1., 0., 0.));

	// �����ӿھ�����ת�����ƽ�ƾ������ˣ���
	mViewer->getCameraManipulator()->setByMatrix(rot * trans);
}