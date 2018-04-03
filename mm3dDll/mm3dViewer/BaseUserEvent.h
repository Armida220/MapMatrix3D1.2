
#pragma once
#include <osgViewer/viewer>
#include <osgGA/GUIEventHandler>
#include <osg/Vec3d>
#include <osg/Node>
#include <osg/Matrix>
#include <mutex>

namespace osgEvent
{
	/**
	 * @brief 是所有客户事件的基类，用于相应用户的gui事件，比如鼠标、按键等消息
	 */
	class CBaseUserEvent : public osgGA::GUIEventHandler
	{
	public:
		/**
		 * @brief 构造函数
		 * @param viewer 浏览器
		 */
		CBaseUserEvent(osgViewer::Viewer* viewer)
		{
			//禁用裁剪小细节
			osg::CullStack::CullingMode cullingMode = viewer->getCamera()->getCullingMode();
			cullingMode &= ~(osg::CullStack::SMALL_FEATURE_CULLING);
			viewer->getCamera()->setCullingMode(cullingMode);
		}

		/**
		 * @brief IsKeyPress 是否按下键
		 * @param ea  gui消息，存储按键、鼠标等消息
		 */
		virtual bool IsKeyPress(const osgGA::GUIEventAdapter &ea) const;

		/**
		 * @brief IsKeyUp 是否松开按键
		 * @param ea  gui消息，存储按键、鼠标等消息
		 */
		virtual bool IsKeyUp(const osgGA::GUIEventAdapter &ea) const;

		/**
		 * @brief isLeftClick 是否左击
		 * @param ea  gui消息，存储按键、鼠标等消息
		 */
		virtual bool isLeftClick(const osgGA::GUIEventAdapter &ea) const;

		/**
		 * @brief IsDoubleClick 是否双击
		 * @param ea  gui消息，存储按键、鼠标等消息
		 */
		virtual bool IsDoubleClick(const osgGA::GUIEventAdapter &ea) const;

		/**
		 * @brief IsRightClick 是否右击
		 * @param ea  gui消息，存储按键、鼠标等消息
		 */
		virtual bool IsRightClick(const osgGA::GUIEventAdapter &ea) const;

		/**
		* @brief IsMiddleClick 是否中键
		* @param ea  gui消息，存储按键、鼠标等消息
		*/
		virtual bool IsMiddleClick(const osgGA::GUIEventAdapter &ea) const;

		/**
		 * @brief isMouseMove 是否鼠标移动
		 * @param ea  gui消息，存储按键、鼠标等消息
		 */
		virtual bool isMouseMove(const osgGA::GUIEventAdapter &ea) const;

		/**
		* @brief isDrag 是否鼠标拖动
		* @param ea  gui消息，存储按键、鼠标等消息
		*/
		virtual bool isDrag(const osgGA::GUIEventAdapter &ea) const;

		/**
		* @brief isBkSpace 是否按下回退键
		* @param ea gui消息，存储按键、鼠标等消息
		*/
		virtual bool isBkSpace(const osgGA::GUIEventAdapter &ea) const;

		/**
		* @brief isDel 是否按下删除键
		* @param ea gui消息，存储按键、鼠标等消息
		*/
		virtual bool isDel(const osgGA::GUIEventAdapter &ea) const;

		/**
		* @brief isCtrlZ 是否按下ctrl + z键
		* @param ea gui消息，存储按键、鼠标等消息
		*/
		virtual bool isCtrlZ(const osgGA::GUIEventAdapter &ea) const;
	
		/**
		* @brief isPush 是否鼠标按键
		* @param ea  gui消息，存储按键、鼠标等消息
		*/
		virtual bool isPush(const osgGA::GUIEventAdapter &ea) const;

		/**
		* @brief isRelease 是否鼠标按键释放
		* @param ea  gui消息，存储按键、鼠标等消息
		*/
		virtual bool isRelease(const osgGA::GUIEventAdapter &ea) const;

		/**
		* @brief isReleaseLClk 是否鼠标释放左击
		* @param ea  gui消息，存储按键、鼠标等消息
		*/
		bool isReleaseLClk(const osgGA::GUIEventAdapter &ea) const;

		/**
		* @brief isReleaseLClk 是否鼠标释放左击
		* @param ea  gui消息，存储按键、鼠标等消息
		*/
		virtual bool isReleaseRClk(const osgGA::GUIEventAdapter &ea) const;

		/**
		* @brief clearGroupResult 清除组结果, 非线程安全的清除
		* @param group 需要清理的group
		*/
		virtual void clearGroupResult(osg::ref_ptr<osg::Group> group) const;
		/**
		 * @brief clearGroupResult 清除组结果, 经过修改后改为线程安全的清除
		 * @param group 需要清理的group
		 * @param root  根节点
		 */
		virtual void clearGroupResult(osg::ref_ptr<osg::Group> group, osg::Group* root) const;

		/**
		 * @brief CalLength 计算长度
		 * @param coord 一组三维点
		 */
		virtual double CalLength(const std::vector<osg::Vec3d> coord) const;

		/**
		 * @brief CalculateDist 计算距离
		 * @param pt1 第一个三维点
		 * @param pt2 第二个三维点
		 */
		virtual double CalculateDist(const osg::Vec3d pt1, const osg::Vec3d pt2) const;

		/**
		 * @brief CalculateArea 计算面积
		 * @param coord 一组三维点
		 */
		virtual double CalculateArea(std::vector<osg::Vec3d> coord) const;

		/**
		* @brief lazyRefresh 进行惰性刷新
		*/
		bool lazyRefresh() const;

		/**
		* @brief setManager 设置管理者
		*/
		inline void setManager(CBaseUserEvent* userEvent) { _manager = userEvent; }

		/**
		* @brief getManager 获得管理者
		*/
		inline CBaseUserEvent* getManager() { return _manager; }


	protected:
		osg::ref_ptr<CBaseUserEvent> _manager;
	};
}
