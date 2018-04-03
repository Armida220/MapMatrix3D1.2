// DlgPrintOption.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgPrintOption.h"
#include "RegDef.h "
#include "RegDef2.h "
#include "SilenceDDX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPrintOption dialog


CDlgPrintOption::CDlgPrintOption(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPrintOption::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPrintOption)
	m_nPrintScope = 0;
	m_nPrintScale = 0;
	m_fDefaultWid = 0.15;
	m_fMarginLeft = 25;
	m_fMarginRight = 25;
	m_fMarginTop = 25;
	m_fMarginBottom = 25;
	m_fExtension = 50;
	//}}AFX_DATA_INIT
	m_bOverImg = FALSE;
}


void CDlgPrintOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPrintOption)
	DDX_Radio(pDX, IDC_RADIO_WORKSPACE, m_nPrintScope);
	DDX_Radio(pDX, IDC_RADIO_ADJUST_TO_PAPER, m_nPrintScale);
	DDX_Text_Silence(pDX,IDC_EDIT_LEFT,m_fMarginLeft);
	DDX_Text_Silence(pDX,IDC_EDIT_RIGHT,m_fMarginRight);
	DDX_Text_Silence(pDX,IDC_EDIT_TOP,m_fMarginTop);
	DDX_Text_Silence(pDX,IDC_EDIT_BOTTOM,m_fMarginBottom);
	DDX_Text_Silence(pDX,IDC_EDIT_EXTENSION,m_fExtension);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPrintOption, CDialog)
	//{{AFX_MSG_MAP(CDlgPrintOption)
	ON_BN_CLICKED(IDC_RADIO_WORKSPACE, OnRadioWorkspace)
	ON_BN_CLICKED(IDC_RADIO_IMG, OnRadioWorkspace)
	ON_BN_CLICKED(IDC_RADIO_CURWIN, OnRadioWorkspace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPrintOption message handlers

void CDlgPrintOption::OnRadioWorkspace() 
{
	UpdateData(TRUE);

	GetDlgItem(IDC_EDIT_EXTENSION)->EnableWindow(m_nPrintScope==0);
}

void CDlgPrintOption::OnOK() 
{
	UpdateData(TRUE);
	
	CDialog::OnOK();
	AfxGetApp()->WriteProfileInt(REGPATH_OTHER,REGITEM_PRINTOPTION_SCOPE,m_nPrintScope);
	AfxGetApp()->WriteProfileInt(REGPATH_OTHER,REGITEM_PRINTOPTION_SCALE,m_nPrintScale);
	WriteProfileDouble(REGPATH_OTHER,REGITEM_PRINTOPTION_WIDE,m_fDefaultWid);

	WriteProfileDouble(REGPATH_OTHER,"PrintMarginLeft",m_fMarginLeft);
	WriteProfileDouble(REGPATH_OTHER,"PrintMarginRight",m_fMarginRight);
	WriteProfileDouble(REGPATH_OTHER,"PrintMarginTop",m_fMarginTop);
	WriteProfileDouble(REGPATH_OTHER,"PrintMarginBottom",m_fMarginBottom);
	WriteProfileDouble(REGPATH_OTHER,"PrintExtension",m_fExtension);
}

BOOL CDlgPrintOption::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_nPrintScope = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_PRINTOPTION_SCOPE,m_nPrintScope);
	m_nPrintScale = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_PRINTOPTION_SCALE,m_nPrintScale);
	m_fDefaultWid = GetProfileDouble(REGPATH_OTHER,REGITEM_PRINTOPTION_WIDE,m_fDefaultWid);

	m_fMarginLeft = GetProfileDouble(REGPATH_OTHER,"PrintMarginLeft",m_fMarginLeft);
	m_fMarginRight = GetProfileDouble(REGPATH_OTHER,"PrintMarginRight",m_fMarginRight);
	m_fMarginTop = GetProfileDouble(REGPATH_OTHER,"PrintMarginTop",m_fMarginTop);
	m_fMarginBottom = GetProfileDouble(REGPATH_OTHER,"PrintMarginBottom",m_fMarginBottom);
	m_fExtension = GetProfileDouble(REGPATH_OTHER,"PrintExtension",m_fExtension);
	
	if (!m_bOverImg)
	{
		GetDlgItem(IDC_RADIO_IMG)->EnableWindow(FALSE);
		if (m_nPrintScope==1)
		{
			m_nPrintScope = 0;
//			((CButton*)GetDlgItem(IDC_RADIO_WORKSPACE))->SetCheck(BST_CHECKED);
		}
	}

	GetDlgItem(IDC_EDIT_EXTENSION)->EnableWindow(m_nPrintScope==0);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
