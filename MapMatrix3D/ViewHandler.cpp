#include "stdafx.h"
#include "ViewHandler.h"
#include "resource.h"

CViewHandler::CViewHandler()
{
}


CViewHandler::~CViewHandler()
{
}


CViewHandler* CViewHandler::getInstance()
{
	static CViewHandler h;
	return &h;
}

void CViewHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("prjView"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_PROJECT, 0), 0);
	}
	else if (funcName == _T("propertyView"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_PROPERTIES, 0), 0);
	}
	else if (funcName == _T("layerView"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_LAYER, 0), 0);
	}
	else if (funcName == _T("collectionView"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_COLLECTION, 0), 0);
	}
	else if (funcName == _T("settingView"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_INPUTSETTING, 0), 0);
	}
	else if (funcName == _T("outputView"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_OUTPUT, 0), 0);
	}
	else if (funcName == _T("resultView"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_RESULT, 0), 0);
	}
	else if (funcName == _T("openOsgbView"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_OsgbView, 0), 0);
	}

}
