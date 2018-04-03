#pragma once
#include <osgViewer/Viewer>
#include <OpenThreads/ReadWriteMutex>
#include "qcomm.h"

namespace mm3dView
{
	/**
	* @brief CSelDefViewer 自定义的三维浏览器，继承自osgviewer，对里面的方法进行重写，对多线程情况进行加锁处理
	*/
	class CSelDefViewer : public osgViewer::Viewer
	{
	public:
		CSelDefViewer();

	protected:
		virtual ~CSelDefViewer();
		/**
		* @brief frame 三维渲染的每一帧，对模型渲染每一帧都是通过这个函数进行绘制
		*/
		virtual void frame(double simulationTime = USE_REFERENCE_TIME);

	private:

		/**
		* @brief pushEvents 事件
		*/
		std::deque<osg::ref_ptr<osgGA::Event>> pushEvents;

	};
}

