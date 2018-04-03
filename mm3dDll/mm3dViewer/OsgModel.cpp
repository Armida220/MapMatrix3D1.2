#include "stdafx.h"
#include "OsgModel.h"
#include "ThreadSafeUpdateCallback.h"
#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osgText/Text>
#include <vector>
#include "ICallBack.h"
#include <osg/ComputeBoundsVisitor>
using namespace std;
using namespace osg;
using namespace osgUtil;
using namespace osgText;
using namespace mm3dView;
using namespace osgCall;

COsgModel::COsgModel()
{
	
}


COsgModel::~COsgModel()
{
}

void COsgModel::init(string modelName)
{
	mRoot = new Group;
	mRoot->addUpdateCallback(new CThreadSafeUpdateCallback());

	mSceneGroup = new Group;
	mSceneGroup->setName("sceneGroup");
	mEditSceneGroup = new Group;
	mEditSceneGroup->setName("editGroup");
	mMapperGroup = new Group;
	mMapperGroup->setName("mapperGroup");
	mTempGroup = new Group;
	mTempGroup->setName("tempGroup");
	mSnapBoxGroup = new Group;
	mSnapBoxGroup->setName("snapBoxGroup");
	mHighLightGroup = new Group;
	mHighLightGroup->setName("highLightGroup");
	mTinGroup = new Group;
	mTinGroup->setName("tinGroup");
	mCurGroup = new Group;
	mCurGroup->setName("curGroup");
	mVarGroup = new Group;
	mVarGroup->setName("varGroup");

	// Load the Model from the model name	

	mModel = osgDB::readNodeFile(modelName);

	if (!mModel)
	{
		return;
	}

	mModel->setName("model1");

	if (!mModel)
	{
		return;
	}

	// Optimize the model
	Optimizer optimizer;
	optimizer.optimize(mSceneGroup.get());
	optimizer.reset();

	ref_ptr<CThreadSafeUpdateCallback> spCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());
	// Add the model to the scene
	if (spCallback != NULL)
	{
		mRoot->addChild(mSceneGroup.get());
		mRoot->addChild(mMapperGroup.get());
		mRoot->addChild(mEditSceneGroup.get());
		mRoot->addChild(mTempGroup.get());
		mRoot->addChild(mSnapBoxGroup.get());
		mRoot->addChild(mHighLightGroup.get());
		mRoot->addChild(mTinGroup.get());
		mRoot->addChild(mCurGroup.get());
		mRoot->addChild(mVarGroup.get());
	}

	if (spCallback != NULL)
	{
		mSceneGroup->addChild(mModel.get());
	}
}

Group* COsgModel::getRoot()
{
	return mRoot;
}

Group* COsgModel::getTempNode()
{
	return mTempGroup;
}

Group* COsgModel::getMapNode()
{
	return mMapperGroup;
}

Group* COsgModel::getEditNode()
{
	return mEditSceneGroup;
}

Group* COsgModel::getSceneNode()
{
	return mSceneGroup;
}

Group* COsgModel::getSnapNode()
{
	return mSnapBoxGroup;
}

Group* COsgModel::getHighLighNode()
{
	return mHighLightGroup;
}

osg::Group* COsgModel::getTinNode()
{
	return mTinGroup;
}

osg::Group* COsgModel::getCursorNode()
{
	return mCurGroup;
}

osg::Group* COsgModel::getVarNode()
{
	return mVarGroup;
}

BoundingBox COsgModel::calBox()
{
	ref_ptr<Node> node = mSceneGroup->asGroup()->getChild(0);

	if (!node)
	{
		return BoundingBox();
	}

	ComputeBoundsVisitor cbbv;
	node->accept(cbbv);
	BoundingBox bb = cbbv.getBoundingBox();
	return bb;
}

void COsgModel::addFtrData(const vector<osgFtr> &vecFtr)
{
	//mVecFtr.insert(mVecFtr.end(), vecFtr.begin(), vecFtr.end());
}

void COsgModel::removeFtrData(const vector<osgFtr> &vecFtr)
{
// 	for (auto ftr : vecFtr)
// 	{
// 		auto iter = find(mVecFtr.begin(), mVecFtr.end(), ftr);
// 
// 		while (iter != mVecFtr.end())
// 		{
// 			mVecFtr.erase(iter);
// 			iter = find(mVecFtr.begin(), mVecFtr.end(), ftr);
// 		}
// 	}
}

void COsgModel::setFtrData(const vector<osgFtr> &vecFtr)
{
//	mVecFtr = vecFtr;
}

