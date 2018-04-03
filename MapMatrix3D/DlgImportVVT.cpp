// DlgImportVVT.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgImportVVT.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IMPORTVVT_SETTINGS  "ImportVVT_Settings"

/////////////////////////////////////////////////////////////////////////////
// CDlgImportVVT dialog


CDlgImportVVT::CDlgImportVVT(CWnd* pParent /*=NULL*/)
: CDialog(CDlgImportVVT::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportVVT)
	m_strFilePath = _T("");
	m_strMapName = _T("");
	//}}AFX_DATA_INIT
}


void CDlgImportVVT::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportVVT)
	DDX_Text(pDX, IDC_EDIT_IC_FILEPATH, m_strFilePath);
	DDX_Text(pDX, IDC_EDIT_MAPNAME, m_strMapName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportVVT, CDialog)
//{{AFX_MSG_MAP(CDlgImportVVT)
ON_BN_CLICKED(IDC_BUTTON_IC_BROWSE, OnFileBrowse)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportVVT message handlers

void CDlgImportVVT::OnFileBrowse() 
{
	CString filter(StrFromResID(IDS_LOADVVT_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	m_strFilePath = dlg.GetPathName();

	//Í¼·ùÃû
	m_strMapName = dlg.GetFileTitle();
	UpdateData(FALSE);
}

BOOL CDlgImportVVT::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgImportVVT::OnOK() 
{
	UpdateData(TRUE);
	CDialog::OnOK();
}
