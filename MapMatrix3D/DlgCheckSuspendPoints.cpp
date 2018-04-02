// DlgCheckSuspendPoints.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgCheckSuspendPoints.h"
#include "DlgDataSource.h"
#include "DlgSelectLayer.h"
#include "RegDef2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckSuspendPoints dialog


CDlgCheckSuspendPoints::CDlgCheckSuspendPoints(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCheckSuspendPoints::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCheckSuspendPoints)
	m_bCrossLay = FALSE;
	m_bCheckZ = FALSE;
	m_bCheckVEP = TRUE;
	m_bCheckBound = TRUE;
	m_strLayNames = _T("");
	m_strBoundLayer = _T("");
	m_pDS = NULL;
	m_lfRange = 10e6;
	m_bCheckRange = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgCheckSuspendPoints::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCheckSuspendPoints)
	DDX_Check(pDX, IDC_CHECK_SUS_CROSSLAY, m_bCrossLay);
	DDX_Check(pDX, IDC_CHECK_SUS_Z, m_bCheckZ);
	DDX_Check(pDX, IDC_CHECK_VEP, m_bCheckVEP);
	DDX_Check(pDX, IDC_CHECK4, m_bCheckBound);
	DDX_Check(pDX, IDC_CHECK_OUTRANGE, m_bCheckRange);
	DDX_Text(pDX, IDC_EDIT_LAYNAME, m_strLayNames);
	DDX_Text(pDX, IDC_BOUNDLAYER, m_strBoundLayer);
	DDX_Text(pDX, IDC_EDIT_RANGE, m_lfRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCheckSuspendPoints, CDialog)
	//{{AFX_MSG_MAP(CDlgCheckSuspendPoints)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE1, OnButtonBrowse1)
	ON_BN_CLICKED(IDC_CHECK4, OnBoundCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckSuspendPoints message handlers

BOOL CDlgCheckSuspendPoints::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_bCheckZ = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTZ,m_bCheckZ);
	m_bCrossLay = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTCROSSLAY,m_bCrossLay);
	m_bCheckVEP = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTVEP,m_bCheckVEP);
	m_bCheckBound = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_SUSPTBOUND,m_bCheckBound);
	m_lfRange = GetProfileDouble(REGPATH_QT,"SusPointRange",m_lfRange);
	m_strLayNames  = AfxGetApp()->GetProfileString(REGPATH_QT,REGITEM_QT_SUSPTLAYS,m_strLayNames);
	m_strBoundLayer = AfxGetApp()->GetProfileString(REGPATH_QT,REGITEM_QT_BOUNDLAY,m_strBoundLayer);
	m_bCheckRange = AfxGetApp()->GetProfileInt(REGPATH_QT,"CheckRange",m_bCheckRange);

	GetDlgItem(IDC_BOUNDLAYER)->EnableWindow(m_bCheckBound);
	GetDlgItem(IDC_BUTTON_BROWSE1)->EnableWindow(m_bCheckBound);

	UpdateData(FALSE);

	return TRUE;
}

void CDlgCheckSuspendPoints::OnButtonBrowse() 
{
	if( !m_pDS )
		return;

	UpdateData(TRUE);

	CDlgSelectFtrLayer dlg(NULL,LAYER_SEL_MODE_MUTISEL);
	dlg.m_pDS = m_pDS;	
	dlg.m_bUsed = TRUE;
	dlg.m_bLocal = TRUE;
	dlg.m_bNotLocal = TRUE;
	dlg.m_strLayers = m_strLayNames;

	if( dlg.DoModal()!=IDOK )
		return;

	m_strLayNames = dlg.m_strLayers;

	UpdateData(FALSE);
	
}

void CDlgCheckSuspendPoints::OnButtonBrowse1() 
{
	if( !m_pDS )
		return;
	
	UpdateData(TRUE);
	
	CDlgSelectFtrLayer dlg;
	dlg.m_pDS = m_pDS;	
	dlg.m_bUsed = TRUE;
	dlg.m_bLocal = TRUE;
	dlg.m_bNotLocal = TRUE;
	
	if( dlg.DoModal()!=IDOK )
		return;
	
	m_strBoundLayer = dlg.m_SingleLayer;
	
	UpdateData(FALSE);
	
}

void CDlgCheckSuspendPoints::OnOK() 
{
	UpdateData(TRUE);

	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_SUSPTZ,m_bCheckZ);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_SUSPTCROSSLAY,m_bCrossLay);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_SUSPTVEP,m_bCheckVEP);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_SUSPTBOUND,m_bCheckBound);
	
	AfxGetApp()->WriteProfileString(REGPATH_QT,REGITEM_QT_SUSPTLAYS,m_strLayNames);
	AfxGetApp()->WriteProfileString(REGPATH_QT,REGITEM_QT_BOUNDLAY,m_strBoundLayer);
	WriteProfileDouble(REGPATH_QT,"SusPointRange", m_lfRange);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,"CheckRange",m_bCheckRange);
	CDialog::OnOK();
}

