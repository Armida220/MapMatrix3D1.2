#pragma once
#include "mm3d_clientHandler.h"
#include "resource.h"
// CMM3DDialog dialog

class CMM3DDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMM3DDialog)

public:
	CMM3DDialog(CString typeHtml, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMM3DDialog();

// Dialog Data
	enum { IDD = IDD_MM3DDIALOG };

	static CefRefPtr<CMm3dClientHandler> getClientHandler();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();

protected:
	CString mTypeHtml;



protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
