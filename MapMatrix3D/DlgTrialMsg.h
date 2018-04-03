#if !defined(AFX_DLGTRIALMSG_H__A0A3332A_8F3C_4C25_86FB_CE4D754F5D29__INCLUDED_)
#define AFX_DLGTRIALMSG_H__A0A3332A_8F3C_4C25_86FB_CE4D754F5D29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTrialMsg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTrialMsg dialog

class CDlgTrialMsg : public CDialog
{
// Construction
public:
	CDlgTrialMsg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTrialMsg)
	enum { IDD = IDD_TRIALMSG };
	CButton	m_wndOKBtn;
	//}}AFX_DATA

	DWORD m_time0;
	CString m_strBtnText0;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTrialMsg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTrialMsg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTRIALMSG_H__A0A3332A_8F3C_4C25_86FB_CE4D754F5D29__INCLUDED_)
