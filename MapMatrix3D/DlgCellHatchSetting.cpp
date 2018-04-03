// CDlgCellHatchSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgCellHatchSetting.h"
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
// CDlgCellHatchSetting dialog


CDlgCellHatchSetting::CDlgCellHatchSetting(CWnd* pParent /*=NULL*/)
	: CSonDialog(CDlgCellHatchSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCellHatchSetting)
	m_fAngle = 0.0f;
	m_strCellDefName = _T("");
	m_fddx = 0.0f;
	m_fddy = 0.0f;
	m_fdx = 0.0f;
	m_fdy = 0.0f;
	m_fkx = 0.0f;
	m_fky = 0.0f;
	m_fxoff = 0.0f;
	m_fyoff = 0.0f;
	m_fWidth = 0.0f;
	//}}AFX_DATA_INIT
	m_pCellHatch = NULL;
	m_bAccurateCutCell = FALSE;
	m_bRandomAngle = FALSE;
}


void CDlgCellHatchSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCellHatchSetting)
	DDX_Text_Silence(pDX, IDC_ANGLE_EDIT, m_fAngle);
	DDX_Text(pDX, IDC_CELLDEFNAME_EDIT, m_strCellDefName);
	DDX_Text_Silence(pDX, IDC_DDX_EDIT, m_fddx);
	DDX_Text_Silence(pDX, IDC_DDY_EDIT, m_fddy);
	DDX_Text_Silence(pDX, IDC_DX_EDIT, m_fdx);
	DDX_Text_Silence(pDX, IDC_DY_EDIT, m_fdy);
	DDX_Text_Silence(pDX, IDC_KX_EDIT, m_fkx);
	DDX_Text_Silence(pDX, IDC_KY_EDIT, m_fky);
	DDX_Text_Silence(pDX, IDC_XOFF_EDIT, m_fxoff);
	DDX_Text_Silence(pDX, IDC_YOFF_EDIT, m_fyoff);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	DDX_Check(pDX, IDC_CHECK_ACCURATE_CUTCELL, m_bAccurateCutCell);
	DDX_Check(pDX, IDC_CHECK_RANDOMANGLE, m_bRandomAngle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCellHatchSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgCellHatchSetting)
	ON_EN_CHANGE(IDC_ANGLE_EDIT, OnChangeAngleEdit)
	ON_EN_CHANGE(IDC_CELLDEFNAME_EDIT, OnChangeCelldefnameEdit)
	ON_EN_CHANGE(IDC_DDX_EDIT, OnChangeDdxEdit)
	ON_EN_CHANGE(IDC_DDY_EDIT, OnChangeDdyEdit)
	ON_EN_CHANGE(IDC_DX_EDIT, OnChangeDxEdit)
	ON_EN_CHANGE(IDC_DY_EDIT, OnChangeDyEdit)
	ON_EN_CHANGE(IDC_KX_EDIT, OnChangeKxEdit)
	ON_EN_CHANGE(IDC_KY_EDIT, OnChangeKyEdit)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	ON_BN_CLICKED(IDC_CHECK_ACCURATE_CUTCELL, OnAccurateCutCell)
	ON_BN_CLICKED(IDC_CHECK_RANDOMANGLE, OnRandomAngle)
	ON_EN_CHANGE(IDC_XOFF_EDIT, OnChangeXoffEdit)
	ON_EN_CHANGE(IDC_YOFF_EDIT, OnChangeYoffEdit)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCellHatchSetting message handlers

void CDlgCellHatchSetting::OnChangeAngleEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellHatch == NULL)
		return;
	
	m_pCellHatch->m_fAngle = m_fAngle*PI/180;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellHatchSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	if(m_pCellHatch == NULL)
		return;
	
	m_pCellHatch->m_fWidth = m_fWidth;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellHatchSetting::OnChangeCelldefnameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellHatch == NULL)
		return;
	
	m_pCellHatch->m_strCellDefName = m_strCellDefName;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellHatchSetting::OnChangeDdxEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellHatch == NULL)
		return;
	
	m_pCellHatch->m_fddx = m_fddx;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}


void CDlgCellHatchSetting::OnChangeDdyEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellHatch == NULL)
		return;
	
	m_pCellHatch->m_fddy = m_fddy;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellHatchSetting::OnChangeDxEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellHatch == NULL)
		return;
	
	m_pCellHatch->m_fdx = m_fdx;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellHatchSetting::OnChangeDyEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellHatch == NULL)
		return;
	
	m_pCellHatch->m_fdy = m_fdy;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellHatchSetting::OnChangeKxEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellHatch == NULL)
		return;
	
	m_pCellHatch->m_fkx = m_fkx;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellHatchSetting::OnChangeKyEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellHatch == NULL)
		return;
	
	m_pCellHatch->m_fky = m_fky;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellHatchSetting::SetMem(CCellHatch *pCellHatch, ConfigLibItem config)
{
	m_pCellHatch = pCellHatch;
	m_config = config;
	
	if(m_pCellHatch==NULL || m_config.pCellDefLib==NULL)
		return;
	
	m_fAngle = m_pCellHatch->m_fAngle*180/PI;
	m_fddx = m_pCellHatch->m_fddx;
	m_fddy = m_pCellHatch->m_fddy;
	m_fdx = m_pCellHatch->m_fdx;
	m_fdy = m_pCellHatch->m_fdy;
	m_fkx = m_pCellHatch->m_fkx;
	m_fky = m_pCellHatch->m_fky;
	m_fxoff = m_pCellHatch->m_fxoff;
	m_fyoff = m_pCellHatch->m_fyoff;
	m_fWidth = m_pCellHatch->m_fWidth;
	m_strCellDefName = m_pCellHatch->m_strCellDefName;
	m_bAccurateCutCell = m_pCellHatch->m_bAccurateCutCell;
	m_bRandomAngle = pCellHatch->m_bRandomAngle;
//	UpdateData(FALSE);
}


void CDlgCellHatchSetting::OnPreviewButton() 
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
	m_pCellHatch->m_strCellDefName = m_strCellDefName;
	
	UpdateData(FALSE);
	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgCellHatchSetting::OnChangeXoffEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellHatch == NULL)
		return;
	
	m_pCellHatch->m_fxoff = m_fxoff;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgCellHatchSetting::OnChangeYoffEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pCellHatch == NULL)
		return;
	
	m_pCellHatch->m_fyoff = m_fyoff;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgCellHatchSetting::OnAccurateCutCell()
{
	UpdateData(TRUE);

	if(m_pCellHatch == NULL)
		return;
	
	m_pCellHatch->m_bAccurateCutCell = m_bAccurateCutCell;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}


void CDlgCellHatchSetting::OnRandomAngle()
{
	UpdateData(TRUE);
	
	if(m_pCellHatch == NULL)
		return;
	
	m_pCellHatch->m_bRandomAngle = m_bRandomAngle;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}