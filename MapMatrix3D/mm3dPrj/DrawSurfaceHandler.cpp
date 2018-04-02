#include "stdafx.h"
#include "DrawSurfaceHandler.h"
#include "resource.h"

CDrawSurfaceHandler::CDrawSurfaceHandler()
{
}


CDrawSurfaceHandler::~CDrawSurfaceHandler()
{
}

CDrawSurfaceHandler* CDrawSurfaceHandler::getInstance()
{
	static CDrawSurfaceHandler h;
	return &h;
}


void CDrawSurfaceHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("drawPoly"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_FACE_FACE, 0), 0);
	}
	else if (funcName == _T("drawComplxPoly"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_FACE_COMPLEX, 0), 0);
	}
	else if (funcName == _T("drawAutoCompletePoly"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ELEMENT_FACE_AUTO, 0), 0);
	}

}