#if !defined(AFX_DLGINPUTSTR_H__C922715F_2B59_4706_9CB2_2D7A80449420__INCLUDED_)
#define AFX_DLGINPUTSTR_H__C922715F_2B59_4706_9CB2_2D7A80449420__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlginputstr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInputStr dialog

class CDlgInputStr : public CDialog
{
// Construction
public:
	CDlgInputStr(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgInputStr)
	enum { IDD = IDD_INPUT_STR };
	CString	m_strInput;
	//}}AFX_DATA
	void SetTitle(LPCTSTR title){
		m_strTitle = title;
	}
	CString m_strTitle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInputStr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInputStr)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINPUTSTR_H__C922715F_2B59_4706_9CB2_2D7A80449420__INCLUDED_)
