// Op_Page_QT2.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "OptionsPages.h"
#include "Op_Page_QT2.h"
#include "RegDef.h "
#include "RegDef2.h "

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COp_Page_QT2 property page

IMPLEMENT_DYNCREATE(COp_Page_QT2, COp_Page_Base)

COp_Page_QT2::COp_Page_QT2() : COp_Page_Base(COp_Page_QT2::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_QT2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_lfSamePtsInlineToler = 0.1;
	m_lfOverlapLineToler = 0.1;
	m_lfOverlapPntToler = 0.1;
}

COp_Page_QT2::~COp_Page_QT2()
{
}

void COp_Page_QT2::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_QT2)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_SAMEPTINLINE_TOLER, m_lfSamePtsInlineToler);	
	DDX_Text(pDX, IDC_EDIT_OVERLAPLINE_TOLER, m_lfOverlapLineToler);
}


BEGIN_MESSAGE_MAP(COp_Page_QT2, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_QT2)
	ON_EN_CHANGE(IDC_EDIT_SAMEPTINLINE_TOLER, OnCheckModified)
	ON_EN_CHANGE(IDC_EDIT_OVERLAPLINE_TOLER, OnCheckModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_QT2 message handlers

void COp_Page_QT2::OnCheckModified() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}

BOOL COp_Page_QT2::OnInitDialog() 
{
	COp_Page_Base::OnInitDialog();
	
	m_lfOverlapPntToler = GetProfileDouble(REGPATH_QT,REGITEM_QT_OVERLAPPTTOLER,m_lfOverlapPntToler);
	int nTemp = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_OVERLAPPTCROSSLAY,0);
	m_checkBtnPtCrossLay.SetCheck(nTemp);
	nTemp = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_OVERLAPPTZ,0);
	m_checkBtnPtZ.SetCheck(nTemp);

	m_lfOverlapLineToler = GetProfileDouble(REGPATH_QT,REGITEM_QT_OVERLAPLINETOLER,m_lfOverlapLineToler);
	m_lfSamePtsInlineToler	= GetProfileDouble(REGPATH_QT,REGITEM_QT_SAMEPTINLINETOLER,m_lfSamePtsInlineToler);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COp_Page_QT2::OnOK() 
{	
	COp_Page_Base::OnOK();
	
	WriteProfileDouble(REGPATH_QT,REGITEM_QT_OVERLAPPTTOLER,m_lfOverlapPntToler);
	int nTemp = m_checkBtnPtCrossLay.GetCheck();
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_OVERLAPPTCROSSLAY,nTemp);
	nTemp = m_checkBtnPtZ.GetCheck();
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_OVERLAPPTZ,nTemp);

	WriteProfileDouble(REGPATH_QT,REGITEM_QT_OVERLAPLINETOLER,m_lfOverlapLineToler);
	WriteProfileDouble(REGPATH_QT,REGITEM_QT_SAMEPTINLINETOLER,m_lfSamePtsInlineToler);
}
