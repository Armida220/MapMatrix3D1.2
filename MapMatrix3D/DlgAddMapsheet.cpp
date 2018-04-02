// DlgAddMapsheet.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgAddMapsheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAddMapsheet dialog


CDlgAddMapsheet::CDlgAddMapsheet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddMapsheet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddMapsheet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgAddMapsheet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddMapsheet)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddMapsheet, CDialog)
	//{{AFX_MSG_MAP(CDlgAddMapsheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddMapsheet message handlers

void CDlgAddMapsheet::OnOK() 
{
	// TODO: Add extra validation here

	GetDlgItem(IDC_EDIT_MAPNAME)->GetWindowText(m_mapsheet_name);

	if (m_mapsheet_name.IsEmpty())
	{
		AfxMessageBox(StrFromResID(IDS_CANNT_EMPTY));
		::SetFocus(GetDlgItem(IDC_EDIT_MAPNAME)->m_hWnd);
		return;
	}

	CDialog::OnOK();
}
