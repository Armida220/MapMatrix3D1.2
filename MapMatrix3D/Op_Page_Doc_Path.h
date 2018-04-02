#if !defined(AFX_OP_PAGE_DOC_PATH_H__1EF54A2F_D9D8_4921_908D_BA1C8F2270A3__INCLUDED_)
#define AFX_OP_PAGE_DOC_PATH_H__1EF54A2F_D9D8_4921_908D_BA1C8F2270A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Op_Page_Doc_Path.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_Path dialog

class COp_Page_Doc_Path : public COp_Page_Base
{
// Construction
public:
	COp_Page_Doc_Path(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COp_Page_Doc_Path)
	enum { IDD = IDD_OP_PAGE_DOC_PATH };
	CString	m_strSymPath;
	BOOL	m_bColorByObj;
	BOOL	m_AutoSet;
	BOOL	m_RoundLine;
	double  m_lfArcToler;
	BOOL	m_bNotDisplayAnnot;
	BOOL	m_bAnnotUpward;
	BOOL	m_bAnnotToText;
	double	m_fDrawScale;
	double  m_fAnnoScale;
	BOOL	m_bTrimSurface;
	double  m_lfMultiPointSize;
	CMFCColorButton m_MultipointColor;
	BOOL	m_bDisplayDemLine;
	BOOL    m_bkeepOneCell;
	int     m_Transparency;
	//}}AFX_DATA
	BOOL    m_bTextDrawingClear;
	BOOL	m_bDisplayAccel;

	CString m_strSymPath0;

	double	m_fDrawScale0;
	double  m_fAnnoScale0;

	double  m_lfArcToler0;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COp_Page_Doc_Path)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COp_Page_Doc_Path)
	afx_msg void OnBrowse();
	afx_msg void OnAutoSet();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckModified();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OP_PAGE_DOC_PATH_H__1EF54A2F_D9D8_4921_908D_BA1C8F2270A3__INCLUDED_)
