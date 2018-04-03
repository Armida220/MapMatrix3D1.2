#pragma once
#include <memory>
#include <osgGA/GUIEventAdapter>
#include <osg/Vec3d>
#include "EventHandler.h"

namespace osgEvent
{
	class COsgPicker;

	/**
	* @brief 高度锁定类,用于对高程进行锁定
	*/
	class COsgHeightLocker : public CEventHandler
	{
	public:
		COsgHeightLocker();
		virtual ~COsgHeightLocker();

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
		* @brief lock 开启高程锁定
		*/
		inline void lock()
		{
			bIsLockHeight = true;
		}

		/**
		* @brief lock 取消高程锁定
		*/
		inline void unlock()
		{
			bIsLockHeight = false;
		}

		/**
		* @brief setHeight 设置高度锁定
		* @param h 需要锁定的高度
		*/
		virtual void setHeight(double h);

		/**
		* @brief replace 替换原来的点击点的高程坐标为锁定高程坐标
		* @param clkPt 原来的点击点
		*/
		virtual void replace(osg::Vec3d &clkPt);

		/**
		* @brief shift 如果是开启状态，则进行关闭,返回最终切换状态
		*/
		virtual bool shift();

		/**
		* @brief getLockStatus 获取锁定高程状态
		*/
		inline bool getLockStatus()
		{
			return bIsLockHeight;
		}

	protected:
		/**
		* @brief bIsLockHeight 是否锁住高程
		*/
		bool bIsLockHeight;

		/**
		* @brief lockHeight 锁定的三维点高程
		*/
		double lockHeight;



	};
}



