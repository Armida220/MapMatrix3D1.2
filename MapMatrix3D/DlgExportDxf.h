#if !defined(AFX_DLGEXPORTDXF_H__3FA81F44_2217_4D17_8E83_5F65157AEE88__INCLUDED_)
#define AFX_DLGEXPORTDXF_H__3FA81F44_2217_4D17_8E83_5F65157AEE88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportDxf.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportDxf dialog

class CDlgExportDxf : public CDialog
{
// Construction
public:
	CDlgExportDxf(CWnd* pParent = NULL);   // standard constructor
	enum InitType
	{	
		BATCHEXPORTDXF = 0,
		EXPORTEXF  =  1			
			
	};
// Dialog Data
	//{{AFX_DATA(CDlgExportDxf)
	enum { IDD = IDD_EXPORT_DXF };
	CString	m_strDxfFile;
	CString	m_strLstFile;
	CString	m_strColourFile;
	CString m_strFdbFile;
	CButton m_addfdbBtn;
	BOOL	m_bUseIndex;
	BOOL    m_bUseLayerGroup;
	BOOL	m_bAddAnnot;
	BOOL	m_bMapFrame;
	BOOL	m_bLinearize;
	BOOL	m_bLines;
	BOOL	m_bLineSym;
	BOOL	m_bPoint;
	BOOL	m_bPointSym;
	BOOL	m_bSepPara;
	BOOL    m_bParaCenline;
	BOOL	m_bSurface;
	BOOL	m_bSurfaceSym;
	BOOL	m_bText;
	BOOL	m_b3DLines;
	BOOL	m_bDirPtAsLine;
	BOOL	m_bClosedCurve;
	BOOL	m_bTextSym;
	BOOL    m_bDifferBaselineSym;
	BOOL	m_bBlockScale;
	int		m_nDxfFormat;
	float	m_fBlockScale;
	BOOL	m_bExportAsBlock;
	BOOL    m_bUseRGBTrueColor;
	//}}AFX_DATA
	CStringArray m_arrStr;
	CStringArray m_arrFdbStr;
	CStringArray m_arrDxfStr;
	CArray<int,int> m_arrFlags;
	int m_Type;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportDxf)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateFormatButton();

	// Generated message map functions
	//{{AFX_MSG(CDlgExportDxf)
	afx_msg void OnBrowseDxf();
	afx_msg void OnBrowseLst();
	afx_msg void OnBrowseFdb();
	afx_msg void OnBrowseColour();
	virtual void OnOK();
	afx_msg void OnSetfocusEditLstFile();
	afx_msg void OnKillfocusEditLstFile();
	afx_msg void OnSetfocusEditColorFile();
	afx_msg void OnKillfocusEdiColorFile();
	afx_msg void OnCheckPoints();
	afx_msg void OnCheckLines();
	afx_msg void OnCheckSurfaces();
	afx_msg void OnCheckUseIndex();
	afx_msg void OnCheckLayerGroup();
	afx_msg void OnShiftDxfDwg();
	afx_msg void OnEditChangeDxfFile();
	afx_msg void OnBtnExportMLine();
	afx_msg void OnBtnExportMLineAndSymbol();
	afx_msg void OnBtnExportSymbol();
	afx_msg void OnBlockScale();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTDXF_H__3FA81F44_2217_4D17_8E83_5F65157AEE88__INCLUDED_)
