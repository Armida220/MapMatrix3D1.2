// Op_Page_QT1.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "OptionsPages.h"
#include "Op_Page_QT1.h"
#include "RegDef.h "
#include "RegDef2.h "


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COp_Page_QT1 dialog
IMPLEMENT_DYNCREATE(COp_Page_QT1, COp_Page_Base)

COp_Page_QT1::COp_Page_QT1() : COp_Page_Base(COp_Page_QT1::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_QT1)
	m_nSusPntRadius = 1;
	m_nPsePntRadius = 1;
	m_nSusMode = 0;
	//}}AFX_DATA_INIT
}


void COp_Page_QT1::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_QT1)
	DDX_Control(pDX, IDC_CHECK_VEP, m_checkBtnSusVEP);
	DDX_Control(pDX, IDC_CHECK_PSE_CROSSLAY, m_checkBtnPseCrossLay);
	DDX_Control(pDX, IDC_CHECK_PSE_Z, m_checkBtnPseZ);
	DDX_Control(pDX, IDC_CHECK_SUS_CROSSLAY, m_checkBtnSusCrossLay);
	DDX_Control(pDX, IDC_CHECK_SUS_Z, m_checkBtnSusZ);
	DDX_Control(pDX, IDC_BUTTON_PSEPT_COLOR, m_colorPsePnt);
	DDX_Control(pDX, IDC_BUTTON_SUSPT_COLOR, m_colorSusPnt);
	DDX_Text(pDX, IDC_EDIT_SUSPT_RADIUS, m_nSusPntRadius);
	DDX_Text(pDX, IDC_EDIT_PSEPT_RADIUS, m_nPsePntRadius);
	DDX_Radio(pDX, IDC_RADIO_EV, m_nSusMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COp_Page_QT1, COp_Page_Base)
//{{AFX_MSG_MAP(COp_Page_QT1)
ON_EN_CHANGE(IDC_EDIT_SUSPT_RADIUS, OnCheckModified)
ON_EN_CHANGE(IDC_EDIT_PSEPT_RADIUS, OnCheckModified)
ON_BN_CLICKED(IDC_CHECK_VEP, OnCheckModified)
ON_BN_CLICKED(IDC_CHECK_PSE_CROSSLAY, OnCheckModified)
ON_BN_CLICKED(IDC_CHECK_PSE_Z, OnCheckModified)
ON_BN_CLICKED(IDC_CHECK_SUS_CROSSLAY, OnCheckModified)
ON_BN_CLICKED(IDC_CHECK_SUS_Z, OnCheckModified)
ON_BN_CLICKED(IDC_BUTTON_PSEPT_COLOR, OnCheckModified)
ON_BN_CLICKED(IDC_BUTTON_SUSPT_COLOR, OnCheckModified)
ON_BN_CLICKED(IDC_RADIO_EV, OnCheckModified)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_QT1 message handlers

void COp_Page_QT1::OnCheckModified() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}

BOOL COp_Page_QT1::OnInitDialog() 
{
	COp_Page_Base::OnInitDialog();
	
	COLORREF clrSusPt = RGB(0,255,0);
	COLORREF clrPsePt = RGB(0,0,255);
	m_nSusPntRadius	= AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTRADIUS,m_nSusPntRadius);
	clrSusPt  = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTCOLOR,clrSusPt);
	m_nSusMode = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTMODE,m_nSusMode);
	int nTemp = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTZ,0);
	m_checkBtnSusZ.SetCheck(nTemp);
	nTemp = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTCROSSLAY,0);
	m_checkBtnSusCrossLay.SetCheck(nTemp);
	nTemp = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTVEP,0);
	m_checkBtnSusVEP.SetCheck(nTemp);

	m_nPsePntRadius	= AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_PSEPTRADIUS,m_nPsePntRadius);
	clrPsePt  = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_PSEPTCOLOR,clrPsePt);
	nTemp = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_PSEPTZ,0);
	m_checkBtnPseZ.SetCheck(nTemp);
	nTemp = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_PSEPTCROSSLAY,0);
	m_checkBtnPseCrossLay.SetCheck(nTemp);
	
	m_colorSusPnt.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	
	m_colorPsePnt.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_colorSusPnt.SetColor(clrSusPt);
	m_colorPsePnt.SetColor(clrPsePt);
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COp_Page_QT1::OnOK() 
{
	COp_Page_Base::OnOK();
	COLORREF clrSusPt = m_colorSusPnt.GetColor();
	COLORREF clrPsePt = m_colorPsePnt.GetColor();
	
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_SUSPTRADIUS,m_nSusPntRadius);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_SUSPTCOLOR,clrSusPt);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_SUSPTMODE,m_nSusMode);
	int nTemp = m_checkBtnSusZ.GetCheck();
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_SUSPTZ,nTemp);
	nTemp = m_checkBtnSusCrossLay.GetCheck();
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_SUSPTCROSSLAY,nTemp);
	nTemp = m_checkBtnSusVEP.GetCheck();
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_SUSPTVEP,nTemp);	
	
	
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_PSEPTRADIUS,m_nPsePntRadius);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_PSEPTCOLOR,clrPsePt);
	nTemp = m_checkBtnPseZ.GetCheck();
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_PSEPTZ,nTemp);
	nTemp = m_checkBtnPseCrossLay.GetCheck();
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_PSEPTCROSSLAY,nTemp);

}
