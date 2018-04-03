// DeleteCommonSideDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DeleteCommonSideDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteCommonSideDlg dialog


CDeleteCommonSideDlg::CDeleteCommonSideDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteCommonSideDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeleteCommonSideDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDeleteCommonSideDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteCommonSideDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteCommonSideDlg, CDialog)
	//{{AFX_MSG_MAP(CDeleteCommonSideDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteCommonSideDlg message handlers

void CDeleteCommonSideDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	m_side = 0;
	CDialog::OnOK();
}

void CDeleteCommonSideDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	m_side = 1;
	CDialog::OnOK();
}

BOOL CDeleteCommonSideDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	GetDlgItem(IDC_BUTTON1)->SetWindowText(m_layname1);
	GetDlgItem(IDC_BUTTON2)->SetWindowText(m_layname2);
	m_side = -1;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
