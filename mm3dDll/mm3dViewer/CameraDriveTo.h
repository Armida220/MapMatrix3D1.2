#pragma once
#include "NodeOperator.h"
#include <osgViewer/Viewer>

namespace mm3dView
{
	/**
	* @brief CCameraDriveTo 相机移动到(x, y)
	*/
	class CCameraDriveTo : public NodeOperator
	{
	public:
		CCameraDriveTo(osg::ref_ptr<osgViewer::Viewer> viewer,
			osg::Vec3d center);

		virtual ~CCameraDriveTo();

		/**
		* @brief operator 增加节点操作
		*/
		virtual void operator()();

	protected:
		/**
		* @brief mViewer 浏览器
		*/
		osg::ref_ptr<osgViewer::Viewer> mViewer;

		/**
		* @brief mCenter 浏览中心
		*/
		osg::Vec3d mCenter;
	};
}
