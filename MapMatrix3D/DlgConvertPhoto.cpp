// DlgConvertPhoto.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgConvertPhoto.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConvertPhoto dialog


CDlgConvertPhoto::CDlgConvertPhoto(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConvertPhoto::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConvertPhoto)
	m_strPrjFile = _T("");
	//}}AFX_DATA_INIT

	m_bIsFilePath = FALSE;
}


void CDlgConvertPhoto::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConvertPhoto)
	DDX_Control(pDX, IDC_COMBO_IMG, m_wndComboImg);
	DDX_Control(pDX, IDC_COMBO_DEM, m_wndComboDEM);
	DDX_Text(pDX, IDC_EDIT_PROJECTFILE1, m_strPrjFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConvertPhoto, CDialog)
	//{{AFX_MSG_MAP(CDlgConvertPhoto)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE1, OnBrowseProject)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE2, OnBrowseDEM)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConvertPhoto message handlers

void CDlgConvertPhoto::OnBrowseProject() 
{
	CString filter(StrFromResID(IDS_LOADXML_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	if( !m_prj.LoadProject(dlg.GetPathName()) )
		return;

	m_strPrjFile = dlg.GetPathName();
	
	m_wndComboDEM.ResetContent();
	CoreObject core = m_prj.GetCoreObject();
	for( int i=0; i<core.tmp.dems.iDemNum; i++)
	{
		m_wndComboDEM.AddString(core.tmp.dems.dem[i].strDemID);
	}

	if( m_wndComboDEM.GetCount()>0 )m_wndComboDEM.SetCurSel(0);

	m_wndComboImg.ResetContent();
	for( i=0; i<core.iStripNum; i++)
	{
		for( int j=0; j<core.strip[i].iImageNum; j++)
		{
			m_wndComboImg.AddString(core.strip[i].image[j].strImageID);
		}
	}

	if( m_wndComboImg.GetCount()>0 )m_wndComboImg.SetCurSel(0);
	
	UpdateData(FALSE);
}



void CDlgConvertPhoto::OnBrowseDEM() 
{
	CString filter(StrFromResID(IDS_LOADDEM_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;

	m_wndComboDEM.SetWindowText(dlg.GetPathName());	
}


void CDlgConvertPhoto::OnOK() 
{
	UpdateData(TRUE);
	int nsel = m_wndComboDEM.GetCurSel();
	if( nsel>=0 )
	{
		m_bIsFilePath = FALSE;
		m_wndComboDEM.GetLBText(nsel,m_strDEM);
	}
	else
	{
		m_bIsFilePath = TRUE;
		m_wndComboDEM.GetWindowText(m_strDEM);
	}

	nsel = m_wndComboImg.GetCurSel();
	if( nsel>=0 )m_wndComboImg.GetLBText(nsel,m_strImg);
	else m_strImg = _T("");
	
	CDialog::OnOK();
}

BOOL CDlgConvertPhoto::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
