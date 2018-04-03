#include "stdafx.h"
#include "OsgAccurateClip.h"
#include "OsgAccuratePagedLodClip.h"
#include <osgDB/ReadFile>
#include "osgTransNodeFinder.h"
#include "ObtainSelTileFileName.h"
#include "OsgPagedLodReader.h"
#include "OsgPagedLodOutputer.h"
#include "OsgPageLodSetter.h"
#include "OsgAccurateHeadGeodeClip.h"
#include <osgDB/WriteFile>
#include "ScriptExecutor.h"
#include <thread>

using namespace std;
using namespace osg;
using namespace osgDB;

void COsgAccurateClip::accurateClip(string inFileName, string outputFileName, vector<Vec3d> area)
{
	//��ʼ��
	mInFileName = inFileName;
	mOutputFileName = outputFileName;
	mArea = area;

	startClipScene();
}

void COsgAccurateClip::clipHalfInPolygonTile(ref_ptr<osg::Node> node, string inFilePath, string outFilePath, vector<Vec3d> area, Matrix matrix)
{
	int a = 0;

	CAccuratePagedLodClip ive(inFilePath, outFilePath, area, matrix);
	node->accept(ive);
}

void COsgAccurateClip::clip(string inFileName, string outputFileName, vector<Vec3d> area, int xTileNum, int yTileNum)
{
	int pos = inFileName.rfind("\\");
	string inFilePath = inFileName.substr(0, pos);

	int pos1 = outputFileName.rfind("\\");
	string outFilePath = outputFileName.substr(0, pos1);

	ref_ptr<Node> sceneNode = readNodeFile(inFileName);

	if (!sceneNode)
	{
		return;
	}

	COsgTransNodeFinder ive;
	vector<MatrixTransform*> vecTrans;
	ive.findTransNode(sceneNode, vecTrans);

	ref_ptr<MatrixTransform> trans = vecTrans[0];
	Matrix worldMat = vecTrans[0]->getMatrix();

	ref_ptr<Group> mGroup = trans->getChild(0)->asGroup();

	if (!mGroup)
	{
		return;
	}

	Matrix worldMatrix = trans->getMatrix();

	CObtainSelTileFileName tileFileNameGetter(area);
	tileFileNameGetter.obtainSelTileName(mGroup, worldMatrix);							//���tile��
	vector<string> vecHalfInPolygonFileName = tileFileNameGetter.getHalfInFileName();
	vector<string> vecTotalInPolygonFileName = tileFileNameGetter.getTotalInFileName();

	//�ϲ���
	COsgPagedLodReader reader(inFilePath);
	osg::ref_ptr<osg::Group> combineHalfInPolygonGroup = reader.combinePagedLod(vecHalfInPolygonFileName);						//�����е�tile�ϲ�

	string script = "setProgressVal(20)";
	CScriptExecutor::getInstance()->excuteScript(script);


	//���ڲ���ȫ�ڷ�Χ�ڽ��в���
	clipHalfInPolygonTile(combineHalfInPolygonGroup, inFilePath, outFilePath, area, worldMat);

	

	//������ȫ�ڷ�Χ�ڵ�ֱ�����
	COsgPagedLodOutputer pagedLodOutput(outFilePath);
	pagedLodOutput.OutputTotalInScopeOsgb(inFilePath, vecTotalInPolygonFileName);

	COsgAccurateHeadGeodeClip iveClipper(area, worldMat);
	sceneNode->accept(iveClipper);

	//�����Ƭ
	ref_ptr<Options> spOptions = new Options;
	spOptions->setPluginStringData("WriteImageHint", "IncludeFile");

	//�������·��
	COsgPageLodSetter iveSetter("");
	sceneNode->accept(iveSetter);
	writeNodeFile(*sceneNode, outputFileName, spOptions.get());

	script = "setProgressVal(100)";
	CScriptExecutor::getInstance()->excuteScript(script);
}

void COsgAccurateClip::startClipScene()
{
	int xTileNum = 1;
	int yTileNum = 1;

	std::vector<std::thread> threads;
	threads.push_back(std::thread(std::mem_fn(&COsgAccurateClip::clip), this,
		mInFileName, mOutputFileName, mArea, xTileNum, yTileNum));

	std::for_each(threads.begin(), threads.end(),
		std::mem_fn(&std::thread::detach));
}


