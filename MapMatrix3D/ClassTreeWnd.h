#if !defined(AFX_CLASSTREEWND_H__0A4CA63A_09F5_4754_BF18_A39DB7582E95__INCLUDED_)
#define AFX_CLASSTREEWND_H__0A4CA63A_09F5_4754_BF18_A39DB7582E95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClassTreeWnd.h : header file
//
#include "editbasedoc.h"
/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx0 window

class CTreeCtrlEx0 : public CTreeCtrl
{
	DECLARE_DYNAMIC(CTreeCtrlEx0)
public:
	CTreeCtrlEx0();
	virtual ~CTreeCtrlEx0();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	DECLARE_MESSAGE_MAP()
};

#define TREELBTNDOWN   WM_USER+2211
#define TREELBTNUP     WM_USER+2212
#define TREELBTNDBCLK  WM_USER+2213
class CClassTreeWndLayer : public CTreeCtrlEx0
{
public:
	enum MBTNDOWMPOS
	{
		NONE,
		BUTTON,
		CHECKBOX,
        ICON,
		LABEL
	};
public:
	CClassTreeWndLayer()
	{

	}
	~CClassTreeWndLayer()
	{
	}

	MBTNDOWMPOS getMBtnDownPos()
	{
		return mpos;
	}
	int getSelCount()
	{
		if(m_selitems.GetSize()>0)
		   return m_selitems.GetSize();
		else if(GetSelectedItem() != NULL)
			return 1;
		else
			return 0;
		   
	}
	HTREEITEM getSelitem(int index)
	{
		if(m_selitems.GetSize()>0)
		   return m_selitems[index];
		else if(GetSelectedItem() != NULL && index==0)
			return GetSelectedItem();
		else 
			return NULL;
	}

	HTREEITEM getCurrentItem()
	{
		return m_current_item;
	}
private:
	
private:
    MBTNDOWMPOS mpos;
	CArray<HTREEITEM,HTREEITEM> m_selitems;
	HTREEITEM m_current_item;
	void ClearSelection();//清除所有项
protected:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

class CClassTreeWndProject : public CTreeCtrlEx0
{
// Construction
public:
	//##ModelId=41466B7F01C5
	CClassTreeWndProject();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassTreeWndProject)
	protected:
	//##ModelId=41466B7F01C6
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=41466B7F01D4
	virtual ~CClassTreeWndProject();

	// Generated message map functions
protected:
	//{{AFX_MSG(CClassTreeWndProject)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CClassTreeWndGeometry : public CTreeCtrlEx0
{
// Construction
public:
	//##ModelId=41466B7F01C5
	CClassTreeWndGeometry();

// Attributes
public:
	CDlgDataSource *m_pData;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassTreeWndGeometry)
	protected:
	//##ModelId=41466B7F01C6
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=41466B7F01D4
	virtual ~CClassTreeWndGeometry();

	// Generated message map functions
protected:
	//{{AFX_MSG(CClassTreeWndGeometry)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CClassTreeWndCollection : public CTreeCtrlEx0
{
// Construction
public:
	//##ModelId=41466B7F01C5
	CClassTreeWndCollection();

// Attributes
public:
	CDlgDataSource *m_pData;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassTreeWndCollection)
	protected:
	//##ModelId=41466B7F01C6
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=41466B7F01D4
	virtual ~CClassTreeWndCollection();

	// Generated message map functions
protected:
	//{{AFX_MSG(CClassTreeWndCollection)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

class CClassTreeWndSymbolLib : public CTreeCtrlEx0
{
// Construction
public:
	//##ModelId=41466B7F01C5
	CClassTreeWndSymbolLib();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassTreeWndSymbolLib)
	protected:
	//##ModelId=41466B7F01C6
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=41466B7F01D4
	virtual ~CClassTreeWndSymbolLib();

	// Generated message map functions
protected:
	//{{AFX_MSG(CClassTreeWndSymbolLib)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLASSTREEWND_H__0A4CA63A_09F5_4754_BF18_A39DB7582E95__INCLUDED_)
