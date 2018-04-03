#if !defined(AFX_DLGLAYERCODECHECK_H__FB9F993B_C6EE_41B7_AF13_38F1C14F9F6D__INCLUDED_)
#define AFX_DLGLAYERCODECHECK_H__FB9F993B_C6EE_41B7_AF13_38F1C14F9F6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLayerCodeCheck.h : header file
//


#include "editbaseDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgLayerCodeCheck dialog

class CDlgLayerCodeCheck : public CDialog
{
// Construction
public:
	CDlgLayerCodeCheck(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgLayerCodeCheck)
	enum { IDD = IDD_LAYERCODE_CHECK };
	CListCtrl	m_wndLayers;
	BOOL	m_bCheckAlllayers;
	//}}AFX_DATA

	CDlgDoc *m_pDoc;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLayerCodeCheck)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void FillLayers();

	int m_nCurColumn;
	BOOL m_bSortAscending;

	// Generated message map functions
	//{{AFX_MSG(CDlgLayerCodeCheck)
	afx_msg void OnCheckAlllayers();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListLayers(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListLayers(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLAYERCODECHECK_H__FB9F993B_C6EE_41B7_AF13_38F1C14F9F6D__INCLUDED_)
