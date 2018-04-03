#pragma once
#include "BaseUserEvent.h"
#include <memory>
#include <osg/Group>


namespace osgCall
{
	class ICallBack;
}

namespace mm3dView
{
	class CSelfDefTerrainManipulator;
	class COsgModel;
}

namespace osgEvent
{
	class CBaseUserEvent;
	class CEventHandler;
	class COsgHeightLocker;
	class CCornerPtCatcher;
	class COsgPicker;
	class CPrompter;
	class CTimeKeeper;

	/**
	* @brief δ֪�¼���,��ʱ���ڲ�֪���¼�����ʱ����û�����
	*/
	class COsgUnknowEvent : public CBaseUserEvent
	{
	public:
		COsgUnknowEvent(osgViewer::Viewer *view, mm3dView::COsgModel* osgModel,
			std::shared_ptr<osgCall::ICallBack> ic);
		virtual ~COsgUnknowEvent();

		/**
		* @brief addHandler �����¼�������
		* @param pHandler ��Ҫ�����¼�������
		*/
		virtual void addHandler(std::shared_ptr<CEventHandler> pHandler);

		/**
		* @brief removeHandler �Ƴ��¼�������
		* @param pHandler ��Ҫ�Ƴ��¼�������
		*/
		virtual void removeHandler(std::shared_ptr<CEventHandler> pHandler);

	protected:
		virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

		/**
		* @brief pushButton ����
		* @param ea gui�¼�
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
		* @brief keyDown ���̰���
		* @param ea gui�¼�
		*/
		virtual void keyDown(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief notifyHandlerLfClk ֪ͨ��������������Ϣ
		* @param clkPt ���е�����
		*/
		virtual void notifyHandlerLfClk(osg::Vec3d &clkPt);

		/**
		* @brief notifyHandlerRightClk ֪ͨ����������һ���Ϣ
		* @param clkPt ���е�����
		*/
		virtual void notifyHandlerRightClk(osg::Vec3d &clkPt);

		/**
		* @brief notifyHandlerMouseMv ֪ͨ����������ƶ���Ϣ
		* @param clkPt ���е�����
		*/
		virtual void notifyHandlerMouseMv(osg::Vec3d &clkPt);
		/**
		* @brief back ������һ����
		*/
		virtual void back();
		virtual void shiftHeightLock(const osgGA::GUIEventAdapter &ea);
	public:
		/**
		* @brief shiftHeightLock �л������߶�.
		* @param ea �¼�
		*/
		virtual void shiftHeightLock(float x, float y);

		/**
		* @brief shiftCatchCorner �л���׽�ڽǵ�
		*/
		virtual void shiftCatchCorner();

		virtual void setLastConstDragLine(osg::Vec3d pt1, osg::Vec3d pt2);
		virtual void clearConstDragLine();

		/**
		* @brief shiftOperation �ı佻�������ķ�ʽ
		*/
		virtual void shiftOperation();

	private:
		/**
		* @brief mView �����
		*/
		osgViewer::Viewer *mView;

		/**
		* @brief mModel ����ģ��
		*/
		mm3dView::COsgModel* mModel;

		/**
		* @brief iCall �¼��ص���
		*/
		std::shared_ptr<osgCall::ICallBack> iCall;	

		/**
		* @brief vecHandler δ֪�¼�������ջ
		*/
		std::vector<std::shared_ptr<CEventHandler>> vecHandler;	

		std::shared_ptr<COsgHeightLocker> locker;
		std::shared_ptr<CCornerPtCatcher> catcher;

		/**
		* @brief mouseEventType ����¼�����
		*/
		int mouseEventType;

		/**
		* @brief mouseXY ��갴��ȥʱ�����Ļ����
		*/
		osg::Vec2d mouseXY;

		/**
		* @brief pCornerPtCatcher �ڽǵ㲶׽��
		*/
		std::shared_ptr<COsgPicker> pOsgPicker;

		/**
		* @brief pPrompter ��ʾ��Ϣ��
		*/
		std::shared_ptr<CPrompter> pPrompter;

		/**
		* @brief timeKeeper ��ʱ��
		*/
		std::shared_ptr<CTimeKeeper> timeKeeper;
		

	};


}



