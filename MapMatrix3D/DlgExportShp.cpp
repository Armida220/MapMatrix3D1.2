// DlgExportShp.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgExportShp.h"
#include "SmartViewFunctions.h"
#include "UIFFileDialogEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportShp dialog
static CString lstFile = _T("");
static CString lstFile2 = _T("");
static CString strPath = _T("");
static BOOL    bInit   = FALSE;
static BOOL    bBaseAtt = TRUE;
static BOOL    bExtAtt = TRUE;


CDlgExportShp::CDlgExportShp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportShp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportShp)
	m_strPath = _T("");
	m_bBaseAtt = TRUE;
	m_bExtAtt = TRUE;
	//}}AFX_DATA_INIT

	if( !bInit )
	{
		bInit = TRUE;
		lstFile = StrFromResID(IDS_NOLISTFILE);
		lstFile2 = StrFromResID(IDS_NOLISTFILE);
		strPath = _T("");
		bBaseAtt = TRUE;
		bExtAtt = TRUE;
	}

	m_strPath = strPath;
	m_strLstFile = lstFile;
	m_strLstFile2 = lstFile2;
	m_bBaseAtt = bBaseAtt;
	m_bExtAtt = bExtAtt;
}


void CDlgExportShp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportShp)
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH2, m_strLstFile);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH3, m_strLstFile2);
	DDX_Check(pDX, IDC_CHECK_BASEATT, m_bBaseAtt);
	DDX_Check(pDX, IDC_CHECK_EXTATT, m_bExtAtt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportShp, CDialog)
	//{{AFX_MSG_MAP(CDlgExportShp)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnButtonIdBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE2, OnBrowseLst)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE3, OnBrowseLst2)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH2, OnSetfocusEditLstFile)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH2, OnKillfocusEditLstFile)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH3, OnSetfocusEditLstFile2)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH3, OnKillfocusEditLstFile2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportShp message handlers

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)   
{   
    if  (uMsg == BFFM_INITIALIZED )
    {   
        ::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,lpData);   
    }   
    return 0;  
}

BOOL BrowseFolder(LPCTSTR szTitle, CString& strPath, LPCTSTR szInitDir, HWND hOwner )
{
	LPMALLOC pMalloc;
	bool     bResult = false;
	if(CString(szInitDir).IsEmpty()==TRUE)
	{
	}
	// Get's the Shell's default allocator
	if(::SHGetMalloc(&pMalloc) == NOERROR)
    {
		BROWSEINFO bi;
		char szBuffer[MAX_PATH];
		LPITEMIDLIST pidl;
		
		bi.hwndOwner = hOwner;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = szBuffer;
		bi.lpszTitle = szTitle;
		bi.lpfn = BrowseCallbackProc;
		bi.lParam = NULL;
	//	int BIF_NEWDIALOGSTYLE=0x40;
		bi.ulFlags =  BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS|/*BIF_NEWDIALOGSTYLE*/0x40;
		
		if(!CString(szInitDir).IsEmpty())
		{ 
			bi.lParam = LPARAM(szInitDir);
		}		
		
		// This next call issues the dialog box
		if((pidl = ::SHBrowseForFolder(&bi))!=NULL)   //error
		{
			if(::SHGetPathFromIDList(pidl, szBuffer))
			{
				//At this point pszBuffer contains the selected path
				strPath= szBuffer;
				bResult = TRUE;
			}
			// Free the PIDL allocated by SHBrowseForFolder
			pMalloc->Free(pidl);
		}
    }
	
	// Release the shell's allocator
	pMalloc->Release();
	
	return bResult;
}


void CDlgExportShp::OnButtonIdBrowse() 
{
	CString retPath;
	if( !BrowseFolder(StrFromResID(IDS_SELECT_FILE),retPath,NULL,m_hWnd) )
		return;

	UpdateData(TRUE);
	m_strPath = retPath;
	UpdateData(FALSE);
}

void CDlgExportShp::OnOK() 
{
	UpdateData(TRUE);

	lstFile = m_strLstFile;
	lstFile2 = m_strLstFile2;
	strPath = m_strPath;
	bBaseAtt = m_bBaseAtt;
	bExtAtt = m_bExtAtt;

	CDialog::OnOK();
	
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile = _T("");

	if( m_strLstFile2.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile2 = _T("");
}



void CDlgExportShp::OnBrowseLst() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile = dlg.GetPathName();
	UpdateData(FALSE);		
}

void CDlgExportShp::OnBrowseLst2() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile2 = dlg.GetPathName();
	UpdateData(FALSE);		
}

void CDlgExportShp::OnSetfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgExportShp::OnKillfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.GetLength()<=0 )
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}	
}
void CDlgExportShp::OnSetfocusEditLstFile2() 
{
	UpdateData(TRUE);
	if( m_strLstFile2.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile2 = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgExportShp::OnKillfocusEditLstFile2() 
{
	UpdateData(TRUE);
	if( m_strLstFile2.GetLength()<=0 )
	{
		m_strLstFile2 = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}	
}