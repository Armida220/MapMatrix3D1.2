// DlgConvertCoordSys.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgConvertCoordSys.h"
#include "EditbaseDoc.h"
#include "Functions_temp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConvertCoordSys dialog


CDlgConvertCoordSys::CDlgConvertCoordSys(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConvertCoordSys::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConvertCoordSys)
	m_nRange = 0;
	m_strPath = _T("");
	//}}AFX_DATA_INIT
}


void CDlgConvertCoordSys::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConvertCoordSys)
	DDX_Radio(pDX, IDC_RADIO_ALLMAP, m_nRange);
	DDX_Text(pDX, IDC_EDIT_PATH1, m_strPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConvertCoordSys, CDialog)
	//{{AFX_MSG_MAP(CDlgConvertCoordSys)
	ON_BN_CLICKED(IDC_RADIO_ALLMAP, OnRadioRange)
	ON_BN_CLICKED(IDC_RADIO_SELECTION, OnRadioRange)
	ON_BN_CLICKED(IDC_RADIO_FILES, OnRadioRange)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_COORDSYS0, OnButtonCoordsys0)
	ON_BN_CLICKED(IDC_BUTTON_COORDSYS1, OnButtonCoordsys1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConvertCoordSys message handlers

extern BOOL DlgSelectMultiFiles(HWND hOwner, LPCTSTR filter, LPCTSTR defExt, CStringArray& fileName, CString *all_name);


void CDlgConvertCoordSys::OnRadioRange()
{
	UpdateData(TRUE);
	UpdateFileEdit();	
}


void CDlgConvertCoordSys::UpdateFileEdit()
{
	CWnd *pWnd = GetDlgItem(IDC_EDIT_PATH1);
	if( pWnd )
	{
		pWnd->EnableWindow(m_nRange==2);
	}
	
	pWnd = GetDlgItem(IDC_STATIC_FILEDESC);
	if( pWnd )
	{
		pWnd->EnableWindow(m_nRange==2);
	}
}

void CDlgConvertCoordSys::OnButtonBrowse() 
{
	if( !DlgSelectMultiFiles(GetSafeHwnd(),
		_T("Supported files (*.fdb;*.dxf;*.dwg)\0*.fdb;*.dxf;*.dwg\0All Files (*.*)\0*.*\0\0"),
		_T("*.fdb"),m_arrFileNames,&m_strPath) )
		return;
	
	UpdateData(FALSE);
}

BOOL CDlgConvertCoordSys::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	BeginCheck41License

	// TODO: Add extra initialization here
	if( GetActiveDlgDoc()==NULL )
	{
		CWnd *pWnd = GetDlgItem(IDC_RADIO_ALLMAP);
		if( pWnd )
		{
			pWnd->EnableWindow(FALSE);
		}
		pWnd = GetDlgItem(IDC_RADIO_SELECTION);
		if( pWnd )
		{
			pWnd->EnableWindow(FALSE);
		}
		m_nRange = 2;

		UpdateData(FALSE);
	}

	UpdateFileEdit();

	UpdateCoordSysEdit(0);
	UpdateCoordSysEdit(1);

	EndCheck41License
	else
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConvertCoordSys::OnButtonCoordsys0() 
{
	m_tm0.ShowDatum();
	UpdateCoordSysEdit(0);

}

void CDlgConvertCoordSys::OnButtonCoordsys1() 
{
	m_tm1.ShowDatum();
	UpdateCoordSysEdit(1);
}

/*
char zoneName[36];	// 投影带名
char tmName[36];	// 椭球名
double central;		// 中央子午线
double origin_Lat;	// 起始纬度
double scale;		// 缩放比
double false_Easting;	// 东部偏移
double false_Northing;	// 北部偏移
*/

void CDlgConvertCoordSys::UpdateCoordSysEdit(int idx)
{
	CTM * ptm;

	CWnd *pWnd = GetDlgItem(idx==0?IDC_EDIT_COORDSYS0:IDC_EDIT_COORDSYS1);
	if( pWnd==NULL )
		return;

	if( idx==0 )
		ptm = &m_tm0;
	else
		ptm = &m_tm1;

	CString text;

	TMProjectionZone prj;
	ptm->GetZone(&prj);

	text.Format(StrFromResID(IDS_CS_FORMAT), prj.tmName, prj.zoneName, (int)prj.central, prj.false_Easting);

	pWnd->SetWindowText(text);
}


void CDlgConvertCoordSys::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}