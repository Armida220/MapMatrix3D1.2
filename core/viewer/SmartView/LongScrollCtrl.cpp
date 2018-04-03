#include "stdafx.h"
#include "LongScrollCtrl.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CLongScrollCtrl::CLongScrollCtrl() : m_szVirRange(0,0), m_szActRange(0,0)
{
	m_hViewWnd	= NULL;
	m_nHPage= m_nVPage = 10;
	m_nHPos = 0; 
	m_nVPos = 0; 
	m_nHOrg = 0;
	m_nVOrg = 0;

	m_hHScrBar= NULL;
	m_hVScrBar= NULL;
}

CLongScrollCtrl::~CLongScrollCtrl()
{
}


void CLongScrollCtrl::SetRangeAndPos(int xPos,int yPos,CSize szClt, CRect rcRange)
{
	SetRange(SB_HORZ,rcRange.left,rcRange.right);
	SetRange(SB_VERT,rcRange.top,rcRange.bottom);

	SetPageSize(SB_HORZ,szClt.cx);
	SetPageSize(SB_VERT,szClt.cy);

	SetPos(xPos,yPos);
}



void CLongScrollCtrl::SetPageSize(int nBar, int step)
{
	SCROLLINFO info;
	info.fMask = SIF_PAGE;
	info.cbSize = sizeof(info);
	
	if( nBar==SB_HORZ )
	{
		if( m_szActRange.cx>0 )
		{
			m_nHPage = step;
			info.nPage = m_nHPage;

			if( step>=m_szActRange.cx )
			{
				m_nHPage = 0;
				EnableScrollBar(nBar,ESB_DISABLE_BOTH);
			}
			else
			{
				if( m_hHScrBar )
				{
					CScrollBar *pBar = (CScrollBar*)CScrollBar::FromHandle(m_hHScrBar);
					if( pBar )pBar->SetScrollInfo(&info);
				}
				else
				{
					if( m_hViewWnd )::SetScrollInfo(m_hViewWnd,nBar,&info,TRUE);
				}
			}
		}
		else
		{
			m_nHPage = 0;
		}
	}
	
	if( nBar==SB_VERT )
	{
		if( m_szActRange.cy>0 )
		{
			m_nVPage = step;
			info.nPage = m_nVPage;
			if( step>=m_szActRange.cy )
			{
				m_nVPage = 0;
				EnableScrollBar(nBar,ESB_DISABLE_BOTH);			
			}
			else
			{
				if( m_hVScrBar )
				{				
					CScrollBar *pBar = (CScrollBar*)CScrollBar::FromHandle(m_hVScrBar);
					if( pBar )pBar->SetScrollInfo(&info);
				}
				else 
				{
					if( m_hViewWnd )::SetScrollInfo(m_hViewWnd,nBar,&info,TRUE);
				}
			}
		}
		else
		{
			m_nVPage = 0;
		}
	}
}

BOOL CLongScrollCtrl::OnVScroll(UINT nSBCode,UINT nPos)
{
	int pagesize = m_nVPage/6;
	int linesize = 1;
	if( linesize<m_nVPage/10 )linesize = m_nVPage/10;
	switch( nSBCode )
	{
	case SB_THUMBPOSITION:
		m_nVPos = (m_szVirRange.cy-m_nVPage) * (float)nPos/(m_szActRange.cy-m_nVPage) + m_nVOrg;
		break;
	case SB_PAGEDOWN:
		if( m_nVPos+pagesize>m_szVirRange.cy-m_nVPage + m_nVOrg )
		{
			if( m_nVPos>=m_szVirRange.cy-m_nVPage + m_nVOrg )
				return FALSE;
			else
			{
				m_nVPos = m_szVirRange.cy-m_nVPage + m_nVOrg;
				break;
			}
		}
		m_nVPos += pagesize;
		break;
	case SB_PAGEUP:
		if( m_nVPos-pagesize<m_nVOrg )
		{
			if( m_nVPos<=m_nVOrg )
				return FALSE;
			else
			{
				m_nVPos = m_nVOrg;
				break;
			}
		}
		m_nVPos -= pagesize;
		break;
	case SB_LINEUP:
		m_nVPos -= linesize;
		break;
	case SB_LINEDOWN:
		if( m_nVPos+linesize>m_szVirRange.cy-m_nVPage + m_nVOrg )
			return FALSE;
		m_nVPos += linesize;
		break;
	default:
		return FALSE;
	}
	
	if( m_nVPos>m_szVirRange.cy-m_nVPage + m_nVOrg )m_nVPos = m_szVirRange.cy-m_nVPage + m_nVOrg;
	if( m_nVPos<m_nVOrg )m_nVPos = m_nVOrg;
	
	int nActPos = (unsigned int)((float)(m_szActRange.cy-m_nVPage)/(m_szVirRange.cy-m_nVPage)*(m_nVPos-m_nVOrg));

	if( m_hVScrBar )
	{
		CScrollBar *pBar = (CScrollBar*)CScrollBar::FromHandle(m_hVScrBar);
		if( pBar )pBar->SetScrollPos( nActPos );
	}
	else if( m_hViewWnd )
		::SetScrollPos( m_hViewWnd,SB_VERT,nActPos,TRUE );
	return TRUE;
}

