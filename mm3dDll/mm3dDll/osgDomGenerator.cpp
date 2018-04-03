// osgDomGenerator.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgDomGenerator.h"
#include "SceneDomGenerator.h"
#include <process.h>
#include <osgDB/ReadFile>
#include <thread>
#include <functional>
#include  <direct.h>  
#include <mutex>
#include <algorithm>
using namespace std;
using namespace osg;

osgDomGenerator::osgDomGenerator()
	: mXTileNum(1),
	mYTileNum(1)
{

}

bool osgDomGenerator::generateDOM(string sceneFileName, string outFileName, double res)
{
	//初始化
	mSceneFileName = sceneFileName;
	mRes = res;
	mOutputFileName = outFileName;

	int pos = mOutputFileName.rfind('.');
	int length = mOutputFileName.length() - 1;
	string extFile = mOutputFileName.substr(pos, length);

	if (extFile != ".tif")
	{
		mOutputFileName += ".tif";
	}

	startProducingDom();

	return TRUE;
}



void osgDomGenerator::produce(string sceneFileName, string outputFileName, double res, int xTileNum, int yTileNum)
{
	CSceneDomGenerator geneDom(sceneFileName, outputFileName, res, xTileNum, yTileNum);
	bool isSuccess = geneDom.startProducingDOM();
}

void osgDomGenerator::startProducingDom()
{
	/*_beginthread(&produce, 0, this);*/
	std::vector<std::thread> threads;
	threads.push_back(std::thread(std::mem_fn(&osgDomGenerator::produce), this,
		mSceneFileName, mOutputFileName, mRes, mXTileNum, mYTileNum));

	std::for_each(threads.begin(), threads.end(),
		std::mem_fn(&std::thread::detach));

	/*produce(mSceneFileName, mOutputFileName, mRes, mXTileNum, mYTileNum);*/

}