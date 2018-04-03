// DlgCefMinToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "DlgCefMinToolBar.h"
#include "afxdialogex.h"
#include "DlgCefMainToolBar.h"

// CDlgCefMinToolBar dialog

IMPLEMENT_DYNAMIC(CDlgCefMinToolBar, CDialogEx)

CDlgCefMinToolBar::CDlgCefMinToolBar(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCefMinToolBar::IDD, pParent)
{

}

CDlgCefMinToolBar::~CDlgCefMinToolBar()
{
}

void CDlgCefMinToolBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgCefMinToolBar, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CEFMINTOOLBAR_CLOSE, &CDlgCefMinToolBar::OnBnClickedButtonCefmintoolbarClose)
	ON_BN_CLICKED(IDC_BUTTON_CEFTOOLBAR_MAX, &CDlgCefMinToolBar::OnBnClickedButtonCeftoolbarMax)
END_MESSAGE_MAP()


// CDlgCefMinToolBar message handlers


BOOL CDlgCefMinToolBar::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	SetBackgroundColor(RGB(50, 50, 50));
	RECT r;
	GetWindowRect(&r);
	int width = r.right - r.left;
	int height = r.bottom - r.top;
	r.left = 0;
	r.right = width;
	r.bottom = GetSystemMetrics(SM_CYSCREEN) - 80;
	r.top = r.bottom - height;
	MoveWindow(&r);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgCefMinToolBar::OnBnClickedButtonCefmintoolbarClose()
{
	// TODO: Add your control notification handler code here
	CDlgCefMainToolBar* dlg = (CDlgCefMainToolBar*)parent;
	dlg->close();
}


void CDlgCefMinToolBar::OnBnClickedButtonCeftoolbarMax()
{
	// TODO: Add your control notification handler code here
	CDlgCefMainToolBar* dlg = (CDlgCefMainToolBar*)parent;
	dlg->ShowWindow(SW_SHOW);
	ShowWindow(SW_HIDE);
}
