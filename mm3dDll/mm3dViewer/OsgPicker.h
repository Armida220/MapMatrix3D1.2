#pragma once
#include <osg/Vec3d>
#include <osg/Node>
#include <osg/Matrix>
#include <osgViewer/Viewer>

namespace osgEvent
{
	/**
	* @brief ��ײ����ȡ�Ľ��
	*/
	class CPickResult
	{
	public:
		/**
		* @brief ԭʼ��
		*/
		osg::Vec3d localPt;

		/**
		* @brief ���������
		*/
		osg::Vec3d worldPt;

		/**
		* @brief ������
		*/
		osg::Vec3d worldNm;

		/**
		* @brief �ڼ���ģ�ͱ�ѡ��
		*/
		int modelIndex;

		/**
		* @brief ��ײ�нڵ����·��
		*/
		osg::NodePath nodePath;

		/**
		* @brief ��Ͳ��ĸ��ӽڵ�
		*/
		osg::ref_ptr<osg::Node> parent;

		/**
		* @brief ����ת������ľ���
		*/
		osg::Matrix matrix;
	};

	/**
	* @brief ��ײ�����
	*/
	class COsgPicker
	{
	public:
		COsgPicker(osgViewer::Viewer *view);
		virtual ~COsgPicker();

		/**
		* @brief pickResult ��ײ���
		* @param x ������ӿ��ϵ�x����
		* @param y ������ӿ��ϵ�y����
		*/
		virtual const CPickResult pickResult(const float x, const float y) const;

		/**
		* @brief linePick ��ײ���
		* @param pUp ���϶˵�
		* @param pDown ����Ķ˵�
		* @param mView �����
		*/
		virtual const CPickResult linePick(const osg::Vec3d &pUp, const osg::Vec3d &pDown, osg::Group *scene) const;

		/**
		* @brief DefaultlinePick Ĭ�ϵĴ�ֱ��ײ���
		* @param x y ƽ���xy����
		* @param mRoot ��ײ�Ľڵ�
		*/
		const CPickResult DefaultlinePick(double x, double y, osg::Group *mRoot) const;
		
		std::vector<osg::Vec3d> DefaultlinePickAll(double x, double y, osg::Group *mRoot) const;

	protected:
		osg::ref_ptr<osgViewer::Viewer> mView;
	};

}

