#if !defined(AFX_DLGUSEDFONTS_H__51FAF00C_E966_4B0B_A2E5_5C48D3607BEE__INCLUDED_)
#define AFX_DLGUSEDFONTS_H__51FAF00C_E966_4B0B_A2E5_5C48D3607BEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgUsedFonts.h : header file
//

#include "GeoText.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgNewTextStyle dialog

class CDlgNewTextStyle : public CDialog
{
	// Construction
public:
	CDlgNewTextStyle(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgNewTextStyle)
	enum { IDD = IDD_NEWTEXTSTYLE_DIALOG };
	CString	m_strStyleName;
	CString	m_strFontName;
	int		m_nInclinedType;
	float	m_fInclinedAngle;
	float	m_fWidScale;
	int		m_nBold;
	//}}AFX_DATA
	
	TextStyle m_style;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewTextStyle)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgNewTextStyle)
	afx_msg void OnBrowsefont();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CStatic_TextStylePreview : public CStatic
{
	// Construction
	DECLARE_DYNAMIC(CStatic_TextStylePreview)
public:
	CStatic_TextStylePreview();
	
	TextStyle m_style; 
	
	// Attributes
public:
	
	// Operations
public:
	
	// Overrides
public:
	virtual ~CStatic_TextStylePreview();
	
	// Generated message map functions
	//{{AFX_MSG(CStatic_TextStylePreview)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CDlgUsedFonts dialog

class CDlgUsedFonts : public CDialog
{
// Construction
public:
	CDlgUsedFonts(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgUsedFonts)
	enum { IDD = IDD_USEDFONT };
	CListCtrl	m_wndStyles;
	CStatic_TextStylePreview m_wndPreview;
	//}}AFX_DATA

	TextStyles m_styles;

	CString m_strResult;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgUsedFonts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InsertItem(int index, const TextStyle& item);

	// Generated message map functions
	//{{AFX_MSG(CDlgUsedFonts)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnModify();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListStyles(NMHDR * pNotifyStruct, LRESULT * result);
	afx_msg void OnSelchangeStyles(NMHDR * pNotifyStruct, LRESULT * result);
	afx_msg void OnUp();
	afx_msg void OnDown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGUSEDFONTS_H__51FAF00C_E966_4B0B_A2E5_5C48D3607BEE__INCLUDED_)
