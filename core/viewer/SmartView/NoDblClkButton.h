// NoDblClkButton.h: interface for the CNoDblClkButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODBLCLKBUTTON_H__6C505A8E_D9A5_45A4_BF03_0AE0D732324A__INCLUDED_)
#define AFX_NODBLCLKBUTTON_H__6C505A8E_D9A5_45A4_BF03_0AE0D732324A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxButton.h"

class CNoDblClkButton : public CMFCButton
{
	DECLARE_DYNCREATE(CNoDblClkButton)
public:
	CNoDblClkButton();
	virtual ~CNoDblClkButton();
protected:
	virtual void OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState);
	virtual void OnDraw(CDC* pDC, const CRect& rect, UINT uiState);
protected:
	//{{AFX_MSG(CMFCButton)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_NODBLCLKBUTTON_H__6C505A8E_D9A5_45A4_BF03_0AE0D732324A__INCLUDED_)
