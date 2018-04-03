#include "stdafx.h"
#include "OsgUnknowEvent.h"
#include "UnknowHandler.h"
#include "ICallBack.h"
#include "qcomm.h"
#include "SelfDefTerrainManipulator.h"
#include "OsgHeightLocker.h"
#include "CornerPtCatcher.h"
#include "OsgPicker.h"
#include "Prompter.h"
#include "TimeKeeper.h"
#include "IBaseDrawer.h"
#include <memory>
#include "OsgModel.h"


using namespace osgEvent;		
using namespace osgViewer;
using namespace osg;
using namespace osgCall;
using namespace std;
using namespace osgGA;
using namespace chrono;
using namespace std::chrono;
using namespace mm3dView;
using namespace osgDraw;

//����ƶ������̶�
const int torelance = 10;

COsgUnknowEvent::COsgUnknowEvent(Viewer *view, COsgModel* osgModel,
	shared_ptr<ICallBack> ic)
	: CBaseUserEvent(view),
	mView(view),
	mModel(osgModel),
	iCall(ic),
	mouseEventType(0),
	mouseXY(0, 0),
	pOsgPicker(new COsgPicker(view)),
	pPrompter(new CPrompter(ic)),
	timeKeeper(new CTimeKeeper),
	locker(new COsgHeightLocker),
	catcher(new CCornerPtCatcher)
{


	vecHandler.push_back(locker);
	vecHandler.push_back(catcher);
}


COsgUnknowEvent::~COsgUnknowEvent()
{
}

void COsgUnknowEvent::addHandler(shared_ptr<CEventHandler> pHandler)
{
	vector<shared_ptr<CEventHandler>>::iterator itr = std::find(vecHandler.begin(), vecHandler.end(), pHandler);

	if (itr == vecHandler.end())
	{
		vecHandler.push_back(pHandler);
	}
}

void COsgUnknowEvent::removeHandler(shared_ptr<CEventHandler> pHandler)
{
	vector<shared_ptr<CEventHandler>>::iterator itr = std::find(vecHandler.begin(), vecHandler.end(), pHandler);

	if (itr != vecHandler.end())
	{
		vecHandler.erase(itr);
	}
}

bool COsgUnknowEvent::handle(const GUIEventAdapter &ea, GUIActionAdapter &aa)
{
	if (isPush(ea))
	{
		pushButton(ea);	
	}
	else if (isRelease(ea))
	{
		releaseButton(ea);
	}
	else if (isMouseMove(ea))
	{
		mouseMove(ea);
	}
	else if (IsKeyPress(ea))
	{
		keyDown(ea);
	}
	else if (IsDoubleClick(ea))
	{
		double mouseX = ea.getX();
		double mouseY = ea.getY();
		const CPickResult pResult = pOsgPicker->pickResult(mouseX, mouseY);
		Vec3d worldPt = pResult.worldPt;
		Matrix worldMat = pResult.matrix;

		if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
		{
			return false;
		}
		iCall->callDBC(worldPt.x(), worldPt.y(), worldPt.z(), ea.getButton());
	}

	return false;
}

void COsgUnknowEvent::pushButton(const GUIEventAdapter &ea)
{
	timeKeeper->recordStart();

	//���
	if (isLeftClick(ea))
	{
		mouseEventType = 1;
	}
	//�Ҽ�
	else if (IsRightClick(ea))
	{
		mouseEventType = 2;
	}
	else if (IsMiddleClick(ea))
	{
		mouseEventType = 3;
	}
	else
	{
		mouseEventType = 0;
	}
}

void COsgUnknowEvent::releaseButton(const GUIEventAdapter &ea)
{
	if (timeKeeper->getActivate())
	{
		//��ʱ����ʱ����0.2��
		if (timeKeeper->isEnoughTime())
		{
			return;
		}
	}
	else
	{
		if (GetKeyState(VK_SHIFT) < 0)
		{
			return;
		}
	}

	//������ײ��⣬��ȡ��ײ���
	double mouseX = ea.getX();
	double mouseY = ea.getY();
	const CPickResult pResult = pOsgPicker->pickResult(mouseX, mouseY);
	Vec3d worldPt = pResult.worldPt;
	Matrix worldMat = pResult.matrix;

	if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
	{
		return;
	}

	//������
	if (mouseEventType == 1)
	{
		Vec3d clkPt = worldPt;

		notifyHandlerLfClk(clkPt);

// 		if (GetKeyState(VK_CONTROL) < 0)
// 		{
// 			Vec4d color(1.0, 0, 1.0, 1.0);
// 			double size = 3.5f;
// 
// 			Group* varGroup = mModel->getVarNode();
// 			Group* root = mModel->getRoot();
// 
// 			shared_ptr<IBaseDrawer> IVaryDrawer = IBaseDrawerFactory::create(color, size, varGroup, root);
// 			clearGroupResult(varGroup, root);
// 			IVaryDrawer->drawPt(worldPt, worldMat);
// 
// 			iCall->callShowImg(clkPt.x(), clkPt.y(), clkPt.z());
// 		}
// 		else
		{
			//�ص�����¼�
			iCall->callLfClk(clkPt.x(), clkPt.y(), clkPt.z());
		}

		//��������¼�����
		mouseEventType = 0;
	}
	//����Ҽ�
	else if (mouseEventType == 2)
	{
		notifyHandlerRightClk(worldPt);
		//�ص��һ��¼�
		iCall->callRightClk(worldPt.x(), worldPt.y(), worldPt.z());

		//��������¼�����
		mouseEventType = 0;
	}
}

