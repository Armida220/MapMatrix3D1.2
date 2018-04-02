#include "stdafx.h"
#include "EventHandler.h"
#include "ThreadSafeUpdateCallback.h"

using namespace osg;
using namespace osgEvent;
using namespace mm3dView;


CEventHandler::CEventHandler()
{

}


CEventHandler::~CEventHandler()
{
}


void CEventHandler::clearGroupResult(ref_ptr<Group> group, Group* root) const
{
	ref_ptr<CThreadSafeUpdateCallback> spChildCallback = dynamic_cast<CThreadSafeUpdateCallback*>(root->getUpdateCallback());

	if (spChildCallback == NULL)
	{
		return;
	}

	int numChild = group->getNumChildren();

	for (int i = numChild - 1; i >= 0; i--)
	{
		ref_ptr<Node> node = group->getChild(i);
		spChildCallback->RemoveChild(group, node);
	}
}