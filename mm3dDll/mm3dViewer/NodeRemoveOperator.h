#pragma once
#include "NodeOperator.h"

namespace mm3dView
{
	/**
	* @brief CNodeRemoveOperator ��������ڵ� ������ɾ������
	*/
	class CNodeRemoveOperator : public NodeOperator
	{
	public:
		CNodeRemoveOperator(osg::ref_ptr<osg::Group> parent, osg::ref_ptr<osg::Node> child, int id);

		/**
		* @brief operator ���нڵ����
		*/
		virtual void operator()();

		virtual ~CNodeRemoveOperator();

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
