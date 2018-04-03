// DlgParalineSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgParalineSetting.h"
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
// CDlgParalineSetting dialog


CDlgParalineSetting::CDlgParalineSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgParalineSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgParalineSetting)
	m_fIntv = 0.0f;
	m_strLineName = _T("");
	m_fWidth = 0.0f;
	//}}AFX_DATA_INIT
	m_pPara = NULL;
}


void CDlgParalineSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgParalineSetting)
	DDX_Text_Silence(pDX, IDC_INTV_EDIT, m_fIntv);
	DDX_Text(pDX, IDC_LINETYPENAME_EDIT, m_strLineName);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgParalineSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgParalineSetting)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_INTV_EDIT, OnChangeIntvEdit)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	ON_EN_CHANGE(IDC_LINETYPENAME_EDIT, OnChangeLinetypenameEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgParalineSetting message handlers

void CDlgParalineSetting::OnPreviewButton() 
{
	// TODO: Add your control notification handler code here
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT,-1);
	
	CDlgCellDefLinetypeView dlg;
	
	CSwitchScale scale(m_config.GetScale());
	// 	dlg.SetLineTypeLib(m_pBaseLib);
	dlg.SetShowMode(FALSE,TRUE,1,m_strLineName);
	// 	dlg.SetScale(m_dScale);
	dlg.SetConfig(m_config);
	
	if(IDOK != dlg.DoModal())
	{
		pWnd->SendMessage(WM_SYMBOL_REPAINT,-2);
		return;
	}
	
	m_strLineName = dlg.GetName();
	m_pPara->m_strBaseLinetypeName = m_strLineName;
	
	UpdateData(FALSE);
	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
	return;
}

void CDlgParalineSetting::OnChangeIntvEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pPara == NULL)
		return;
	
	m_pPara->m_fIntv = m_fIntv;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgParalineSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pPara == NULL)
		return;
	
	m_pPara->m_fWidth = m_fWidth;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgParalineSetting::SetMem(CParaLinetype *pPara, ConfigLibItem config)
{
	m_pPara = pPara;
	m_config = config;
	
	if(m_pPara==NULL || m_config.pLinetypeLib==NULL)
		return;
	
	m_fIntv = m_pPara->m_fIntv;
	m_fWidth = m_pPara->m_fWidth;
	m_strLineName = pPara->m_strBaseLinetypeName;
}

void CDlgParalineSetting::OnChangeLinetypenameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pPara == NULL)
		return;
	
	m_pPara->m_strBaseLinetypeName = m_strLineName;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}
