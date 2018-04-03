// StereoFrame.cpp: implementation of the CStereoFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "StereoFrame.h"
#include "ExMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CStereoFrame, CFrameWndEx)
CStereoFrame::CStereoFrame()
{

}

CStereoFrame::~CStereoFrame()
{

}

BEGIN_MESSAGE_MAP(CStereoFrame, CFrameWndEx)
	//{{AFX_MSG_MAP(CStereoFrame)
	ON_WM_CREATE()
	ON_COMMAND(IDC_BUTTON_FULLSCREEN, OnFullscreen)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_FULLSCREEN, OnUpdateFullscreen)
	ON_MESSAGE(FCCM_GETCUSTOMSTATE,OnGetCustomState)
	ON_MESSAGE(FCCM_SETCUSTOMSTATE,OnSetCustomState)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStereoFrame message handlers

BOOL CStereoFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.hwndParent = AfxGetMainWnd()->m_hWnd;
	cs.hMenu = NULL;
	return CFrameWndEx::PreCreateWindow(cs);
}

BOOL CStereoFrame::IsFullScreen()
{
	return m_fsAssist.IsFullScreen();
}

int CStereoFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_fsAssist.Attach(this);
	RecalcLayout(TRUE);
	return 0;
}



LRESULT CStereoFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case WM_MOVING:
	case WM_MOVE:	
	case WM_WINDOWPOSCHANGING:
	case WM_WINDOWPOSCHANGED:
		{
			CView *pView = GetActiveView();
			if( pView )pView->SendMessage(message, wParam, lParam);
		}
	}
	return CFrameWnd::DefWindowProc(message, wParam, lParam);
}


void CStereoFrame::OnFullscreen() 
{
	m_fsAssist.FullScreen(!m_fsAssist.IsFullScreen());
}

void CStereoFrame::OnUpdateFullscreen(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_fsAssist.IsFullScreen());
}

void CStereoFrame::RecalcLayout (BOOL bNotify)
{
	CFrameWnd::RecalcLayout (bNotify);
}

BOOL CStereoFrame::PreTranslateMessage(MSG* pMsg) 
{	
	BOOL ret = CFrameWndEx::PreTranslateMessage(pMsg);
	if( !ret )ret = AfxGetMainWnd()->PreTranslateMessage(pMsg);
	return ret;
}

BOOL CStereoFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	if (!CFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}
	return TRUE;
}



void CStereoFrame::AdjustDockingLayout (HDWP hdwp)
{
	ASSERT_VALID (this);
	
	if (m_dockManager.IsInAdjustLayout ())
	{
		return;
	}
	
	m_dockManager.AdjustDockingLayout (hdwp);
	
	//AdjustClientArea ();
	if (m_dockManager.IsOLEContainerMode ())
	{
		RecalcLayout ();
	}
}

LRESULT CStereoFrame::OnGetCustomState(WPARAM wParam, LPARAM lParam)
{
	if( lParam )
	{
		CUSTOM_STATE *state = (CUSTOM_STATE*)lParam;
		state->dataLen = sizeof(BOOL);

		BOOL bFull = m_fsAssist.IsFullScreen();
		memcpy(state->data,&bFull,sizeof(BOOL));
	}
	return 0;
}


LRESULT CStereoFrame::OnSetCustomState(WPARAM wParam, LPARAM lParam)
{
	if( lParam )
	{
		CUSTOM_STATE *state = (CUSTOM_STATE*)lParam;
		if( state->dataLen==sizeof(BOOL) )
		{
			BOOL bFull = FALSE;
			memcpy(&bFull,state->data,sizeof(BOOL));
			m_fsAssist.FullScreen(bFull);
		}
	}
	return 0;
}
