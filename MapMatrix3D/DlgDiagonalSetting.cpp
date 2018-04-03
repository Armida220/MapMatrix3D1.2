// DlgDiagonalSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgDiagonalSetting.h"
#include "ExMessage.h"
#include "DlgCellDefLinetypeView.h"
#include "SmartViewFunctions.h"
#include "SilenceDDX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDiagonalSetting dialog


CDlgDiagonalSetting::CDlgDiagonalSetting(CWnd* pParent /*=NULL*/)
	: CSonDialog(CDlgDiagonalSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDiagonalSetting)
	m_strLineName = _T("");
	m_fWidth = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgDiagonalSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDiagonalSetting)
	DDX_Text(pDX, IDC_LINETYPENAME_EDIT, m_strLineName);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	DDX_Control(pDX, IDC_DIAGONALTYPE_COMBO, m_cDiagonalType);
	//}}AFX_DATA_MAP
}

BOOL CDlgDiagonalSetting::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_cDiagonalType.AddString(StrFromResID(IDS_DIAGONALTYPE_COMMON));
	m_cDiagonalType.AddString(StrFromResID(IDS_DIAGONALTYPE_ELECTRICALWIRE));
	
	m_cDiagonalType.SetCurSel(m_pDiagonal->m_nDiagonalType);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgDiagonalSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgDiagonalSetting)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_LINETYPENAME_EDIT, OnChangeLinetypenameEdit)
	ON_CBN_SELCHANGE(IDC_DIAGONALTYPE_COMBO, OnSelchangeDiagonaltypeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDiagonalSetting message handlers


void CDlgDiagonalSetting::OnChangeWidthEdit() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pDiagonal == NULL)
		return;
	
	m_pDiagonal->m_fWidth = m_fWidth;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgDiagonalSetting::OnChangeLinetypenameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pDiagonal == NULL)
		return;
	
	m_pDiagonal->m_strBaseLinetypeName = m_strLineName;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgDiagonalSetting::OnPreviewButton() 
{
	// TODO: Add your control notification handler code here
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT,-1);
	
	CDlgCellDefLinetypeView dlg;
	
	CSwitchScale scale(m_config.GetScale());
	
	//	dlg.SetCellDefLib(m_pCellLib);
	dlg.SetConfig(m_config);
	dlg.SetShowMode(FALSE,TRUE,1,m_strLineName);
	
	if(IDOK != dlg.DoModal())
	{
		pWnd->SendMessage(WM_SYMBOL_REPAINT,-2);
		return;
	}
	
	m_strLineName = dlg.GetName();
	m_pDiagonal->m_strBaseLinetypeName = m_strLineName;
	
	UpdateData(FALSE);
	
	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
	return;
	
}

void CDlgDiagonalSetting::SetMem(CDiagonal *pDiagonal, ConfigLibItem config)
{
	m_pDiagonal = pDiagonal;
	m_config = config;
	
	if(m_config.pCellDefLib==NULL || m_config.pCellDefLib==NULL)
		return;
	
	m_fWidth = pDiagonal->m_fWidth;
	m_strLineName = pDiagonal->m_strBaseLinetypeName;
	
	//	UpdateData(FALSE);
}

void CDlgDiagonalSetting::OnSelchangeDiagonaltypeCombo()
{
	if(m_pDiagonal == NULL)
		return;
	
	m_pDiagonal->m_nDiagonalType = m_cDiagonalType.GetCurSel();
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}
