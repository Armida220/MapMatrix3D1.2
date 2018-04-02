#include "stdafx.h"
#include "ICallBack.h"
#include "CallBack.h"

using namespace osgCall;
using namespace std;

ICallBack::ICallBack()
{
}


ICallBack::~ICallBack()
{
	
}


shared_ptr<ICallBack> ICallFactory::create(const Fun &funLfClk, const Fun &funMouseMv,
	const Fun &funRightClk, const Fun4 &funPromptLock, const Fun4 &funPromptCatch,
	const Fun2& funBack, const Fun4 &funPromptOpera, const Fun &funPan, const Fun &funZoom, 
	const Fun &funShowImg, const Fun6 & funDBL, const Fun &funChangeAbsOriDlg, const Fun2 &funFetchOriData)
{
	shared_ptr<ICallBack> pCall(new CallBack(funLfClk, funMouseMv, funRightClk, 
		funPromptLock, funPromptCatch, funBack, funPromptOpera, funPan, funZoom,
		funShowImg, funDBL, funChangeAbsOriDlg, funFetchOriData));
	return pCall;
}