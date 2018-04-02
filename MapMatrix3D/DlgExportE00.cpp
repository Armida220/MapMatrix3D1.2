// DLGEXPORTE00.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DLGEXPORTE00.h"
#include "UIFFileDialogEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportE00 dialog
static CString lstFile = _T("");
static CString lstFile2 = _T("");
static CString strPath = _T("");
static BOOL    bInit   = FALSE;

CDlgExportE00::CDlgExportE00(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportE00::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportE00)
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

void CDlgExportE00::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportE00)
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH2, m_strLstFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportE00, CDialog)
	//{{AFX_MSG_MAP(CDlgExportE00)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnButtonIdBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE2, OnBrowseLst)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH2, OnSetfocusEditLstFile)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH2, OnKillfocusEditLstFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


extern BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] 窗口标题
						   LPTSTR lpszPath,		// [out] 返回的文件路径
						   LPCTSTR lpszInitDir,	// [in] 初始文件路径
						   HWND hWnd,				// [in] 父系窗口
						   BOOL bNetwork=FALSE,	// [in] 限制在网络路径范围内
						   UINT ulFlags=BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS );// [in] 设置风格

void CDlgExportE00::OnButtonIdBrowse() 
{
	char retPath[500];
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_FILE),retPath,NULL,m_hWnd,FALSE,BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS|0x40) )
		return;

	UpdateData(TRUE);
	m_strPath = retPath;
	UpdateData(FALSE);
}


void CDlgExportE00::OnOK() 
{
	UpdateData(TRUE);

	lstFile = m_strLstFile;
	strPath = m_strPath;

	CDialog::OnOK();
	
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile = _T("");
}



void CDlgExportE00::OnBrowseLst() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile = dlg.GetPathName();
	UpdateData(FALSE);		
}



void CDlgExportE00::OnSetfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgExportE00::OnKillfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.GetLength()<=0 )
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}	
}

