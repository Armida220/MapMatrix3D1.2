#if !defined(AFX_DLGEDITTEXT_H__E64827D7_0906_4524_9A01_4EDC507A7DD7__INCLUDED_)
#define AFX_DLGEDITTEXT_H__E64827D7_0906_4524_9A01_4EDC507A7DD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditText.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgEditText dialog

class CDlgEditText : public CDialog
{
// Construction
public:
	CDlgEditText(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEditText)
	enum { IDD = IDD_EDITTEXT };
	CEdit	m_wndEdit;
	//}}AFX_DATA

	CString m_strEdit;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditText)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEditText)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITTEXT_H__E64827D7_0906_4524_9A01_4EDC507A7DD7__INCLUDED_)
