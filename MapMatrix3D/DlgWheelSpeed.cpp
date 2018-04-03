// DlgWheelSpeed.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgWheelSpeed.h"
#include "silenceddx.h"
#include "RegDef2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWheelSpeed dialog



CDlgWheelSpeed::CDlgWheelSpeed(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWheelSpeed::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWheelSpeed)
	m_fSpeed = 1.0f;
	//}}AFX_DATA_INIT
}


void CDlgWheelSpeed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWheelSpeed)
	DDX_Text_Silence(pDX, IDC_EDIT_SPEED, m_fSpeed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWheelSpeed, CDialog)
	//{{AFX_MSG_MAP(CDlgWheelSpeed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWheelSpeed message handlers

BOOL CDlgWheelSpeed::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_fSpeed = GetProfileDouble(REGPATH_CONFIG,REGITEM_WHEELSPEED,m_fSpeed);
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgWheelSpeed::OnOK()
{
	UpdateData(TRUE);
	WriteProfileDouble(REGPATH_CONFIG,REGITEM_WHEELSPEED,m_fSpeed);
	CDialog::OnOK();
}


void CDlgWheelSpeed::OnCancel()
{
	CDialog::OnCancel();
}