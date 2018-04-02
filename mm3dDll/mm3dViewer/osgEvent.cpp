// mm3dDrawEvent.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "osgEvent.h"
#include "OsgUnknowEvent.h"
#include "ICallBack.h"
#include "EventManager.h"
#include "OsgModel.h"
#include "osgRepairHoleEvent.h"
#include "OsgClipEvent.h"
#include "OsgAbsOriEvent.h"

using namespace osgEvent;
using namespace osgCall;
using namespace osg;
using namespace std;
using namespace osgViewer;
using namespace mm3dView;

COsgEvent::COsgEvent(Viewer *view, COsgModel* osgModel, 
	shared_ptr<osgCall::ICallBack> ic)
{
	userEvent = new COsgUnknowEvent(view, osgModel, ic);

	Group* editGroup = osgModel->getEditNode();
	Group* variantGroup = osgModel->getTempNode();
	Group* root = osgModel->getRoot();

	CBaseUserEvent* osgRepairEvent = new COsgRepairHoleEvent(view, editGroup, variantGroup, root);
	CBaseUserEvent* osgClipEvent = new COsgClipEvent(view, editGroup, variantGroup, root);
	CBaseUserEvent* osgAbsOriEvent = new COsgAbsOriEvent(view, editGroup, variantGroup, root, ic);


	manager = new CSwitchEventManager(view);
	manager->addEvent(0, "unknow event", userEvent);
	manager->addEvent(1, "repair event", osgRepairEvent);
	manager->addEvent(2, "clip event", osgClipEvent);
	manager->addEvent(3, "absOriEvent", osgAbsOriEvent);

	//Æô¶¯ÊÂ¼þ
	manager->selectEventByNum(0);
}

void COsgEvent::addEventToViewer(Viewer* view)
{
	view->addEventHandler(manager);
}

void COsgEvent::startEvent(int order)
{
	manager->selectEventByNum(order);
}


void COsgEvent::shiftCatchCorner()
{
	manager->shiftCatchCorner();
}

void COsgEvent::shiftHeightLock(int x, int y)
{
	manager->shiftHeightLock(x, y);
}

void COsgEvent::shiftOperation()
{
	manager->shiftOperation();
}

void COsgEvent::setLastConstDragLine(osg::Vec3d pt1, osg::Vec3d pt2)
{
	((COsgUnknowEvent*)userEvent.get())->setLastConstDragLine(pt1, pt2);
}

void COsgEvent::clearConstDragLine()
{
	((COsgUnknowEvent*)userEvent.get())->clearConstDragLine();
}

