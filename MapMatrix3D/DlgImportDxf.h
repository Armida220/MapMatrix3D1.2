#if !defined(AFX_DLGIMPORTDXF_H__D5A51FB8_0884_4239_AD90_FA04E49ED425__INCLUDED_)
#define AFX_DLGIMPORTDXF_H__D5A51FB8_0884_4239_AD90_FA04E49ED425__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportDxf.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgImportDxf dialog

class CDlgImportDxf : public CDialog
{
// Construction
public:
	CDlgImportDxf(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgImportDxf)
	enum { IDD = IDD_IMPORT_DXF };
	CString	m_strDxfFile;
	CString	m_strLstFile;
	BOOL	m_bUseIndex;
	BOOL	m_bBlockAsPoint;
	CString	m_strMapName;
	BOOL	m_bUnImport;
	BOOL	m_bImportAnno;
	BOOL	m_bSaveLinetype;
	BOOL    m_bReadCassCode;
	//}}AFX_DATA

	BOOL	m_bLidarPoints;
	CStringArray m_arrFileNames;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportDxf)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImportDxf)
	afx_msg void OnBrowseDxf();
	afx_msg void OnBrowseLst();
	virtual void OnOK();
	afx_msg void OnKillfocusEditLstFile();
	afx_msg void OnSetfocusEditLstFile();
	afx_msg void OnExistnewlayerCheck();
	afx_msg void OnNewlayerCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTDXF_H__D5A51FB8_0884_4239_AD90_FA04E49ED425__INCLUDED_)
