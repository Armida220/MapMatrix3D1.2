// MFC_OSG.cpp : implementation of the cOSG class
//
#include "stdafx.h"
#include "ThreadSafeUpdateCallback.h"
#include <osg/ComputeBoundsVisitor>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osgText/Text>
#include <osgViewer/GraphicsWindow>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include "MFC_OSG.h"
#include "qcomm.h"
#include "IOsgEvent.h"
#include "IBaseDrawer.h"
#include "EventManager.h"
#include "BaseUserEvent.h"
#include "OsgModel.h"
#include "windows.h"
#include <process.h>
#include "seldefViewer.h"
#include "SelfStateManipulator.h"	
#include <numeric>
#include "OsgSyn.h"
#include <osg/ComputeBoundsVisitor>
#include <osgDB/WriteFile>
#include "OsgPicker.h"
#include <osgViewer/Renderer>
#include "json.h"
#include "IOsgOrient.h"
#include "osgTransNodeFinder.h"
#include "Compass.h"
using namespace mm3dView;
using namespace osgDraw;
using namespace osgEvent;	
using namespace osgCall;
using namespace osgGA;
using namespace osgUtil;
using namespace osg;
using namespace std;
using namespace osgViewer;

cOSG::cOSG(HWND hWnd) :
	m_hWnd(hWnd)
{
	// create the viewer for this window
	mViewer = new CSelDefViewer();
	osgbModel = new COsgModel();
	syn = new COsgSyn(osgbModel);

}

cOSG::~cOSG()
{
    mViewer->setDone(true);
    Sleep(1000);
    mViewer->stopThreading();
}

double cOSG::getScale()
{
	return terrain->getScale();
}

double cOSG::getScaleFromDist()
{
	return terrain->getScaleFromDist();
}

double* cOSG::getScrren2WorldMat()
{
	Matrix mat = terrain->getScreen2WorldMat();
	return (double*) mat.ptr();
}

void cOSG::InitOSG(string modelname)
{
	// Store the name of the model to load

    m_ModelName = modelname;

    // Init different parts of OSG
	InitManipulators();
    InitSceneGraph();
    InitCameraConfig();
	
}

void cOSG::InitManipulators(void)
{
	ref_ptr<Camera> camera = mViewer->getCamera();
	ref_ptr<Group> root = osgbModel->getRoot();
    // create a trackball manipulator
	terrain = new CSelfDefTerrainManipulator(camera, icall);

    // create a Manipulator Switcher
    keyswitchManipulator = new KeySwitchMatrixManipulator;

    // Add our trackball manipulator to the switcher
	keyswitchManipulator->addMatrixManipulator( '1', "Terrain", terrain.get());

    // Init the switcher to the first manipulator (in this case the only manipulator)
    keyswitchManipulator->selectMatrixManipulator(0);  // Zero based index Value
}

void cOSG::InitSceneGraph(void)
{
    // Init the main Root Node/Group
	osgbModel->init(m_ModelName);
}


