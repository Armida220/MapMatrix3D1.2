#pragma once
#include <osgViewer/Viewer>
#include <osg/Node>
#include <osg/Group>
#include <osg/Matrix>



namespace osgEvent
{
	/*
	* @brief CEventHandler 事件处理类，基类，需要重写下列接口
	*/
	class CEventHandler
		{
	public:
		CEventHandler();
		virtual ~CEventHandler();

		/**
		* @brief lfClk 左击事件
		* @param clkPt 击中点
		*/
		virtual void lfClk(osg::Vec3d &clkPt) = 0;

		/**
		* @brief mouseMv 鼠标移动事件
		* @param clkPt 击中点
		*/
		virtual void mouseMv(osg::Vec3d &clkPt) = 0;

		/**
		* @brief dbClk 双击事件
		* @param clkPt 击中点
		*/
		virtual void dbClk(const osg::Vec3d &clkPt) = 0;

		/**
		* @brief rightClk 右击事件
		* @param clkPt 击中点
		*/
		virtual bool rightClk(const osg::Vec3d &clkPt) = 0;

		/**
		* @brief keyDown 按键事件
		* @param ea 事件
		*/
		virtual bool keyDown(const osgGA::GUIEventAdapter &ea) = 0;

		/**
		* @brief start 切换事件, 返回最终的切换状态
		*/
		virtual bool shift() = 0;

		/**
		* @brief clearGroupResult 清除组结果
		* @param group 需要清除的组
		* @param root  获取更新回调的组
		*/
		void clearGroupResult(osg::ref_ptr<osg::Group> group, osg::Group* root) const;
	};
}

