#pragma once
#include <osg/NodeVisitor>
#include <osg/MatrixTransform>
#include <vector>

/*
** brief trans节点查找
*/
class COsgTransNodeFinder : public osg::NodeVisitor
{
protected:
	/*
	** brief trans节点
	*/
	std::vector<osg::MatrixTransform*> vecTrans;

public:
	COsgTransNodeFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{

	}

	/*
	** brief findTransNode查找trans节点
	** param node 需要查找的节点
	** param vecTrans 查询回来的trans节点
	*/
	void findTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecTrans);		//查找trans节点

protected:
	virtual void apply(osg::MatrixTransform& node);

};