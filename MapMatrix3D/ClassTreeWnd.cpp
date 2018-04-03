// ClassTreeWnd.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "ClassTreeWnd.h"
#include "ExMessage.h "
#include "DlgDataSource.h "
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CTreeCtrlEx0, CTreeCtrl)

BEGIN_MESSAGE_MAP(CTreeCtrlEx0, CTreeCtrl)
//{{AFX_MSG_MAP(CTreeCtrlEx0)
ON_WM_KEYDOWN()
ON_WM_CHAR()
ON_WM_GETDLGCODE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTreeCtrlEx0::CTreeCtrlEx0()
{
}

CTreeCtrlEx0::~CTreeCtrlEx0()
{
}

void CTreeCtrlEx0::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar==VK_CONTROL || nChar==VK_SHIFT)
	{
		CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	}
	else
	{
		MSG msg = *GetCurrentMessage();
		AfxGetMainWnd()->SendMessage(FCCM_TRANSLATEMSG,0,(LPARAM)&msg);
	}
}


void CTreeCtrlEx0::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}


UINT CTreeCtrlEx0::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}


BOOL CTreeCtrlEx0::PreTranslateMessage(MSG* pMsg)
{
	return CTreeCtrl::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
//CClassTreeWndLayer
BEGIN_MESSAGE_MAP(CClassTreeWndLayer, CTreeCtrlEx0)
//{{AFX_MSG_MAP(CTreeCtrlEx0)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
END_MESSAGE_MAP()

void CClassTreeWndLayer::ClearSelection()
{
	SelectItem(NULL);
    for (HTREEITEM hItem=GetRootItem();hItem!=NULL;hItem=GetNextVisibleItem(hItem))
		if (GetItemState(hItem,TVIS_SELECTED)&TVIS_SELECTED)
			SetItemState(hItem,0,TVIS_SELECTED);
	//
	m_selitems.RemoveAll();
}

void CClassTreeWndLayer::OnLButtonDown(UINT nFlags, CPoint point)
{	
	UINT uFlags = 0;
	HTREEITEM hTreeItem = HitTest (point,&uFlags);
	m_current_item = hTreeItem;

	//
	if (hTreeItem != NULL)
	{
		if(uFlags == 64)
		{
			if( 0 == ( GetItemState( hTreeItem,TVIS_SELECTED ) & TVIS_SELECTED ) )
			{
				ClearSelection();
				SelectItem(hTreeItem);
				SetItemState( hTreeItem, TVIS_SELECTED,TVIS_SELECTED );
				m_selitems.Add(hTreeItem);
			}
			//
			mpos = MBTNDOWMPOS::CHECKBOX;
			this->GetParent()->SendMessage(TREELBTNDOWN);
			return;
		}
		else if(uFlags & TVHT_ONITEMICON)
		{
			ClearSelection();
			SelectItem(hTreeItem);
			SetItemState( hTreeItem, TVIS_SELECTED,TVIS_SELECTED );
			m_selitems.Add(hTreeItem);
			//在位图上；
			mpos = MBTNDOWMPOS::ICON;
		}
		else if(uFlags & TVHT_ONITEMBUTTON)
		{
			ClearSelection();
			SelectItem(hTreeItem);
			SetItemState( hTreeItem, TVIS_SELECTED,TVIS_SELECTED );
			m_selitems.Add(hTreeItem);
			//在按钮上；
			mpos = MBTNDOWMPOS::BUTTON;
		}
		else if(uFlags & TVHT_ONITEMLABEL)
		{
			//
            if(nFlags & MK_CONTROL)
			{
				if( 0 == ( GetItemState( hTreeItem,TVIS_SELECTED ) & TVIS_SELECTED ) )
				{
					m_selitems.Add(hTreeItem);
					SetItemState( hTreeItem, TVIS_SELECTED, TVIS_SELECTED );
				}
				else
				{
					for(int i=0; i<m_selitems.GetSize(); i++)
					{
						if(m_selitems[i]==hTreeItem)
						{
							m_selitems.RemoveAt(i);
							break;
						}
					}
					SetItemState( hTreeItem, 0, TVIS_SELECTED );
				}
				return;
			}
			else if(nFlags & MK_SHIFT)
			{
				if(m_selitems.GetSize()==0)
				{
					m_selitems.Add(hTreeItem);
					SetItemState( hTreeItem, TVIS_SELECTED,TVIS_SELECTED );
				}
				else
				{
					HTREEITEM m_hFirst = m_selitems[0];
					ClearSelection();
					m_selitems.Add(m_hFirst);
					SetItemState( m_hFirst, TVIS_SELECTED,TVIS_SELECTED );
					//
					CRect rcFrom,rcTo;
					GetItemRect( m_hFirst,&rcFrom,FALSE );
					GetItemRect( hTreeItem,&rcTo,FALSE );
					
					HTREEITEM hItemUp,hItemDown;
					if( rcFrom.top < rcTo.top )
					{
						hItemUp = m_hFirst;
						hItemDown = hTreeItem;
					}
					else
					{
						hItemUp = hTreeItem;
						hItemDown = m_hFirst;
					}
                    //
					HTREEITEM hItemTemp = hItemUp;
					while( hItemTemp )
					{
						if(hItemTemp!=m_hFirst)
						{
							m_selitems.Add(hItemTemp);
					        SetItemState( hItemTemp, TVIS_SELECTED,TVIS_SELECTED );
						}
						if( hItemTemp == hItemDown )break;
						hItemTemp = GetNextVisibleItem( hItemTemp );
					}
				}
				//
				return;
			}
			if( 0 == ( GetItemState( hTreeItem,TVIS_SELECTED ) & TVIS_SELECTED ) )
			{
				ClearSelection();
				SelectItem(hTreeItem);
				SetItemState( hTreeItem, TVIS_SELECTED,TVIS_SELECTED );
				m_selitems.Add(hTreeItem);
			}
			//在标签上；
			mpos = MBTNDOWMPOS::LABEL;
		}
		else
		{
			mpos = MBTNDOWMPOS::NONE;
			ClearSelection();
		}
	}
	else
	{
		ClearSelection();
	}
	
	this->GetParent()->SendMessage(TREELBTNDOWN);
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CClassTreeWndLayer::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	ClearSelection();
	UINT uFlags = 0;
	HTREEITEM hTreeItem = HitTest (point,&uFlags);
	if(hTreeItem && (uFlags & TVHT_ONITEMLABEL) )
	{
		SelectItem(hTreeItem);
		SetItemState( hTreeItem, TVIS_SELECTED,TVIS_SELECTED );
		m_selitems.Add(hTreeItem);
	}
	//
    this->GetParent()->PostMessage(TREELBTNDBCLK);
}

void CClassTreeWndLayer::OnRButtonDown(UINT nFlags, CPoint point)
{
	UINT uFlags = 0;
	HTREEITEM hTreeItem = HitTest (point,&uFlags);
	if( 0 == ( GetItemState( hTreeItem,TVIS_SELECTED ) & TVIS_SELECTED ) )
	{
		ClearSelection();
		SelectItem(hTreeItem);
		SetItemState( hTreeItem, TVIS_SELECTED,TVIS_SELECTED );
		m_selitems.Add(hTreeItem);
	}
	m_current_item = hTreeItem;
	//
	CTreeCtrl::OnRButtonDown(nFlags,point);
}

int CClassTreeWndLayer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrlEx0::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	SetBkColor(RGB(50, 50, 50));
	SetTextColor(RGB(255, 255, 255));
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
CClassTreeWndProject::CClassTreeWndProject()
{
}

CClassTreeWndProject::~CClassTreeWndProject()
{
}


BEGIN_MESSAGE_MAP(CClassTreeWndProject, CTreeCtrlEx0)
	//{{AFX_MSG_MAP(CClassTreeWndProject)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassTreeWndProject message handlers

BOOL CClassTreeWndProject::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	BOOL bRes = CTreeCtrlEx0::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT (pNMHDR != NULL);

	if (pNMHDR->code == TTN_SHOW && GetToolTips () != NULL)
	{
		GetToolTips ()->SetWindowPos (&wndTop, -1, -1, -1, -1,
			SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

void CClassTreeWndProject::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HTREEITEM item = GetSelectedItem();
	DWORD_PTR dwData = GetItemData(item);

	*pResult = 0;
}

CClassTreeWndGeometry::CClassTreeWndGeometry()
{
	m_pData = NULL;
}

CClassTreeWndGeometry::~CClassTreeWndGeometry()
{
}


BEGIN_MESSAGE_MAP(CClassTreeWndGeometry, CTreeCtrlEx0)
	//{{AFX_MSG_MAP(CClassTreeWndGeometry)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassTreeWndGeometry message handlers

BOOL CClassTreeWndGeometry::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	BOOL bRes = CTreeCtrlEx0::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT (pNMHDR != NULL);

	if (pNMHDR->code == TTN_SHOW && GetToolTips () != NULL)
	{
		GetToolTips ()->SetWindowPos (&wndTop, -1, -1, -1, -1,
			SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

void CClassTreeWndGeometry::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HTREEITEM item = GetSelectedItem();
	DWORD_PTR dwData = GetItemData(item);
	CString strText = GetItemText(item);
	
	if (dwData==0x04)
	{ // Feature
		HTREEITEM iParent = GetParentItem(item);
		CString strLayer = GetItemText(iParent);

// 		if(!m_pData->SetCurFtrLayer(strLayer)) return;
// 		CFtrLayer *pCurLayer = m_pData->GetFtrLayer();
// 		if (pCurLayer)
		{
// 			for (int i=0;i<pCurLayer->;i++)
// 			{
// 				FTRCLASS ftrcls = pCurLayer->m_FtrClsList.List.GetAt(i);
// 				CDpDBVariant var;
// 				DpCustomFieldType type;
// 				ftrcls.pGDX->GetAttrValue("FID",var,type);
// 
// 				if (stricmp(var.m_pString,strText)==0x00)
// 				{
// 					pCurLayer->m_FtrClsList.nCur = i;
// 					if (ftrcls.nDefCommandID!=0)
// 					{
// 						// 激发特征缺省命令
// 						AfxGetMainWnd()->SendMessage(WM_COMMAND,ftrcls.nDefCommandID|0x10000);
// 						// 添加到上次最近采集特征列表
// 						m_pData->AddToRecent(-1,var.m_pString);
// 						*pResult = 0;
// 						return;
// 					}
// 				}
// 			}
		}
	}
	else if (dwData==0x01)
	{ // Layer
// 		CGeoLayer *pCurLayer = m_pData->SetCurrentLayer(strText);
// 
// 		// 激发特征缺省命令
// 		FTRCLASS ftrcls = pCurLayer->m_FtrClsList.List.GetAt(pCurLayer->m_FtrClsList.nCur);
// 		if (ftrcls.nDefCommandID!=0) 
// 			AfxGetMainWnd()->SendMessage(WM_COMMAND,ftrcls.nDefCommandID|0x10000);
	}

	*pResult = 0;
}

CClassTreeWndCollection::CClassTreeWndCollection()
{
}

CClassTreeWndCollection::~CClassTreeWndCollection()
{
}


BEGIN_MESSAGE_MAP(CClassTreeWndCollection, CTreeCtrlEx0)
	//{{AFX_MSG_MAP(CClassTreeWndCollection)
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassTreeWndCollection message handlers

BOOL CClassTreeWndCollection::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	BOOL bRes = CTreeCtrlEx0::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1,
			SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

int CClassTreeWndCollection::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrlEx0::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	SetBkColor(RGB(50, 50, 50));
	SetTextColor(RGB(255, 255, 255));
	return 0;
}







CClassTreeWndSymbolLib::CClassTreeWndSymbolLib()
{
}

CClassTreeWndSymbolLib::~CClassTreeWndSymbolLib()
{
}


BEGIN_MESSAGE_MAP(CClassTreeWndSymbolLib, CTreeCtrlEx0)
	//{{AFX_MSG_MAP(CClassTreeWndSymbolLib)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassTreeWndSymbolLib message handlers

BOOL CClassTreeWndSymbolLib::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	BOOL bRes = CTreeCtrlEx0::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT (pNMHDR != NULL);

	if (pNMHDR->code == TTN_SHOW && GetToolTips () != NULL)
	{
		GetToolTips ()->SetWindowPos (&wndTop, -1, -1, -1, -1,
			SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}
	
	return bRes;
}

void CClassTreeWndSymbolLib::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HTREEITEM item = GetSelectedItem();
	DWORD_PTR dwData = GetItemData(item);

	if (dwData==0xFFFFFFFF)
	{ // Sub Root
	}
	else
	{ // Item

	}

	*pResult = 0;
}


