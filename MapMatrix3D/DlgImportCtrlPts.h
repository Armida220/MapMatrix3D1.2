#if !defined(AFX_DLGIMPORTCTRLPTS_H__F4EEE51E_16AA_43C9_AD76_AA3C62748648__INCLUDED_)
#define AFX_DLGIMPORTCTRLPTS_H__F4EEE51E_16AA_43C9_AD76_AA3C62748648__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportCtrlPts.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgImportCtrlPts dialog

class CDlgImportCtrlPts : public CDialog
{
// Construction
public:
	void AddLayerName(const char* name);
	CDlgImportCtrlPts(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgImportCtrlPts)
	enum { IDD = IDD_IMPORT_CTRLPTS };
	CString	m_strFilePath;
	CString	m_strLayer;
	CString m_strField;
	BOOL	m_bInMapBound;
	BOOL	m_bCreateLine;
	CString	m_strMapName;
	//}}AFX_DATA

private:
	CStringArray m_arrLayers;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportCtrlPts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImportCtrlPts)
	afx_msg void OnFileBrowse();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckInmapbound();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTCTRLPTS_H__F4EEE51E_16AA_43C9_AD76_AA3C62748648__INCLUDED_)
