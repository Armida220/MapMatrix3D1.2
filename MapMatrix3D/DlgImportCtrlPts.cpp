// DlgImportCtrlPts.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgImportCtrlPts.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IMPORTCTRL_SETTINGS  "ImportCtrl_Settings"

/////////////////////////////////////////////////////////////////////////////
// CDlgImportCtrlPts dialog


CDlgImportCtrlPts::CDlgImportCtrlPts(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportCtrlPts::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportCtrlPts)
	m_strFilePath = _T("");
	m_strLayer = _T("");
	m_bInMapBound = TRUE;
	m_strMapName = _T("");
	m_strField = _T("POINTNUM");
	m_bCreateLine = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgImportCtrlPts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportCtrlPts)
	DDX_Text(pDX, IDC_EDIT_IC_FILEPATH, m_strFilePath);
	DDX_CBString(pDX, IDC_COMBO_IC_LAYER, m_strLayer);
	DDX_Check(pDX, IDC_CHECK_INMAPBOUND, m_bInMapBound);
	DDX_Text(pDX, IDC_EDIT_MAPNAME, m_strMapName);
	DDX_Check(pDX, IDC_CHECK_CREATELINE, m_bCreateLine);
	DDX_Text(pDX, IDC_EDIT1, m_strField);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportCtrlPts, CDialog)
	//{{AFX_MSG_MAP(CDlgImportCtrlPts)
	ON_BN_CLICKED(IDC_BUTTON_IC_BROWSE, OnFileBrowse)
	ON_BN_CLICKED(IDC_CHECK_INMAPBOUND, OnCheckInmapbound)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportCtrlPts message handlers

void CDlgImportCtrlPts::OnFileBrowse() 
{
	CString filter(StrFromResID(IDS_ALLFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;

	m_strFilePath = dlg.GetPathName();


	//Í¼·ùÃû
	m_strMapName = dlg.GetFileTitle();
	// Í¼·ùÃû
// 	int spos, epos;
// 	spos = m_strFilePath.ReverseFind('\\');
// 	epos = m_strFilePath.ReverseFind('.');
// 	if (spos != -1 && epos != -1)
// 	{
// 		m_strMapName = m_strFilePath.Mid(spos+1,epos-spos-1);
// 	}

	UpdateData(FALSE);
}

BOOL CDlgImportCtrlPts::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_strLayer = AfxGetApp()->GetProfileString(IMPORTCTRL_SETTINGS,"DefLayer");
	m_strField  = AfxGetApp()->GetProfileString(IMPORTCTRL_SETTINGS,"DefField");
	// TODO: Add extra initialization here
	CComboBox *pCtrl = (CComboBox*)GetDlgItem(IDC_COMBO_IC_LAYER);
	if( pCtrl )
	{
		int num = m_arrLayers.GetSize();
		for( int i=0; i<num; i++)
		{
			pCtrl->AddString(m_arrLayers[i]);
		}

		if( m_strLayer.GetLength()<=0 || pCtrl->SelectString(-1,m_strLayer)==CB_ERR )
		{
			if( num>0 )pCtrl->SetCurSel(0);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgImportCtrlPts::AddLayerName(const char *name)
{
	if( name )m_arrLayers.Add(CString(name));
}

void CDlgImportCtrlPts::OnOK() 
{
	UpdateData(TRUE);
	AfxGetApp()->WriteProfileString(IMPORTCTRL_SETTINGS,"DefLayer",m_strLayer);
	AfxGetApp()->WriteProfileString(IMPORTCTRL_SETTINGS,"DefField",m_strField);
	CDialog::OnOK();
}

void CDlgImportCtrlPts::OnCheckInmapbound() 
{
	// TODO: Add your control notification handler code here
// 	CButton *pWnd = (CButton*)GetDlgItem(IDC_CHECK_INMAPBOUND);
// 	if (pWnd)
// 	{
// 		m_bInMapBound = pWnd->GetCheck();
// 	}
	m_bInMapBound = !m_bInMapBound;
}
