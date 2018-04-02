#include "stdafx.h"
#include "DrawCircleHandler.h"
#include "resource.h"

CDrawCircleHandler::CDrawCircleHandler()
{
}


CDrawCircleHandler::~CDrawCircleHandler()
{
}

CDrawCircleHandler* CDrawCircleHandler::getInstance()
{
	static CDrawCircleHandler h;
	return &h;
}

void CDrawCircleHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("drawCircle2Pt"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_CIRCLE_TWODOT, 0), 0);
	}
	else if (funcName == _T("drawCircle3Pt"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_LINE_THREEDOT, 0), 0);
	}
	else if (funcName == _T("drawCircleRadius"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_CIRCLE_CENTERRADIUS, 0), 0);
	}


}