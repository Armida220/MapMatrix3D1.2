#if !defined(AFX_PROPERTYFILEBASEINFOPAGE_H__5E74F1D4_0E61_4F27_866C_5279CCF92192__INCLUDED_)
#define AFX_PROPERTYFILEBASEINFOPAGE_H__5E74F1D4_0E61_4F27_866C_5279CCF92192__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// propertyfilebaseinfopage.h : header file
//
#include "editbasedoc.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyFileBaseInfoPage dialog

class CPropertyFileBaseInfoPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropertyFileBaseInfoPage)

// Construction
public:
	CPropertyFileBaseInfoPage();
	CPropertyFileBaseInfoPage(CDlgDoc *pDoc);
	~CPropertyFileBaseInfoPage();

// Dialog Data
	//{{AFX_DATA(CPropertyFileBaseInfoPage)
	enum { IDD = IDD_FILEBASEINFO_PROPPAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyFileBaseInfoPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyFileBaseInfoPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CDlgDoc *m_pDoc;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYFILEBASEINFOPAGE_H__5E74F1D4_0E61_4F27_866C_5279CCF92192__INCLUDED_)
