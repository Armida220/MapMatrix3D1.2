// DlgImportXYZ.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgImportXYZ.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IMPORTXYZ_SETTINGS  "ImportXYZ_Settings"

/////////////////////////////////////////////////////////////////////////////
// CDlgImportXYZ dialog


CDlgImportXYZ::CDlgImportXYZ(CWnd* pParent /*=NULL*/)
: CDialog(CDlgImportXYZ::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportXYZ)
	m_strFilePath = _T("");
	 m_strMapName = _T("");
	//}}AFX_DATA_INIT
}


void CDlgImportXYZ::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportXYZ)
	DDX_Text(pDX, IDC_EDIT_IC_FILEPATH, m_strFilePath);
	DDX_Text(pDX, IDC_EDIT_MAPNAME, m_strMapName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportXYZ, CDialog)
//{{AFX_MSG_MAP(CDlgImportXYZ)
ON_BN_CLICKED(IDC_BUTTON_IC_BROWSE, OnFileBrowse)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportXYZ message handlers

void CDlgImportXYZ::OnFileBrowse() 
{
	CString filter(StrFromResID(IDS_LOADXYZ_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	m_strFilePath = dlg.GetPathName();

	//Í¼·ùÃû
	m_strMapName = dlg.GetFileTitle();
	UpdateData(FALSE);
}

BOOL CDlgImportXYZ::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgImportXYZ::OnOK() 
{
	UpdateData(TRUE);
	CDialog::OnOK();
}
