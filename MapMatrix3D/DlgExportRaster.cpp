// DlgExportRaster.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgExportRaster.h"
#include "UIFFileDialogEx.h"
#include "SmartViewFunctions.h"
#include "SilenceDDX.h"
#include "Regdef2.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportRaster dialog


CDlgExportRaster::CDlgExportRaster(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportRaster::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportRaster)
	m_strFilePath = _T("");
	m_nBoundType = 0;
	m_nResolutionType = 0;
	m_nDPI = 300;
	m_fMarginLeft = 25;
	m_fMarginRight = 25;
	m_fMarginTop = 25;
	m_fMarginBottom = 25;
	//}}AFX_DATA_INIT
}


void CDlgExportRaster::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportRaster)
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strFilePath);
	DDX_Radio(pDX, IDC_RADIO_IMAGEBOUND, m_nBoundType);
	DDX_Radio(pDX, IDC_RADIO_IMAGERESOLUTION, m_nResolutionType);
	DDX_Text(pDX, IDC_EDIT_DPI, m_nDPI);
	DDX_Text_Silence(pDX,IDC_EDIT_LEFT,m_fMarginLeft);
	DDX_Text_Silence(pDX,IDC_EDIT_RIGHT,m_fMarginRight);
	DDX_Text_Silence(pDX,IDC_EDIT_TOP,m_fMarginTop);
	DDX_Text_Silence(pDX,IDC_EDIT_BOTTOM,m_fMarginBottom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportRaster, CDialog)
	//{{AFX_MSG_MAP(CDlgExportRaster)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_RADIO_IMAGEBOUND, OnRadioClickBoundType)
	ON_BN_CLICKED(IDC_RADIO_WORKSPACE, OnRadioClickBoundType)
	ON_BN_CLICKED(IDC_RADIO_DPI, OnRadioClickDPI)
	ON_BN_CLICKED(IDC_RADIO_VIEWSCALE, OnRadioClickDPI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportRaster message handlers


BOOL CDlgExportRaster::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	m_fMarginLeft = GetProfileDouble(REGPATH_OTHER,"ExportRasterMarginLeft",m_fMarginLeft);
	m_fMarginRight = GetProfileDouble(REGPATH_OTHER,"ExportRasterMarginRight",m_fMarginRight);
	m_fMarginTop = GetProfileDouble(REGPATH_OTHER,"ExportRasterMarginTop",m_fMarginTop);
	m_fMarginBottom = GetProfileDouble(REGPATH_OTHER,"ExportRasterMarginBottom",m_fMarginBottom);
	
	if (!m_bOverImg)
	{
		m_nBoundType = 1;
	}	
	
	UpdateData(FALSE);
	OnRadioClickBoundType();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void CDlgExportRaster::OnOK() 
{
	UpdateData(TRUE);
	
	CDialog::OnOK();
	
	WriteProfileDouble(REGPATH_OTHER,"ExportRasterMarginLeft",m_fMarginLeft);
	WriteProfileDouble(REGPATH_OTHER,"ExportRasterMarginRight",m_fMarginRight);
	WriteProfileDouble(REGPATH_OTHER,"ExportRasterMarginTop",m_fMarginTop);
	WriteProfileDouble(REGPATH_OTHER,"ExportRasterMarginBottom",m_fMarginBottom);
}

void CDlgExportRaster::OnButtonBrowse() 
{
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, ".tif", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("TIFF File(*.tif)|*.tif|All Files(*.*)|*.*||"));
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strFilePath = dlg.GetPathName();
	UpdateData(FALSE);
}


void CDlgExportRaster::OnRadioClickBoundType()
{
	UpdateData(TRUE);

	EnableCtrl(IDC_RADIO_IMAGERESOLUTION,m_nBoundType==0);
	EnableCtrl(IDC_RADIO_DPI,m_nBoundType!=0);
	EnableCtrl(IDC_RADIO_VIEWSCALE,m_nBoundType!=0);
	
	if(m_nBoundType==0)
	{
		m_nResolutionType = 0;
	}
	else
	{
		if(m_nResolutionType==0)
			m_nResolutionType = 1;
	}

	EnableCtrl(IDC_EDIT_DPI,m_nBoundType!=0 && m_nResolutionType==1 );
	
	EnableCtrl(IDC_EDIT_LEFT,m_nBoundType!=0 && m_nResolutionType==1 );
	EnableCtrl(IDC_EDIT_RIGHT,m_nBoundType!=0 && m_nResolutionType==1 );
	EnableCtrl(IDC_EDIT_TOP,m_nBoundType!=0 && m_nResolutionType==1 );
	EnableCtrl(IDC_EDIT_BOTTOM,m_nBoundType!=0 && m_nResolutionType==1 );
	
	CheckCtrl(IDC_RADIO_IMAGERESOLUTION,m_nResolutionType==0);
	CheckCtrl(IDC_RADIO_DPI,m_nResolutionType==1);
	CheckCtrl(IDC_RADIO_VIEWSCALE,m_nResolutionType==2);
	
	UpdateData(TRUE);
}



void CDlgExportRaster::OnRadioClickDPI()
{
	UpdateData(TRUE);
	
	EnableCtrl(IDC_EDIT_DPI,m_nBoundType!=0 && m_nResolutionType==1 );
	EnableCtrl(IDC_EDIT_LEFT,m_nBoundType!=0 && m_nResolutionType==1 );
	EnableCtrl(IDC_EDIT_RIGHT,m_nBoundType!=0 && m_nResolutionType==1 );
	EnableCtrl(IDC_EDIT_TOP,m_nBoundType!=0 && m_nResolutionType==1 );
	EnableCtrl(IDC_EDIT_BOTTOM,m_nBoundType!=0 && m_nResolutionType==1 );
}

void CDlgExportRaster::EnableCtrl(int ID, BOOL bEnable)
{
	CWnd *pWnd = GetDlgItem(ID);
	if(pWnd)
	{
		pWnd->EnableWindow(bEnable);
	}
}



void CDlgExportRaster::CheckCtrl(int ID, BOOL bEnable)
{
	CWnd *pWnd = GetDlgItem(ID);
	if(pWnd)
	{
		pWnd->SendMessage(BM_SETCHECK, bEnable?1:0, 0);
	}
}