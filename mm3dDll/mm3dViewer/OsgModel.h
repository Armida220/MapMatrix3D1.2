#pragma once
#include <osg/Group>

namespace osgCall
{
	struct osgFtr;
}

namespace mm3dView
{
	/**
	* @brief COsgModel 场景模型
	*/
	class COsgModel
	{
	public:
		COsgModel();
		virtual ~COsgModel();

		/**
		* @brief init 初始化
		* @param modelName 文件名
		*/
		void init(std::string modelName);

		/**
		* @brief getRoot 获得根节点
		*/
		osg::Group* getRoot();

		/**
		* @brief getTempNode 获得临时节点
		*/
		osg::Group* getTempNode();

		/**
		* @brief getMapNode 获得测图节点
		*/
		osg::Group* getMapNode();

		/**
		* @brief getEditNode 获得根节点
		*/
		osg::Group* getEditNode();

		/**
		* @brief getSceneNode 获得场景节点
		*/
		osg::Group* getSceneNode();

		/**
		* @brief getSnapNode 获得捕捉节点
		*/
		osg::Group* getSnapNode();

		/**
		* @brief getHighLighNode 获得高亮节点
		*/
		osg::Group* getHighLighNode();

		/**
		* @brief getTinNode 获得tin节点
		*/
		osg::Group* getTinNode();

		/**
		* @brief getCursorNode 获得cursor节点
		*/
		osg::Group* getCursorNode();

		/**
		* @brief getVarNode 获得var节点
		*/
		osg::Group* getVarNode();

		/**
		* @brief calBox 计算范围盒
		*/
		osg::BoundingBox calBox();

		/**
		* @brief addFtrData 增加特征数据
		* @param vecFtr 需要增加特征集合
		*/
		void addFtrData(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief removeFtrData 清除特征数据
		* @param vecFtr 需要清除的特征集合
		*/
		void removeFtrData(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief setFtrData 设置特征数据
		* @param vecFtr 特征集合
		*/
		void setFtrData(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief getFtrData 获取特征数据
		*/
		std::vector<osgCall::osgFtr> getFtrData();

		/**
		* @brief AddChild 获得场景节点
		* @param node 节点
		*/
		void AddChild(osg::Node* node);

		/**
		* @brief RemoveChild 移除场景节点
		* @param node 节点
		*/
		void RemoveChild(osg::Node* node);

		/**
		* @brief RemoveAllChild 移除所有场景节点
		*/
		void RemoveAllChild();

	protected:
		/**
		* @brief InitialViewerGroup 初始化viewerGroup
		* @param mViewerGroup 视图组
		*/
		void InitialViewerGroup(osg::ref_ptr<osg::Group> mViewerGroup);

		/**
		* @brief threadSafeAddChild 线程安全增加场景节点
		* @param parent 父节点
		* @param node 节点
		*/
		void threadSafeAddChild(osg::Node* parent, osg::Node* node);

		/**
		* @brief threadSafeRemoveChild 线程安全移除场景节点
		* @param parent 父节点
		* @param node 节点
		*/
		void threadSafeRemoveChild(osg::Node* parent, osg::Node* node);

		/**
		* @brief threadSafeRemoveAllChild 线程安全地移除场景所有节点
		*/
		void threadSafeRemoveAllChild();

	private:
		/**
		* @brief mRoot 场景节点
		*/
		osg::ref_ptr<osg::Group> mRoot;

		/**
		* @brief mModel 渲染模型节点
		*/
		osg::ref_ptr<osg::Node> mModel;

		/**
		* @brief mSceneGroup 保存场景数据节点组
		*/
		osg::ref_ptr<osg::Group> mSceneGroup;

		/**
		* @brief mEditSceneGroup 用于保存constant数据
		*/
		osg::ref_ptr<osg::Group> mEditSceneGroup;

		/**
		* @brief mMapperGroup 保存测图结果
		*/
		osg::ref_ptr<osg::Group> mMapperGroup;

		/**
		* @brief mTempGroup 临时组, 用于保存variant数据
		*/
		osg::ref_ptr<osg::Group> mTempGroup;

		/**
		* @brief mSnapBoxGroup 临时组, 用于捕捉时候用于标识捕捉上了的黄框
		*/
		osg::ref_ptr<osg::Group> mSnapBoxGroup;

		/**
		* @brief mHighLightGroup 高亮组, 保存高亮信息
		*/
		osg::ref_ptr<osg::Group> mHighLightGroup;

		/**
		* @brief mTinGroup tin组
		*/
		osg::ref_ptr<osg::Group> mTinGroup;

		/**
		* @brief mCurGroup cursor组
		*/
		osg::ref_ptr<osg::Group> mCurGroup;

		/**
		* @brief mVarGroup 变化组
		*/
		osg::ref_ptr<osg::Group> mVarGroup;

		/**
		* @brief vecFtr 特征集合
		*/
		std::vector<osgCall::osgFtr> mVecFtr;

	};
}

