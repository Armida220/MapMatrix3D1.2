// DlgCefToolBox.cpp : implementation file
//

#include "stdafx.h"
#include "DlgCefToolBox.h"
#include "afxdialogex.h"


// CDlgCefToolBox dialog

IMPLEMENT_DYNAMIC(CDlgCefToolBox, CDialogEx)

CDlgCefToolBox::CDlgCefToolBox(std::string url,CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCefToolBox::IDD, pParent)
{
	URL = url;
}

CDlgCefToolBox::~CDlgCefToolBox()
{
}

void CDlgCefToolBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgCefToolBox, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CEFTOOLBOX_CLOSE, &CDlgCefToolBox::OnBnClickedButtonCeftoolboxClose)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CDlgCefToolBox message handlers
void CDlgCefToolBox::showImg()
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
		CRect childRect(left + 3, top + 25, right - 3, bottom - 10);

		CefWindowInfo window_info;
		window_info.SetAsChild(this->GetSafeHwnd(), childRect);

		CefRefPtr<SimpleHandler> handler = getInstance();
		
		/*CString  strPathName;
		GetModuleFileName(NULL, strPathName.GetBuffer(256), 256);
		strPathName.ReleaseBuffer(256);
		int nPos = strPathName.ReverseFind('\\');
		strPathName = strPathName.Left(nPos + 1);
		USES_CONVERSION;
		std::string spath = W2A(strPathName);

		CefBrowserSettings browser_settings;
		std::string url;
		url = std::string("file://" + spath + "index2.html");*/
		CefBrowserSettings browser_settings;
		bool bSuccess = CefBrowserHost::CreateBrowser(window_info, handler.get(), URL, browser_settings, NULL);

		if (!bSuccess)
		{
			AfxMessageBox(_T("无法启动浏览器"));
		}
	}


}


BOOL CDlgCefToolBox::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	SetBackgroundColor(RGB(50, 50, 50));
	SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) ^ 0x80000);
	SetLayeredWindowAttributes(RGB(253, 254, 255), 100, LWA_COLORKEY);
	showImg();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgCefToolBox::OnBnClickedButtonCeftoolboxClose()
{
	// TODO: Add your control notification handler code here
	DestroyWindow();
}

CefRefPtr<SimpleHandler>& CDlgCefToolBox::getInstance()
{
	static CefRefPtr<SimpleHandler> handler(new SimpleHandler());
	return handler;
}

void CDlgCefToolBox::OnSize(UINT nType, int cx, int cy)
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
}


void CDlgCefToolBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CDialogEx::OnLButtonDown(nFlags, point);
	PostMessage(WM_NCLBUTTONDOWN,
		HTCAPTION,
		MAKELPARAM(point.x, point.y));
}
