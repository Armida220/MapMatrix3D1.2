#pragma once
#include <memory>
#include <vector>
#include <map>
#include <osg/Node>

namespace osgDraw
{
	class IBaseDrawer;
}

namespace osgCall
{
	class ICallBack;
	class osgFtr;
	struct Coord;
}

namespace mm3dView
{
	class COsgModel;
	/**
	* @brief COsgSyn �Ѷ�ά��ͼ��ʸ��ͬ������ά��ͼ��ͬ������
	*/
	class COsgSyn
	{
	public:
		COsgSyn(COsgModel* model);

		virtual ~COsgSyn();

		/**
		* @brief synFtr ����ά�����ݺ�vecview����ͬ��
		* @param IDrawer ����
		* @param vecFtr ʸ������������
		* @param bIsMap �Ƿ�ͬ������mapGroup
		*/
		virtual void synFtr(osg::Group * root, std::shared_ptr<osgDraw::IBaseDrawer> IDrawer, const std::vector<osgCall::osgFtr> &vecFtr,
			bool bIsMap = false);

		/**
		* @brief removeFtr �������
		* @param vecFtr ʸ������������
		* @param root ���ڵ�
		*/
		virtual void removeFtr(const std::vector<osgCall::osgFtr> &vecFtr, osg::ref_ptr<osg::Group> root);
		virtual void removeFtrByID(const std::vector<std::string> &vecID, osg::ref_ptr<osg::Group> root);

		/**
		* @brief synFtrVerts ͬ�������Ķ���, ��ѡ��ʱ�����������Ķ���
		* @param IDrawer ����
		* @param vecFtr ��������
		*/
		virtual void synFtrVerts(const std::shared_ptr<osgDraw::IBaseDrawer> &IDrawer, const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief synFtrArrow ͬ��������ͷ�� ��ѡ��ʱ�����ָ����������ı�ʶ
		* @param IDrawer ����
		* @param vecFtr ��������
		*/
		virtual void synFtrArrow(const std::shared_ptr<osgDraw::IBaseDrawer> &IDrawer, const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief synAuxGraph ͬ�����и�����ʶ������
		* @param IDrawer ����
		* @param vecFtr ��������
		*/
		virtual void synAuxGraph(const std::shared_ptr<osgDraw::IBaseDrawer> &IDrawer, const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief showAllFtr ��ʾ��������
		* @param vecFtr ��������
		* @param vecFtrNotIn ������֪�����㼯��
		*/
		virtual void showAllFtr(const std::vector<osgCall::osgFtr> &vecFtr, std::vector<osgCall::osgFtr>& vecFtrNotIn);

		/**
		* @brief hideAllFtr ��ʾ��������
		* @param vecFtr ��������
		*/
		virtual void hideAllFtr(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief showLocalFtr ��ʾ��������
		* @param vecFtr ��������
		*/
		virtual void showLocalFtr(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief showExternalFtr ͬ���ⲿ����
		* @param vecFtr ��������
		*/
		virtual void showExternalFtr(const std::vector<osgCall::osgFtr> &vecFtr);

	protected:
		/**
		* @brief drawBaseGeom ��������������
		* @param IDrawer ����
		* @param ftr ����
		*/
		osg::ref_ptr<osg::Node> drawBaseGeom(const std::shared_ptr<osgDraw::IBaseDrawer> &IDrawer, const osgCall::osgFtr &ftr);

		/**
		* @brief calAve ����һ�������ƽ��ֵ
		* @param coords ��������Ľṹ������
		*/
		virtual osgCall::Coord calAve(const std::vector<osgCall::Coord> & coords);

	private:
		/**
		* @brief osgbModel ������ģ������
		*/
		COsgModel* osgbModel;

		typedef std::map<std::string, osg::ref_ptr<osg::Group>> MAP_NODE;
		typedef std::map<std::string, osg::ref_ptr<osg::Group>>::iterator MAP_ITER;
		/**
		* @brief mapNode id�źͽ��ӳ�������
		*/
		MAP_NODE mapNode;
	};



}
