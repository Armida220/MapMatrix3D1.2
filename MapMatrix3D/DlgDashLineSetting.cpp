// DlgDashLineSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgDashLineSetting.h"
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
// CDlgDashLineSetting dialog


CDlgDashLineSetting::CDlgDashLineSetting(CWnd* pParent /*=NULL*/)
	: CSonDialog(CDlgDashLineSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDashLineSetting)
	m_fBaseOffSet = 0.0f;
	m_strLineTypeName = _T("");
	m_fIndent = 0.0f;
	m_fXOffset = 0.0f;
	m_fWidth = 0.0f;
	//}}AFX_DATA_INIT
//	m_dScale = 0;
	m_pDashLine = NULL;
	m_nPlaceType = 0;
	m_nIndentType = CDashLinetype::Node;
	m_bAdjustXOffset = FALSE;
	m_bDashAlign = TRUE;
}


void CDlgDashLineSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDashLineSetting)
	DDX_Control(pDX, IDC_COLOR_EDIT, m_Color);
	DDX_Control(pDX, IDC_USESELFCOLOR_COMBO, m_cUseSefcolor);
	DDX_Control(pDX, IDC_PLACETYPE_COMBO, m_cPlaceType);
	DDX_Control(pDX, IDC_INDENT_COMBO, m_cIndentType);
	DDX_Text_Silence(pDX, IDC_BASEOFF_EDIT, m_fBaseOffSet);
	DDX_Text(pDX, IDC_LINETYPENAME_EDIT, m_strLineTypeName);
	DDX_Text_Silence(pDX, IDC_INDENT_EDIT, m_fIndent);
	DDX_Text_Silence(pDX, IDC_XOFF_EDIT, m_fXOffset);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	DDX_Check(pDX,IDC_CHECK_ADJUSTXOFFSET,m_bAdjustXOffset);
	DDX_Check(pDX,IDC_CHECK_DASHALIGN,m_bDashAlign);
	//}}AFX_DATA_MAP
}

BOOL CDlgDashLineSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cPlaceType.AddString(StrFromResID(IDS_BASE_LINE));
	m_cPlaceType.SetItemData(0,1);
	m_cPlaceType.AddString(StrFromResID(IDS_AID_LINE));
	m_cPlaceType.SetItemData(1,2);
	m_cPlaceType.AddString(StrFromResID(IDS_MID_LINE));
	m_cPlaceType.SetItemData(2,3);

	for (int i=0; i<3; i++)
	{
		if (m_cPlaceType.GetItemData(i) == m_nPlaceType)
		{
			m_cPlaceType.SetCurSel(i);
			break;
		}
	}


	m_cIndentType.AddString(StrFromResID(IDS_INDENT_NODE));
	m_cIndentType.SetItemData(0,0);
	m_cIndentType.AddString(StrFromResID(IDS_INDENT_MID));
	m_cIndentType.SetItemData(1,1);
	m_cIndentType.AddString(StrFromResID(IDS_INDENT_HEADTAIL));
	m_cIndentType.SetItemData(2,2);

	m_cIndentType.SetCurSel(m_nIndentType);

	CString str = m_bUseSelfcolor?StrFromResID(IDS_YES):StrFromResID(IDS_NO);
	m_cUseSefcolor.SelectString(-1,str);

	m_Color.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);

	m_Color.SetColor(m_dColor);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgDashLineSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgDashLineSetting)
	ON_EN_CHANGE(IDC_LINETYPENAME_EDIT, OnChangeLinetypenameEdit)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	ON_EN_CHANGE(IDC_BASEOFF_EDIT, OnChangeBaseoffEdit)
	ON_EN_CHANGE(IDC_XOFF_EDIT, OnChangeXOffsetEdit)
	ON_EN_CHANGE(IDC_INDENT_EDIT, OnChangeIndentEdit)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_BN_CLICKED(IDC_CHECK_ADJUSTXOFFSET, OnCheckAdjustXOffset)
	ON_BN_CLICKED(IDC_CHECK_DASHALIGN, OnCheckDashAlign)
	ON_CBN_SELCHANGE(IDC_PLACETYPE_COMBO, OnSelchangePlacetypeCombo)
	ON_CBN_SELCHANGE(IDC_INDENT_COMBO, OnSelchangeIndenttypeCombo)
	ON_CBN_SELCHANGE(IDC_USESELFCOLOR_COMBO, OnSelchangeUseselfcolorCombo)
	ON_BN_CLICKED(IDC_COLOR_EDIT, OnColorEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDashLineSetting message handlers


void CDlgDashLineSetting::SetMem(CDashLinetype *pCell, ConfigLibItem config)
{
 	m_pDashLine = pCell;
	m_config = config;

	if(m_pDashLine==NULL || m_config.pLinetypeLib==NULL)
		return;
	
	m_fBaseOffSet = m_pDashLine->m_fBaseOffset;
	m_fWidth = m_pDashLine->m_fWidth;
	m_nPlaceType = m_pDashLine->m_nPlaceType;
	m_strLineTypeName = m_pDashLine->m_strBaseLinetypeName;
	m_dColor = m_pDashLine->m_nColor;
	m_bUseSelfcolor = m_pDashLine->m_bUseSelfcolor;
	m_fXOffset = m_pDashLine->m_fXOffset0;
	m_fIndent = m_pDashLine->m_fIndent;
	m_nIndentType = m_pDashLine->m_nIndentType;
	m_bAdjustXOffset = m_pDashLine->m_bAdjustXOffset;
	m_bDashAlign = m_pDashLine->m_bDashAlign;
//	m_dScale = dScale;
	
//	UpdateData(FALSE);
}

void CDlgDashLineSetting::OnChangeLinetypenameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pDashLine == NULL)
		return;
	
	m_pDashLine->m_strBaseLinetypeName = m_strLineTypeName;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgDashLineSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	if(m_pDashLine == NULL)
		return;
	
	m_pDashLine->m_fWidth = m_fWidth;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}



