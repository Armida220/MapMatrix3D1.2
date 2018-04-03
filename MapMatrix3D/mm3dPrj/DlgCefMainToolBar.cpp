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

	//����Ӱ�����ƴ����ؼ�������ʾӰ��
	int ctrlNum = 0;
	ctrlNum = 1;

	for (int i = 0; i < ctrlNum; i++)
	{
		//�ӿؼ��ķ�Χ
		double left = rect.left + i * rect.Width() / ctrlNum;
		double right = left + rect.Width() / ctrlNum;
		double top = rect.top;
		double bottom = rect.bottom;

		//�����ؼ�
		CRect childRect(left + 3, top + 5, right - 20, bottom - 3);

		CefWindowInfo window_info;
		window_info.SetAsChild(this->GetSafeHwnd(), childRect);

		CefRefPtr<SimpleHandler> handler = getInstance();

		CefBrowserSettings browser_settings;
		bool bSuccess = CefBrowserHost::CreateBrowser(window_info, handler.get(), URL, browser_settings, NULL);

		if (!bSuccess)
		{
			AfxMessageBox(_T("�޷����������"));
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

		//�ӿؼ��ķ�Χ
		double left = 0 + num * cx / listBrowser.size();
		double right = left + cx / listBrowser.size();
		double top = 0;
		double bottom = cy;

		//�ƶ��ؼ���
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
	// ��ȡ��Ļ���  
	int nMaxX = GetSystemMetrics(SM_CXSCREEN);
	// ��ȡ��Ļ�߶�  
	int nMaxY = GetSystemMetrics(SM_CYSCREEN);

	// �жϴ���X�������޳���������档  
	if (lpwndpos->x < 0)
	{
		lpwndpos->x = 0;
	}

	// �жϴ���X�������޳����ұ����档  
	if (lpwndpos->x + lpwndpos->cx > nMaxX)
	{
		lpwndpos->x = nMaxX - lpwndpos->cx;
	}

	// �жϴ���Y�������޳����������档  
	if (lpwndpos->y < 0)
	{
		lpwndpos->y = 0;
	}

	// �жϴ���Y�������޳����ײ����档  
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
