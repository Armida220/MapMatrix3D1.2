// LongScrollCtrl.h: interface for the CLongScrollCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LONGSCROLLCTRL_H__1CF2A7DF_1DF4_483D_BD76_3F9F842F67B8__INCLUDED_)
#define AFX_LONGSCROLLCTRL_H__1CF2A7DF_1DF4_483D_BD76_3F9F842F67B8__INCLUDED_

#include "SmartViewDef.h"


class EXPORT_SMARTVIEW CLongScrollCtrl
{	
public:
	CLongScrollCtrl();
	~CLongScrollCtrl();

public:
	void SetViewWnd(HWND hwnd);
	void AttachScrollBar(UINT nBar, CScrollBar* pBar);
	BOOL EnableScrollBar(int nSBFlags, UINT nArrowFlags=ESB_ENABLE_BOTH );
	void SetPageSize(int nBar, int step);
	void SetPos(int x,int y);
	void SetRange( int nBar,int Min,int Max);
	void SetRangeAndPos(int xPos,int yPos,CSize szClt, CRect rcRange);
	int  GetPos( int nBar );
	CSize GetRange();
	BOOL OnVScroll(UINT nSBCode,UINT nPos);
	BOOL OnHScroll(UINT nSBCode,UINT nPos);
	BOOL TryOnScroll(UINT nSBCode,UINT nPos);
	
private:
	HWND	m_hViewWnd;
	HWND    m_hHScrBar;
	HWND    m_hVScrBar;
	int		m_nVPos,m_nHPos;
	int		m_nVOrg,m_nHOrg;
	CSize   m_szActRange;
	CSize	m_szVirRange;
	int		m_nHPage, m_nVPage;
};

#endif
