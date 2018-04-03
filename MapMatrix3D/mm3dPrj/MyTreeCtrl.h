#pragma once


// CMyTreeCtrl

class CMyTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CMyTreeCtrl)

public:
	CMyTreeCtrl();
	virtual ~CMyTreeCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


