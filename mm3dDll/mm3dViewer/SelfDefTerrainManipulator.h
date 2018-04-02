#pragma once
#include <osgGA/TerrainManipulator>
#include <osg/Node>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include "ICallBack.h"
#include <osg/Vec3d>
#include <deque>
#include <chrono>

namespace osgCall
{
	class ICallBack;
}

namespace mm3dView
{
	/**
	* @brief stViewPara 视角参数
	*/
	struct stViewPara
	{
		stViewPara(osg::Vec3d eye, osg::Vec3d center, osg::Vec3d up)
			: mEye(eye), mCenter(center), mUp(up)
		{
		}

		/**
		* @brief mEye 视点位置
		*/
		osg::Vec3d mEye;
		
		/**
		* @brief mCenter 物点位置
		*/
		osg::Vec3d mCenter;

		/**
		* @brief mUp 相机方向
		*/
		osg::Vec3d mUp;
	};

	/**
	* @brief CSelfDefTerrainManipulator 用户自定义操控器
	*/
	class CSelfDefTerrainManipulator : public osgGA::TerrainManipulator
	{
	public:
		CSelfDefTerrainManipulator(osg::ref_ptr<osg::Camera> camera, std::shared_ptr<osgCall::ICallBack> pCall);

		/**
		* @brief topView 顶视图
		*/
		virtual void topView();

		/**
		* @brief leftView 顶视图
		*/
		virtual void leftView();

		/**
		* @brief rightView 右视图
		*/
		virtual void rightView();

		/**
		* @brief getRotation 获得旋转信息
		*/
		inline osg::Quat getRotation()
		{
			return _rotation;
		}

		/**
		* @brief getSide 根据旋转矩阵, 获得旁向的方向向量
		* @param mat 旋转矩阵
		*/
		osg::Vec3d getSide(const osg::Matrix& mat);

		/**
		* @brief getFront 根据旋转矩阵, 获得q前向的方向向量
		* @param mat 旋转矩阵
		*/
		osg::Vec3d getFront(const osg::Matrix& mat);

		/**
		* @brief getScale 根据距离获得尺度
		*/
		double getScale();

		/**
		* @brief getScaleFromDist 根据距离获得尺度,适用于绘制半径
		*/
		virtual double getScaleFromDist();

		/**
		* @brief getScreen2WorldMat 获得屏幕投到世界坐标系的矩阵
		*/
		virtual osg::Matrix getScreen2WorldMat();

		/**
		* @brief changePosition 位置变换函数
		* @param x y, 相对于相机移动的向前，向后，向左向右的方向的移动距离
		*/
		virtual void changePosition(double dx, double dy);

		/**
		* @brief panToXY 位置移动(x, y)
		* @param x y, 把中心移动到（x, y)位置，高度保持不变
		*/
		virtual void panToXY(double x, double y);

		/**
		* @brief zoomTo 位置缩放到和二维视图同样比例
		* @param scale 与二维视图同样比例
		*/
		virtual void zoomTo(double scale);

		/**
		* @brief shiftOperation 切换操作
		*/
		virtual bool shiftOperation();
	protected:
		/**
		* @brief handle 
		*/
		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

		/**
		* @brief performMovementLeftMouseButton 进行鼠标左击操作, 旋转时候必须按下shift键才可进行旋转
		*/
		virtual bool performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy);

		/**
		* @brief handleKeyDown 取消按键
		*/
		virtual bool handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		* @brief handleKeyUp 进行漫游
		*/
		virtual bool handleKeyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		* @brief handleMouseWheel 重写处理鼠标滚轮，把放大缩小滚轮的方向改下
		*/	
		virtual bool handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		* @brief performMovementMiddleMouseButton 重写中键移动鼠标按钮,取消左右键中键移动, 修改移动过程中上下震动的问题
		*/
		virtual bool performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy);

		/**
		* @brief performMovementRightMouseButton 重写右键缩放按钮，取消缩放
		*/
		virtual bool performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy);

		/**
		* @brief handleMouseRelease 重写中键移动鼠标按钮,取消了释放后还在移动
		*/
		virtual bool handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		* @brief handleMousePush 按键鼠标按下
		*/
		virtual bool handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		* @brief performMovement 调整
		*/
		virtual bool performMovement();

		

		/**
		* @brief subZoomModel 缩放模型
		*/
		virtual void subZoomModel(const float dy, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us, bool pushForwardIfNeeded = true);

		/**
		* @brief subSetCenterByMousePointerIntersection 重写原来的根据鼠标设置中心点，通过屏幕中心的碰撞点，设置中心
		*/
		virtual bool subSetCenterByMousePointerIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

	protected:
		/**
		* @brief mCamera 场景相机
		*/
		osg::ref_ptr<osg::Camera> mCamera;

		/**
		* @brief stViewPara 相机的视角参数 eye center up
		*/
		std::deque<stViewPara> vecStViewPara;

		/**
		* @brief mRoot 场景根节点
		*/
		osg::ref_ptr<osg::Group> mRoot;

		/**
		* @brief bMidButDown 鼠标中键按下
		*/
		bool bMidButDown;

		/**
		* @brief bShitRotate 是否使用shift进行旋转
		*/
		bool bShitRotate;

		/**
		* @brief iCall 回调类
		*/
		std::shared_ptr<osgCall::ICallBack> iCall;

	};
}


