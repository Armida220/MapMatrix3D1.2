#if !defined(AFX_DLGWORKSPACEBOUND_H__D7A3CE6E_3E86_4D6B_9208_830B5B1BBA16__INCLUDED_)
#define AFX_DLGWORKSPACEBOUND_H__D7A3CE6E_3E86_4D6B_9208_830B5B1BBA16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWorkSpaceBound.h : header file
//

#define UTM_a

#include "tm.h"

#undef UTM_a

/////////////////////////////////////////////////////////////////////////////
// CDlgWorkSpaceBound dialog

class CDlgWorkSpaceBound : public CDialog
{
// Construction
public:
	virtual void OnOK();
	CDlgWorkSpaceBound(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWorkSpaceBound)
	enum { IDD = IDD_WORKSPACE_SETBOUND };
	double	m_lfX1;
	double	m_lfX2;
	double	m_lfX3;
	double	m_lfX4;
	double	m_lfY1;
	double	m_lfY2;
	double	m_lfY3;
	double	m_lfY4;
	int		m_nScale;
	double	m_lfZmax;
	double	m_lfZmin;
	//}}AFX_DATA

	CString m_strName;

	BOOL	m_bTmValid;
	CTM		m_tm;

	BOOL m_bUVS;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWorkSpaceBound)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	
	// Implementation
	protected:
		
		// Generated message map functions
		//{{AFX_MSG(CDlgWorkSpaceBound)
		afx_msg void OnInsertWorkspace();
		afx_msg void OnAutoalign();
		afx_msg void OnExtendRange();
		afx_msg void OnCalcfromname();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	private:
		double GetEditData(UINT id);
};


/////////////////////////////////////////////////////////////////////////////
// CDlgExtendWid dialog

class CDlgExtendWid : public CDialog
{
	// Construction
public:
	CDlgExtendWid(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgExtendWid)
	enum { IDD = IDD_EXTENDSETTINGS };
	BOOL	m_bBottom;
	BOOL	m_bLeft;
	BOOL	m_bRight;
	BOOL	m_bTop;
	float	m_fWidth;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExtendWid)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgExtendWid)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWORKSPACEBOUND_H__D7A3CE6E_3E86_4D6B_9208_830B5B1BBA16__INCLUDED_)
