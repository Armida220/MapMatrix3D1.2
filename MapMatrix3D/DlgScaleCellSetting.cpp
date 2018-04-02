// DlgScaleCellSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgScaleCellSetting.h"
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
// CDlgScaleCellSetting dialog


CDlgScaleCellSetting::CDlgScaleCellSetting(CWnd* pParent /*=NULL*/)
	: CSonDialog(CDlgScaleCellSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgScaleCellSetting)
	m_strCellDefName = _T("");
	m_fX1 = 0.0f;
	m_fX2 = 0.0f;
	m_fY1 = 0.0f;
	m_fWidth = 0.0f;	
	//}}AFX_DATA_INIT
	m_pSym = NULL;
}


void CDlgScaleCellSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScaleCellSetting)
	DDX_Text(pDX, IDC_CELLDEFNAME_EDIT, m_strCellDefName);
	DDX_Text_Silence(pDX, IDC_DX_EDIT, m_fX1);
	DDX_Text_Silence(pDX, IDC_DX_EDIT2, m_fX2);
	DDX_Text_Silence(pDX, IDC_DY_EDIT, m_fY1);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	//}}AFX_DATA_MAP
}

BOOL CDlgScaleCellSetting::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgScaleCellSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgScaleCellSetting)
	ON_EN_CHANGE(IDC_CELLDEFNAME_EDIT, OnChangeCelldefnameEdit)
	ON_EN_CHANGE(IDC_DX_EDIT, OnChangeX1Edit)
	ON_EN_CHANGE(IDC_DX_EDIT2, OnChangeX2Edit)
	ON_EN_CHANGE(IDC_DY_EDIT, OnChangeY1Edit)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleCellSetting message handlers

void CDlgScaleCellSetting::OnChangeCelldefnameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_strCellDefName = m_strCellDefName;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleCellSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fWidth = m_fWidth;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}


void CDlgScaleCellSetting::OnChangeX1Edit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fX1 = m_fX1;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleCellSetting::OnChangeY1Edit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fY1 = m_fY1;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}



void CDlgScaleCellSetting::OnChangeX2Edit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pSym == NULL)
		return;
	
	m_pSym->m_fX2 = m_fX2;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}
void CDlgScaleCellSetting::OnPreviewButton() 
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
	m_pSym->m_strCellDefName = m_strCellDefName;
	
	UpdateData(FALSE);
	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
	return;
}

void CDlgScaleCellSetting::SetMem(CScaleCell *pSym, ConfigLibItem config)
{
	m_pSym = pSym;
	m_config = config;
	
	if(m_pSym==NULL || m_config.pCellDefLib==NULL)
		return;	
	
	m_fWidth = m_pSym->m_fWidth;
	m_fX1 = m_pSym->m_fX1;
	m_fY1 = m_pSym->m_fY1;
	m_fX2 = m_pSym->m_fX2;
	m_strCellDefName = m_pSym->m_strCellDefName;
}
