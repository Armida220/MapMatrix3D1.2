#include "stdafx.h"
#include "MyListBox.h"


CMyListBox::CMyListBox()
{
}


CMyListBox::~CMyListBox()
{
}
BEGIN_MESSAGE_MAP(CMyListBox, CListBox)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
END_MESSAGE_MAP()


int CMyListBox::AppendString(LPCSTR strText, COLORREF fgColor, COLORREF bgColor)
{
	LISTBOX_COLOR_* pInfo = new LISTBOX_COLOR_;

	pInfo->strText = strText;
	pInfo->fgColor = fgColor;
	pInfo->bgColor = bgColor;

	int nindex = AddString(strText);
	SetItemDataPtr(nindex, pInfo);
	

	return nindex;
}

CString CMyListBox::getString(int idx)
{
	LISTBOX_COLOR_* dataPtr = (LISTBOX_COLOR_*) GetItemDataPtr(idx);
	return dataPtr->strText;
}

void CMyListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);

	LISTBOX_COLOR_* pListBox = (LISTBOX_COLOR_*)GetItemDataPtr(lpDrawItemStruct->itemID);
	ASSERT(NULL != pListBox);

	CDC dc;

	dc.Attach(lpDrawItemStruct->hDC);

	// Save these value to restore them when done drawing.
	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();

	// If this item is selected, set the background color 
	// and the text color to appropriate values. Also, erase
	// rect by filling it with the background color.
	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
		(lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		dc.SetTextColor(pListBox->bgColor);
		dc.SetBkColor(pListBox->fgColor);
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, pListBox->fgColor);
	}
	else
	{
		dc.SetTextColor(pListBox->fgColor);
		dc.SetBkColor(pListBox->bgColor);
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, pListBox->bgColor);
	}

	lpDrawItemStruct->rcItem.left += 5;
	// Draw the text.

	dc.DrawText(pListBox->strText, pListBox->strText.GetLength(), &lpDrawItemStruct->rcItem, DT_WORDBREAK);

	// Reset the background color and the text color back to their
	// original values.
	dc.SetTextColor(crOldTextColor);
	dc.SetBkColor(crOldBkColor);

	dc.Detach();
}

void CMyListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	ASSERT(lpMeasureItemStruct->CtlType == ODT_LISTBOX);

	CString strText(_T(""));
	GetText(lpMeasureItemStruct->itemID, strText);
	ASSERT(TRUE != strText.IsEmpty());

	CRect rect;
	GetItemRect(lpMeasureItemStruct->itemID, &rect);

	CDC* pDC = GetDC();
	lpMeasureItemStruct->itemHeight = pDC->DrawText(strText, -1, rect, DT_WORDBREAK | DT_CALCRECT);
	ReleaseDC(pDC);
}
