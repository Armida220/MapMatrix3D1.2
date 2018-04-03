// DlgBuildDEM.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgBuildDEM.h"
#include "cadlib.h"
#include "dsm.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBuildDEM dialog


CString gSaveDemPath = _T("");

CDlgBuildDEM::CDlgBuildDEM(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBuildDEM::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBuildDEM)
	m_strDEMPath = gSaveDemPath;
	m_fDX = 5.0f;
	m_nFormat = 2;
	//}}AFX_DATA_INIT
}


void CDlgBuildDEM::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBuildDEM)
	DDX_Text(pDX, IDC_EDIT_DEMPATH, m_strDEMPath);
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_fDX);
	DDX_CBIndex(pDX, IDC_COMBO_DEMFORMAT, m_nFormat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBuildDEM, CDialog)
	//{{AFX_MSG_MAP(CDlgBuildDEM)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_CBN_SELCHANGE(IDC_COMBO_DEMFORMAT, OnSelchangeComboDemformat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBuildDEM message handlers

void CDlgBuildDEM::OnButtonBrowse() 
{
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("All Files(*.*)|*.*||"));
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strDEMPath = dlg.GetPathName();
	UpdateDEMExt();
	UpdateData(FALSE);	

	gSaveDemPath = m_strDEMPath;
}
/*
VZDEM,
GRID,
NSDTF,
BIL,
XX,
DXF,
TXT,
DTM,
IGSXYZ,
VZCTRL,
TIN,
DTED2,
TIFF,
IMG,
CNSDTF,
VZDEM2,// VirtuoZo Integer
DTM2,//ImageInfo DTM
CNT,//DXF格式的等高线，这里负责标识，外部完成输入输出
GEOID99, 
*/
void CDlgBuildDEM::OnOK() 
{

	UpdateData(TRUE);
	if(m_strDEMPath.IsEmpty() )
	{
		AfxMessageBox(IDS_STRING_PATHINVALID);
		return;
	}
	int nFmt = UpdateDEMExt();
	CDialog::OnOK();

	m_nFormat = nFmt;
}

int CDlgBuildDEM::UpdateDEMExt()
{
	int fmts[] = {
		CDSM::DSM_VZ_FLOAT,
		CDSM::DSM_VZ_INT,
		CDSM::DSM_NSDTF,
		CDSM::DSM_CNSDTF,
		CDSM::DSM_TXT_POINT,
		CDSM::DSM_GRID,
		CDSM::DSM_IMAGEINFO_DTM
	};

	int nFmt = m_nFormat;
	
	if( nFmt>=0 && nFmt<sizeof(fmts)/sizeof(fmts[0]) )
		nFmt = fmts[nFmt];
	else
		nFmt = fmts[0];
	
	if( m_strDEMPath.GetLength()>0 )
	{
		CString strLeft = m_strDEMPath;
		int nDot = m_strDEMPath.ReverseFind('.');
		if( nDot>0 )
		{
			CString strExt = m_strDEMPath.Mid(nDot);
			if( strExt.CompareNoCase(".dem")==0 || strExt.CompareNoCase(".dtm")==0 || 
				strExt.CompareNoCase(".txt")==0 || strExt.CompareNoCase(".grd")==0 )
			{
				strLeft = m_strDEMPath.Left(nDot);
			}
		}
		
		switch( nFmt )
		{
		case CDSM::DSM_VZ_FLOAT:
			m_strDEMPath = strLeft + ".dem";
			break;
		case CDSM::DSM_VZ_INT:
			m_strDEMPath = strLeft + ".dem";
			break;
		case CDSM::DSM_NSDTF:
		case CDSM::DSM_CNSDTF:
			m_strDEMPath = strLeft + ".dem";
			break;
		case CDSM::DSM_GRID:
			m_strDEMPath = strLeft + ".grd";
			break;
		case CDSM::DSM_TXT_POINT:
			m_strDEMPath = strLeft + ".txt";
			break;
		case CDSM::DSM_IMAGEINFO_DTM:
			m_strDEMPath = strLeft + ".dtm";
			break;
		}
	}
	return nFmt;
}

void CDlgBuildDEM::OnSelchangeComboDemformat() 
{
	UpdateData(TRUE);
	UpdateDEMExt();
	UpdateData(FALSE);
}

CScrollBar* CDlgBuildDEM::GetScrollBarCtrl(int nBar) const
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::GetScrollBarCtrl(nBar);
}
