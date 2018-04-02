#pragma once
#include <osg/Group>
#include "NodeOperator.h"

namespace mm3dView
{
	/**
	* @brief CRemoveAllOperator ������������ӽڵ�Ĳ���
	*/
	class CRemoveAllOperator : public NodeOperator
	{
	public:
		CRemoveAllOperator(osg::Group* group);
		virtual ~CRemoveAllOperator();

		/**
		* @brief operator ������������ӽڵ�Ĳ���
		*/
		virtual void operator()();

	private:
		/**
		* @brief mGroup ������������ӽڵ�ȫ��ɾ��
		*/
		osg::Group* mGroup;
	};
}


