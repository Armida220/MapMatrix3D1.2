#if !defined(AFX_DLGREFDATABOUND_H__CA63FFD7_21BE_4AB8_A71C_27AC8003C038__INCLUDED_)
#define AFX_DLGREFDATABOUND_H__CA63FFD7_21BE_4AB8_A71C_27AC8003C038__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRefDataBound.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgRefDataBound dialog

class CDlgRefDataBound : public CDialog
{
// Construction
public:
	CDlgRefDataBound(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRefDataBound)
	enum { IDD = IDD_RESET_DISPLAY_BOUND };
	float	m_fBoundExt;
	int		m_nOption;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRefDataBound)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgRefDataBound)
	virtual void OnOK();
	afx_msg void OnRadioAll();
	afx_msg void OnRadioBound();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGREFDATABOUND_H__CA63FFD7_21BE_4AB8_A71C_27AC8003C038__INCLUDED_)
