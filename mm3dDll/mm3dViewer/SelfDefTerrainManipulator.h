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
	* @brief stViewPara �ӽǲ���
	*/
	struct stViewPara
	{
		stViewPara(osg::Vec3d eye, osg::Vec3d center, osg::Vec3d up)
			: mEye(eye), mCenter(center), mUp(up)
		{
		}

		/**
		* @brief mEye �ӵ�λ��
		*/
		osg::Vec3d mEye;
		
		/**
		* @brief mCenter ���λ��
		*/
		osg::Vec3d mCenter;

		/**
		* @brief mUp �������
		*/
		osg::Vec3d mUp;
	};

	/**
	* @brief CSelfDefTerrainManipulator �û��Զ���ٿ���
	*/
	class CSelfDefTerrainManipulator : public osgGA::TerrainManipulator
	{
	public:
		CSelfDefTerrainManipulator(osg::ref_ptr<osg::Camera> camera, std::shared_ptr<osgCall::ICallBack> pCall);

		/**
		* @brief topView ����ͼ
		*/
		virtual void topView();

		/**
		* @brief leftView ����ͼ
		*/
		virtual void leftView();

		/**
		* @brief rightView ����ͼ
		*/
		virtual void rightView();

		/**
		* @brief getRotation �����ת��Ϣ
		*/
		inline osg::Quat getRotation()
		{
			return _rotation;
		}

		/**
		* @brief getSide ������ת����, �������ķ�������
		* @param mat ��ת����
		*/
		osg::Vec3d getSide(const osg::Matrix& mat);

		/**
		* @brief getFront ������ת����, ���qǰ��ķ�������
		* @param mat ��ת����
		*/
		osg::Vec3d getFront(const osg::Matrix& mat);

		/**
		* @brief getScale ���ݾ����ó߶�
		*/
		double getScale();

		/**
		* @brief getScaleFromDist ���ݾ����ó߶�,�����ڻ��ư뾶
		*/
		virtual double getScaleFromDist();

		/**
		* @brief getScreen2WorldMat �����ĻͶ����������ϵ�ľ���
		*/
		virtual osg::Matrix getScreen2WorldMat();

		/**
		* @brief changePosition λ�ñ任����
		* @param x y, ���������ƶ�����ǰ������������ҵķ�����ƶ�����
		*/
		virtual void changePosition(double dx, double dy);

		/**
		* @brief panToXY λ���ƶ�(x, y)
		* @param x y, �������ƶ�����x, y)λ�ã��߶ȱ��ֲ���
		*/
		virtual void panToXY(double x, double y);

		/**
		* @brief zoomTo λ�����ŵ��Ͷ�ά��ͼͬ������
		* @param scale ���ά��ͼͬ������
		*/
		virtual void zoomTo(double scale);

		/**
		* @brief shiftOperation �л�����
		*/
		virtual bool shiftOperation();
	protected:
		/**
		* @brief handle 
		*/
		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

		/**
		* @brief performMovementLeftMouseButton ��������������, ��תʱ����밴��shift���ſɽ�����ת
		*/
		virtual bool performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy);

		/**
		* @brief handleKeyDown ȡ������
		*/
		virtual bool handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		* @brief handleKeyUp ��������
		*/
		virtual bool handleKeyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		* @brief handleMouseWheel ��д���������֣��ѷŴ���С���ֵķ������
		*/	
		virtual bool handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		* @brief performMovementMiddleMouseButton ��д�м��ƶ���갴ť,ȡ�����Ҽ��м��ƶ�, �޸��ƶ������������𶯵�����
		*/
		virtual bool performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy);

		/**
		* @brief performMovementRightMouseButton ��д�Ҽ����Ű�ť��ȡ������
		*/
		virtual bool performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy);

		/**
		* @brief handleMouseRelease ��д�м��ƶ���갴ť,ȡ�����ͷź����ƶ�
		*/
		virtual bool handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		* @brief handleMousePush ������갴��
		*/
		virtual bool handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

		/**
		* @brief performMovement ����
		*/
		virtual bool performMovement();

		

		/**
		* @brief subZoomModel ����ģ��
		*/
		virtual void subZoomModel(const float dy, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us, bool pushForwardIfNeeded = true);

		/**
		* @brief subSetCenterByMousePointerIntersection ��дԭ���ĸ�������������ĵ㣬ͨ����Ļ���ĵ���ײ�㣬��������
		*/
		virtual bool subSetCenterByMousePointerIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

	protected:
		/**
		* @brief mCamera �������
		*/
		osg::ref_ptr<osg::Camera> mCamera;

		/**
		* @brief stViewPara ������ӽǲ��� eye center up
		*/
		std::deque<stViewPara> vecStViewPara;

		/**
		* @brief mRoot �������ڵ�
		*/
		osg::ref_ptr<osg::Group> mRoot;

		/**
		* @brief bMidButDown ����м�����
		*/
		bool bMidButDown;

		/**
		* @brief bShitRotate �Ƿ�ʹ��shift������ת
		*/
		bool bShitRotate;

		/**
		* @brief iCall �ص���
		*/
		std::shared_ptr<osgCall::ICallBack> iCall;

	};
}


