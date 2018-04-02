#pragma once
#include <osg/Vec3d>
#include <vector>
#include "EventHandler.h"

namespace osgEvent
{
	/**
	* @brief 内角点捕捉类
	*/
	class CCornerPtCatcher : public CEventHandler
	{
	public:
		CCornerPtCatcher();
		virtual ~CCornerPtCatcher();

		/**
		* @brief lfClk 左击事件
		* @param clkPt 击中点
		*/
		virtual void lfClk(osg::Vec3d &clkPt);

		/**
		* @brief mouseMv 鼠标移动事件
		* @param clkPt 击中点
		*/
		virtual void mouseMv(osg::Vec3d &clkPt);

		/**
		* @brief dbClk 双击事件
		* @param clkPt 击中点
		*/
		virtual void dbClk(const osg::Vec3d &clkPt);

		/**
		* @brief rightClk 右击事件
		* @param clkPt 击中点
		*/
		virtual bool rightClk(const osg::Vec3d &clkPt);

		/**
		* @brief keyDown 按键事件
		* @param ea 事件
		*/
		virtual bool keyDown(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief shift 事件切换
		*/
		virtual bool shift();

		/**
		* @brief pop 把点击点出栈一个
		*/
		virtual void pop();

		virtual void setLastConstDragLine(osg::Vec3d pt1, osg::Vec3d pt2);
		virtual void clearConstDragLine();

	protected:
		/**
		* @brief replace 替换原来的点击点成为新的捕捉点
		* @param worldPt 原来的点击点
		*/
		virtual void catchPt(osg::Vec3d &clkPt);

		/**
		* @brief push 把点击点保存起来
		* @param clkPt 原来的点击点
		*/
		virtual void push(osg::Vec3d clkPt);

		/**
		* @brief clear 把点击点清除掉
		*/
		virtual void clear();

		/**
		* @brief getCatchStatus 获取捕捉状态
		*/
		inline bool getCatchStatus()
		{
			return bIsCatch;
		}

		/**
		* @brief calCornerPt 计算内角点
		* @param pt1  倒数第一个点
		* @param pt2  倒数第二个点
		* @param pt3  当前碰撞的点
		*/
		osg::Vec3d calCornerPt(const osg::Vec3d pt1, const osg::Vec3d pt2, const osg::Vec3d pt3);

	protected:
		/**
		* @brief bIsCatch 是否捕捉内墙角点
		*/
		bool bIsCatch;

		/**
		* @brief vecClkPt 保存的三维测点坐标
		*/
		std::vector<osg::Vec3d> vecClkPt;


	};



}
