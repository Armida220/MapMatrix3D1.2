#if !defined(AFX_DLGEXPORTMODEL_H__A21E272C_5711_4DBE_B711_2F2CFBCC62EB__INCLUDED_)
#define AFX_DLGEXPORTMODEL_H__A21E272C_5711_4DBE_B711_2F2CFBCC62EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportModel.h : header file
//

#include "GeoBuilderPrj2.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgExportModel dialog

class CDlgExportModel : public CDialog
{
// Construction
public:
	CDlgExportModel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExportModel)
	enum { IDD = IDD_EXPORT_MODEL };
	CComboBox	m_wndStereo;
	CListCtrl	m_lstCtrl;
	CString	m_strMdlFile;
	CString	m_strDEMFile;
	CString	m_strOrthoFile;
	BOOL	m_bTexture;
	int		m_nLeftImg;
	CString	m_strPrjFile;
	BOOL	m_bFixHide;
	BOOL    m_bExportSideFace;
	int		m_nTextureSource;
	//}}AFX_DATA

	CString m_strStereo;
	//CGeoBuilderPrj m_prj;

	CStringArray m_arrDEMFileNames;
	CArray<CGeoBuilderPrj*,CGeoBuilderPrj*> m_arrPrjs;
	int m_nCurPrj;

	CStringArray m_strInitList;
	CStringArray m_strSelList;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportModel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableTextureCtrls(BOOL bEnableStereo, BOOL bEnableOrtho);

	// Generated message map functions
	//{{AFX_MSG(CDlgExportModel)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBrowseXML();
	afx_msg void OnBrowseDEM();
	afx_msg void OnBrowsePrj();
	afx_msg void OnBrowseOrtho();
	afx_msg void OnCheckTexture();
	afx_msg void OnCheckFixHide();
	afx_msg void OnCheckExportSideFace();
	afx_msg void OnStereoSelChanged();
	afx_msg void OnRadioStereo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTMODEL_H__A21E272C_5711_4DBE_B711_2F2CFBCC62EB__INCLUDED_)