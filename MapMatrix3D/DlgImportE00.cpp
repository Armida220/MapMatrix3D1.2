// DlgImportShp.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgImportE00.h"
#include "SmartViewFunctions.h"
#include "UIFFileDialogEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImportE00 dialog
static CString lstFile = _T("");
static CString lstFile2 = _T("");
static CString strPath = _T("");
static BOOL    bInit   = FALSE;

CDlgImportE00::CDlgImportE00(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportE00::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportE00)
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

	if (!m_strPath.IsEmpty())
	{
		m_arrFileNames.Add(m_strPath);
	}
}


void CDlgImportE00::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportE00)
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH2, m_strLstFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportE00, CDialog)
	//{{AFX_MSG_MAP(CDlgImportE00)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnButtonIdBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE2, OnBrowseLst)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH2, OnSetfocusEditLstFile)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH2, OnKillfocusEditLstFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportE00 message handlers

#define MAX_FILE_NUM 2000

void CDlgImportE00::OnButtonIdBrowse() 
{
	UpdateData(TRUE);

	OPENFILENAME ofn;
	// 存储获取的多个文件名
	TCHAR *filename = new TCHAR[MAX_PATH*MAX_FILE_NUM];
	memset(filename,0,sizeof(TCHAR)*MAX_PATH*MAX_FILE_NUM);

	TCHAR OldDir[MAX_PATH];
	
	CString strTitle;
	strTitle.LoadString(IDS_SELECT_FILE);
	
	::GetCurrentDirectory(MAX_PATH, OldDir);
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetSafeHwnd();
	ofn.lpstrFilter = _T("E00 files (*.e00)\0*.e00\0All Files (*.*)\0*.*\0\0");
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filename; // 指定返回的文件名
	ofn.nMaxFile = MAX_PATH*MAX_FILE_NUM;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = strTitle.GetBuffer(0);
	ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST
		| OFN_ALLOWMULTISELECT | OFN_EXPLORER; // 允许多文件选择
	ofn.lpstrDefExt = _T("*.shp");
	
	if(!GetOpenFileName(&ofn))
	{
		delete[] filename;
		strTitle.ReleaseBuffer();
		return;
	}
	
	strTitle.ReleaseBuffer();
	
	CString path = ofn.lpstrFile; //遇到第一个\0为止前的就是路径: path\0filename的path
	TCHAR *fname = ofn.lpstrFile;
	
	m_arrFileNames.RemoveAll();
	
	DWORD attr = ::GetFileAttributes(path);

	m_strPath.Empty();
	
	//单个文件
	if( attr!=-1 && (attr&FILE_ATTRIBUTE_DIRECTORY)==0 )
	{
		m_arrFileNames.Add(path);
		m_strPath = path;
	}
	else
	{
		if( path.Right(1)!=_T("\\") )
			path += _T("\\");
		
		// 跳过path, 找到第一个文件名的起始位置
		while (*fname++);  
		// 此时fname指向第一个文件名
		int i = 0;
		while(*fname)
		{
			CString shpFile = path+CString(fname);
			m_arrFileNames.Add(shpFile);

			m_strPath = m_strPath + "\"" + fname + "\"" + " ";
			
			// 找下一个文件名， 找到第一个分隔符\0后循环停止，同时偏移加一，刚好指向下一个文件名
			while (*fname++);
			i++;
		}
	}

	UpdateData(FALSE);
	delete[] filename;
}

void CDlgImportE00::OnOK() 
{
	UpdateData(TRUE);

	CDialog::OnOK();
	
	lstFile = m_strLstFile;
	strPath = m_strPath;
	
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile = _T("");
}



void CDlgImportE00::OnBrowseLst() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile = dlg.GetPathName();
	UpdateData(FALSE);		
}


void CDlgImportE00::OnSetfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgImportE00::OnKillfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.GetLength()<=0 )
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}	
}
