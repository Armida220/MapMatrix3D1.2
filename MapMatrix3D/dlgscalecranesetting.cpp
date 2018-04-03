// dlgscaleturnplatesetting.cpp : implementation file
//

#include "stdafx.h"
#include "Editbase.h"
#include "dlgscalecranesetting.h"
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


CDlgScaleCraneSetting::CDlgScaleCraneSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgScaleCraneSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgScaleCraneSetting)
	m_strName = _T("");
	m_fWidth = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgScaleCraneSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScaleCraneSetting)
	DDX_Text(pDX, IDC_LINETYPENAME_EDIT, m_strName);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgScaleCraneSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgScaleTurnplateSetting)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_LINETYPENAME_EDIT, OnChangeLinetypenameEdit)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleTurnplateSetting message handlers

void CDlgScaleCraneSetting::OnPreviewButton() 
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

void CDlgScaleCraneSetting::OnChangeLinetypenameEdit() 
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

void CDlgScaleCraneSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_fWidth = m_fWidth;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	// TODO: Add your control notification handler code here
	
}

void CDlgScaleCraneSetting::SetMem(CScaleCranetype *pScale, ConfigLibItem config)
{
	m_pScale = pScale;
	// 	m_pBaseLib = pLib;
	m_config = config;
	
	if(m_pScale==NULL || config.pLinetypeLib==NULL)
		return;
	
	m_fWidth = m_pScale->m_fWidth;
	m_strName = m_pScale->m_strBaseLinetypeName;
	
	//	UpdateData(FALSE);
}
