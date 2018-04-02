#include "stdafx.h"
#include "EditContourHandler.h"
#include "resource.h"

CEditContourHandler::CEditContourHandler()
{
}


CEditContourHandler::~CEditContourHandler()
{
}


CEditContourHandler* CEditContourHandler::getInstance()
{
	static CEditContourHandler h;
	return &h;
}

void CEditContourHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("modifyCurve"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_REPLACECURVE, 0), 0);
	}
	else if (funcName == _T("modifyInterCurve"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_INTERPCONTOUR, 0), 0);
	}
	else if (funcName == _T("modifyLink"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_LINKCONTOUR, 0), 0);
	}
	else if (funcName == _T("modifySmooth"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_SMOOTHCURVES, 0), 0);
	}
}
