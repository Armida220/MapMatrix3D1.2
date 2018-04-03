#if !defined(AFX_DLGDISPLAYCONTOURSSPECIALLY_H__D89871AD_8692_49F0_90C5_D2D413939ACC__INCLUDED_)
#define AFX_DLGDISPLAYCONTOURSSPECIALLY_H__D89871AD_8692_49F0_90C5_D2D413939ACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDisplayContoursSpecially.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDisplayContoursSpecially dialog

class CDlgDisplayContoursSpecially : public CDialog
{
// Construction
public:
	CDlgDisplayContoursSpecially(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CDlgDisplayContoursSpecially)
	enum { IDD = IDD_DISPLAY_CONTOURS_SPECIALLY };
	float	m_fInterval;
	int		m_nInterNum;
	//}}AFX_DATA
	
	CMFCColorButton m_btnColor0;
	CMFCColorButton m_btnColor1;
	CMFCColorButton m_btnColor2;
	CMFCColorButton m_btnColor3;
	CMFCColorButton m_btnColor4;
	
	COLORREF m_clr0;
	COLORREF m_clr1;
	COLORREF m_clr2;
	COLORREF m_clr3;
	COLORREF m_clr4;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDisplayContoursSpecially)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgDisplayContoursSpecially)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDISPLAYCONTOURSSPECIALLY_H__D89871AD_8692_49F0_90C5_D2D413939ACC__INCLUDED_)
