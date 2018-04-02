#pragma once
#include <osg/Group>

namespace osgCall
{
	struct osgFtr;
}

namespace mm3dView
{
	/**
	* @brief COsgModel ����ģ��
	*/
	class COsgModel
	{
	public:
		COsgModel();
		virtual ~COsgModel();

		/**
		* @brief init ��ʼ��
		* @param modelName �ļ���
		*/
		void init(std::string modelName);

		/**
		* @brief getRoot ��ø��ڵ�
		*/
		osg::Group* getRoot();

		/**
		* @brief getTempNode �����ʱ�ڵ�
		*/
		osg::Group* getTempNode();

		/**
		* @brief getMapNode ��ò�ͼ�ڵ�
		*/
		osg::Group* getMapNode();

		/**
		* @brief getEditNode ��ø��ڵ�
		*/
		osg::Group* getEditNode();

		/**
		* @brief getSceneNode ��ó����ڵ�
		*/
		osg::Group* getSceneNode();

		/**
		* @brief getSnapNode ��ò�׽�ڵ�
		*/
		osg::Group* getSnapNode();

		/**
		* @brief getHighLighNode ��ø����ڵ�
		*/
		osg::Group* getHighLighNode();

		/**
		* @brief getTinNode ���tin�ڵ�
		*/
		osg::Group* getTinNode();

		/**
		* @brief getCursorNode ���cursor�ڵ�
		*/
		osg::Group* getCursorNode();

		/**
		* @brief getVarNode ���var�ڵ�
		*/
		osg::Group* getVarNode();

		/**
		* @brief calBox ���㷶Χ��
		*/
		osg::BoundingBox calBox();

		/**
		* @brief addFtrData ������������
		* @param vecFtr ��Ҫ������������
		*/
		void addFtrData(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief removeFtrData �����������
		* @param vecFtr ��Ҫ�������������
		*/
		void removeFtrData(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief setFtrData ������������
		* @param vecFtr ��������
		*/
		void setFtrData(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief getFtrData ��ȡ��������
		*/
		std::vector<osgCall::osgFtr> getFtrData();

		/**
		* @brief AddChild ��ó����ڵ�
		* @param node �ڵ�
		*/
		void AddChild(osg::Node* node);

		/**
		* @brief RemoveChild �Ƴ������ڵ�
		* @param node �ڵ�
		*/
		void RemoveChild(osg::Node* node);

		/**
		* @brief RemoveAllChild �Ƴ����г����ڵ�
		*/
		void RemoveAllChild();

	protected:
		/**
		* @brief InitialViewerGroup ��ʼ��viewerGroup
		* @param mViewerGroup ��ͼ��
		*/
		void InitialViewerGroup(osg::ref_ptr<osg::Group> mViewerGroup);

		/**
		* @brief threadSafeAddChild �̰߳�ȫ���ӳ����ڵ�
		* @param parent ���ڵ�
		* @param node �ڵ�
		*/
		void threadSafeAddChild(osg::Node* parent, osg::Node* node);

		/**
		* @brief threadSafeRemoveChild �̰߳�ȫ�Ƴ������ڵ�
		* @param parent ���ڵ�
		* @param node �ڵ�
		*/
		void threadSafeRemoveChild(osg::Node* parent, osg::Node* node);

		/**
		* @brief threadSafeRemoveAllChild �̰߳�ȫ���Ƴ��������нڵ�
		*/
		void threadSafeRemoveAllChild();

	private:
		/**
		* @brief mRoot �����ڵ�
		*/
		osg::ref_ptr<osg::Group> mRoot;

		/**
		* @brief mModel ��Ⱦģ�ͽڵ�
		*/
		osg::ref_ptr<osg::Node> mModel;

		/**
		* @brief mSceneGroup ���泡�����ݽڵ���
		*/
		osg::ref_ptr<osg::Group> mSceneGroup;

		/**
		* @brief mEditSceneGroup ���ڱ���constant����
		*/
		osg::ref_ptr<osg::Group> mEditSceneGroup;

		/**
		* @brief mMapperGroup �����ͼ���
		*/
		osg::ref_ptr<osg::Group> mMapperGroup;

		/**
		* @brief mTempGroup ��ʱ��, ���ڱ���variant����
		*/
		osg::ref_ptr<osg::Group> mTempGroup;

		/**
		* @brief mSnapBoxGroup ��ʱ��, ���ڲ�׽ʱ�����ڱ�ʶ��׽���˵Ļƿ�
		*/
		osg::ref_ptr<osg::Group> mSnapBoxGroup;

		/**
		* @brief mHighLightGroup ������, ���������Ϣ
		*/
		osg::ref_ptr<osg::Group> mHighLightGroup;

		/**
		* @brief mTinGroup tin��
		*/
		osg::ref_ptr<osg::Group> mTinGroup;

		/**
		* @brief mCurGroup cursor��
		*/
		osg::ref_ptr<osg::Group> mCurGroup;

		/**
		* @brief mVarGroup �仯��
		*/
		osg::ref_ptr<osg::Group> mVarGroup;

		/**
		* @brief vecFtr ��������
		*/
		std::vector<osgCall::osgFtr> mVecFtr;

	};
}

