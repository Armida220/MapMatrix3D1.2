// DlgCellSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgCellSetting.h"
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
// CDlgCellSetting dialog


CDlgCellSetting::CDlgCellSetting(CWnd* pParent /*=NULL*/)
	: CSonDialog(CDlgCellSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCellSetting)
	m_fAngle = 0.0f;
	m_strCellName = _T("");
	m_fDx = 0.0f;
	m_fDy = 0.0f;
	m_fKx = 0.0f;
	m_fKy = 0.0f;
	m_nPlaceType = 0;
	m_fWidth = 0.0f;
	m_fExtendDis = 0.0f;
	//}}AFX_DATA_INIT
	m_pCell = NULL;
}


void CDlgCellSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCellSetting)
	DDX_Control(pDX, IDC_PLACETYPE_COMBO, m_cPlaceType);
	DDX_Control(pDX, IDC_COVERTYPE_COMBO, m_cCoverType);
	DDX_Text_Silence(pDX, IDC_ANGLE_EDIT, m_fAngle);
	DDX_Text(pDX, IDC_CELLNAEM_EDIT, m_strCellName);
	DDX_Text_Silence(pDX, IDC_DX_EDIT, m_fDx);
	DDX_Text_Silence(pDX, IDC_DY_EDIT, m_fDy);
	DDX_Text_Silence(pDX, IDC_KX_EDIT, m_fKx);
	DDX_Text_Silence(pDX, IDC_KY_EDIT, m_fKy);
	DDX_Control(pDX, IDC_FILLTYPE_COMBO, m_cFillType);
	DDX_Control(pDX, IDC_CENLINEMODE_COMBO, m_cCenLineMode);
	DDX_Control(pDX, IDC_FIRSTDIR_COMBO, m_cDirWithFistLine);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	DDX_Text_Silence(pDX, IDC_EXTENDDIS_EDIT, m_fExtendDis);
	//}}AFX_DATA_MAP
}

BOOL CDlgCellSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (int i=0; i<4; i++)
	{
		m_cPlaceType.AddString(StrFromResID(IDS_FIRST_POINT+i));
		m_cPlaceType.SetItemData(i,i);

	}
