#pragma once
#include "osg/NodeCallback"
#include "osg/Group"
#include <map>
#include "ThreadSafeQueue.h"
#include <osgViewer/Viewer>

namespace osgGA
{
	class TerrainManipulator;
}

namespace mm3dView
{
	class NodeOperator;
	class COsgModel;
	class CSelfDefTerrainManipulator;
	/**
	* @brief CThreadSafeUpdateCallback 通过更新回调，确保线程安全地对场景中的数据的更改
	*/
	class CThreadSafeUpdateCallback : public osg::NodeCallback
	{
	public:
		CThreadSafeUpdateCallback(void);
		virtual ~CThreadSafeUpdateCallback(void);

	public:
		/**
		* @brief operator  节点更新
		* @param node      正在进行更新的场景节点
		* @param nv        节点遍历器
		*/
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

	public:
		/**
		* @brief AddChild  添加节点
		* @param parent    父节点
		* @param node      需要增加的节点
		*/
		bool AddChild(osg::Node* parent, osg::Node* node);

		/**
		* @brief RemoveChild  清除节点节点
		* @param parent    父节点
		* @param node	   需要删除的节点
		*/
		bool RemoveChild(osg::Node* parent, osg::Node* node);

		/**
		* @brief setHomeCenter  线程安全的设置回家的中心点位置，
		* @param terrain    地形浏览操控器
		* @param osgbModel    osgb场景渲染数据模型
		*/
		bool setHomeCenter(osg::ref_ptr<osgGA::TerrainManipulator> terrain,
			COsgModel* osgbModel);

		/**
		* @brief home  线程安全的进行回家操作
		* @param terrain    地形浏览操控器
		*/
		bool home(osg::ref_ptr<osgGA::TerrainManipulator> terrain);

		/**
		* @brief removeAllChild  线程安全的清除场景所有节点
		* @param group 组节点
		*/
		bool removeAllChild(osg::ref_ptr<osg::Group> group);

		/**
		* @brief driveToHighLight  线程安全的移动视口到高亮组中心
		* @param highGroup 高亮节点
		*/
		bool driveToHighLight(osg::ref_ptr<osgViewer::Viewer> viewer,
			osg::ref_ptr<osg::Group> highGroup);

		/**
		* @brief driveToXy  线程安全的移动视口到高亮组中心
		* @param viewer 浏览器
		* @param x y 中心
		*/
		bool driveToXy(osg::ref_ptr<osgViewer::Viewer> viewer,
			double x, double y);

		/**
		* @brief panTo  线程安全的移动视口并且不会
		* @param terrain 操控器
		* @param x y 中心
		*/
		bool panTo(osg::ref_ptr<CSelfDefTerrainManipulator> terrain,
			double x, double y);

		/**
		* @brief zoomTo  线程安全的缩放场景
		* @param terrain 操控器
		* @param scale 缩放到的尺度
		*/
		bool zoomTo(osg::ref_ptr<CSelfDefTerrainManipulator> terrain,
			double scale);

		/**
		* @brief isNeedToWait 判断队列是否繁忙，如果繁忙跳过等待
		*/
		bool isNeedToWait();

	protected:
		typedef CThreadSafeQueue<osg::ref_ptr<NodeOperator>>* QUEUE_NODE;

		/**
		* @brief m_tasks 操作任务所在容器
		*/
		QUEUE_NODE m_tasks;

	};

}


