#pragma once
#include <osgViewer/Viewer>
#include <osg/Group>
#include <memory>

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
	class IOsgEvent
	{
	public:
		/**
		 * @brief addEventToViewer 增加事件到浏览器
		 * @param view  浏览器
		 */
		virtual void addEventToViewer(osgViewer::Viewer* view) = 0;

		/**
		* @brief startEvent 切换事件
		* @param order 切换事件序号
		*/
		virtual void startEvent(int order) = 0;

		virtual void shiftCatchCorner() = 0;
		virtual void shiftHeightLock(int x, int y) = 0;
		virtual void shiftOperation() = 0;
		virtual void setLastConstDragLine(osg::Vec3d pt1, osg::Vec3d pt2) = 0;
		virtual void clearConstDragLine() = 0;

	protected:
		/**
		 * @brief 不允许delete
		 */
		~IOsgEvent() {}

	};

	/**
	 * @brief 生产绘制事件的接口的工厂
	 */
	class IOsgEventFactory
	{
	public:
		/**
		 * @brief create 建造浏览器事件窗口
		 * @param view  浏览器
		 * @param root 场景根节点
		 */
		typedef std::shared_ptr<IOsgEvent> ptrDrawEvent;
		static ptrDrawEvent create(osgViewer::Viewer *view, mm3dView::COsgModel* pModel,
			std::shared_ptr<osgCall::ICallBack> ic);
	};
}
