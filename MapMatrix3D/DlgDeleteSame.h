#if !defined(AFX_DLGDELETESAME_H__8BC1E08E_8E3C_4B6F_A691_4511274566C0__INCLUDED_)
#define AFX_DLGDELETESAME_H__8BC1E08E_8E3C_4B6F_A691_4511274566C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDeleteSame.h : header file
//

class CDlgDataSource;

/////////////////////////////////////////////////////////////////////////////
// DlgDeleteSame dialog

class CDlgDeleteSame : public CDialog
{
// Construction
public:
	CDlgDeleteSame(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DlgDeleteSame)
	enum { IDD = IDD_DEL_SAME_FTR };
	BOOL	m_bCrossLayer;
	CString	m_strLayer;
	BOOL m_bSameShape;
	//}}AFX_DATA

	CDlgDataSource *m_pDS;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgDeleteSame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(DlgDeleteSame)
	virtual void OnOK();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDELETESAME_H__8BC1E08E_8E3C_4B6F_A691_4511274566C0__INCLUDED_)
