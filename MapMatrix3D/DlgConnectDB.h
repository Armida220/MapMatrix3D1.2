#if !defined(AFX_DIALGDBCONNECT_H__DD2680BD_2620_4A8C_BC2B_F1F4C7BE74D4__INCLUDED_)
#define AFX_DIALGDBCONNECT_H__DD2680BD_2620_4A8C_BC2B_F1F4C7BE74D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialgDBConnect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DlgOpenWP dialog

namespace uvsclient
{
	struct IUVSClient;
}

class CDlgOpenWP : public CDialog
{
	// Construction
public:
	CDlgOpenWP(CWnd* pParent = NULL);   // standard constructor
	// Dialog Data
	//{{AFX_DATA(DlgOpenWP)
	enum { IDD = IDD_DBCONNECT1 };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	CString m_username;
	CString m_password;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DialgDBConnect)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(DialgDBConnect)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// DialgDBConnect dialog

class CDlgConnectDB : public CDialog
{
// Construction
public:
   CDlgConnectDB(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(DialgDBConnect)
	enum { IDD = IDD_DBCONNECT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	BOOL ConnectDB();
	BOOL OpenWorkSpace();

	CString m_current_wp;//当前工作区名称

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DialgDBConnect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(DialgDBConnect)
	afx_msg void OnDblclkWorkspace();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	uvsclient::IUVSClient * puvs;
	CString identity_id,identity_id2;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALGDBCONNECT_H__DD2680BD_2620_4A8C_BC2B_F1F4C7BE74D4__INCLUDED_)
