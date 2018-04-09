#include "stdafx.h"
#include "GraphicEditHandler.h"
#include "resource.h"

CGraphicEditHandler::CGraphicEditHandler()
{
}


CGraphicEditHandler::~CGraphicEditHandler()
{
}


CGraphicEditHandler* CGraphicEditHandler::getInstance()
{
	static CGraphicEditHandler h;
	return &h;
}


void CGraphicEditHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("move"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_MOVE, 0), 0);
	}
	else if (funcName == _T("delete"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_DEL, 0), 0);
	}
	else if (funcName == _T("copy"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_COPY, 0), 0);
	}
	else if (funcName == _T("rotate"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_ROTATE, 0), 0);
	}
	else if (funcName == _T("copyRotate"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_COPYWITHROTATION, 0), 0);
	}
	else if (funcName == _T("parallelMove"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_PARALLELMOVE, 0), 0);
	}
	else if (funcName == _T("editVert"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_MODIFYVERTEX, 0), 0);
	}
	else if (funcName == _T("insertVert"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_INSERTVERTEX, 0), 0);
	}
	else if (funcName == _T("delVert"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_DELETEVERTEX, 0), 0);
	}
	else if (funcName == _T("closed"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_CLOSE, 0), 0);
	}
	else if (funcName == _T("reverse"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MIDIFY_REVERSE, 0), 0);
	}
	else if (funcName == _T("curveSmooth"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_SMOOTHCURVES, 0), 0);
	}
	else if (funcName == _T("rectify"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_RECTIFY, 0), 0);
	}
	else if (funcName == _T("seperate"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_SEPARATE, 0), 0);
	}
	else if (funcName == _T("break"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_BREAK, 0), 0);
	}
	else if (funcName == _T("replaceCurve"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_REPLACECURVE, 0), 0);
	}
	else if (funcName == _T("replaceCurveOverlap"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_REPLACECURVE_OVERLAP, 0), 0);
	}

}
