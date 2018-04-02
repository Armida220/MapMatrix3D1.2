#pragma once
#include "res\resource.h"
#include <vector>
#include <string>
#include "mm3dPrj\simple_handler.h"
#include "DlgCefMinToolBar.h"

CString GetModuleFolder();
// CDlgCefMainToolBar dialog

class CDlgCefMainToolBar : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCefMainToolBar)

public:
	CDlgCefMainToolBar(std::string url = " ", CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCefMainToolBar();

// Dialog Data
	enum { IDD = IDD_DIALOG_CEF_MAINTOOLBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CDlgCefMinToolBar* pChild;
	int dlgHeight;
	int dlgWidth;
	std::string URL;

	void close();
	void showImg();
	CefRefPtr<SimpleHandler>& getInstance();
	std::vector<CefRefPtr<SimpleHandler>> vecHandler;
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonCeftoolbarClose();
	afx_msg void OnBnClickedButtonCeftoolbarHide();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnBnClickedButtonCeftoolbarAdd();
};