/*	//获得ComboBox控件的下拉宽度
	CRect rcWnd;
	m_cPlaceType.GetClientRect(&rcWnd);
	int nTextWid = 0;
	CDC *pDC = m_cPlaceType.GetDC();
	if( pDC )
	{
		nTextWid = pDC->GetTextExtent(maxName).cx;
		m_cPlaceType.ReleaseDC(pDC);
	}
	if( rcWnd.Width()<nTextWid )
		m_cPlaceType.SetDroppedWidth(nTextWid);
	else
		m_cPlaceType.SetDroppedWidth(rcWnd.Width());
*/	
	for (i=0; i<4; i++)
	{
		if (m_cPlaceType.GetItemData(i) == m_nPlaceType)
		{
			m_cPlaceType.SetCurSel(i);
			break;
		}
	}

	m_cFillType.AddString(StrFromResID(IDS_BASE_LINE));
	m_cFillType.SetItemData(0,CCell::BaseLine);
	m_cFillType.AddString(StrFromResID(IDS_AID_LINE));
	m_cFillType.SetItemData(1,CCell::AidLine);
	m_cFillType.AddString(StrFromResID(IDS_MID_LINE));
	m_cFillType.SetItemData(2,CCell::MidLine);
	for (i=0; i<m_cFillType.GetCount(); i++)
	{
		if (m_cFillType.GetItemData(i) == m_nFillType)
		{
			m_cFillType.SetCurSel(i);
			break;
		}
	}

	m_cCenLineMode.AddString(StrFromResID(IDS_CENLINE_NULL));
	m_cCenLineMode.SetItemData(0,CCell::cenNULL);
	m_cCenLineMode.AddString(StrFromResID(IDS_CENLINE_HEADTAIL));
	m_cCenLineMode.SetItemData(1,CCell::cenHead2Tail);
	m_cCenLineMode.AddString(StrFromResID(IDS_CENLINE_MIDMID));
	m_cCenLineMode.SetItemData(2,CCell::cenMid2Mid);
	for (i=0; i<m_cCenLineMode.GetCount(); i++)
	{
		if (m_cCenLineMode.GetItemData(i) == m_nCenlineMode)
		{
			m_cCenLineMode.SetCurSel(i);
			break;
		}
	}

	m_cCoverType.AddString(StrFromResID(IDS_COVERTYPE_NONE));
	m_cCoverType.AddString(StrFromResID(IDS_COVERTYPE_RECT));
	m_cCoverType.AddString(StrFromResID(IDS_COVERTYPE_CIRCLE));
	m_cCoverType.SetCurSel(m_pCell->m_nCoverType);

	m_cDirWithFistLine.AddString(StrFromResID(IDS_YES));
	m_cDirWithFistLine.SetItemData(0,1);
	m_cDirWithFistLine.AddString(StrFromResID(IDS_NO));
	m_cDirWithFistLine.SetItemData(1,0);
	m_cDirWithFistLine.SetCurSel(m_pCell->m_bDirWithFirstLine?0:1);

	if (m_nPlaceType != CCell::Center)
	{
		m_cDirWithFistLine.EnableWindow(FALSE);
	}
		
	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgCellSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgCellSetting)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_EN_CHANGE(IDC_CELLNAEM_EDIT, OnChangeCellnaemEdit)
	ON_EN_CHANGE(IDC_DX_EDIT, OnChangeDxEdit)
	ON_EN_CHANGE(IDC_KX_EDIT, OnChangeKxEdit)
	ON_EN_CHANGE(IDC_ANGLE_EDIT, OnChangeAngleEdit)
	ON_EN_CHANGE(IDC_DY_EDIT, OnChangeDyEdit)
	ON_EN_CHANGE(IDC_KY_EDIT, OnChangeKyEdit)
	ON_EN_CHANGE(IDC_EXTENDDIS_EDIT, OnChangeExtendDisEdit)
	ON_CBN_SELCHANGE(IDC_PLACETYPE_COMBO, OnSelchangePlacetypeCombo)
	ON_CBN_SELCHANGE(IDC_FILLTYPE_COMBO, OnSelchangeFilltypeCombo)
	ON_CBN_SELCHANGE(IDC_FIRSTDIR_COMBO, OnSelchangeDirWithFistLineCombo)
	ON_CBN_SELCHANGE(IDC_CENLINEMODE_COMBO, OnSelchangeCenlinemodeCombo)
	ON_CBN_SELCHANGE(IDC_COVERTYPE_COMBO, OnSelchangeCovertypeCombo)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCellSetting message handlers


void CDlgCellSetting::SetMem(CCell *pCell, ConfigLibItem config)
{
	m_pCell = pCell;
	m_config = config;

	if(m_config.pCellDefLib==NULL || m_config.pCellDefLib==NULL)
		return;

	m_fDx = m_pCell->m_dx;
	m_fDy = m_pCell->m_dy;
	m_fWidth  = m_pCell->m_fWidth;
	m_fAngle = m_pCell->m_angle*180/PI;
	m_fKx = m_pCell->m_kx;
	m_fKy = m_pCell->m_ky;
	m_strCellName = m_pCell->m_strCellDefName;
	m_nPlaceType = pCell->m_nPlaceType;
	m_nFillType = pCell->m_nFillType;
	m_nCenlineMode = pCell->m_nCenterLineMode;
	m_fExtendDis = pCell->m_fExtendDis;

//	UpdateData(FALSE);
}

void CDlgCellSetting::OnPreviewButton() 
{
	// TODO: Add your control notification handler code here
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT,-1);

	CDlgCellDefLinetypeView dlg;

	CSwitchScale scale(m_config.GetScale());

