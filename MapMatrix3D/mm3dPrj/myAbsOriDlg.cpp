// G:\testMm3D\FeatureOne\FeatureCollector2009\mm3dPrj\EmptyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myAbsOriDlg.h"
#include "afxdialogex.h"
#include "include/wrapper/cef_helpers.h"
#include "qcomm.h"

using namespace std;
// CMyAbsOriDlg dialog

IMPLEMENT_DYNAMIC(CMyAbsOriDlg, CDialogEx)

CMyAbsOriDlg::CMyAbsOriDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyAbsOriDlg::IDD, pParent)
{

}

CMyAbsOriDlg::~CMyAbsOriDlg()
{
}

void CMyAbsOriDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMyAbsOriDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CMyAbsOriDlg message handlers
void CMyAbsOriDlg::reloadReportHtml(string reportStr)
{
	mReportStr = reportStr;
	//获取当前路径
	TCHAR exePath[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, exePath, _MAX_FNAME);
	TCHAR *pos = _tcsrchr(exePath, _T('\\'));
	if (pos)pos[1] = _T('\0');
	CString path = exePath;
	string htmlFullPath = path + _T("html\\report.html");

	CefRefPtr<SimpleHandler> handler = getInstance();
	std::list<CefRefPtr<CefBrowser>> listBrowser = handler->getBrowserList();

	for (auto br : listBrowser)
	{
		CefRefPtr<CefFrame> frame = br->GetMainFrame();
		CefString str = htmlFullPath;
		frame->LoadURL(str);
	}
}

void CMyAbsOriDlg::showPrecision()
{
	CefRefPtr<SimpleHandler> handler = getInstance();
	std::list<CefRefPtr<CefBrowser>> listBrowser = handler->getBrowserList();

	for (auto br : listBrowser)
	{
		string jsScript = "setReport(" + mReportStr + ");";

		CefRefPtr<CefFrame> frame = br->GetMainFrame();
		frame->ExecuteJavaScript(jsScript, frame->GetURL(), 0);
	}
}

void CMyAbsOriDlg::backToAbsDlg()
{
	CefRefPtr<SimpleHandler> handler = getInstance();
	std::list<CefRefPtr<CefBrowser>> listBrowser = handler->getBrowserList();

	for (auto br : listBrowser)
	{
		br->GoBack();
	}
}


void CMyAbsOriDlg::setVmData(string vmDataStr)
{
	CefRefPtr<SimpleHandler> handler = getInstance();
	std::list<CefRefPtr<CefBrowser>> listBrowser = handler->getBrowserList();

	for (auto br : listBrowser)
	{
		string jsScript = "setCtrlVm(" + vmDataStr + ");";

		CefRefPtr<CefFrame> frame = br->GetMainFrame();
		frame->ExecuteJavaScript(jsScript, frame->GetURL(), 0);
	}

}

int CMyAbsOriDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}

CefRefPtr<SimpleHandler>& CMyAbsOriDlg::getInstance()
{
	static CefRefPtr<SimpleHandler> handler(new SimpleHandler());
	return handler;
}

void CMyAbsOriDlg::OnSize(UINT nType, int cx, int cy)
{
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

	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}


BOOL CMyAbsOriDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//return CDialogEx::OnEraseBkgnd(pDC);
	return FALSE;
}


void CMyAbsOriDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetCapture();

	m_bDrag = true;

	m_oldPt = point;
}


BOOL CMyAbsOriDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CefRefPtr<SimpleHandler> handler = getInstance();

	CRect rect;
	GetWindowRect(&rect);

	//::SetWindowPos(this->m_hWnd, HWND_TOP, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);

	//用浏览器显示影像
	CefWindowInfo window_info;
	window_info.SetAsChild(this->GetSafeHwnd(), rect);
	CefBrowserSettings browser_settings;

	//获取当前路径
	TCHAR exePath[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, exePath, _MAX_FNAME);
	TCHAR *pos = _tcsrchr(exePath, _T('\\'));
	if (pos)pos[1] = _T('\0');
	CString path = exePath;
	string htmlFullPath = path + _T("html\\absDialog.html");

	bool bSuccess = CefBrowserHost::CreateBrowser(window_info, handler.get(), htmlFullPath, browser_settings, NULL);

	this->MoveWindow(200, 200, rect.Width(), rect.Height(), TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CMyAbsOriDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	/*CDialogEx::OnLButtonDblClk(nFlags, point);*/
}


void CMyAbsOriDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	/*CDialogEx::OnLButtonUp(nFlags, point);*/

	m_bDrag = false;

	ReleaseCapture();
}


void CMyAbsOriDlg::OnMouseMove(UINT nFlags, CPoint point)
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
				this->GetWindowRect(&rcWindow);
				rcWindow.OffsetRect(ptTemp.x, ptTemp.y);
				this->MoveWindow(&rcWindow);
				return;
			}
		}

		m_oldPt = point;
	}

	CDialogEx::OnMouseMove(nFlags, point);
}
