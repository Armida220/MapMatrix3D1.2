#pragma once
#include "NodeOperator.h"
#include <osgViewer/Viewer>

namespace mm3dView
{
	/**
	* @brief ����������ӿڵ������������λ��
	*/
	class CDriveToHighLightOperator : public NodeOperator
	{
	public:
		CDriveToHighLightOperator(osg::ref_ptr<osgViewer::Viewer> viewer,
			osg::ref_ptr<osg::Group> highGroup);
		virtual ~CDriveToHighLightOperator();

		/**
		* @brief operator �ƶ��ӿڵ����ĵ�
		*/
		virtual void operator()();

	private:
		/**
		* @brief mViewer �����
		*/
		osg::ref_ptr<osgViewer::Viewer> mViewer;

		/**
		* @brief mHighGroup ������
		*/
		osg::ref_ptr<osg::Group> mHighGroup;
	};
}

