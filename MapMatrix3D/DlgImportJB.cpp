// DlgImportJB.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgImportJB.h"
#include "SmartViewFunctions.h"
#include "UIFFileDialogEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImportJB dialog

static CString lstFile = _T("");
static CString strPath = _T("");
static BOOL    bInit   = FALSE;

extern BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] 窗口标题
						   LPTSTR lpszPath,		// [out] 返回的文件路径
						   LPCTSTR lpszInitDir,	// [in] 初始文件路径
						   HWND hWnd,				// [in] 父系窗口
						   BOOL bNetwork=FALSE,	// [in] 限制在网络路径范围内
						   UINT ulFlags=BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS );// [in] 设置风格





CDlgImportJB::CDlgImportJB(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportJB::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportJB)
	m_strFilePath = _T("");
	m_strLstFile = _T("");
	m_strMapName = _T("");
	//}}AFX_DATA_INIT

	if( !bInit )
	{
		bInit = TRUE;
		lstFile = StrFromResID(IDS_NOLISTFILE);
		strPath = _T("");
	}
	
	m_strFilePath = strPath;
	m_strLstFile = lstFile;
}


void CDlgImportJB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportJB)
	DDX_Text(pDX, IDC_EDIT_IC_FILEPATH, m_strFilePath);
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH2, m_strLstFile);
	DDX_Text(pDX, IDC_EDIT_IC_EXCODE, m_strMapName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportJB, CDialog)
	//{{AFX_MSG_MAP(CDlgImportJB)
	ON_BN_CLICKED(IDC_BUTTON_IC_BROWSE, OnButtonIcBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE2, OnButtonIdBrowse2)
	ON_EN_SETFOCUS(IDC_EDIT_ID_FILEPATH2, OnSetfocusEditIdFilepath2)
	ON_EN_KILLFOCUS(IDC_EDIT_ID_FILEPATH2, OnKillfocusEditIdFilepath2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportJB message handlers

void CDlgImportJB::OnButtonIcBrowse() 
{
	CString retPath;
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_FILE),retPath.GetBuffer(256),NULL,GetSafeHwnd()) )
		return;
	
	UpdateData(TRUE);
	m_strFilePath = retPath;
	UpdateData(FALSE);
}

void CDlgImportJB::OnButtonIdBrowse2() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile = dlg.GetPathName();
	UpdateData(FALSE);		
}

void CDlgImportJB::OnOK() 
{
	UpdateData(TRUE);
	
	lstFile = m_strLstFile;
	strPath = m_strFilePath;
	
	CDialog::OnOK();
	
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile = _T("");
}

BOOL CDlgImportJB::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgImportJB::OnSetfocusEditIdFilepath2() 
{
	UpdateData(TRUE);
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgImportJB::OnKillfocusEditIdFilepath2() 
{
	UpdateData(TRUE);
	if( m_strLstFile.GetLength()<=0 )
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}	
	
}
