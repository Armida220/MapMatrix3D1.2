#pragma once
#include "EventHandler.h"

namespace osgEvent
{
	class COsgUnknowEvent;
	/**
	* @brief 未知事件处理类
	*/
	class CUnknowHandler : public CEventHandler
	{
	public:
		CUnknowHandler(osg::Group *root, osg::Group* mapGroup, osg::Group* editGroup,
			osg::Group* tempGroup, COsgUnknowEvent* parent);
		virtual ~CUnknowHandler();

	protected:
		/**
		* @brief lfClk 左击
		* @param clkPt 最后的点坐标，在锁定高程时候，高度是锁定高度，不锁定时候和worldPt一致
		*/
		virtual void lfClk(const osg::Vec3d &clkPt);

		/**
		* @brief mouseMv 鼠标移动
		* @param clkPt 移动点的坐标，世界坐标系
		*/
		virtual void mouseMv(const osg::Vec3d &clkPt);

		/**
		* @brief dbClk 双击
		* @param worldPt 双击点的坐标，世界坐标系
		* @param worldMatrix 物体到世界坐标系矩阵
		*/
		virtual void dbClk(const osg::Vec3d &clkPt);

		/**
		* @brief rightClk 右击
		* @param worldPt 右点的坐标，世界坐标系
		* @param worldMatrix 物体到世界坐标系矩阵
		*/
		virtual bool rightClk(const osg::Vec3d &clkPt);

	protected:
		/**
		* @brief vecWorldCoord 存储鼠标点击的点的坐标信息，是世界坐标系
		*/
		std::vector<osg::Vec3d> vecWorldCoord;

		/**
		* @brief mRoot 场景根节点
		*/
		osg::Group* mRoot;

		/**
		* @brief mapGroup 保存绘制图形的组
		*/
		osg::Group* mMapGroup;

		/**
		* @brief mEditGroup 保存绘制图形的组
		*/
		osg::Group* mEditGroup;

		/**
		* @brief mTempGroup 保存临时的组
		*/
		osg::Group* mTempGroup;

		/**
		* @brief mParent 父节点指向unKnowEvent
		*/
		COsgUnknowEvent* mParent;

	};

}


