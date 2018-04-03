// ProjectPane.cpp : implementation file
//

#include "stdafx.h"
#include "DuiTreeView.h"
#include "include/wrapper/cef_helpers.h"

// CProjectPane

IMPLEMENT_DYNAMIC(CDuiTreeView, CDockablePane)

CDuiTreeView::CDuiTreeView()
{

}

CDuiTreeView::~CDuiTreeView()
{
}


BEGIN_MESSAGE_MAP(CDuiTreeView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCCALCSIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CProjectPane message handlers

CefRefPtr<SimpleHandler>& CDuiTreeView::getInstance()
{
	static CefRefPtr<SimpleHandler> handler(new SimpleHandler());
	return handler;
}


int CDuiTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


void CDuiTreeView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	int num = 0;

	CefRefPtr<SimpleHandler> handler = getInstance();
	std::list<CefRefPtr<CefBrowser>> listBrowser = handler->getBrowserList();

	for (auto br : listBrowser)
	{
		HWND hwnd = br->GetHost()->GetWindowHandle();

		//子控件的范围
		double left = 0 + num * cx / listBrowser.size();
		double right = left + cx / listBrowser.size();
		double top = 0;
		double bottom = cy;

		//移动控件到
		CRect childRect(left, top, right, bottom);
		::MoveWindow(hwnd, childRect.left, childRect.top, childRect.Width(), childRect.Height(), TRUE);

		num++;
	}

}


void CDuiTreeView::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default

	/*CDockablePane::OnNcCalcSize(bCalcValidRects, lpncsp);*/
}




void CDuiTreeView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDockablePane::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
}


void CDuiTreeView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages

	static bool bInit = TRUE;

	if (bInit)
	{
		bInit = false;

		//显示浏览器
		CRect cltRect;
		GetClientRect(&cltRect);

		CefRefPtr<SimpleHandler> handler = getInstance();

		//用浏览器显示影像
		CefWindowInfo window_info;
		window_info.SetAsChild(this->GetSafeHwnd(), cltRect);
		CefBrowserSettings browser_settings;

		bool bSuccess = CefBrowserHost::CreateBrowser(window_info, handler.get(), "file:///G:/testMm3D/bin_FeatureOne/html/collapseMenu.html", browser_settings, NULL);
	}
}
