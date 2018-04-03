#include "stdafx.h"
#include "OsgAccuratePagedLodClip.h"
#include "OsgTriangleNumFinder.h"
#include "OsgAccurateHeadGeodeClip.h"
#include <osgDB/WriteFile>
#include "OsgPageLodSetter.h"
#include "Executor.h"

using namespace std;
using namespace osg;
using namespace osgDB;

CAccuratePagedLodClip::CAccuratePagedLodClip(string inFilePath, string outFilePath, vector<Vec3d> area, Matrix mat)
	: CTraveller(inFilePath),
	mInFilePath(inFilePath),
	mOutFilePath(outFilePath),
	mArea(area),
	mMat(mat)
{
}


void CAccuratePagedLodClip::applyChild(Node* node)
{
	//查找三角形个数
	int numTri = 0;
	COsgTriangleNumFinder ive;
	node->accept(ive);

	if (ive.numTri == 0)
	{
		return;
	}

	COsgAccurateHeadGeodeClip iveClipper(mArea, mMat);
	node->accept(iveClipper);

	string curTitle = mCurTitle;
	string tileFile = obtainTileFileName(mCurFullFileName);
	string outFileName = mOutFilePath + tileFile;
	int pos = outFileName.rfind("\\");
	string outFilePath = outFileName.substr(0, pos + 1);

	//输出照片
	ref_ptr<Options> spOptions = new Options;
	spOptions->setPluginStringData("WriteImageHint", "IncludeFile");
	//设置相对路径
	COsgPageLodSetter iveSetter("");
	node->accept(iveSetter);

	CMFCExcute ext;
	ext.createFolder(outFilePath);

	writeNodeFile(*node, outFileName, spOptions.get());

	return;
}



CTraveller* CAccuratePagedLodClip::clone(string childPath)
{
	return new CAccuratePagedLodClip(childPath, mOutFilePath, mArea, mMat);
}