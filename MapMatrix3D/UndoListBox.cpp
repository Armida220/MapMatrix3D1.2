// UndoListBox.cpp : implementation file
//

#include "stdafx.h"
#include "UndoListBox.h"
#include "UndoBar.h"
#include "editbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUndoListBox

//##ModelId=41466B7A00CC
CUndoListBox::CUndoListBox(CUndoBar& bar) :
	m_Bar (bar)
{
}

//##ModelId=41466B7A00DE
CUndoListBox::~CUndoListBox()
{
}


BEGIN_MESSAGE_MAP(CUndoListBox, CListBox)
	//{{AFX_MSG_MAP(CUndoListBox)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUndoListBox message handlers

//##ModelId=41466B7A00EB
void CUndoListBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	BOOL bOutside;
	ItemFromPoint (point, bOutside);

	if (!bOutside)
	{
		m_Bar.DoUndo ();
	}

	if (IsWindow (GetSafeHwnd ()))
	{
		CListBox::OnLButtonUp(nFlags, point);
	}
}

//##ModelId=41466B7A00E0
void CUndoListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	CListBox::OnMouseMove(nFlags, point);

	BOOL bOutside;
	int iItem = ItemFromPoint (point, bOutside);

	if (!bOutside)
	{
		SelItemRange (FALSE, iItem + 1, GetCount () - 1);

		if (iItem == 0)
		{
			SetSel (0);
		}
		else
		{
			SelItemRange (TRUE, 0, iItem);
		}

		CString strLabel;
		strLabel.Format (IDS_UNDO_TIP, iItem+1);

		m_Bar.SetLabel (strLabel);
	}
}
