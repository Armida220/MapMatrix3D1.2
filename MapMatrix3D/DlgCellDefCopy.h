#if !defined(AFX_DLGCELLDEFCOPY_H__DDA0A18D_75EF_4BF9_AB02_67C818DCEEE4__INCLUDED_)
#define AFX_DLGCELLDEFCOPY_H__DDA0A18D_75EF_4BF9_AB02_67C818DCEEE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCellDefCopy.h : header file
//

#include "SymbolLib.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgCellDefCopy dialog

class CDlgCellDefCopy : public CDialog
{
// Construction
public:
	CDlgCellDefCopy(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCellDefCopy)
	enum { IDD = IDD_CELLCOPY };
	CString	m_strCellDefName;
	CString	m_strLTName;
	CString	m_strPath1;
	CString	m_strPath2;
	//}}AFX_DATA

	BOOL m_bCheckCover;

	ConfigLibItem m_lib1,m_lib2;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCellDefCopy)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// type:0,celldef,1,linetype
	void CopyItem(LPCTSTR name, int type, BOOL bCover);

	// Generated message map functions
	//{{AFX_MSG(CDlgCellDefCopy)
	afx_msg void OnButtonBrowse1();
	afx_msg void OnButtonBrowse2();
	afx_msg void OnButtonBrowse3();
	afx_msg void OnButtonBrowse4();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCELLDEFCOPY_H__DDA0A18D_75EF_4BF9_AB02_67C818DCEEE4__INCLUDED_)
