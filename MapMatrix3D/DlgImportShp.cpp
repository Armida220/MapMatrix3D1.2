// DlgImportShp.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgImportShp.h"
#include "SmartViewFunctions.h"
#include "UIFFileDialogEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImportShp dialog
static CString lstFile = _T("");
static CString lstFile2 = _T("");
static CString strPath = _T("");
static BOOL    bInit   = FALSE;

CDlgImportShp::CDlgImportShp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportShp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportShp)
	m_strPath = _T("");
	//}}AFX_DATA_INIT

	if( !bInit )
	{
		bInit = TRUE;
		lstFile = StrFromResID(IDS_NOLISTFILE);
		lstFile2 = StrFromResID(IDS_NOLISTFILE);
		strPath = _T("");
	}
	
	m_strPath = strPath;
	m_strLstFile = lstFile;
	m_strLstFile2 = lstFile2;

	if (!m_strPath.IsEmpty())
	{
		m_arrFileNames.Add(m_strPath);
	}
}


void CDlgImportShp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportShp)
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH2, m_strLstFile);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH3, m_strLstFile2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportShp, CDialog)
	//{{AFX_MSG_MAP(CDlgImportShp)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnButtonIdBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE2, OnBrowseLst)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH2, OnSetfocusEditLstFile)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH2, OnKillfocusEditLstFile)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE3, OnBrowseLst2)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH3, OnSetfocusEditLstFile2)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH3, OnKillfocusEditLstFile2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportShp message handlers

#define MAX_FILE_NUM 2000

void CDlgImportShp::OnButtonIdBrowse() 
{
	UpdateData(TRUE);

	OPENFILENAME ofn;
	// �洢��ȡ�Ķ���ļ���
	TCHAR *filename = new TCHAR[MAX_PATH*MAX_FILE_NUM];
	memset(filename,0,sizeof(TCHAR)*MAX_PATH*MAX_FILE_NUM);

	TCHAR OldDir[MAX_PATH];
	
	CString strTitle;
	strTitle.LoadString(IDS_SELECT_FILE);
	
	::GetCurrentDirectory(MAX_PATH, OldDir);
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetSafeHwnd();
	ofn.lpstrFilter = _T("SHP files (*.shp)\0*.shp\0All Files (*.*)\0*.*\0\0");
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filename; // ָ�����ص��ļ���
	ofn.nMaxFile = MAX_PATH*MAX_FILE_NUM;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = strTitle.GetBuffer(0);
	ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST
		| OFN_ALLOWMULTISELECT | OFN_EXPLORER; // ������ļ�ѡ��
	ofn.lpstrDefExt = _T("*.shp");
	
	if(!GetOpenFileName(&ofn))
	{
		delete[] filename;
		strTitle.ReleaseBuffer();
		return;
	}
	
	strTitle.ReleaseBuffer();
	
	CString path = ofn.lpstrFile; //������һ��\0Ϊֹǰ�ľ���·��: path\0filename��path
	TCHAR *fname = ofn.lpstrFile;
	
	m_arrFileNames.RemoveAll();
	
	DWORD attr = ::GetFileAttributes(path);

	m_strPath.Empty();
	
	//�����ļ�
	if( attr!=-1 && (attr&FILE_ATTRIBUTE_DIRECTORY)==0 )
	{
		m_arrFileNames.Add(path);
		m_strPath = path;
	}
	else
	{
		if( path.Right(1)!=_T("\\") )
			path += _T("\\");
		
		// ����path, �ҵ���һ���ļ�������ʼλ��
		while (*fname++);  
		// ��ʱfnameָ���һ���ļ���
		int i = 0;
		while(*fname)
		{
			CString shpFile = path+CString(fname);
			m_arrFileNames.Add(shpFile);

			m_strPath = m_strPath + "\"" + fname + "\"" + " ";
			
			// ����һ���ļ����� �ҵ���һ���ָ���\0��ѭ��ֹͣ��ͬʱƫ�Ƽ�һ���պ�ָ����һ���ļ���
			while (*fname++);
			i++;
		}
	}

	UpdateData(FALSE);

	delete[] filename;
}

void CDlgImportShp::OnOK() 
{
	UpdateData(TRUE);

	CDialog::OnOK();
	
	lstFile = m_strLstFile;
	lstFile2 = m_strLstFile2;
	strPath = m_strPath;
	
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile = _T("");
	if( m_strLstFile2.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile2 = _T("");
}



void CDlgImportShp::OnBrowseLst() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile = dlg.GetPathName();
	UpdateData(FALSE);		
}


void CDlgImportShp::OnSetfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgImportShp::OnKillfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.GetLength()<=0 )
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}	
}

void CDlgImportShp::OnBrowseLst2() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile2 = dlg.GetPathName();
	UpdateData(FALSE);		
}

void CDlgImportShp::OnSetfocusEditLstFile2() 
{
	UpdateData(TRUE);
	if( m_strLstFile2.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile2 = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgImportShp::OnKillfocusEditLstFile2() 
{
	UpdateData(TRUE);
	if( m_strLstFile2.GetLength()<=0 )
	{
		m_strLstFile2 = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}	
}