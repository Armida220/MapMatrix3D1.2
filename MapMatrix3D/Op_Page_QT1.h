#if !defined(AFX_OP_PAGE_QT1_H__A63ADBE7_5A3A_4C40_856F_865DB610B8D3__INCLUDED_)
#define AFX_OP_PAGE_QT1_H__A63ADBE7_5A3A_4C40_856F_865DB610B8D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Op_Page_QT1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COp_Page_QT1 dialog

class COp_Page_QT1 : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_QT1)
// Construction
public:
	COp_Page_QT1();   // standard constructor

// Dialog Data
	//{{AFX_DATA(COp_Page_QT1)
	enum { IDD = IDD_OP_PAGE_QT1 };
	CButton	m_checkBtnSusVEP;
	CButton	m_checkBtnPseCrossLay;
	CButton	m_checkBtnPseZ;
	CButton	m_checkBtnSusCrossLay;
	CButton	m_checkBtnSusZ;
	CMFCColorButton	m_colorPsePnt;
	CMFCColorButton	m_colorSusPnt;
	UINT	m_nSusPntRadius;
	UINT	m_nPsePntRadius;
	int		m_nSusMode;
	//}}AFX_DATA
	
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_QT1)
public:
	virtual void OnOK();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_QT1)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OP_PAGE_QT1_H__A63ADBE7_5A3A_4C40_856F_865DB610B8D3__INCLUDED_)
