#if !defined(AFX_DLGCONVERTBYPTS_H__720495E3_AF22_4DD1_B6B2_972FB0557ABA__INCLUDED_)
#define AFX_DLGCONVERTBYPTS_H__720495E3_AF22_4DD1_B6B2_972FB0557ABA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConvertByPts.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgConvertByPts dialog

class CDlgConvertByPts : public CDialog
{
// Construction
public:
	CDlgConvertByPts(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgConvertByPts)
	enum { IDD = IDD_PTCONVERT };
	CString	m_strPath;
	CString	m_strPath2;
	CString	m_strPath3;
	int		m_nRange;
	BOOL	m_b2D;
	//}}AFX_DATA

	CStringArray m_arrFileNames;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConvertByPts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void UpdateFileEdit();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgConvertByPts)
	afx_msg void OnButtonBrowse();
	afx_msg void OnButtonBrowse2();
	afx_msg void OnButtonBrowse3();
	virtual void OnOK();
	afx_msg void OnRadioRange();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONVERTBYPTS_H__720495E3_AF22_4DD1_B6B2_972FB0557ABA__INCLUDED_)
