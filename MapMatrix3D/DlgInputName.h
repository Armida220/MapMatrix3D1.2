#if !defined(AFX_DLGINPUTNAME_H__35D9580B_9A0A_45E1_91AD_C173F74B0001__INCLUDED_)
#define AFX_DLGINPUTNAME_H__35D9580B_9A0A_45E1_91AD_C173F74B0001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgInputName.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInputName dialog

class CDlgInputName : public CDialog
{
// Construction
public:
	CDlgInputName(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgInputName)
	enum { IDD = IDD_INPUT_NAME };
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInputName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInputName)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINPUTNAME_H__35D9580B_9A0A_45E1_91AD_C173F74B0001__INCLUDED_)