BOOL CLongScrollCtrl::OnHScroll(UINT nSBCode,UINT nPos)
{
	int pagesize = m_nHPage/6;
	int linesize = 1;
	if( linesize<m_nHPage/10 )linesize = m_nHPage/10;
	switch( nSBCode )
	{
	case SB_THUMBPOSITION:
		m_nHPos = (m_szVirRange.cx-m_nHPage) * (float)nPos/(m_szActRange.cx-m_nHPage) + m_nHOrg;
		break;
	case SB_PAGEDOWN:
		if( m_nHPos+pagesize>m_szVirRange.cx-m_nHPage + m_nHOrg )
		{
			if( m_nHPos>=m_szVirRange.cx-m_nHPage + m_nHOrg )
				return FALSE;
			else
			{
				m_nHPos = m_szVirRange.cx-m_nHPage+m_nHOrg;
				break;
			}
		}
		m_nHPos += pagesize;
		break;
	case SB_PAGEUP:
		if( m_nHPos-pagesize<m_nHOrg )
		{
			if( m_nHPos<=m_nHOrg )
				return FALSE;
			else
			{
				m_nHPos = m_nHOrg;
				break;
			}
		}
		m_nHPos -= pagesize;
		break;
	case SB_LINEUP:
		m_nHPos -= linesize;
		break;
	case SB_LINEDOWN:
		if( m_nHPos+linesize>m_szVirRange.cx-m_nHPage + m_nHOrg )
			return FALSE;
		m_nHPos += linesize;
		break;
	default:
		return FALSE;
	}
	
	if( m_nHPos>m_szVirRange.cx-m_nHPage + m_nHOrg )m_nHPos = m_szVirRange.cx-m_nHPage+m_nHOrg;
	if( m_nHPos<m_nHOrg )m_nHPos = m_nHOrg;

	int nActPos = (unsigned)((float)(m_szActRange.cx-m_nHPage)/(m_szVirRange.cx-m_nHPage)*(m_nHPos-m_nHOrg));

	if( m_hHScrBar )
	{
		CScrollBar *pBar = (CScrollBar*)CScrollBar::FromHandle(m_hHScrBar);
		if( pBar )pBar->SetScrollPos( nActPos );
	}
	else if( m_hViewWnd )
		::SetScrollPos( m_hViewWnd,SB_HORZ,nActPos,TRUE );
	return TRUE;
}

BOOL CLongScrollCtrl::TryOnScroll(UINT nSBCode,UINT nPos)
{
	switch( nSBCode )
	{
	case SB_THUMBPOSITION:
	case SB_PAGEDOWN:
	case SB_PAGEUP:
	case SB_LINEUP:
	case SB_LINEDOWN:
		return TRUE;
	default:
		return FALSE;
	}
}

void CLongScrollCtrl::SetViewWnd(HWND hwnd)
{
	m_hViewWnd = hwnd;
}

void CLongScrollCtrl::AttachScrollBar(UINT nBar, CScrollBar* pBar)
{
	if( nBar==SB_HORZ )
	{
		m_hHScrBar = pBar->GetSafeHwnd();
	}
	else if( nBar==SB_VERT )
	{
		m_hVScrBar = pBar->GetSafeHwnd();
	}
}

int CLongScrollCtrl::GetPos( int nBar )
{	
	if( nBar==SB_HORZ )return m_nHPos;
	if( nBar==SB_VERT )return m_nVPos;
	return -1;
}

