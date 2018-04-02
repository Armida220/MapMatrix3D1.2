#if !defined(AFX_DLGIMPORTXYZ_H__F02D5630_8053_4B2F_A88B_0A46C23C8BEF__INCLUDED_)
#define AFX_DLGIMPORTXYZ_H__F02D5630_8053_4B2F_A88B_0A46C23C8BEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportXYZ.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgImportXYZ dialog

class CDlgImportXYZ : public CDialog
{
	// Construction
public:
	CDlgImportXYZ(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgImportXYZ)
	enum { IDD = IDD_IMPORT_XYZ };
	CString	m_strFilePath;
	CString	m_strMapName;
	//}}AFX_DATA
	
private:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportXYZ)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgImportXYZ)
	afx_msg void OnFileBrowse();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTXYZ_H__F02D5630_8053_4B2F_A88B_0A46C23C8BEF__INCLUDED_)
