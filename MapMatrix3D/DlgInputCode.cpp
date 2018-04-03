// DlgInputCode.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgInputCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInputCode dialog

static CString s_strCode = "";


CDlgInputCode::CDlgInputCode(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputCode::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInputCode)
	m_strCode = s_strCode;
	//}}AFX_DATA_INIT
}


void CDlgInputCode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInputCode)
	DDX_Text(pDX, IDC_EDIT_CODE, m_strCode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInputCode, CDialog)
	//{{AFX_MSG_MAP(CDlgInputCode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInputCode message handlers

BOOL CDlgInputCode::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgInputCode::OnOK()
{
	UpdateData(TRUE);
	s_strCode = m_strCode;
	CDialog::OnOK();
}


void CDlgInputCode::OnCancel()
{
	CDialog::OnCancel();
}