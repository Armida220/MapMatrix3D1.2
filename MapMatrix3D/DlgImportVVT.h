#if !defined(AFX_DLGIMPORTVVT_H__56271F9A_A39A_43F8_B2F2_D2040D956A54__INCLUDED_)
#define AFX_DLGIMPORTVVT_H__56271F9A_A39A_43F8_B2F2_D2040D956A54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportVVT.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgImportVVT dialog

class CDlgImportVVT : public CDialog
{
	// Construction
public:
	CDlgImportVVT(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgImportVVT)
	enum { IDD = IDD_IMPORT_VVT };
	CString	m_strFilePath;
	CString	m_strMapName;
	//}}AFX_DATA
	
private:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportVVT)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgImportVVT)
	afx_msg void OnFileBrowse();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTVVT_H__56271F9A_A39A_43F8_B2F2_D2040D956A54__INCLUDED_)
