// MyTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TreeCtrl.h"
#include "Resource.h"
#include "LayersMgrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLVLTreeCtrl

CLVLTreeCtrl::CLVLTreeCtrl()
{
}

CLVLTreeCtrl::~CLVLTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CLVLTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CLVLTreeCtrl)
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLVLTreeCtrl message handlers

void CLVLTreeCtrl::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	// Create the pop up menu
	CMenu obMenu;
	obMenu.LoadMenu(IDR_TREE_POPUP); 

	CMenu* pPopupMenu = obMenu.GetSubMenu(0);
	ASSERT(pPopupMenu); 

	HTREEITEM hItem = GetSelectedItem();
	if(!hItem)
		return;

}

BOOL CLVLTreeCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	cs.style |= TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES |TVS_EDITLABELS |TVS_SHOWSELALWAYS;  
	return CTreeCtrl::PreCreateWindow(cs);
}

//新建Filter
void CLVLTreeCtrl::OnNew() 
{
	// TODO: Add your command handler code here
	HTREEITEM hItem = GetSelectedItem();
	if(hItem != NULL)
	{
	}
}

void CLVLTreeCtrl::OnRename() 
{
}

void CLVLTreeCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	*pResult = -1;

	HTREEITEM hItem = GetSelectedItem();
	if(hItem != NULL)
	{
	}	
	
	*pResult = 0;
}

void CLVLTreeCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(pTVDispInfo->item.pszText != NULL)
	{
		CString	strFilterName(pTVDispInfo->item.pszText);


	}
	*pResult = 0;
}

void CLVLTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	UINT info;
	HTREEITEM hItem = HitTest(point,&info);

	if((info & TVHT_ONITEMLABEL) ||
		(info & TVHT_ONITEMICON) ||
		(info & TVHT_ONITEMSTATEICON))
	{
		SelectItem(hItem);
		CTreeCtrl::OnRButtonDown(nFlags,point);
	}

}


int CLVLTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CLVLTreeCtrl::OnDestroy() 
{
	CTreeCtrl::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

BOOL CLVLTreeCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		//快捷键F2重命名
		if (pMsg->wParam == VK_F2)
			EditLabel(GetSelectedItem());
	}

	return CTreeCtrl::PreTranslateMessage(pMsg);
}
