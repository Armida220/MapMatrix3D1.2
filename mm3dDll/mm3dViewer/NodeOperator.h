#pragma once
#include <osg/Group>
#include <osg/Node>

namespace mm3dView
{
	/**
	* @brief �����ڵ� �����Ǹ��»���ɾ��
	*/
	class NodeOperator : public osg::Referenced
	{
	public:
		/**
		* @brief operator ���в���
		*/
		virtual void operator()() {}

		virtual ~NodeOperator() {}
	};
}


