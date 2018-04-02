#if !defined(AFX_OP_PAGE_QT2_H__AE86C81D_4E1C_4D96_B28D_F494B1CB3723__INCLUDED_)
#define AFX_OP_PAGE_QT2_H__AE86C81D_4E1C_4D96_B28D_F494B1CB3723__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Op_Page_QT2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COp_Page_QT2 dialog

class COp_Page_QT2 : public COp_Page_Base
{
	DECLARE_DYNCREATE(COp_Page_QT2)

// Construction
public:
	COp_Page_QT2();
	~COp_Page_QT2();

// Dialog Data
	//{{AFX_DATA(COp_Page_QT2)
	enum { IDD = IDD_OP_PAGE_QT2 };
	CButton	m_checkBtnPtZ;
	CButton	m_checkBtnPtCrossLay;
	//}}AFX_DATA
	double	m_lfSamePtsInlineToler;
	double	m_lfOverlapLineToler;
	double	m_lfOverlapPntToler;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_QT2)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_QT2)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OP_PAGE_QT2_H__AE86C81D_4E1C_4D96_B28D_F494B1CB3723__INCLUDED_)
