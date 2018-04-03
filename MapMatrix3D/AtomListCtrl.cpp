// AtomListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "AtomListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAtomListCtrl

CAtomListCtrl::CAtomListCtrl()
{
	m_crBtnFace             = ::GetSysColor(COLOR_BTNFACE);

	m_crHighLight           = ::GetSysColor(COLOR_HIGHLIGHT);
	m_crHighLightText       = ::GetSysColor(COLOR_HIGHLIGHTTEXT);

	m_crWindow     = ::GetSysColor(COLOR_WINDOW);
	m_crWindowText = ::GetSysColor(COLOR_WINDOWTEXT);
	
	m_crGrayText      = ::GetSysColor(COLOR_GRAYTEXT);
}

CAtomListCtrl::~CAtomListCtrl()
{
}

BEGIN_MESSAGE_MAP(CAtomListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CAtomListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAtomListCtrl message handlers
void CAtomListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);

	*pResult = CDRF_DODEFAULT;

}

void CAtomListCtrl::GetDrawColors(int nItem,
								  int nSubItem,
							      COLORREF& colorText,
							      COLORREF& colorBkgnd)
{
	DWORD dwStyle    = GetStyle();
	DWORD dwExStyle  = GetExtendedStyle();

	COLORREF crText  = colorText;
	COLORREF crBkgnd = colorBkgnd;

	if (GetItemState(nItem, LVIS_SELECTED))
	{
		if (dwExStyle & LVS_EX_FULLROWSELECT)
		{
			// selected?  if so, draw highlight background
			crText  = m_crHighLightText;
			crBkgnd = m_crHighLight;

			// has focus?  if not, draw gray background
			if (m_hWnd != ::GetFocus())
			{
				if (dwStyle & LVS_SHOWSELALWAYS)
				{
					crText  = m_crWindowText;
					crBkgnd = m_crBtnFace;
				}
				else
				{
					crText  = colorText;
					crBkgnd = colorBkgnd;
				}
			}
		}
		else	// not full row select
		{
			if (nSubItem == 0)
			{
				// selected?  if so, draw highlight background
				crText  = m_crHighLightText;
				crBkgnd = m_crHighLight;

				// has focus?  if not, draw gray background
				if (m_hWnd != ::GetFocus())
				{
					if (dwStyle & LVS_SHOWSELALWAYS)
					{
						crText  = m_crWindowText;
						crBkgnd = m_crBtnFace;
					}
					else
					{
						crText  = colorText;
						crBkgnd = colorBkgnd;
					}
				}
			}
		}
	}

	colorText  = crText;
	colorBkgnd = crBkgnd;
}


void CAtomListCtrl::DrawCheckbox(int nItem,
							  int nSubItem,
							  CDC *pDC,							
							  CRect& rect )
{
	ASSERT(pDC);

	COLORREF crText  = m_crWindowText;
	COLORREF crBkgnd = m_crWindow;

	GetDrawColors(nItem, nSubItem, crText, crBkgnd);

	pDC->FillSolidRect(&rect, crBkgnd);

	CRect chkboxrect;
	chkboxrect = rect;
	
	chkboxrect.top    += 5;
	chkboxrect.bottom -= 6;

	chkboxrect.left += 1;
	chkboxrect.right = chkboxrect.left + chkboxrect.Height();	

	pDC->FillSolidRect(&chkboxrect, m_crWindow);

	// draw border
	pDC->DrawEdge(&chkboxrect, EDGE_SUNKEN, BF_RECT);

	if (GetCheck(nItem) ==  TRUE)
	{
		CPen *pOldPen = NULL;

		CPen graypen(PS_SOLID, 1, m_crGrayText);
		CPen blackpen(PS_SOLID, 1, RGB(255,0,0));

		pOldPen = pDC->SelectObject(&blackpen);
	
		// draw the checkmark
		int x = chkboxrect.left + 9;
		int y = chkboxrect.top  + 4;

		int i;
		for (i = 0; i < 4; i++)
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x, y+3);
			x--;
			y++;
		}
		for (i = 0; i < 3; i++)
		{
			pDC->MoveTo(x, y);
			pDC->LineTo(x, y+3);
			x--;
			y--;
		}

		if (pOldPen)
			pDC->SelectObject(pOldPen);
	}

}
