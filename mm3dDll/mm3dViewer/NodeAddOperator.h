#pragma once
#include "NodeOperator.h"

namespace mm3dView
{
	/**
	* @brief CNodeAddOperator �������ӽڵ� �ڵ���Ӳ���
	*/
	class CNodeAddOperator : public NodeOperator
	{
	public:
		CNodeAddOperator(osg::ref_ptr<osg::Group> parent, osg::ref_ptr<osg::Node> child, int id);

		/**
		* @brief operator ���ӽڵ����
		*/
		virtual void operator()();

		virtual ~CNodeAddOperator();

	private:
		/**
		* @brief spParent ���ڵ�
		*/
		osg::ref_ptr<osg::Group> spParent;

		/**
		* @brief spChild �ӽڵ�
		*/
		osg::ref_ptr<osg::Node> spChild;

		/**
		* @brief id �������
		*/
		int mId;
	};


}