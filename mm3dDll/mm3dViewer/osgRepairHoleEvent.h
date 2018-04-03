#pragma once

#include "BaseUserEvent.h"
#include <osgViewer/Viewer>
#include <osg/Group>
#include "osg/MatrixTransform"
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <memory>

namespace osgDraw
{
	class IBaseDrawer;
}

namespace osgEvent
{
	class COsgPicker;

	class CTimeKeeper;

	/**
	* @brief 修补空洞事件
	*/
	class COsgRepairHoleEvent : public CBaseUserEvent
	{
	public:
		COsgRepairHoleEvent(osgViewer::Viewer *viewer, osg::Group* constantGroup, osg::Group* variantGroup, osg::Group* root);

	protected:
		virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

		/**
		* @brief startRepairHole 进行修补空洞
		*/
		virtual bool startRepairHole();

		/**
		* @brief ObtainTransNodeFromNodePath 从nodePath获取trans节点
		* @brief nodePath 节点路径
		* @brief mTrans 返回矩阵
		*/
		virtual bool obtainTransNodeFromNodePath(osg::NodePath nodePath, osg::ref_ptr<osg::MatrixTransform> &mTrans);

		/**
		* @brief LfClkDw 画闭合线
		* @brief worldMatrix 局部到世界矩阵
		*/
		virtual void lfClkDw(osg::Matrix worldMatrix);

		/**
		* @brief mouseMvDw 移动画闭合线
		* @brief worldMatrix 局部到世界矩阵
		* @brief worldPt 鼠标移动的碰撞点
		*/
		virtual void mouseMvDw(osg::Matrix worldMatrix, osg::Vec3d worldPt);

		/**
		* @brief bkSpackDw 回退画闭合线
		*/
		virtual void bkSpackDw();

		/**
		* @brief revertDw 撤销画闭合线
		*/
		virtual void revertDw();

		/**
		* @brief pushButton 按下按钮
		*/
		virtual void pushButton(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief releaseButton 释放按键
		* @param ea gui事件
		*/
		virtual void releaseButton(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief mouseMove 鼠标移动
		* @param ea gui事件
		*/
		virtual void mouseMove(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief keyDown 按键
		* @param ea gui事件
		*/
		virtual void keyDown(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief TCHAR2STRING tchar 转string
		* @param STR 需要转的TCHAR
		*/
		std::string TCHAR2STRING(TCHAR *STR);

	protected:
		/**
		* @brief 绘制点数组
		*/
		std::vector<osg::Vec3d> vecWorldCoord;

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
		* @brief 场景的trans节点
		*/
		osg::ref_ptr<osg::MatrixTransform> mTrans;

		/**
		* @brief 子节点
		*/
		std::vector<osg::ref_ptr<osg::Node>> vecChildNode;

		/**
		* @brief 父节点
		*/
		std::vector<osg::ref_ptr<osg::Group>> vecParentNode;

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
	};

}
