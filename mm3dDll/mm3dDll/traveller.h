#pragma once
#include <string>
#include <osg/NodeVisitor>
#include <osg/PagedLOD>
#include <stack>

/*
* brief CTraveller osgb���ݲ㼶�ı����ߣ����Ա������е�����
*/

class CTraveller : public osg::NodeVisitor
{
public:
	CTraveller(std::string inPath)
		: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
		mInPath(inPath)
	{
	}

protected:
	/*
	* brief apply ������pagedlod�ڵ�ʱ��Ļص�
	* param pagedLod �ص���pagedlod�ڵ�
	*/
	virtual void apply(osg::PagedLOD& pagedLod);

	/*
	* brief apply �������ӽڵ�Ļص�
	* param node �ص����ӽڵ�
	*/
	virtual void applyChild(osg::Node* node);

	/*
	* brief clone ���
	*/
	virtual CTraveller* clone(std::string childPath);

	/*
	* brief getPath ����ȫ·����ȡ·��
	* param fileName ȫ·��
	*/
	std::string getPath(std::string fileName);

	/*
	* brief getTitle ����ȫ·����ȡ�ļ���
	* param fileName ȫ·��
	*/
	std::string getTitle(std::string fileName);

	/*
	* brief obtainTileFileName ����ȫ·����ȡtile���ļ���
	* param fileName ȫ·��
	*/
	std::string obtainTileFileName(std::string fileName);

protected:
	/*
	* brief mInPath �ڵ�����·��
	*/
	std::string mInPath;

	/*
	* brief mCurTitle ��ǰ�������ļ���
	*/
	std::string mCurTitle;

	/*
	* brief mCurFullFileName ��ǰ�������ļ�ȫ��
	*/
	std::string mCurFullFileName;

	/*
	* brief mTitlePath �ļ���·��
	*/
	static std::stack<std::string> mTitlePath;
};

