#if !defined(AFX_DLGMAPINFO_H__67E16B37_58C7_4123_803C_6A9AF27EA8ED__INCLUDED_)
#define AFX_DLGMAPINFO_H__67E16B37_58C7_4123_803C_6A9AF27EA8ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMapInfo.h : header file
//

#include "tm.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgMapInfo dialog

class CDlgMapInfo : public CDialog
{
// Construction
public:
	CDlgMapInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMapInfo)
	enum { IDD = IDD_MAPINFO };
	CString	m_strProjection;
	//}}AFX_DATA

	//投影信息
	CTM m_tm0;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMapInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateCoordSysEdit();
	// Generated message map functions
	//{{AFX_MSG(CDlgMapInfo)
	afx_msg void OnButtonProjection();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMAPINFO_H__67E16B37_58C7_4123_803C_6A9AF27EA8ED__INCLUDED_)
