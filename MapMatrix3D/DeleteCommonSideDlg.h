#if !defined(AFX_DELETECOMMONSIDEDLG_H__62685DF6_5452_43C2_8E95_8F1E0F2F265C__INCLUDED_)
#define AFX_DELETECOMMONSIDEDLG_H__62685DF6_5452_43C2_8E95_8F1E0F2F265C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeleteCommonSideDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeleteCommonSideDlg dialog

class CDeleteCommonSideDlg : public CDialog
{
// Construction
public:
	CDeleteCommonSideDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeleteCommonSideDlg)
	enum { IDD = IDD_DELETE_COMMONSIDE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteCommonSideDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CString m_layname1;
	CString m_layname2;
	int m_side;
	// Generated message map functions
	//{{AFX_MSG(CDeleteCommonSideDlg)
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETECOMMONSIDEDLG_H__62685DF6_5452_43C2_8E95_8F1E0F2F265C__INCLUDED_)
