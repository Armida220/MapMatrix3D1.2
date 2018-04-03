#pragma once
#include "afxwin.h"
#include <map>
typedef struct _LISTBOX_COLOR_1
{
	CString strText;
	COLORREF fgColor;
	COLORREF bgColor;
	_LISTBOX_COLOR_1()
	{
		strText.Empty();
		fgColor = RGB(0, 0, 0);
		bgColor = RGB(255, 255, 255);
	}
}LISTBOX_COLOR_, *PLISTBOX_COLOR_;

class CMyListBox : public CListBox
{
public:
	CMyListBox();
	~CMyListBox();

	int AppendString(LPCSTR strText, COLORREF fgColor, COLORREF bgColor);

	CString getString(int idx);

protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

protected:
	CBrush m_brush;

	std::map<int, CString> m_mapText;
};

