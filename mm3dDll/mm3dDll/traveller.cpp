#include "stdafx.h"
#include "traveller.h"
#include <osgDB/ReadFile>
#include <iostream>


using namespace osgDB;
using namespace std;
using namespace osg;

std::stack<std::string> CTraveller::mTitlePath;

void CTraveller::apply(PagedLOD& pagedLod) 
{
	//读取节点
	string childTitle = pagedLod.getFileName(1);
	string childName = mInPath + childTitle;
	ref_ptr<Node> node = readNodeFile(childName);

	if (!node)
	{
		return;
	}

	//路径全名
	mCurFullFileName = childName;

	//保存当前表名称	
	mCurTitle = getTitle(childTitle);
	
	applyChild(node);

	//遍历下一级时候，当前的文件名作为子的父表名称，进栈
	mTitlePath.push(mCurTitle);

	//遍历下一级
	string childPath = getPath(childName);
	ref_ptr<CTraveller> childTravel = clone(childPath);
	node->accept(*childTravel);

	//遍历完成子级后，出栈
	mTitlePath.pop();

	traverse(pagedLod);
}

void CTraveller::applyChild(Node* node)
{
	
}


CTraveller* CTraveller::clone(string childPath)
{
	return new CTraveller(childPath);
}

string CTraveller::getPath(string fileName)
{
	int pos = fileName.rfind("\\");
	return fileName.substr(0, pos + 1) + ".\\";
}

string CTraveller::getTitle(string fileName)
{
	int pos = fileName.rfind("\\");
	int length = fileName.length() - 1;
	return fileName.substr(pos + 1, length - pos);
}


string CTraveller::obtainTileFileName(string fileName)
{
	int pos = fileName.find("Tile");
	int length = fileName.length();
	string tileFileName = ".\\" + fileName.substr(pos, length - pos);
	return tileFileName;
}