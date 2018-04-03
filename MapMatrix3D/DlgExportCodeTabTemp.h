#if !defined(AFX_DLGEXPORTCODETABTEMP_H__C3EBCA1B_8EAA_46F7_89B3_BDFAF3E189E4__INCLUDED_)
#define AFX_DLGEXPORTCODETABTEMP_H__C3EBCA1B_8EAA_46F7_89B3_BDFAF3E189E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportCodeTabTemp.h : header file
//

#include "SymbolLib.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgExportCodeTabTemp dialog

class CDlgExportCodeTabTemp : public CDialog
{
// Construction
public:
	CDlgExportCodeTabTemp(CWnd* pParent = NULL, CConfigLibManager* pCon = NULL);   // standard constructor
	void SetConfigData(CConfigLibManager* pCon);
// Dialog Data
	//{{AFX_DATA(CDlgExportCodeTabTemp)
	enum { IDD = IDD_EXPORTCODETABTEMP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CComboBox	m_comBoxScale;
	CString	m_strFilePath;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportCodeTabTemp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExportCodeTabTemp)
	afx_msg void OnButtonBrower();
	//}}AFX_MSG
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
private:
	CConfigLibManager* m_pData;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTCODETABTEMP_H__C3EBCA1B_8EAA_46F7_89B3_BDFAF3E189E4__INCLUDED_)
