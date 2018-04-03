#if !defined(AFX_DLGINPUTCODE_H__8EAF2028_C26C_4229_8F44_F11BA27E5D48__INCLUDED_)
#define AFX_DLGINPUTCODE_H__8EAF2028_C26C_4229_8F44_F11BA27E5D48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgInputCode.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInputCode dialog

class CDlgInputCode : public CDialog
{
// Construction
public:
	CDlgInputCode(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgInputCode)
	enum { IDD = IDD_INPUTCODE };
	CString	m_strCode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInputCode)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInputCode)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINPUTCODE_H__8EAF2028_C26C_4229_8F44_F11BA27E5D48__INCLUDED_)