void cOSG::InitCameraConfig(void)
{
    // Local Variable to hold window size data
	RECT rect;

    // Add a Stats Handler to the viewer
    mViewer->addEventHandler(new osgViewer::StatsHandler);

    // Get the current window size
    ::GetClientRect(m_hWnd, &rect);
	POINT pt;
	pt.x = 0;
	pt.y = 0;
	::ClientToScreen(m_hWnd, &pt);
    // Init the GraphicsContext Traits

    ref_ptr<GraphicsContext::Traits> traits = new GraphicsContext::Traits;
    // Init the Windata Variable that holds the handle for the Window to display OSG in.
    ref_ptr<Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

	if (!windata)
	{
		return;
	}

    // Setup the traits parameters
    traits->x = 0;
    traits->y = 0;
    traits->width = rect.right - rect.left;
    traits->height = rect.bottom - rect.top;

    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->setInheritedWindowPixelFormat = true;
    traits->inheritedWindowData = windata;
    traits->useCursor = false;
    // create the Graphics Context
    GraphicsContext* gc = GraphicsContext::createGraphicsContext(traits.get());

    // Init Master Camera for this View
    ref_ptr<Camera> camera = mViewer->getCamera();

    // Assign Graphics Context to the Camera
    camera->setGraphicsContext(gc);

    // Set the viewport for the Camera
    camera->setViewport(new Viewport(rect.left, rect.top, traits->width, traits->height));

    // Set projection matrix and camera attribtues
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    /*camera->setClearColor(Vec4f(0.2f, 0.2f, 0.4f, 1.0f));*/
	camera->setClearColor(Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
    camera->setProjectionMatrixAsPerspective(
        30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0, 1000.0);

    // Add the Camera to the Viewer
    //mViewer->addSlave(camera.get());
    mViewer->setCamera(camera.get());

    // Add the Camera Manipulator to the Viewer
    mViewer->setCameraManipulator(keyswitchManipulator.get());

	Group* root = new Group;
	root->addChild(osgbModel->getRoot());
    // Set the Scene Data
	mViewer->setSceneData(root);
	osg::ref_ptr<osg::Viewport> vp = new osg::Viewport(0, 0, 100, 100);
	root->addChild(new Compass(vp.get()));
	initEvent();

	setHomeCetner();

	home();

	/*mViewer->addEventHandler(new osgViewer::WindowSizeHandler); */ 
	//mViewer->addEventHandler(new osgViewer::HelpHandler);
	//mViewer->addEventHandler(new osgViewer::ThreadingHandler);
	//mViewer->addEventHandler(new osgViewer::RecordCameraPathHandler);
	//mViewer->addEventHandler(new osgViewer::LODScaleHandler);
	//mViewer->addEventHandler(new osgViewer::ScreenCaptureHandler);

    // Realize the Viewer
    mViewer->realize();

	mViewer->setKeyEventSetsDone(0);
    // Correct aspect ratio
    /*double fovy,aspectRatio,z1,z2;
    mViewer->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
    aspectRatio=double(traits->width)/double(traits->height);
    mViewer->getCamera()->setProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);*/
}

void cOSG::setHomeCetner()
{
	Group* root = osgbModel->getRoot();
	ref_ptr<CThreadSafeUpdateCallback> mCallBack = dynamic_cast<CThreadSafeUpdateCallback*> (root->getUpdateCallback());

	if (mCallBack)
	{
		mCallBack->setHomeCenter(terrain, osgbModel);
	}
}

void cOSG::initEvent()
{
	curStateManipulator = new CSelfStateManipulator(mViewer->getCamera()->getOrCreateStateSet());
	mViewer->addEventHandler(curStateManipulator);

	pEvent = IOsgEventFactory::create(mViewer, osgbModel, icall);
	pEvent->addEventToViewer(mViewer);
}

void cOSG::home()
{
	Group* root = osgbModel->getRoot();
	ref_ptr<CThreadSafeUpdateCallback> mCallBack = dynamic_cast<CThreadSafeUpdateCallback*> (root->getUpdateCallback());

	if (mCallBack)
	{
		mCallBack->setHomeCenter(terrain, osgbModel);
	}

	mCallBack->home(terrain);
}

void cOSG::light()
{
	curStateManipulator->light();
}


void cOSG::setCall(const shared_ptr<ICallBack> &ic)
{
	icall = ic;
}

void cOSG::driveTo(const double x, const double y)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	ref_ptr<CThreadSafeUpdateCallback> mCallBack = dynamic_cast<CThreadSafeUpdateCallback*> (root->getUpdateCallback());

	if (mCallBack)
	{
		mCallBack->driveToXy(mViewer, x, y);
	}
}

void cOSG::panTo(const double x, const double y)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	ref_ptr<CThreadSafeUpdateCallback> mCallBack = dynamic_cast<CThreadSafeUpdateCallback*> (root->getUpdateCallback());

	if (mCallBack)
	{
		mCallBack->panTo(terrain, x, y);
	}
}

void cOSG::zoomTo(const double scale)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	ref_ptr<CThreadSafeUpdateCallback> mCallBack = dynamic_cast<CThreadSafeUpdateCallback*> (root->getUpdateCallback());

	if (mCallBack)
	{
		mCallBack->zoomTo(terrain, scale);
	}
}

