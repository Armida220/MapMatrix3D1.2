#if !defined(AFX_MERGETREE_H__84EB387B_8CC3_4939_A69F_AA606C81E39D__INCLUDED_)
#define AFX_MERGETREE_H__84EB387B_8CC3_4939_A69F_AA606C81E39D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MergeTree.h : header file
#include "TreeCtrlEx.h"
//

/////////////////////////////////////////////////////////////////////////////
// CMergeTree window

class CMergeTree : public CTreeCtrlEx
{
// Construction
public:
	CMergeTree();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);	
// Attributes
public:
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMergeTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMergeTree)
	
	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGETREE_H__84EB387B_8CC3_4939_A69F_AA606C81E39D__INCLUDED_)
