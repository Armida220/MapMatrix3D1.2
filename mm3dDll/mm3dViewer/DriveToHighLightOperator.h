#pragma once
#include "NodeOperator.h"
#include <osgViewer/Viewer>

namespace mm3dView
{
	/**
	* @brief 操作浏览器视口到高亮组的中心位置
	*/
	class CDriveToHighLightOperator : public NodeOperator
	{
	public:
		CDriveToHighLightOperator(osg::ref_ptr<osgViewer::Viewer> viewer,
			osg::ref_ptr<osg::Group> highGroup);
		virtual ~CDriveToHighLightOperator();

		/**
		* @brief operator 移动视口到中心点
		*/
		virtual void operator()();

	private:
		/**
		* @brief mViewer 浏览器
		*/
		osg::ref_ptr<osgViewer::Viewer> mViewer;

		/**
		* @brief mHighGroup 高亮组
		*/
		osg::ref_ptr<osg::Group> mHighGroup;
	};
}