bool cOSG::isLinePick(const double x, const double y)
{
	Vec3d pDown(x, y, -1000);
	Vec3d pUp(x, y, 5000);
	//进行垂直碰撞检测获得三维点
	//碰撞检测
	ref_ptr<LineSegmentIntersector> ls = new LineSegmentIntersector(pDown, pUp);
	// 创建一个IV
	IntersectionVisitor iv(ls);

	Group* root = osgbModel->getRoot();
	// 把线段添加到IV中
	root->accept(iv);

	//碰撞有效
	if (ls && ls.valid() && ls->containsIntersections())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void cOSG::addObj(const vector<osgFtr> &vecFtr)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	Group* mapGroup = osgbModel->getMapNode();

	Vec4d color(1.0, 0, 1.0, 1.0);
	double size = 3.5f;
	shared_ptr<osgDraw::IBaseDrawer> IMapDrawer = IBaseDrawerFactory::create(color, size, mapGroup, root);

	osgbModel->addFtrData(vecFtr);

	//同步特征
	syn->synFtr(root, IMapDrawer, vecFtr, true);
}

void cOSG::removeObj(const vector<osgFtr> &vecFtr)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	Group* mapGroup = osgbModel->getMapNode();

	osgbModel->removeFtrData(vecFtr);

	//清除特征
	syn->removeFtr(vecFtr, root);
}

void cOSG::synData(const vector<osgFtr> &vecFtr)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	Group* mapGroup = osgbModel->getMapNode();

	Vec4d color(1.0, 0, 1.0, 1.0);
	double size = 3.5f;
	shared_ptr<osgDraw::IBaseDrawer> IMapDrawer = IBaseDrawerFactory::create(color, size, mapGroup, root);

	//清除mapgrou
	threadSafeClear(mapGroup);

	osgbModel->setFtrData(vecFtr);

	//同步特征
	syn->synFtr(root, IMapDrawer, vecFtr, true);
}

void cOSG::updataDate(const std::vector<osgCall::osgFtr> &vecFtr)
{
// 	removeObj(vecFtr);
// 	addObj(vecFtr);
	
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	Group* mapGroup = osgbModel->getMapNode();

	Vec4d color(1.0, 0, 1.0, 1.0);
	double size = 3.5f;
	shared_ptr<osgDraw::IBaseDrawer> IMapDrawer = IBaseDrawerFactory::create(color, size, mapGroup, root);

	//同步特征
	syn->synFtr(root, IMapDrawer, vecFtr, true);
}


void cOSG::synHighLightData(const vector<osgFtr> &vecHighFtr, bool bIsOsgbAcitve)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	Group* highGroup = osgbModel->getHighLighNode();

	Vec4d color(0.0, 1.0, 1.0, 1.0);
	double size = 10.5f;
	shared_ptr<IBaseDrawer> IHighLineDrawer = IBaseDrawerFactory::create(color, size, highGroup, root);
	
	int numChild = highGroup->getNumChildren();

	//清除mapgrou
	threadSafeClear(highGroup);

	numChild = highGroup->getNumChildren();

	if (vecHighFtr.size() > 0)
	{
		//同步高亮特征
		syn->synFtr(root, IHighLineDrawer, vecHighFtr);

// 		Vec4d colorVert(1.0, 0.0, 1.0, 1.0);
// 		double sizeVert = 6.5f;
// 		IHighLineDrawer->resetColor(colorVert);
// 		IHighLineDrawer->resetSize(sizeVert);
// 		//绘制辅助顶点
// 		syn->synFtrVerts(IHighLineDrawer, vecHighFtr);
	}
}

void cOSG::clearHighLightData()
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	Group* highGroup = osgbModel->getHighLighNode();

	//清除highGroup
	threadSafeClear(highGroup);
}

