// DlgSlider.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSlider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSlider dialog


CDlgSlider::CDlgSlider(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSlider::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSlider)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSlider::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSlider)
	DDX_Control(pDX, IDC_SLIDER_SACLE, m_ctrlSlider);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSlider, CDialog)
	//{{AFX_MSG_MAP(CDlgSlider)
		ON_WM_VSCROLL()
	ON_WM_NCACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSlider message handlers
BOOL CDlgSlider::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctrlSlider.SetRange(0,m_arrCanZoomRate.GetSize(),TRUE);
	m_ctrlSlider.SetPos(m_nPos);

	CRect rect0,rect;
	GetWindowRect(&rect0);	
	
	rect.bottom = m_ShowPos.y;
	rect.top = rect.bottom - rect0.Height();
	
	rect.left = m_ShowPos.x;
	rect.right = rect.left + rect0.Width();

	MoveWindow(&rect);

	//SetCapture();

	return TRUE;
}
void CDlgSlider::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{

	if( m_updateMsg.hwnd )
	{
		m_nPos = m_ctrlSlider.GetPos();
		::PostMessage(m_updateMsg.hwnd,m_updateMsg.message,m_nPos,m_updateMsg.lParam);
	}
	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDlgSlider::SetService(HWND hWnd, UINT msgId, WPARAM wParam, LPARAM lParam)
{
	m_updateMsg.hwnd   = hWnd;
	m_updateMsg.message= msgId;
	m_updateMsg.wParam = wParam;
	m_updateMsg.lParam = lParam;
}

BOOL CDlgSlider::OnNcActivate( BOOL bActive )
{
	if (!bActive)
	{
		PostMessage(WM_CLOSE, NULL, NULL);
	}
	return CDialog::OnNcActivate (bActive);
}

