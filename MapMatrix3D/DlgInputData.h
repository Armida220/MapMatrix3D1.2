#if !defined(AFX_DLGINPUTDATA_H__D5A7F6A6_7D6E_41F4_8FC8_D5213F1BC310__INCLUDED_)
#define AFX_DLGINPUTDATA_H__D5A7F6A6_7D6E_41F4_8FC8_D5213F1BC310__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgInputData.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInputData dialog

class CDlgInputData : public CDialog
{
// Construction
public:
	CDlgInputData(CWnd* pParent = NULL);   // standard constructor
	double GetInputData() {
		return m_fData;
	}
	void  SetInputData(double data){
		m_fData = data;
	}
	void SetShowPos(CPoint pt) {
		m_ptShow = pt;
		m_bptShow = TRUE;
	}
// Dialog Data
	//{{AFX_DATA(CDlgInputData)
	enum { IDD = IDD_INPUT };
		// NOTE: the ClassWizard will add data members here
	double	m_fData;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInputData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInputData)
		// NOTE: the ClassWizard will add member functions here
	afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor ); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CPoint   m_ptShow;
	BOOL     m_bptShow;
	COLORREF m_colorBak;
	CBrush	 m_brushBak;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINPUTDATA_H__D5A7F6A6_7D6E_41F4_8FC8_D5213F1BC310__INCLUDED_)
