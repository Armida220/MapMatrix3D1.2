// DlgParalineSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgAngBisectorSetting.h"
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


CDlgAngbisectorSetting::CDlgAngbisectorSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAngbisectorSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgParalineSetting)
	m_fLength = 1.0f;
	m_strLineName = _T("");
	m_fWidth = 0.0f;
	//}}AFX_DATA_INIT
	m_pAng = NULL;
}


void CDlgAngbisectorSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgParalineSetting)
	DDX_Text_Silence(pDX, IDC_LEN_EDIT, m_fLength);
	DDX_Text(pDX, IDC_LINETYPENAME_EDIT, m_strLineName);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAngbisectorSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgParalineSetting)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_LEN_EDIT, OnChangeLenEdit)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	ON_EN_CHANGE(IDC_LINETYPENAME_EDIT, OnChangeLinetypenameEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgParalineSetting message handlers

void CDlgAngbisectorSetting::OnPreviewButton() 
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
	m_pAng->m_strBaseLinetypeName = m_strLineName;
	
	UpdateData(FALSE);
	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
	return;
}

void CDlgAngbisectorSetting::OnChangeLenEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pAng == NULL)
		return;
	
	m_pAng->m_fLength = m_fLength;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgAngbisectorSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pAng == NULL)
		return;
	
	m_pAng->m_fWidth = m_fWidth;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgAngbisectorSetting::SetMem(CAngBisectortype *pAng, ConfigLibItem config)
{
	m_pAng = pAng;
	m_config = config;
	
	if(m_pAng==NULL || m_config.pLinetypeLib==NULL)
		return;
	
	m_fLength = m_pAng->m_fLength;
	m_fWidth = m_pAng->m_fWidth;
	m_strLineName = m_pAng->m_strBaseLinetypeName;
}

void CDlgAngbisectorSetting::OnChangeLinetypenameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pAng == NULL)
		return;
	
	m_pAng->m_strBaseLinetypeName = m_strLineName;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}