void CDlgCheckSuspendPoints::OnBoundCheck()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_BOUNDLAYER)->EnableWindow(m_bCheckBound);
	GetDlgItem(IDC_BUTTON_BROWSE1)->EnableWindow(m_bCheckBound);
}


/////////////////////////////////////////////////////////////////////////////
// CDlgCheckPseudoPoints dialog


CDlgCheckPseudoPoints::CDlgCheckPseudoPoints(CWnd* pParent /*=NULL*/)
: CDialog(CDlgCheckPseudoPoints::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCheckPseudoPoints)
	m_bCrossLay = FALSE;
	m_bCheckZ = TRUE;
	m_strLayNames = _T("");
	m_pDS = NULL;
	//}}AFX_DATA_INIT
}


void CDlgCheckPseudoPoints::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCheckPseudoPoints)
	DDX_Check(pDX, IDC_CHECK_SUS_CROSSLAY, m_bCrossLay);
	DDX_Check(pDX, IDC_CHECK_SUS_Z, m_bCheckZ);
	DDX_Text(pDX, IDC_EDIT_LAYNAME, m_strLayNames);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCheckPseudoPoints, CDialog)
	//{{AFX_MSG_MAP(CDlgCheckPseudoPoints)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckPseudoPoints message handlers

BOOL CDlgCheckPseudoPoints::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bCheckZ = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_PSEPTZ,m_bCheckZ);
	m_bCrossLay = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_PSEPTCROSSLAY,m_bCrossLay);
	
	m_strLayNames  = AfxGetApp()->GetProfileString(REGPATH_QT,REGITEM_QT_PSEPTLAYS,m_strLayNames);
	
	UpdateData(FALSE);

	return TRUE;
}

void CDlgCheckPseudoPoints::OnButtonBrowse() 
{
	if( !m_pDS )
		return;

	UpdateData(TRUE);
	
	CDlgSelectFtrLayer dlg(NULL,LAYER_SEL_MODE_MUTISEL);
	dlg.m_pDS = m_pDS;	
	dlg.m_bUsed = TRUE;
	dlg.m_bLocal = TRUE;
	dlg.m_bNotLocal = TRUE;
	dlg.m_strLayers = m_strLayNames;
	
	if( dlg.DoModal()!=IDOK )
		return;

	m_strLayNames = dlg.m_strLayers;

	UpdateData(FALSE);
	
}

void CDlgCheckPseudoPoints::OnOK() 
{
	UpdateData(TRUE);
	
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_PSEPTZ,m_bCheckZ);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_PSEPTCROSSLAY,m_bCrossLay);
	
	AfxGetApp()->WriteProfileString(REGPATH_QT,REGITEM_QT_PSEPTLAYS,m_strLayNames);
	
	CDialog::OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// CDlgProcessSuspendPoints dialog


CDlgProcessSuspendPoints::CDlgProcessSuspendPoints(CWnd* pParent /*=NULL*/)
: CDialog(CDlgProcessSuspendPoints::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProcessSuspendPoints)
	m_fToler = 0.2f;
	m_fTolerZ = 0.2f;
	m_bInsertPoint = FALSE;
	m_bAllowVSuspend = TRUE;
	//}}AFX_DATA_INIT
}


void CDlgProcessSuspendPoints::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProcessSuspendPoints)
	DDX_Text(pDX, IDC_EDIT_TOLERANCE, m_fToler);
	DDX_Text(pDX, IDC_EDIT_TOLERANCEZ, m_fTolerZ);
	DDX_Check(pDX, IDC_CHECK_INSERTPOINT, m_bInsertPoint);
	DDX_Check(pDX, IDC_CHECK_AllOW_V, m_bAllowVSuspend);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProcessSuspendPoints, CDialog)
	//{{AFX_MSG_MAP(CDlgProcessSuspendPoints)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProcessSuspendPoints message handlers

BOOL CDlgProcessSuspendPoints::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_fToler	= GetProfileDouble(REGPATH_QT,REGITEM_QT_SUSPTRADIUS,m_fToler);
	m_fTolerZ	= GetProfileDouble(REGPATH_QT,REGITEM_QT_SUSPTZTOLER,m_fTolerZ);
	m_bInsertPoint	= AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_QT_INSERTPOINT,m_bInsertPoint);
	m_bAllowVSuspend = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_ALLOWVSUSPEND,m_bAllowVSuspend);
	
	UpdateData(FALSE);
	
	return TRUE;
}


