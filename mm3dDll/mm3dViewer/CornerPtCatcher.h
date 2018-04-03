#pragma once
#include <osg/Vec3d>
#include <vector>
#include "EventHandler.h"

namespace osgEvent
{
	/**
	* @brief �ڽǵ㲶׽��
	*/
	class CCornerPtCatcher : public CEventHandler
	{
	public:
		CCornerPtCatcher();
		virtual ~CCornerPtCatcher();

		/**
		* @brief lfClk ����¼�
		* @param clkPt ���е�
		*/
		virtual void lfClk(osg::Vec3d &clkPt);

		/**
		* @brief mouseMv ����ƶ��¼�
		* @param clkPt ���е�
		*/
		virtual void mouseMv(osg::Vec3d &clkPt);

		/**
		* @brief dbClk ˫���¼�
		* @param clkPt ���е�
		*/
		virtual void dbClk(const osg::Vec3d &clkPt);

		/**
		* @brief rightClk �һ��¼�
		* @param clkPt ���е�
		*/
		virtual bool rightClk(const osg::Vec3d &clkPt);

		/**
		* @brief keyDown �����¼�
		* @param ea �¼�
		*/
		virtual bool keyDown(const osgGA::GUIEventAdapter &ea);

		/**
		* @brief shift �¼��л�
		*/
		virtual bool shift();

		/**
		* @brief pop �ѵ�����ջһ��
		*/
		virtual void pop();

		virtual void setLastConstDragLine(osg::Vec3d pt1, osg::Vec3d pt2);
		virtual void clearConstDragLine();

	protected:
		/**
		* @brief replace �滻ԭ���ĵ�����Ϊ�µĲ�׽��
		* @param worldPt ԭ���ĵ����
		*/
		virtual void catchPt(osg::Vec3d &clkPt);

		/**
		* @brief push �ѵ���㱣������
		* @param clkPt ԭ���ĵ����
		*/
		virtual void push(osg::Vec3d clkPt);

		/**
		* @brief clear �ѵ���������
		*/
		virtual void clear();

		/**
		* @brief getCatchStatus ��ȡ��׽״̬
		*/
		inline bool getCatchStatus()
		{
			return bIsCatch;
		}

		/**
		* @brief calCornerPt �����ڽǵ�
		* @param pt1  ������һ����
		* @param pt2  �����ڶ�����
		* @param pt3  ��ǰ��ײ�ĵ�
		*/
		osg::Vec3d calCornerPt(const osg::Vec3d pt1, const osg::Vec3d pt2, const osg::Vec3d pt3);

	protected:
		/**
		* @brief bIsCatch �Ƿ�׽��ǽ�ǵ�
		*/
		bool bIsCatch;

		/**
		* @brief vecClkPt �������ά�������
		*/
		std::vector<osg::Vec3d> vecClkPt;


	};



}
