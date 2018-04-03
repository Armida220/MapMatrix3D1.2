// DlgCellLineSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgCellLineSetting.h"
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
// CDlgCellLineSetting dialog


CDlgCellLineSetting::CDlgCellLineSetting(CWnd* pParent /*=NULL*/)
	: CSonDialog(CDlgCellLineSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCellLineSetting)
	m_strCellDefName = _T("");
	m_fAngle = 0.0f;
	m_fBaseXOffset = 0.0f;
	m_fBaseYOffset = 0.0f;
	m_fCycle = 0.0f;
	m_fkx = 0.0f;
	m_fky = 0.0f;
	m_nPlacePos = 0;
	m_fWidth = 0.0f;
// 	m_fXOffset = 0.0f;
// 	m_fYOffset = 0.0f;
	//}}AFX_DATA_INIT
	m_pCellLine = NULL;
	m_bCellAlign = FALSE;
	m_bInsideBaseLine = FALSE;
}


void CDlgCellLineSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCellLineSetting)
	DDX_Control(pDX, IDC_FILLTYPE_COMBO, m_cFillType);
	DDX_Control(pDX, IDC_PLACETYPE_COMBO, m_cPlaceType);
	DDX_Text(pDX, IDC_CELLDEFNAME_EDIT, m_strCellDefName);
	DDX_Text_Silence(pDX, IDC_ANGLE_EDIT, m_fAngle);
	DDX_Text_Silence(pDX, IDC_BASEXOFFSET_EDIT, m_fBaseXOffset);
	DDX_Text_Silence(pDX, IDC_BASEYOFFSET_EDIT, m_fBaseYOffset);
	DDX_Text_Silence(pDX, IDC_CYCLE_EDIT, m_fCycle);
	DDX_Text_Silence(pDX, IDC_KX_EDIT, m_fkx);
	DDX_Text_Silence(pDX, IDC_KY_EDIT, m_fky);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
// 	DDX_Text_Silence(pDX, IDC_XOFFSET_EDIT, m_fXOffset);
// 	DDX_Text_Silence(pDX, IDC_YOFFSET_EDIT, m_fYOffset);
	DDX_Check(pDX,IDC_CHECK_CELLALIGN, m_bCellAlign);
	DDX_Check(pDX,IDC_CHECK_INSIDEBASELINE, m_bInsideBaseLine);
	//}}AFX_DATA_MAP
}

BOOL CDlgCellLineSetting::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_cFillType.AddString(StrFromResID(IDS_CYCLE));
	m_cFillType.SetItemData(0,1);
	m_cFillType.AddString(StrFromResID(IDS_VERTEX));
	m_cFillType.SetItemData(1,2);
	m_cFillType.AddString(StrFromResID(IDS_VERTEXDOUBLE));
	m_cFillType.SetItemData(2,3);
	for (int i=0; i<m_cFillType.GetCount(); i++)
	{
		if (m_cFillType.GetItemData(i) == m_nPlacePos)
		{
			m_cFillType.SetCurSel(i);
			break;
		}
	}

	m_cPlaceType.AddString(StrFromResID(IDS_BASE_LINE));
	m_cPlaceType.SetItemData(0,1);
	m_cPlaceType.AddString(StrFromResID(IDS_AID_LINE));
	m_cPlaceType.SetItemData(1,2);
	m_cPlaceType.AddString(StrFromResID(IDS_MID_LINE));
	m_cPlaceType.SetItemData(2,3);
	for (i=0; i<m_cPlaceType.GetCount(); i++)
	{
		if (m_cPlaceType.GetItemData(i) == m_nPlaceType)
		{
			m_cPlaceType.SetCurSel(i);
			break;
		}
	}
	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgCellLineSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgCellLineSetting)
	ON_EN_CHANGE(IDC_CELLDEFNAME_EDIT, OnChangeCelldefnameEdit)
	ON_EN_CHANGE(IDC_CYCLE_EDIT, OnChangeCycleEdit)
	ON_EN_CHANGE(IDC_KY_EDIT, OnChangeKyEdit)