void cOSG::addConstDragLine(const vector<osgFtr> &vecFtr)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}
	{
		if (vecFtr.size() > 0)
		{
			const osgFtr & _ftr = vecFtr.back();
			if (_ftr.vecCoord.size() >= 2)
			{
				int idx0 = _ftr.vecCoord.size() - 2;
				int idx1 = idx0 + 1;
				const Coord & pt0 = _ftr.vecCoord[idx0];
				const Coord & pt1 = _ftr.vecCoord[idx1];
				pEvent->setLastConstDragLine(
					osg::Vec3d(pt0.x, pt0.y, pt0.z),
					osg::Vec3d(pt1.x, pt1.y, pt1.z)
					);
			}
			else
				pEvent->clearConstDragLine();
		}
		else
		{
			pEvent->clearConstDragLine();
		}
	}
	Group* editGroup = osgbModel->getEditNode();

	Vec4d color(1.0, 0, 1.0, 1.0);
	double size = 3.5f;
	shared_ptr<IBaseDrawer> IEditDrawer = IBaseDrawerFactory::create(color, size, editGroup, root);
	Matrix mat;

	//同步特征
	syn->synFtr(root, IEditDrawer, vecFtr);

}

void cOSG::setConstDragLine(const vector<osgFtr> &vecFtr)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	{
		if (vecFtr.size() > 0)
		{
			const osgFtr & _ftr = vecFtr.back();
			if (_ftr.vecCoord.size() >= 2)
			{
				int idx0 = _ftr.vecCoord.size() - 2;
				int idx1 = idx0 + 1;
				const Coord & pt0 = _ftr.vecCoord[idx0];
				const Coord & pt1 = _ftr.vecCoord[idx1];
				pEvent->setLastConstDragLine(
					osg::Vec3d(pt0.x, pt0.y, pt0.z),
					osg::Vec3d(pt1.x, pt1.y, pt1.z)
					);
			}
			else
				pEvent->clearConstDragLine();
		}
		else
		{
			pEvent->clearConstDragLine();
		}
	}
	Group* editGroup = osgbModel->getEditNode();

	Vec4d color(1.0, 0, 1.0, 1.0);
	double size = 3.5f;
	shared_ptr<IBaseDrawer> IEditDrawer = IBaseDrawerFactory::create(color, size, editGroup, root);

	//清除threadSafeClear
	threadSafeClear(editGroup);

	if (!vecFtr.empty())
	{
		//同步特征
		syn->synFtr(root, IEditDrawer, vecFtr);
	}
}

void cOSG::setVariantDragLine(const vector<osgFtr> &vecFtr)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(root->getUpdateCallback());

	if (spChildCallback->isNeedToWait())
	{
		return;
	}

	Group* tempGroup = osgbModel->getTempNode();

	Vec4d color(1.0, 0, 1.0, 1.0);
	double size = 3.5f;
	shared_ptr<IBaseDrawer> ITempDrawer = IBaseDrawerFactory::create(color, size, tempGroup, root);

	//清除mapgrou
	threadSafeClear(tempGroup);

	if (!vecFtr.empty())
	{
		//同步特征
		syn->synFtr(root, ITempDrawer, vecFtr);
	}
}

void cOSG::addVariantDragLine(const vector<osgFtr> &vecFtr)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(root->getUpdateCallback());

	if (spChildCallback->isNeedToWait())
	{
		return;
	}

	Group* tempGroup = osgbModel->getTempNode();

	Vec4d color(1.0, 0, 1.0, 1.0);
	double size = 3.5f;
	shared_ptr<IBaseDrawer> ITempDrawer = IBaseDrawerFactory::create(color, size, tempGroup, root);

	//同步特征
	syn->synFtr(root, ITempDrawer, vecFtr);
}

void cOSG::UpdatesnapDraw(const vector<osgFtr> &vecFtr)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	Group* snapGroup = osgbModel->getSnapNode();

	Vec4d color(1.0, 1.0, 0.0, 1.0);
	double size = 3.5f;
	shared_ptr<IBaseDrawer> ISnapDrawer = IBaseDrawerFactory::create(color, size, snapGroup, root);

	//清除mapgrou
	threadSafeClear(snapGroup);

	if (!vecFtr.empty())
	{
		//同步捕捉盒子
		syn->synAuxGraph(ISnapDrawer, vecFtr);
	}
}

void cOSG::addGraph(const vector<osgFtr> &vecFtr)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	Group* tinGroup = osgbModel->getTinNode();

	Vec4d color(1.0, 1.0, 0.0, 1.0);
	double size = 3.5f;
	shared_ptr<IBaseDrawer> ITinDrawer = IBaseDrawerFactory::create(color, size, tinGroup, root);

	if (!vecFtr.empty())
	{
		//同步捕捉盒子
		syn->synAuxGraph(ITinDrawer, vecFtr);
	}
}


