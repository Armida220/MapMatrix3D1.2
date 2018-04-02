// dlgxattributes.cpp : implementation file
//

#include "stdafx.h"
#include "..\editbase.h"
#include "dlgxattributes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgXAttributes dialog


CDlgXAttributes::CDlgXAttributes(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgXAttributes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgXAttributes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDlgXAttributes::~CDlgXAttributes()
{
}

void CDlgXAttributes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgXAttributes)
	DDX_Control(pDX, IDC_LIST_XATTRIBUTES, m_ctlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgXAttributes, CDialog)
	//{{AFX_MSG_MAP(CDlgXAttributes)
	ON_BN_CLICKED(IDCANCLE, OnCancle)
	ON_BN_CLICKED(IDC_ALLCHECK, OnAllcheck)
	ON_BN_CLICKED(IDC_ALLNOCHECK, OnAllnocheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgXAttributes message handlers
extern CString XAttriColName(const XAttributeItem &item);


BOOL CDlgXAttributes::OnInitDialog()
{
	CDialog::OnInitDialog();

	ListView_SetExtendedListViewStyle(m_ctlList.m_hWnd, LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES );

	CString test = StrFromResID(IDS_ATTRIBUTES_NAME);
	CRect rect;
	m_ctlList.GetWindowRect(&rect);
	m_ctlList.InsertColumn(0,StrFromResID(IDS_ATTRIBUTES_NAME),LVCFMT_LEFT,rect.Width()-1);

	FillList();

	return TRUE;
}

void CDlgXAttributes::FillList()
{
	POSITION pos = m_arrXAttibutes.GetStartPosition();
	int index = 0;
	while (pos != NULL)
	{
		XAttributeItem xItem;
		BOOL show = FALSE;
		m_arrXAttibutes.GetNextAssoc(pos,xItem,show);
		
		CString name = XAttriColName(xItem);

		m_ctlList.InsertItem(index,name);

		//m_ctlList.SetCheck(index,show);		
		
		index++;
	}
}

void CDlgXAttributes::XAttributes(CMap<XAttributeItem,XAttributeItem&,int,int> &xattibutes, AttributeMode mode)
{
	if (mode == waySet)
	{
		m_arrXAttibutes.RemoveAll();
		m_arrXAttibutes.InitHashTable(97);
		POSITION pos = xattibutes.GetStartPosition();
		while (pos != NULL)
		{
			XAttributeItem xItem;
			BOOL show = FALSE;
			xattibutes.GetNextAssoc(pos,xItem,show);
			
			m_arrXAttibutes.SetAt(xItem,show);
		}
	}
	else
	{
		xattibutes.RemoveAll();
		xattibutes.InitHashTable(97);
		POSITION pos = m_arrXAttibutes.GetStartPosition();
		while (pos != NULL)
		{
			XAttributeItem xItem;
			BOOL show = FALSE;
			m_arrXAttibutes.GetNextAssoc(pos,xItem,show);
			
			xattibutes.SetAt(xItem,show);
		}

	}
	
}

void CDlgXAttributes::OnOK() 
{
	// TODO: Add extra validation here
	CArray<XAttributeItem*,XAttributeItem*> arr;
	POSITION pos = m_arrXAttibutes.GetStartPosition();
	
	int index = 0;
	while (pos != NULL)
	{		
		XAttributeItem xItem;
		BOOL show = FALSE;
		m_arrXAttibutes.GetNextAssoc(pos,xItem,show);

		BOOL res = m_ctlList.GetCheck(index);

		m_arrXAttibutes.SetAt(xItem,res);

		++index;
	}

	CDialog::OnOK();
}

void CDlgXAttributes::OnCancle() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void CDlgXAttributes::OnAllcheck() 
{
	int	iCount = m_ctlList.GetItemCount();
	
	for (int i = 0; i < iCount; i++)
		m_ctlList.SetCheck(i,TRUE);
	
}

void CDlgXAttributes::OnAllnocheck() 
{
	int	iCount = m_ctlList.GetItemCount();
	
	for (int i = 0; i < iCount; i++)
		m_ctlList.SetCheck(i,FALSE);
}

