// GridCellNumeric.cpp: implementation of the CGridCellNumeric class.
//
// Written by Andrew Truckle [ajtruckle@wsatkins.co.uk]
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCellNumeric.h"
#include "../GridCtrl_src/inpedit.h"
#include "../GridCtrl_src/GridCtrl.h"

IMPLEMENT_DYNCREATE(CGridCellNumeric, CGridCell)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Create a control to do the editing
BOOL CGridCellNumeric::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
    m_bEditing = TRUE;
    
    // CInPEdit auto-deletes itself
    m_pEditWnd = new CInPEdit(GetGrid(), rect, /*GetStyle() |*/ ES_NUMBER, nID, nRow, nCol,
		GetText(), nChar);

    return TRUE;
}

// Cancel the editing.
void CGridCellNumeric::EndEdit()
{
    if (m_pEditWnd)
        ((CInPEdit*)m_pEditWnd)->EndEdit();
}

