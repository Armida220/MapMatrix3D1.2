// dlginputstr.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "dlginputstr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInputStr dialog


CDlgInputStr::CDlgInputStr(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputStr::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInputStr)
	m_strInput = _T("");
	//}}AFX_DATA_INIT
	m_strTitle = _T("");
}


void CDlgInputStr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInputStr)
	DDX_Text(pDX, IDC_EDIT_INPUT, m_strInput);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInputStr, CDialog)
	//{{AFX_MSG_MAP(CDlgInputStr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInputStr message handlers

BOOL CDlgInputStr::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(m_strTitle);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
