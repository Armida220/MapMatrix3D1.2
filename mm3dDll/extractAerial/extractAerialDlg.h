
// extractAerialDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "CMyListCtr.h"
#include "afxwin.h"

// CextractAerialDlg dialog
class CextractAerialDlg : public CDialogEx
{
// Construction
public:
	CextractAerialDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EXTRACTAERIAL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CFont m_font, m_font1;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CMyListCtrl m_wndList;
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	CString m_inXML;
	CString m_outXML;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CButton m_EXIT;
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonImgpath();
//	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonInxml();
	afx_msg void OnBnClickedButtonOutxml();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);

//	BOOL m_showWarning;
	CButton m_cShowWarning;
};
