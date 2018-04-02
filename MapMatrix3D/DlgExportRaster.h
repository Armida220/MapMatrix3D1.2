#if !defined(AFX_DLGEXPORTRASTER_H__3A7CCB0B_9D58_4CF9_BE93_71E71EF81E73__INCLUDED_)
#define AFX_DLGEXPORTRASTER_H__3A7CCB0B_9D58_4CF9_BE93_71E71EF81E73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportRaster.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportRaster dialog

class CDlgExportRaster : public CDialog
{
// Construction
public:
	CDlgExportRaster(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExportRaster)
	enum { IDD = IDD_EXPORT_RASTER };
	CString	m_strFilePath;
	int	m_nBoundType;
	int m_nResolutionType;
	int m_nDPI;
	float	m_fMarginLeft;
	float	m_fMarginRight;
	float	m_fMarginTop;
	float	m_fMarginBottom;
	//}}AFX_DATA

	BOOL m_bOverImg;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportRaster)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableCtrl(int ID, BOOL bEnable);
	void CheckCtrl(int ID, BOOL bEnable);
	// Generated message map functions
	//{{AFX_MSG(CDlgExportRaster)
	afx_msg void OnButtonBrowse();
	afx_msg void OnRadioClickBoundType();
	afx_msg void OnRadioClickDPI();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTRASTER_H__3A7CCB0B_9D58_4CF9_BE93_71E71EF81E73__INCLUDED_)
