#include "stdafx.h"
#include "CMyListCtr.h"
#include <algorithm>
BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMyListCtrl)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

void CMyListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (m_nRowHeight>0)
	{
		lpMeasureItemStruct->itemHeight = m_nRowHeight;
	}
}

void CMyListCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CMyListCtrl::SetRowHeigt(int nHeight)
{
	m_nRowHeight = nHeight;
	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}

void CMyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpMeasureItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpMeasureItemStruct->hDC);    
	LVITEM lvi = {0}; 
	lvi.mask = LVIF_STATE;//|LVIF_IMAGE; 
	lvi.stateMask = LVIS_FOCUSED | LVIS_SELECTED ; 
	lvi.iItem = lpMeasureItemStruct->itemID; 
	BOOL bGet = GetItem(&lvi); 
	//高亮显示
	BOOL bHighlight =((lvi.state & LVIS_DROPHILITED)||((lvi.state & LVIS_SELECTED) && 
		((GetFocus() == this)|| (GetStyle() & LVS_SHOWSELALWAYS)))); 
	// 画文本背景 
	CRect rcBack = lpMeasureItemStruct->rcItem; 
	pDC->SetBkMode(TRANSPARENT); 
	if( bHighlight ) //如果被选中
	{ 
		pDC->SetTextColor(RGB(10,10,255)); //文本为白色
		//pDC->FillRect(rcBack, &CBrush(color)); 
		pDC->FillSolidRect(&rcBack, RGB(255,255,0));
	} 
	else 
	{ 
		pDC->SetTextColor(RGB(255,255,255));      
		pDC->FillRect(rcBack, &CBrush(color)); 
	} 
	if (lpMeasureItemStruct->itemAction & ODA_DRAWENTIRE) 
	{ 
		//写文本 
		CString szText; 
		int nCollumn = GetHeaderCtrl()->GetItemCount();//列数
		for (int i = 0; i < GetHeaderCtrl()->GetItemCount(); i++) 
		{ //循环得到文本 
			CRect rcItem; 
			if ( !GetSubItemRect(lpMeasureItemStruct->itemID, i, LVIR_LABEL, rcItem )) 
				continue; 
			szText = GetItemText( lpMeasureItemStruct->itemID, i ); 
			rcItem.left += 5; rcItem.right -= 1; 
			pDC->DrawText(szText, lstrlen(szText), &rcItem,  DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
		} 
	} 

}

void CMyListCtrl::refreshIndex()
{
	USES_CONVERSION;
	
}


int CMyListCtrl::InsertItem(int nItem, LPCTSTR lpszItem)
{
	if (m_itemSet.find(lpszItem) == m_itemSet.end()){
		m_itemSet.insert(lpszItem);
		return CListCtrl::InsertItem(nItem, lpszItem);
	}
	return -1;
}

BOOL CMyListCtrl::DeleteItem(int nItem)
{
	m_itemSet.emplace(GetItemText(nItem, 0));
	return CListCtrl::DeleteItem(nItem);
}

BOOL CMyListCtrl::DeleteAllItems()
{
	m_itemSet.clear();
	return CListCtrl::DeleteAllItems();
}

void CMyListCtrl::OnDropFiles(HDROP hDropInfo)
{
	
	int DropCount = DragQueryFile(hDropInfo, -1, NULL, 0);
	for (int i = 0; i < DropCount; i++)
	{
		TCHAR wcStr[MAX_PATH];
		DragQueryFile(hDropInfo, i, wcStr, MAX_PATH);
		CString path = wcStr;
		
		CString ext = path.Right(path.GetLength() - path.ReverseFind('.'));
		if (ext.CompareNoCase(".jpg") == 0 || ext.CompareNoCase(".jpeg") == 0){
			InsertItem(0, path);
			//SetItemText(0, 0, path);
		}
	}
	DragFinish(hDropInfo);  //拖放结束后,释放内存  
	CListCtrl::OnDropFiles(hDropInfo);
}
