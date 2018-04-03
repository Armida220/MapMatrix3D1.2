#include "stdafx.h"
#include "DrawLineHandler.h"
#include "resource.h"

CDrawLineHandler::CDrawLineHandler()
{
}


CDrawLineHandler::~CDrawLineHandler()
{
}



CDrawLineHandler* CDrawLineHandler::getInstance()
{
	static CDrawLineHandler h;
	return &h;
}

void CDrawLineHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("drawLine"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_LINE_LINE, 0), 0);
	}
	else if (funcName == _T("drawLineRect"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_LINE_RECT, 0), 0);
	}
	else if (funcName == _T("drawUnpararell"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_PARALLELOGRAM, 0), 0);
	}
	else if (funcName == _T("drawRegularPoly"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_REGULAR_POLYGON, 0), 0);
	}
	else if (funcName == _T("drawPararell"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_LINE_PARALLEL, 0), 0);
	}
	else if (funcName == _T("drawPararellSingle"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_LINE_SINGLEPARALLEL, 0), 0);
	}
	else if (funcName == _T("drawDoubleLine"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_LINE_DLINE, 0), 0);
	}
}
