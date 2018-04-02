#pragma once
#include "NodeOperator.h"

namespace mm3dView
{
	class CSelfDefTerrainManipulator;
	
	/**
	* @brief CCameraPanTo ���ƽ�Ƶ�(x, y)
	*/
	class CCameraPanTo : public NodeOperator
	{
	public:
		CCameraPanTo(CSelfDefTerrainManipulator* manipulator,
			double x, double y);
		virtual ~CCameraPanTo();

		/**
		* @brief operator ƽ�Ʋ���
		*/
		virtual void operator()();

	protected:
		/**
		* @brief mManipulator �����ٿ���
		*/
		osg::ref_ptr<CSelfDefTerrainManipulator> mManipulator;

		/**
		* @brief mX ƽ�Ƶ�λ��x
		*/
		double mX;

		/**
		* @brief mY ƽ�Ƶ�λ��y
		*/
		double mY;
	};

}

