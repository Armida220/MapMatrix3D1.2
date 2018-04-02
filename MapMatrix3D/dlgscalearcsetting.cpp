// DlgScaleArcSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgScaleArcSetting.h"
#include "SilenceDDX.h"
#include "ExMessage.h"
#include "DlgCellDefLinetypeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleArcSetting dialog


CDlgScaleArcSetting::CDlgScaleArcSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgScaleArcSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgScaleArcSetting)
	m_fArcLen = 0.0f;
	m_strName = _T("");
	m_fWidth = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgScaleArcSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScaleArcSetting)
	DDX_Control(pDX, IDC_PLACETYPE_COMBO, m_cPlaceType);
	DDX_Text_Silence(pDX, IDC_LEN_EDIT, m_fArcLen);
	DDX_Text(pDX, IDC_LINETYPENAME_EDIT, m_strName);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgScaleArcSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgScaleArcSetting)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_LEN_EDIT, OnChangeLenEdit)
	ON_EN_CHANGE(IDC_LINETYPENAME_EDIT, OnChangeLinetypenameEdit)
	ON_CBN_SELCHANGE(IDC_PLACETYPE_COMBO, OnSelchangePlacetypeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleArcSetting message handlers

BOOL CDlgScaleArcSetting::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_cPlaceType.AddString(StrFromResID(IDS_FIRST_POINT));
	m_cPlaceType.AddString(StrFromResID(IDS_END_POINT));
	m_cPlaceType.AddString(StrFromResID(IDS_HEADEND_POINT));
	
	m_cPlaceType.SetCurSel(m_pScale->m_nPlaceType);
	
	
	return TRUE;
}

void CDlgScaleArcSetting::OnPreviewButton() 
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

void CDlgScaleArcSetting::OnChangeLenEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_fArcLength = m_fArcLen;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
	// TODO: Add your control notification handler code here
	
}

void CDlgScaleArcSetting::OnChangeLinetypenameEdit() 
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

void CDlgScaleArcSetting::OnSelchangePlacetypeCombo() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_nPlaceType = m_cPlaceType.GetCurSel();
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgScaleArcSetting::SetMem(CScaleArctype *pScale, ConfigLibItem config)
{
	m_pScale = pScale;
	// 	m_pBaseLib = pLib;
	m_config = config;
	
	if(m_pScale==NULL || config.pLinetypeLib==NULL)
		return;
	
	m_fWidth = m_pScale->m_fWidth;
	m_fArcLen = m_pScale->m_fArcLength;
	m_strName = m_pScale->m_strBaseLinetypeName;
	
	//	UpdateData(FALSE);
}
