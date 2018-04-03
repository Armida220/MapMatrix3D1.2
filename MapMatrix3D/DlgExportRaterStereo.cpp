// DlgExportRaterStereo.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgExportRaterStereo.h"
#include "UIFFileDialogEx.h "
#include "SmartViewFunctions.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportRaterStereo dialog


CDlgExportRaterStereo::CDlgExportRaterStereo(CWnd* pParent /*=NULL*/)
: CDialog(CDlgExportRaterStereo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportRaterStereo)
	m_strFilePath = _T("");
	m_nImage = 0;
	m_fPGScale = 1.0;
	m_fPixelSize = 1.0;
	m_nScaleType = 0;
	//}}AFX_DATA_INIT
}


void CDlgExportRaterStereo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportRaterStereo)
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strFilePath);
	DDX_Radio(pDX, IDC_RADIO_LEFTIMAGE, m_nImage);
	DDX_Text(pDX, IDC_PGSCALE_EDIT, m_fPGScale);
	DDX_Text(pDX, IDC_PIXELSIZE_EDIT, m_fPixelSize);
	DDX_Radio(pDX, IDC_RADIO_CURRENT_SCALE, m_nScaleType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportRaterStereo, CDialog)
//{{AFX_MSG_MAP(CDlgExportRaterStereo)
ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_RADIO_MAP_SCALE, OnRadioMapScale)
	ON_BN_CLICKED(IDC_RADIO_CURRENT_SCALE, OnRadioCurrentScale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportRaterStereo message handlers

void CDlgExportRaterStereo::OnButtonBrowse() 
{
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, ".tif", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("TIFF File(*.tif)|*.tif|All Files(*.*)|*.*||"));
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strFilePath = dlg.GetPathName();
	UpdateData(FALSE);	
}

void CDlgExportRaterStereo::OnOK() 
{
	UpdateData(TRUE);
	CDialog::OnOK();
}

BOOL CDlgExportRaterStereo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( m_nImage<0 )
	{
		CWnd *pWnd = GetDlgItem(IDC_RADIO_LEFTIMAGE);
		if( pWnd )pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_RADIO_RIGHTIMAGE);
		if( pWnd )pWnd->EnableWindow(FALSE);
	}

	if (m_nScaleType == 0)
	{
		CWnd *pWnd = GetDlgItem(IDC_PGSCALE_EDIT);
		if( pWnd )pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_PIXELSIZE_EDIT);
		if( pWnd )pWnd->EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExportRaterStereo::OnRadioMapScale() 
{
	// TODO: Add your control notification handler code here
	CWnd *pWnd = GetDlgItem(IDC_PGSCALE_EDIT);
	if( pWnd )pWnd->EnableWindow(TRUE);
	pWnd = GetDlgItem(IDC_PIXELSIZE_EDIT);
	if( pWnd )pWnd->EnableWindow(TRUE);
}

void CDlgExportRaterStereo::OnRadioCurrentScale() 
{
	// TODO: Add your control notification handler code here
	CWnd *pWnd = GetDlgItem(IDC_PGSCALE_EDIT);
	if( pWnd )pWnd->EnableWindow(FALSE);
	pWnd = GetDlgItem(IDC_PIXELSIZE_EDIT);
	if( pWnd )pWnd->EnableWindow(FALSE);
}
