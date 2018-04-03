// DlgTrialMsg.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgTrialMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTrialMsg dialog


CDlgTrialMsg::CDlgTrialMsg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTrialMsg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTrialMsg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgTrialMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTrialMsg)
	DDX_Control(pDX, IDOK, m_wndOKBtn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTrialMsg, CDialog)
	//{{AFX_MSG_MAP(CDlgTrialMsg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTrialMsg message handlers

void CDlgTrialMsg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CDlgTrialMsg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CDlgTrialMsg::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent==1 )
	{
		DWORD time1 = GetTickCount();
		CString text;
		text.Format("(%d s)",30-(int)(time1-m_time0)/1000);

		m_wndOKBtn.SetWindowText(m_strBtnText0+text);

		if( time1>m_time0+30000 )
		{
			m_wndOKBtn.EnableWindow(TRUE);
			m_wndOKBtn.SetWindowText(m_strBtnText0);
			KillTimer(1);
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}

BOOL CDlgTrialMsg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_time0 = GetTickCount();
	
// 	SetTimer(1,200,NULL);
// 
// 	m_wndOKBtn.GetWindowText(m_strBtnText0);
// 	m_wndOKBtn.EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
