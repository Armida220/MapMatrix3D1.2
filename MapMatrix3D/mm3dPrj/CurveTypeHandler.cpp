#include "stdafx.h"
#include "CurveTypeHandler.h"
#include "resource.h"

CCurveTypeHandler::CCurveTypeHandler()
{
}


CCurveTypeHandler::~CCurveTypeHandler()
{
}


CCurveTypeHandler* CCurveTypeHandler::getInstance()
{
	static CCurveTypeHandler h;
	return &h;
}

void CCurveTypeHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("drawLineStr"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_CURVETYPE_LINESTRING, 0), 0);
	}
	else if (funcName == _T("drawCurve"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_CURVETYPE_CURVE, 0), 0);
	}
	else if (funcName == _T("drawArc"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_CURVETYPE_ARC, 0), 0);
	}
	else if (funcName == _T("draw3Arc"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_CURVETYPE_3ARC, 0), 0);
	}
}