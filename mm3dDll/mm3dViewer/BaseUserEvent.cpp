// BaseUserEvent.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h" 
#include "BaseUserEvent.h"
#include "ThreadSafeUpdateCallback.h"
#include <time.h>
#include "qcomm.h"


using namespace osgEvent;
using namespace osgGA;
using namespace osg;
using namespace std;
using namespace osgUtil;
using namespace mm3dView;

bool CBaseUserEvent::IsKeyPress(const GUIEventAdapter &ea) const
{
	if (ea.getEventType() == GUIEventAdapter::KEYDOWN)
	{
		return true;
	}

	return false;
}

bool CBaseUserEvent::IsKeyUp(const GUIEventAdapter &ea) const
{
	if (ea.getEventType() == GUIEventAdapter::KEYUP)
	{
		return true;
	}

	return false;
}

bool CBaseUserEvent::isLeftClick(const GUIEventAdapter &ea) const
{
	if (ea.getEventType() == GUIEventAdapter::PUSH)
	{
		if (ea.getButtonMask() == GUIEventAdapter::LEFT_MOUSE_BUTTON)//鼠标左键
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool CBaseUserEvent::IsDoubleClick(const GUIEventAdapter &ea) const
{
	if (ea.getEventType() == GUIEventAdapter::DOUBLECLICK)
	{
		return true;
	}

	return false;
}

bool CBaseUserEvent::IsRightClick(const GUIEventAdapter &ea) const
{
	if (ea.getEventType() == GUIEventAdapter::PUSH)
	{
		if (ea.getButtonMask() == GUIEventAdapter::RIGHT_MOUSE_BUTTON)//鼠标双击
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool CBaseUserEvent::IsMiddleClick(const GUIEventAdapter &ea) const
{
	if (ea.getEventType() == GUIEventAdapter::PUSH)
	{
		if (ea.getButtonMask() == GUIEventAdapter::MIDDLE_MOUSE_BUTTON)//鼠标中键
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool CBaseUserEvent::isMouseMove(const GUIEventAdapter &ea) const
{
	if (ea.getEventType() == GUIEventAdapter::MOVE)//鼠标移动
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CBaseUserEvent::isDrag(const GUIEventAdapter &ea) const
{
	if (ea.getEventType() == GUIEventAdapter::DRAG) //鼠标拖动
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CBaseUserEvent::isBkSpace(const osgGA::GUIEventAdapter &ea) const
{
	if (ea.getKey() == GUIEventAdapter::KEY_BackSpace && GetKeyState(VK_MENU) >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CBaseUserEvent::isDel(const osgGA::GUIEventAdapter &ea) const
{
	if (ea.getKey() == GUIEventAdapter::KEY_Delete)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CBaseUserEvent::isCtrlZ(const osgGA::GUIEventAdapter &ea) const
{
	if (ea.getKey() == GUIEventAdapter::KEY_Z && GetKeyState(VK_CONTROL) <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CBaseUserEvent::isPush(const GUIEventAdapter &ea) const
{
	if (ea.getEventType() == GUIEventAdapter::PUSH)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CBaseUserEvent::isRelease(const GUIEventAdapter &ea) const
{
	if (ea.getEventType() == GUIEventAdapter::RELEASE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CBaseUserEvent::isReleaseLClk(const GUIEventAdapter &ea) const
{
	if (isRelease(ea))
	{
		if (ea.getButtonMask() == GUIEventAdapter::LEFT_MOUSE_BUTTON)//鼠标左键
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool CBaseUserEvent::isReleaseRClk(const GUIEventAdapter &ea) const
{
	if (isRelease(ea))
	{
		if (ea.getButtonMask() == GUIEventAdapter::RIGHT_MOUSE_BUTTON)//鼠标右键
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}



void CBaseUserEvent::clearGroupResult(ref_ptr<Group> group) const
{
	while (group->getNumChildren() > 0)
	{
		osg::ref_ptr<osg::Group> childGroup = group->getChild(0)->asGroup();
		//如果是Group，先删除其子节点，再删除自身
		if (childGroup)
		{
			while (childGroup->getNumChildren() > 0)
				childGroup->removeChildren(0, 1);
		}

		group->removeChildren(0, 1);
	}
}

void CBaseUserEvent::clearGroupResult(ref_ptr<Group> group, Group* root) const
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


double CBaseUserEvent::CalLength(const vector<Vec3d> coord) const
{
	if (coord.size() < 2)
	{
		return 0;
	}

	double Length = 0;

	for (int i = 0; i < coord.size() - 1; i++)
	{
		Length += CalculateDist(coord[i], coord[i + 1]);
	}

	return Length;
};

double CBaseUserEvent::CalculateDist(const Vec3d pt1, const Vec3d pt2) const
{
	double distance = sqrt((pt1.x() - pt2.x()) * (pt1.x() - pt2.x()) + (pt1.y() - pt2.y()) * (pt1.y() - pt2.y()) + (pt1.z() - pt2.z()) * (pt1.z() - pt2.z()));
	return distance;
}

double CBaseUserEvent::CalculateArea(const vector<Vec3d> coord) const
{
	if (coord.size() < 3)
	{
		return 0;
	}

	double area = 0.0;
	//每后两个点与第一个点形成的三角形面积之和
	for (unsigned int i = 2; i < coord.size(); i++)
	{
		float a = CalculateDist(coord[0], coord[i - 1]);      //第0个点到第i-1个点的距离
		float b = CalculateDist(coord[i - 1], coord[i]);		//第i-1个点到第i个点的距离
		float c = CalculateDist(coord[i], coord[0]);		//第i个点到第0个点的距离
		area += sqrt((a + b + c) * (a + b - c) * (a + c - b) * (b + c - a)) / 4;
	}

	return area;
}

bool CBaseUserEvent::lazyRefresh() const
{
	//降低刷新的频率
	static clock_t cur = 0, last = 0;
	cur = clock();
	double duration = cur - last;
	last = cur;

	if (duration > 10)
	{
		return true;
	}
	else
	{
		return false;
	}

}