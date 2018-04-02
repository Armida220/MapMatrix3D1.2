#pragma once
#include "IOsgDomGenerator.h"
#include "osg/Node"
/*
** brief ����dom��
*/
class osgDomGenerator : public IOsgDomGenerator
{
public:
	osgDomGenerator();

	/*
	** brief generateDOM ����������Ӱ��
	** param sceneRootFile ��������ڵ��ļ�·��
	** param outFileName ��������ļ�
	** param res ������Ӱ��ֱ��ʴ�С
	*/
	virtual bool generateDOM(std::string sceneFileName, std::string outFileName, double res);


protected:
	void produce(std::string sceneFileName, std::string outputFileName, double res, int xTileNum, int yTileNum);

	/*
	** brief startProducingDom ��ʼ����dom
	*/
	void startProducingDom();

protected:
	
	/*
	** brief sceneFileName �����ڵ��ļ�·��
	*/
	std::string mSceneFileName;

	/*
	** brief mRes ������ֱ��ʴ�С
	*/
	double mRes;

	/*
	** brief mXTileNum x����tile��С 
	*/
	int mXTileNum;

	/*
	** brief mYTileNum y����tile��С
	*/
	int mYTileNum;

	/*
	** brief mOutputFileName ���dom�ļ�
	*/
	std::string mOutputFileName;
};