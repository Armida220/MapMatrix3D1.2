#if !defined(AFX_DLGCELLDEFVIEW_H__A67EB2A5_9A5F_4B6E_B895_8DBD08E49E85__INCLUDED_)
#define AFX_DLGCELLDEFVIEW_H__A67EB2A5_9A5F_4B6E_B895_8DBD08E49E85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCelldefView.h : header file
//

#include "Symbol.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgCelldefView dialog

class CDlgCelldefView : public CDialog
{
// Construction
public:
	CDlgCelldefView(CWnd* pParent = NULL);   // standard constructor
	void SetCellDefLib(CCellDefLib *pLib);

// Dialog Data
	//{{AFX_DATA(CDlgCelldefView)
	enum { IDD = IDD_CELLDEF_VIEW };
	CListCtrl	m_wndListSymbol;
	//}}AFX_DATA

	CCellDefLib *m_pLib;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCelldefView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	CImageList m_listImages;
	CArray<int,int> m_arrIdxCreateFlag;
	int m_nIdxToCreateImage, m_nImageWid, m_nImageHei;

private:
	void FillSymbolList();	
	void CreateImageList(int cx, int cy, int nsize);
	BOOL CreateImageItem(int idx);
	void DrawSymbol(CellDef *def, GrBuffer2d *buf);
	void DrawImageItem(int idx, int cx, int cy, const GrBuffer2d *pBuf);
	
	void DrawPreview(LPCTSTR strSymName);

	void OnStaticPreview();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCelldefView)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditSearch();
	afx_msg void OnSetfocusEditSearch();
	afx_msg void OnKillfocusEditSearch();
	afx_msg void OnItemchangedListSymbol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCELLDEFVIEW_H__A67EB2A5_9A5F_4B6E_B895_8DBD08E49E85__INCLUDED_)
