#if !defined(AFX_DLGEXPORTCODETABLE_H__99FD3E7E_34B3_40A8_9F31_3EA4E261C861__INCLUDED_)
#define AFX_DLGEXPORTCODETABLE_H__99FD3E7E_34B3_40A8_9F31_3EA4E261C861__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportCodeTable.h : header file
//

#include "SymbolLib.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgExportCodeTable dialog

class CDlgExportCodeTable : public CDialog
{
// Construction
public:
	CDlgExportCodeTable(CWnd* pParent = NULL, CConfigLibManager* pCon = NULL);   // standard constructor

	void SetConfigData(CConfigLibManager* pCon);
// Dialog Data
	//{{AFX_DATA(CDlgExportCodeTable)
	enum { IDD = IDD_EXPORTCODETAB };
	CComboBox	m_comBoxScale;
	CString	m_strFilePath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportCodeTable)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExportCodeTable)
	afx_msg void OnButtonBrower();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CConfigLibManager* m_pData;
};



/////////////////////////////////////////////////////////////////////////////
// CDlgExportCodeTable dialog

class CDlgExportCodeTable_Symbols : public CDialog
{
	// Construction
public:
	CDlgExportCodeTable_Symbols(CWnd* pParent = NULL, CConfigLibManager* pCon = NULL);   // standard constructor
	
	void SetConfigData(CConfigLibManager* pCon);
	// Dialog Data
	//{{AFX_DATA(CDlgExportCodeTable)
	enum { IDD = IDD_EXPORTCODETAB_SYMBOLS };
	CComboBox	m_comBoxScale;
	CString	m_strFilePath;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportCodeTable)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgExportCodeTable)
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

#endif // !defined(AFX_DLGEXPORTCODETABLE_H__99FD3E7E_34B3_40A8_9F31_3EA4E261C861__INCLUDED_)
