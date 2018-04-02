#pragma once


// CMyEditTip

class CMyEditTip : public CEdit
{
	DECLARE_DYNAMIC(CMyEditTip)

public:
	CMyEditTip();
	virtual ~CMyEditTip();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

protected:
	CBrush m_brush;
	
};


