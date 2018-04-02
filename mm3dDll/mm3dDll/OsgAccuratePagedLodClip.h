#pragma once
#include "traveller.h"
#include <string>
#include <vector>
/*
** brief ��ȷ������
*/
class CAccuratePagedLodClip : public CTraveller
{
public:
	CAccuratePagedLodClip(std::string inFilePath, std::string outFilePath, std::vector<osg::Vec3d> area, osg::Matrix mat);

protected:
	/*
	** brief applyChild Ӧ������
	** param node �ӽڵ�
	*/
	virtual void applyChild(osg::Node* node);

	/*
	** brief clone ��¡
	** param childPath ��·��
	*/
	virtual CTraveller* clone(std::string childPath);

private:
	/*
	** brief ���·��
	*/
	std::string mOutFilePath;
	
	/*
	** brief ����·��
	*/
	std::string mInFilePath;
	
	/*
	** brief ���з�Χ
	*/
	std::vector<osg::Vec3d> mArea;

	/*
	** brief �ֲ����������
	*/
	osg::Matrix mMat;
};

