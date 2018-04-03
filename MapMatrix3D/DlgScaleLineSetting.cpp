// DlgScallLineSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgScaleLineSetting.h"
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
// CDlgScaleLineSetting dialog


CDlgScaleLineSetting::CDlgScaleLineSetting(CWnd* pParent /*=NULL*/)
	: CSonDialog(CDlgScaleLineSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgScaleLineSetting)
	m_strBaseLinetypeName = _T("");
	m_lfCycle = 0.0;
	m_lfWidth = 0.0;
	m_lfXOffset0 = 0.0;
	m_lfXOffset1 = 0.0;
	m_lfYOffset0 = 0.0;
	m_lfYOffset1 = 0.0;
	m_nPlaceType = 0;
	m_lfYMod0 = 0.0;
	m_lfYMod1 = 0.0;
	//}}AFX_DATA_INIT
	m_pScale = NULL;
}


void CDlgScaleLineSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScaleLineSetting)
// 	DDX_Control(pDX, IDC_YOFFSETTYPE1_COMBO, m_cYoffsetType1);
// 	DDX_Control(pDX, IDC_YOFFSETTYPE0_COMBO, m_cYoffsetType0);
	DDX_Control(pDX, IDC_PLACETYPE_COMBO, m_cPlaceType);
	DDX_Control(pDX, IDC_AVERAGEDRAW_COMBO, m_cAverageDraw);
	DDX_Text(pDX, IDC_BASELINETYPENAME_EDIT, m_strBaseLinetypeName);
	DDX_Text_Silence(pDX, IDC_CYCLE_EDIT, m_lfCycle);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_lfWidth);
	DDX_Text_Silence(pDX, IDC_XOFFSET0_EDIT, m_lfXOffset0);
	DDX_Text_Silence(pDX, IDC_XOFFSET1_EDIT, m_lfXOffset1);
	DDX_Text_Silence(pDX, IDC_YOFFSET0_EDIT, m_lfYOffset0);
	DDX_Text_Silence(pDX, IDC_YOFFSET1_EDIT, m_lfYOffset1);
	DDX_Text_Silence(pDX, IDC_YMOD0_EDIT, m_lfYMod0);
	DDX_Text_Silence(pDX, IDC_YMOD1_EDIT, m_lfYMod1);
	//}}AFX_DATA_MAP
}
BOOL CDlgScaleLineSetting::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_cPlaceType.AddString(StrFromResID(IDS_FIXEDCYCLE));
	m_cPlaceType.AddString(StrFromResID(IDS_SCALECYCLE));
	m_cPlaceType.AddString(StrFromResID(IDS_VERTEX));
	m_cPlaceType.AddString(StrFromResID(IDS_FIRST_POINT));
	m_cPlaceType.AddString(StrFromResID(IDS_END_POINT));
	m_cPlaceType.AddString(StrFromResID(IDS_HEADEND_POINT));
	m_cPlaceType.AddString(StrFromResID(IDS_MIDMID_POINT));

	for (int i=0; i<m_cPlaceType.GetCount(); i++)
	{
		m_cPlaceType.SetItemData(i,i);
	}

	for (i=0; i<m_cPlaceType.GetCount(); i++)
	{
		if (m_cPlaceType.GetItemData(i) == m_nPlaceType)
		{
			m_cPlaceType.SetCurSel(i);
			break;
		}
	}

	m_cAverageDraw.SetCurSel(m_pScale->m_bAverageDraw?0:1);

	
	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgScaleLineSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgScaleLineSetting)
	ON_EN_CHANGE(IDC_BASELINETYPENAME_EDIT, OnChangeBaselinetypenameEdit)
	ON_EN_CHANGE(IDC_CYCLE_EDIT, OnChangeCycleEdit)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	ON_EN_CHANGE(IDC_XOFFSET0_EDIT, OnChangeXoffset0Edit)
	ON_EN_CHANGE(IDC_XOFFSET1_EDIT, OnChangeXoffset1Edit)
	ON_EN_CHANGE(IDC_YOFFSET0_EDIT, OnChangeYoffset0Edit)
	ON_EN_CHANGE(IDC_YOFFSET1_EDIT, OnChangeYoffset1Edit)
	ON_CBN_SELCHANGE(IDC_PLACETYPE_COMBO, OnSelchangePlacetypeCombo)
	ON_CBN_SELCHANGE(IDC_AVERAGEDRAW_COMBO, OnSelchangeAveragedrawCombo)
