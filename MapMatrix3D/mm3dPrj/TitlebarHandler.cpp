#include "stdafx.h"
#include "TitlebarHandler.h"
#include "resource.h"
#include "../MainFrm.h"


CTitlebarHandler::CTitlebarHandler()
{
}


CTitlebarHandler::~CTitlebarHandler()
{
}

CTitlebarHandler* CTitlebarHandler::getInstance()
{
	static CTitlebarHandler h;
	return &h;
}


void CTitlebarHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{

	if (funcName == _T("selectWin"))
	{
		double x = arguments[0]->GetDoubleValue();
		double y = arguments[1]->GetDoubleValue();

		CMainFrame* mainFrame = dynamic_cast<CMainFrame*> (AfxGetApp()->GetMainWnd());

		if (mainFrame)
		{
			CDuiToolbar* duiTitle = mainFrame->getToolbarPane();
			::SendMessage(duiTitle->GetSafeHwnd(), WM_LBUTTONDOWN, 0, MAKELPARAM(x, y));
		}

	}
	else if (funcName == _T("minusMainWin"))
	{
		AfxGetApp()->GetMainWnd()->ShowWindow(SW_SHOWMINIMIZED);
	}
	else if (funcName == _T("fullScreen"))
	{
		if (AfxGetApp()->GetMainWnd()->IsZoomed())
		{
			AfxGetApp()->GetMainWnd()->ShowWindow(SW_RESTORE);
		}
		else
		{
			AfxGetApp()->GetMainWnd()->ShowWindow(SW_SHOWMAXIMIZED);
		}

	}
	else if (funcName == _T("closeMainWin"))
	{
		AfxGetApp()->GetMainWnd()->PostMessage(WM_CLOSE);
	}
}
