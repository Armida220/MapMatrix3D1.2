#pragma once
#include <osg/Group>
#include <osg/Node>

namespace mm3dView
{
	/**
	* @brief 操作节点 用于是更新还是删除
	*/
	class NodeOperator : public osg::Referenced
	{
	public:
		/**
		* @brief operator 进行操作
		*/
		virtual void operator()() {}

		virtual ~NodeOperator() {}
	};
}


