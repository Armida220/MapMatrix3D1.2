#include "stdafx.h"
#include "EditVectorHandler.h"
#include "resource.h"

CEditVectorHandler::CEditVectorHandler()
{
}


CEditVectorHandler::~CEditVectorHandler()
{
}


CEditVectorHandler* CEditVectorHandler::getInstance()
{
	static CEditVectorHandler h;
	return &h;
}

void CEditVectorHandler::handle(const CefString& funcName, const CefV8ValueList& arguments)
{
	if (funcName == _T("modifyReverse"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MIDIFY_REVERSE, 0), 0);
	}
	else if (funcName == _T("modifyClose"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_CLOSE, 0), 0);
	}
	else if (funcName == _T("modifyCloseTole"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_CLOSETOLE, 0), 0);
	}
	else if (funcName == _T("modifyRectify"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_RECTIFY, 0), 0);
	}
	else if (funcName == _T("modifyBreak"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_BREAK, 0), 0);
	}
	else if (funcName == _T("modifyIntesection"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_MODIFYINTERSCCTION, 0), 0);
	}
	else if (funcName == _T("modifyCombine"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_COMBINE, 0), 0);
	}
	else if (funcName == _T("modifySeperate"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_SEPARATE, 0), 0);
	}
	else if (funcName == _T("modifyLineSerial"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_LINESERIAL, 0), 0);
	}
	else if (funcName == _T("modifyCutPart"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_CUTPART, 0), 0);
	}
	else if (funcName == _T("modifyExtent"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_EXTEND, 0), 0);
	}
	else if (funcName == _T("modifyFormatObj"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_FORMATOBJ, 0), 0);
	}
	else if (funcName == _T("modifyZ"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_MODIFYZ, 0), 0);
	}
	else if (funcName == _T("modifyCompress"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_COMPRESS, 0), 0);
	}
	else if (funcName == _T("modifyParallel"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_PARALLELMOVE, 0), 0);
	}
	else if (funcName == _T("modifyTrim"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_TRIM, 0), 0);
	}
	else if (funcName == _T("modifyMove"))
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_MODIFY_MOVETOLOCAL, 0), 0);
	}

}