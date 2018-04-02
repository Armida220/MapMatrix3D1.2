#if !defined(AFX_OPTIONSDLG_H__8C61C1D5_B379_4D85_BD46_4F783E2FD13F__INCLUDED_)
#define AFX_OPTIONSDLG_H__8C61C1D5_B379_4D85_BD46_4F783E2FD13F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDlg.h : header file
//

#include "OptionsPages.h"
#include "Op_Page_Doc_Path.h"
#include "Op_Page_QT1.h"
#include "Op_Page_QT2.h"
#include "Op_Page_QT3.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg

//##ModelId=41466B7C02FD
class COptionsDlg : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(COptionsDlg)

// Construction
public:
	//##ModelId=41466B7C030D
	COptionsDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	//##ModelId=41466B7C0312
	COp_Page_View_Commonly	m_View_Commonly;
	//##ModelId=41466B7C031E
	COp_Page_View_Measure	m_View_Measure;
	//##ModelId=41466B7C0312
	COp_Page_View_VectView	m_View_VectView;
	COp_Page_View_ImageView	m_View_ImageView;
	//##ModelId=41466B7C032C
	COp_Page_Doc_Snap		m_Doc_Snap;
	//##ModelId=41466B7C0331
	COp_Page_Doc_Save		m_Doc_Save;
	//##ModelId=41466B7C0312
	COp_Page_Doc_User		m_Doc_User;
	//##ModelId=41466B7C034D
	COp_Page_Ui_Commonly	m_Ui_Commonly;
	//##ModelId=41466B7C035B
	COp_Page_Ui_SaveOptions	m_Ui_SaveOptions;
	//##MOdelId=41466B7C02D0
	COp_Page_Doc_Alert      m_Doc_Alert;
	//##ModelId=41466b7c02d2
	COp_Page_Doc_Select     m_Doc_Select;
	COp_Page_Doc_Path		m_Doc_Path;
	COp_Page_Precision		m_Precision;
	COp_Page_QT1			m_QT_Param1;
	COp_Page_QT2			m_QT_Param2;
	COp_Page_QT3			m_QT_Param3;
	COp_Page_NetWork  m_Page_NetWork;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	public:
	virtual INT_PTR DoModal();
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=41466B7C0369
	virtual ~COptionsDlg();

	// Generated message map functions
protected:
	//{{AFX_MSG(COptionsDlg)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg void OnSelectTree(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__8C61C1D5_B379_4D85_BD46_4F783E2FD13F__INCLUDED_)