vector<osgFtr> COsgModel::getFtrData()
{
	return mVecFtr;
}

void COsgModel::AddChild(Node* node)
{
	ref_ptr<Node> group = mRoot->getChild(0);
	threadSafeAddChild(group, node);
}

void COsgModel::RemoveChild(Node* node)
{
	ref_ptr<Node> group = mRoot->getChild(0);
	threadSafeRemoveChild(group, node);
}

void COsgModel::RemoveAllChild()
{
	threadSafeRemoveAllChild();
}

void COsgModel::threadSafeAddChild(Node* parent, Node* node)
{
	ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());

	if (spChildCallback != NULL)
	{
		spChildCallback->AddChild(parent, node);
	}
}

void COsgModel::threadSafeRemoveChild(Node* parent, Node* node)
{
	ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());

	if (spChildCallback != NULL)
	{
		spChildCallback->RemoveChild(parent, node);
	}
}

void COsgModel::threadSafeRemoveAllChild()
{
	ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());

	if (spChildCallback != nullptr)
	{
		for (int i = 0; i < mRoot->getNumChildren(); i++)
		{
			ref_ptr<Group> group = mRoot->getChild(i)->asGroup();

			if (group)
			{
				spChildCallback->removeAllChild(group);
			}
		}
	}
}

void COsgModel::InitialViewerGroup(ref_ptr<Group> mViewerGroup)
{
	//��ʼ����������
	Text* posText = new Text;
	Text* posText1 = new Text;
	Text* posText2 = new Text;
	Text* posText3 = new Text;
	std::string caiyun("fonts/simhei.ttf");//�˴����õ��Ǻ�������
	posText->setFont(caiyun);
	posText1->setFont(caiyun);
	posText2->setFont(caiyun);
	posText3->setFont(caiyun);
	//����������ʾ��λ��
	Vec3 position(10.0f, 50.0f, 0.0f);
	posText->setPosition(position);
	position.y() -= 12;
	posText1->setPosition(position);
	position.y() -= 12;
	posText2->setPosition(position);
	position.y() -= 12;
	posText3->setPosition(position);

	posText->setCharacterSize(8.0);//�����С
	posText1->setCharacterSize(8.0);//�����С
	posText2->setCharacterSize(8.0);//�����С
	posText3->setCharacterSize(8.0);//�����С
	posText->setColor(Vec4(1, 1, 1, 1));
	posText1->setColor(Vec4(1, 1, 1, 1));
	posText2->setColor(Vec4(1, 1, 1, 1));
	posText3->setColor(Vec4(1, 1, 1, 1));
	posText->setDataVariance(Object::DYNAMIC);
	posText1->setDataVariance(Object::DYNAMIC);
	posText2->setDataVariance(Object::DYNAMIC);
	posText3->setDataVariance(Object::DYNAMIC);

	posText->setText("Row:0 Col:0");//������ʾ������
	posText1->setText("[X]:0.0");
	posText2->setText("[Y]:0.0");
	posText3->setText("[Z]:0.0");
	Geode* textGeode = new Geode();
	textGeode->addDrawable(posText);
	textGeode->addDrawable(posText1);
	textGeode->addDrawable(posText2);
	textGeode->addDrawable(posText3);
	//����״̬
	StateSet* stateset = textGeode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);//�رյƹ�
	stateset->setMode(GL_DEPTH_TEST, StateAttribute::OFF);//�ر���Ȳ���
	//��GL_BLEND���ģʽ���Ա�֤Alpha������ȷ��
	stateset->setMode(GL_BLEND, StateAttribute::ON);

	Camera* textCamera = new Camera;
	//����͸�Ӿ���
	textCamera->setProjectionMatrix(Matrix::ortho2D(0, 600, 0, 600));//����ͶӰ   
	//���þ��Բο�����ϵ��ȷ����ͼ���󲻻ᱻ�ϼ��ڵ�ı任����Ӱ��
	textCamera->setReferenceFrame(Transform::ABSOLUTE_RF);
	//��ͼ����ΪĬ�ϵ�
	textCamera->setViewMatrix(Matrix::identity());
	//���ñ���Ϊ͸��������Ļ���������ClearColor 
	textCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
	textCamera->setAllowEventFocus(false);//����Ӧ�¼���ʼ�յò�������
	//������Ⱦ˳�򣬱����������Ⱦ
	textCamera->setRenderOrder(Camera::RenderOrder::POST_RENDER);

	ref_ptr<CThreadSafeUpdateCallback> spCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());

	if (spCallback != NULL)
	{
		mViewerGroup->addChild(textCamera);
		textCamera->addChild(textGeode);
	}

}