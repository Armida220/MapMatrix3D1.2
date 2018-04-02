// DlgImportDxf.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgImportDxf.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IMPORTDXF_SETTINGS  "IO32\\ImportDXF_Settings"

/////////////////////////////////////////////////////////////////////////////
// CDlgImportDxf dialog
static CString dxfFile = _T("");
static CStringArray arrFileNames;


CDlgImportDxf::CDlgImportDxf(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportDxf::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportDxf)
	m_strDxfFile = dxfFile;
	m_strLstFile = AfxGetApp()->GetProfileString(IMPORTDXF_SETTINGS, "strLstFile", _T(""));
	m_bUseIndex = AfxGetApp()->GetProfileInt(IMPORTDXF_SETTINGS, "bUseIndex", FALSE);
	m_bBlockAsPoint = AfxGetApp()->GetProfileInt(IMPORTDXF_SETTINGS, "bBlockAsPoint", FALSE);
	m_strMapName = _T("*");
	m_bUnImport = AfxGetApp()->GetProfileInt(IMPORTDXF_SETTINGS, "bUnImport", FALSE);
	m_bLidarPoints = AfxGetApp()->GetProfileInt(IMPORTDXF_SETTINGS, "bLidarPoints", FALSE);
	m_bImportAnno = AfxGetApp()->GetProfileInt(IMPORTDXF_SETTINGS, "bImportAnno", FALSE);
	m_bSaveLinetype = AfxGetApp()->GetProfileInt(IMPORTDXF_SETTINGS, "bSaveLinetype", FALSE);
	m_bReadCassCode = AfxGetApp()->GetProfileInt(IMPORTDXF_SETTINGS, "bReadCassCode", TRUE);
	//}}AFX_DATA_INIT
	if (m_strLstFile.IsEmpty())
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
	}
	m_arrFileNames.Copy(arrFileNames);
}


void CDlgImportDxf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportDxf)
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strDxfFile);
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH2, m_strLstFile);
	DDX_Check(pDX, IDC_CHECK_USEINDEX, m_bUseIndex);
	DDX_Check(pDX, IDC_CHECK_BLOCKASPOINT, m_bBlockAsPoint);
	DDX_Text(pDX, IDC_EDIT_MAPNAME, m_strMapName);
	DDX_Check(pDX, IDC_CHECK_UNIMPORT, m_bUnImport);
	DDX_Check(pDX, IDC_CHECK_LIDARPOINTS, m_bLidarPoints);
	DDX_Check(pDX, IDC_CHECK_IMPORTANNO, m_bImportAnno);
	DDX_Check(pDX, IDC_CHECK_SAVELINETYPE, m_bSaveLinetype);
	DDX_Check(pDX, IDC_READ_CASS_CODE, m_bReadCassCode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportDxf, CDialog)
	//{{AFX_MSG_MAP(CDlgImportDxf)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnBrowseDxf)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE2, OnBrowseLst)
	ON_EN_KILLFOCUS(IDC_EDIT_ID_FILEPATH2, OnKillfocusEditLstFile)
	ON_EN_SETFOCUS(IDC_EDIT_ID_FILEPATH2, OnSetfocusEditLstFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL DlgSelectMultiFiles(HWND hOwner, LPCTSTR filter, LPCTSTR defExt, CStringArray& fileName, CString *all_name)
{
	OPENFILENAME ofn;

	// 存储获取的多个文件名
	TCHAR filename[MAX_PATH*1024]={0};
	TCHAR OldDir[MAX_PATH];

	CString strTitle;
	strTitle.LoadString(IDS_SELECT_FILE);

	::GetCurrentDirectory(MAX_PATH, OldDir);
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hOwner;
	ofn.lpstrFilter = filter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filename; // 指定返回的文件名
	ofn.nMaxFile = sizeof(filename);
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = strTitle.GetBuffer(0);
	ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST
	| OFN_ALLOWMULTISELECT | OFN_EXPLORER; // 允许多文件选择
	ofn.lpstrDefExt = defExt;

	if(!GetOpenFileName(&ofn))
	{
		strTitle.ReleaseBuffer();
		return FALSE;
	}

	strTitle.ReleaseBuffer();

	CString path = ofn.lpstrFile; //遇到第一个\0为止前的就是路径: path\0filename的path
	TCHAR *fname = ofn.lpstrFile;

	fileName.RemoveAll();

	DWORD attr = ::GetFileAttributes(path);

	CString allFilePath;

	//单个文件
	if( attr!=-1 && (attr&FILE_ATTRIBUTE_DIRECTORY)==0 )
	{
		fileName.Add(path);
		allFilePath = path;
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
			fileName.Add(shpFile);
			
			allFilePath = allFilePath + "\"" + fname + "\"" + " ";
			
			// 找下一个文件名， 找到第一个分隔符\0后循环停止，同时偏移加一，刚好指向下一个文件名
			while (*fname++);
			i++;
		}
	}

	if( all_name )
	{
		*all_name = allFilePath;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgImportDxf message handlers

void CDlgImportDxf::OnBrowseDxf() 
{
	CString strFilter = StrFromResID(IDS_LOADDXF_FILTER);
	strFilter.Replace('|','\0');

	if( !DlgSelectMultiFiles(GetSafeHwnd(),
		strFilter,
		NULL,m_arrFileNames,&m_strDxfFile) )
		return;

	UpdateData(FALSE);	
}

void CDlgImportDxf::OnBrowseLst() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile = dlg.GetPathName();
	UpdateData(FALSE);	
}

void CDlgImportDxf::OnOK() 
{	
	CDialog::OnOK();
	
	dxfFile = m_strDxfFile;
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile = _T("");

	AfxGetApp()->WriteProfileString(IMPORTDXF_SETTINGS, "strLstFile", m_strLstFile);
	AfxGetApp()->WriteProfileInt(IMPORTDXF_SETTINGS, "bUseIndex", m_bUseIndex);
	AfxGetApp()->WriteProfileInt(IMPORTDXF_SETTINGS, "bBlockAsPoint", m_bBlockAsPoint);
	AfxGetApp()->WriteProfileInt(IMPORTDXF_SETTINGS, "bUnImport", m_bUnImport);
	AfxGetApp()->WriteProfileInt(IMPORTDXF_SETTINGS, "bLidarPoints", m_bLidarPoints);
	AfxGetApp()->WriteProfileInt(IMPORTDXF_SETTINGS, "bImportAnno", m_bImportAnno);
	AfxGetApp()->WriteProfileInt(IMPORTDXF_SETTINGS, "bSaveLinetype", m_bSaveLinetype);
	AfxGetApp()->WriteProfileInt(IMPORTDXF_SETTINGS, "bReadCassCode", m_bReadCassCode);

	if( m_arrFileNames.GetSize()==1 )
	{
		m_arrFileNames.RemoveAll();
		m_arrFileNames.Add(m_strDxfFile);
	}

	arrFileNames.Copy(m_arrFileNames);
}

void CDlgImportDxf::OnKillfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.GetLength()<=0 )
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}		
}

void CDlgImportDxf::OnSetfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile = _T("");
		UpdateData(FALSE);
	}
}
