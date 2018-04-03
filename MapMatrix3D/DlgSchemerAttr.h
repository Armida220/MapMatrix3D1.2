#if !defined(AFX_DLGSCHEMERATTR_H__769A1508_7270_43B1_9DDE_33546608A7CF__INCLUDED_)
#define AFX_DLGSCHEMERATTR_H__769A1508_7270_43B1_9DDE_33546608A7CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSchemerAttr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSchemerAttr dialog

class CDlgSchemerAttr : public CDialog
{
// Construction
public:
	CDlgSchemerAttr(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSchemerAttr)
	enum { IDD = IDD_SCHEMERDLG_ATTR_BUTTONS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSchemerAttr)
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSchemerAttr)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCHEMERATTR_H__769A1508_7270_43B1_9DDE_33546608A7CF__INCLUDED_)
