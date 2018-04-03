#if !defined(AFX_DLGSELECTSHIFT_H__61A6FAC1_005E_4D56_9075_D4A535AF6662__INCLUDED_)
#define AFX_DLGSELECTSHIFT_H__61A6FAC1_005E_4D56_9075_D4A535AF6662__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelectShift.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CMulitLineListBox window
typedef struct _LISTBOX_COLOR_
{
	CString strText;
	COLORREF fgColor;
	COLORREF bgColor;
	_LISTBOX_COLOR_()
	{
		strText.Empty();
		fgColor = RGB(0, 0, 0);
		bgColor = RGB(255, 255, 255);
	}
}LISTBOX_COLOR, *PLISTBOX_COLOR;

class CFocusColorListBox : public CListBox
{
// Construction
public:
	CFocusColorListBox();

// Attributes
public:
	void AppendString(LPCSTR lpszText, COLORREF fgColor, COLORREF bgColor);

protected:
	int m_nCurFocusItem;
// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFocusColorListBox)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFocusColorListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFocusColorListBox)
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


class CDlgDoc;
class EditBase::CFeature;



class CNoDblClkButton_copy : public CMFCButton
{
	DECLARE_DYNCREATE(CNoDblClkButton_copy)
public:
	CNoDblClkButton_copy() :CMFCButton(){};
protected:
	//{{AFX_MSG(CNoDblClkButton_copy)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectShift dialog

class CDlgSelectShift : public CDialog
{
// Construction
public:
	CDlgSelectShift(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSelectShift)
	enum { IDD = IDD_SELECT_SHIFT };
	CFocusColorListBox	m_wndList;
	//}}AFX_DATA

	CString m_strBtnLabel;

	CDlgDoc *m_pDoc;
	CFtrArray m_arrFtrs;
	int m_nCurFtrIndex;
	CRect m_rcClientInit;
	BOOL m_bExpand;

	CNoDblClkButton_copy m_btnExpand;

	void UpdateButton();
	void UpdateList();
	void SetViewFocus();

	void ShowExpand(BOOL bExpand);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectShift)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelectShift)
	afx_msg void OnButtonShift();
	afx_msg void OnButtonExpand();
	afx_msg void OnButtonSelectall();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListSelection();
	afx_msg LRESULT OnFocusListItem(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTSHIFT_H__61A6FAC1_005E_4D56_9075_D4A535AF6662__INCLUDED_)
