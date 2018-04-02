#if !defined(AFX_DLGIMPORTCODETABLE_H__F4A4C891_AFCF_4D8F_A866_99E05D609CFB__INCLUDED_)
#define AFX_DLGIMPORTCODETABLE_H__F4A4C891_AFCF_4D8F_A866_99E05D609CFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportCodeTable.h : header file
//
#include "SymbolLib.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgImportCodeTable dialog

class CDlgImportCodeTable : public CDialog
{
// Construction
public:
	CDlgImportCodeTable(CWnd* pParent = NULL, CConfigLibManager* pCon=NULL);   // standard constructor

	void SetConfigData(CConfigLibManager* pCon);
// Dialog Data
	//{{AFX_DATA(CDlgImportCodeTable)
	enum { IDD = IDD_IMPORTCODETAB };
	CComboBox	m_comBoxMode;
	CComboBox	m_comBoxScale;
	CString	m_strFilePath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportCodeTable)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImportCodeTable)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnButtonBrower();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CConfigLibManager* m_pData;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTCODETABLE_H__F4A4C891_AFCF_4D8F_A866_99E05D609CFB__INCLUDED_)
