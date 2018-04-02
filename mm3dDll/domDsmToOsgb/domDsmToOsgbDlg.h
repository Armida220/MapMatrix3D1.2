
// domDsmToOsgbDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <string>
#include "resource.h"
#include "redirect.h"
// CdomDsmToOsgbDlg dialog
class CdomDsmToOsgbDlg : public CDialogEx
{
// Construction
public:
	CdomDsmToOsgbDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DOMDSMTOOSGB_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	CFont m_font1, m_font;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_domPath;
	CString m_dsmPath;
	CString m_Xnum;
	//CString m_Ynum;
//	CProgressCtrl m_progress;
	afx_msg void OnBnClickedButtonDom();
	CButton m_EXIT;
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonDsm();
	afx_msg void OnBnClickedOk();

	int xPart;
	int yPart;
	std::string filepath;
	std::string domfile;
	std::string demfile;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClickedButtonOsg();
	CString m_osgPath;

	redirect _redirect;
	CEdit m_messageControl;
};


