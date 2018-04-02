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
	//初始化坐标文字
	Text* posText = new Text;
	Text* posText1 = new Text;
	Text* posText2 = new Text;
	Text* posText3 = new Text;
	std::string caiyun("fonts/simhei.ttf");//此处设置的是汉字字体
	posText->setFont(caiyun);
	posText1->setFont(caiyun);
	posText2->setFont(caiyun);
	posText3->setFont(caiyun);
	//设置文字显示的位置
	Vec3 position(10.0f, 50.0f, 0.0f);
	posText->setPosition(position);
	position.y() -= 12;
	posText1->setPosition(position);
	position.y() -= 12;
	posText2->setPosition(position);
	position.y() -= 12;
	posText3->setPosition(position);

	posText->setCharacterSize(8.0);//字体大小
	posText1->setCharacterSize(8.0);//字体大小
	posText2->setCharacterSize(8.0);//字体大小
	posText3->setCharacterSize(8.0);//字体大小
	posText->setColor(Vec4(1, 1, 1, 1));
	posText1->setColor(Vec4(1, 1, 1, 1));
	posText2->setColor(Vec4(1, 1, 1, 1));
	posText3->setColor(Vec4(1, 1, 1, 1));
	posText->setDataVariance(Object::DYNAMIC);
	posText1->setDataVariance(Object::DYNAMIC);
	posText2->setDataVariance(Object::DYNAMIC);
	posText3->setDataVariance(Object::DYNAMIC);

	posText->setText("Row:0 Col:0");//设置显示的文字
	posText1->setText("[X]:0.0");
	posText2->setText("[Y]:0.0");
	posText3->setText("[Z]:0.0");
	Geode* textGeode = new Geode();
	textGeode->addDrawable(posText);
	textGeode->addDrawable(posText1);
	textGeode->addDrawable(posText2);
	textGeode->addDrawable(posText3);
	//设置状态
	StateSet* stateset = textGeode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, StateAttribute::OFF);//关闭灯光
	stateset->setMode(GL_DEPTH_TEST, StateAttribute::OFF);//关闭深度测试
	//打开GL_BLEND混合模式（以保证Alpha纹理正确）
	stateset->setMode(GL_BLEND, StateAttribute::ON);

	Camera* textCamera = new Camera;
	//设置透视矩阵
	textCamera->setProjectionMatrix(Matrix::ortho2D(0, 600, 0, 600));//正交投影   
	//设置绝对参考坐标系，确保视图矩阵不会被上级节点的变换矩阵影响
	textCamera->setReferenceFrame(Transform::ABSOLUTE_RF);
	//视图矩阵为默认的
	textCamera->setViewMatrix(Matrix::identity());
	//设置背景为透明，否则的话可以设置ClearColor 
	textCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
	textCamera->setAllowEventFocus(false);//不响应事件，始终得不到焦点
	//设置渲染顺序，必须在最后渲染
	textCamera->setRenderOrder(Camera::RenderOrder::POST_RENDER);

	ref_ptr<CThreadSafeUpdateCallback> spCallback = dynamic_cast<CThreadSafeUpdateCallback*>(mRoot->getUpdateCallback());

	if (spCallback != NULL)
	{
		mViewerGroup->addChild(textCamera);
		textCamera->addChild(textGeode);
	}

}