#if !defined(AFX_UNDOLISTBOX_H__3CB08857_F7B9_4B5E_B019_D2E748A2CCD4__INCLUDED_)
#define AFX_UNDOLISTBOX_H__3CB08857_F7B9_4B5E_B019_D2E748A2CCD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UndoListBox.h : header file
//

class CUndoBar;

/////////////////////////////////////////////////////////////////////////////
// CUndoListBox window

//##ModelId=41466B7A00CA
class CUndoListBox : public CListBox
{
// Construction
public:
	//##ModelId=41466B7A00CC
	CUndoListBox(CUndoBar& bar);

// Attributes
protected:
	//##ModelId=41466B7A00DA
	CUndoBar&	m_Bar;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUndoListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=41466B7A00DE
	virtual ~CUndoListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUndoListBox)
	//##ModelId=41466B7A00E0
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//##ModelId=41466B7A00EB
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNDOLISTBOX_H__3CB08857_F7B9_4B5E_B019_D2E748A2CCD4__INCLUDED_)
