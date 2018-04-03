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
	* @brief COsgSyn 把二维视图的矢量同步到三维视图的同步器类
	*/
	class COsgSyn
	{
	public:
		COsgSyn(COsgModel* model);

		virtual ~COsgSyn();

		/**
		* @brief synFtr 对三维的数据和vecview进行同步
		* @param IDrawer 画笔
		* @param vecFtr 矢量特征的容器
		* @param bIsMap 是否同步的是mapGroup
		*/
		virtual void synFtr(osg::Group * root, std::shared_ptr<osgDraw::IBaseDrawer> IDrawer, const std::vector<osgCall::osgFtr> &vecFtr,
			bool bIsMap = false);

		/**
		* @brief removeFtr 清除特征
		* @param vecFtr 矢量特征的容器
		* @param root 根节点
		*/
		virtual void removeFtr(const std::vector<osgCall::osgFtr> &vecFtr, osg::ref_ptr<osg::Group> root);
		virtual void removeFtrByID(const std::vector<std::string> &vecID, osg::ref_ptr<osg::Group> root);

		/**
		* @brief synFtrVerts 同步特征的顶点, 在选中时候会出现特征的顶点
		* @param IDrawer 画笔
		* @param vecFtr 特征集合
		*/
		virtual void synFtrVerts(const std::shared_ptr<osgDraw::IBaseDrawer> &IDrawer, const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief synFtrArrow 同步特征箭头， 在选中时候出现指向特征方向的标识
		* @param IDrawer 画笔
		* @param vecFtr 特征集合
		*/
		virtual void synFtrArrow(const std::shared_ptr<osgDraw::IBaseDrawer> &IDrawer, const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief synAuxGraph 同步进行辅助标识的增加
		* @param IDrawer 画笔
		* @param vecFtr 特征集合
		*/
		virtual void synAuxGraph(const std::shared_ptr<osgDraw::IBaseDrawer> &IDrawer, const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief showAllFtr 显示所有特征
		* @param vecFtr 特征集合
		* @param vecFtrNotIn 不在已知特征点集合
		*/
		virtual void showAllFtr(const std::vector<osgCall::osgFtr> &vecFtr, std::vector<osgCall::osgFtr>& vecFtrNotIn);

		/**
		* @brief hideAllFtr 显示所有特征
		* @param vecFtr 特征集合
		*/
		virtual void hideAllFtr(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief showLocalFtr 显示本地特征
		* @param vecFtr 特征集合
		*/
		virtual void showLocalFtr(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief showExternalFtr 同步外部特征
		* @param vecFtr 特征集合
		*/
		virtual void showExternalFtr(const std::vector<osgCall::osgFtr> &vecFtr);

	protected:
		/**
		* @brief drawBaseGeom 画基本几何特征
		* @param IDrawer 画笔
		* @param ftr 特征
		*/
		osg::ref_ptr<osg::Node> drawBaseGeom(const std::shared_ptr<osgDraw::IBaseDrawer> &IDrawer, const osgCall::osgFtr &ftr);

		/**
		* @brief calAve 计算一组坐标的平均值
		* @param coords 保存坐标的结构体容器
		*/
		virtual osgCall::Coord calAve(const std::vector<osgCall::Coord> & coords);

	private:
		/**
		* @brief osgbModel 场景的模型数据
		*/
		COsgModel* osgbModel;

		typedef std::map<std::string, osg::ref_ptr<osg::Group>> MAP_NODE;
		typedef std::map<std::string, osg::ref_ptr<osg::Group>>::iterator MAP_ITER;
		/**
		* @brief mapNode id号和结点映射的容器
		*/
		MAP_NODE mapNode;
	};



}
