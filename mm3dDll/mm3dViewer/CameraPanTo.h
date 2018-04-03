#pragma once
#include "NodeOperator.h"

namespace mm3dView
{
	class CSelfDefTerrainManipulator;
	
	/**
	* @brief CCameraPanTo 相机平移到(x, y)
	*/
	class CCameraPanTo : public NodeOperator
	{
	public:
		CCameraPanTo(CSelfDefTerrainManipulator* manipulator,
			double x, double y);
		virtual ~CCameraPanTo();

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
		* @brief mX 平移到位置x
		*/
		double mX;

		/**
		* @brief mY 平移到位置y
		*/
		double mY;
	};

}

