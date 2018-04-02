// DlgImportDGN.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgImportDGN.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImportDGN dialog

static CString lstFile = _T("");
static CString strPath = _T("");
static BOOL    bInit   = FALSE;

CDlgImportDGN::CDlgImportDGN(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportDGN::IDD, pParent)
{
	if( !bInit )
	{
		bInit = TRUE;
		lstFile = StrFromResID(IDS_NOLISTFILE);
		strPath = _T("");
	}

	//{{AFX_DATA_INIT(CDlgImportDGN)
		// NOTE: the ClassWizard will add member initialization here
	m_strPath = strPath;
	m_strLstFile = lstFile;
	m_strMapName = _T("");
	//}}AFX_DATA_INIT
	
	if (strPath.CompareNoCase(StrFromResID(IDS_NOLISTFILE)) != 0)
	{
		int spos, epos;
		spos = strPath.ReverseFind('\\');
		epos = strPath.ReverseFind('.');
		if (spos != -1 && epos != -1)
		{
			m_strMapName = strPath.Mid(spos+1,epos-spos-1);
		}
		
	}
}


void CDlgImportDGN::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportDGN)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH2, m_strLstFile);
	DDX_Text(pDX, IDC_EDIT_MAPNAME, m_strMapName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportDGN, CDialog)
	//{{AFX_MSG_MAP(CDlgImportDGN)
		// NOTE: the ClassWizard will add message map macros here
		ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnButtonIdBrowse)
		ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE2, OnButtonEdBrowse2)
		ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH2, OnKillfocusEditEdFilepath2)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH2, OnSetfocusEditEdFilepath2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportDGN message handlers
void CDlgImportDGN::OnButtonIdBrowse() 
{
	CString filter(StrFromResID(IDS_LOADDGN_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strPath = dlg.GetPathName();
	
	//Í¼·ùÃû
	m_strMapName = dlg.GetFileTitle();
	
	UpdateData(FALSE);	
}
#include "UIFFileDialogEx.h"
void CDlgImportDGN::OnButtonEdBrowse2() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile = dlg.GetPathName();
	UpdateData(FALSE);	
}

void CDlgImportDGN::OnKillfocusEditEdFilepath2() 
{
	UpdateData(TRUE);
	if( m_strLstFile.GetLength()<=0 )
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}	
}

void CDlgImportDGN::OnSetfocusEditEdFilepath2() 
{
	UpdateData(TRUE);
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgImportDGN::OnOK() 
{
	UpdateData(TRUE);
	
	CDialog::OnOK();
	
	strPath = m_strPath;
	lstFile = m_strLstFile;
	
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile = _T("");
}
