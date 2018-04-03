#pragma once
#include "NodeOperator.h"

namespace mm3dView
{
	class CSelfDefTerrainManipulator;

	/**
	* @brief CCameraZoomTo 相机缩放到固定尺度
	*/
	class CCameraZoomTo : public NodeOperator
	{
	public:
		CCameraZoomTo(CSelfDefTerrainManipulator* manipulator,
			double scale);
		virtual ~CCameraZoomTo();

		/**
		* @brief operator 平移操作
		*/
		virtual void operator()();

	protected:
		/**
		* @brief mManipulator 场景操控器
		*/
		osg::ref_ptr<CSelfDefTerrainManipulator> mManipulator;

		/**
		* @brief scale 缩放到的尺度
		*/
		double mScale;
	};

}

