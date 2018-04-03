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
		* @brief 获取定向数据
		*/
		virtual void fetchOriData();

		/**
		* @brief 显示定向数据
		*/
		virtual void showOriPt(std::string str);

		/**
		* @brief 清除定向数据
		*/
		virtual void clearOriPt();

	protected:
		virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

		virtual void pushButton(const osgGA::GUIEventAdapter &ea);

		virtual void releaseButton(const osgGA::GUIEventAdapter &ea);
		
	protected:
		/**
		* @brief 视景体
		*/
		osgViewer::Viewer *mViewer;

		/**
		* @brief constant线
		*/
		osg::Group* mConstantGroup;

		/**
		* @brief variant线
		*/
		osg::Group* mVariantGroup;

		/**
		* @brief 根节点
		*/
		osg::Group* mRoot;

		/**
		* @brief pCornerPtCatcher 内角点捕捉类
		*/
		std::shared_ptr<COsgPicker> pOsgPicker;

		/**
		* @brief pConstantDrawer constant画图类
		*/
		std::shared_ptr<osgDraw::IBaseDrawer> pConstantDrawer;

		/**
		* @brief pConstantDrawer variant画图类
		*/
		std::shared_ptr<osgDraw::IBaseDrawer> pVariantDrawer;

		/**
		* @brief timeKeeper 计时器
		*/
		std::shared_ptr<CTimeKeeper> timeKeeper;

		/**
		* @brief mouseEventType 鼠标事件类型
		*/
		int mouseEventType;

		/**
		* @brief iCall 事件回调类
		*/
		std::shared_ptr<osgCall::ICallBack> iCall;
	};


}
