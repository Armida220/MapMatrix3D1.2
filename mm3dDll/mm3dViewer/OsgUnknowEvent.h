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
	* @brief 未知事件类,暂时用于不知道事件类型时候的用户交互
	*/
	class COsgUnknowEvent : public CBaseUserEvent
	{
	public:
		COsgUnknowEvent(osgViewer::Viewer *view, mm3dView::COsgModel* osgModel,
			std::shared_ptr<osgCall::ICallBack> ic);
		virtual ~COsgUnknowEvent();

		/**
		* @brief addHandler 增加事件处理类
		* @param pHandler 需要增加事件处理类
		*/
		virtual void addHandler(std::shared_ptr<CEventHandler> pHandler);

		/**
		* @brief removeHandler 移除事件处理类
		* @param pHandler 需要移除事件处理类
		*/
		virtual void removeHandler(std::shared_ptr<CEventHandler> pHandler);

	protected:
		virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

		/**
		* @brief pushButton 按键
		* @param ea gui事件
		*/
		virtual void pushButton(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief releaseButton 释放按键
		* @param ea gui事件
		*/
		virtual void releaseButton(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief mouseMove 鼠标移动
		* @param ea gui事件
		*/
		virtual void mouseMove(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief keyDown 键盘按键
		* @param ea gui事件
		*/
		virtual void keyDown(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief notifyHandlerLfClk 通知处理类鼠标左击消息
		* @param clkPt 击中点坐标
		*/
		virtual void notifyHandlerLfClk(osg::Vec3d &clkPt);

		/**
		* @brief notifyHandlerRightClk 通知处理类鼠标右击消息
		* @param clkPt 击中点坐标
		*/
		virtual void notifyHandlerRightClk(osg::Vec3d &clkPt);

		/**
		* @brief notifyHandlerMouseMv 通知处理类鼠标移动消息
		* @param clkPt 击中点坐标
		*/
		virtual void notifyHandlerMouseMv(osg::Vec3d &clkPt);
		/**
		* @brief back 回退上一个点
		*/
		virtual void back();
		virtual void shiftHeightLock(const osgGA::GUIEventAdapter &ea);
	public:
		/**
		* @brief shiftHeightLock 切换锁定高度.
		* @param ea 事件
		*/
		virtual void shiftHeightLock(float x, float y);

		/**
		* @brief shiftCatchCorner 切换捕捉内角点
		*/
		virtual void shiftCatchCorner();

		virtual void setLastConstDragLine(osg::Vec3d pt1, osg::Vec3d pt2);
		virtual void clearConstDragLine();

		/**
		* @brief shiftOperation 改变交互操作的方式
		*/
		virtual void shiftOperation();

	private:
		/**
		* @brief mView 浏览器
		*/
		osgViewer::Viewer *mView;

		/**
		* @brief mModel 场景模型
		*/
		mm3dView::COsgModel* mModel;

		/**
		* @brief iCall 事件回调类
		*/
		std::shared_ptr<osgCall::ICallBack> iCall;	

		/**
		* @brief vecHandler 未知事件处理类栈
		*/
		std::vector<std::shared_ptr<CEventHandler>> vecHandler;	

		std::shared_ptr<COsgHeightLocker> locker;
		std::shared_ptr<CCornerPtCatcher> catcher;

		/**
		* @brief mouseEventType 鼠标事件类型
		*/
		int mouseEventType;

		/**
		* @brief mouseXY 鼠标按下去时候的屏幕坐标
		*/
		osg::Vec2d mouseXY;

		/**
		* @brief pCornerPtCatcher 内角点捕捉类
		*/
		std::shared_ptr<COsgPicker> pOsgPicker;

		/**
		* @brief pPrompter 提示信息类
		*/
		std::shared_ptr<CPrompter> pPrompter;

		/**
		* @brief timeKeeper 计时器
		*/
		std::shared_ptr<CTimeKeeper> timeKeeper;
		

	};


}



