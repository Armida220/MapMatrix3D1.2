// DlgEditText.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgEditText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEditText dialog


CDlgEditText::CDlgEditText(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditText::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditText)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgEditText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditText)
	DDX_Control(pDX, IDC_EDITTEXT, m_wndEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditText, CDialog)
	//{{AFX_MSG_MAP(CDlgEditText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditText message handlers

void CDlgEditText::OnOK() 
{
	// TODO: Add extra validation here
	
	m_wndEdit.GetWindowText(m_strEdit);
	m_strEdit.Replace("\r\n", "\n");

	CDialog::OnOK();
}

BOOL CDlgEditText::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_strEdit.Replace("\r\n","\n");
	m_strEdit.Replace("\n","\r\n");
	m_wndEdit.SetWindowText(m_strEdit);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CDlgEditText::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message==WM_KEYDOWN )
	{
		if( pMsg->wParam==VK_RETURN && GetKeyState(VK_CONTROL)<0 )
		{
			PostMessage(WM_COMMAND,IDOK,0);
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}