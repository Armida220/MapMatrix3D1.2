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
	* @brief CThreadSafeUpdateCallback ͨ�����»ص���ȷ���̰߳�ȫ�ضԳ����е����ݵĸ���
	*/
	class CThreadSafeUpdateCallback : public osg::NodeCallback
	{
	public:
		CThreadSafeUpdateCallback(void);
		virtual ~CThreadSafeUpdateCallback(void);

	public:
		/**
		* @brief operator  �ڵ����
		* @param node      ���ڽ��и��µĳ����ڵ�
		* @param nv        �ڵ������
		*/
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

	public:
		/**
		* @brief AddChild  ��ӽڵ�
		* @param parent    ���ڵ�
		* @param node      ��Ҫ���ӵĽڵ�
		*/
		bool AddChild(osg::Node* parent, osg::Node* node);

		/**
		* @brief RemoveChild  ����ڵ�ڵ�
		* @param parent    ���ڵ�
		* @param node	   ��Ҫɾ���Ľڵ�
		*/
		bool RemoveChild(osg::Node* parent, osg::Node* node);

		/**
		* @brief setHomeCenter  �̰߳�ȫ�����ûؼҵ����ĵ�λ�ã�
		* @param terrain    ��������ٿ���
		* @param osgbModel    osgb������Ⱦ����ģ��
		*/
		bool setHomeCenter(osg::ref_ptr<osgGA::TerrainManipulator> terrain,
			COsgModel* osgbModel);

		/**
		* @brief home  �̰߳�ȫ�Ľ��лؼҲ���
		* @param terrain    ��������ٿ���
		*/
		bool home(osg::ref_ptr<osgGA::TerrainManipulator> terrain);

		/**
		* @brief removeAllChild  �̰߳�ȫ������������нڵ�
		* @param group ��ڵ�
		*/
		bool removeAllChild(osg::ref_ptr<osg::Group> group);

		/**
		* @brief driveToHighLight  �̰߳�ȫ���ƶ��ӿڵ�����������
		* @param highGroup �����ڵ�
		*/
		bool driveToHighLight(osg::ref_ptr<osgViewer::Viewer> viewer,
			osg::ref_ptr<osg::Group> highGroup);

		/**
		* @brief driveToXy  �̰߳�ȫ���ƶ��ӿڵ�����������
		* @param viewer �����
		* @param x y ����
		*/
		bool driveToXy(osg::ref_ptr<osgViewer::Viewer> viewer,
			double x, double y);

		/**
		* @brief panTo  �̰߳�ȫ���ƶ��ӿڲ��Ҳ���
		* @param terrain �ٿ���
		* @param x y ����
		*/
		bool panTo(osg::ref_ptr<CSelfDefTerrainManipulator> terrain,
			double x, double y);

		/**
		* @brief zoomTo  �̰߳�ȫ�����ų���
		* @param terrain �ٿ���
		* @param scale ���ŵ��ĳ߶�
		*/
		bool zoomTo(osg::ref_ptr<CSelfDefTerrainManipulator> terrain,
			double scale);

		/**
		* @brief isNeedToWait �ж϶����Ƿ�æ�������æ�����ȴ�
		*/
		bool isNeedToWait();

	protected:
		typedef CThreadSafeQueue<osg::ref_ptr<NodeOperator>>* QUEUE_NODE;

		/**
		* @brief m_tasks ����������������
		*/
		QUEUE_NODE m_tasks;

	};

}


