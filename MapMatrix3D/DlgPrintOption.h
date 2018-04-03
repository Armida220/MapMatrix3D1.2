#if !defined(AFX_DLGPRINTOPTION_H__7A47E4B5_9F12_4BD3_BEED_C017A0B848EC__INCLUDED_)
#define AFX_DLGPRINTOPTION_H__7A47E4B5_9F12_4BD3_BEED_C017A0B848EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPrintOption.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPrintOption dialog

class CDlgPrintOption : public CDialog
{
// Construction
public:
	CDlgPrintOption(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPrintOption)
	enum { IDD = IDD_PRINT_OPTION };
	int		m_nPrintScope;
	int		m_nPrintScale;
	float   m_fDefaultWid;
	float	m_fMarginLeft;
	float	m_fMarginRight;
	float	m_fMarginTop;
	float	m_fMarginBottom;
	float   m_fExtension;
	//}}AFX_DATA

	BOOL m_bOverImg;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPrintOption)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPrintOption)
	afx_msg void OnRadioWorkspace();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPRINTOPTION_H__7A47E4B5_9F12_4BD3_BEED_C017A0B848EC__INCLUDED_)
