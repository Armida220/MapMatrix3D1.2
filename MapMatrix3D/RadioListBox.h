#if !defined(AFX_RADIOLISTBOX_H__E853DB3F_FF3C_4DAB_A4AB_1E300E7CC15D__INCLUDED_)
#define AFX_RADIOLISTBOX_H__E853DB3F_FF3C_4DAB_A4AB_1E300E7CC15D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RadioListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRadioListBox window

class CRadioListBox : public CListBox
{
// Construction
public:
	CRadioListBox();

// Attributes
public:
  
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadioListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRadioListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRadioListBox)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIOLISTBOX_H__E853DB3F_FF3C_4DAB_A4AB_1E300E7CC15D__INCLUDED_)
