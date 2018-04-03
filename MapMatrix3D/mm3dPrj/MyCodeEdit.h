#pragma once


// CMyCodeEdit

class CMyCodeEdit : public CEdit
{
	DECLARE_DYNAMIC(CMyCodeEdit)

public:
	CMyCodeEdit();
	virtual ~CMyCodeEdit();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

protected:
	CBrush m_brush;

};


