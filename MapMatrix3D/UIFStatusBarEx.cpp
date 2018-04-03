// UIFStatusBarEx.cpp: implementation of the CUIFStatusBarEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "UIFStatusBarEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CUIFStatusBarEx, CMFCStatusBar)

BEGIN_MESSAGE_MAP(CUIFStatusBarEx, CMFCStatusBar)
	//{{AFX_MSG_MAP(CUIFStatusBarEx)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFStatusBarEx::CUIFStatusBarEx()
{
	m_nCurUpdateLen = 0;
	m_nStepUpdateTimes = 100;
	EnablePaneDoubleClick(TRUE);
}

CUIFStatusBarEx::~CUIFStatusBarEx()
{

}

void CUIFStatusBarEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	{
		CMFCStatusBarPaneInfo* pSBP = HitTest(point);
		if (pSBP != NULL)
		{
			GetOwner()->PostMessage (WM_COMMAND, pSBP->nID);
		}
	}
	
	CMFCStatusBar::OnLButtonDown(nFlags, point);
}


void CUIFStatusBarEx::SetProgressStepUpdateTimes(int times)
{
	ASSERT(times>0);
	m_nStepUpdateTimes = times;
}


void CUIFStatusBarEx::SetPaneProgressEx (int nIndex, long nCurr)
{
	ASSERT_VALID(this);

	CMFCStatusBar::SetPaneProgress(nIndex, nCurr);

	/*CMFCStatusBarPaneInfo* pSBP = _GetPanePtr(nIndex);
	if (pSBP == NULL)
	{
		ASSERT (FALSE);
		return;
	}
	
	ASSERT (nCurr >= 0);
	//ASSERT (nCurr <= pSBP->nProgressTotal);
	
	long lPos = min (max (0, nCurr), pSBP->nProgressTotal);
	if (pSBP->nProgressCurr != lPos)
	{
		BOOL bUpdate = FALSE;
		if( lPos-m_nCurUpdateLen>=(float)pSBP->nProgressTotal/m_nStepUpdateTimes )
			bUpdate = TRUE;

		pSBP->nProgressCurr = lPos;
		if (bUpdate)
		{
			m_nCurUpdateLen = pSBP->nProgressCurr;
			InvalidatePaneContent (nIndex);
		}
	}
	else
	{
		if( m_nCurUpdateLen<lPos )
		{
			m_nCurUpdateLen = lPos;
			InvalidatePaneContent (nIndex);
		}
	}*/
}


void CUIFStatusBarEx::EnablePaneProgressBarEx(int nIndex, long nTotal, 
										   BOOL bDisplayText,
										   COLORREF clrBar, COLORREF clrBarDest,
										   COLORREF clrProgressText)
{
	ASSERT_VALID(this);

	CMFCStatusBar::EnablePaneProgressBar(nIndex, nTotal, bDisplayText, clrBar, clrBarDest, clrProgressText);
	
	/*CMFCStatusBarPaneInfo* pSBP = _GetPanePtr(nIndex);
	if (pSBP == NULL)
	{
		ASSERT (FALSE);
		return;
	}
	
	pSBP->bProgressText = bDisplayText;
	pSBP->clrProgressBar = clrBar;
	pSBP->clrProgressBarDest = clrBarDest;
	pSBP->nProgressTotal = nTotal;
	pSBP->nProgressCurr = 0;
	pSBP->clrProgressText = clrProgressText;
	
	if (clrBarDest != (COLORREF)-1 && pSBP->bProgressText)
	{
		// Progress text is not available when the gradient is ON
		ASSERT (FALSE);
		pSBP->bProgressText = FALSE;
	}
	
	m_nCurUpdateLen = 0;
	InvalidatePaneContent (nIndex);*/
}
