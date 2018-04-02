#pragma once


// CMyMFCButton

class CMyMFCButton : public CMFCButton
{
	DECLARE_DYNAMIC(CMyMFCButton)

public:
	CMyMFCButton();
	virtual ~CMyMFCButton();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

protected:
	CBrush m_brush;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


