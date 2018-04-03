#include "StdAfx.h"
#include "OsgTransNodeFinder.h"



void COsgTransNodeFinder::findTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecTrans)
{
	if (!node)
	{
		return;
	}

	COsgTransNodeFinder ive;
	node->accept(ive);
	vecTrans = ive.vecTrans;
}

void COsgTransNodeFinder::apply(osg::MatrixTransform& node)
{
	if (node.getName() != "repairMatrix")
	{
		osg::Matrix matrix = node.getMatrix();
		vecTrans.push_back(&node);
	}

	traverse(node);
}