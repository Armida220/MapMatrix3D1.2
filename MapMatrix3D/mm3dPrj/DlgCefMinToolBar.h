#pragma once
#include "res\resource.h"
// CDlgCefMinToolBar dialog

class CDlgCefMinToolBar : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCefMinToolBar)

public:
	CDlgCefMinToolBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCefMinToolBar();

// Dialog Data
	enum { IDD = IDD_DIALOG_CEFTOOLBAR_MIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CObject* parent;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCefmintoolbarClose();
	afx_msg void OnBnClickedButtonCeftoolbarMax();
};
