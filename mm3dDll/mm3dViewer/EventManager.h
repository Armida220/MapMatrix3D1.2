#pragma once
#include <vector>
#include "BaseUserEvent.h"
#include "osgViewer/Viewer"
#include <osg/Node>

namespace osgEvent
{
	/**
	* @brief 切换事件管理器，可以添加事件，拥有多个子事件，并在子事件中进行切换
	*/
	class CSwitchEventManager : public CBaseUserEvent
	{
	public:
		CSwitchEventManager(osgViewer::Viewer* viewer);

		/**
		* @brief addEvnet 增加事件
		* @param key  事件的序号
		* @param name 事件的名称
		* @param ce 的事件
		*/
		virtual void addEvent(int key, std::string name, CBaseUserEvent *ce);

		/**
		* @brief selectEventByNum 根据序号选择事件
		* @param num 序号
		*/
		virtual void selectEventByNum(unsigned int num);

		/**
		* @brief setCtrlPtNo 设置控制点序号
		* @param ctrlNo 控制点序号
		*/
		/*virtual void setCtrlPtNo(std::string ctrlNo);*/

		/**
		* @brief showOriPt 显示定向点
		* @param oriRes 刺点结果
		*/
		virtual void showOriPt(std::string oriRes);

		/**
		* @brief clearOriPt 清除定向点
		*/
		virtual void clearOriPt();

		/**
		* @brief fetchOriData 获取定向数据
		*/
		virtual void fetchOriData();
		
		virtual void shiftHeightLock(float x, float y);

		/**
		* @brief shiftCatchCorner 切换捕捉内角点
		*/
		virtual void shiftCatchCorner();

		/**
		* @brief shiftOperation 改变交互操作的方式
		*/
		virtual void shiftOperation();

	protected:
		bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	private:
		typedef std::pair<std::string, osg::ref_ptr<CBaseUserEvent>> NamedBaseEvent;
		typedef std::map<int, NamedBaseEvent> KeyMainMap;

		/**
		* @brief 事件map容器
		*/
		KeyMainMap _manips;

		/**
		* @brief 当前事件
		*/
		osg::ref_ptr<CBaseUserEvent> _current;

	};

}
