// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "EditBase.h"
#include "ChildFrm.h"
#include "editbasedoc.h"
#include "DlgSelectShift.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	//{{AFX_MSG_MAP(CChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVING()
	ON_WM_SETFOCUS()
	ON_WM_NCCALCSIZE()
//	ON_COMMAND(ID_WINDOW_TILE_VERT, &CChildFrame::OnWindowTileVert)
	ON_WM_NCACTIVATE()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if (!CMDIChildWndEx::PreCreateWindow(cs))
		return FALSE;
	
	/*cs.style &= ~(LONG)FWS_ADDTOTITLE;*/
	//cs.style &= ~WS_SYSMENU;
	//cs.style &= ~WS_HSCROLL;
	//cs.style &= ~WS_VSCROLL;

	return TRUE;
}

void CChildFrame::OnSize(UINT nType, int cx, int cy)
{
	RecalcLayout(TRUE);
	MoveDlgSelectShift();
	CMDIChildWndEx::OnSize(nType, cx, cy);
}


void CChildFrame::OnMoving(UINT fwSide, LPRECT pRect)
{
	MoveDlgSelectShift();
	CMDIChildWndEx::OnMoving(fwSide, pRect);
}

void CChildFrame::OnSetFocus(CWnd* pOldWnd)
{
	MoveDlgSelectShift();
	CMDIChildWndEx::OnSetFocus(pOldWnd);
}

void CChildFrame::MoveDlgSelectShift()
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (pDoc && pDoc->m_pDlgSelectShift)
	{
		if (pDoc->m_pDlgSelectShift->IsWindowVisible())
		{
			CView *pView = GetActiveView();
			if (!pView) return;
			CRect rcClient, rcDlg;
			::GetWindowRect(pView->GetSafeHwnd(), rcClient);
			pDoc->m_pDlgSelectShift->GetWindowRect(&rcDlg);
			rcDlg.OffsetRect(rcClient.left - rcDlg.left, rcClient.top - rcDlg.top);
			pDoc->m_pDlgSelectShift->MoveWindow(&rcDlg);
			pView->SetFocus();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}

#endif //_DEBUG

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	/*ModifyStyle(WS_CAPTION | WS_SYSMENU, 0, SWP_FRAMECHANGED);*/

	RecalcLayout(TRUE);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

void CChildFrame::ActivateFrame(int nCmdShow) 
{
	//nCmdShow = SW_MAXIMIZE;
	CMDIChildWndEx::ActivateFrame(nCmdShow);
}

void CChildFrame::MDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIChildWndEx::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
}

void CChildFrame::RecalcLayout (BOOL bNotify)
{
	CMDIChildWndEx::RecalcLayout (bNotify);
	m_dockManager.RecalcLayout (bNotify);
	
	CView* pView = GetActiveView ();
	if (pView != NULL && pView->IsKindOf (RUNTIME_CLASS (CPreviewView)) && 
		m_dockManager.IsPrintPreviewValid ())
	{
		CRect rectClient = m_dockManager.GetClientAreaBounds ();
		pView->SetWindowPos (NULL, rectClient.left, rectClient.top, 
			rectClient.Width (), rectClient.Height (),
			SWP_NOZORDER  | SWP_NOACTIVATE);
	}
	else
	{
		CObList lstBars;
		m_dockManager.GetPaneList(lstBars);
		//if( !lstBars.IsEmpty() );
		//	AdjustClientArea ();
	}
}

void CChildFrame::AdjustDockingLayout (HDWP hdwp)
{
	ASSERT_VALID (this);
	
	if (m_dockManager.IsInAdjustLayout ())
	{	
		return;
	}
	
	m_dockManager.AdjustDockingLayout (hdwp);
	
	CObList lstBars;
	m_dockManager.GetPaneList(lstBars);
	//if( !lstBars.IsEmpty() );
	//	AdjustClientArea ();
}

void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);
	
	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!
	
	CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle)
	{
		TCHAR szText[256+_MAX_PATH];
		if (pDocument == NULL)
			lstrcpy(szText, m_strTitle);
		else
			lstrcpy(szText, pDocument->GetTitle());

		if (m_nWindow == 0)
			SetWindowText(szText);
		else if (m_nWindow > 0)
			wsprintf(szText + lstrlen(szText), _T(":%d"), m_nWindow);
		
		if( m_strTitle.GetLength()>0 )
		{
			wsprintf(szText + lstrlen(szText), _T(" - %s"), m_strTitle);
		}
		
		// set title if changed, but don't remove completely
		AfxSetWindowText(m_hWnd, szText);
	}
}

void CChildFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default
	CMDIChildWndEx::OnNcCalcSize(bCalcValidRects, lpncsp);
}



BOOL CChildFrame::OnNcActivate(BOOL bActive)
{
	// TODO: Add your message handler code here and/or call default

	return FALSE;/*CMDIChildWndEx::OnNcActivate(bActive);*/
}


void CChildFrame::OnNcPaint()
{
	// TODO: Add your message handler code here
	// Do not call CMDIChildWndEx::OnNcPaint() for painting messages
	CMDIChildWndEx::OnNcPaint();                  
}
