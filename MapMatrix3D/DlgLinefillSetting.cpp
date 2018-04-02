// DlgLinefillSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgLinefillSetting.h"
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
// CDlgLinefillSetting dialog


CDlgLinefillSetting::CDlgLinefillSetting(CWnd* pParent /*=NULL*/)
	: CSonDialog(CDlgLinefillSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLinefillSetting)
	m_fAngle = 0.0f;
	m_fIntv = 0.0f;
	m_strLineName = _T("");
	m_fWidth = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgLinefillSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLinefillSetting)
	DDX_Text_Silence(pDX, IDC_ANGLE_EDIT, m_fAngle);
	DDX_Text_Silence(pDX, IDC_INTV_EDIT, m_fIntv);
	DDX_Text(pDX, IDC_LINETYPENAME_EDIT, m_strLineName);
	DDX_Text_Silence(pDX, IDC_WIDTH_EDIT, m_fWidth);
	//}}AFX_DATA_MAP
}

BOOL CDlgLinefillSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgLinefillSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgLinefillSetting)
	ON_EN_CHANGE(IDC_LINETYPENAME_EDIT, OnChangeLinetypenameEdit)
	ON_EN_CHANGE(IDC_INTV_EDIT, OnChangeIntvEdit)
	ON_EN_CHANGE(IDC_ANGLE_EDIT, OnChangeAngleEdit)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, OnChangeWidthEdit)
	ON_BN_CLICKED(IDC_PREVIEW_BUTTON, OnPreviewButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLinefillSetting message handlers

void CDlgLinefillSetting::OnChangeLinetypenameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pLineHatch == NULL)
		return;
	
	m_pLineHatch->m_strBaseLinetypeName = m_strLineName;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgLinefillSetting::OnChangeIntvEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pLineHatch == NULL)
		return;
	
	m_pLineHatch->m_fIntv = m_fIntv;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgLinefillSetting::OnChangeAngleEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pLineHatch == NULL)
		return;
	
	m_pLineHatch->m_fAngle = m_fAngle*PI/180;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgLinefillSetting::OnChangeWidthEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pLineHatch == NULL)
		return;
	
	m_pLineHatch->m_fWidth = m_fWidth;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgLinefillSetting::OnPreviewButton() 
{
	// TODO: Add your control notification handler code here
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT,-1);
	
	CDlgCellDefLinetypeView dlg;
	
	CSwitchScale scale(m_config.GetScale());
	
	//	dlg.SetCellDefLib(m_pCellLib);
	dlg.SetConfig(m_config);
	dlg.SetShowMode(FALSE,TRUE,1,m_strLineName);
	
	if(IDOK != dlg.DoModal())
	{
		pWnd->SendMessage(WM_SYMBOL_REPAINT,-2);
		return;
	}
	
	m_strLineName = dlg.GetName();
	m_pLineHatch->m_strBaseLinetypeName = m_strLineName;
	
	UpdateData(FALSE);
	
	
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
	return;
	
}

void CDlgLinefillSetting::SetMem(CLineHatch *pLineHatch, ConfigLibItem config)
{
	m_pLineHatch = pLineHatch;
	m_config = config;
	
	if(m_config.pCellDefLib==NULL || m_config.pCellDefLib==NULL)
		return;
	
	m_fAngle = pLineHatch->m_fAngle*180/PI;
	m_fWidth = pLineHatch->m_fWidth;
	m_fIntv = pLineHatch->m_fIntv;
	m_strLineName = pLineHatch->m_strBaseLinetypeName;
	
	//	UpdateData(FALSE);
}