// 	ON_CBN_SELCHANGE(IDC_XOFFSETTYPE0_COMBO, OnSelchangeXoffsettype0Combo)
// 	ON_CBN_SELCHANGE(IDC_YOFFSETTYPE0_COMBO, OnSelchangeYoffsettype0Combo)
// 	ON_CBN_SELCHANGE(IDC_XOFFSETTYPE1_COMBO, OnSelchangeXoffsettype1Combo)
// 	ON_CBN_SELCHANGE(IDC_YOFFSETTYPE1_COMBO, OnSelchangeYoffsettype1Combo)
	ON_EN_CHANGE(IDC_YMOD0_EDIT, OnChangeYmod0Edit)
	ON_EN_CHANGE(IDC_YMOD1_EDIT, OnChangeYmod1Edit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleLineSetting message handlers

void CDlgScaleLineSetting::OnChangeBaselinetypenameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_strBaseLinetypeName = m_strBaseLinetypeName;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleLineSetting::OnChangeCycleEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_lfCycle = m_lfCycle;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}



void CDlgScaleLineSetting::OnPreviewButton() 
{
	// TODO: Add your control notification handler code here
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT,-1);

	CDlgCellDefLinetypeView dlg;
	CSwitchScale scale(m_config.GetScale());
// 	dlg.SetLineTypeLib(m_pBaseLib);
 	dlg.SetShowMode(FALSE,TRUE,1,m_strBaseLinetypeName);
// 	dlg.SetScale(m_dScale);
	dlg.SetConfig(m_config);
	
	if(IDOK != dlg.DoModal())
	{
		pWnd->SendMessage(WM_SYMBOL_REPAINT,-2);
		return;
	}
	
	m_strBaseLinetypeName = dlg.GetName();
	m_pScale->m_strBaseLinetypeName = m_strBaseLinetypeName;
	
	UpdateData(FALSE);
	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgScaleLineSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_lfWidth = m_lfWidth;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleLineSetting::OnChangeXoffset0Edit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_lfXOffset0 = m_lfXOffset0;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleLineSetting::OnChangeXoffset1Edit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_lfXOffset1 = m_lfXOffset1;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleLineSetting::OnChangeYoffset0Edit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_lfYOffset0 = m_lfYOffset0;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleLineSetting::OnChangeYoffset1Edit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_lfYOffset1 = m_lfYOffset1;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}


void CDlgScaleLineSetting::SetMem(CScaleLinetype *pScale, ConfigLibItem config)
{
 	m_pScale = pScale;
// 	m_pBaseLib = pLib;
	m_config = config;
	
	if(m_pScale==NULL || config.pLinetypeLib==NULL)
		return;
	
	m_lfCycle = m_pScale->m_lfCycle;
	m_lfWidth = m_pScale->m_lfWidth;
	m_lfXOffset0 = m_pScale->m_lfXOffset0;
	m_lfXOffset1 = m_pScale->m_lfXOffset1;
	m_lfYOffset0 = m_pScale->m_lfYOffset0;
	m_lfYOffset1 = m_pScale->m_lfYOffset1;
	m_lfYMod0 = m_pScale->m_lfYOffsetType0;
	m_lfYMod1 = m_pScale->m_lfYOffsetType1;
	m_nPlaceType = m_pScale->m_nPlaceType;
	m_bAverageDraw = m_pScale->m_bAverageDraw;
	m_strBaseLinetypeName = m_pScale->m_strBaseLinetypeName;
	
//	UpdateData(FALSE);
}

void CDlgScaleLineSetting::OnSelchangePlacetypeCombo() 
{
	// TODO: Add your control notification handler code here
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_nPlaceType = m_cPlaceType.GetItemData(m_cPlaceType.GetCurSel());
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgScaleLineSetting::OnSelchangeAveragedrawCombo() 
{
	// TODO: Add your control notification handler code here
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_bAverageDraw = m_cAverageDraw.GetCurSel()==0?TRUE:FALSE;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgScaleLineSetting::OnChangeYmod0Edit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_lfYOffsetType0 = m_lfYMod0;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgScaleLineSetting::OnChangeYmod1Edit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pScale == NULL)
		return;
	
	m_pScale->m_lfYOffsetType1 = m_lfYMod1;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}
