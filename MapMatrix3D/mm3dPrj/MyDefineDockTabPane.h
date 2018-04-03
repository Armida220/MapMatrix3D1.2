#pragma once


// CMyDefineDockTabPane

/*
** brief 
*/
class CMyDefineDockTabPane : public CTabbedPane
{
	DECLARE_DYNAMIC(CMyDefineDockTabPane)

public:
	CMyDefineDockTabPane();
	virtual ~CMyDefineDockTabPane();

	virtual void GetTabArea(CRect& rectTabAreaTop, CRect& rectTabAreaBottom) const {};

	void setNoClose();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	//virtual BOOL CanBeClosed() const { return FALSE; }

	afx_msg void OnPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
};


