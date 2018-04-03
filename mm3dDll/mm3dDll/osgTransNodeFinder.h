#pragma once
#include <osg/NodeVisitor>
#include <osg/MatrixTransform>
#include <vector>

/*
** brief trans�ڵ����
*/
class COsgTransNodeFinder : public osg::NodeVisitor
{
protected:
	/*
	** brief trans�ڵ�
	*/
	std::vector<osg::MatrixTransform*> vecTrans;

public:
	COsgTransNodeFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) 
	{

	}

	/*
	** brief findTransNode����trans�ڵ�
	** param node ��Ҫ���ҵĽڵ�
	** param vecTrans ��ѯ������trans�ڵ�
	*/
	void findTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecTrans);		//����trans�ڵ�

protected:
	virtual void apply(osg::MatrixTransform& node);

};