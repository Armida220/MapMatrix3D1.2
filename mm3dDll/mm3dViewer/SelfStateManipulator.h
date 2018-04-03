#pragma once
#include <osgGA/StateSetManipulator>

namespace mm3dView
{
	/**
	* @brief CSelfStateManipulator  �Զ�������ڲٿس���״̬�Ŀ�������
	* @brief ��ԭ���İ����л�״̬ȥ����
	*/
	class CSelfStateManipulator : public osgGA::StateSetManipulator
	{
	public:
		CSelfStateManipulator(osg::StateSet* stateset = 0);
		virtual ~CSelfStateManipulator();

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

		/**
		* @brief toggleTexture  ȥ���Ϳ�������
		*/
		virtual void toggleTexture();

		/**
		* @brief light  ��Դ�л�
		*/
		virtual void light();
	};

}

