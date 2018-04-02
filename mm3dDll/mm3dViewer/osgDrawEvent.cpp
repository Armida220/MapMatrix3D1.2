// mm3dDrawEvent.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "osgDrawEvent.h"
#include "OsgUnknowEvent.h"
#include "ICallBack.h"
#include "EventManager.h"
#include "OsgModel.h"

using namespace osgEvent;
using namespace osgCall;
using namespace osg;
using namespace std;
using namespace osgViewer;
using namespace mm3dView;

COsgDrawEvent::COsgDrawEvent(Viewer *view, COsgModel* osgModel, 
	shared_ptr<osgCall::ICallBack> ic)
{
	userEvent = new COsgUnknowEvent(view, osgModel, ic);

	manager = new CSwitchEventManager(view);
	manager->addEvent(0, "unknow event", userEvent);

	//Æô¶¯ÊÂ¼þ
	manager->selectEventByNum(0);
}

void COsgDrawEvent::addEventToViewer(Viewer* view)
{
	view->addEventHandler(manager);
}

void osgEvent::COsgDrawEvent::shiftCatchCorner()
{
	manager->shiftCatchCorner();
}

void osgEvent::COsgDrawEvent::shiftHeightLock(int x, int y)
{
	manager->shiftHeightLock(x, y);
}

void osgEvent::COsgDrawEvent::shiftOperation()
{
	manager->shiftOperation();
}

void osgEvent::COsgDrawEvent::setLastConstDragLine(osg::Vec3d pt1, osg::Vec3d pt2)
{
	((COsgUnknowEvent*)userEvent.get())->setLastConstDragLine(pt1, pt2);
}

void osgEvent::COsgDrawEvent::clearConstDragLine()
{
	((COsgUnknowEvent*)userEvent.get())->clearConstDragLine();
}
