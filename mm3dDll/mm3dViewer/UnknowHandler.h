#pragma once
#include "EventHandler.h"

namespace osgEvent
{
	class COsgUnknowEvent;
	/**
	* @brief δ֪�¼�������
	*/
	class CUnknowHandler : public CEventHandler
	{
	public:
		CUnknowHandler(osg::Group *root, osg::Group* mapGroup, osg::Group* editGroup,
			osg::Group* tempGroup, COsgUnknowEvent* parent);
		virtual ~CUnknowHandler();

	protected:
		/**
		* @brief lfClk ���
		* @param clkPt ���ĵ����꣬�������߳�ʱ�򣬸߶��������߶ȣ�������ʱ���worldPtһ��
		*/
		virtual void lfClk(const osg::Vec3d &clkPt);

		/**
		* @brief mouseMv ����ƶ�
		* @param clkPt �ƶ�������꣬��������ϵ
		*/
		virtual void mouseMv(const osg::Vec3d &clkPt);

		/**
		* @brief dbClk ˫��
		* @param worldPt ˫��������꣬��������ϵ
		* @param worldMatrix ���嵽��������ϵ����
		*/
		virtual void dbClk(const osg::Vec3d &clkPt);

		/**
		* @brief rightClk �һ�
		* @param worldPt �ҵ�����꣬��������ϵ
		* @param worldMatrix ���嵽��������ϵ����
		*/
		virtual bool rightClk(const osg::Vec3d &clkPt);

	protected:
		/**
		* @brief vecWorldCoord �洢������ĵ��������Ϣ������������ϵ
		*/
		std::vector<osg::Vec3d> vecWorldCoord;

		/**
		* @brief mRoot �������ڵ�
		*/
		osg::Group* mRoot;

		/**
		* @brief mapGroup �������ͼ�ε���
		*/
		osg::Group* mMapGroup;

		/**
		* @brief mEditGroup �������ͼ�ε���
		*/
		osg::Group* mEditGroup;

		/**
		* @brief mTempGroup ������ʱ����
		*/
		osg::Group* mTempGroup;

		/**
		* @brief mParent ���ڵ�ָ��unKnowEvent
		*/
		COsgUnknowEvent* mParent;

	};

}


