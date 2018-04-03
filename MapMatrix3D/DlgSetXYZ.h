#if !defined(AFX_DLGSETXYZ_H__A4F7F7D0_E74A_46C9_95E3_1FA2F8498AA7__INCLUDED_)
#define AFX_DLGSETXYZ_H__A4F7F7D0_E74A_46C9_95E3_1FA2F8498AA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetXYZ.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetXYZ dialog

class CDlgSetXYZ : public CDialog
{
	// Construction
public:
	CDlgSetXYZ(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgSetXYZ)
	enum { IDD = IDD_SET_XYZ };
	double	m_lfX;
	double	m_lfY;
	double	m_lfZ;
	BOOL	m_bSetX;
	BOOL	m_bSetY;
	BOOL	m_bSetZ;
	BOOL	m_bPlacePt;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetXYZ)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgSetXYZ)
	afx_msg void OnChangeSxyzEditX();
	afx_msg void OnChangeSxyzEditY();
	afx_msg void OnChangeSxyzEditZ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CDlgSetXYZ1 : public CDialog
{
	// Construction
public:
	CDlgSetXYZ1(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgSetXYZ1)
	enum { IDD = IDD_SET_XYZ1 };
	double	m_lfX;
	double	m_lfY;
	double	m_lfZ;
	BOOL	m_bSetX;
	BOOL	m_bSetY;
	BOOL	m_bSetZ;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetXYZ1)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgSetXYZ1)
	afx_msg void OnChangeSxyzEditX();
	afx_msg void OnChangeSxyzEditY();
	afx_msg void OnChangeSxyzEditZ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETXYZ_H__A4F7F7D0_E74A_46C9_95E3_1FA2F8498AA7__INCLUDED_)
