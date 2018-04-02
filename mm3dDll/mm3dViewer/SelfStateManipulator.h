#pragma once
#include <osgGA/StateSetManipulator>

namespace mm3dView
{
	/**
	* @brief CSelfStateManipulator  自定义的用于操控场景状态的控制器，
	* @brief 把原来的按键切换状态去除了
	*/
	class CSelfStateManipulator : public osgGA::StateSetManipulator
	{
	public:
		CSelfStateManipulator(osg::StateSet* stateset = 0);
		virtual ~CSelfStateManipulator();

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

		/**
		* @brief toggleTexture  去除和开启纹理
		*/
		virtual void toggleTexture();

		/**
		* @brief light  光源切换
		*/
		virtual void light();
	};

}

