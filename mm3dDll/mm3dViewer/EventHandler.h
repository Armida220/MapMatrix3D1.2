#pragma once
#include <osgViewer/Viewer>
#include <osg/Node>
#include <osg/Group>
#include <osg/Matrix>



namespace osgEvent
{
	/*
	* @brief CEventHandler �¼������࣬���࣬��Ҫ��д���нӿ�
	*/
	class CEventHandler
		{
	public:
		CEventHandler();
		virtual ~CEventHandler();

		/**
		* @brief lfClk ����¼�
		* @param clkPt ���е�
		*/
		virtual void lfClk(osg::Vec3d &clkPt) = 0;

		/**
		* @brief mouseMv ����ƶ��¼�
		* @param clkPt ���е�
		*/
		virtual void mouseMv(osg::Vec3d &clkPt) = 0;

		/**
		* @brief dbClk ˫���¼�
		* @param clkPt ���е�
		*/
		virtual void dbClk(const osg::Vec3d &clkPt) = 0;

		/**
		* @brief rightClk �һ��¼�
		* @param clkPt ���е�
		*/
		virtual bool rightClk(const osg::Vec3d &clkPt) = 0;

		/**
		* @brief keyDown �����¼�
		* @param ea �¼�
		*/
		virtual bool keyDown(const osgGA::GUIEventAdapter &ea) = 0;

		/**
		* @brief start �л��¼�, �������յ��л�״̬
		*/
		virtual bool shift() = 0;

		/**
		* @brief clearGroupResult �������
		* @param group ��Ҫ�������
		* @param root  ��ȡ���»ص�����
		*/
		void clearGroupResult(osg::ref_ptr<osg::Group> group, osg::Group* root) const;
	};
}

