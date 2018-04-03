#if !defined(AFX_DLGSEARCHCODEDLG_H__042152A9_A28A_4F04_B622_627BEFCD2715__INCLUDED_)
#define AFX_DLGSEARCHCODEDLG_H__042152A9_A28A_4F04_B622_627BEFCD2715__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSearchCodeDlg.h : header file
//

#include "Scheme.h"
#include "CollectionTreeCtrl.h"
#include "../mm3dPrj/MyListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSearchCodeDlg dialog


class CDlgSearchCodeDlg : public CDialog
{
// Construction
public:
	CDlgSearchCodeDlg(CWnd* pParent = NULL);   // standard constructor

	void SetScheme(CScheme *pSchme, int nScale);
	BOOL FillTree();
	BOOL CreateCtrls();
	void FillSymbolList();
	void PreviewGroup(CString groupName);
	void CreateImageList(int cx, int cy, int nsize);
	BOOL CreateImageItem(int idx);
	void DrawImageItem(int idx, int cx, int cy, const GrBuffer2d *pBuf, COLORREF col=0, COLORREF colBak=RGB(255,255,255));
	CString GetSelName() { return m_strSelLayer; };
// Dialog Data
	//{{AFX_DATA(CDlgSearchCodeDlg)
	enum { IDD = IDD_SEARCHCODE_DIALOG };
	CMyListCtrl	m_wndListSymbol;
	//}}AFX_DATA
	CCollectionTreeCtrl m_wndIdxTree;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSearchCodeDlg)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSearchCodeDlg)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSelchangeSymbolList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDClickSymbolList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CScheme *m_pScheme;
	int		m_nScale;
	USERIDX m_UserIdx;
	CString m_strGroupName;

	CString m_strSelLayer;

	//SymbolList
	CImageList m_listImages;
	CArray<int,int> m_arrIdxCreateFlag;
	int m_nIdxToCreateImage, m_nImageWid, m_nImageHei;

	CBrush m_brush;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSEARCHCODEDLG_H__042152A9_A28A_4F04_B622_627BEFCD2715__INCLUDED_)
