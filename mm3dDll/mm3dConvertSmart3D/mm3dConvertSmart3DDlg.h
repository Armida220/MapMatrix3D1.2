
// mm3dConvertSmart3DDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <string>
#include <vector>
#include "resource.h"
// Cmm3dConvertSmart3DDlg dialog
class Cmm3dConvertSmart3DDlg : public CDialogEx
{
// Construction
public:
	Cmm3dConvertSmart3DDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MM3DCONVERTSMART3D_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	CFont m_font;
	CFont m_font1;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_path;
	CButton m_EXIT;
	CProgressCtrl m_progress;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButtonSelectOsgb();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	std::string outputFilePath;
};
