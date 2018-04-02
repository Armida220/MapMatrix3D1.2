#pragma once
#include "IOsgDomGenerator.h"
#include "osg/Node"
/*
** brief 生成dom类
*/
class osgDomGenerator : public IOsgDomGenerator
{
public:
	osgDomGenerator();

	/*
	** brief generateDOM 生成真正射影像
	** param sceneRootFile 场景顶层节点文件路径
	** param outFileName 输出场景文件
	** param res 真正射影像分辨率大小
	*/
	virtual bool generateDOM(std::string sceneFileName, std::string outFileName, double res);


protected:
	void produce(std::string sceneFileName, std::string outputFileName, double res, int xTileNum, int yTileNum);

	/*
	** brief startProducingDom 开始生产dom
	*/
	void startProducingDom();

protected:
	
	/*
	** brief sceneFileName 场景节点文件路径
	*/
	std::string mSceneFileName;

	/*
	** brief mRes 真正射分辨率大小
	*/
	double mRes;

	/*
	** brief mXTileNum x方向tile大小 
	*/
	int mXTileNum;

	/*
	** brief mYTileNum y方向tile大小
	*/
	int mYTileNum;

	/*
	** brief mOutputFileName 输出dom文件
	*/
	std::string mOutputFileName;
};