void CDlgProcessSuspendPoints::OnCancel() 
{
	UpdateData(TRUE);
	
	WriteProfileDouble(REGPATH_QT,REGITEM_QT_SUSPTRADIUS,m_fToler);
	WriteProfileDouble(REGPATH_QT,REGITEM_QT_SUSPTZTOLER,m_fTolerZ);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_INSERTPOINT,m_bInsertPoint);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_ALLOWVSUSPEND,m_bAllowVSuspend);
	
	CDialog::OnCancel();
}


void CDlgProcessSuspendPoints::OnOK() 
{
	UpdateData(TRUE);
	
	WriteProfileDouble(REGPATH_QT,REGITEM_QT_SUSPTRADIUS,m_fToler);
	WriteProfileDouble(REGPATH_QT,REGITEM_QT_SUSPTZTOLER,m_fTolerZ);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_QT_INSERTPOINT,m_bInsertPoint);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_ALLOWVSUSPEND,m_bAllowVSuspend);
	
	CDialog::OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// CDlgProcessMatchEndPoints dialog


CDlgProcessMatchEndPoints::CDlgProcessMatchEndPoints(CWnd* pParent /*=NULL*/)
: CDialog(CDlgProcessMatchEndPoints::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProcessMatchEndPoints)
	m_bCrossLay = FALSE;
	m_bCheckZ = FALSE;
	m_strLayNames = _T("");
	m_fTolerXY = 0.2;
	m_fTolerZ = 0.2;
	m_pDS = NULL;
	//}}AFX_DATA_INIT
}


void CDlgProcessMatchEndPoints::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProcessMatchEndPoints)
	DDX_Check(pDX, IDC_CHECK_SUS_CROSSLAY, m_bCrossLay);
	DDX_Check(pDX, IDC_CHECK_SUS_Z, m_bCheckZ);
	DDX_Text(pDX, IDC_EDIT_LAYNAME, m_strLayNames);
	DDX_Text(pDX, IDC_EDIT_TOLERANCE, m_fTolerXY);
	DDX_Text(pDX, IDC_EDIT_TOLERANCEZ, m_fTolerZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProcessMatchEndPoints, CDialog)
	//{{AFX_MSG_MAP(CDlgProcessMatchEndPoints)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckSuspendPoints message handlers

BOOL CDlgProcessMatchEndPoints::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bCheckZ = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_MATCHPT_Z,m_bCheckZ);
	m_bCrossLay = AfxGetApp()->GetProfileInt(REGPATH_QT,REGITEM_MATCHPT_CROSSLAY,m_bCrossLay);	
	m_strLayNames  = AfxGetApp()->GetProfileString(REGPATH_QT,REGITEM_MATCHPT_LAYS,m_strLayNames);

	m_fTolerXY	= GetProfileDouble(REGPATH_QT,REGITEM_MATCHPT_XYTOLER,m_fTolerXY);
	m_fTolerZ	= GetProfileDouble(REGPATH_QT,REGITEM_MATCHPT_ZTOLER,m_fTolerZ);
	
	UpdateData(FALSE);
	
	return TRUE;
}

void CDlgProcessMatchEndPoints::OnButtonBrowse() 
{
	if( !m_pDS )
		return;
	
	UpdateData(TRUE);
	
	CDlgSelectFtrLayer dlg(NULL,LAYER_SEL_MODE_MUTISEL);
	dlg.m_pDS = m_pDS;	
	dlg.m_bUsed = TRUE;
	dlg.m_bLocal = TRUE;
	dlg.m_bNotLocal = TRUE;
	dlg.m_strLayers = m_strLayNames;
	
	if( dlg.DoModal()!=IDOK )
		return;
	
	m_strLayNames = dlg.m_strLayers;
	
	UpdateData(FALSE);
	
}

void CDlgProcessMatchEndPoints::OnOK() 
{
	UpdateData(TRUE);
	
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_MATCHPT_Z,m_bCheckZ);
	AfxGetApp()->WriteProfileInt(REGPATH_QT,REGITEM_MATCHPT_CROSSLAY,m_bCrossLay);
	
	AfxGetApp()->WriteProfileString(REGPATH_QT,REGITEM_MATCHPT_LAYS,m_strLayNames);

	WriteProfileDouble(REGPATH_QT,REGITEM_MATCHPT_XYTOLER,m_fTolerXY);
	WriteProfileDouble(REGPATH_QT,REGITEM_MATCHPT_ZTOLER,m_fTolerZ);
	
	CDialog::OnOK();
}
