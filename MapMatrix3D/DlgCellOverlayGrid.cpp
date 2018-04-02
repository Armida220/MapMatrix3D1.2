// DlgCellOverlayGrid.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgCellOverlayGrid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCellOverlayGrid dialog


CDlgCellOverlayGrid::CDlgCellOverlayGrid(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCellOverlayGrid::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCellOverlayGrid)
	m_bOverlay = FALSE;
	m_fHei = 100.0f;
	m_fWid = 100.0f;
	m_fXOrigin = 0.0f;
	m_fYOrigin = 0.0f;
	m_bImgView = FALSE;
	m_bVectView = FALSE;
	m_fXRange = 1000.0f;
	m_fYRange = 1000.0f;
	m_bSnapGrid = FALSE;
	m_strZ = _T("0.0");
	//}}AFX_DATA_INIT

	m_color = RGB(255,255,255);
	memset(m_ptRegion,0,sizeof(m_ptRegion));
}


void CDlgCellOverlayGrid::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCellOverlayGrid)
	DDX_Control(pDX, IDC_BUTTON_OG_COLOR, m_colorBtn);
	DDX_Check(pDX, IDC_CHECK_OG_OVERLAY, m_bOverlay);
	DDX_Text(pDX, IDC_EDIT_OG_HEI, m_fHei);
	DDX_Text(pDX, IDC_EDIT_OG_WID, m_fWid);
	DDX_Text(pDX, IDC_EDIT_OG_XO, m_fXOrigin);
	DDX_Text(pDX, IDC_EDIT_OG_YO, m_fYOrigin);
	DDX_Check(pDX, IDC_CHECK_OG_IMGVIEW, m_bImgView);
	DDX_Check(pDX, IDC_CHECK_OG_VECTVIEW, m_bVectView);
	DDX_Text(pDX, IDC_EDIT_OG_XR, m_fXRange);
	DDX_Text(pDX, IDC_EDIT_OG_YR, m_fYRange);
	DDX_Check(pDX, IDC_CHECK_OG_SNAP, m_bSnapGrid);
	DDX_Text(pDX, IDC_EDIT_OG_Z, m_strZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCellOverlayGrid, CDialog)
	//{{AFX_MSG_MAP(CDlgCellOverlayGrid)
	ON_BN_CLICKED(IDC_BUTTON_OG_WSREGION, OnWSRegion)
	ON_BN_CLICKED(IDC_BUTTON_DEM, OnButtonDem)
	ON_EN_CHANGE(IDC_EDIT_OG_WID, OnChangeEditOgWid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayGrid message handlers

// BOOL CheckStrZ(LPCTSTR strz, float& z)
// {
// 	char *pos;
// 	float v = strtod(strz,&pos);
// 	if( strlen(pos)<=0 )
// 	{
// 		z = v;
// 		return TRUE;
// 	}
// 	return FALSE;
// }

void CDlgCellOverlayGrid::OnOK() 
{
	m_color = m_colorBtn.GetColor();
	CDialog::OnOK();
}

BOOL CDlgCellOverlayGrid::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_colorBtn.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_colorBtn.SetColor(m_color);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgCellOverlayGrid::OnWSRegion() 
{
	UpdateData(TRUE);

	Envelope evlp;
	evlp.CreateFromPts(m_ptRegion,4);

	m_fXOrigin = evlp.m_xl;
	m_fYOrigin = evlp.m_yl;
	m_fXRange = evlp.m_xh-evlp.m_xl;
	m_fYRange = evlp.m_yh-evlp.m_yl;

	UpdateData(FALSE);
}

void CDlgCellOverlayGrid::OnButtonDem() 
{
	UpdateData(TRUE);
	CString filter(StrFromResID(IDS_LOADDEM_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	m_strZ = dlg.GetPathName();
	UpdateData(FALSE);
}

void CDlgCellOverlayGrid::OnChangeEditOgWid() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	m_fHei = m_fWid;

	UpdateData(FALSE);

	
}
