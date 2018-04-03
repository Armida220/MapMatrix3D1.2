#pragma once
#include "BaseUserEvent.h"
#include <memory>
#include <map>

namespace osgCall
{
	class ICallBack;
}

namespace osgDraw
{
	class IBaseDrawer;
}

namespace osgEvent
{
	class COsgPicker;

	class CTimeKeeper;

	class COsgAbsOriEvent : public CBaseUserEvent
	{
	public:
		COsgAbsOriEvent(osgViewer::Viewer *viewer, osg::Group* constantGroup, osg::Group* variantGroup, 
			osg::Group* root, std::shared_ptr<osgCall::ICallBack> ic);
		virtual ~COsgAbsOriEvent();

		/**
		* @brief ��ȡ��������
		*/
		virtual void fetchOriData();

		/**
		* @brief ��ʾ��������
		*/
		virtual void showOriPt(std::string str);

		/**
		* @brief �����������
		*/
		virtual void clearOriPt();

	protected:
		virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

		virtual void pushButton(const osgGA::GUIEventAdapter &ea);

		virtual void releaseButton(const osgGA::GUIEventAdapter &ea);
		
	protected:
		/**
		* @brief �Ӿ���
		*/
		osgViewer::Viewer *mViewer;

		/**
		* @brief constant��
		*/
		osg::Group* mConstantGroup;

		/**
		* @brief variant��
		*/
		osg::Group* mVariantGroup;

		/**
		* @brief ���ڵ�
		*/
		osg::Group* mRoot;

		/**
		* @brief pCornerPtCatcher �ڽǵ㲶׽��
		*/
		std::shared_ptr<COsgPicker> pOsgPicker;

		/**
		* @brief pConstantDrawer constant��ͼ��
		*/
		std::shared_ptr<osgDraw::IBaseDrawer> pConstantDrawer;

		/**
		* @brief pConstantDrawer variant��ͼ��
		*/
		std::shared_ptr<osgDraw::IBaseDrawer> pVariantDrawer;

		/**
		* @brief timeKeeper ��ʱ��
		*/
		std::shared_ptr<CTimeKeeper> timeKeeper;

		/**
		* @brief mouseEventType ����¼�����
		*/
		int mouseEventType;

		/**
		* @brief iCall �¼��ص���
		*/
		std::shared_ptr<osgCall::ICallBack> iCall;
	};


}
