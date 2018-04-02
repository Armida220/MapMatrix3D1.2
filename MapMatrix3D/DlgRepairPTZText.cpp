// DlgRepairPTZText.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgRepairPTZText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRepairPTZText dialog


CDlgRepairPTZText::CDlgRepairPTZText(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRepairPTZText::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRepairPTZText)
	m_sel = -1;
	//}}AFX_DATA_INIT
}


void CDlgRepairPTZText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRepairPTZText)
	DDX_Control(pDX, IDC_RADIO1, m_AnnoToZ);
	DDX_Radio(pDX, IDC_RADIO1, m_sel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRepairPTZText, CDialog)
	//{{AFX_MSG_MAP(CDlgRepairPTZText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRepairPTZText message handlers

void CDlgRepairPTZText::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL CDlgRepairPTZText::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_AnnoToZ.SetCheck( 1 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
