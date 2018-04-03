// DlgSymbolizeFlag.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgSymbolizeFlag.h"

#include "Symbol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSymbolizeFlag dialog


CDlgSymbolizeFlag::CDlgSymbolizeFlag(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSymbolizeFlag::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSymbolizeFlag)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nSymbolizeFlag_old = m_nSymbolizeFlag = 0;

	AutoAddFlagItems();
}


void CDlgSymbolizeFlag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSymbolizeFlag)
	DDX_Control(pDX, IDC_LIST, m_wndList1);
	DDX_Control(pDX, IDC_LIST2, m_wndList2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSymbolizeFlag, CDialog)
	//{{AFX_MSG_MAP(CDlgSymbolizeFlag)
	//ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSymbolizeFlag message handlers


void CDlgSymbolizeFlag::AutoAddFlagItems()
{
	AddFlagItem(StrFromResID(IDS_SYMBOLIZETEXT),SYMFLAG_TEXT,1);
	AddFlagItem(StrFromResID(IDS_HIDE_SURFACE_BOUND),SYMFLAG_HIDE_SURFACE_BOUND,1);
	AddFlagItem(StrFromResID(IDS_HIDE_SURFACE_INNER_BOUND),SYMFLAG_HIDE_SURFACE_INNER_BOUND,1);

	AddFlagItem(StrFromResID(IDS_SYMBOLIZEFLAG_ZERO),SYMFLAG_ENUM_ZERO,0);
	AddFlagItem(StrFromResID(IDS_SYMBOLIZEFLAG_NODIANGAN1),SYMFLAG_ENUM_NODIANGAN1,0);
	AddFlagItem(StrFromResID(IDS_SYMBOLIZEFLAG_NODIANGAN2),SYMFLAG_ENUM_NODIANGAN2,0);
	AddFlagItem(StrFromResID(IDS_SYMBOLIZEFLAG_NODIANGAN3),SYMFLAG_ENUM_NODIANGAN3,0);
}

BOOL CDlgSymbolizeFlag::OnInitDialog()
{
	CDialog::OnInitDialog();

	for( int i=0; i<m_arrItems.GetSize(); i++)
	{
		if( m_arrItems[i].isbit )
		{
			int nItem = m_wndList1.AddString(m_arrItems[i].name);
			m_wndList1.SetItemData(nItem,i);
		}
		else
		{
			int nItem = m_wndList2.AddString(m_arrItems[i].name);
			m_wndList2.SetItemData(nItem,i);
		}
	}

	FlagBit bit(m_nSymbolizeFlag);

	for( i=0; i<m_wndList1.GetCount(); i++)
	{
		int idx = m_wndList1.GetItemData(i);
		if( bit.GetBit(m_arrItems[idx].data) )
		{
			m_wndList1.SetCheck(idx,1);
		}
		else
		{
			m_wndList1.SetCheck(idx,0);
		}
	}

	int value = bit.GetEnumValue();
	m_wndList2.SetCurSel(value);

	return TRUE;
}


void CDlgSymbolizeFlag::AddFlagItem(LPCTSTR name, int data, int isbit)
{
	m_arrItems.Add(FlagItem(name,data,isbit));
}

void CDlgSymbolizeFlag::SetSymbolizeFlag(long flag)
{
	m_nSymbolizeFlag = m_nSymbolizeFlag_old = flag;
}


long CDlgSymbolizeFlag::GetSymbolizeFlag()
{
	return m_nSymbolizeFlag;
}

void CDlgSymbolizeFlag::OnSelchangeList1()
{
	int nsel = m_wndList1.GetCurSel();
	m_wndList1.SetCheck(nsel,1-m_wndList1.GetCheck(nsel));
}


CString CDlgSymbolizeFlag::GetDisplayText()
{
	CString text;
	FlagBit bit(m_nSymbolizeFlag);

	for( int i=0; i<m_arrItems.GetSize(); i++)
	{
		if( m_arrItems[i].isbit )
		{
			if( bit.GetBit(m_arrItems[i].data) )
			{
				text += m_arrItems[i].name + ',';
			}
		}
		else
		{
			if( bit.GetEnumValue()==m_arrItems[i].data )
			{
				text += m_arrItems[i].name + ',';
			}
		}
	}

	if( !text.IsEmpty() )
	{
		text.Delete(text.GetLength()-1);
	}
	return text;
}


void CDlgSymbolizeFlag::OnOK()
{
	FlagBit bit(0);
	
	for( int i=0; i<m_wndList1.GetCount(); i++)
	{
		int idx = m_wndList1.GetItemData(i);
		if( m_wndList1.GetCheck(idx) )
		{
			bit.SetBit(m_arrItems[idx].data,TRUE);
		}
		else
		{
			bit.SetBit(m_arrItems[idx].data,FALSE);
		}
	}

	bit.SetEnumValue(m_wndList2.GetCurSel());

	m_nSymbolizeFlag = bit;

	CDialog::OnOK();
}


void CDlgSymbolizeFlag::OnCancel()
{
	m_nSymbolizeFlag = m_nSymbolizeFlag_old;
	CDialog::OnCancel();
}


