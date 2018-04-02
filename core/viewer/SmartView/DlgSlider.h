#if !defined(AFX_DLGSLIDER_H__F9C7C3CD_B74E_4960_B98F_683FEE970AA3__INCLUDED_)
#define AFX_DLGSLIDER_H__F9C7C3CD_B74E_4960_B98F_683FEE970AA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSlider.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgSlider dialog

class CDlgSlider : public CDialog
{
// Construction
public:
	CDlgSlider(CWnd* pParent = NULL);   // standard constructor

	void SetService(HWND hWnd, UINT msgId, WPARAM wParam, LPARAM lParam);
// Dialog Data
	//{{AFX_DATA(CDlgSlider)
	enum { IDD = IDD_DIALOG_SLIDER };
	CSliderCtrl	m_ctrlSlider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSlider)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	MSG  m_updateMsg;
public:
	CArray<double,double> m_arrCanZoomRate;
	int m_nPos;
	CPoint m_ShowPos;
	// Generated message map functions
	//{{AFX_MSG(CDlgSlider)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnNcActivate( BOOL bActive );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSLIDER_H__F9C7C3CD_B74E_4960_B98F_683FEE970AA3__INCLUDED_)
