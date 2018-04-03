#if !defined(AFX_UNDOBAR_H__F88FEBBF_A4D0_4BEC_80DE_0CC1D1C8B073__INCLUDED_)
#define AFX_UNDOBAR_H__F88FEBBF_A4D0_4BEC_80DE_0CC1D1C8B073__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UndoBar.h : header file
//

#include "UndoListBox.h"

#include "baseview.h"

class CUndoButton;

/////////////////////////////////////////////////////////////////////////////
// CUndoBar window

//##ModelId=41466B7A0108
class CUndoBar : public CMFCPopupMenuBar
{
	DECLARE_SERIAL(CUndoBar)

// Construction
public:
	//##ModelId=41466B7A010A
	CUndoBar();

// Attributes
protected:
	//##ModelId=41466B7A0119
	CUndoListBox	m_wndList;
	//##ModelId=41466B7A0127
	CRect			m_rectLabel;
	//##ModelId=41466B7A028F
	int				m_nLabelHeight;
	//##ModelId=41466B7A029E
	CString			m_strLabel;

// Operations
public:
	//##ModelId=41466B7A029F
	void DoUndo ();
	//##ModelId=41466B7A02AE
	void SetLabel (const CString& strLabel);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUndoBar)
	//}}AFX_VIRTUAL

	//##ModelId=41466B7A02B0
	virtual void OnFillBackground (CDC* pDC);
	//##ModelId=41466B7A02B3
	virtual CSize CalcSize (BOOL bVertDock);

// Implementation
public:
	//##ModelId=41466B7A02C0
	virtual ~CUndoBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUndoBar)
	//##ModelId=41466B7A02C2
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//##ModelId=41466B7A02D0
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//##ModelId=41466B7A02DD
	CUndoButton* GetParentButton () const;
};

//////////////////////////////////////////////////////////////////////////////
// CUndoMenu

//##ModelId=41466B7A02DF
class CUndoMenu : public CMFCPopupMenu
{
	DECLARE_SERIAL(CUndoMenu)

	//##ModelId=41466B7A02ED
	virtual CMFCPopupMenuBar* GetMenuBar ()
	{
		return &m_wndUndoBar;
	}

	//##ModelId=41466B7A02FD
	CUndoBar	m_wndUndoBar;

    // Generated message map functions
protected:
    //{{AFX_MSG(CUndoMenu)
	//##ModelId=41466B7A0301
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////
// CUndoButton

//##ModelId=41466B7A031B
class CUndoButton : public CMFCToolBarMenuButton
{
	friend class CUndoBar;

	DECLARE_SERIAL(CUndoButton)

public:
	//##ModelId=41466B7A031D
	CUndoButton()
	{
	}
	//##ModelId=41466B7A031E
	CUndoButton(UINT uiCmdID, LPCTSTR lpszText ) :
		CMFCToolBarMenuButton(uiCmdID, NULL,
		GetCmdMgr()->GetCmdImage(uiCmdID, FALSE), lpszText)
	{
		m_nSelNum = 0;
	}


	//##ModelId=41466B7A032D
	CStringList			m_lstActions;
	//##ModelId=41466B7A033B
	int			        m_nSelNum;

	//##ModelId=41466B7A033C
	int GetSelNum ()
	{
		return m_nSelNum;
	}

	//##ModelId=41466B7A033D
	void ResetSelNum ()
	{
		m_nSelNum = 0;
	}

protected:
	//##ModelId=41466B7A034A
	virtual CMFCPopupMenu* CreatePopupMenu ();

	//##ModelId=41466B7A034C
	virtual BOOL IsEmptyMenuAllowed () const
	{
		return TRUE;
	}

	//##ModelId=41466B7A034E
	virtual void OnChangeParentWnd (CWnd* pWndParent)
	{
		CMFCToolBarMenuButton::OnChangeParentWnd (pWndParent);
		m_bDrawDownArrow = TRUE;
	}
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNDOBAR_H__F88FEBBF_A4D0_4BEC_80DE_0CC1D1C8B073__INCLUDED_)
