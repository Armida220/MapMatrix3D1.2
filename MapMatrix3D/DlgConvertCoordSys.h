#if !defined(AFX_DLGCONVERTCOORDSYS_H__50402F16_1859_4033_B357_C53049FE7025__INCLUDED_)
#define AFX_DLGCONVERTCOORDSYS_H__50402F16_1859_4033_B357_C53049FE7025__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConvertCoordSys.h : header file
//

#include "tm.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgConvertCoordSys dialog

class CDlgConvertCoordSys : public CDialog
{
// Construction
public:
	CDlgConvertCoordSys(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgConvertCoordSys)
	enum { IDD = IDD_COORDSYS_CONVERT };
	int		m_nRange;
	CString	m_strPath;
	//}}AFX_DATA

	//投影信息
	CTM m_tm0, m_tm1;
	CStringArray m_arrFileNames;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConvertCoordSys)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void UpdateFileEdit();
	void UpdateCoordSysEdit(int idx);
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgConvertCoordSys)
	afx_msg void OnButtonBrowse();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonCoordsys0();
	afx_msg void OnButtonCoordsys1();
	afx_msg void OnRadioRange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONVERTCOORDSYS_H__50402F16_1859_4033_B357_C53049FE7025__INCLUDED_)
