#if !defined(AFX_PROPERTYSUMMARYPAGE_H__64020796_95D4_4EBE_AEFA_4D18182F6084__INCLUDED_)
#define AFX_PROPERTYSUMMARYPAGE_H__64020796_95D4_4EBE_AEFA_4D18182F6084__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// propertysummarypage.h : header file
//
#include "editbasedoc.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertySummaryPage dialog

class CPropertySummaryPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropertySummaryPage)

// Construction
public:
	CPropertySummaryPage();
	CPropertySummaryPage(CDlgDoc *pDoc);
	~CPropertySummaryPage();

// Dialog Data
	//{{AFX_DATA(CPropertySummaryPage)
	enum { IDD = IDD_SUMMARY_PROPPAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	CString m_strTitle;
	CString m_strTopic;
	CString m_strAuthor;
	CString m_strKeyWord;
	CString m_strRemark;
	CString m_strLink;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertySummaryPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertySummaryPage)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnAutoSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CDlgDoc *m_pDoc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYSUMMARYPAGE_H__64020796_95D4_4EBE_AEFA_4D18182F6084__INCLUDED_)
