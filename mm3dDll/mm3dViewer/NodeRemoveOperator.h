#pragma once
#include "NodeOperator.h"

namespace mm3dView
{
	/**
	* @brief CNodeRemoveOperator 操作清除节点 用于是删除操作
	*/
	class CNodeRemoveOperator : public NodeOperator
	{
	public:
		CNodeRemoveOperator(osg::ref_ptr<osg::Group> parent, osg::ref_ptr<osg::Node> child, int id);

		/**
		* @brief operator 进行节点清除
		*/
		virtual void operator()();

		virtual ~CNodeRemoveOperator();

	private:
		/**
		* @brief spParent 父节点
		*/
		osg::ref_ptr<osg::Group> spParent;

		/**
		* @brief spChild 子节点
		*/
		osg::ref_ptr<osg::Node> spChild;

		/**
		* @brief id 操作标记
		*/
		int mId;
	};

}
