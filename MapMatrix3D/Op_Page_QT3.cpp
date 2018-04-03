// Op_Page_QT3.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "OptionsPages.h"
#include "Op_Page_QT3.h"
#include "RegDef.h "
#include "RegDef2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COp_Page_QT3 dialog
IMPLEMENT_DYNCREATE(COp_Page_QT3, COp_Page_Base)

COp_Page_QT3::COp_Page_QT3() : COp_Page_Base(COp_Page_QT3::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_QT3)
	m_lfSusToler = 5.0;
	m_lfSnapToler = 1.0;
	m_lfPerToler = 1.0;
	m_lfXYMatchToler = 1.0;
	m_lfZMatchToler = 1.0;
	//}}AFX_DATA_INIT
}


void COp_Page_QT3::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_QT3)
	DDX_Control(pDX, IDC_CHECK_HANDEL_VEP, m_checkBtnSusPtVEP);
	DDX_Control(pDX, IDC_CHECK_MODIFYZ, m_checkBtnSusZ);
	DDX_Control(pDX, IDC_CHECK_CROSSLAYER2, m_checkBtn2);
	DDX_Control(pDX, IDC_CHECK_CROSSLAYER, m_checkBtn);
	DDX_Text(pDX, IDC_EDIT_SUS_TOLER, m_lfSusToler);
	DDX_Text(pDX, IDC_EDIT_SNAP_TOLER, m_lfSnapToler);
	DDX_Text(pDX, IDC_EDIT_PER_TOLER, m_lfPerToler);
	DDX_Text(pDX, IDC_EDIT_MATCH_XYTOLER, m_lfXYMatchToler);
	DDX_Text(pDX, IDC_EDIT_MATCH_ZTOLER, m_lfZMatchToler);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COp_Page_QT3, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_QT3)	
ON_BN_CLICKED(IDC_CHECK_HANDEL_VEP, OnCheckModified)
ON_BN_CLICKED(IDC_CHECK_MODIFYZ, OnCheckModified)
ON_BN_CLICKED(IDC_CHECK_CROSSLAYER2, OnCheckModified)
ON_BN_CLICKED(IDC_CHECK_CROSSLAYER, OnCheckModified)
ON_EN_CHANGE(IDC_EDIT_SUS_TOLER, OnCheckModified)
ON_EN_CHANGE(IDC_EDIT_SNAP_TOLER, OnCheckModified)
ON_EN_CHANGE(IDC_EDIT_PER_TOLER, OnCheckModified)
ON_EN_CHANGE(IDC_EDIT_MATCH_XYTOLER, OnCheckModified)
ON_EN_CHANGE(IDC_EDIT_MATCH_ZTOLER, OnCheckModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_QT3 message handlers

void COp_Page_QT3::OnCheckModified() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}

BOOL COp_Page_QT3::OnInitDialog() 
{
	COp_Page_Base::OnInitDialog();
	
	m_lfSusToler  = GetProfileDouble(REGPATH_QT,REGITEM_QT_SUSTOLER,m_lfSusToler);	
	m_lfPerToler  = GetProfileDouble(REGPATH_QT,REGITEM_QT_PERTOLER,m_lfPerToler);	
	m_lfSnapToler = GetProfileDouble(REGPATH_QT,REGITEM_QT_SNAPTOLER,m_lfSnapToler);
	int nTemp = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SNAP_CROSSLAY,0);
	m_checkBtn.SetCheck(nTemp);
	nTemp = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SNAP_MODIFYZ,1);
	m_checkBtnSusZ.SetCheck(nTemp);
	nTemp = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SNAP_VEP,1);
	m_checkBtnSusPtVEP.SetCheck(nTemp);
	
	m_lfXYMatchToler = GetProfileDouble(REGPATH_QT,REGITEM_QT_XYMATCHTOLER,m_lfXYMatchToler);
	m_lfZMatchToler = GetProfileDouble(REGPATH_QT,REGITEM_QT_ZMATCHTOLER,m_lfZMatchToler);
	nTemp = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_MATCH_CROSSLAY,0);
	m_checkBtn2.SetCheck(nTemp);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COp_Page_QT3::OnOK()
{
	COp_Page_Base::OnOK();
	WriteProfileDouble(REGPATH_QT,REGITEM_QT_SUSTOLER,m_lfSusToler);
	WriteProfileDouble(REGPATH_QT,REGITEM_QT_PERTOLER,m_lfPerToler);
	WriteProfileDouble(REGPATH_QT,REGITEM_QT_SNAPTOLER,m_lfSnapToler);
	int nTemp = m_checkBtn.GetCheck();
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_SNAP_CROSSLAY,nTemp);
	nTemp = m_checkBtnSusZ.GetCheck();
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_SNAP_MODIFYZ,nTemp);
	nTemp = m_checkBtnSusPtVEP.GetCheck();
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_SNAP_VEP,nTemp);

	WriteProfileDouble(REGPATH_QT,REGITEM_QT_XYMATCHTOLER,m_lfXYMatchToler);
	WriteProfileDouble(REGPATH_QT,REGITEM_QT_ZMATCHTOLER,m_lfZMatchToler);
	nTemp = m_checkBtn2.GetCheck();
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_MATCH_CROSSLAY,nTemp);
}
