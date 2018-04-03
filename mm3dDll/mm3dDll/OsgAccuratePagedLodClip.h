#pragma once
#include "traveller.h"
#include <string>
#include <vector>
/*
** brief 精确裁切类
*/
class CAccuratePagedLodClip : public CTraveller
{
public:
	CAccuratePagedLodClip(std::string inFilePath, std::string outFilePath, std::vector<osg::Vec3d> area, osg::Matrix mat);

protected:
	/*
	** brief applyChild 应用子类
	** param node 子节点
	*/
	virtual void applyChild(osg::Node* node);

	/*
	** brief clone 克隆
	** param childPath 子路径
	*/
	virtual CTraveller* clone(std::string childPath);

private:
	/*
	** brief 输出路径
	*/
	std::string mOutFilePath;
	
	/*
	** brief 输入路径
	*/
	std::string mInFilePath;
	
	/*
	** brief 裁切范围
	*/
	std::vector<osg::Vec3d> mArea;

	/*
	** brief 局部到世界矩阵
	*/
	osg::Matrix mMat;
};

