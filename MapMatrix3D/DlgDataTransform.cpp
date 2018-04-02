// DlgDataTransform.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgDataTransform.h"
#include "editbasedoc.h"
#include "Functions_temp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDataTransform dialog


CDlgDataTransform::CDlgDataTransform(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDataTransform::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDataTransform)
	m_lfDX = 0.0;
	m_lfDY = 0.0;
	m_lfDZ = 0.0;
	m_lfKX = 1.0;
	m_lfKY = 1.0;
	m_lfKZ = 1.0;
	m_strPath = _T("");
	m_nRange = 0;
	//}}AFX_DATA_INIT
}


void CDlgDataTransform::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDataTransform)
	DDX_Control(pDX, IDC_COMBO_METRIC, m_wndMetric);
	DDX_Text(pDX, IDC_EDIT_DX, m_lfDX);
	DDX_Text(pDX, IDC_EDIT_DY, m_lfDY);
	DDX_Text(pDX, IDC_EDIT_DZ, m_lfDZ);
	DDX_Text(pDX, IDC_EDIT_KX, m_lfKX);
	DDX_Text(pDX, IDC_EDIT_KY, m_lfKY);
	DDX_Text(pDX, IDC_EDIT_KZ, m_lfKZ);
	DDX_Text(pDX, IDC_EDIT_PATH1, m_strPath);
	DDX_Radio(pDX, IDC_RADIO_ALLMAP, m_nRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDataTransform, CDialog)
	//{{AFX_MSG_MAP(CDlgDataTransform)
	ON_CBN_SELCHANGE(IDC_COMBO_METRIC, OnSelchangeComboMetric)
	ON_BN_CLICKED(IDC_RADIO_ALLMAP, OnRadioRange)
	ON_BN_CLICKED(IDC_RADIO_SELECTION, OnRadioRange)
	ON_BN_CLICKED(IDC_RADIO_FILES, OnRadioRange)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern BOOL DlgSelectMultiFiles(HWND hOwner, LPCTSTR filter, LPCTSTR defExt, CStringArray& fileName, CString *all_name);

/////////////////////////////////////////////////////////////////////////////
// CDlgDataTransform message handlers

void CDlgDataTransform::OnSelchangeComboMetric() 
{
	UpdateData(TRUE);
	int nsel = m_wndMetric.GetCurSel();
	if( nsel>=0 )
	{
		double v = m_arrCoefs.GetAt(nsel);
		m_lfKX = m_lfKY = m_lfKZ = v;
		UpdateData(FALSE);
	}
}

BOOL CDlgDataTransform::OnInitDialog() 
{
	CDialog::OnInitDialog();

	BeginCheck41License

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
	
	int i, num = m_wndMetric.GetCount();
	CString text;
	CStringArray strlist;
	for( i=0; i<num; i++)
	{
		m_wndMetric.GetLBText(i,text);
		strlist.Add(text);
	}

	m_wndMetric.ResetContent();
	for( i=0; i<num; i++)
	{
		text = strlist.GetAt(i);
		int pos = text.Find('|');
		double v = 0;
		if( pos>=0 )
		{
			m_arrCoefs.Add(atof(text.Mid(pos+1)));
			text = text.Left(pos);
		}		
		m_wndMetric.AddString(text);
	}

	m_wndMetric.SetCurSel(0);
	OnSelchangeComboMetric();

	UpdateFileEdit();

	EndCheck41License
	else
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDataTransform::OnOK() 
{	
	CDialog::OnOK();
}


void CDlgDataTransform::OnRadioRange()
{
	UpdateData(TRUE);
	UpdateFileEdit();	
}


void CDlgDataTransform::UpdateFileEdit()
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



void CDlgDataTransform::OnButtonBrowse() 
{
	if( !DlgSelectMultiFiles(GetSafeHwnd(),
		_T("Supported files (*.fdb;*.dxf;*.dwg)\0*.fdb;*.dxf;*.dwg\0All Files (*.*)\0*.*\0\0"),
		_T("*.fdb"),m_arrFileNames,&m_strPath) )
		return;
	
	UpdateData(FALSE);
}
