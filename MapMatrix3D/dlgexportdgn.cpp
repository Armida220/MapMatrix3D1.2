// dlgexportdgn.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "UIFFileDialogEx.h"
#include "dlgexportdgn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportDGN dialog
static CString lstFile = _T("");
static CString strPath = _T("");
static BOOL    bInit   = FALSE;


CDlgExportDGN::CDlgExportDGN(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportDGN::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportDGN)
		// NOTE: the ClassWizard will add member initialization here
	m_strPath = _T("");
	//}}AFX_DATA_INIT
	
	if( !bInit )
	{
		bInit = TRUE;
		lstFile = StrFromResID(IDS_NOLISTFILE);
		strPath = _T("");
	}
	
	m_strPath = strPath;
	m_strLstFile = lstFile;
}


void CDlgExportDGN::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportDGN)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH2, m_strLstFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportDGN, CDialog)
	//{{AFX_MSG_MAP(CDlgExportDGN)
		// NOTE: the ClassWizard will add message map macros here
		ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnButtonIdBrowse)
		ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE2, OnBrowseLst)
		ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH2, OnSetfocusEditLstFile)
		ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH2, OnKillfocusEditLstFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportDGN message handlers
void CDlgExportDGN::OnButtonIdBrowse() 
{
	CString filter("Dgn(*.dgn)|*.dgn|All Files(*.*)|*.*||");
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, "*.dgn", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strPath = dlg.GetPathName();
	UpdateData(FALSE);
}


void CDlgExportDGN::OnOK() 
{
	UpdateData(TRUE);
	
	lstFile = m_strLstFile;
	strPath = m_strPath;
	
	CDialog::OnOK();
	
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile = _T("");
}



void CDlgExportDGN::OnBrowseLst() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile = dlg.GetPathName();
	UpdateData(FALSE);		
}



void CDlgExportDGN::OnSetfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgExportDGN::OnKillfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.GetLength()<=0 )
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}	
}