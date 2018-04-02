// DlgExportEps.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgExportEps.h"
#include "SmartViewFunctions.h"
#include "UIFFileDialogEx.h "
#include "IO.H "
#include "editbasedoc.h "
#include "DlgDataSource.h "
#include "SymbolLib.h "

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportEps dialog

static CString strPath = _T("");

CDlgExportEps::CDlgExportEps(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportEps::IDD, pParent)
{
	m_strFilePath = strPath;
	m_strCodeList = AfxGetApp()->GetProfileString("IO32\\ExportEPS_Settings", "strCodeList", _T(""));
	if (m_strCodeList.IsEmpty())
	{
		long  scale = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager()->GetSuitedScale(GetActiveDlgDoc()->GetDlgDataSource()->GetScale());
		char line[12];
		m_strCodeList = GetConfigPath() + _T("\\");
		m_strCodeList += _ltoa(scale, line, 10);
		m_strCodeList += _T("\\Epcode.lst");
	}
	m_strTemplateFile = AfxGetApp()->GetProfileString("IO32\\ExportEPS_Settings", "strTemplateFile", _T(""));
	if (m_strTemplateFile.IsEmpty())
	{
		m_strTemplateFile = StrFromResID(IDS_USEDEFAULTTEMP);
	}
}


void CDlgExportEps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportEps)
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strFilePath);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH2, m_strCodeList);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH3, m_strTemplateFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportEps, CDialog)
	//{{AFX_MSG_MAP(CDlgExportEps)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnButtonIdBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE2, OnButtonEdBrowse2)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH2, OnSetfocusEditEdFilepath2)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH2, OnKillfocusEditEdFilepath2)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE3, OnButtonBrowse3)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH3, OnSetfocusEditEdFilepath3)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH3, OnKillfocusEditEdFilepath3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportEps message handlers

void CDlgExportEps::OnButtonIdBrowse() 
{
	CString filter(StrFromResID(IDS_LOADEPS_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, _T(".edb"), m_strFilePath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strFilePath = dlg.GetPathName();
	UpdateData(FALSE);		
	
}

void CDlgExportEps::OnButtonEdBrowse2() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strCodeList = dlg.GetPathName();
	UpdateData(FALSE);	
}

void CDlgExportEps::OnOK() 
{
	UpdateData(TRUE);
	
	strPath = m_strFilePath;
	
	if( m_strCodeList.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strCodeList = _T("");
	if (m_strTemplateFile.Compare(StrFromResID(IDS_USEDEFAULTTEMP))==0 )
		m_strTemplateFile = _T("");

	AfxGetApp()->WriteProfileString("IO32\\ExportEPS_Settings", "strCodeList", m_strCodeList);
	AfxGetApp()->WriteProfileString("IO32\\ExportEPS_Settings", "strTemplateFile", m_strTemplateFile);

	CDialog::OnOK();
}

void CDlgExportEps::OnSetfocusEditEdFilepath2() 
{
	UpdateData(TRUE);
	if( m_strCodeList.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strCodeList = _T("");
		UpdateData(FALSE);
	}
}

void CDlgExportEps::OnKillfocusEditEdFilepath2() 
{
	UpdateData(TRUE);
	if( m_strCodeList.GetLength()<=0 )
	{
		m_strCodeList = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}	
}
void CDlgExportEps::OnButtonBrowse3() 
{
	CString filter(StrFromResID(ID_STRING_MDTFILITER ));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	if( dlg.DoModal()!=IDOK )return;
	UpdateData(TRUE);
	m_strTemplateFile= dlg.GetPathName();
	UpdateData(FALSE);	
}
void CDlgExportEps::OnSetfocusEditEdFilepath3() 
{
	UpdateData(TRUE);
	if( m_strTemplateFile.Compare(StrFromResID(IDS_USEDEFAULTTEMP))==0 )
	{
		m_strTemplateFile = _T("");
		UpdateData(FALSE);
	}
}
void CDlgExportEps::OnKillfocusEditEdFilepath3() 
{
	UpdateData(TRUE);
	if( m_strTemplateFile.GetLength()<=0 )
	{
		m_strTemplateFile = StrFromResID(IDS_USEDEFAULTTEMP);
		UpdateData(FALSE);
	}	
}
