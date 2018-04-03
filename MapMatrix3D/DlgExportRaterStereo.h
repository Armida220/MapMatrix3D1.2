#if !defined(AFX_DLGEXPORTRATERSTEREO_H__B76009E3_5CFC_4104_9283_0B18C47B8557__INCLUDED_)
#define AFX_DLGEXPORTRATERSTEREO_H__B76009E3_5CFC_4104_9283_0B18C47B8557__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportRaterStereo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportRaterStereo dialog

class CDlgExportRaterStereo : public CDialog
{
	// Construction
public:
	CDlgExportRaterStereo(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgExportRaterStereo)
	enum { IDD = IDD_EXPORT_STEREO_RASTER };
	CString	m_strFilePath;
	int		m_nImage;
	double	m_fPGScale;
	double	m_fPixelSize;
	int		m_nScaleType;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportRaterStereo)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgExportRaterStereo)
	afx_msg void OnButtonBrowse();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioMapScale();
	afx_msg void OnRadioCurrentScale();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTRATERSTEREO_H__B76009E3_5CFC_4104_9283_0B18C47B8557__INCLUDED_)
