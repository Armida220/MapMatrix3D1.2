// DlgTidalWaterSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgTidalWaterSetting.h"
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
// CDlgTidalWaterSetting dialog


CDlgTidalWaterSetting::CDlgTidalWaterSetting(CWnd* pParent /*=NULL*/)
	: CSonDialog(CDlgTidalWaterSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTidalWaterSetting)
	m_strCellDefName = _T("");
	m_fkxMax = 1.0f;
	m_fkxMin = 1.0f;
	m_fWidth = 0.0f;
	m_fInterval = 0.0f;
	//}}AFX_DATA_INIT
	m_pSymbol = NULL;
}


void CDlgTidalWaterSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTidalWaterSetting)
	DDX_Text(pDX, IDC_CELLDEFNAME_EDIT, m_strCellDefName);
	DDX_Text_Silence(pDX, IDC_EDIT_KXMIN, m_fkxMin);
	DDX_Text_Silence(pDX, IDC_EDIT_KXMAX, m_fkxMax);
	DDX_Text_Silence(pDX, IDC_EDIT_INTERVAL, m_fInterval);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	//}}AFX_DATA_MAP
}

BOOL CDlgTidalWaterSetting::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgTidalWaterSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgTidalWaterSetting)
	ON_EN_CHANGE(IDC_CELLDEFNAME_EDIT, OnChangeCelldefnameEdit)
	ON_EN_CHANGE(IDC_EDIT_KXMIN, OnChangeKxmin)
	ON_EN_CHANGE(IDC_EDIT_KXMAX, OnChangeKxmax)
	ON_EN_CHANGE(IDC_EDIT_INTERVAL, OnChangeInterval)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTidalWaterSetting message handlers

void CDlgTidalWaterSetting::OnChangeCelldefnameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pSymbol == NULL)
		return;
	
	m_pSymbol->m_strCellDefName = m_strCellDefName;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgTidalWaterSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	if(m_pSymbol == NULL)
		return;
	
	m_pSymbol->m_fWidth = m_fWidth;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgTidalWaterSetting::OnChangeKxmin() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pSymbol == NULL)
		return;
	
	m_pSymbol->m_fkxMin = m_fkxMin;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgTidalWaterSetting::OnChangeKxmax() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pSymbol == NULL)
		return;
	
	m_pSymbol->m_fkxMax = m_fkxMax;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgTidalWaterSetting::OnChangeInterval() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pSymbol == NULL)
		return;
	
	m_pSymbol->m_fInterval = m_fInterval;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}


void CDlgTidalWaterSetting::OnPreviewButton() 
{
	// TODO: Add your control notification handler code here
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT,-1);

	CDlgCellDefLinetypeView dlg;
	
	CSwitchScale scale(m_config.GetScale());
//	dlg.SetCellDefLib(m_pCellLib);
	dlg.SetConfig(m_config);
	dlg.SetShowMode(FALSE,TRUE,0,m_strCellDefName);
	
	if(IDOK != dlg.DoModal())
	{
		pWnd->SendMessage(WM_SYMBOL_REPAINT,-2);
		return;
	}
	
	m_strCellDefName = dlg.GetName();
	m_pSymbol->m_strCellDefName = m_strCellDefName;
	
	UpdateData(FALSE);
	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
	return;
}

void CDlgTidalWaterSetting::SetMem(CTidalWaterSymbol *pSymbol, ConfigLibItem config)
{
	m_pSymbol = pSymbol;
	m_config = config;
	
	if(m_pSymbol==NULL || m_config.pCellDefLib==NULL)
		return;		
	
	m_strCellDefName = m_pSymbol->m_strCellDefName;
	m_fkxMax = m_pSymbol->m_fkxMax;
	m_fkxMin = m_pSymbol->m_fkxMin;
	m_fInterval = m_pSymbol->m_fInterval;
	m_fWidth = m_pSymbol->m_fWidth;
}

