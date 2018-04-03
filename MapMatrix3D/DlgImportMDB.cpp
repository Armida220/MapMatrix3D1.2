// DlgImportJB.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgImportMDB.h"
#include "SmartViewFunctions.h"
#include "UIFFileDialogEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImportJB dialog

static CString strPath = _T("");

extern BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] 窗口标题
						   LPTSTR lpszPath,		// [out] 返回的文件路径
						   LPCTSTR lpszInitDir,	// [in] 初始文件路径
						   HWND hWnd,				// [in] 父系窗口
						   BOOL bNetwork=FALSE,	// [in] 限制在网络路径范围内
						   UINT ulFlags=BIF_RETURNFSANCESTORS | BIF_EDITBOX |BIF_BROWSEINCLUDEFILES );// [in] 设置风格


CDlgImportMDB::CDlgImportMDB(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportMDB::IDD, pParent)
{
	m_strFilePath = strPath;
	m_strKeyFields = AfxGetApp()->GetProfileString("IO32\\ImportMDB_Settings", "strKeyFields", "CODE");
	m_strLstFile = AfxGetApp()->GetProfileString("IO32\\ImportMDB_Settings", "strLstFile", _T(""));
	m_strMapName = _T("");

	m_nType = -1;
	if (m_strLstFile.IsEmpty())
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
	}
}


void CDlgImportMDB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportJB)
	DDX_Text(pDX, IDC_EDIT_IC_FILEPATH, m_strFilePath);
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH2, m_strLstFile);
	DDX_Text(pDX, IDC_EDIT_IC_EXCODE, m_strMapName);
	DDX_Text(pDX, IDC_EDIT1, m_strKeyFields);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportMDB, CDialog)
	//{{AFX_MSG_MAP(CDlgImportJB)
	ON_BN_CLICKED(IDC_BUTTON_MDB_BROWSE, OnButtonIcBrowse)
	ON_BN_CLICKED(IDC_BUTTON_MDB_BROWSE2, OnButtonIdBrowse2)
	ON_EN_SETFOCUS(IDC_EDIT_ID_FILEPATH2, OnSetfocusEditIdFilepath2)
	ON_EN_KILLFOCUS(IDC_EDIT_ID_FILEPATH2, OnKillfocusEditIdFilepath2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportJB message handlers

void CDlgImportMDB::OnButtonIcBrowse() 
{
	CString retPath;
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_FILE),retPath.GetBuffer(256),strPath,GetSafeHwnd()) )
		return;
	
	UpdateData(TRUE);
	m_strFilePath = retPath;
	UpdateData(FALSE);
}

void CDlgImportMDB::OnButtonIdBrowse2() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile = dlg.GetPathName();
	UpdateData(FALSE);		
}

void CDlgImportMDB::OnOK() 
{
	UpdateData(TRUE);

	if (m_strFilePath.IsEmpty())
	{
		AfxMessageBox(StrFromResID(IDS_SELECT_MDBORGDB));
		return;
	}
	CString strsuffix = m_strFilePath.Right(3);
	if(0!= strsuffix.CompareNoCase(_T("mdb"))&&0!= strsuffix.CompareNoCase(_T("gdb")))
	{
		AfxMessageBox(StrFromResID(IDS_SELECT_MDBORGDB));
		m_nType = -1;
		return;
	}
	if (0 == strsuffix.CompareNoCase(_T("mdb")))
	{
		m_nType = 0;
	}
	else if (0 == strsuffix.CompareNoCase(_T("gdb")))
	{
		m_nType = 1;
	}
	
	strPath = m_strFilePath;
	
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile = _T("");

	AfxGetApp()->WriteProfileString("IO32\\ImportMDB_Settings", "strKeyFields", m_strKeyFields);
	AfxGetApp()->WriteProfileString("IO32\\ImportMDB_Settings", "strLstFile", m_strLstFile);
	
	CDialog::OnOK();
}

BOOL CDlgImportMDB::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgImportMDB::OnSetfocusEditIdFilepath2() 
{
	UpdateData(TRUE);
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgImportMDB::OnKillfocusEditIdFilepath2() 
{
	UpdateData(TRUE);
	if( m_strLstFile.GetLength()<=0 )
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}	
	
}
