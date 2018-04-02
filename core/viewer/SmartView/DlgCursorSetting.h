#if !defined(AFX_DLGCURSORSETTING_H__77979535_4B41_49F6_B6A7_D6E78CAA3788__INCLUDED_)
#define AFX_DLGCURSORSETTING_H__77979535_4B41_49F6_B6A7_D6E78CAA3788__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif

#include "resource.h"
#include "afxcolorbutton.h"
#include "CursorLayer.h"

using namespace MyNameSpaceName;

class CBmpListBox : public CListBox
{
public:
	CBmpListBox():m_nHeight(30){};
	virtual ~CBmpListBox(){};
	void SetMyHeight(int nHeight){ m_nHeight=nHeight; }
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct );
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpDrawItemStruct);

	void DrawItem(CDC *pDC, CBitmap *pBmp, CRect rect, BOOL bSelect);
protected:
	int m_nHeight;
};

class CDlgCursorSetting : public CDialog
{
	DECLARE_DYNCREATE(CDlgCursorSetting)
public:
	CDlgCursorSetting();
	virtual ~CDlgCursorSetting();
	BOOL SetCursor(CCursorLayer *pCursor);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMdiSetCursorPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMdiSetCursorPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
// Dialog Data
	//{{AFX_DATA(CMdiSetCursorPage)
	enum { IDD = IDD_PROPPAGE_SETCURSORTYPE };
	CMFCColorButton m_wndColorBtn;
	CBmpListBox	m_wndCursorList;
	//}}AFX_DATA
	int m_nCursorIdx;

protected:	
	CBitmap m_bmpItems[10];
	CCursorLayer *m_pCursor;
	int m_nItemNum;
};

#endif // !defined(AFX_DLGCURSORSETTING_H__77979535_4B41_49F6_B6A7_D6E78CAA3788__INCLUDED_)
