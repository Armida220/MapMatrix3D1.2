#pragma once
#include <osg/Matrix>
#include <osg/NodeVisitor>
#include <string>
#include "cv.h"



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

class CNodeDemCreator :  public osg::NodeVisitor 
{
public:
	CNodeDemCreator::CNodeDemCreator(osg::Matrix MATRIX, CvMat* DEMMAT, GridDem GDem, bool IsRepair) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{
		matrix = MATRIX;
		demMat = DEMMAT;
		gDem = GDem;
		isRepair = IsRepair;
	}
	void apply(osg::Geode& geode)
	{
		InterpolateDem(geode, matrix, demMat, gDem, isRepair);
	}

private:
	void InterpolateDem(osg::Geode& geode, osg::Matrix matrix, CvMat* demMat, GridDem gDem, bool isRepair);
	
private:
	osg::Matrix matrix;
	CvMat* demMat;
	GridDem gDem;
	bool isRepair;
};

class CSceneDemGenerator
{
public:
	std::vector<std::string> vecFileName;
	GridDem gDem;

public:
	bool StartGeneratingDem(std::vector<osg::MatrixTransform*> vecTransNode, std::string rootModelPath, std::string outfileName, double xRes, double yRes);

private:
	CvMat* GenerateDem(std::string rootModelPath, osg::ref_ptr<osg::MatrixTransform> transNode, double Xinterval, double Yinterval);

	void ObtainImageGeoRect(std::string twfFileName, osg::Vec4d &rect, std::string imageFileName);

	bool ObtainGeoRect(osg::ref_ptr<osg::Node> node, osg::Matrix matrix, std::vector<osg::Vec3d> &minMaxGeode);

	std::vector<osg::Vec3d> findMinMaxNode(std::vector<osg::Vec3d> minMaxNode, std::vector<osg::Vec3d> minMaxNode1);
	

	std::vector<std::vector<std::string>> SeparateScenenTile(std::vector<std::string> vecChildFilePath, double Xinterval, double Yinterval);

	void SelectHighLevelLodFileName(std::vector<std::vector<std::string>> vecGroupChildName, std::vector<std::vector<std::string>> &vecGroupHighLevelChildName);

	bool GenerateDemForSingleModel(std::string rootModelPath, osg::ref_ptr<osg::MatrixTransform> transNode, std::string outDemFileName, double xRes, double yRes);

	void CombineImage(osg::Vec4d maxRect, std::string outfileName, std::vector<osg::Vec4d> vecRect, std::vector<std::string> vecDemTiffFileName, 
		std::vector<std::string> vecDemTwfFileName, double xRes, double yRes);

	osg::Vec4d findMaxRect(std::vector<osg::Vec4d> vecRect);

};

