#if !defined(AFX_DLGCONVERTPHOTO_H__076C2558_CB88_4FBD_8F35_3B7352A2AE3E__INCLUDED_)
#define AFX_DLGCONVERTPHOTO_H__076C2558_CB88_4FBD_8F35_3B7352A2AE3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConvertPhoto.h : header file
//

#include "CoordCenter.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgConvertPhoto dialog

class CDlgConvertPhoto : public CDialog
{
// Construction
public:
	CDlgConvertPhoto(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgConvertPhoto)
	enum { IDD = IDD_CONVERTPHOTO };
	CComboBox	m_wndComboImg;
	CComboBox	m_wndComboDEM;
	CString	m_strPrjFile;
	//}}AFX_DATA

	CGeoBuilderPrj m_prj;
	CString m_strDEM, m_strImg;

	BOOL m_bIsFilePath;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConvertPhoto)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgConvertPhoto)
	afx_msg void OnBrowseProject();
	afx_msg void OnBrowseDEM();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONVERTPHOTO_H__076C2558_CB88_4FBD_8F35_3B7352A2AE3E__INCLUDED_)
