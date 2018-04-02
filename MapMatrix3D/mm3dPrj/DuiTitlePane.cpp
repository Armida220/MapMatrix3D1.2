// G:\testMm3D\FeatureOne\FeatureCollector2009\mm3dPrj\DuiTitlePane.cpp : implementation file
//

#include "stdafx.h"
#include "DuiTitlePane.h"
#include "include/wrapper/cef_helpers.h"
#include <string>
#include "glog/log_severity.h"
#include "glog/logging.h"


// CDuiTitlePane

IMPLEMENT_DYNAMIC(CDuiTitlePane, CDockablePane)

CDuiTitlePane::CDuiTitlePane()
{

}

CDuiTitlePane::~CDuiTitlePane()
{
}


BEGIN_MESSAGE_MAP(CDuiTitlePane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCCALCSIZE()

	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_NCMOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CDuiTitlePane message handlers


int CDuiTitlePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	return 0;
}


void CDuiTitlePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	 /*TODO: Add your message handler code here*/
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


void CDuiTitlePane::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default

	/*CDockablePane::OnNcCalcSize(bCalcValidRects, lpncsp);*/
}

CefRefPtr<SimpleHandler>& CDuiTitlePane::getInstance()
{
	static CefRefPtr<SimpleHandler> handler(new SimpleHandler());
	return handler;
}




void CDuiTitlePane::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	SetCapture();

	m_bDrag = true;

	m_oldPt = point;
}


void CDuiTitlePane::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
}

void CDuiTitlePane::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_bDrag)
	{
		if (MK_LBUTTON == nFlags)
		{
			if (point != m_oldPt)
			{
				CPoint ptTemp = point - m_oldPt;
				CRect rcWindow;
				AfxGetApp()->GetMainWnd()->GetWindowRect(&rcWindow);
				rcWindow.OffsetRect(ptTemp.x, ptTemp.y);
				AfxGetApp()->GetMainWnd()->MoveWindow(&rcWindow);
				return;
			}
		}

		m_oldPt = point;
	}

	CDockablePane::OnMouseMove(nFlags, point);
}


void CDuiTitlePane::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	m_bDrag = false;

	ReleaseCapture();
}


void CDuiTitlePane::OnNcMouseMove(UINT nHitTest, CPoint point)
{

}




BOOL CDuiTitlePane::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return FALSE;/*CDockablePane::OnEraseBkgnd(pDC);*/
}


void CDuiTitlePane::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages

	static bool bInit = true;

	if (bInit)
	{
		bInit = false;
		CRect cltRect;
		GetClientRect(&cltRect);

		CefRefPtr<SimpleHandler> handler = getInstance();

		//用浏览器显示影像
		CefWindowInfo window_info;
		window_info.SetAsChild(this->GetSafeHwnd(), cltRect);
		CefBrowserSettings browser_settings;

		std::string toolbarHtml = "file:///G:/testMm3D/bin_FeatureOne/html/title.html";

		bool bSuccess = CefBrowserHost::CreateBrowser(window_info, handler.get(), toolbarHtml, browser_settings, NULL);
	}
}
