#include "stdafx.h"
#include "Mm3dHandler.h"
#include "MainFrm.h"
#include "OsgbView.h"
#include "myAbsOriDlg.h"
#include <fstream>
#include <json.h>  
#include <iostream>
#include "AbsOriDlgHandler.h"

using namespace std;

extern COsgbView* getOsgView();

CMm3dHandler::CMm3dHandler()
{

}


CMm3dHandler::~CMm3dHandler()
{
}

CMm3dHandler* CMm3dHandler::getInstance()
{
	static CMm3dHandler h;
	return &h;
}

void CMm3dHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("repair"))
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		
		int order = 1;
		COsgbView* pOsgbView = getOsgView();

		if (pOsgbView != nullptr)
		{
			pOsgbView->GetDocument()->UpdateAllViews(NULL, 70, (CObject*)&order);
		}
		
	}
	else if (funcName == _T("produceDom"))
	{
		TCHAR module[_MAX_PATH] = { 0 };
		GetModuleFileName(NULL, module, _MAX_PATH);
		CString strDir = module;
		int pos = strDir.ReverseFind('\\') + 1;
		CString path = strDir.Left(pos);
		CString exeFullPath = path + "mm3dDll.exe";

		SHELLEXECUTEINFO shExecInfo = { 0 };
		shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shExecInfo.hwnd = NULL;
		shExecInfo.lpVerb = _T("open");
		shExecInfo.lpFile = exeFullPath;
		shExecInfo.lpParameters = _T("1");
		shExecInfo.lpDirectory = path;
		shExecInfo.nShow = SW_SHOW;
		shExecInfo.hInstApp = NULL;
		ShellExecuteEx(&shExecInfo);
	}
	else if (funcName == _T("produceDsm"))
	{
		TCHAR module[_MAX_PATH] = { 0 };
		GetModuleFileName(NULL, module, _MAX_PATH);
		CString strDir = module;
		int pos = strDir.ReverseFind('\\') + 1;
		CString path = strDir.Left(pos);
		CString exeFullPath = path + "mm3dDll.exe";

		SHELLEXECUTEINFO shExecInfo = { 0 };
		shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shExecInfo.hwnd = NULL;
		shExecInfo.lpVerb = _T("open");
		shExecInfo.lpFile = exeFullPath;
		shExecInfo.lpParameters = _T("2");
		shExecInfo.lpDirectory = path;
		shExecInfo.nShow = SW_SHOW;
		shExecInfo.hInstApp = NULL;
		ShellExecuteEx(&shExecInfo);
	}
	else if (funcName == _T("clip"))
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;

		int order = 2;
		COsgbView* pOsgbView = getOsgView();

		if (pOsgbView != nullptr)
		{
			pOsgbView->GetDocument()->UpdateAllViews(NULL, 70, (CObject*)&order);
		}

	}
	
	CAbsOriDlgHandler::getInstance()->handle(funcName, arguments);

}