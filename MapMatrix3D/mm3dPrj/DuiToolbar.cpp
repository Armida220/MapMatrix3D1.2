#include "stdafx.h"
#include "DuiToolbar.h"
#include "include/wrapper/cef_helpers.h"

using namespace std;

CDuiToolbar::CDuiToolbar()
{
}


CDuiToolbar::~CDuiToolbar()
{
}
BEGIN_MESSAGE_MAP(CDuiToolbar, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_NCCALCSIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


int CDuiToolbar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


void CDuiToolbar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

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


void CDuiToolbar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default
	/*CDockablePane::OnNcCalcSize(bCalcValidRects, lpncsp);*/
}


void CDuiToolbar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages

	static bool bInit = TRUE;

	if (bInit)
	{
		bInit = FALSE;

		//获取当前路径
		TCHAR exePath[_MAX_PATH] = { 0 };
		GetModuleFileName(NULL, exePath, _MAX_FNAME);
		TCHAR *pos = _tcsrchr(exePath, _T('\\'));
		if (pos)pos[1] = _T('\0');
		CString path = exePath;
		CString htmlFullPath = path + _T("html\\ribbonToolbar.html");

		CefRefPtr<SimpleHandler> handler = getInstance();

		//用浏览器显示影像
		CRect cltRect;
		GetClientRect(&cltRect);

		CefWindowInfo window_info;
		window_info.SetAsChild(this->GetSafeHwnd(), cltRect);
		CefBrowserSettings browser_settings;

		string toolbarHtml = htmlFullPath;

		bool bSuccess = CefBrowserHost::CreateBrowser(window_info, handler.get(), toolbarHtml, browser_settings, NULL);
	}
}

CefRefPtr<SimpleHandler>& CDuiToolbar::getInstance()
{
	static CefRefPtr<SimpleHandler> handler(new SimpleHandler());
	return handler;
}

BOOL CDuiToolbar::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return FALSE/*CDockablePane::OnEraseBkgnd(pDC)*/;
}


void CDuiToolbar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	/*CDockablePane::OnLButtonDown(nFlags, point);*/

	SetCapture();

	m_bDrag = true;

	m_oldPt = point;
}


void CDuiToolbar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	/*CDockablePane::OnLButtonDblClk(nFlags, point);*/
}


void CDuiToolbar::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	m_bDrag = false;

	ReleaseCapture();
}


void CDuiToolbar::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	/*CDockablePane::OnMouseMove(nFlags, point);*/
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
