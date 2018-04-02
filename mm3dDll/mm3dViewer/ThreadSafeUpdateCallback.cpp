#include "stdafx.h"
#include "qcomm.h"
#include "ThreadSafeUpdateCallback.h"
#include "CondSafeQueue.h"
#include "LockSafeQueue.h"
#include "FineLockQueue.h"
#include "FineCondQueue.h"
#include "AtomicSafeQueue.h"
#include "NodeAddOperator.h"
#include "NodeRemoveOperator.h"
#include "NodeOperator.h"
#include "SetHomerOperator.h"
#include "HomeOperator.h"
#include "RemoveAllOperator.h"
#include "DriveToHighLightOperator.h"
#include "CameraDriveTo.h"
#include "CameraPanTo.h"
#include "CameraZoomTo.h"
#include "SelfDefTerrainManipulator.h"

using namespace osg;
using namespace std;
using namespace mm3dView;
using namespace osgGA;
using namespace osgViewer;


static int taskNum = 0;

CThreadSafeUpdateCallback::CThreadSafeUpdateCallback(void)
{
	typedef CCondSafeQueue<ref_ptr<NodeOperator>> CONDQUEUE_NODE;
	typedef CLockSafeQueue<ref_ptr<NodeOperator>> LOCKQUEUE_NODE;
	typedef CFineLockQueue<ref_ptr<NodeOperator>> FINE_LOKCQUEUE_NODE;
	typedef CFineCondQueue<ref_ptr<NodeOperator>> FINE_CONDQUEUE_NODE;
	typedef CAtomicSafeQueue<ref_ptr<NodeOperator>> ATOMIC_SAFE_QUEUE;
	
	m_tasks = new LOCKQUEUE_NODE();
}


CThreadSafeUpdateCallback::~CThreadSafeUpdateCallback(void)
{
}

void CThreadSafeUpdateCallback::operator()(Node* node, NodeVisitor* nv)
{
	if (!node)
	{
		return;
	}

	// note, callback is responsible for scenegraph traversal so
	// they must call traverse(node,nv) to ensure that the
	// scene graph subtree (and associated callbacks) are traversed.

	//////////////////////////////////////////////////////////////////////////
	// operator 

	while (!m_tasks->empty())
	{
		ref_ptr<NodeOperator> nodeOperator = nullptr;
		m_tasks->pop(nodeOperator);

		if (nodeOperator)
		{
			(*nodeOperator)();
		}
	}

	traverse(node,nv);
}

bool CThreadSafeUpdateCallback::AddChild(Node* parent, Node* node)
{
	if( parent==NULL || node==NULL )
		return false;

	//构建任务
	taskNum++;
	ref_ptr<Group> spGroup = parent->asGroup();
	ref_ptr<NodeOperator> ops = new CNodeAddOperator(spGroup, node, taskNum);

	//进入任务池
	m_tasks->push(move(ops));

	return true;
}

bool CThreadSafeUpdateCallback::RemoveChild(Node* parent, Node* node)
{
	if( parent==NULL || node==NULL )
		return false;

	//构建任务
	taskNum++;
	ref_ptr<Group> spGroup = parent->asGroup();
	ref_ptr<NodeOperator> ops = new CNodeRemoveOperator(spGroup, node, taskNum);
	
	//进入任务池
	m_tasks->push(move(ops));

	return true;
}

bool CThreadSafeUpdateCallback::setHomeCenter(ref_ptr<TerrainManipulator> terrain,
	COsgModel* osgbModel)
{
	if (terrain == nullptr || osgbModel == nullptr)
	{
		return false;
	}

	//构建任务
	taskNum++;
	ref_ptr<NodeOperator> ops = new CSetHomerOperator(terrain, osgbModel);
	
	//进入任务池
	m_tasks->push(move(ops));

	return true;
}

bool CThreadSafeUpdateCallback::home(ref_ptr<TerrainManipulator> terrain)
{
	if (terrain == nullptr)
	{
		return false;
	}

	//构建任务
	taskNum++;
	ref_ptr<NodeOperator> ops = new CHomeOperator(terrain);

	//进入任务池
	m_tasks->push(move(ops));

	return true;
}

bool CThreadSafeUpdateCallback::removeAllChild(ref_ptr<Group> group)
{
	if (group == nullptr)
	{
		return false;
	}

	//构建任务
	taskNum++;

	ref_ptr<NodeOperator> ops = new CRemoveAllOperator(group);

	//进入任务池
	m_tasks->push(move(ops));

	return true;
}

bool CThreadSafeUpdateCallback::driveToHighLight(ref_ptr<Viewer> viewer,
	ref_ptr<Group> highGroup)
{
	if (viewer == nullptr && highGroup == nullptr)
	{
		return false;
	}

	//构建任务
	taskNum++;

	ref_ptr<NodeOperator> ops = new CDriveToHighLightOperator(viewer, highGroup);

	//进入任务池
	m_tasks->push(move(ops));

	return true;
}

bool CThreadSafeUpdateCallback::driveToXy(ref_ptr<Viewer> viewer,
	double x, double y)
{
	if (viewer == nullptr)
	{
		return false;
	}

	//构建任务
	taskNum++;

	ref_ptr<NodeOperator> ops = new CCameraDriveTo(viewer, Vec3d(x, y, 0));

	//进入任务池
	m_tasks->push(move(ops));

	return true;
}

bool CThreadSafeUpdateCallback::panTo(ref_ptr<CSelfDefTerrainManipulator> terrain,
	double x, double y)
{
	//构建任务
	taskNum++;

	ref_ptr<NodeOperator> ops = new CCameraPanTo(terrain, x, y);

	//进入任务池
	m_tasks->push(move(ops));

	return true;
}

bool CThreadSafeUpdateCallback::zoomTo(osg::ref_ptr<CSelfDefTerrainManipulator> terrain,
	double scale)
{
	//构建任务
	taskNum++;

	ref_ptr<NodeOperator> ops = new CCameraZoomTo(terrain, scale);

	//进入任务池
	m_tasks->push(move(ops));

	return true;
}


bool CThreadSafeUpdateCallback::isNeedToWait()
{
	return m_tasks->isNeedToWait();
}
