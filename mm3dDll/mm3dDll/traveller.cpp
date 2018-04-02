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
	//��ȡ�ڵ�
	string childTitle = pagedLod.getFileName(1);
	string childName = mInPath + childTitle;
	ref_ptr<Node> node = readNodeFile(childName);

	if (!node)
	{
		return;
	}

	//·��ȫ��
	mCurFullFileName = childName;

	//���浱ǰ������	
	mCurTitle = getTitle(childTitle);
	
	applyChild(node);

	//������һ��ʱ�򣬵�ǰ���ļ�����Ϊ�ӵĸ������ƣ���ջ
	mTitlePath.push(mCurTitle);

	//������һ��
	string childPath = getPath(childName);
	ref_ptr<CTraveller> childTravel = clone(childPath);
	node->accept(*childTravel);

	//��������Ӽ��󣬳�ջ
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