// DlgStretchImg.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgStretchImg.h"
#include "RegDef.h "
#include "RegDef2.h "
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGITEM_STRETCHIMG_XSCALE	_T("StretchImgXscale")
#define REGITEM_STRETCHIMG_YSCALE	_T("StretchImgYscale")
#define REGITEM_STRETCHIMG_ANGLE	_T("StretchImgAngle")

#define REGITEM_STRETCHIMG_RIGHTSAME	_T("StretchRightSame")

#define REGITEM_STRETCHIMG_XSCALE2	_T("StretchImgXscale2")
#define REGITEM_STRETCHIMG_YSCALE2	_T("StretchImgYscale2")
#define REGITEM_STRETCHIMG_ANGLE2	_T("StretchImgAngle2")

/////////////////////////////////////////////////////////////////////////////
// CDlgStretchImg dialog


CDlgStretchImg::CDlgStretchImg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStretchImg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgStretchImg)
	m_nAngIndex = 0;
	m_fXScale = 1.0f;
	m_fYScale = 1.0f;
	m_nAngIndex2 = 0;
	m_fXScale2 = 1.0f;
	m_fYScale2 = 1.0f;
	m_bRightSame = FALSE;
	//}}AFX_DATA_INIT
	m_nAngle = 0;
	m_nAngle2 = 0;
}


void CDlgStretchImg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgStretchImg)
	DDX_CBIndex(pDX, IDC_COMBO_ANGLE, m_nAngIndex);
	DDX_Text(pDX, IDC_EDIT_XSCALE, m_fXScale);
	DDX_Text(pDX, IDC_EDIT_YSCALE, m_fYScale);
	DDX_CBIndex(pDX, IDC_COMBO_ANGLE2, m_nAngIndex2);
	DDX_Text(pDX, IDC_EDIT_XSCALE2, m_fXScale2);
	DDX_Text(pDX, IDC_EDIT_YSCALE2, m_fYScale2);
	DDX_Check(pDX, IDC_CHECK_RIGHT, m_bRightSame);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgStretchImg, CDialog)
	//{{AFX_MSG_MAP(CDlgStretchImg)
	ON_BN_CLICKED(IDC_CHECK_RIGHT, OnCheckRight)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgStretchImg message handlers

void CDlgStretchImg::OnOK() 
{
	UpdateData(TRUE);
	CComboBox *pBox = (CComboBox*)GetDlgItem(IDC_COMBO_ANGLE);
	if( pBox )
	{
		int pos = pBox->GetCurSel();
		if( pos>=0 )
		{
			CString strText;
			pBox->GetLBText(pos,strText);
			m_nAngle = atol(strText);
		}
	}

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_ANGLE2);
	if( pBox )
	{
		int pos = pBox->GetCurSel();
		if( pos>=0 )
		{
			CString strText;
			pBox->GetLBText(pos,strText);
			m_nAngle2 = atol(strText);
		}
	}

	CDialog::OnOK();

	if( m_bRightSame )
	{
		m_nAngIndex2 = m_nAngIndex;
		m_fXScale2 = m_fXScale;
		m_fYScale2 = m_fYScale;
		m_nAngle2 = m_nAngle;
	}

	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_ANGLE,m_nAngle);	
	WriteProfileDouble(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_XSCALE,m_fXScale);
	WriteProfileDouble(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_YSCALE,m_fYScale);

	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_RIGHTSAME,m_bRightSame);	

	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_ANGLE2,m_nAngle2);	
	WriteProfileDouble(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_XSCALE2,m_fXScale2);
	WriteProfileDouble(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_YSCALE2,m_fYScale2);
}

BOOL CDlgStretchImg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	m_nAngle = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_ANGLE,m_nAngle);	
	m_fXScale = GetProfileDouble(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_XSCALE,m_fXScale);
	m_fYScale = GetProfileDouble(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_YSCALE,m_fYScale);
	m_nAngIndex = 0;

	CComboBox *pBox = (CComboBox*)GetDlgItem(IDC_COMBO_ANGLE);
	if( pBox )
	{
		for( int i=0; i<pBox->GetCount(); i++)
		{
			CString strText;
			pBox->GetLBText(i,strText);
			if( m_nAngle==atol(strText) )
			{
				m_nAngIndex = i;
				break;
			}
		}
	}

	m_bRightSame = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_RIGHTSAME,m_bRightSame);	

	m_nAngle2 = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_ANGLE2,m_nAngle2);	
	m_fXScale2 = GetProfileDouble(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_XSCALE2,m_fXScale2);
	m_fYScale2 = GetProfileDouble(REGPATH_VIEW_IMG,REGITEM_STRETCHIMG_YSCALE2,m_fYScale2);
	m_nAngIndex2 = 0;
	
	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_ANGLE2);
	if( pBox )
	{
		for( int i=0; i<pBox->GetCount(); i++)
		{
			CString strText;
			pBox->GetLBText(i,strText);
			if( m_nAngle2==atol(strText) )
			{
				m_nAngIndex2 = i;
				break;
			}
		}
	}

	if( m_bRightSame )
	{
		m_nAngIndex2 = m_nAngIndex;
		m_fXScale2 = m_fXScale;
		m_fYScale2 = m_fYScale;
		m_nAngle2 = m_nAngle;
	}

	UpdateData(FALSE);

	if( m_bRightSame )
	{
		CWnd *pWnd = GetDlgItem(IDC_COMBO_ANGLE2);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_EDIT_XSCALE2);
		if( pWnd )pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_EDIT_YSCALE2);
		if( pWnd )pWnd->EnableWindow(FALSE);
	}
	else
	{
		CWnd *pWnd = GetDlgItem(IDC_COMBO_ANGLE2);
		if( pWnd )pWnd->EnableWindow(TRUE);
		
		pWnd = GetDlgItem(IDC_EDIT_XSCALE2);
		if( pWnd )pWnd->EnableWindow(TRUE);
		
		pWnd = GetDlgItem(IDC_EDIT_YSCALE2);
		if( pWnd )pWnd->EnableWindow(TRUE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgStretchImg::OnCheckRight() 
{
	UpdateData(TRUE);
	if( m_bRightSame )
	{
		CWnd *pWnd = GetDlgItem(IDC_COMBO_ANGLE2);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_XSCALE2);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_YSCALE2);
		if( pWnd )pWnd->EnableWindow(FALSE);
	}
	else
	{
		CWnd *pWnd = GetDlgItem(IDC_COMBO_ANGLE2);
		if( pWnd )pWnd->EnableWindow(TRUE);
		
		pWnd = GetDlgItem(IDC_EDIT_XSCALE2);
		if( pWnd )pWnd->EnableWindow(TRUE);
		
		pWnd = GetDlgItem(IDC_EDIT_YSCALE2);
		if( pWnd )pWnd->EnableWindow(TRUE);
	}
}

void CDlgStretchImg::OnCancel() 
{
	CDialog::OnCancel();

	m_nAngIndex = 0;
	m_fXScale = 1.0f;
	m_fYScale = 1.0f;
	m_nAngIndex2 = 0;
	m_fXScale2 = 1.0f;
	m_fYScale2 = 1.0f;
	m_bRightSame = FALSE;
	m_nAngle = 0;
	m_nAngle2 = 0;
}
