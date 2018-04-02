// DlgMapInfo.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgMapInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMapInfo dialog


CDlgMapInfo::CDlgMapInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMapInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMapInfo)
	m_strProjection = _T("");
	//}}AFX_DATA_INIT
}


void CDlgMapInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMapInfo)
	DDX_Text(pDX, IDC_EDIT_PROJECTION, m_strProjection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMapInfo, CDialog)
	//{{AFX_MSG_MAP(CDlgMapInfo)
	ON_BN_CLICKED(IDC_BUTTON_PROJECTION, OnButtonProjection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMapInfo message handlers

void CDlgMapInfo::OnButtonProjection() 
{
	m_tm0.ShowDatum();
	UpdateCoordSysEdit();
}

void CDlgMapInfo::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL CDlgMapInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateCoordSysEdit();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CDlgMapInfo::UpdateCoordSysEdit()
{
	CTM * ptm;
	
	CWnd *pWnd = GetDlgItem(IDC_EDIT_PROJECTION);
	if( pWnd==NULL )
		return;
	
	ptm = &m_tm0;
	
	CString text;
	
	TMProjectionZone prj;
	ptm->GetZone(&prj);
	
	text.Format(StrFromResID(IDS_CS_FORMAT), prj.tmName, prj.zoneName, (int)prj.central, prj.false_Easting);
	
	pWnd->SetWindowText(text);
}
