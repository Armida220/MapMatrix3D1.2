// DlgDeleteSame.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgDeleteSame.h"
#include "DlgSelectLayer.h"
#include "RegDef2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgDeleteSame dialog


CDlgDeleteSame::CDlgDeleteSame(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDeleteSame::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDeleteSame)
	m_bCrossLayer = FALSE;
	m_strLayer = _T("");
	m_bSameShape = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgDeleteSame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgDeleteSame)
	DDX_Text(pDX, IDC_EDIT1, m_strLayer);
	DDX_Radio(pDX, IDC_RADIO1,m_bSameShape);
	DDX_Check(pDX, IDC_CHECK1, m_bCrossLayer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDeleteSame, CDialog)
	//{{AFX_MSG_MAP(DlgDeleteSame)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgDeleteSame message handlers

BOOL CDlgDeleteSame::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bCrossLayer = AfxGetApp()->GetProfileInt(REGPATH_QT,_T("DeleteSameCrossLay"),m_bCrossLayer);
	m_bSameShape = AfxGetApp()->GetProfileInt(REGPATH_QT,_T("DeleteSameSameShape"),m_bSameShape);
	m_strLayer  = AfxGetApp()->GetProfileString(REGPATH_QT,_T("DeleteSamelayers"),m_strLayer);
	
	UpdateData(FALSE);
	
	return TRUE;
}

void CDlgDeleteSame::OnOK() 
{
	UpdateData(TRUE);
	
	AfxGetApp()->WriteProfileInt(REGPATH_QT,_T("DeleteSameCrossLay"),m_bCrossLayer);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,_T("DeleteSameSameShape"),m_bSameShape);
	AfxGetApp()->WriteProfileString(REGPATH_QT,_T("DeleteSamelayers"),m_strLayer);
	
	CDialog::OnOK();
}

void CDlgDeleteSame::OnButton1() 
{
	// TODO: Add your control notification handler code here
	if( !m_pDS )
		return;
	
	UpdateData(TRUE);
	
	CDlgSelectFtrLayer dlg(NULL,LAYER_SEL_MODE_MUTISEL);
	dlg.m_pDS = m_pDS;	
	dlg.m_bUsed = TRUE;
	dlg.m_bLocal = TRUE;
	dlg.m_bNotLocal = TRUE;
	dlg.m_strLayers = m_strLayer;
	
	if( dlg.DoModal()!=IDOK )
		return;
	
	m_strLayer = dlg.m_strLayers;
	
	UpdateData(FALSE);
}
