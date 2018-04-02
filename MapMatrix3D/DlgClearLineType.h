#if !defined(AFX_DLGCLEARLINETYPE_H__91F63AC0_E716_4B06_B301_66A4009144A4__INCLUDED_)
#define AFX_DLGCLEARLINETYPE_H__91F63AC0_E716_4B06_B301_66A4009144A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgClearLineType.h : header file
//
//hcw,2012.2.22,for clearing the invalid LineType
/////////////////////////////////////////////////////////////////////////////
// CDlgClearLineType dialog
#include "SymbolLib.h"
class CDlgClearLineType : public CDialog
{
// Construction
public:
	CDlgClearLineType(CWnd* pParent = NULL);   // standard constructor
	ConfigLibItem m_config; 
// Dialog Data
	//{{AFX_DATA(CDlgClearLineType)
	enum { IDD = IDD_CLEAR_LINETYPE };
	
	BOOL	m_bOverZero;
	
	//}}AFX_DATA
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgClearLineType)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgClearLineType)
	afx_msg void OnButtonPreview();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCLEARLINETYPE_H__91F63AC0_E716_4B06_B301_66A4009144A4__INCLUDED_)
