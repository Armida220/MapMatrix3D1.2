#include "stdafx.h"
#include "EventManager.h"
#include "OsgUnknowEvent.h"
#include "OsgAbsOriEvent.h"
using namespace osgEvent;
using namespace osg;
using namespace osgGA;
using namespace std;
using namespace osgViewer;


CSwitchEventManager::CSwitchEventManager(Viewer* viewer)
	: CBaseUserEvent(viewer)
{

}

void CSwitchEventManager::addEvent(int key, string name, CBaseUserEvent *ce)
{
	if (!ce) return;

	_manips[key] = make_pair(name, ref_ptr<CBaseUserEvent>(ce));

	//设置管理者
	ce->setManager(this);

	if (!_current)
	{
		_current = ce;
	}
}


void CSwitchEventManager::selectEventByNum(unsigned int num)
{
	unsigned int manipNo = 0;
	KeyMainMap::iterator itr;
	for (itr = _manips.begin();
		manipNo != num && itr != _manips.end();
		++itr, ++manipNo)
	{
	}

	if (itr != _manips.end())
	{
		_current = itr->second.second;
	}
}

void CSwitchEventManager::showOriPt(string oriRes)
{
	CBaseUserEvent* temp = _current;
	osg::ref_ptr<COsgAbsOriEvent> absOriEvent = dynamic_cast<COsgAbsOriEvent*> (temp);

	if (absOriEvent != nullptr)
	{
		absOriEvent->showOriPt(oriRes);
	}
}

void CSwitchEventManager::clearOriPt()
{
	CBaseUserEvent* temp = _current;
	osg::ref_ptr<COsgAbsOriEvent> absOriEvent = dynamic_cast<COsgAbsOriEvent*> (temp);

	if (absOriEvent != nullptr)
	{
		absOriEvent->clearOriPt();
	}
}

void CSwitchEventManager::fetchOriData()
{
	CBaseUserEvent* temp = _current;
	osg::ref_ptr<COsgAbsOriEvent> absOriEvent = dynamic_cast<COsgAbsOriEvent*> (temp);

	if (absOriEvent != nullptr)
	{
		absOriEvent->fetchOriData();
	}
}

bool CSwitchEventManager::handle(const GUIEventAdapter& ea, GUIActionAdapter& aa)
{
	if (!_current) return false;

	return _current->handle(ea, aa);
}

void osgEvent::CSwitchEventManager::shiftHeightLock(float x, float y)
{
	if (!_current) return ;

	CBaseUserEvent* temp = _current;
	osg::ref_ptr<COsgUnknowEvent> unknowEvent = dynamic_cast<COsgUnknowEvent*> (temp);

	unknowEvent->shiftHeightLock(x, y);
}

void osgEvent::CSwitchEventManager::shiftCatchCorner()
{
	if (!_current) return;

	CBaseUserEvent* temp = _current;
	osg::ref_ptr<COsgUnknowEvent> unknowEvent = dynamic_cast<COsgUnknowEvent*> (temp);

	unknowEvent->shiftCatchCorner();
}

void osgEvent::CSwitchEventManager::shiftOperation()
{
	if (!_current) return;

	CBaseUserEvent* temp = _current;
	osg::ref_ptr<COsgUnknowEvent> unknowEvent = dynamic_cast<COsgUnknowEvent*> (temp);

	unknowEvent->shiftOperation();
}
