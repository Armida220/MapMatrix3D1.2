
// linkFetchCoordDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <string>

// ClinkFetchCoordDlg dialog
class ClinkFetchCoordDlg : public CDialogEx
{
// Construction
public:
	ClinkFetchCoordDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LINKFETCHCOORD_DIALOG };

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
	CButton m_EXIT;
	CProgressCtrl m_progress;
	CString m_xmlPath;
	CString m_outputPath;
	CString m_IndexPath;
	CString LoadString(UINT uid);
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonIndex();
	afx_msg void OnBnClickedButtonXml();
	afx_msg void OnBnClickedButtonOutput();
	afx_msg void OnBnClickedOk();

	void extract(std::string str, double &x, double &y, double &z);
};
