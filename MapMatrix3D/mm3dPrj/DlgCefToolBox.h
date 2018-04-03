#pragma once
#include "res\resource.h"
#include <vector>
#include <string>
#include "mm3dPrj\simple_handler.h"
// CDlgCefToolBox dialog

class CDlgCefToolBox : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCefToolBox)

public:
	CDlgCefToolBox(std::string url = " ", CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCefToolBox();

// Dialog Data
	enum { IDD = IDD_DIALOG_CEFTOOLBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	std::string URL;

	void close();
	void showImg();
	CefRefPtr<SimpleHandler>& getInstance();
	std::vector<CefRefPtr<SimpleHandler>> vecHandler;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCeftoolboxClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
