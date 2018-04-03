// StereoFrame.h: interface for the CStereoFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEREOFRAME_H__29C6A09E_5F5D_482E_8A56_28D461848168__INCLUDED_)
#define AFX_STEREOFRAME_H__29C6A09E_5F5D_482E_8A56_28D461848168__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FullScreenAssist.h"

class CStereoFrame : public CFrameWndEx 
{
	DECLARE_DYNCREATE(CStereoFrame)
public:
	CStereoFrame();           // protected constructor used by dynamic creation
	BOOL IsFullScreen();
	// Attributes
public:
	
private:
	CFullScreenAssist m_fsAssist;
	
	// Operations
public:
	virtual void RecalcLayout (BOOL bNotify = TRUE);
	virtual void AdjustDockingLayout (HDWP hdwp = NULL);
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStereoFrame)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);		
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	
	// Implementation
protected:	
	virtual ~CStereoFrame();
	
	// Generated message map functions
	//{{AFX_MSG(CStereoFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFullscreen();
	afx_msg void OnUpdateFullscreen(CCmdUI* pCmdUI);
	afx_msg LRESULT OnGetCustomState(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetCustomState(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_STEREOFRAME_H__29C6A09E_5F5D_482E_8A56_28D461848168__INCLUDED_)
