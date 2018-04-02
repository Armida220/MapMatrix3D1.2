#if !defined(AFX_DLGSTRETCHIMG_H__0BBE0558_98BE_44B6_9B94_88E4C39C32A5__INCLUDED_)
#define AFX_DLGSTRETCHIMG_H__0BBE0558_98BE_44B6_9B94_88E4C39C32A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgStretchImg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgStretchImg dialog

class CDlgStretchImg : public CDialog
{
	// Construction
public:
	CDlgStretchImg(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgStretchImg)
	enum { IDD = IDD_STRETCHIMG };
	int		m_nAngIndex;
	int		m_nAngle;
	int		m_nAngle2;
	float	m_fXScale;
	float	m_fYScale;
	int		m_nAngIndex2;
	float	m_fXScale2;
	float	m_fYScale2;
	BOOL	m_bRightSame;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgStretchImg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgStretchImg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckRight();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSTRETCHIMG_H__0BBE0558_98BE_44B6_9B94_88E4C39C32A5__INCLUDED_)
