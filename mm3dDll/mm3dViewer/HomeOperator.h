#pragma once
#include "NodeOperator.h"

namespace osgGA
{
	class TerrainManipulator;
}

namespace mm3dView
{
	/**
	* @brief CHomeOperator 进行回家的操作
	*/
	class CHomeOperator : public NodeOperator
	{
	public:
		CHomeOperator(osg::ref_ptr<osgGA::TerrainManipulator> terrain);
		virtual ~CHomeOperator();

		/**
		* @brief operator 增加节点操作
		*/
		virtual void operator()();

	private:
		/**
		* @brief terrain 模型的操控器，按地形的进行操控，比较适合浏览大型场景
		*/
		osg::ref_ptr<osgGA::TerrainManipulator> mTerrain;

	};

}


