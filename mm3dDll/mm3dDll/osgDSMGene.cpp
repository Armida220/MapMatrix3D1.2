#include "StdAfx.h"
#include "osgDSMGene.h"
#include "SceneDemGenerator.h"
#include <process.h>
#include <osg/MatrixTransform>
#include "osgTransNodeFinder.h"
#include "osgDB/ReadFile"
#include <thread>
#include <functional>

using namespace std;
using namespace osg;
using namespace osgDB;

void osgDSMGene::generateDSM(string sceneFileName, string outDemFileName, double res)
{
	string extSelect = "TIF文件 (*.tif)|*.tif||";
	string outputFileName;
	vector<MatrixTransform*> vecTransNode;
	
	//初始化
	mSceneFileName = sceneFileName;
	mOutputFileName = outDemFileName;
	mRes = res;

	StartProducingDSM();
}

void osgDSMGene::Produce(string sceneFileName, string outputFileName, double res)
{
	vector<MatrixTransform*> vecTransNode;

	ref_ptr<Node> sceneNode = readNodeFile(sceneFileName);

	int pos = sceneFileName.rfind('\\') + 1;
	string sceneFilePath = sceneFileName.substr(0, pos);

	COsgTransNodeFinder ive;
	ive.findTransNode(sceneNode, vecTransNode);

	CSceneDemGenerator DemGene;
	DemGene.StartGeneratingDem(vecTransNode, sceneFilePath, outputFileName, res, res);

}

void osgDSMGene::StartProducingDSM()
{
	std::vector<std::thread> threads;
	threads.push_back(std::thread(std::mem_fn(&osgDSMGene::Produce), this,
		mSceneFileName, mOutputFileName, mRes));

	std::for_each(threads.begin(), threads.end(),
		std::mem_fn(&std::thread::detach));

}