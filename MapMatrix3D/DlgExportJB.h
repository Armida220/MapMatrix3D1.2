#if !defined(AFX_DLGEXPORTJB_H__D870ABC7_B955_47B6_AC9C_34D23FE75E5E__INCLUDED_)
#define AFX_DLGEXPORTJB_H__D870ABC7_B955_47B6_AC9C_34D23FE75E5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportJB.h : header file
//

#include "valueTable.h"
#include "UIFPropListEx.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgExportJB dialog


class CDlgDoc;

class CDlgExportJB : public CDialog
{
// Construction
public:
	CDlgExportJB(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExportJB)
	enum { IDD = IDD_EXPORT_JB };
	CString	m_strFilePath;
	CString	m_strLstFile;
	//}}AFX_DATA

	CDlgDoc *m_pDoc;

	CValueTable m_dat;
	CUIFPropListEx m_wndPropList;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportJB)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillList();
	void GetValues();
	
	// Generated message map functions
	//{{AFX_MSG(CDlgExportJB)
	afx_msg void OnButtonIcBrowse();
	afx_msg void OnButtonIdBrowse2();
	afx_msg void OnButtonIdBrowse3();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusEditIdFilepath2();
	afx_msg void OnKillfocusEditIdFilepath2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTJB_H__D870ABC7_B955_47B6_AC9C_34D23FE75E5E__INCLUDED_)
