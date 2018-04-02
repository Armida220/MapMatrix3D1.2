#include "stdafx.h"
#include "EditBaseHandler.h"
#include "resource.h"

CEditBaseHandler::CEditBaseHandler()
{
}


CEditBaseHandler::~CEditBaseHandler()
{
}

CEditBaseHandler* CEditBaseHandler::getInstance()
{
	static CEditBaseHandler h;
	return &h;
}

void CEditBaseHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("select"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_SELECT, 0), 0);
	}
	else if (funcName == _T("selectRect"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_SELECT_RECT, 0), 0);
	}
	else if (funcName == _T("selectMove"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_MOVE, 0), 0);
	}
	else if (funcName == _T("copy"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_COPY, 0), 0);
	}
	else if (funcName == _T("rotateCopy"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_COPYWITHROTATION, 0), 0);
	}
	else if (funcName == _T("delete"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_DEL, 0), 0);
	}
	else if (funcName == _T("rotate"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_ROTATE, 0), 0);
	}
	else if (funcName == _T("mirror"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_MIRROR, 0), 0);
	}

}