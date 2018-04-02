#include "stdafx.h"
#include "CallBack.h"

using namespace std;
using namespace osgCall;

CallBack::CallBack(const Fun &funLfClk, const Fun &funMouseMv, const Fun &funRightClk,
	const Fun4 &funPromptLock, const Fun4 &funPromptCatch, const Fun2 &funBack,
	const Fun4 &funPromptOpera, const Fun &funPan, const Fun &funZoom, const Fun &funShowImg,
	const Fun6 & funDBL, const Fun &funChangeAbsOriDlg, const Fun2 &funFetchOriData)
	: mFunLfClk(funLfClk),
	mFunMouseMv(funMouseMv),
	mFunRightClk(funRightClk),
	mFunPromptLock(funPromptLock),
	mFunPrompCatch(funPromptCatch),
	mFunBack(funBack),
	mFunPromptOpera(funPromptOpera),
	mFunPan(funPan),
	mFunZoom(funZoom), 
	mFunShowImg(funShowImg),
	mFunDBC(funDBL),
	mFunChangeAbsOriDlg(funChangeAbsOriDlg),
	mFunFetchOriData(funFetchOriData)
{
}


CallBack::~CallBack()
{
}

void CallBack::callLfClk(double x, double y, double z)
{
	mFunLfClk(x, y, z);
}

void CallBack::callMouseMv(double x, double y, double z)
{
	mFunMouseMv(x, y, z);
}

void CallBack::callRightClk(double x, double y, double z)
{
	mFunRightClk(x, y, z);
}

void CallBack::promptLock(bool bLock)
{
	mFunPromptLock(bLock);
}

void CallBack::promptCatch(bool bCatch)
{
	mFunPrompCatch(bCatch);
}

void CallBack::callBkspace()
{
	mFunBack();
}

void CallBack::promptOpera(bool bShiftOpera)
{
	mFunPromptOpera(bShiftOpera);
}

void CallBack::callPan(double x, double y, double z)
{
	mFunPan(x, y, z);
}

void CallBack::callZoom(double x, double y, double scale)
{
	mFunZoom(x, y, scale);
}

void CallBack::callShowImg(double x, double y, double z)
{
	mFunShowImg(x, y, z);
}

void CallBack::callChangeAbsOriDlg(double relaX, double relaY, double relaZ)
{
	mFunChangeAbsOriDlg(relaX, relaY, relaZ);
}

void CallBack::callFetchOriData()
{
	return mFunFetchOriData();
}

void osgCall::CallBack::callDBC(double x, double y, double z, int button)
{
	mFunDBC(x, y, z, button);
}
