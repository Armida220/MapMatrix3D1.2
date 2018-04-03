// dlgscaleturnplatesetting.cpp : implementation file
//

#include "stdafx.h"
#include "Editbase.h"
#include "dlgscaleChuanSongDaisetting.h"
#include "SilenceDDX.h"
#include "ExMessage.h"
#include "DlgCellDefLinetypeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleFunnelSetting dialog


CDlgScaleChuanSongDaiSetting::CDlgScaleChuanSongDaiSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgScaleChuanSongDaiSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgScaleFunnelSetting)
	m_strName = _T("");
	m_fWidth = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgScaleChuanSongDaiSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScaleFunnelSetting)
	DDX_Text(pDX, IDC_LINETYPENAME_EDIT, m_strName);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	DDX_Control(pDX, IDC_FUNNELTYPE_COMBO, m_cFunnelType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgScaleChuanSongDaiSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgScaleFunnelSetting)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_LINETYPENAME_EDIT, OnChangeLinetypenameEdit)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	ON_CBN_SELCHANGE(IDC_FUNNELTYPE_COMBO, OnSelchangeFunnelypeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleTurnplateSetting message handlers

void CDlgScaleChuanSongDaiSetting::OnPreviewButton() 
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

void CDlgScaleChuanSongDaiSetting::OnChangeLinetypenameEdit() 
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

void CDlgScaleChuanSongDaiSetting::OnChangeWidthEdit() 
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

void CDlgScaleChuanSongDaiSetting::SetMem(CScaleChuanSongDai *pScale, ConfigLibItem config)
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

BOOL CDlgScaleChuanSongDaiSetting::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_cFunnelType.AddString(StrFromResID(IDS_CHUANSONGDAIDISHANGTYPE));
	m_cFunnelType.AddString(StrFromResID(IDS_CHUANSONGDAIDIXIATYPE));

	m_cFunnelType.SetCurSel(m_pScale->m_nCarryType);

	return TRUE;
}

void CDlgScaleChuanSongDaiSetting::OnSelchangeFunnelypeCombo()
{
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_nCarryType = m_cFunnelType.GetCurSel();
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}