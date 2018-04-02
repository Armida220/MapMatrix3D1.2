// DlgOpSettings.cpp : implementation file
//

#include "stdafx.h"
#include "editBase.h"
#include "DlgOpSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOpSettings dialog


CDlgOpSettings::CDlgOpSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOpSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOpSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_param = NULL;
}


CDlgOpSettings::CDlgOpSettings(UINT nIDTemplate, CWnd* pParent)
	: CDialog(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(CDlgOpSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_param = NULL;
}


void CDlgOpSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOpSettings)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOpSettings, CDialog)
	//{{AFX_MSG_MAP(CDlgOpSettings)	
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOpSettings message handlers


BOOL CDlgOpSettings::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWnd *pWnd = GetDlgItem(IDC_STATIC_RECT);
	if( pWnd )
	{
		CRect rcPos;
		pWnd->GetWindowRect(rcPos);
		
		ScreenToClient(&rcPos);
		
		if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, rcPos, this, 2))
		{
		}
		
		m_wndPropList.SetWindowPos(NULL,rcPos.left,rcPos.top,
			rcPos.Width(),rcPos.Height(),0);
		
		m_wndPropList.EnableHeaderCtrl(FALSE);

		m_wndPropList.LoadParams(m_param);
		m_wndPropList.ShowParams(m_param);
	}

	return TRUE;
}


void CDlgOpSettings::OnOK()
{
	m_tab.DelAll();
	m_tab.BeginAddValueItem();
	m_wndPropList.GetCmdParams(m_tab);
	m_tab.EndAddValueItem();
	CDialog::OnOK();
}

void CDlgOpSettings::GetValueTable(CValueTable& tab)
{
	tab.CopyFrom(m_tab);
}

