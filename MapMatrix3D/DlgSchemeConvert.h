#if !defined(AFX_DLGSCHEMECONVERT_H__FAD74AB4_C75F_461E_A313_C2F5653A44B7__INCLUDED_)
#define AFX_DLGSCHEMECONVERT_H__FAD74AB4_C75F_461E_A313_C2F5653A44B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSchemeConvert.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSchemeConvert dialog

class CDlgSchemeConvert : public CDialog
{
// Construction
public:
	CDlgSchemeConvert(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSchemeConvert)
	enum { IDD = IDD_SCHEME_CONVERT };
	CString	m_strFdbFile;
	CString	m_strSrcConfig;
	CString	m_strDestConfig;
	CString	m_strLayerMapFile;
	//}}AFX_DATA

	CStringArray m_arrFdbStr;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSchemeConvert)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSchemeConvert)
	afx_msg void OnButtonBrowse1();
	afx_msg void OnButtonBrowse2();
	afx_msg void OnButtonBrowse3();
	afx_msg void OnButtonBrowse4();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCHEMECONVERT_H__FAD74AB4_C75F_461E_A313_C2F5653A44B7__INCLUDED_)
