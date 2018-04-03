#include "StdAfx.h"
#include "V8JsHandler.h"
#include "OsgbView.h"
#include <string>
#include "DrawPointHandler.h"
#include "DrawLineHandler.h"
#include "DrawSurfaceHandler.h"
#include "DrawCircleHandler.h"
#include "TitlebarHandler.h"
#include "DatabaseOpsHandler.h"
#include "EditBaseHandler.h"
#include "CurveTypeHandler.h"
#include "DetectResultHandler.h"
#include "EditContourHandler.h"
#include "EditVectorHandler.h"
#include "Mm3dHandler.h"

using namespace std;

/**
* @brief replace_all 将字符串中的所有出现的字符替换成另一个字符
* @param s 字符串
* @param t 要被替换的字符
* @param w 用来替换的字符
*/
extern void replace_all(string& s, string const & t, string const & w);


/**
* @brief getOsgView 获取osgbview指针
*/
COsgbView* getOsgView()
{
	POSITION POS;
	POS = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate*   pDocTemplate = AfxGetApp()->GetNextDocTemplate(POS);
	POS = pDocTemplate->GetFirstDocPosition();
	CDocument*   pDoc = pDocTemplate->GetNextDoc(POS);
	POS = pDoc->GetFirstViewPosition();
	CView* pView;
	while (POS != NULL)
	{
		pView = (CView*)pDoc->GetNextView(POS);
		if (pView->IsKindOf(RUNTIME_CLASS(COsgbView)))
		{
			COsgbView* pOsgView = DYNAMIC_DOWNCAST(COsgbView, pView);
			return pOsgView;
		}
	}

	return nullptr;
}


CV8JsHandler::CV8JsHandler()
{
}

CV8JsHandler::~CV8JsHandler(void)
{
}

bool CV8JsHandler::Execute(const CefString& func_name,
							 CefRefPtr<CefV8Value> object,
							 const CefV8ValueList& arguments,
							 CefRefPtr<CefV8Value>& retval,
							 CefString& exception)
{
	if (arguments.size() == 3)
	{
		string imgName = arguments[0]->GetStringValue();
		double x = arguments[1]->GetDoubleValue();
		double y = arguments[2]->GetDoubleValue();
		replace_all(imgName, "\/", "\\");

		if (func_name == _T("reCalGrdPt"))
		{
			COsgbView* pView = getOsgView();
			
			if (pView != nullptr)
			{
				pView->reCalFront(imgName, x, y);
			}
		}
		else if (func_name == _T("reCalFakePt"))
		{
			COsgbView* pView = getOsgView();

			if (pView != nullptr)
			{
				pView->changeFakePt(imgName, x, y);
			}
		}
	}
	else if (arguments.size() == 0)
	{
		if (func_name == _T("revertOps"))
		{
			COsgbView* pView = getOsgView();
			
			if (pView != nullptr)
			{
				pView->bkSpcaeLastOps();
			}
		}
		
		if (func_name == _T("unDo"))
		{
			AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_EDIT_UNDO, 0), 0);
		}
		else if (func_name == _T("reDo"))
		{
			AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_EDIT_REDO, 0), 0);
		}
		
		if (func_name == _T("showProperty"))
		{
			AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_PROPERTIES, 0), 0);
		}
		else if (func_name == _T("showOutput"))
		{
			AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_OUTPUT, 0), 0);
		}
		
		if (func_name == _T("openOsgbView"))
		{
			AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_OsgbView, 0), 0);
		}
		else if (func_name == _T("adjustOsgbArea"))
		{
			AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_SET_OSGB_EXTENT, 0), 0);
		}
		else if (func_name == _T("adjustTileVert"))
		{
			AfxGetApp()->GetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_WINDOW_TILE_VERT, 0), 0);
		}

	}

	//标题栏事件回调
	CTitlebarHandler::getInstance()->handle(func_name, arguments);

	//编辑事件回调

	//数据库工程文件的工具栏
	CDatabaseOpsHandler::getInstance()->handle(func_name, arguments);

	//画点事件回调
	CDrawPointHandler::getInstance()->handle(func_name, arguments);

	//画线事件回调
	CDrawLineHandler::getInstance()->handle(func_name, arguments);

	//画面事件回调
	CDrawSurfaceHandler::getInstance()->handle(func_name, arguments);

	//画面事件回调
	CDrawCircleHandler::getInstance()->handle(func_name, arguments);

	//线串事件回调
	CCurveTypeHandler::getInstance()->handle(func_name, arguments);

	//编辑矢量事件回调
	CEditBaseHandler::getInstance()->handle(func_name, arguments);

	//编辑等高线事件
	CEditContourHandler::getInstance()->handle(func_name, arguments);

	//查看结果事件回调
	CDetectResultHandler::getInstance()->handle(func_name, arguments);

	//编辑矢量事件回调
	CEditVectorHandler::getInstance()->handle(func_name, arguments);

	//mm3d事件回调
	CMm3dHandler::getInstance()->handle(func_name, arguments);

	return true;
}