// 	ON_EN_CHANGE(IDC_XOFFSET_EDIT, OnChangeXoffsetEdit)
// 	ON_EN_CHANGE(IDC_YOFFSET_EDIT, OnChangeYoffsetEdit)
	ON_EN_CHANGE(IDC_ANGLE_EDIT, OnChangeAngleEdit)
	ON_EN_CHANGE(IDC_BASEXOFFSET_EDIT, OnChangeBasexoffsetEdit)
	ON_EN_CHANGE(IDC_BASEYOFFSET_EDIT, OnChangeBaseyoffsetEdit)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_BN_CLICKED(IDC_CHECK_CELLALIGN, OnCheckCellAlign)
	ON_BN_CLICKED(IDC_CHECK_INSIDEBASELINE, OnCheckInsideBaseLine)
	ON_EN_CHANGE(IDC_KX_EDIT, OnChangeKxEdit)
	ON_CBN_SELCHANGE(IDC_FILLTYPE_COMBO, OnSelchangeFilltypeCombo)
	ON_CBN_SELCHANGE(IDC_PLACETYPE_COMBO, OnSelchangePlacetypeCombo)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCellLineSetting message handlers

void CDlgCellLineSetting::OnChangeCelldefnameEdit() 
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

void CDlgCellLineSetting::OnChangeWidthEdit() 
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

void CDlgCellLineSetting::OnChangeCycleEdit() 
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

void CDlgCellLineSetting::OnChangeKxEdit() 
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

void CDlgCellLineSetting::OnChangeKyEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellLine == NULL)
		return;
	
	m_pCellLine->m_fky = m_fky;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}



/*
void CDlgCellLineSetting::OnChangeXoffsetEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellLine == NULL)
		return;
	
	m_pCellLine->m_fXOffset = m_fXOffset;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellLineSetting::OnChangeYoffsetEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellLine == NULL)
		return;
	
	m_pCellLine->m_fYOffset = m_fYOffset;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}
*/
void CDlgCellLineSetting::OnChangeAngleEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellLine == NULL)
		return;
	
	m_pCellLine->m_fAngle = m_fAngle*PI/180;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellLineSetting::OnChangeBasexoffsetEdit() 
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

void CDlgCellLineSetting::OnChangeBaseyoffsetEdit() 
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

void CDlgCellLineSetting::OnPreviewButton() 
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

void CDlgCellLineSetting::SetMem(CCellLinetype *pCellline, ConfigLibItem config)
{
	m_pCellLine = pCellline;
	m_config = config;
	
	if(m_pCellLine==NULL || m_config.pCellDefLib==NULL)
		return;
	
	m_fAngle = m_pCellLine->m_fAngle*180/PI;
	m_fBaseXOffset = m_pCellLine->m_fBaseXOffset;
	m_fBaseYOffset = m_pCellLine->m_fBaseYOffset;
	m_fCycle = m_pCellLine->m_fCycle;
	m_fWidth = m_pCellLine->m_fWidth;
	m_fkx = m_pCellLine->m_fkx;
	m_fky = m_pCellLine->m_fky;
// 	m_fXOffset = m_pCellLine->m_fXOffset;
// 	m_fYOffset = m_pCellLine->m_fYOffset;
	m_nPlaceType = m_pCellLine->m_nPlaceType;
	m_nPlacePos = m_pCellLine->m_nPlacePos;
	m_strCellDefName = m_pCellLine->m_strCellDefName;
	m_bCellAlign = m_pCellLine->m_bCellAlign;
	m_bInsideBaseLine = m_pCellLine->m_bInsideBaseLine;
	
//	UpdateData(FALSE);
}



void CDlgCellLineSetting::OnSelchangeFilltypeCombo() 
{
	// TODO: Add your control notification handler code here
	if(m_pCellLine == NULL)
		return;
	
	m_pCellLine->m_nPlacePos = m_cFillType.GetItemData(m_cFillType.GetCurSel());
	
	CWnd *pWnd = GetDlgItem(IDC_CHECK_CELLALIGN );
	if( pWnd )
	{
		pWnd->EnableWindow(m_pCellLine->m_nPlacePos==CCellLinetype::Cycle);
	}
	
	pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgCellLineSetting::OnSelchangePlacetypeCombo() 
{
	// TODO: Add your control notification handler code here
	if(m_pCellLine == NULL)
		return;
	
	m_pCellLine->m_nPlaceType = m_cPlaceType.GetItemData(m_cPlaceType.GetCurSel());
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgCellLineSetting::OnCheckCellAlign()
{
	if(m_pCellLine == NULL)
		return;
	
	UpdateData(TRUE);
	
	m_pCellLine->m_bCellAlign = m_bCellAlign;	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgCellLineSetting::OnCheckInsideBaseLine()
{
	if(m_pCellLine == NULL)
		return;
	
	UpdateData(TRUE);
	
	m_pCellLine->m_bInsideBaseLine = m_bInsideBaseLine;	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}
