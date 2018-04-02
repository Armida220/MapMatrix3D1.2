#pragma once


// CMyListCtrl

class CMyListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CMyListCtrl)

public:
	CMyListCtrl();
	virtual ~CMyListCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

protected:
	virtual void DrawItem(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	CBrush m_brush;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


