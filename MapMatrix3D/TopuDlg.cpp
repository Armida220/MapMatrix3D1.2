// TopuDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Editbase.h"
#include "TopuDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTopuDlg dialog


CTopuDlg::CTopuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTopuDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTopuDlg)
	m_lfToler = 0.0;
	m_strLayerCode = _T("");
	m_nOption = -1;
	//}}AFX_DATA_INIT
}


void CTopuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTopuDlg)
	DDX_Control(pDX, IDC_COMBO_SCHEME, m_comboxScheme);
	DDX_Text(pDX, IDC_EDIT_TOL, m_lfToler);
	DDX_Text(pDX, IDC_EDIT_TOPOLAYER, m_strLayerCode);
	DDX_Radio(pDX, IDC_RADIO_INPUT, m_nOption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTopuDlg, CDialog)
	//{{AFX_MSG_MAP(CTopuDlg)
	ON_BN_CLICKED(IDC_RADIO_INPUT, OnRadioInput)
	ON_BN_CLICKED(IDC_RADIO_OPTION, OnRadioOption)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTopuDlg message handlers

BOOL CTopuDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_lfToler = GetProfileDouble("topo\\toler","toler",0.01);
	m_strLayerCode = AfxGetApp()->GetProfileString("topo\\layercode","layercode","");
	m_nOption = AfxGetApp()->GetProfileInt("topo\\option","option",0);
	m_strCurSchemaName = AfxGetApp()->GetProfileString("topo\\schemaname","schemaname","");
	CComboBox *pCom = (CComboBox *)GetDlgItem(IDC_COMBO_SCHEME);
	BOOL bValid = FALSE;
	int sel = 0;
	for (int i=0;i<m_arrStrSchemaNames.GetSize();i++)
	{
		pCom->AddString(m_arrStrSchemaNames[i]);
		if (!bValid)
		{
			if (m_arrStrSchemaNames[i]==m_strCurSchemaName)
			{
				bValid = TRUE;
				sel = i;
			}
		}		
	}
	pCom->SetCurSel(sel);
	
	UpdateData(FALSE);
	if (m_nOption==0)
	{
		GetDlgItem(IDC_EDIT_TOPOLAYER)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_SCHEME)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_TOPOLAYER)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_SCHEME)->EnableWindow(TRUE);
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTopuDlg::OnRadioInput() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_nOption==0)
	{
		GetDlgItem(IDC_EDIT_TOPOLAYER)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_SCHEME)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_TOPOLAYER)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_SCHEME)->EnableWindow(TRUE);
	}
}

void CTopuDlg::OnRadioOption() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_nOption==0)
	{
		GetDlgItem(IDC_EDIT_TOPOLAYER)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_SCHEME)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_TOPOLAYER)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_SCHEME)->EnableWindow(TRUE);
	}	
}

void CTopuDlg::OnOK() 
{
	CDialog::OnOK();
// 	CComboBox *pCom = (CComboBox *)GetDlgItem(IDC_COMBO_SCHEME);
// 	pCom->getit
	GetDlgItem(IDC_COMBO_SCHEME)->GetWindowText(m_strCurSchemaName);
	WriteProfileDouble("topo\\toler","toler",m_lfToler);
	AfxGetApp()->WriteProfileString("topo\\layercode","layercode",m_strLayerCode);
	AfxGetApp()->WriteProfileInt("topo\\option","option",m_nOption);
	AfxGetApp()->WriteProfileString("topo\\schemaname","schemaname",m_strCurSchemaName);	
}
