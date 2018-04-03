// DlgNewGroup.cpp : implementation file
//

#include "stdafx.h"
#include "DlgNewGroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNewGroup dialog


CDlgNewGroup::CDlgNewGroup(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewGroup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewGroup)
	m_strGroupName = _T("");
	//}}AFX_DATA_INIT
}


void CDlgNewGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewGroup)
	DDX_Text(pDX, IDC_GROUP_EDIT, m_strGroupName);
	DDV_MaxChars(pDX, m_strGroupName, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewGroup, CDialog)
	//{{AFX_MSG_MAP(CDlgNewGroup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewGroup message handlers

BOOL CDlgNewGroup::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	if (!m_strTitle.IsEmpty() && !m_strText.IsEmpty())
	{
		SetWindowText(m_strTitle);
		CWnd *pWnd = GetDlgItem(IDC_GROUPNAME_STATIC);
		if (pWnd)
		{
			pWnd->SetWindowText(m_strText);
		}
	}
	
	
	return TRUE;
}

void CDlgNewGroup::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	if(m_strGroupName == "")
	{
		CString msg,cap;
		msg.LoadString(IDS_CANNT_NULL);
		cap.LoadString(IDS_ATTENTION);
		MessageBox(msg,cap,MB_OK|MB_ICONASTERISK);
		return;
	}
	CDialog::OnOK();
}
