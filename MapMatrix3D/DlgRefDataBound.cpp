// DlgRefDataBound.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgRefDataBound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRefDataBound dialog


CDlgRefDataBound::CDlgRefDataBound(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRefDataBound::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRefDataBound)
	m_fBoundExt = 0.0f;
	m_nOption = -1;
	//}}AFX_DATA_INIT
}


void CDlgRefDataBound::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRefDataBound)
	DDX_Text(pDX, IDC_EDIT_EXTENSION, m_fBoundExt);
	DDX_Radio(pDX, IDC_RADIO_ALL, m_nOption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRefDataBound, CDialog)
	//{{AFX_MSG_MAP(CDlgRefDataBound)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnRadioAll)
	ON_BN_CLICKED(IDC_RADIO_BOUND, OnRadioBound)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRefDataBound message handlers

void CDlgRefDataBound::OnOK() 
{
	UpdateData();
	
	CDialog::OnOK();
}

void CDlgRefDataBound::OnRadioAll() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_EXTENSION)->EnableWindow(FALSE);
	
}

void CDlgRefDataBound::OnRadioBound() 
{
	GetDlgItem(IDC_EDIT_EXTENSION)->EnableWindow(TRUE);
	
}

BOOL CDlgRefDataBound::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_nOption==0)
		GetDlgItem(IDC_EDIT_EXTENSION)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_EDIT_EXTENSION)->EnableWindow(TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
