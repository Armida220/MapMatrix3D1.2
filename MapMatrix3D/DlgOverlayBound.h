#if !defined(AFX_DLGOVERLAYBOUND_H__4E6F7976_7638_4A28_B658_A9384C683815__INCLUDED_)
#define AFX_DLGOVERLAYBOUND_H__4E6F7976_7638_4A28_B658_A9384C683815__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOverlayBound.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayBound dialog

class CDlgOverlayBound : public CDialog
{
	// Construction
public:
	CDlgOverlayBound(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgOverlayBound)
	enum { IDD = IDD_OVERLAY_BOUND };
	CMFCColorButton	m_colorBtn;
	double	m_lfX1;
	double	m_lfX2;
	double	m_lfX3;
	double	m_lfX4;
	double	m_lfY1;
	double	m_lfY2;
	double	m_lfY3;
	double	m_lfY4;
	double	m_lfZ1;
	double	m_lfZ2;
	double	m_lfZ3;
	double	m_lfZ4;
	BOOL	m_bImgView;
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	COLORREF m_color;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOverlayBound)
protected:
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgOverlayBound)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditZ1();
	afx_msg void OnChangeEditSwsbX2();
	afx_msg void OnReferenceDatafile();
	afx_msg void OnUpdateReferenceDatafile(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOVERLAYBOUND_H__4E6F7976_7638_4A28_B658_A9384C683815__INCLUDED_)
