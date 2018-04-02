// DlgColorLayout.cpp : implementation file
//

#include "stdafx.h"
#include "DlgColorLayout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgColorLayout dialog


CDlgColorLayout::CDlgColorLayout(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgColorLayout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgColorLayout)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgColorLayout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgColorLayout)
	DDX_Control(pDX, IDC_COLORCOMBOBOX, m_ColorCombobox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgColorLayout, CDialog)
	//{{AFX_MSG_MAP(CDlgColorLayout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgColorLayout message handlers

BOOL CDlgColorLayout::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
