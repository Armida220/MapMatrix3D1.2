// DlgTestContourSettings.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgTestContourSettings.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTestContourSettings dialog


CDlgTestContourSettings::CDlgTestContourSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTestContourSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTestContourSettings)
	m_fInterval = 5.0f;
	//}}AFX_DATA_INIT
}


void CDlgTestContourSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTestContourSettings)
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_fInterval);
	DDX_Control(pDX, IDC_LIST_FEATURECODE, m_lstCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTestContourSettings, CDialog)
	//{{AFX_MSG_MAP(CDlgTestContourSettings)
// 	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
// 	ON_BN_CLICKED(IDC_DESELECTALL, OnDeselectall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTestContourSettings message handlers

BOOL CDlgTestContourSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_lstCtrl.SetExtendedStyle(m_lstCtrl.GetExtendedStyle()|LVS_EX_CHECKBOXES);
	
    m_lstCtrl.InsertColumn (0, StrFromResID(IDS_SELECT_FIELD),LVCFMT_CENTER);
	m_lstCtrl.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER);

	CStringArray strUsedList;
	int pos = -1;
	do
	{
		pos = m_strLayers.Find(',');
		if( pos>=0 )
		{
			strUsedList.Add(m_strLayers.Left(pos));
			m_strLayers = m_strLayers.Mid(pos+1);
		}
		else
		{
			if( m_strLayers.GetLength()>0 )
				strUsedList.Add(m_strLayers);
		}
	}while( pos>=0 );

	int nsize = m_strInitList.GetSize();
	int nsize2 = strUsedList.GetSize();
	for( int i=0; i<nsize; i++)
	{
		int idx = m_lstCtrl.InsertItem(i,m_strInitList[i]);

		for( int j=0; j<nsize2; j++)
		{
			if( m_strInitList[i].CompareNoCase(strUsedList[j])==0 )
				break;
		}
		if( nsize2==0 || j<nsize2 )
			m_lstCtrl.SetCheck(idx);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTestContourSettings::OnOK() 
{
	CString str;
	m_strLayers.Empty();
	int	iCount = m_lstCtrl.GetItemCount();
	for (int i=0; i<iCount; i++)
	{	
		if(m_lstCtrl.GetCheck(i))
		{
			str = m_lstCtrl.GetItemText(i,0);

			if( !m_strLayers.IsEmpty() )m_strLayers += ',';
			m_strLayers += str;
		}
	}

	if( m_strLayers.IsEmpty() )
		m_strLayers = "@";
	
	CDialog::OnOK();
}

// void CDlgTestContourSettings::OnSelectall() 
// {
// 	int	iCount = m_lstCtrl.GetItemCount();
// 	for (int i=0; i<iCount; i++)
// 	{	
// 		m_lstCtrl.SetCheck(i,TRUE);
// 	}
// }
// 
// void CDlgTestContourSettings::OnDeselectall() 
// {
// 	int	iCount = m_lstCtrl.GetItemCount();
// 	for (int i=0; i<iCount; i++)
// 	{	
// 		m_lstCtrl.SetCheck(i,FALSE);
// 	}
// }
