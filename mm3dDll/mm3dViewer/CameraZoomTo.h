#pragma once
#include "NodeOperator.h"

namespace mm3dView
{
	class CSelfDefTerrainManipulator;

	/**
	* @brief CCameraZoomTo ������ŵ��̶��߶�
	*/
	class CCameraZoomTo : public NodeOperator
	{
	public:
		CCameraZoomTo(CSelfDefTerrainManipulator* manipulator,
			double scale);
		virtual ~CCameraZoomTo();

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
		* @brief scale ���ŵ��ĳ߶�
		*/
		double mScale;
	};

}

