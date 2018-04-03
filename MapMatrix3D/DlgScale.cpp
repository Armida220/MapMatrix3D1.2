// DlgScale.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgScale.h"
#include "SymbolLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgScale dialog


CDlgScale::CDlgScale(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgScale::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgScale)
	m_nScale = 0;
	//}}AFX_DATA_INIT
}


void CDlgScale::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScale)
	DDX_Text(pDX, IDC_SCALE_EDIT, m_nScale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgScale, CDialog)
	//{{AFX_MSG_MAP(CDlgScale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScale message handlers

void CDlgScale::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	if(pApp == NULL)
		return;
	CConfigLibManager *pConfigManager = pApp->GetConfigLibManager();
	if(pConfigManager == NULL)
		return;
	
	for(int i=0; i<pConfigManager->GetConfigLibCount(); i++)
	{
		if(m_nScale == pConfigManager->GetConfigLibItem(i).GetScale())
		{
			CString msg,cap;
			msg.LoadString(IDS_SCALE_EXIST);
			cap.LoadString(IDS_ATTENTION);
			MessageBox(msg,cap,MB_OK|MB_ICONASTERISK);
			return;
		}
	}
	if(m_nScale <= 0)
	{
		CString msg,cap;
		msg.LoadString(IDS_CANNOT_NEGATIVE);
		cap.LoadString(IDS_ATTENTION);
		MessageBox(msg,cap,MB_OK|MB_ICONASTERISK);
		return;
	}
	CDialog::OnOK();
}
