#if !defined(AFX_DLGBUILDDEM_H__50779442_EDD1_4166_AEEE_2B885653B48F__INCLUDED_)
#define AFX_DLGBUILDDEM_H__50779442_EDD1_4166_AEEE_2B885653B48F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBuildDEM.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBuildDEM dialog

class CDlgBuildDEM : public CDialog
{
	// Construction
public:
	CDlgBuildDEM(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgBuildDEM)
	enum { IDD = IDD_CREATE_DEM };
	CString	m_strDEMPath;
	float	m_fDX;
	int		m_nFormat;
	//}}AFX_DATA
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBuildDEM)
	public:
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	int UpdateDEMExt();
	// Generated message map functions
	//{{AFX_MSG(CDlgBuildDEM)
	afx_msg void OnButtonBrowse();
	virtual void OnOK();
	afx_msg void OnSelchangeComboDemformat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBUILDDEM_H__50779442_EDD1_4166_AEEE_2B885653B48F__INCLUDED_)
