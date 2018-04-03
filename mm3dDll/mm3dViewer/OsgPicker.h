#pragma once
#include <osg/Vec3d>
#include <osg/Node>
#include <osg/Matrix>
#include <osgViewer/Viewer>

namespace osgEvent
{
	/**
	* @brief 碰撞检测获取的结果
	*/
	class CPickResult
	{
	public:
		/**
		* @brief 原始点
		*/
		osg::Vec3d localPt;

		/**
		* @brief 世界坐标点
		*/
		osg::Vec3d worldPt;

		/**
		* @brief 法向量
		*/
		osg::Vec3d worldNm;

		/**
		* @brief 第几个模型被选中
		*/
		int modelIndex;

		/**
		* @brief 碰撞中节点遍历路径
		*/
		osg::NodePath nodePath;

		/**
		* @brief 最低层点的父子节点
		*/
		osg::ref_ptr<osg::Node> parent;

		/**
		* @brief 物体转到世界的矩阵
		*/
		osg::Matrix matrix;
	};

	/**
	* @brief 碰撞检测器
	*/
	class COsgPicker
	{
	public:
		COsgPicker(osgViewer::Viewer *view);
		virtual ~COsgPicker();

		/**
		* @brief pickResult 碰撞检测
		* @param x 浏览器视口上的x坐标
		* @param y 浏览器视口上的y坐标
		*/
		virtual const CPickResult pickResult(const float x, const float y) const;

		/**
		* @brief linePick 碰撞检测
		* @param pUp 顶上端点
		* @param pDown 下面的端点
		* @param mView 浏览器
		*/
		virtual const CPickResult linePick(const osg::Vec3d &pUp, const osg::Vec3d &pDown, osg::Group *scene) const;

		/**
		* @brief DefaultlinePick 默认的垂直碰撞检测
		* @param x y 平面的xy坐标
		* @param mRoot 碰撞的节点
		*/
		const CPickResult DefaultlinePick(double x, double y, osg::Group *mRoot) const;
		
		std::vector<osg::Vec3d> DefaultlinePickAll(double x, double y, osg::Group *mRoot) const;

	protected:
		osg::ref_ptr<osgViewer::Viewer> mView;
	};

}

