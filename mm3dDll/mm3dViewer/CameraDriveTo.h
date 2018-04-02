#pragma once
#include "NodeOperator.h"
#include <osgViewer/Viewer>

namespace mm3dView
{
	/**
	* @brief CCameraDriveTo ����ƶ���(x, y)
	*/
	class CCameraDriveTo : public NodeOperator
	{
	public:
		CCameraDriveTo(osg::ref_ptr<osgViewer::Viewer> viewer,
			osg::Vec3d center);

		virtual ~CCameraDriveTo();

		/**
		* @brief operator ���ӽڵ����
		*/
		virtual void operator()();

	protected:
		/**
		* @brief mViewer �����
		*/
		osg::ref_ptr<osgViewer::Viewer> mViewer;

		/**
		* @brief mCenter �������
		*/
		osg::Vec3d mCenter;
	};
}