void cOSG::delGraph()
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	Group* tinGroup = osgbModel->getTinNode();

	threadSafeClear(tinGroup);
}

void cOSG::clearGroupResult(ref_ptr<Group> group) const
{
	int numChild = group->getNumChildren();

	for (int i = numChild - 1; i >= 0; i--)
	{
		ref_ptr<Node> node = group->getChild(i);
		group->removeChild(node);
	}
}

void cOSG::threadSafeClear(ref_ptr<Group> group) const
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(root->getUpdateCallback());

	if (spChildCallback != NULL)
	{
		spChildCallback->removeAllChild(group);
	}
}


vector<double> cOSG::linePickPt(const double x, const double y) const
{
	vector<double> pickPt;
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return pickPt;
	}

	//进行垂直碰撞检测
	Group* scene = root->getChild(0)->asGroup();
	
	if (!scene)
	{
		return pickPt;
	}

	//执行垂直碰撞检测
	shared_ptr<COsgPicker> picker(new COsgPicker(mViewer));
	vector<Vec3d> pickResult = picker->DefaultlinePickAll(x, y, scene);
	pickPt.reserve(pickResult.size() * 3);
	for (auto & worldPt : pickResult)
	{
		pickPt.push_back(worldPt.x());
		pickPt.push_back(worldPt.y());
		pickPt.push_back(worldPt.z());
	}
	

	return pickPt;
}

void cOSG::viewAll(const vector<osgCall::osgFtr> &vecFtr)
{
	vector<osgCall::osgFtr> vecFtrNotIn;
	syn->showAllFtr(vecFtr, vecFtrNotIn);
	if (!vecFtrNotIn.empty())
		addObj(vecFtrNotIn);
}

void cOSG::ViewHideAll(const vector<osgCall::osgFtr> &vecFtr)
{
	removeObj(vecFtr);
	//syn->hideAllFtr(vecFtr);
}

void cOSG::viewLocal(const vector<osgFtr> &vecFtr)
{
	syn->showLocalFtr(vecFtr);
}

void cOSG::viewExternal(const vector<osgFtr> &vecFtr)
{
	syn->showExternalFtr(vecFtr);
}

void cOSG::getOsgbExtent(double &left, double &right, double &top, double &bottom)
{
	Group* scence = osgbModel->getSceneNode();
	if (scence)
	{
		ComputeBoundsVisitor ive;
		scence->accept(ive);

		BoundingBox bb = ive.getBoundingBox();
		left = bb.xMin();
		right = bb.xMax();
		top = bb.yMax();
		bottom = bb.yMin();
	}
	else
	{
		left = right = top = bottom = 0;
	}
}

void cOSG::topView()
{
	terrain->topView();
}

void cOSG::leftView()
{
	terrain->leftView();
}

void cOSG::rightView()
{
	terrain->rightView();
}