void CLongScrollCtrl::SetRange( int nBar, int nMin, int nMax )
{
	if( nBar==SB_HORZ )
	{
		m_nVOrg = nMin;	
		m_szVirRange.cx = nMax-nMin;
		if( m_szVirRange.cx<0 )m_szVirRange.cx = 0;
		m_szActRange.cx = m_szVirRange.cx;

		if( m_szActRange.cx<=0 )
		{
			EnableScrollBar(nBar,ESB_DISABLE_BOTH);	
		}
		else
		{
			if( m_szActRange.cx>32000 )
				m_szActRange.cx = 32000;
			if( m_hHScrBar )
			{
				CScrollBar *pBar = (CScrollBar*)CScrollBar::FromHandle(m_hHScrBar);
				pBar->EnableScrollBar(ESB_ENABLE_BOTH);
				pBar->SetScrollRange( 0,m_szActRange.cx,TRUE);
			}
			else if( m_hViewWnd )
			{
				::EnableScrollBar(m_hViewWnd,nBar,ESB_ENABLE_BOTH);
				::SetScrollRange(m_hViewWnd,nBar,0,m_szActRange.cx,TRUE);
			}
		}
	}
	else if( nBar==SB_VERT )
	{
		m_nHOrg = nMin;	
		m_szVirRange.cy = nMax-nMin;
		if( m_szVirRange.cy<0 )m_szVirRange.cy = 0;
		m_szActRange.cy = m_szVirRange.cy;

		if( m_szActRange.cy<=0 )
		{
			EnableScrollBar(nBar,ESB_DISABLE_BOTH);	
		}
		else
		{
			if( m_szActRange.cy>32000 )
				m_szActRange.cy = 32000;
			if( m_hVScrBar )
			{
				CScrollBar *pBar = (CScrollBar*)CScrollBar::FromHandle(m_hVScrBar);
				pBar->EnableScrollBar(ESB_ENABLE_BOTH);
				pBar->SetScrollRange( 0,m_szActRange.cy,TRUE);
			}
			else if( m_hViewWnd )
			{
				::EnableScrollBar(m_hViewWnd,nBar,ESB_ENABLE_BOTH);
				::SetScrollRange(m_hViewWnd,nBar,0,m_szActRange.cy,TRUE);
			}
		}
	}
}

void CLongScrollCtrl::SetPos(int xPos,int yPos)
{
	m_nHPos = xPos; m_nVPos = yPos;
	
	if( m_nHPos>m_nHOrg+m_szVirRange.cx-m_nHPage )m_nHPos = m_nHOrg+m_szVirRange.cx-m_nHPage;
	if( m_nHPos<m_nHOrg )m_nHPos = m_nHOrg;

	if( m_nVPos>m_nVOrg+m_szVirRange.cy-m_nVPage )m_nVPos = m_nVOrg+m_szVirRange.cy-m_nVPage;
	if( m_nVPos<m_nVOrg )m_nVPos = m_nVOrg;

	if( m_szActRange.cx>0 && m_nHPage<m_szActRange.cx )
	{
		int nActHPos = (unsigned int)((float)(m_szActRange.cx-m_nHPage)/(m_szVirRange.cx-m_nHPage)*(m_nHPos-m_nHOrg));
		if( m_hHScrBar )
		{
			CScrollBar *pBar = (CScrollBar*)CScrollBar::FromHandle(m_hHScrBar);
			if( pBar )pBar->SetScrollPos( nActHPos );
		}
		else if( m_hViewWnd )::SetScrollPos( m_hViewWnd,SB_HORZ,nActHPos,TRUE );
	}
	else
	{
		EnableScrollBar(SB_HORZ,ESB_DISABLE_BOTH);	
	}

	if( m_szActRange.cy>0 && m_nVPage<m_szActRange.cy )
	{
		int nActVPos = (unsigned int)((float)(m_szActRange.cy-m_nVPage)/(m_szVirRange.cy-m_nVPage)*(m_nVPos-m_nVOrg));
		if( m_hVScrBar )
		{
			CScrollBar *pBar = (CScrollBar*)CScrollBar::FromHandle(m_hVScrBar);
			if( pBar )pBar->SetScrollPos( nActVPos );
		}
		else if( m_hViewWnd )::SetScrollPos( m_hViewWnd,SB_VERT,nActVPos,TRUE );
	}
	else
	{
		EnableScrollBar(SB_VERT,ESB_DISABLE_BOTH);	
	}
}

BOOL CLongScrollCtrl::EnableScrollBar(int nBar, UINT nArrowFlags )
{
	int flag = (nArrowFlags==ESB_ENABLE_BOTH?ESB_ENABLE_BOTH:ESB_DISABLE_BOTH);
	if( nBar==SB_HORZ )
	{
		if( m_hHScrBar )
		{
			CScrollBar *pBar = (CScrollBar*)CScrollBar::FromHandle(m_hHScrBar);
			if( pBar )pBar->EnableScrollBar(flag);
		}
		else
		{
			if( m_hViewWnd )::EnableScrollBar(m_hViewWnd,nBar,flag);
		}
	}
	else
	{
		if( m_hVScrBar )
		{				
			CScrollBar *pBar = (CScrollBar*)CScrollBar::FromHandle(m_hVScrBar);
			if( pBar )pBar->EnableScrollBar(flag);
		}
		else 
		{
			if( m_hViewWnd )::EnableScrollBar(m_hViewWnd,nBar,flag);
		}
	}

	return TRUE;
}


CSize CLongScrollCtrl::GetRange()
{ 
	return m_szVirRange;
}