//	dlg.SetCellDefLib(m_pCellLib);
	dlg.SetConfig(m_config);
	dlg.SetShowMode(FALSE,TRUE,0,m_strCellName);

	if(IDOK != dlg.DoModal())
	{
		pWnd->SendMessage(WM_SYMBOL_REPAINT,-2);
		return;
	}

	m_strCellName = dlg.GetName();
	m_pCell->m_strCellDefName = m_strCellName;

	UpdateData(FALSE);

	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);

	return;
	
}
//DEL 
//DEL void CDlgCellSetting::OnOK() 
//DEL {
//DEL 	// TODO: Add extra validation here
//DEL 
//DEL 	UpdateData(TRUE);
//DEL 	if(m_pCell == NULL)
//DEL 		return;
//DEL 	
//DEL 	m_pCell->m_dx = m_fDx;
//DEL 	m_pCell->m_dy = m_fDy;
//DEL 	m_pCell->m_angle = m_fAngle;
//DEL 	m_pCell->m_kx = m_fKx;
//DEL 	m_pCell->m_ky = m_fKy;
//DEL 	m_pCell->m_nPlaceType = m_nPlaceType;
//DEL 	m_pCell->m_strCellDefName =	m_strCellName;
//DEL 	
//DEL 
//DEL 	CWnd *pWnd = GetParentOwner();
//DEL 	pWnd->SendMessage(WM_SYMBOL_REPAINT);
//DEL 	
//DEL 	CDialog::OnOK();
//}



void CDlgCellSetting::OnChangeCellnaemEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCell == NULL)
		return;
	
	m_pCell->m_strCellDefName =	m_strCellName;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellSetting::OnChangeDxEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCell == NULL)
		return;
	
	m_pCell->m_dx = m_fDx;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellSetting::OnChangeKxEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCell == NULL)
		return;
	
	m_pCell->m_kx = m_fKx;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellSetting::OnChangeAngleEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCell == NULL)
		return;

	m_pCell->m_angle = m_fAngle*PI/180;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellSetting::OnChangeDyEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCell == NULL)
		return;
	
	m_pCell->m_dy = m_fDy;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellSetting::OnChangeKyEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCell == NULL)
		return;
	
	m_pCell->m_ky = m_fKy;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellSetting::OnChangeExtendDisEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCell == NULL)
		return;
	
	m_pCell->m_fExtendDis = m_fExtendDis;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellSetting::OnSelchangePlacetypeCombo() 
{
	// TODO: Add your control notification handler code here
	if(m_pCell == NULL)
		return;
	
	m_pCell->m_nPlaceType = m_cPlaceType.GetItemData(m_cPlaceType.GetCurSel());

	m_cDirWithFistLine.EnableWindow(m_pCell->m_nPlaceType==CCell::Center?TRUE:FALSE);
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgCellSetting::OnSelchangeCovertypeCombo() 
{
	// TODO: Add your control notification handler code here
	if(m_pCell == NULL)
		return;
	
	m_pCell->m_nCoverType = m_cCoverType.GetCurSel();
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgCellSetting::OnSelchangeDirWithFistLineCombo()
{
	if(m_pCell == NULL)
		return;
	
	m_pCell->m_bDirWithFirstLine = m_cDirWithFistLine.GetItemData(m_cDirWithFistLine.GetCurSel());
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgCellSetting::OnSelchangeFilltypeCombo() 
{
	// TODO: Add your control notification handler code here
	if(m_pCell == NULL)
		return;
	
	m_pCell->m_nFillType = m_cFillType.GetItemData(m_cFillType.GetCurSel());
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellSetting::OnSelchangeCenlinemodeCombo()
{	
	if(m_pCell == NULL)
		return;
	
	m_pCell->m_nCenterLineMode = m_cCenLineMode.GetItemData(m_cCenLineMode.GetCurSel());
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);

}

void CDlgCellSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	if(m_pCell == NULL)
		return;
	
	m_pCell->m_fWidth = m_fWidth;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}
