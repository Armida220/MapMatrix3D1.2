#pragma once

#include <vector>
#include <osg/Vec3d>
#include <osg/Geode>
#include <osg/Matrix>


typedef struct tagGridDem
{
	double x0, y0;    // ������DEM���½�����
	double x1, y1;    // ������DEM���Ͻ�����
	double dx, dy;    // ������DEM�������
	double kappa;     // ������DEM����ת��
	int cols, rows;   // ������DEM����������������
	float* z;         // ������DEM������ĸ߳�����
	float avgz;       // ������DEM��ƽ���߳�
	float minz, maxz; // ������DEM����С�����߳�
	float denum;
	float* nx;        // ������DEM�ķ�����X����
	float* ny;        // ������DEM�ķ�����Y����
	float* nz;        // ������DEM�ķ�����Z����
	float valid;
	// ��ʼ��������DEM���ݽṹ
	tagGridDem(){
		x0 = y0 = 0;
		x1 = y1 = 0;
		dx = dy = 0;
		kappa = 0;
		cols = rows = 0;
		z = NULL;
		avgz = minz = maxz = 0.0;
		denum = 0.0f;
		nx = NULL;
		ny = NULL;
		nz = NULL;
		valid = 0;
	};
}GridDem;

class CSceneRepairor
{
public:
	CSceneRepairor(void);
	~CSceneRepairor(void);

	//�ӿ�
	osg::ref_ptr<osg::Node> createInterpolateRepairPatch(std::vector<osg::Vec3d> vecCoord, osg::Matrix worldMatrix, std::string imgFileName);	//������ֵ����

private:
	void InterpolatePt(osg::Geode& geode, osg::Matrix matrix, std::vector<osg::Vec3d> &vecPt, GridDem gDem, bool isRepair);						//��ֵ��

	osg::ref_ptr<osg::Node> createRepairPatch(std::vector<osg::Vec3d> vecCoord, std::string imageFileName);										//�����޲��ڵ�

	bool ObtainGeoRect(osg::ref_ptr<osg::Node> node, osg::Matrix matrix, std::vector<osg::Vec3d> &minMaxGeode);									//��ȡ���Χ
};





