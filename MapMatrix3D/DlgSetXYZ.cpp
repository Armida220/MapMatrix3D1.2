// DlgSetXYZ.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgSetXYZ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void FormatFloatDDX(CDialog *pdlg, UINT id, double value, int digit)
{
	CString format;
	format.Format("%%.%df",digit);
	CString str;
	CWnd *pCtrl = pdlg->GetDlgItem(id);
	if( pCtrl )
	{
		str.Format(format,value);
		pCtrl->SetWindowText(str);
	}
}

static void DDX_Text_Silence(CDataExchange* pDX, int nIDC, double& value)
{
	if( pDX->m_bSaveAndValidate )
	{
		HWND hCtrl = ::GetDlgItem(pDX->m_pDlgWnd->GetSafeHwnd(),nIDC);
		CWnd *pWnd = CWnd::FromHandle(hCtrl);
		if( pWnd )
		{
			CString text;
			pWnd->GetWindowText(text);
			value = atof(text);
		}
	}
	else
	{
		DDX_Text(pDX,nIDC,value);
	}
}

static void DDX_Text_Silence(CDataExchange* pDX, int nIDC, float& value)
{
	if( pDX->m_bSaveAndValidate )
	{
		HWND hCtrl = ::GetDlgItem(pDX->m_pDlgWnd->GetSafeHwnd(),nIDC);
		CWnd *pWnd = CWnd::FromHandle(hCtrl);
		if( pWnd )
		{
			CString text;
			pWnd->GetWindowText(text);
			value = atof(text);
		}
	}
	else
	{
		DDX_Text(pDX,nIDC,value);
	}
}


static void DDX_Text_Silence(CDataExchange* pDX, int nIDC, int& value)
{
	if( pDX->m_bSaveAndValidate )
	{
		HWND hCtrl = ::GetDlgItem(pDX->m_pDlgWnd->GetSafeHwnd(),nIDC);
		CWnd *pWnd = CWnd::FromHandle(hCtrl);
		if( pWnd )
		{
			CString text;
			pWnd->GetWindowText(text);
			value = atoi(text);
		}
	}
	else
	{
		DDX_Text(pDX,nIDC,value);
	}
}


static void DDX_Text_Silence(CDataExchange* pDX, int nIDC, UINT& value)
{
	if( pDX->m_bSaveAndValidate )
	{
		HWND hCtrl = ::GetDlgItem(pDX->m_pDlgWnd->GetSafeHwnd(),nIDC);
		CWnd *pWnd = CWnd::FromHandle(hCtrl);
		if( pWnd )
		{
			CString text;
			pWnd->GetWindowText(text);
			value = atoi(text);
		}
	}
	else
	{
		DDX_Text(pDX,nIDC,value);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgSetXYZ dialog


CDlgSetXYZ::CDlgSetXYZ(CWnd* pParent /*=NULL*/)
: CDialog(CDlgSetXYZ::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetXYZ)
	m_lfX = 0.0;
	m_lfY = 0.0;
	m_lfZ = 0.0;
	m_bSetX = FALSE;
	m_bSetY = FALSE;
	m_bSetZ = FALSE;
	m_bPlacePt = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgSetXYZ::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetXYZ)
	DDX_Text_Silence(pDX, IDC_SXYZ_EDIT_X, m_lfX);
	DDX_Text_Silence(pDX, IDC_SXYZ_EDIT_Y, m_lfY);
	DDX_Text_Silence(pDX, IDC_SXYZ_EDIT_Z, m_lfZ);
	DDX_Check(pDX, IDC_SXYZ_CHECK_X, m_bSetX);
	DDX_Check(pDX, IDC_SXYZ_CHECK_Y, m_bSetY);
	DDX_Check(pDX, IDC_SXYZ_CHECK_Z, m_bSetZ);
	DDX_Check(pDX, IDC_SXYZ_CHECK_PLACEPT, m_bPlacePt);
	//}}AFX_DATA_MAP
	
	//格式化
	if( !pDX->m_bSaveAndValidate )
	{
		FormatFloatDDX(this,IDC_SXYZ_EDIT_X,m_lfX,4);
		FormatFloatDDX(this,IDC_SXYZ_EDIT_Y,m_lfY,4);
		FormatFloatDDX(this,IDC_SXYZ_EDIT_Z,m_lfZ,4);
	}
}


BEGIN_MESSAGE_MAP(CDlgSetXYZ, CDialog)
//{{AFX_MSG_MAP(CDlgSetXYZ)
ON_EN_CHANGE(IDC_SXYZ_EDIT_X, OnChangeSxyzEditX)
ON_EN_CHANGE(IDC_SXYZ_EDIT_Y, OnChangeSxyzEditY)
ON_EN_CHANGE(IDC_SXYZ_EDIT_Z, OnChangeSxyzEditZ)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetXYZ message handlers

void CDlgSetXYZ::OnChangeSxyzEditX() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_SXYZ_CHECK_X);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	
}

void CDlgSetXYZ::OnChangeSxyzEditY() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_SXYZ_CHECK_Y);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	
}

void CDlgSetXYZ::OnChangeSxyzEditZ() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_SXYZ_CHECK_Z);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	
}

/////////////////////////////////////////////////////////////////////////////
// CDlgSetXYZ1 dialog


CDlgSetXYZ1::CDlgSetXYZ1(CWnd* pParent /*=NULL*/)
: CDialog(CDlgSetXYZ1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetXYZ1)
	m_lfX = 0.0;
	m_lfY = 0.0;
	m_lfZ = 0.0;
	m_bSetX = FALSE;
	m_bSetY = FALSE;
	m_bSetZ = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgSetXYZ1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetXYZ1)
	DDX_Text_Silence(pDX, IDC_SXYZ_EDIT_X, m_lfX);
	DDX_Text_Silence(pDX, IDC_SXYZ_EDIT_Y, m_lfY);
	DDX_Text_Silence(pDX, IDC_SXYZ_EDIT_Z, m_lfZ);
	DDX_Check(pDX, IDC_SXYZ_CHECK_X, m_bSetX);
	DDX_Check(pDX, IDC_SXYZ_CHECK_Y, m_bSetY);
	DDX_Check(pDX, IDC_SXYZ_CHECK_Z, m_bSetZ);
	//}}AFX_DATA_MAP
	
	//格式化
	if( !pDX->m_bSaveAndValidate )
	{
		FormatFloatDDX(this,IDC_SXYZ_EDIT_X,m_lfX,4);
		FormatFloatDDX(this,IDC_SXYZ_EDIT_Y,m_lfY,4);
		FormatFloatDDX(this,IDC_SXYZ_EDIT_Z,m_lfZ,4);
	}
}


BEGIN_MESSAGE_MAP(CDlgSetXYZ1, CDialog)
//{{AFX_MSG_MAP(CDlgSetXYZ1)
ON_EN_CHANGE(IDC_SXYZ_EDIT_X, OnChangeSxyzEditX)
ON_EN_CHANGE(IDC_SXYZ_EDIT_Y, OnChangeSxyzEditY)
ON_EN_CHANGE(IDC_SXYZ_EDIT_Z, OnChangeSxyzEditZ)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetXYZ1 message handlers

void CDlgSetXYZ1::OnChangeSxyzEditX() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_SXYZ_CHECK_X);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	
}

void CDlgSetXYZ1::OnChangeSxyzEditY() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_SXYZ_CHECK_Y);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	
}

void CDlgSetXYZ1::OnChangeSxyzEditZ() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_SXYZ_CHECK_Z);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	
}

