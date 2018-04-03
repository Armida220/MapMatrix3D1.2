#pragma once
#include "NodeOperator.h"

namespace mm3dView
{
	/**
	* @brief CNodeAddOperator 操作增加节点 节点添加操作
	*/
	class CNodeAddOperator : public NodeOperator
	{
	public:
		CNodeAddOperator(osg::ref_ptr<osg::Group> parent, osg::ref_ptr<osg::Node> child, int id);

		/**
		* @brief operator 增加节点操作
		*/
		virtual void operator()();

		virtual ~CNodeAddOperator();

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