// DlgChkPtsList.cpp : implementation file
//

#include "stdafx.h"
//#include "featurecollector2009.h"
#include "DlgChkPtsList.h"
#include "DlgSearchCheckPt.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//Author:hcw,2013.1.14,
/////////////////////////////////////////////////////////////////////////////
// CDlgChkPtsList dialog


CDlgChkPtsList::CDlgChkPtsList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChkPtsList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChkPtsList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bDbClickListItem = FALSE;
}


void CDlgChkPtsList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChkPtsList)
	DDX_Control(pDX, IDC_LIST_CHKPTS, m_listChkPts);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChkPtsList, CDialog)
	//{{AFX_MSG_MAP(CDlgChkPtsList)
	ON_LBN_DBLCLK(IDC_LIST_CHKPTS, OnDblclkListChkpts)
	ON_WM_CANCELMODE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChkPtsList message handlers

void CDlgChkPtsList::OnDblclkListChkpts() 
{
	// TODO: Add your control notification handler code here
	CString str;
	str = GetSelectedStr();
	::SendMessage(g_hwnd,WM_SETTEXT,0,(LPARAM)str.GetBuffer(0));
	
	CWnd *pWnd = FromHandle(g_hwnd);	
	if (!pWnd)
	{
		return;
	}
   if (pWnd->GetParent()->IsKindOf(RUNTIME_CLASS(CSearchCheckPtDlg)))
   {
	   
	   ((CSearchCheckPtDlg*)pWnd->GetParent())->m_bPoppedoutListSearchResult = FALSE;
	   ::SendMessage(pWnd->GetParent()->GetSafeHwnd(),WM_MYDBLCLK,0,0);
   }
	return;
}

void CDlgChkPtsList::AddtoList( CStringArray& strArr )
{
	UpdateData(TRUE);
	if (strArr.GetSize()<=0)
	{
		return;
	}
	CString str;
	for (int i=0; i<strArr.GetSize(); i++)
	{
		str = strArr.GetAt(i);
		if(m_listChkPts.FindString(-1,str)==LB_ERR)
		{
			m_listChkPts.InsertString(-1,str);
		}
	}
	UpdateData(FALSE);
	return;
}

CString CDlgChkPtsList::GetSelectedStr()
{
	CString str="";
	int nIndex = m_listChkPts.GetCurSel();
	m_listChkPts.GetText(nIndex, str);
	return str;
}



BOOL CDlgChkPtsList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CDlgChkPtsList::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (m_listChkPts.GetSafeHwnd())
	{
		m_listChkPts.MoveWindow(0,0,cx,cy);
	}
	
}


