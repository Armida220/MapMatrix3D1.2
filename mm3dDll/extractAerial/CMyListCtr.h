#pragma once
#include <vector>
#include <set>
class CMyListCtrl : public CListCtrl
{
public:
	
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void SetRowHeigt(int nHeight);
	void setColor(COLORREF c)
	{
		color = c;
	}

	std::vector<UINT> indexVector;
	// Operations  
	void refreshIndex();
	int InsertItem(int nItem, LPCTSTR lpszItem);
	BOOL DeleteItem(int nItem);
	BOOL DeleteAllItems();
protected:
	int m_nRowHeight;
	COLORREF color;
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpMeasureItemStruct);
	DECLARE_MESSAGE_MAP()

	std::set<CString> m_itemSet;
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};