bool cOSG::islineBlock(double x1, double y1, double z1,
	double x2, double y2, double z2)
{
	Vec3d start(x1, y1, z1);
	Vec3d end(x2, y2, z2);
	//进行垂直碰撞检测获得三维点
	//碰撞检测
	ref_ptr<LineSegmentIntersector> ls = new LineSegmentIntersector(start, end);
	// 创建一个IV
	IntersectionVisitor iv(ls);

	Group* root = osgbModel->getRoot();
	// 把线段添加到IV中
	root->accept(iv);

	//碰撞有效
	if (ls && ls.valid() && ls->containsIntersections())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool cOSG::drawVaryPt(double x, double y, double z)
{
	Group* varGroup = osgbModel->getVarNode();
	Group* root = osgbModel->getRoot();

	Vec4d color(1.0, 0, 1.0, 1.0);
	double size = 3.5f;
	Matrix worldMat;
	worldMat.setTrans(x, y, z);
	shared_ptr<IBaseDrawer> IVaryDrawer = IBaseDrawerFactory::create(color, size, varGroup, root);
	clearGroupResult(varGroup);
	IVaryDrawer->drawPt(Vec3d(x, y, z), worldMat);

	return true;
}

void cOSG::clearVaryGroup()
{
	Group* varGroup = osgbModel->getVarNode();
	clearGroupResult(varGroup);
}

void cOSG::Render(void* ptr)
{
    cOSG* osg = (cOSG*)ptr;
    osgViewer::Viewer* viewer = osg->getViewer();

    // You have two options for the main viewer loop
    //      viewer->run()   or
    //      while(!viewer->done()) { viewer->frame(); }

	
    viewer->run();
     //while(!viewer->done())
     //{
     //    osg->PreFrameUpdate();
     //    viewer->frame();
     //    osg->PostFrameUpdate();
     //   /* Sleep(1000);*/         // Use this command if you need to allow other processes to have cpu time
     //}

    // For some reason this has to be here to avoid issue:
    // if you have multiple OSG windows up
    // and you exit one then all stop rendering
    //AfxMessageBox("Exit Rendering Thread");

    _endthread();
}

void cOSG::SetSceneExtent()
{
	Group* root = osgbModel->getRoot();
	//设置范围
	ref_ptr<KeySwitchMatrixManipulator> keyswitchManipulator = dynamic_cast<KeySwitchMatrixManipulator*> (mViewer->getCameraManipulator());
	ref_ptr<TerrainManipulator> terrainManipulator = dynamic_cast<TerrainManipulator*> (keyswitchManipulator->getMatrixManipulatorWithIndex(0));
	terrainManipulator->setNode(root);
}

HANDLE cOSG::StartRendering()
{
	HANDLE mThreadHandle = (HANDLE)_beginthread(&Render, 0, this);
	return mThreadHandle;
}

void cOSG::addScene(const string &strModel) 
{
	ref_ptr<Node> tempNodeStart = osgDB::readNodeFile(strModel);
	osgbModel->AddChild(tempNodeStart);
	SetSceneExtent();

	//重新设置回家点
	setHomeCetner();

	//回家操作
	home();
}

void cOSG::removeAllScene()
{
	osgbModel->RemoveAllChild();
}

void cOSG::startEvent(int order)
{
	for (osgViewer::View::EventHandlers::iterator hitr = mViewer->getEventHandlers().begin(); hitr != mViewer->getEventHandlers().end(); ++hitr)
	{
		EventHandler* pEvent = *hitr;

		ref_ptr<CSwitchEventManager> ph = dynamic_cast<CSwitchEventManager*> (pEvent);

		if (!ph)
		{
			continue;
		}
		

		ph->selectEventByNum(order);
	}
}

string cOSG::absOri(string oriInfo)
{
	Group* sceneNode = osgbModel->getSceneNode();
	IOsgOrient* iOsgOrient = IOsgOrientFactory::create();
	string res = iOsgOrient->absOrient(sceneNode, oriInfo);

	Group* editNode = osgbModel->getEditNode();
	threadSafeClear(editNode);

	fetchOriData();

	home();

	return res;

}

void cOSG::driveToRelaPt(double relaX, double relaY, double relaZ)
{
	Group* sceneGroup = osgbModel->getSceneNode();

	vector<MatrixTransform*> vecTrans;
	COsgTransNodeFinder ive;
	ive.findTransNode(sceneGroup, vecTrans);

	if (vecTrans.size() > 0)
	{
		Matrix worldMat = vecTrans[0]->getMatrix();
		Vec3d pt(relaX, relaY, relaZ);
		Vec3d absPt = pt * worldMat;
	
		driveTo(absPt.x(), absPt.y());
	}

}

void cOSG::fetchOriData()
{
	for (osgViewer::View::EventHandlers::iterator hitr = mViewer->getEventHandlers().begin(); hitr != mViewer->getEventHandlers().end(); ++hitr)
	{
		EventHandler* pEvent = *hitr;

		ref_ptr<CSwitchEventManager> ph = dynamic_cast<CSwitchEventManager*> (pEvent);

		if (!ph)
		{
			continue;
		}


		ph->fetchOriData();
	}
}

void cOSG::showOriPt(string oriRes)
{
	for (osgViewer::View::EventHandlers::iterator hitr = mViewer->getEventHandlers().begin(); hitr != mViewer->getEventHandlers().end(); ++hitr)
	{
		EventHandler* pEvent = *hitr;

		ref_ptr<CSwitchEventManager> ph = dynamic_cast<CSwitchEventManager*> (pEvent);

		if (!ph)
		{
			continue;
		}


		ph->showOriPt(oriRes);
	}
}

string cOSG::predictOriPt(string oriRes)
{
	Group* sceneNode = osgbModel->getSceneNode();
	IOsgOrient* iOsgOrient = IOsgOrientFactory::create();
	string res = iOsgOrient->predict(oriRes);

	return res;
}

void cOSG::clearOriPt()
{
	for (osgViewer::View::EventHandlers::iterator hitr = mViewer->getEventHandlers().begin(); hitr != mViewer->getEventHandlers().end(); ++hitr)
	{
		EventHandler* pEvent = *hitr;

		ref_ptr<CSwitchEventManager> ph = dynamic_cast<CSwitchEventManager*> (pEvent);

		if (!ph)
		{
			continue;
		}


		ph->clearOriPt();
	}
}

void cOSG::clearDragLine()
{
	Group* tempGroup = osgbModel->getTempNode();
	Group* editGroup = osgbModel->getEditNode();
	pEvent->clearConstDragLine();
	threadSafeClear(tempGroup);
	threadSafeClear(editGroup);
}

void cOSG::setCursor(double x, double y, double z)
{
	ref_ptr<Renderer> render = dynamic_cast<Renderer*> (mViewer->getCamera()->getRenderer());
	ref_ptr<SceneView> sceneView = render->getSceneView(0);

	Vec3 winXY;
	sceneView->projectObjectIntoWindow(Vec3d(x, y, z), winXY);
}


bool mm3dView::cOSG::shiftOperation()
{
	pEvent->shiftOperation();
	terrain->shiftOperation();
	return true;
}

bool mm3dView::cOSG::shiftCatchCorner()
{
	pEvent->shiftCatchCorner();
	return true;
}

bool mm3dView::cOSG::lockHeight(int x, int y)
{
	pEvent->shiftHeightLock(x, y);
	return true;
}

bool mm3dView::cOSG::updataFtrView(bool bShow)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return true;
	}

	Group* mapGroup = osgbModel->getMapNode();
	mapGroup->setNodeMask(bShow ? 1 : 0);
	return true;
}

