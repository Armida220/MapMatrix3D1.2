#if !defined(AFX_DLGREPAIRPTZTEXT_H__18190147_882C_426D_85D7_7E657A8A636B__INCLUDED_)
#define AFX_DLGREPAIRPTZTEXT_H__18190147_882C_426D_85D7_7E657A8A636B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRepairPTZText.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgRepairPTZText dialog

class CDlgRepairPTZText : public CDialog
{
// Construction
public:
	CDlgRepairPTZText(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRepairPTZText)
	enum { IDD = IDD_REPAIR_PTZTEXT };
	CButton	m_AnnoToZ;
	int		m_sel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRepairPTZText)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRepairPTZText)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGREPAIRPTZTEXT_H__18190147_882C_426D_85D7_7E657A8A636B__INCLUDED_)
