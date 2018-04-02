// DlgOverlayBound.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgOverlayBound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayBound dialog


CDlgOverlayBound::CDlgOverlayBound(CWnd* pParent /*=NULL*/)
: CDialog(CDlgOverlayBound::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOverlayBound)
	m_bImgView = TRUE;
	m_lfZ1 = 0;
	m_lfZ2 = 0;
	m_lfZ3 = 0;
	m_lfZ4 = 0;
	m_color = RGB(255,0,0);
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgOverlayBound::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOverlayBound)
	DDX_Text(pDX, IDC_EDIT_SWSB_X1, m_lfX1);
	DDX_Text(pDX, IDC_EDIT_SWSB_X2, m_lfX2);
	DDX_Text(pDX, IDC_EDIT_SWSB_X5, m_lfX3);
	DDX_Text(pDX, IDC_EDIT_SWSB_X4, m_lfX4);
	DDX_Text(pDX, IDC_EDIT_SWSB_Y1, m_lfY1);
	DDX_Text(pDX, IDC_EDIT_SWSB_Y2, m_lfY2);
	DDX_Text(pDX, IDC_EDIT_SWSB_Y3, m_lfY3);
	DDX_Text(pDX, IDC_EDIT_SWSB_Y4, m_lfY4);
	DDX_Text(pDX, IDC_EDIT_SWSB_Z1, m_lfZ1);
	DDX_Text(pDX, IDC_EDIT_SWSB_Z2, m_lfZ2);
	DDX_Text(pDX, IDC_EDIT_SWSB_Z3, m_lfZ3);
	DDX_Text(pDX, IDC_EDIT_SWSB_Z4, m_lfZ4);
	DDX_Check(pDX, IDC_CHECK_OG_IMGVIEW, m_bImgView);
	DDX_Control(pDX, IDC_BUTTON_OG_COLOR, m_colorBtn);
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOverlayBound, CDialog)
//{{AFX_MSG_MAP(CDlgOverlayBound)
ON_EN_CHANGE(IDC_EDIT_SWSB_Z1, OnChangeEditZ1)
ON_EN_CHANGE(IDC_EDIT_SWSB_X2, OnChangeEditSwsbX2)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayBound message handlers

BOOL CDlgOverlayBound::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_colorBtn.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_colorBtn.SetColor(m_color);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgOverlayBound::OnOK() 
{
	m_color = m_colorBtn.GetColor();
	CDialog::OnOK();
}

void CDlgOverlayBound::OnChangeEditZ1() 
{
	CString strText;
	CWnd *pWnd = GetDlgItem(IDC_EDIT_SWSB_Z1);
	if( pWnd )
	{
		pWnd->GetWindowText(strText);
		
		pWnd = GetDlgItem(IDC_EDIT_SWSB_Z2);
		if( pWnd )pWnd->SetWindowText(strText);
		
		pWnd = GetDlgItem(IDC_EDIT_SWSB_Z3);
		if( pWnd )pWnd->SetWindowText(strText);
		
		pWnd = GetDlgItem(IDC_EDIT_SWSB_Z4);
		if( pWnd )pWnd->SetWindowText(strText);
	}
	
}

void CDlgOverlayBound::OnChangeEditSwsbX2() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

