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
	//�������¼��ص�
	CTitlebarHandler::getInstance()->handle(func_name, arguments);

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

	//��ͼ�¼��ص�
	CViewHandler::getInstance()->handle(func_name, arguments);

	//ͼ���޸Ļص�
	CGraphicEditHandler::getInstance()->handle(func_name, arguments);

	//mm3d�¼��ص�
	CMm3dHandler::getInstance()->handle(func_name, arguments);

	

	return true;
}

