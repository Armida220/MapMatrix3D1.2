#if !defined(AFX_DLGCELLOVERLAYGRID_H__7EB36B28_4961_45C4_9CF2_1789F20B38F1__INCLUDED_)
#define AFX_DLGCELLOVERLAYGRID_H__7EB36B28_4961_45C4_9CF2_1789F20B38F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCellOverlayGrid.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCellOverlayGrid dialog

class CDlgCellOverlayGrid : public CDialog
{
// Construction
public:
	CDlgCellOverlayGrid(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCellOverlayGrid)
	enum { IDD = IDD_CELL_OVERLAY_GRID };
	CMFCColorButton	m_colorBtn;
	BOOL	m_bOverlay;
	float	m_fHei;
	float	m_fWid;
	float	m_fXOrigin;
	float	m_fYOrigin;
	BOOL	m_bImgView;
	BOOL	m_bVectView;
	float	m_fXRange;
	float	m_fYRange;
	BOOL	m_bSnapGrid;
	CString	m_strZ;
	//}}AFX_DATA
	
	COLORREF m_color;
	PT_3D m_ptRegion[4];
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOverlayGrid)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgOverlayGrid)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnWSRegion();
	afx_msg void OnButtonDem();
	afx_msg void OnChangeEditOgWid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCELLOVERLAYGRID_H__7EB36B28_4961_45C4_9CF2_1789F20B38F1__INCLUDED_)
