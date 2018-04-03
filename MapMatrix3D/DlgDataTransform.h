#if !defined(AFX_DLGDATATRANSFORM_H__60D57A5D_B70E_4B77_8DE7_E1B8F1C37CEA__INCLUDED_)
#define AFX_DLGDATATRANSFORM_H__60D57A5D_B70E_4B77_8DE7_E1B8F1C37CEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDataTransform.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDataTransform dialog

class CDlgDataTransform : public CDialog
{
// Construction
public:
	CDlgDataTransform(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDataTransform)
	enum { IDD = IDD_DATATRANSFORM };
	CComboBox	m_wndMetric;
	double	m_lfDX;
	double	m_lfDY;
	double	m_lfDZ;
	double	m_lfKX;
	double	m_lfKY;
	double	m_lfKZ;
	CString	m_strPath;
	int		m_nRange;
	//}}AFX_DATA

	CStringArray m_arrFileNames;

private:
	CArray<double,double> m_arrCoefs;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDataTransform)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void UpdateFileEdit();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDataTransform)
	afx_msg void OnSelchangeComboMetric();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonBrowse();
	afx_msg void OnRadioRange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDATATRANSFORM_H__60D57A5D_B70E_4B77_8DE7_E1B8F1C37CEA__INCLUDED_)
