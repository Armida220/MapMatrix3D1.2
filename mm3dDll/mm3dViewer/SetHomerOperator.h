#pragma once
#include "NodeOperator.h"
#include <osgGA/TerrainManipulator>


namespace mm3dView
{
	class COsgModel;

	/**
	* @brief CSetHomerOperator 操作设置回家的操作
	*/
	class CSetHomerOperator : public NodeOperator
	{
	public:
		CSetHomerOperator(osg::ref_ptr<osgGA::TerrainManipulator> terrain, COsgModel* osgbModel);
		virtual ~CSetHomerOperator();

		/**
		* @brief operator 设置回家中心的操作
		*/
		virtual void operator()();

	private:
		/**
		* @brief terrain 模型的操控器，按地形的进行操控，比较适合浏览大型场景
		*/
		osg::ref_ptr<osgGA::TerrainManipulator> mTerrain;

		/**
		* @brief osgbModel 场景的模型数据
		*/
		COsgModel* mOsgbModel;
	};
}

