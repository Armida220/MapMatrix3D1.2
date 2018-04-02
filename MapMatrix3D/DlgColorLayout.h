#if !defined(AFX_DLGCOLORLAYOUT_H__414F58C7_CF28_44B0_9F00_5203C25CE76C__INCLUDED_)
#define AFX_DLGCOLORLAYOUT_H__414F58C7_CF28_44B0_9F00_5203C25CE76C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgColorLayout.h : header file
//
#include "res\resource.h"
#include "ColorCombobox.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgColorLayout dialog

class CDlgColorLayout : public CDialog
{
// Construction
public:
	CDlgColorLayout(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgColorLayout)
	enum { IDD = IDD_COLORLAYOUT };
	CColorComboBox	m_ColorCombobox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgColorLayout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgColorLayout)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOLORLAYOUT_H__414F58C7_CF28_44B0_9F00_5203C25CE76C__INCLUDED_)