void mm3dView::cOSG::removeObj(const std::vector<std::string> &vecID)
{
	Group* root = osgbModel->getRoot();

	if (!root)
	{
		return;
	}

	Group* mapGroup = osgbModel->getMapNode();
	//清除特征
	syn->removeFtrByID(vecID, root);
}

void mm3dView::cOSG::groundToScreen(double x, double y, double z, double & _x, double &_y)
{
	ref_ptr<Renderer> render = dynamic_cast<Renderer*> (mViewer->getCamera()->getRenderer());
	ref_ptr<SceneView> sceneView = render->getSceneView(0);

	Vec3 winXY;
	sceneView->projectObjectIntoWindow(Vec3d(x, y, z), winXY);
	_x = winXY.x();
	_y = winXY.y();
}

bool mm3dView::cOSG::screenToGround(double x, double y, double & _x, double &_y, double &_z)
{
	ref_ptr<Renderer> render = dynamic_cast<Renderer*> (mViewer->getCamera()->getRenderer());
	ref_ptr<SceneView> sceneView = render->getSceneView(0);
	Vec3 ObjXYZ;
	if (sceneView->projectWindowIntoObject(Vec3(x, y, 0), ObjXYZ))
	{
		_x = ObjXYZ.x();
		_y = ObjXYZ.y();
		_z = ObjXYZ.z();
		return true;
	}
	return false;
}

CRenderingThread::CRenderingThread(cOSG* ptr)
:   OpenThreads::Thread(), _ptr(ptr), _done(false)
{
}

CRenderingThread::~CRenderingThread()
{
    _done = true;
    if (isRunning())
    {
        cancel();
        join();
    }
}

void CRenderingThread::run()
{
    if ( !_ptr )
    {
        _done = true;
        return;
    }

    osgViewer::Viewer* viewer = _ptr->getViewer();
    do
    {
        viewer->frame();

    } while ( !testCancel() && !viewer->done() && !_done );
}

