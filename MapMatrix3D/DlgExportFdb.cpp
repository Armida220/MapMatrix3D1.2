// DlgExportFdb.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgExportFdb.h"
#include "UIFFileDialogEx.h "

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportFdb dialog


CDlgExportFdb::CDlgExportFdb(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportFdb::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportFdb)
	m_strFilePath = _T("");
	m_nOption = 0;
	//}}AFX_DATA_INIT
}


void CDlgExportFdb::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportFdb)
	DDX_Text(pDX, IDC_EDIT_FDB_PATH, m_strFilePath);
	DDX_Radio(pDX, IDC_RADIO_VISIBLE, m_nOption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportFdb, CDialog)
	//{{AFX_MSG_MAP(CDlgExportFdb)
	ON_BN_CLICKED(IDC_BUTTON_FDB_BROWSER, OnButtonFdbBrowser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportFdb message handlers

void CDlgExportFdb::OnButtonFdbBrowser() 
{
	CString filter(StrFromResID(IDS_LOADMDB_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strFilePath = dlg.GetPathName();
	UpdateData(FALSE);
	
}
