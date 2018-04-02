#pragma once

#include <vector>
#include <osg/Vec3d>
#include <osg/Geode>
#include <osg/Matrix>


typedef struct tagGridDem
{
	double x0, y0;    // 方格网DEM左下角坐标
	double x1, y1;    // 方格网DEM右上角坐标
	double dx, dy;    // 方格网DEM格网间距
	double kappa;     // 方格网DEM的旋转角
	int cols, rows;   // 方格网DEM格网的列数和行数
	float* z;         // 方格网DEM格网点的高程坐标
	float avgz;       // 方格网DEM的平均高程
	float minz, maxz; // 方格网DEM的最小和最大高程
	float denum;
	float* nx;        // 方格网DEM的法向量X分量
	float* ny;        // 方格网DEM的法向量Y分量
	float* nz;        // 方格网DEM的法向量Z分量
	float valid;
	// 初始化方格网DEM数据结构
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

	//接口
	osg::ref_ptr<osg::Node> createInterpolateRepairPatch(std::vector<osg::Vec3d> vecCoord, osg::Matrix worldMatrix, std::string imgFileName);	//创建插值补块

private:
	void InterpolatePt(osg::Geode& geode, osg::Matrix matrix, std::vector<osg::Vec3d> &vecPt, GridDem gDem, bool isRepair);						//插值点

	osg::ref_ptr<osg::Node> createRepairPatch(std::vector<osg::Vec3d> vecCoord, std::string imageFileName);										//创建修补节点

	bool ObtainGeoRect(osg::ref_ptr<osg::Node> node, osg::Matrix matrix, std::vector<osg::Vec3d> &minMaxGeode);									//获取最大范围
};





