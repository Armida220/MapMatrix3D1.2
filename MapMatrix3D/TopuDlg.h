#if !defined(AFX_TOPUDLG_H__45F77C03_8E04_46AA_BF05_4FA0E8321EC4__INCLUDED_)
#define AFX_TOPUDLG_H__45F77C03_8E04_46AA_BF05_4FA0E8321EC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TopuDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTopuDlg dialog

class CTopuDlg : public CDialog
{
// Construction
public:
	CTopuDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTopuDlg)
	enum { IDD = IDD_TOPO };
	CComboBox	m_comboxScheme;
	double	m_lfToler;
	CString	m_strLayerCode;	
	int		m_nOption;
	//}}AFX_DATA
	CString m_strCurSchemaName;
	CStringArray m_arrStrSchemaNames;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTopuDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTopuDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioInput();
	afx_msg void OnRadioOption();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOPUDLG_H__45F77C03_8E04_46AA_BF05_4FA0E8321EC4__INCLUDED_)
