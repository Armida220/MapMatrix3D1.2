// DlgProcSymLTSetting.cpp : implementation file
//

#include "stdafx.h"
#include "Editbase.h"
#include "DlgProcSymLTSetting.h"
#include "SilenceDDX.h"
#include "ExMessage.h"
#include "DlgCellDefLinetypeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProcSymLTSetting dialog


CDlgProcSymLTSetting::CDlgProcSymLTSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcSymLTSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProcSymLTSetting)
	m_strName = _T("");
	m_fWidth = 0.0f;
	//}}AFX_DATA_INIT

	m_pSymbol = NULL;
}


void CDlgProcSymLTSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProcSymLTSetting)
	DDX_Text(pDX, IDC_LINETYPENAME_EDIT, m_strName);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProcSymLTSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgScaleTurnplateSetting)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_LINETYPENAME_EDIT, OnChangeLinetypenameEdit)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleTurnplateSetting message handlers

void CDlgProcSymLTSetting::OnPreviewButton() 
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
	if( m_pSymbol )m_pSymbol->m_strBaseLinetypeName = m_strName;
	
	UpdateData(FALSE);
	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgProcSymLTSetting::OnChangeLinetypenameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if(m_pSymbol == NULL)
		return;
	
	m_pSymbol->m_strBaseLinetypeName = m_strName;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	// TODO: Add your control notification handler code here
	
}

void CDlgProcSymLTSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if(m_pSymbol == NULL)
		return;
	
	m_pSymbol->m_fWidth = m_fWidth;	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	// TODO: Add your control notification handler code here
	
}

void CDlgProcSymLTSetting::SetMem(CProcSymbol_LT *pSym, ConfigLibItem config)
{
	m_pSymbol = pSym;
	m_config = config;
	
	if(m_pSymbol==NULL || config.pLinetypeLib==NULL)
		return;
	
	m_fWidth = m_pSymbol->m_fWidth;
	m_strName = m_pSymbol->m_strBaseLinetypeName;
}




/////////////////////////////////////////////////////////////////////////////
// CDlgScaleYouGuanSetting dialog


CDlgScaleYouGuanSetting::CDlgScaleYouGuanSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgScaleYouGuanSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProcSymLTSetting)
	m_strName = _T("");
	m_fWidth = 0.0f;
	//}}AFX_DATA_INIT

	m_pSymbol = NULL;
}


void CDlgScaleYouGuanSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProcSymLTSetting)
	DDX_Text(pDX, IDC_LINETYPENAME_EDIT, m_strName);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	DDX_Check(pDX, IDC_CHECK_FILLED, m_bFilled);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgScaleYouGuanSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgScaleTurnplateSetting)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_LINETYPENAME_EDIT, OnChangeLinetypenameEdit)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	ON_BN_CLICKED(IDC_CHECK_FILLED, OnCheckFilled)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleTurnplateSetting message handlers

void CDlgScaleYouGuanSetting::OnPreviewButton() 
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
	if( m_pSymbol )m_pSymbol->m_strBaseLinetypeName = m_strName;
	
	UpdateData(FALSE);
	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgScaleYouGuanSetting::OnChangeLinetypenameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if(m_pSymbol == NULL)
		return;
	
	m_pSymbol->m_strBaseLinetypeName = m_strName;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	// TODO: Add your control notification handler code here
	
}

void CDlgScaleYouGuanSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if(m_pSymbol == NULL)
		return;
	
	m_pSymbol->m_fWidth = m_fWidth;	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	// TODO: Add your control notification handler code here
	
}

void CDlgScaleYouGuanSetting::SetMem(CScaleYouGuan *pSym, ConfigLibItem config)
{
	m_pSymbol = pSym;
	m_config = config;
	
	if(m_pSymbol==NULL || config.pLinetypeLib==NULL)
		return;
	
	m_fWidth = m_pSymbol->m_fWidth;
	m_strName = m_pSymbol->m_strBaseLinetypeName;
	m_bFilled = m_pSymbol->m_bFilled;
}


void CDlgScaleYouGuanSetting::OnCheckFilled()
{
	UpdateData(TRUE);

	if(m_pSymbol == NULL)
		return;
	
	m_pSymbol->m_bFilled = m_bFilled;	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}
