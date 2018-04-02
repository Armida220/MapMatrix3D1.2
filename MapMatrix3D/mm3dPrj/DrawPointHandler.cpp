#include "stdafx.h"
#include "DrawPointHandler.h"
#include "resource.h"

using namespace std;

CDrawPointHandler::CDrawPointHandler()
{
}


CDrawPointHandler::~CDrawPointHandler()
{
}

CDrawPointHandler* CDrawPointHandler::getInstance()
{
	static CDrawPointHandler h;
	return &h;
}

void CDrawPointHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("drawDot"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_DOT_DOT, 0), 0);
	}
	else if (funcName == _T("drawVectorDot"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_DOT_VECTORDOT, 0), 0);
	}
	else if (funcName == _T("drawHeightDot"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_DOT_HEIGHTDOT, 0), 0);
	}
	else if (funcName == _T("drawAutoHeightDot"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_DOT_AUTOHEIGHT, 0), 0);
	}
	else if (funcName == _T("drawSurfacePoint"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_SURFACEPOINT, 0), 0);
	}
}
