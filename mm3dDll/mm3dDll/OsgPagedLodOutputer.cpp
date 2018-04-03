#include "stdafx.h"
#include "OsgPagedLodOutputer.h"
#include <osg/PagedLOD>
#include <osgDB/WriteFile>
#include <osgDB/Options>
#include "OsgPageLodSetter.h"
#include <stdlib.h>  
#include <string.h>  
#include "Executor.h"
#include "ScriptExecutor.h"

using namespace std;
using namespace osg;
using namespace osgDB;

bool COsgPagedLodOutputer::outputPagedLodNode(PagedLOD& pagedLod, ref_ptr<Node> node)
{
	if (!node)
	{
		return false;
	}

	//先删除
	string childFileTitle = pagedLod.getFileName(1);

	//检查clip文件夹
	string outChildFileName;
	CheckClipFolder(outFilePath, childFileTitle, outChildFileName);
	
	CMFCExcute excute;
	if (excute.IsFileExists(outChildFileName))
	{
		excute.RemoveFile(outChildFileName);
	}

	//创建目录
	int pos = outChildFileName.rfind('\\');
	string outChildFilePath = outChildFileName.substr(0, pos + 1);
											
	excute.createFolder(outChildFilePath);
	//输出
	WriteNodeToFile(node, outChildFileName);

	return true;
}

void COsgPagedLodOutputer::setOutFilePath(string OutFilePath)
{
	outFilePath = OutFilePath;
}


void COsgPagedLodOutputer::OutputTotalInScopeOsgb(string inFilePath, vector<string> &vecTotalInPolygonFileName)
{
	for (int i = 0; i < vecTotalInPolygonFileName.size(); i++)
	{
		int pos = vecTotalInPolygonFileName[i].rfind('\\');
		string osgbFilePath = inFilePath + vecTotalInPolygonFileName[i].substr(0, pos);
		int length = vecTotalInPolygonFileName[i].length() - 1;

		string outOsgbFileName;
		CheckClipFolder(outFilePath, vecTotalInPolygonFileName[i], outOsgbFileName);

		pos = outOsgbFileName.rfind('\\');
		string outOsgbFilePath = outOsgbFileName.substr(0, pos);

		CMFCExcute excute;
		excute.createFolder(outOsgbFilePath);
		excute.FileCopyTo(osgbFilePath, outOsgbFilePath, "*.osgb");

		int percent = (int)((double)(i + 1) / (double)vecTotalInPolygonFileName.size() * 60) + 20;

		string script = "setProgressVal(" + to_string(percent) + ")";
		CScriptExecutor::getInstance()->excuteScript(script);
	}
}


void COsgPagedLodOutputer::CheckClipFolder(string filePath, string fileTitle, string &outFileName)
{
	//判断是否有clip
	int pos = filePath.find("Clip");

	if (pos != -1)
	{
		outFileName = filePath + ObtainTileFileName(fileTitle);
	}
	else
	{
		outFileName = filePath + fileTitle;														// 合并
	}
}

string COsgPagedLodOutputer::ObtainTileFileName(string fileName)
{
	int pos = fileName.find("Tile");
	int length = fileName.length();
	string tileFileName = ".\\" + fileName.substr(pos, length - pos);
	return tileFileName;
}

void COsgPagedLodOutputer::WriteNodeToFile(ref_ptr<Node> node, string outFileNameInPiece)
{
	//输出照片
	ref_ptr<Options> spOptions = new Options;
	spOptions->setPluginStringData("WriteImageHint", "IncludeFile");
	//设置相对路径
	COsgPageLodSetter ive("");
	node->accept(ive);
	writeNodeFile(*node, outFileNameInPiece, spOptions.get());
}
