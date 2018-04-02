#if !defined(AFX_DLGSUPPORTGEOCLASSNAME_H__EA9DC592_4645_42C5_B52F_0A3EE78EE761__INCLUDED_)
#define AFX_DLGSUPPORTGEOCLASSNAME_H__EA9DC592_4645_42C5_B52F_0A3EE78EE761__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSupportGeoClassName.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSupportGeoClassName dialog

class CDlgSupportGeoClassName : public CDialog
{
// Construction
public:
	CDlgSupportGeoClassName(CWnd* pParent = NULL);   // standard constructor
	void SetData(const CStringArray &data);
	BOOL GetData(CStringArray &str);
	int m_nSupportFirstgeo;

// Dialog Data
	//{{AFX_DATA(CDlgSupportGeoClassName)
	enum { IDD = IDD_SUPPORTGEOCLASS_DIALOG };
	CButton	m_cDLine;
	CButton	m_cText;
	CButton	m_cSurface;
	CButton	m_cPoint;
	CButton	m_cParallel;
	CButton	m_cLine;
	CButton	m_cDirPoint;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSupportGeoClassName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CStringArray m_strSupportGeoClsName;

	// Generated message map functions
	//{{AFX_MSG(CDlgSupportGeoClassName)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSUPPORTGEOCLASSNAME_H__EA9DC592_4645_42C5_B52F_0A3EE78EE761__INCLUDED_)
