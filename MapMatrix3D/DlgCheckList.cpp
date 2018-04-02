// DlgCheckList.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgCheckList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckList dialog


CDlgCheckList::CDlgCheckList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCheckList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCheckList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgCheckList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCheckList)
	DDX_Control(pDX, IDC_LIST, m_wndList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCheckList, CDialog)
	//{{AFX_MSG_MAP(CDlgCheckList)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectAll)
	ON_BN_CLICKED(IDC_SELECTNONE, OnSelectNone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckList message handlers


BOOL CDlgCheckList::OnInitDialog()
{
	if( !CDialog::OnInitDialog() )
		return FALSE;

	m_wndList.SetCheckStyle(BS_CHECKBOX);

	for( int i=0; i<m_arrStrList.GetSize(); i++)
	{
		m_wndList.AddString(m_arrStrList[i]);
	}
	for( i=0; i<m_arrFlags.GetSize(); i++)
	{
		m_wndList.SetCheck(i,m_arrFlags[i]);
	}
	return TRUE;
}

void CDlgCheckList::OnOK() 
{
	for( int i=0; i<m_arrFlags.GetSize(); i++)
	{
		m_arrFlags[i] = m_wndList.GetCheck(i);
	}
	
	CDialog::OnOK();
}


void CDlgCheckList::OnSelchangeList()
{
	int nsel = m_wndList.GetCurSel();
	m_wndList.SetCheck(nsel,1-m_wndList.GetCheck(nsel));
}


void CDlgCheckList::OnSelectAll()
{
	for( int i=0; i<m_arrFlags.GetSize(); i++)
	{
		m_arrFlags[i] = 1;
		m_wndList.SetCheck(i,1);
	}
}


void CDlgCheckList::OnSelectNone()
{
	for( int i=0; i<m_arrFlags.GetSize(); i++)
	{
		m_arrFlags[i] = 0;
		m_wndList.SetCheck(i,0);
	}
}
