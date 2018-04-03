#include "stdafx.h"
#include "OsgClipEvent.h"
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include "OsgPicker.h"
#include "BaseDrawer.h"
#include "TimeKeeper.h"
#include "shellapi.h"
#include "tchar.h"

using namespace osgGA;
using namespace osgEvent;
using namespace osgViewer;
using namespace osg;
using namespace osgDraw; 
using namespace std;

string TCHAR2STRING(TCHAR *STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	string str(chRtn);

	return str;
}

TCHAR* STRING2TCHAR(string str)
{
	size_t size = str.length();
	wchar_t *buffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), size, buffer, size * sizeof(wchar_t));
	buffer[size] = 0;  //确保以 '\0' 结尾 
	return buffer;
}

COsgClipEvent::COsgClipEvent(Viewer *viewer, Group* constantGroup,
	Group* variantGroup, Group* root)
	:CBaseUserEvent(viewer),
	pOsgPicker(new COsgPicker(viewer)),
	pConstantDrawer(new CSceneDrawer(Vec4d(1, 1, 0, 0), 3, constantGroup, root)),
	pVariantDrawer(new CSceneDrawer(Vec4d(1, 1, 0, 0), 3, variantGroup, root)),
	mViewer(viewer),
	mConstantGroup(constantGroup),
	mVariantGroup(variantGroup),
	mRoot(root),
	timeKeeper(new CTimeKeeper)
{
}


COsgClipEvent::~COsgClipEvent()
{
} 



bool COsgClipEvent::handle(const GUIEventAdapter &ea, GUIActionAdapter &aa)
{
	if (isPush(ea))
	{
		pushButton(ea);
	}
	else if (IsDoubleClick(ea))
	{
		string para = "3 ";

		for (auto coord : vecWorldCoord)
		{
			para += to_string(coord.x()) + " " + to_string(coord.y()) + " ";
		}


		size_t size = para.length();
		wchar_t *buffer = new wchar_t[size];
		MultiByteToWideChar(CP_ACP, 0, para.c_str(), size, buffer, size * sizeof(wchar_t));
		buffer[size] = 0;

		TCHAR module[_MAX_PATH] = { 0 };
		GetModuleFileName(NULL, module, _MAX_PATH);
		string strDir = TCHAR2STRING(module);
		int pos = strDir.rfind("\\");
		string path = strDir.substr(0, pos + 1);
		string exeFullPath = path + "mm3dDll.exe";

		SHELLEXECUTEINFO shExecInfo = { 0 };
		shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shExecInfo.hwnd = NULL;
		shExecInfo.lpVerb = _T("open");
		shExecInfo.lpFile = STRING2TCHAR(exeFullPath);
		shExecInfo.lpParameters = buffer;
		shExecInfo.lpDirectory = STRING2TCHAR(path);
		shExecInfo.nShow = SW_SHOW;
		shExecInfo.hInstApp = NULL;
		ShellExecuteEx(&shExecInfo);

		vecWorldCoord.clear();
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

	return false;
}



void COsgClipEvent::pushButton(const GUIEventAdapter &ea)
{
	timeKeeper->recordStart();

	if (isLeftClick(ea))
	{
		mouseEventType = 1;
	}
	//右键
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



void COsgClipEvent::releaseButton(const GUIEventAdapter &ea)
{
	if (timeKeeper->getActivate())
	{
		//计时器计时超过0.2秒
		if (timeKeeper->isEnoughTime())
		{
			return;
		}
	}

	//鼠标左键
	if (mouseEventType == 1)
	{
		//重置鼠标事件类型
		mouseEventType = 0;

		CPickResult pickResult = pOsgPicker->pickResult(ea.getX(), ea.getY());
		Vec3d worldPt = pickResult.worldPt;
		Matrix worldMat = pickResult.matrix;

		if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
		{
			return;
		}

		vecWorldCoord.push_back(worldPt);

		lfClkDw(worldMat);
	}
	//鼠标右键
	else if (mouseEventType == 2)
	{
		//重置鼠标事件类型
		mouseEventType = 0;
	}

}

void COsgClipEvent::mouseMove(const GUIEventAdapter &ea)
{
	CPickResult pickResult = pOsgPicker->pickResult(ea.getX(), ea.getY());
	Vec3d worldPt = pickResult.worldPt;
	Matrix worldMat = pickResult.matrix;

	if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
	{
		return;
	}

	mouseMvDw(worldMat, worldPt);
}

void COsgClipEvent::keyDown(const GUIEventAdapter &ea)
{
	if (ea.getKey() == GUIEventAdapter::KEY_BackSpace)
	{
		bkSpackDw();
	}
	else if (ea.getKey() == GUIEventAdapter::KEY_Z)					 //z
	{
		revertDw();
	}
}


void COsgClipEvent::lfClkDw(Matrix worldMatrix)
{
	clearGroupResult(mVariantGroup);
	clearGroupResult(mConstantGroup);

	if (vecWorldCoord.size() >= 3)								//闭合线
	{
		pConstantDrawer->drawLineLoop(vecWorldCoord, worldMatrix);
	}
	else if (vecWorldCoord.size() > 0)							//非闭合
	{
		pConstantDrawer->drawCurLine(vecWorldCoord, worldMatrix);
	}
}

void COsgClipEvent::mouseMvDw(Matrix worldMatrix, Vec3d worldPt)
{
	clearGroupResult(mVariantGroup);
	clearGroupResult(mConstantGroup);

	int totalNum = vecWorldCoord.size();

	if (totalNum == 0)
	{
		return;
	}

	Vec3d endPt = vecWorldCoord[totalNum - 1];
	Vec3d startPt = vecWorldCoord[0];

	pConstantDrawer->drawCurLine(vecWorldCoord, worldMatrix);

	if (vecWorldCoord.size() >= 3)								//闭合线
	{
		pVariantDrawer->drawLine(worldPt, startPt, worldMatrix);
		pVariantDrawer->drawLine(worldPt, endPt, worldMatrix);
	}
	else if (vecWorldCoord.size() > 0)							//非闭合
	{
		pVariantDrawer->drawLine(worldPt, endPt, worldMatrix);
	}
}

void COsgClipEvent::bkSpackDw()
{
	if (vecWorldCoord.size() > 0)
	{
		//撤销上个点
		vecWorldCoord.pop_back();
	}
}

void COsgClipEvent::revertDw()
{
	vecWorldCoord.clear();
	clearGroupResult(mVariantGroup);
	clearGroupResult(mConstantGroup);
}