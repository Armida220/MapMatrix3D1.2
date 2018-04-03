#include "stdafx.h"
#include "DatabaseOpsHandler.h"
#include "resource.h"

CDatabaseOpsHandler::CDatabaseOpsHandler()
{
}


CDatabaseOpsHandler::~CDatabaseOpsHandler()
{
}


CDatabaseOpsHandler* CDatabaseOpsHandler::getInstance()
{
	static CDatabaseOpsHandler h;
	return &h;
}


void CDatabaseOpsHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("loadPrj"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_LOAD_PROJECT, 0), 0);
	}
	else if(funcName == _T("unloadPrj"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_UNINSTALL_PROJECT, 0), 0);
	}
	else if (funcName == _T("closePrj"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_CLOSE, 0), 0);
	}
	else if (funcName == _T("newDocument"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_NEW, 0), 0);
	}
	else if (funcName == _T("openFile"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_OPEN, 0), 0);
	}
	else if (funcName == _T("saveFile"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_SAVE, 0), 0);
	}

}
