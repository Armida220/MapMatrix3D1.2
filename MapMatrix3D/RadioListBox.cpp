// RadioListBox.cpp : implementation file
//

#include "stdafx.h"
#include "RadioListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadioListBox

CRadioListBox::CRadioListBox()
{
}

CRadioListBox::~CRadioListBox()
{
}

BEGIN_MESSAGE_MAP(CRadioListBox, CListBox)
	//{{AFX_MSG_MAP(CRadioListBox)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioListBox message handlers


void CRadioListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
     CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

     // just draws focus rectangle when listbox is empty
     if (lpDrawItemStruct->itemID == (UINT)-1)
     {
          if (lpDrawItemStruct->itemAction & ODA_FOCUS)
               pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
          return;
     }
     else
     {
          int selChange   = lpDrawItemStruct->itemAction & ODA_SELECT;
          int focusChange = lpDrawItemStruct->itemAction & ODA_FOCUS;
          int drawEntire  = lpDrawItemStruct->itemAction & ODA_DRAWENTIRE;

          if (selChange || drawEntire) {
               BOOL sel = lpDrawItemStruct->itemState & ODS_SELECTED;

               // Draws background rectangle
			   pDC->FillSolidRect(&lpDrawItemStruct->rcItem, 
				   ::GetSysColor((GetExStyle()&WS_EX_TRANSPARENT)?COLOR_BTNFACE:COLOR_WINDOW));

               // Draw radio button
			   int h = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;
               CRect rect(lpDrawItemStruct->rcItem.left+2, lpDrawItemStruct->rcItem.top+2, 
				   lpDrawItemStruct->rcItem.left+h-3, lpDrawItemStruct->rcItem.top+h-3);
			   pDC->DrawFrameControl(&rect, DFC_BUTTON, DFCS_BUTTONRADIO | (sel?DFCS_CHECKED:0));

			   CString text;
			   GetText(lpDrawItemStruct->itemID,text);

               // Draws item text
               pDC->SetTextColor(COLOR_WINDOWTEXT);
               pDC->SetBkMode(TRANSPARENT);
               lpDrawItemStruct->rcItem.left += h;
               pDC->DrawText((LPCTSTR)text, &lpDrawItemStruct->rcItem, DT_LEFT);
          }
		  // draws focus rectangle
          if (focusChange || (drawEntire && (lpDrawItemStruct->itemState & ODS_FOCUS)))
               pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
     }
}

HBRUSH CRadioListBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
    // If transparent style selected...
	if ( (GetExStyle()&WS_EX_TRANSPARENT) && nCtlColor==CTLCOLOR_LISTBOX)
		return (HBRUSH)::GetSysColorBrush(COLOR_BTNFACE);

	return NULL;
}
