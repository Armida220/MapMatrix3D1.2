// DlgInputName.cpp : implementation file
//

#include "stdafx.h"
#include "editBase.h"
#include "DlgInputName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInputName dialog


CDlgInputName::CDlgInputName(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInputName)
	m_strName = _T("");
	//}}AFX_DATA_INIT
}


void CDlgInputName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInputName)
	DDX_Text(pDX, IDC_EDIT_INPUT, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInputName, CDialog)
	//{{AFX_MSG_MAP(CDlgInputName)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInputName message handlers
