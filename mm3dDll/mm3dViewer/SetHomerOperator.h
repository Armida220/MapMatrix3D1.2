#pragma once
#include "NodeOperator.h"
#include <osgGA/TerrainManipulator>


namespace mm3dView
{
	class COsgModel;

	/**
	* @brief CSetHomerOperator �������ûؼҵĲ���
	*/
	class CSetHomerOperator : public NodeOperator
	{
	public:
		CSetHomerOperator(osg::ref_ptr<osgGA::TerrainManipulator> terrain, COsgModel* osgbModel);
		virtual ~CSetHomerOperator();

		/**
		* @brief operator ���ûؼ����ĵĲ���
		*/
		virtual void operator()();

	private:
		/**
		* @brief terrain ģ�͵Ĳٿ����������εĽ��вٿأ��Ƚ��ʺ�������ͳ���
		*/
		osg::ref_ptr<osgGA::TerrainManipulator> mTerrain;

		/**
		* @brief osgbModel ������ģ������
		*/
		COsgModel* mOsgbModel;
	};
}

