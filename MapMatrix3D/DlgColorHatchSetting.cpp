// DlgColorHatchSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgColorHatchSetting.h"
#include "ExMessage.h"
#include "SilenceDDX.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgColorHatchSetting dialog


CDlgColorHatchSetting::CDlgColorHatchSetting(CWnd* pParent /*=NULL*/)
	: CSonDialog(CDlgColorHatchSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgColorHatchSetting)
	m_dColor = 0;
	m_fTransparence = 0.0f;
	//}}AFX_DATA_INIT
	m_pColorHatch = NULL;
	m_bUseSelfcolor = FALSE;
}


void CDlgColorHatchSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgColorHatchSetting)
	DDX_Control(pDX, IDC_COLOR_EDIT, m_Color);
	DDX_Control(pDX, IDC_USESELFCOLOR_COMBO, m_cUseSefcolor);
	DDX_Text_Silence(pDX, IDC_TRANSPARENCE_EDIT, m_fTransparence);
	//}}AFX_DATA_MAP
}

BOOL CDlgColorHatchSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str = m_bUseSelfcolor?StrFromResID(IDS_YES):StrFromResID(IDS_NO);
	m_cUseSefcolor.SelectString(-1,str);

	m_Color.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	
	m_Color.SetColor(m_dColor);

	UpdateData(TRUE);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgColorHatchSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgColorHatchSetting)
	ON_EN_CHANGE(IDC_TRANSPARENCE_EDIT, OnChangeTransparenceEdit)
	ON_BN_CLICKED(IDC_COLOR_EDIT, OnColorEdit)
	ON_CBN_SELCHANGE(IDC_USESELFCOLOR_COMBO, OnSelchangeUseselfcolorCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgColorHatchSetting message handlers



void CDlgColorHatchSetting::OnChangeTransparenceEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	if(m_pColorHatch == NULL)
		return;
	
	m_pColorHatch->m_fTransparence = m_fTransparence;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgColorHatchSetting::SetMem(CColorHatch *pColorHatch)
{
	m_pColorHatch = pColorHatch;
	
	if(m_pColorHatch == NULL)
		return;
	
	m_dColor = m_pColorHatch->m_nColor;
	m_fTransparence = m_pColorHatch->m_fTransparence;
	m_bUseSelfcolor = m_pColorHatch->m_bUseSelfcolor;
	
//	UpdateData(FALSE);
}

void CDlgColorHatchSetting::OnColorEdit() 
{
	// TODO: Add your control notification handler code here
	m_pColorHatch->m_nColor = m_Color.GetColor();
	GetParentOwner()->SendMessage(WM_SYMBOL_REPAINT);
	/*
	CColorDialog dlg;
	if( dlg.DoModal() == IDOK )
	{
		m_dColor = dlg.GetColor();

		m_pColorHatch->m_nColor = m_dColor;

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


void CDlgColorHatchSetting::OnSelchangeUseselfcolorCombo() 
{
	// TODO: Add your control notification handler code here
	if(m_pColorHatch == NULL)
		return;

	int sel = m_cUseSefcolor.GetCurSel();
	
	m_pColorHatch->m_bUseSelfcolor = !sel;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}
