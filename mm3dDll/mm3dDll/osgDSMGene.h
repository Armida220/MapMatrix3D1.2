#pragma once
#include "IOsgDsmGene.h"
#include <osg/Node>
/*
** brief dsm生成器
*/
class osgDSMGene : public IOsgDsmGene
{
public:
	/*
	** brief dsm生成器接口
	** param sceneFileName 场景文件
	** param outDemFileName 输出dsm场景文件
	** param res x分辨率
	*/
	virtual void generateDSM(std::string sceneFileName, std::string outDemFileName, double res);

protected:
	void Produce(std::string sceneFileName, std::string outputFileName, double res);

	void StartProducingDSM();

protected:

	/*
	** brief mSceneNode 场景节点
	*/
	osg::ref_ptr<osg::Node> mSceneNode;

	/*
	** brief mSceneFileName 场景节点
	*/
	std::string mSceneFileName;

	/*
	** brief mRes 真正射分辨率大小
	*/
	double mRes;

	/*
	** brief mXTileNum x方向tile大小
	*/
	int mXTileNum = 1;

	/*
	** brief mYTileNum y方向tile大小
	*/
	int mYTileNum = 1;

	/*
	** brief mOutputFileName 输出dom文件
	*/
	std::string mOutputFileName;
};

