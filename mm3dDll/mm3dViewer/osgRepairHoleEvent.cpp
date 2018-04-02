// osgRepairHoleEvent.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "osgRepairHoleEvent.h"
#include "IOsgRepair.h"
#include "OsgPicker.h"
#include "osgTransNodeFinder.h"
#include <osgDB/WriteFile>
#include "IBaseDrawer.h"
#include "BaseDrawer.h"
#include "TimeKeeper.h"
#include "commdlg.h"
#include <string>

using namespace osgGA;
using namespace osg;
using namespace osgEvent;
using namespace std;
using namespace osgDB;
using namespace osgDraw;


COsgRepairHoleEvent::COsgRepairHoleEvent(osgViewer::Viewer *viewer, Group* constantGroup, Group* variantGroup, Group* root)
	: CBaseUserEvent(viewer),
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

bool COsgRepairHoleEvent::handle(const GUIEventAdapter &ea, GUIActionAdapter &aa)
{
	if (isPush(ea))
	{
		pushButton(ea);
	}
	else if (IsDoubleClick(ea))
	{
		startRepairHole();
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


void COsgRepairHoleEvent::pushButton(const GUIEventAdapter &ea)
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



void COsgRepairHoleEvent::releaseButton(const GUIEventAdapter &ea)
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

		NodePath nodePath = pickResult.nodePath;
		bool isFindTransNode = obtainTransNodeFromNodePath(pickResult.nodePath, mTrans);

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

void COsgRepairHoleEvent::mouseMove(const GUIEventAdapter &ea)
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

void COsgRepairHoleEvent::keyDown(const GUIEventAdapter &ea)
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



bool COsgRepairHoleEvent::startRepairHole()
{
	if (vecWorldCoord.size() < 3)
	{
		return false;
	}

	std::string imageName = "";

	Matrix worldMatrix = mTrans->getMatrix();

	//选择文件
	OPENFILENAME ofn;
	TCHAR szFile[100];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"文件夹\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn))
	{
		imageName = TCHAR2STRING(szFile);
		//创建补洞节点
		shared_ptr<IOsgRepair> IRepair = IOsgRepairFactory::create();
		ref_ptr<Node> repairNode = IRepair->createRepairPatch(vecWorldCoord, worldMatrix, imageName);
		ref_ptr<Group> parent = mTrans->getChild(0)->asGroup();

		if (!repairNode)
		{
			return false;
		}

		vecParentNode.push_back(parent);
		vecChildNode.push_back(repairNode);

		//插入节点
		parent->insertChild(0, repairNode);
		vecWorldCoord.clear();
	}


	return true;

}

bool COsgRepairHoleEvent::obtainTransNodeFromNodePath(NodePath nodePath,  ref_ptr<MatrixTransform> &mTrans)
{
	//裁切
	bool isFindTransNode = false;

	for (int i = 0; i < nodePath.size(); i++)
	{
		ref_ptr<MatrixTransform> node = dynamic_cast<MatrixTransform*> (nodePath[i]);

		if (!node)
		{
			continue;
		}
			
		if (node->getName() == "repairMatrix")
		{
			continue;
		}

		if (mTrans != NULL)
		{
			vector<MatrixTransform*> vecMatrixTransform;
			COsgTransNodeFinder ive;
			ive.findTransNode(node, vecMatrixTransform);
			
			int nodeIndex = 9999; int transIndex = 0;

			for (int k = 0; k < vecMatrixTransform.size(); k++)
			{
				if (vecMatrixTransform[k] == node)
				{
					nodeIndex = k;
					break;
				}
			}

			for (int k = 0; k < vecMatrixTransform.size(); k++)
			{
				if (vecMatrixTransform[k] == mTrans)
				{
					transIndex = k;
					break;
				}
			}

			if (nodeIndex < transIndex)
			{
				mTrans = node;
			}
		}
		else
		{
			mTrans = node;
		}

		isFindTransNode = true;
		break;
	}

	if (isFindTransNode == false)
	{
		return false; 
	}

	return true;
}

void COsgRepairHoleEvent::lfClkDw(Matrix worldMatrix)
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

void COsgRepairHoleEvent::mouseMvDw(Matrix worldMatrix, Vec3d worldPt)
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

void COsgRepairHoleEvent::bkSpackDw()
{
	if (vecWorldCoord.size() > 0)
	{
		//撤销上个点
		vecWorldCoord.pop_back();
	}
}

void COsgRepairHoleEvent::revertDw()
{
	vecWorldCoord.clear();
	clearGroupResult(mVariantGroup);
	clearGroupResult(mConstantGroup);
}


std::string COsgRepairHoleEvent::TCHAR2STRING(TCHAR *STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str = "";
	str = chRtn;
	return str;
}