void COsgUnknowEvent::mouseMove(const GUIEventAdapter &ea)
{
	//������ײ���
	CPickResult pResult = pOsgPicker->pickResult(ea.getX(), ea.getY());
	Vec3d worldPt = pResult.worldPt;

	if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
	{
		return;
	}

	Vec3d clkPt = worldPt;

	notifyHandlerMouseMv(clkPt);

	//�ص�����ƶ�������
	iCall->callMouseMv(clkPt.x(), clkPt.y(), clkPt.z());
}

void COsgUnknowEvent::keyDown(const GUIEventAdapter &ea)
{
// 	if (ea.getKey() == GUIEventAdapter::KEY_1)
// 	{
// 		//�л������߳�
// 		shiftHeightLock(ea);
// 	}
//  	else if (ea.getKey() == GUIEventAdapter::KEY_5)
//  	{
//  		//�л�
//  		shiftCatchCorner();
//  	}
// 	else 
		if (ea.getKey() == GUIEventAdapter::KEY_BackSpace)
	{
		back();
		iCall->callBkspace();
	}
// 	else if (ea.getKey() == GUIEventAdapter::KEY_6)
// 	{
// 		shiftOperation();
// 	}
}


void COsgUnknowEvent::notifyHandlerLfClk(Vec3d &clkPt)
{
	for (int i = 0; i < vecHandler.size(); i++)
	{
		shared_ptr<CEventHandler> pHandler = vecHandler[i];
		pHandler->lfClk(clkPt);
	}
}

void COsgUnknowEvent::notifyHandlerRightClk(Vec3d &clkPt)
{
	for (int i = 0; i < vecHandler.size(); i++)
	{
		shared_ptr<CEventHandler> pHandler = vecHandler[i];
		pHandler->rightClk(clkPt);
	}
}

void COsgUnknowEvent::notifyHandlerMouseMv(Vec3d &clkPt)
{
	for (int i = 0; i < vecHandler.size(); i++)
	{
		shared_ptr<CEventHandler> pHandler = vecHandler[i];
		pHandler->mouseMv(clkPt);
	}
}

void COsgUnknowEvent::shiftHeightLock(const GUIEventAdapter &ea)
{
	shiftHeightLock(ea.getX(), ea.getY());
}

void osgEvent::COsgUnknowEvent::shiftHeightLock(float x, float y)
{
	//for (int i = 0; i < vecHandler.size(); i++)
	{
		//shared_ptr<CEventHandler> pHandler = vecHandler[i];
		shared_ptr<CEventHandler> pHandler = locker;
		shared_ptr<COsgHeightLocker> pLocker = dynamic_pointer_cast<COsgHeightLocker> (pHandler);

		if (pLocker)
		{
			bool bLock = pHandler->shift();

			if (bLock)
			{
				//������ײ���

				const CPickResult result = pOsgPicker->pickResult(x,y);
				Vec3d pt = result.worldPt;
				pLocker->setHeight(pt.z());
				if (pt.z() == 0) 
					bLock = pHandler->shift();

			}

			pPrompter->promptLock(bLock);
		}

	}
}

void osgEvent::COsgUnknowEvent::setLastConstDragLine(osg::Vec3d pt1, osg::Vec3d pt2)
{
	shared_ptr<CEventHandler> pHandler = catcher;
	shared_ptr<CCornerPtCatcher> pCatcher = dynamic_pointer_cast<CCornerPtCatcher> (pHandler);

	if (pCatcher)
	{
		pCatcher->setLastConstDragLine(pt1, pt2);
		
	}
}

void osgEvent::COsgUnknowEvent::clearConstDragLine()
{
	shared_ptr<CEventHandler> pHandler = catcher;
	shared_ptr<CCornerPtCatcher> pCatcher = dynamic_pointer_cast<CCornerPtCatcher> (pHandler);

	if (pCatcher)
	{
		pCatcher->clearConstDragLine();
	}
}

void COsgUnknowEvent::shiftCatchCorner()
{
	//for (int i = 0; i < vecHandler.size(); i++)
	{
		//shared_ptr<CEventHandler> pHandler = vecHandler[i];
		shared_ptr<CEventHandler> pHandler = catcher;
		shared_ptr<CCornerPtCatcher> pCatcher = dynamic_pointer_cast<CCornerPtCatcher> (pHandler);

		if (pCatcher)
		{
			bool bCatch = pHandler->shift();
			pPrompter->promptCatch(bCatch);
		}
	}
}

void COsgUnknowEvent::back()
{
	//for (int i = 0; i < vecHandler.size(); i++)
// 	{
// 		//shared_ptr<CEventHandler> pHandler = vecHandler[i];
// 		shared_ptr<CEventHandler> pHandler = catcher;
// 		shared_ptr<CCornerPtCatcher> pCatcher = dynamic_pointer_cast<CCornerPtCatcher> (pHandler);
// 
// 		if (pCatcher)
// 		{
// 			pCatcher->pop();
// 		}
// 	}
}

void COsgUnknowEvent::shiftOperation()
{
	bool bAct = timeKeeper->shift();
	pPrompter->promptOpera(!bAct);
}