#pragma once
#include "BaseUserEvent.h"
#include <memory>

namespace osgDraw
{
	class IBaseDrawer;
}

namespace osgEvent
{
	class COsgPicker;

	class CTimeKeeper;

	class COsgClipEvent : public CBaseUserEvent
	{
	public:
		COsgClipEvent(osgViewer::Viewer *viewer, osg::Group* constantGroup, osg::Group* variantGroup, osg::Group* root);
		virtual ~COsgClipEvent();

	protected:
		virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

		/**
		* @brief LfClkDw ���պ���
		* @brief worldMatrix �ֲ����������
		*/
		virtual void lfClkDw(osg::Matrix worldMatrix);

		/**
		* @brief mouseMvDw �ƶ����պ���
		* @brief worldMatrix �ֲ����������
		* @brief worldPt ����ƶ�����ײ��
		*/
		virtual void mouseMvDw(osg::Matrix worldMatrix, osg::Vec3d worldPt);

		/**
		* @brief bkSpackDw ���˻��պ���
		*/
		virtual void bkSpackDw();

		/**
		* @brief revertDw �������պ���
		*/
		virtual void revertDw();

		/**
		* @brief pushButton ���°�ť
		*/
		virtual void pushButton(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief releaseButton �ͷŰ���
		* @param ea gui�¼�
		*/
		virtual void releaseButton(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief mouseMove ����ƶ�
		* @param ea gui�¼�
		*/
		virtual void mouseMove(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief keyDown ����
		* @param ea gui�¼�
		*/
		virtual void keyDown(const osgGA::GUIEventAdapter &ea);

	protected:
		/**
		* @brief ���Ƶ�����
		*/
		std::vector<osg::Vec3d> vecWorldCoord;

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
	};
}

