// UIFStatusBarEx.h: interface for the CUIFStatusBarEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIFSTATUSBAREX_H__1F2A0EB1_79F6_442C_9FB3_9730847E86A9__INCLUDED_)
#define AFX_UIFSTATUSBAREX_H__1F2A0EB1_79F6_442C_9FB3_9730847E86A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUIFStatusBarEx : public CMFCStatusBar
{
	DECLARE_DYNAMIC(CUIFStatusBarEx)
public:
	CUIFStatusBarEx();
	virtual ~CUIFStatusBarEx();
//	long GetPaneProgressTotalLen(int nIndex) const;
	void SetProgressStepUpdateTimes(int times);
	void SetPaneProgressEx (int nIndex, long nCurr);
	void EnablePaneProgressBarEx (int nIndex, long nTotal = 100 /* -1 - disable */,
								BOOL bDisplayText = FALSE, /* display "x%" */
								COLORREF clrBar = -1, COLORREF clrBarDest = -1 /* for gradient fill */,
								COLORREF clrProgressText = -1);
	
protected:
	int m_nStepUpdateTimes;
	int m_nCurUpdateLen;
	
protected:
	//{{AFX_MSG(CUIFStatusBarEx)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_UIFSTATUSBAREX_H__1F2A0EB1_79F6_442C_9FB3_9730847E86A9__INCLUDED_)
