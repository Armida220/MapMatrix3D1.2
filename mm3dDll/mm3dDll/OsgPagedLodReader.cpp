#include "stdafx.h"
#include "OsgPagedLodReader.h"
#include <osgDB/ReadFile>

using namespace osg;
using namespace std;
using namespace osgDB;

ref_ptr<Node> COsgPagedLodReader::readPagedLodNode(PagedLOD& pagedLod)
{
	//��ȡ��Ƭ
	string childFileTitle = pagedLod.getFileName(1);
	string childFileName = inFilePath + childFileTitle;
	ref_ptr<Node> node = readNodeFile(childFileName);
	return node;
}

ref_ptr<Group> COsgPagedLodReader::combinePagedLod(vector<string> vecPagedLodFileName)
{
	//�����е�pagedlod����group
	ref_ptr<Group> group = new Group;

	for (int i = 0; i < vecPagedLodFileName.size(); i++)
	{
		string fileFolder = vecPagedLodFileName[i];
		string fileName = inFilePath + fileFolder;
		ref_ptr<PagedLOD> lod = dynamic_cast<PagedLOD*>(readNodeFile((fileName)));

		if (!lod)
		{
			continue;
		}

		char strMsg[99];
		sprintf_s(strMsg, "%d\n", i);
		lod->setName(strMsg);
		lod->setFileName(1, fileFolder);
		lod->setDatabasePath("");
		group->addChild(lod);
	}

	return group;
}
