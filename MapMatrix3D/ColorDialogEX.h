#if !defined(AFX_COLORDIALOGEX_H__BC7B5DC9_6195_49F7_9AF4_F89EF84C46BC__INCLUDED_)
#define AFX_COLORDIALOGEX_H__BC7B5DC9_6195_49F7_9AF4_F89EF84C46BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorDialogEX.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorDialogEX dialog

class CColorDialogEX : public CColorDialog
{
	DECLARE_DYNAMIC(CColorDialogEX)

public:
	CColorDialogEX(COLORREF clrInit = 0, DWORD dwFlags = 0,
			CWnd* pParentWnd = NULL);

protected:
	//{{AFX_MSG(CColorDialogEX)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORDIALOGEX_H__BC7B5DC9_6195_49F7_9AF4_F89EF84C46BC__INCLUDED_)
