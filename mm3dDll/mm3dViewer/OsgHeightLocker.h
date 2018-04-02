#pragma once
#include <memory>
#include <osgGA/GUIEventAdapter>
#include <osg/Vec3d>
#include "EventHandler.h"

namespace osgEvent
{
	class COsgPicker;

	/**
	* @brief �߶�������,���ڶԸ߳̽�������
	*/
	class COsgHeightLocker : public CEventHandler
	{
	public:
		COsgHeightLocker();
		virtual ~COsgHeightLocker();

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
		* @brief lock �����߳�����
		*/
		inline void lock()
		{
			bIsLockHeight = true;
		}

		/**
		* @brief lock ȡ���߳�����
		*/
		inline void unlock()
		{
			bIsLockHeight = false;
		}

		/**
		* @brief setHeight ���ø߶�����
		* @param h ��Ҫ�����ĸ߶�
		*/
		virtual void setHeight(double h);

		/**
		* @brief replace �滻ԭ���ĵ����ĸ߳�����Ϊ�����߳�����
		* @param clkPt ԭ���ĵ����
		*/
		virtual void replace(osg::Vec3d &clkPt);

		/**
		* @brief shift ����ǿ���״̬������йر�,���������л�״̬
		*/
		virtual bool shift();

		/**
		* @brief getLockStatus ��ȡ�����߳�״̬
		*/
		inline bool getLockStatus()
		{
			return bIsLockHeight;
		}

	protected:
		/**
		* @brief bIsLockHeight �Ƿ���ס�߳�
		*/
		bool bIsLockHeight;

		/**
		* @brief lockHeight ��������ά��߳�
		*/
		double lockHeight;



	};
}



