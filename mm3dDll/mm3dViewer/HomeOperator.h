#pragma once
#include "NodeOperator.h"

namespace osgGA
{
	class TerrainManipulator;
}

namespace mm3dView
{
	/**
	* @brief CHomeOperator ���лؼҵĲ���
	*/
	class CHomeOperator : public NodeOperator
	{
	public:
		CHomeOperator(osg::ref_ptr<osgGA::TerrainManipulator> terrain);
		virtual ~CHomeOperator();

		/**
		* @brief operator ���ӽڵ����
		*/
		virtual void operator()();

	private:
		/**
		* @brief terrain ģ�͵Ĳٿ����������εĽ��вٿأ��Ƚ��ʺ�������ͳ���
		*/
		osg::ref_ptr<osgGA::TerrainManipulator> mTerrain;

	};

}


