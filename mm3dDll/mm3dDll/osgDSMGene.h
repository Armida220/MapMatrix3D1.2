#pragma once
#include "IOsgDsmGene.h"
#include <osg/Node>
/*
** brief dsm������
*/
class osgDSMGene : public IOsgDsmGene
{
public:
	/*
	** brief dsm�������ӿ�
	** param sceneFileName �����ļ�
	** param outDemFileName ���dsm�����ļ�
	** param res x�ֱ���
	*/
	virtual void generateDSM(std::string sceneFileName, std::string outDemFileName, double res);

protected:
	void Produce(std::string sceneFileName, std::string outputFileName, double res);

	void StartProducingDSM();

protected:

	/*
	** brief mSceneNode �����ڵ�
	*/
	osg::ref_ptr<osg::Node> mSceneNode;

	/*
	** brief mSceneFileName �����ڵ�
	*/
	std::string mSceneFileName;

	/*
	** brief mRes ������ֱ��ʴ�С
	*/
	double mRes;

	/*
	** brief mXTileNum x����tile��С
	*/
	int mXTileNum = 1;

	/*
	** brief mYTileNum y����tile��С
	*/
	int mYTileNum = 1;

	/*
	** brief mOutputFileName ���dom�ļ�
	*/
	std::string mOutputFileName;
};

