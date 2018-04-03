#pragma once
#include <string>
#include <osg/NodeVisitor>
#include <osg/PagedLOD>
#include <stack>

/*
* brief CTraveller osgb数据层级的遍历者，可以遍历所有的数据
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
	* brief apply 遍历到pagedlod节点时候的回调
	* param pagedLod 回调的pagedlod节点
	*/
	virtual void apply(osg::PagedLOD& pagedLod);

	/*
	* brief apply 遍历到子节点的回调
	* param node 回调的子节点
	*/
	virtual void applyChild(osg::Node* node);

	/*
	* brief clone 深拷贝
	*/
	virtual CTraveller* clone(std::string childPath);

	/*
	* brief getPath 根据全路径获取路径
	* param fileName 全路径
	*/
	std::string getPath(std::string fileName);

	/*
	* brief getTitle 根据全路径获取文件名
	* param fileName 全路径
	*/
	std::string getTitle(std::string fileName);

	/*
	* brief obtainTileFileName 根据全路径获取tile及文件名
	* param fileName 全路径
	*/
	std::string obtainTileFileName(std::string fileName);

protected:
	/*
	* brief mInPath 节点所在路径
	*/
	std::string mInPath;

	/*
	* brief mCurTitle 当前遍历的文件名
	*/
	std::string mCurTitle;

	/*
	* brief mCurFullFileName 当前遍历的文件全名
	*/
	std::string mCurFullFileName;

	/*
	* brief mTitlePath 文件名路径
	*/
	static std::stack<std::string> mTitlePath;
};

