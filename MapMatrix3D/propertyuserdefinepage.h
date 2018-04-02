#if !defined(AFX_PROPERTYUSERDEFINEPAGE_H__FEB19007_A748_4AE2_A5D5_0377F0A59052__INCLUDED_)
#define AFX_PROPERTYUSERDEFINEPAGE_H__FEB19007_A748_4AE2_A5D5_0377F0A59052__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// propertyuserdefinepage.h : header file
//
#include "editbasedoc.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyUserDefinePage dialog

class CPropertyUserDefinePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropertyUserDefinePage)

// Construction
public:
	CPropertyUserDefinePage();
	CPropertyUserDefinePage(CDlgDoc *pDoc);
	~CPropertyUserDefinePage();

// Dialog Data
	//{{AFX_DATA(CPropertyUserDefinePage)
	enum { IDD = IDD_USERDEFINED_PROPPAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	CString m_strName1, m_strValue1;
	CString m_strName2, m_strValue2;
	CString m_strName3, m_strValue3;
	CString m_strName4, m_strValue4;
	CString m_strName5, m_strValue5;
	CString m_strName6, m_strValue6;
	CString m_strName7, m_strValue7;
	CString m_strName8, m_strValue8;
	CString m_strName9, m_strValue9;
	CString m_strName10, m_strValue10;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyUserDefinePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyUserDefinePage)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnAutoSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CDlgDoc *m_pDoc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYUSERDEFINEPAGE_H__FEB19007_A748_4AE2_A5D5_0377F0A59052__INCLUDED_)
