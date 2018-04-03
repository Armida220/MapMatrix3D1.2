#if !defined(AFX_DLGCHECKSUSPENDPOINTS_H__F0E36F30_7FFF_4D0E_9B02_820F578B8395__INCLUDED_)
#define AFX_DLGCHECKSUSPENDPOINTS_H__F0E36F30_7FFF_4D0E_9B02_820F578B8395__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCheckSuspendPoints.h : header file
//

class CDlgDataSource;

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckSuspendPoints dialog

class CDlgCheckSuspendPoints : public CDialog
{
// Construction
public:
	CDlgCheckSuspendPoints(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCheckSuspendPoints)
	enum { IDD = IDD_CHECK_SUSPENDPOINTS };
	BOOL	m_bCrossLay;
	BOOL	m_bCheckZ;
	BOOL	m_bCheckVEP;
	BOOL    m_bCheckBound;
	BOOL    m_bCheckRange;
	CString	m_strLayNames;
	CString m_strBoundLayer;//不参与检查的图廓层
	float	m_fToler;
	double  m_lfRange;
	//}}AFX_DATA

	CDlgDataSource *m_pDS;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCheckSuspendPoints)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CDlgCheckSuspendPoints)
	afx_msg void OnButtonBrowse();
	afx_msg void OnButtonBrowse1();
	afx_msg void OnBoundCheck();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CDlgCheckPseudoPoints dialog

class CDlgCheckPseudoPoints : public CDialog
{
	// Construction
public:
	CDlgCheckPseudoPoints(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgCheckPseudoPoints)
	enum { IDD = IDD_CHECK_PSEUDOPOINTS };
	BOOL	m_bCrossLay;
	BOOL	m_bCheckZ;
	BOOL	m_bCheckVEP;
	CString	m_strLayNames;
	float	m_fToler;
	//}}AFX_DATA
	
	CDlgDataSource *m_pDS;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCheckPseudoPoints)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	virtual BOOL OnInitDialog();
	
	// Generated message map functions
	//{{AFX_MSG(CDlgCheckPseudoPoints)
	afx_msg void OnButtonBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CDlgProcessSuspendPoints dialog

class CDlgProcessSuspendPoints : public CDialog
{
	// Construction
public:
	CDlgProcessSuspendPoints(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgCheckSuspendPoints)
	enum { IDD = IDD_PROCESS_SUSPENDPOINTS };
	float	m_fToler;
	float	m_fTolerZ;
	BOOL    m_bAllowVSuspend;
	//}}AFX_DATA

	BOOL	m_bInsertPoint;
		
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCheckSuspendPoints)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	virtual BOOL OnInitDialog();
	
	// Generated message map functions
	//{{AFX_MSG(CDlgCheckSuspendPoints)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CDlgProcessMatchEndPoints dialog

class CDlgProcessMatchEndPoints : public CDialog
{
	// Construction
public:
	CDlgProcessMatchEndPoints(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgProcessMatchEndPoints)
	enum { IDD = IDD_PROCESS_MATCHENDPOINTS };
	BOOL	m_bCrossLay;
	BOOL	m_bCheckZ;
	CString	m_strLayNames;
	float	m_fTolerXY;
	float	m_fTolerZ;
	//}}AFX_DATA
	
	CDlgDataSource *m_pDS;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProcessMatchEndPoints)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	virtual BOOL OnInitDialog();
	
	// Generated message map functions
	//{{AFX_MSG(CDlgProcessMatchEndPoints)
	afx_msg void OnButtonBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHECKSUSPENDPOINTS_H__F0E36F30_7FFF_4D0E_9B02_820F578B8395__INCLUDED_)
