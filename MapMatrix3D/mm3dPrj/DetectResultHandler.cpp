#include "stdafx.h"
#include "DetectResultHandler.h"
#include "resource.h"

CDetectResultHandler::CDetectResultHandler()
{
}


CDetectResultHandler::~CDetectResultHandler()
{
}


CDetectResultHandler* CDetectResultHandler::getInstance()
{
	static CDetectResultHandler h;
	return &h;
}

void CDetectResultHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("selObj"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_RESULT_SELOBJ, 0), 0);
	}
	else if (funcName == _T("selAll"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_RESULT_SELALL, 0), 0);
	}
	else if (funcName == _T("unSelAll"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_RESULT_UNSELALL, 0), 0);
	}
	else if (funcName == _T("delObjPt"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_RESULT_DELOBJ, 0), 0);
	}
	else if (funcName == _T("delAll"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_RESULT_DELALLOBJ, 0), 0);
	}
	else if (funcName == _T("markObj"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_RESULT_MARKOBJ, 0), 0);
	}
	else if (funcName == _T("markAll"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_RESULT_MARKALL, 0), 0);
	}
	else if (funcName == _T("clearResult"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_RESULT_CLEAR, 0), 0);
	}
}
