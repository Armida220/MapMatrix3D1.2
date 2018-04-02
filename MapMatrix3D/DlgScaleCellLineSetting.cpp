// DlgScaleCellLineSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgScaleCellLineSetting.h"
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
// CDlgScaleCellLineSetting dialog


CDlgScaleCellLineSetting::CDlgScaleCellLineSetting(CWnd* pParent /*=NULL*/)
	: CSonDialog(CDlgScaleCellLineSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgScaleCellLineSetting)
	m_strCellDefName = _T("");
	m_fBaseXOffset = 0.0f;
	m_fBaseYOffset = 0.0f;
	m_fCycle = 0.0f;
	m_fkx = 0.0f;
	m_fWidth = 0.0f;
	m_fAssistYOffset = 0.0f;
	//}}AFX_DATA_INIT
	m_pCellLine = NULL;
}


void CDlgScaleCellLineSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScaleCellLineSetting)
	DDX_Text(pDX, IDC_CELLDEFNAME_EDIT, m_strCellDefName);
	DDX_Text_Silence(pDX, IDC_BASEXOFFSET_EDIT, m_fBaseXOffset);
	DDX_Text_Silence(pDX, IDC_BASEYOFFSET_EDIT, m_fBaseYOffset);
	DDX_Text_Silence(pDX, IDC_CYCLE_EDIT, m_fCycle);
	DDX_Text_Silence(pDX, IDC_KX_EDIT, m_fkx);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	DDX_Text_Silence(pDX, IDC_ASSISTYOFFSET_EDIT, m_fAssistYOffset);
	//}}AFX_DATA_MAP
}

BOOL CDlgScaleCellLineSetting::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgScaleCellLineSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgScaleCellLineSetting)
	ON_EN_CHANGE(IDC_CELLDEFNAME_EDIT, OnChangeCelldefnameEdit)
	ON_EN_CHANGE(IDC_CYCLE_EDIT, OnChangeCycleEdit)
	ON_EN_CHANGE(IDC_BASEXOFFSET_EDIT, OnChangeBasexoffsetEdit)
	ON_EN_CHANGE(IDC_BASEYOFFSET_EDIT, OnChangeBaseyoffsetEdit)
	ON_EN_CHANGE(IDC_ASSISTYOFFSET_EDIT, OnChangeAssistyoffsetEdit)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_KX_EDIT, OnChangeKxEdit)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleCellLineSetting message handlers

void CDlgScaleCellLineSetting::OnChangeCelldefnameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellLine == NULL)
		return;
	
	m_pCellLine->m_strCellDefName = m_strCellDefName;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleCellLineSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	if(m_pCellLine == NULL)
		return;
	
	m_pCellLine->m_fWidth = m_fWidth;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleCellLineSetting::OnChangeCycleEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellLine == NULL)
		return;
	
	m_pCellLine->m_fCycle = m_fCycle;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleCellLineSetting::OnChangeKxEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellLine == NULL)
		return;
	
	m_pCellLine->m_fkx = m_fkx;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleCellLineSetting::OnChangeKyEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellLine == NULL)
		return;
		
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}


void CDlgScaleCellLineSetting::OnChangeAngleEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellLine == NULL)
		return;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleCellLineSetting::OnChangeBasexoffsetEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellLine == NULL)
		return;
	
	m_pCellLine->m_fBaseXOffset = m_fBaseXOffset;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleCellLineSetting::OnChangeBaseyoffsetEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellLine == NULL)
		return;
	
	m_pCellLine->m_fBaseYOffset = m_fBaseYOffset;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleCellLineSetting::OnChangeAssistyoffsetEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellLine == NULL)
		return;
	
	m_pCellLine->m_fAssistYOffset = m_fAssistYOffset;	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleCellLineSetting::OnPreviewButton() 
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
	m_pCellLine->m_strCellDefName = m_strCellDefName;
	
	UpdateData(FALSE);
	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
	return;
}

void CDlgScaleCellLineSetting::SetMem(CScaleCellLinetype *pCellline, ConfigLibItem config)
{
	m_pCellLine = pCellline;
	m_config = config;
	
	if(m_pCellLine==NULL || m_config.pCellDefLib==NULL)
		return;
	
	m_fBaseXOffset = m_pCellLine->m_fBaseXOffset;
	m_fBaseYOffset = m_pCellLine->m_fBaseYOffset;
	m_fCycle = m_pCellLine->m_fCycle;
	m_fWidth = m_pCellLine->m_fWidth;
	m_fkx = m_pCellLine->m_fkx;
	m_strCellDefName = m_pCellLine->m_strCellDefName;
	m_fAssistYOffset = pCellline->m_fAssistYOffset;
}

