#if !defined(AFX_OP_PAGE_QT3_H__3E2C1E3E_EF8B_4891_AC51_EBEC2BEBF6E8__INCLUDED_)
#define AFX_OP_PAGE_QT3_H__3E2C1E3E_EF8B_4891_AC51_EBEC2BEBF6E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Op_Page_QT3.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COp_Page_QT3 dialog

class COp_Page_QT3 : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_QT3)
// Construction
public:
	COp_Page_QT3();   // standard constructor

// Dialog Data
	//{{AFX_DATA(COp_Page_QT3)
	enum { IDD = IDD_OP_PAGE_QT3 };
	CButton	m_checkBtnSusPtVEP;
	CButton	m_checkBtnSusZ;
	CButton	m_checkBtn2;
	CButton	m_checkBtn;
	double	m_lfSusToler;
	double	m_lfSnapToler;
	double	m_lfPerToler;
	double	m_lfXYMatchToler;
	double	m_lfZMatchToler;
	//}}AFX_DATA

	virtual void OnOK();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_QT3)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COp_Page_QT3)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OP_PAGE_QT3_H__3E2C1E3E_EF8B_4891_AC51_EBEC2BEBF6E8__INCLUDED_)
