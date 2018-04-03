// DlgImportEps.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgImportEps.h"
#include "SmartViewFunctions.h "
#include "UIFFileDialogEx.h"
#include "SymbolLib.h "
#include "editbasedoc.h "
#include "DlgDataSource.h "

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImportEps dialog

static CString strPath = _T("");

CDlgImportEps::CDlgImportEps(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportEps::IDD, pParent)
{
	m_strPath = strPath;
	m_strLstFile = AfxGetApp()->GetProfileString("IO32\\ImportEPS_Settings", "strLstFile", _T(""));
	if (m_strLstFile.IsEmpty())
	{
		CString path = GetConfigPath();		
		long  scale = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager()->GetSuitedScale(GetActiveDlgDoc()->GetDlgDataSource()->GetScale());
		char line[12];
		m_strLstFile = GetConfigPath() + _T("\\");
		m_strLstFile += _ltoa(scale, line, 10);
		m_strLstFile += _T("\\Epcode.lst");
	}
	m_strRefTmp = AfxGetApp()->GetProfileString("IO32\\ImportEPS_Settings", "strRefTmp", _T(""));
	if (m_strRefTmp.IsEmpty())
	{
		m_strRefTmp = StrFromResID(IDS_USEDEFAULTTEMP);
	}
	m_strMapName = _T("");
}


void CDlgImportEps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportEps)
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH2, m_strLstFile);
	DDX_Text(pDX, IDC_EDIT_ED_MAPNAME, m_strMapName);
	DDX_Text(pDX, IDC_EDIT_REFTMP_PATH, m_strRefTmp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportEps, CDialog)
	//{{AFX_MSG_MAP(CDlgImportEps)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnButtonIdBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE2, OnButtonEdBrowse2)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH2, OnKillfocusEditEdFilepath2)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH2, OnSetfocusEditEdFilepath2)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE4, OnButtonEdBrowse4)
	ON_EN_KILLFOCUS(IDC_EDIT_REFTMP_PATH, OnKillfocusEditReftmpPath)
	ON_EN_SETFOCUS(IDC_EDIT_REFTMP_PATH, OnSetfocusEditReftmpPath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportEps message handlers


void CDlgImportEps::OnButtonIdBrowse() 
{
	CString filter(StrFromResID(IDS_LOADEPS_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strPath = dlg.GetPathName();
	
	//Í¼·ùÃû
	m_strMapName = dlg.GetFileTitle();
	
	UpdateData(FALSE);	
}

void CDlgImportEps::OnButtonEdBrowse2() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile = dlg.GetPathName();
	UpdateData(FALSE);	
}

void CDlgImportEps::OnKillfocusEditEdFilepath2() 
{
	UpdateData(TRUE);
	if( m_strLstFile.GetLength()<=0 )
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}	
}

void CDlgImportEps::OnSetfocusEditEdFilepath2() 
{
	UpdateData(TRUE);
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgImportEps::OnOK() 
{
	UpdateData(TRUE);
	
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile = _T("");
	if (m_strRefTmp.Compare(StrFromResID(IDS_USEDEFAULTTEMP))==0 )
		m_strRefTmp = _T("");

	AfxGetApp()->WriteProfileString("IO32\\ImportEPS_Settings", "strLstFile", m_strLstFile);
	AfxGetApp()->WriteProfileString("IO32\\ImportEPS_Settings", "strRefTmp", m_strRefTmp);

	CDialog::OnOK();
}

void CDlgImportEps::OnButtonEdBrowse4() 
{
	CString filter(StrFromResID(ID_STRING_MDTFILITER ));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strRefTmp = dlg.GetPathName();
	UpdateData(FALSE);	
}

void CDlgImportEps::OnKillfocusEditReftmpPath() 
{
	UpdateData(TRUE);
	if( m_strRefTmp.GetLength()<=0 )
	{
		m_strRefTmp = StrFromResID(IDS_USEDEFAULTTEMP);
		UpdateData(FALSE);
	}	
	
}

void CDlgImportEps::OnSetfocusEditReftmpPath() 
{
	UpdateData(TRUE);
	if( m_strRefTmp.Compare(StrFromResID(IDS_USEDEFAULTTEMP))==0 )
	{
		m_strRefTmp = _T("");
		UpdateData(FALSE);
	}
}
