// DlgCefMainToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "DlgCefMainToolBar.h"
#include "afxdialogex.h"
#include "DlgCefToolBox.h"

CString GetModuleFolder()
{
	TCHAR   szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);
	ZeroMemory(_tcsrchr(szPath, _T('\\')), _tcslen(_tcsrchr(szPath, _T('\\')))*sizeof(TCHAR));
	return CString(szPath);
}
// CDlgCefMainToolBar dialog

IMPLEMENT_DYNAMIC(CDlgCefMainToolBar, CDialogEx)

CDlgCefMainToolBar::CDlgCefMainToolBar(std::string url,CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCefMainToolBar::IDD, pParent)
{
	URL = url;
}

CDlgCefMainToolBar::~CDlgCefMainToolBar()
{
}

void CDlgCefMainToolBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgCefMainToolBar, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_CEFTOOLBAR_CLOSE, &CDlgCefMainToolBar::OnBnClickedButtonCeftoolbarClose)
	ON_BN_CLICKED(IDC_BUTTON_CEFTOOLBAR_HIDE, &CDlgCefMainToolBar::OnBnClickedButtonCeftoolbarHide)
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_BUTTON_CEFTOOLBAR_ADD, &CDlgCefMainToolBar::OnBnClickedButtonCeftoolbarAdd)
END_MESSAGE_MAP()


// CDlgCefMainToolBar message handlers
void CDlgCefMainToolBar::showImg()
{
	CRect rect;
	GetClientRect(&rect);

	//根据影像名称创建控件，并显示影像
	int ctrlNum = 0;
	ctrlNum = 1;

	for (int i = 0; i < ctrlNum; i++)
	{
		//子控件的范围
		double left = rect.left + i * rect.Width() / ctrlNum;
		double right = left + rect.Width() / ctrlNum;
		double top = rect.top;
		double bottom = rect.bottom;

		//创建控件
		CRect childRect(left + 3, top + 5, right - 20, bottom - 3);

		CefWindowInfo window_info;
		window_info.SetAsChild(this->GetSafeHwnd(), childRect);

		CefRefPtr<SimpleHandler> handler = getInstance();

		CefBrowserSettings browser_settings;
		bool bSuccess = CefBrowserHost::CreateBrowser(window_info, handler.get(), URL, browser_settings, NULL);

		if (!bSuccess)
		{
			AfxMessageBox(_T("无法启动浏览器"));
		}
	}
}

BOOL CDlgCefMainToolBar::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	pChild = new CDlgCefMinToolBar();
	pChild->Create(IDD_DIALOG_CEFTOOLBAR_MIN);
	pChild->parent = (CObject*)this;
	pChild->ShowWindow(SW_HIDE);

	SetBackgroundColor(RGB(50, 50, 50));
	SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) ^ 0x80000);
	SetLayeredWindowAttributes(RGB(253, 254, 255), 100, LWA_COLORKEY);
	showImg();

	RECT r;
	GetWindowRect(&r);
	dlgHeight = r.bottom - r.top;
	dlgWidth = r.right - r.left;
	int nMaxX = GetSystemMetrics(SM_CXSCREEN);
	int nMaxY = GetSystemMetrics(SM_CYSCREEN);

	r.bottom = nMaxY - 80;
	r.top = r.bottom - dlgHeight;
	r.left = (nMaxX - dlgWidth) / 2.0;
	r.right = r.left + dlgWidth;
	MoveWindow(&r);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

CefRefPtr<SimpleHandler>& CDlgCefMainToolBar::getInstance()
{
	static CefRefPtr<SimpleHandler> handler(new SimpleHandler());
	return handler;
}

void CDlgCefMainToolBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnLButtonDown(nFlags, point);
	PostMessage(WM_NCLBUTTONDOWN,
		HTCAPTION,
		MAKELPARAM(point.x, point.y));
}


void CDlgCefMainToolBar::OnBnClickedButtonCeftoolbarClose()
{
	// TODO: Add your control notification handler code here
	close();
}

void CDlgCefMainToolBar::close()
{
	if (pChild)
		pChild->DestroyWindow();
	DestroyWindow();
}


void CDlgCefMainToolBar::OnBnClickedButtonCeftoolbarHide()
{
	// TODO: Add your control notification handler code here
	ShowWindow(SW_HIDE);
	pChild->ShowWindow(SW_SHOW);
}


void CDlgCefMainToolBar::OnSize(UINT nType, int cx, int cy)
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
	// TODO: Add your message handler code here
}


void CDlgCefMainToolBar::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanging(lpwndpos);

	// TODO: Add your message handler code here
	// 获取屏幕宽度  
	int nMaxX = GetSystemMetrics(SM_CXSCREEN);
	// 获取屏幕高度  
	int nMaxY = GetSystemMetrics(SM_CYSCREEN);

	// 判断窗口X坐标有无超过左边桌面。  
	if (lpwndpos->x < 0)
	{
		lpwndpos->x = 0;
	}

	// 判断窗口X坐标有无超过右边桌面。  
	if (lpwndpos->x + lpwndpos->cx > nMaxX)
	{
		lpwndpos->x = nMaxX - lpwndpos->cx;
	}

	// 判断窗口Y坐标有无超过顶部桌面。  
	if (lpwndpos->y < 0)
	{
		lpwndpos->y = 0;
	}

	// 判断窗口Y坐标有无超过底部桌面。  
	if (lpwndpos->y + lpwndpos->cy > nMaxY)
	{
		lpwndpos->y = nMaxY - lpwndpos->cy;
	}
	// TODO: Add your message handler code here
}


void CDlgCefMainToolBar::OnBnClickedButtonCeftoolbarAdd()
{
	// TODO: Add your control notification handler code here
	std::string path = GetModuleFolder();
	path += "\\index2.html";
	CDlgCefToolBox* dlg = new CDlgCefToolBox(path.c_str());
	dlg->Create(IDD_DIALOG_CEFTOOLBOX);
	dlg->ShowWindow(SW_SHOW);
}
