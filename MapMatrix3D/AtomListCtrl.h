#if !defined(AFX_ATOMLISTCTRL_H__74BBEC92_6E9E_464B_97F7_CB339A18352D__INCLUDED_)
#define AFX_ATOMLISTCTRL_H__74BBEC92_6E9E_464B_97F7_CB339A18352D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AtomListCtrl.h : header file
//

#include "Symbol.h"

/////////////////////////////////////////////////////////////////////////////
// CAtomListCtrl window

class CAtomListCtrl : public CListCtrl
{
// Construction
public:
	CAtomListCtrl();

// Attributes
public:
	COLORREF		m_crBtnFace;
	COLORREF		m_crHighLight;
	COLORREF		m_crHighLightText;
	COLORREF		m_crWindow;
	COLORREF		m_crWindowText;
	COLORREF		m_crGrayText;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAtomListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CAtomListCtrl();

	// Generated message map functions
protected:
	void DrawCheckbox(int nItem, int nSubItem, CDC *pDC, CRect& rect );
	void GetDrawColors(int nItem, int nSubItem, COLORREF& colorText, COLORREF& colorBkgnd);
	//{{AFX_MSG(CAtomListCtrl)
//	afx_msg BOOL OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATOMLISTCTRL_H__74BBEC92_6E9E_464B_97F7_CB339A18352D__INCLUDED_)
