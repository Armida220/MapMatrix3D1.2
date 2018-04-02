#if !defined(AFX_DLGOVERLAYORTHO_H__7330027D_FA23_4B07_9DFA_D1D872FD19A5__INCLUDED_)
#define AFX_DLGOVERLAYORTHO_H__7330027D_FA23_4B07_9DFA_D1D872FD19A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOverlayOrtho.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CDlgList dialog

class CDlgList : public CDialog
{
	// Construction
public:
	CDlgList(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgList)
	enum { IDD = IDD_LIST };
	CListBox	m_wndList;
	//}}AFX_DATA
	
	CStringArray m_strList;
	CString m_strSelItem;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgList)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgList)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayOrtho dialog

class CDlgOverlayOrtho : public CDialog
{
	// Construction
public:
	CDlgOverlayOrtho(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgOverlayOrtho)
	enum { IDD = IDD_OVERLAY_ORTHO };
	double	m_lfMatrix1;
	double	m_lfMatrix2;
	double	m_lfMatrix3;
	double	m_lfMatrix4;
	double	m_lfXOff;
	double	m_lfYOff;
	CString	m_strFileName;
	BOOL	m_bVisible;
	int		m_nPixelBase;
	//}}AFX_DATA
	
	PT_3D m_ptsBound[4];

	double m_lfPixelSizeX, m_lfPixelSizeY;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOverlayOrtho)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgOverlayOrtho)
	afx_msg void OnBrowseAny();
	afx_msg void OnBrowseLocal();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonUnload();
	afx_msg void OnButtonMapbound();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOVERLAYORTHO_H__7330027D_FA23_4B07_9DFA_D1D872FD19A5__INCLUDED_)
