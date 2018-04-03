#pragma once
#include <osg/Group>
#include "NodeOperator.h"

namespace mm3dView
{
	/**
	* @brief CRemoveAllOperator 用于清除所有子节点的操作
	*/
	class CRemoveAllOperator : public NodeOperator
	{
	public:
		CRemoveAllOperator(osg::Group* group);
		virtual ~CRemoveAllOperator();

		/**
		* @brief operator 用于清除所有子节点的操作
		*/
		virtual void operator()();

	private:
		/**
		* @brief mGroup 把它的下面的子节点全部删除
		*/
		osg::Group* mGroup;
	};
}


