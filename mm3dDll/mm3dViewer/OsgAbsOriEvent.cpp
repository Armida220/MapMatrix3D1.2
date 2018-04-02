#include "stdafx.h"
#include "OsgAbsOriEvent.h"
#include "OsgPicker.h"
#include "BaseDrawer.h"
#include "TimeKeeper.h"
#include "ICallBack.h"
#include "osgTransNodeFinder.h"
#include "reader.h"
#include "json.h"

using namespace std;
using namespace osgEvent;
using namespace osgViewer;
using namespace osg;
using namespace osgGA;
using namespace osgDraw;
using namespace osgCall;

template <class Type>
Type stringToNum(const string& str);

COsgAbsOriEvent::COsgAbsOriEvent(Viewer *viewer, Group* constantGroup, Group* variantGroup, 
	Group* root, shared_ptr<ICallBack> ic)
	: CBaseUserEvent(viewer),
	pOsgPicker(new COsgPicker(viewer)),
	pConstantDrawer(new CSceneDrawer(Vec4d(1, 0, 0, 0), 3, constantGroup, root)),
	pVariantDrawer(new CSceneDrawer(Vec4d(1, 0, 0, 0), 3, variantGroup, root)),
	mConstantGroup(constantGroup),
	mVariantGroup(variantGroup),
	mRoot(root),
	timeKeeper(new CTimeKeeper()),
	iCall(ic)
{
}


COsgAbsOriEvent::~COsgAbsOriEvent()
{
}

void COsgAbsOriEvent::fetchOriData()
{
	//��ҳ�����ȡ����
	iCall->callFetchOriData();
}

void COsgAbsOriEvent::showOriPt(string str)
{
	clearGroupResult(mVariantGroup);
	clearGroupResult(mConstantGroup);

	vector<MatrixTransform*> vecTrans;
	COsgTransNodeFinder ive;
	ive.findTransNode(mRoot, vecTrans);
	Matrix worldMatrix;

	if (vecTrans.size() > 0)
	{
		worldMatrix = vecTrans[0]->getMatrix();
	}

	// json����  
	Json::Value value;

	// ��ʼ����json���������json����洢��value�У��ɹ�����true  
	Json::Reader reader;
	if (reader.parse(str, value))
	{
		if (value.size() > 0)
		{
			for (int i = 0; i < value["children"].size(); i++)
			{
				Json::Value child = value["children"][i];
				string strRelaX = child["relaX"].asString();
				string strRelaY = child["relaY"].asString();
				string strRelaZ = child["relaZ"].asString();
				string strPtNo = child["ctrlPtNo"].asString();

				if (strRelaX == "0.0" && strRelaY == "0.0" && strRelaZ == "0.0")
				{
					continue;
				}

				double relaX = stringToNum<double>(strRelaX);
				double relaY = stringToNum<double>(strRelaY);
				double relaZ = stringToNum<double>(strRelaZ);

				Vec3d localPt(relaX, relaY, relaZ);
				Vec3d worldPt = localPt * worldMatrix;
				pConstantDrawer->drawPt(worldPt, worldMatrix);

			}
			
		}
	}

}

void COsgAbsOriEvent::clearOriPt()
{
	clearGroupResult(mVariantGroup);
	clearGroupResult(mConstantGroup);
}


bool COsgAbsOriEvent::handle(const GUIEventAdapter &ea, GUIActionAdapter &aa)
{
	if (isPush(ea))
	{
		pushButton(ea);
	}
	else if (IsDoubleClick(ea))
	{
	}
	else if (isRelease(ea))
	{
		releaseButton(ea);
	}

	return false;
}


void COsgAbsOriEvent::pushButton(const GUIEventAdapter &ea)
{
	timeKeeper->recordStart();

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


void COsgAbsOriEvent::releaseButton(const GUIEventAdapter &ea)
{
	if (timeKeeper->getActivate())
	{
		//��ʱ����ʱ����0.2��
		if (timeKeeper->isEnoughTime())
		{
			return;
		}
	}

	//������
	if (mouseEventType == 1)
	{
		//��������¼�����
		mouseEventType = 0;

		CPickResult pickResult = pOsgPicker->pickResult(ea.getX(), ea.getY());
		Vec3d worldPt = pickResult.worldPt;
		Matrix worldMat = pickResult.matrix;
		Vec3d localPt = pickResult.localPt;

		if (worldPt.x() == 0 && worldPt.y() == 0 && worldPt.z() == 0)
		{
			return;
		}

		iCall->callChangeAbsOriDlg(localPt.x(), localPt.y(), localPt.z());

		//��������
		fetchOriData();
	}
	//����Ҽ�
	else if (mouseEventType == 2)
	{
		//��������¼�����
		mouseEventType = 0;
	}

}




