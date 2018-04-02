// DlgInputCommand.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "dlginputcommand.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInputCommand dialog


CDlgInputCommand::CDlgInputCommand(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputCommand::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInputCommand)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pProcessEventWnd = NULL;
}


void CDlgInputCommand::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInputCommand)
	DDX_Control(pDX, IDC_LIST_COMMAND, m_ctlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInputCommand, CDialog)
	//{{AFX_MSG_MAP(CDlgInputCommand)
	ON_LBN_SELCHANGE(IDC_LIST_COMMAND, OnSelchangeListCommand)
	ON_WM_SIZE()
	ON_WM_NCACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInputCommand message handlers

void CDlgInputCommand::OnSelchangeListCommand() 
{
	// TODO: Add your control notification handler code here
	int idx = m_ctlList.GetCurSel();
	if (idx >= 0 && idx < m_ctlList.GetCount())
	{
		DWORD_PTR commandID = GetItemData(idx);

		if (m_pProcessEventWnd)
		{
			m_pProcessEventWnd->SendMessage(WM_COMMANDSELCHANGE,commandID,0);
		}
	}	
}

int CDlgInputCommand::AddString(LPCTSTR lpszItem)
{
	return m_ctlList.AddString(lpszItem);
}

void CDlgInputCommand::ResetContent()
{
	m_ctlList.ResetContent();
}

int CDlgInputCommand::GetCount() const
{
	return m_ctlList.GetCount();
}

int CDlgInputCommand::GetCurSel() const
{
	return m_ctlList.GetCurSel();
}

int CDlgInputCommand::SetCurSel(int nSelect)
{
	return m_ctlList.SetCurSel(nSelect);
}

DWORD_PTR CDlgInputCommand::GetItemData(int nIndex) const
{
	return m_ctlList.GetItemData(nIndex);
}

int CDlgInputCommand::SetItemData(int nIndex, DWORD_PTR dwItemData)
{
	return m_ctlList.SetItemData(nIndex,dwItemData);
}

BOOL CDlgInputCommand::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message==WM_KEYDOWN )
	{
		if( pMsg->wParam==VK_RETURN )
		{
			int idx = m_ctlList.GetCurSel();
			if (idx >= 0 && idx < m_ctlList.GetCount())
			{
				DWORD_PTR commandID = GetItemData(idx);
				
				if (m_pProcessEventWnd)
				{
					m_pProcessEventWnd->SendMessage(WM_COMMANDSELCHANGE,commandID,0);
				}
			}
		}
		
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgInputCommand::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy);

	CRect rect;
	GetClientRect(rect);

	m_ctlList.MoveWindow(rect);
}


BOOL CDlgInputCommand::OnNcActivate( BOOL bActive )
{
	if (!bActive)
	{
		PostMessage(WM_CLOSE, NULL, NULL);
	}
	return CDialog::OnNcActivate (bActive);
}

void CDlgInputCommand::AdjustSize()
{
	CRect rect;
	GetWindowRect(rect);

	int nItem = m_ctlList.GetCount();
	int height = nItem*m_ctlList.GetItemHeight(0)+4;
	if( height<=20 )height = 20;
	if( height>400 )height = 400;

	rect.top = rect.bottom - height;
	rect.right = 700;
	MoveWindow(rect);
}