// DlgChangeFCode.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgChangeFCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChangeFCode dialog

BOOL CDlgChangeFCode::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
    set_FCode();
	//
	return TRUE;
}
CDlgChangeFCode::CDlgChangeFCode(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChangeFCode::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChangeFCode)
	m_strSrcFCode = _T("");
	m_strTarFCode = _T("");
	str_fcode=_T("");
	can_edit=TRUE;
	//}}AFX_DATA_INIT
}


void CDlgChangeFCode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChangeFCode)
	DDX_Text(pDX, IDC_EDIT_FCODE, m_strSrcFCode);
	DDX_Text(pDX, IDC_EDIT_FCODE2, m_strTarFCode);
	//}}AFX_DATA_MAP
}

void CDlgChangeFCode::set_FCode()
{
	GetDlgItem(IDC_EDIT_FCODE)->SetWindowText((LPCTSTR)str_fcode);
	if(!can_edit)
	{
		((CEdit *)GetDlgItem(IDC_EDIT_FCODE))->EnableWindow(FALSE);
	}
	else
	{
        ((CEdit *)GetDlgItem(IDC_EDIT_FCODE))->EnableWindow(TRUE);
	}
}



BEGIN_MESSAGE_MAP(CDlgChangeFCode, CDialog)
	//{{AFX_MSG_MAP(CDlgChangeFCode)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChangeFCode message handlers
