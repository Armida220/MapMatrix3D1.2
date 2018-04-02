// ColoredListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ColoredListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColoredListCtrl

CColoredListCtrl::CColoredListCtrl()
{
	m_colHilite = RGB(255,0,0);
}

CColoredListCtrl::~CColoredListCtrl()
{
}


BEGIN_MESSAGE_MAP(CColoredListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CColoredListCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColoredListCtrl message handlers

void CColoredListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	
	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	int iRow = lplvcd->nmcd.dwItemSpec;
	
	switch(lplvcd->nmcd.dwDrawStage)
	{
    case CDDS_PREPAINT :
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		}
		
		// Modify item text and or background
    case CDDS_ITEMPREPAINT:
		{
			lplvcd->clrText = RGB(0,0,0);
			
			for(int i=0; i<m_arrHiliteRows.GetSize(); i++)
			{
				if( iRow==m_arrHiliteRows[i] )
					break;
			}
			
			if(i<m_arrHiliteRows.GetSize())
				lplvcd->clrText = m_colHilite;
			
			// If you want the sub items the same as the item,
			// set *pResult to CDRF_NEWFONT
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			*pResult = CDRF_NEWFONT;
			return;
		}
		
		// Modify sub item text and/or background
    case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:
		{
			//lplvcd->clrTextBk = RGB(255,255,255);
			*pResult = CDRF_DODEFAULT;
			return;
		}
	} 

}

BOOL CColoredListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRect rect;
	CColoredListCtrl::GetClientRect(rect);
	
	
	POINT mypoint;  
	
	CBrush brush0(RGB(255,255,255));
	CBrush brush1(RGB(255,255,255));	
	
	
	int chunk_height=GetCountPerPage();
	pDC->FillRect(&rect,&brush1);
	
	for (int i=0;i<=chunk_height;i++)
	{		
		
		GetItemPosition(i,&mypoint);
		rect.top=mypoint.y ;
		GetItemPosition(i+1,&mypoint);
		rect.bottom =mypoint.y;
		pDC->FillRect(&rect,i %2 ? &brush1 : &brush0);
		
	}
	
	brush0.DeleteObject();
	brush1.DeleteObject();
	
	return FALSE;
}


void CColoredListCtrl::SetHiliteRow(int nRow, BOOL bHilite)
{
	for(int i=0; i<m_arrHiliteRows.GetSize(); i++)
	{
		if( nRow==m_arrHiliteRows[i] )
			break;
	}

	if(i<m_arrHiliteRows.GetSize())
	{
		if(!bHilite)
			m_arrHiliteRows.RemoveAt(i);
	}
	else
	{
		if(bHilite)
			m_arrHiliteRows.Add(nRow);
	}
}