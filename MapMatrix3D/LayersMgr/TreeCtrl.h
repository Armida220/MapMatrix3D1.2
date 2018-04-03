#if !defined(AFX_MYTREECTRL_H__2EE2C29B_4FAC_4CF0_B7BF_76B2A98D65A5__INCLUDED_)
#define AFX_MYTREECTRL_H__2EE2C29B_4FAC_4CF0_B7BF_76B2A98D65A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLVLTreeCtrl window
#include "stdAfx.h"
#include <afxcoll.h>
#include "TreeCtrlEx.h"

class CLVLTreeCtrl : public CTreeCtrlEx
{
// Construction
public:
	CLVLTreeCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLVLTreeCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLVLTreeCtrl();

	// Generated message map functions
public:
	//{{AFX_MSG(CLVLTreeCtrl)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) ;
	afx_msg void OnNew();
	afx_msg void OnRename();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTREECTRL_H__2EE2C29B_4FAC_4CF0_B7BF_76B2A98D65A5__INCLUDED_)
