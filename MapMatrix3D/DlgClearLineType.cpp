// DlgClearLineType.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgClearLineType.h"
#include "DlgCellDefLinetypeView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgClearLineType dialog


CDlgClearLineType::CDlgClearLineType(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgClearLineType::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgClearLineType)
	
	m_bOverZero = TRUE;
	//}}AFX_DATA_INIT
}


void CDlgClearLineType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgClearLineType)
	
	DDX_Check(pDX, IDC_CHECK_OVERZERO, m_bOverZero);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgClearLineType, CDialog)
	//{{AFX_MSG_MAP(CDlgClearLineType)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, OnButtonPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgClearLineType message handlers

void CDlgClearLineType::OnButtonPreview() 
{
	// TODO: Add your control notification handler code here
	CDlgCellDefLinetypeView dlg;
	dlg.SetShowMode(FALSE, TRUE, 1, "", TRUE);
	dlg.SetConfig(m_config);
	if (IDOK==dlg.DoModal())
	{

		return;
	}
}

void CDlgClearLineType::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