void CDlgDashLineSetting::OnChangeBaseoffEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pDashLine == NULL)
		return;
	
	m_pDashLine->m_fBaseOffset = m_fBaseOffSet;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgDashLineSetting::OnPreviewButton() 
{
	// TODO: Add your control notification handler code here
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT,-1);

	CDlgCellDefLinetypeView dlg;
	
	CSwitchScale scale(m_config.GetScale());
// 	dlg.SetLineTypeLib(m_pBaseLib);
 	dlg.SetShowMode(FALSE,TRUE,1,m_strLineTypeName);
// 	dlg.SetScale(m_dScale);
	dlg.SetConfig(m_config);
	
	if(IDOK != dlg.DoModal())
	{
		pWnd->SendMessage(WM_SYMBOL_REPAINT,-2);
		return;
	}
	
	m_strLineTypeName = dlg.GetName();
	m_pDashLine->m_strBaseLinetypeName = m_strLineTypeName;
	
	UpdateData(FALSE);
	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
	return;
}


void CDlgDashLineSetting::OnCheckAdjustXOffset()
{
	if(m_pDashLine == NULL)
		return;

	UpdateData(TRUE);

	m_pDashLine->m_bAdjustXOffset = m_bAdjustXOffset;	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}



void CDlgDashLineSetting::OnCheckDashAlign()
{
	if(m_pDashLine == NULL)
		return;
	
	UpdateData(TRUE);
	
	m_pDashLine->m_bDashAlign = m_bDashAlign;	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}


void CDlgDashLineSetting::OnSelchangePlacetypeCombo() 
{
	// TODO: Add your control notification handler code here
	if(m_pDashLine == NULL)
		return;
	
	m_pDashLine->m_nPlaceType = m_cPlaceType.GetItemData(m_cPlaceType.GetCurSel());
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgDashLineSetting::OnSelchangeIndenttypeCombo()
{
	if(m_pDashLine == NULL)
		return;
	
	m_pDashLine->m_nIndentType = m_cIndentType.GetItemData(m_cIndentType.GetCurSel());
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgDashLineSetting::OnSelchangeUseselfcolorCombo() 
{
	// TODO: Add your control notification handler code here
	if(m_pDashLine == NULL)
		return;
	
	int sel = m_cUseSefcolor.GetCurSel();
	
	m_pDashLine->m_bUseSelfcolor = !sel;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgDashLineSetting::OnChangeIndentEdit()
{
	UpdateData(TRUE);
	if(m_pDashLine == NULL)
		return;
	
	m_pDashLine->m_fIndent = m_fIndent;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgDashLineSetting::OnChangeXOffsetEdit()
{
	UpdateData(TRUE);
	if(m_pDashLine == NULL)
		return;
	
	m_pDashLine->m_fXOffset0 = m_fXOffset;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);

}

void CDlgDashLineSetting::OnColorEdit() 
{
	// TODO: Add your control notification handler code here
	m_pDashLine->m_nColor = m_Color.GetColor();
	GetParentOwner()->SendMessage(WM_SYMBOL_REPAINT);
/*	CColorDialog dlg;
	if( dlg.DoModal() == IDOK )
	{
		m_dColor = dlg.GetColor();
		
		m_pDashLine->m_nColor = m_dColor;
		
		CWnd *pWnd = GetDlgItem(IDC_COLOR_EDIT);
		if (pWnd)
		{
			//创建内存设备
			CClientDC cdc(pWnd);
			CRect rect;
			pWnd->GetClientRect(&rect);
			cdc.FillRect(&rect,&CBrush(m_dColor));
		}
		
		pWnd = GetParentOwner();
		pWnd->SendMessage(WM_SYMBOL_REPAINT);
	}*/
}
