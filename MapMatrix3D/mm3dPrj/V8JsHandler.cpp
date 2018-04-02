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
* @brief replace_all ���ַ����е����г��ֵ��ַ��滻����һ���ַ�
* @param s �ַ���
* @param t Ҫ���滻���ַ�
* @param w �����滻���ַ�
*/
extern void replace_all(string& s, string const & t, string const & w);


/**
* @brief getOsgView ��ȡosgbviewָ��
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

	//�������¼��ص�
	CTitlebarHandler::getInstance()->handle(func_name, arguments);

	//�༭�¼��ص�

	//���ݿ⹤���ļ��Ĺ�����
	CDatabaseOpsHandler::getInstance()->handle(func_name, arguments);

	//�����¼��ص�
	CDrawPointHandler::getInstance()->handle(func_name, arguments);

	//�����¼��ص�
	CDrawLineHandler::getInstance()->handle(func_name, arguments);

	//�����¼��ص�
	CDrawSurfaceHandler::getInstance()->handle(func_name, arguments);

	//�����¼��ص�
	CDrawCircleHandler::getInstance()->handle(func_name, arguments);

	//�ߴ��¼��ص�
	CCurveTypeHandler::getInstance()->handle(func_name, arguments);

	//�༭ʸ���¼��ص�
	CEditBaseHandler::getInstance()->handle(func_name, arguments);

	//�༭�ȸ����¼�
	CEditContourHandler::getInstance()->handle(func_name, arguments);

	//�鿴����¼��ص�
	CDetectResultHandler::getInstance()->handle(func_name, arguments);

	//�༭ʸ���¼��ص�
	CEditVectorHandler::getInstance()->handle(func_name, arguments);

	//mm3d�¼��ص�
	CMm3dHandler::getInstance()->handle(func_name, arguments);

	return true;
}

