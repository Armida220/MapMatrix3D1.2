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
#include "CurveTypeHandler.h"
#include "Mm3dHandler.h"
#include "ViewHandler.h"
#include "GraphicEditHandler.h"

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
	//标题栏事件回调
	CTitlebarHandler::getInstance()->handle(func_name, arguments);

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

	//视图事件回调
	CViewHandler::getInstance()->handle(func_name, arguments);

	//图形修改回调
	CGraphicEditHandler::getInstance()->handle(func_name, arguments);

	//mm3d事件回调
	CMm3dHandler::getInstance()->handle(func_name, arguments);

	

	return true;
}

