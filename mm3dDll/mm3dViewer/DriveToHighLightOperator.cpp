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

	//���д�ֱ��ײ���
	Matrix mat = mViewer->getCameraManipulator()->getMatrix();
	Vec3d xyz = mat.getTrans();
	// ��������ָ�����ӵ�ľ��롣
	Matrix trans;
	trans.makeTranslate(center.x(), center.y(), xyz.z());
	// ��תһ���Ƕȣ�����ֵ����
	double angle(0);
	// ������ת����
	Matrix rot;
	rot.makeRotate(DegreesToRadians(angle), Vec3(1., 0., 0.));

	// �����ӿھ�����ת�����ƽ�ƾ������ˣ���
	mViewer->getCameraManipulator()->setByMatrix(rot * trans);
}
