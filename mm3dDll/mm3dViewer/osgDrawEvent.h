#pragma once
#include "IOsgDrawEvent.h"
#include "EventManager.h"

namespace osgDraw
{
	class IBaseDrawer;
}

namespace osgEvent
{
	class CBaseUserEvent;
}

namespace osgCall 
{
	class ICallBack;
}

namespace mm3dView
{
	class COsgModel;
}

namespace osgEvent
{
	class CBaseUserEvent;
	/**
	* @brief 绘制事件的实例
	*/
	class COsgDrawEvent : public IOsgDrawEvent
	{
	public:
		COsgDrawEvent(osgViewer::Viewer *view, mm3dView::COsgModel* osgModel, std::shared_ptr<osgCall::ICallBack> ic);

		/**
		 * @brief addEventToViewer 增加事件到浏览器
		 * @param view  浏览器
		 */
		virtual void addEventToViewer(osgViewer::Viewer* view);

		virtual void shiftCatchCorner();
		virtual void shiftHeightLock(int x, int y);
		virtual void shiftOperation();
		virtual void setLastConstDragLine(osg::Vec3d pt1, osg::Vec3d pt2);
		virtual void clearConstDragLine();
	private:
		/**
		* @brief manager 切换事件管理器，可以添加事件，拥有多个子事件，并在子事件中进行切换
		*/
		osg::ref_ptr<CSwitchEventManager> manager;
		osg::ref_ptr<CBaseUserEvent> userEvent;
	};
}
