#if !defined(AFX_DLGIMPORTDGN_H__58A88268_3C65_48DC_AFD6_24E38B68A8D0__INCLUDED_)
#define AFX_DLGIMPORTDGN_H__58A88268_3C65_48DC_AFD6_24E38B68A8D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportDGN.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgImportDGN dialog

class CDlgImportDGN : public CDialog
{
// Construction
public:
	CDlgImportDGN(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgImportDGN)
	enum { IDD = IDD_IMPORT_DGN };
	CString	m_strPath;
	CString	m_strLstFile;
	CString	m_strMapName;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportDGN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImportDGN)
	afx_msg void OnButtonIdBrowse();
	afx_msg void OnButtonEdBrowse2();
	afx_msg void OnKillfocusEditEdFilepath2();
	afx_msg void OnSetfocusEditEdFilepath2();
	virtual void OnOK();
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTDGN_H__58A88268_3C65_48DC_AFD6_24E38B68A8D0__INCLUDED_)
