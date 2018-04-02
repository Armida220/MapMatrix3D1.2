#if !defined(AFX_DLGCOLORS_H__CA9994B8_5C4A_47C4_838D_EF24CD7F74C9__INCLUDED_)
#define AFX_DLGCOLORS_H__CA9994B8_5C4A_47C4_838D_EF24CD7F74C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgColors.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgColors dialog


class CStaticColor : public CStatic
{
	DECLARE_DYNAMIC(CStaticColor)
		
		// Constructors
public:
	CStaticColor();

	BOOL m_bHot;
	COLORREF m_color;	

	// Message map functions
protected:
	//{{AFX_MSG(CStaticColor)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMouseLeave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CDlgColors : public CDialog
{
// Construction
public:
	CDlgColors(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgColors();

// Dialog Data
	//{{AFX_DATA(CDlgColors)
	enum { IDD = IDD_COLORS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CArray<COLORREF,COLORREF> m_Colors;
	CArray<CStaticColor*,CStaticColor*> m_arrPBtns;

	CMFCButton m_wndOtherColors;

	COLORREF m_RetColor;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgColors)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgColors)
	afx_msg void OnButtonOthercolors();
	afx_msg LRESULT OnClickColor(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnNcActivate( BOOL bActive );	
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOLORS_H__CA9994B8_5C4A_47C4_838D_EF24CD7F74C9__INCLUDED_)
