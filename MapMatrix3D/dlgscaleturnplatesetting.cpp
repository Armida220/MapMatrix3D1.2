// dlgscaleturnplatesetting.cpp : implementation file
//

#include "stdafx.h"
#include "Editbase.h"
#include "dlgscaleturnplatesetting.h"
#include "SilenceDDX.h"
#include "ExMessage.h"
#include "DlgCellDefLinetypeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleTurnplateSetting dialog


CDlgScaleTurnplateSetting::CDlgScaleTurnplateSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgScaleTurnplateSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgScaleTurnplateSetting)
	m_fRoadWid = 0.0f;
	m_strName = _T("");
	m_fWidth = 0.0f;
	m_bSingleLine = FALSE;
	m_fStickupLen = 0;
	m_fSideLineWidth = 0;
	//}}AFX_DATA_INIT
}


void CDlgScaleTurnplateSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScaleTurnplateSetting)
	DDX_Text_Silence(pDX, IDC_LEN_EDIT, m_fRoadWid);
	DDX_Text(pDX, IDC_LINETYPENAME_EDIT, m_strName);
	DDX_Text_Silence(pDX, IDC_SIDELINE_WIDTH, m_fSideLineWidth);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	DDX_Text_Silence(pDX, IDC_STICKUP_LEN, m_fStickupLen);
	DDX_Check(pDX,IDC_CHECK_USESINGLELINE,m_bSingleLine);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgScaleTurnplateSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgScaleTurnplateSetting)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_LEN_EDIT, OnChangeLenEdit)
	ON_EN_CHANGE(IDC_LINETYPENAME_EDIT, OnChangeLinetypenameEdit)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	ON_EN_CHANGE(IDC_STICKUP_LEN, OnChangeStickupLen)
	ON_EN_CHANGE(IDC_SIDELINE_WIDTH, OnChangeSideLineEdit)
	ON_BN_CLICKED(IDC_CHECK_USESINGLELINE, OnCheckSingleLine)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleTurnplateSetting message handlers

void CDlgScaleTurnplateSetting::OnPreviewButton() 
{
	// TODO: Add your control notification handler code here
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT,-1);
	
	CDlgCellDefLinetypeView dlg;
	CSwitchScale scale(m_config.GetScale());
	// 	dlg.SetLineTypeLib(m_pBaseLib);
	dlg.SetShowMode(FALSE,TRUE,1,m_strName);
	// 	dlg.SetScale(m_dScale);
	dlg.SetConfig(m_config);
	
	if(IDOK != dlg.DoModal())
	{
		pWnd->SendMessage(WM_SYMBOL_REPAINT,-2);
		return;
	}
	
	m_strName = dlg.GetName();
	m_pScale->m_strBaseLinetypeName = m_strName;
	
	UpdateData(FALSE);
	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgScaleTurnplateSetting::OnChangeLenEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_fRoadWidth = m_fRoadWid;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	// TODO: Add your control notification handler code here
	
}

void CDlgScaleTurnplateSetting::OnChangeLinetypenameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_strBaseLinetypeName = m_strName;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	// TODO: Add your control notification handler code here
	
}

void CDlgScaleTurnplateSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_fWidth = m_fWidth;
	m_pScale->m_bSingleLine = m_bSingleLine;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	// TODO: Add your control notification handler code here
	
}


void CDlgScaleTurnplateSetting::OnCheckSingleLine() 
{
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_bSingleLine = m_bSingleLine;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	// TODO: Add your control notification handler code here
	
}

void CDlgScaleTurnplateSetting::OnChangeStickupLen() 
{
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_fStickupLen = m_fStickupLen;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	// TODO: Add your control notification handler code here
	
}

void CDlgScaleTurnplateSetting::OnChangeSideLineEdit() 
{
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_fSideLineWidth = m_fSideLineWidth;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	// TODO: Add your control notification handler code here
	
}


void CDlgScaleTurnplateSetting::SetMem(CScaleTurnplatetype *pScale, ConfigLibItem config)
{
	m_pScale = pScale;
	// 	m_pBaseLib = pLib;
	m_config = config;
	
	if(m_pScale==NULL || config.pLinetypeLib==NULL)
		return;
	
	m_fWidth = m_pScale->m_fWidth;
	m_fRoadWid = m_pScale->m_fRoadWidth;
	m_strName = m_pScale->m_strBaseLinetypeName;
	m_bSingleLine = m_pScale->m_bSingleLine;
	m_fStickupLen = m_pScale->m_fStickupLen;
	m_fSideLineWidth = m_pScale->m_fSideLineWidth;
	//	UpdateData(FALSE);
}
