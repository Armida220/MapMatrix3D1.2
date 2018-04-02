// DlgColors.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgColors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CStaticColor,CStatic)

/////////////////////////////////////////////////////////////////////////////
// CStaticColor

CStaticColor::CStaticColor()
{
	m_bHot = FALSE;
	m_color = RGB(255,255,255);
}

BEGIN_MESSAGE_MAP(CStaticColor, CStatic)
	//{{AFX_MSG_MAP(CStaticColor)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_MOUSELEAVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CStaticColor::OnLButtonDown(UINT nFlags, CPoint point)
{
	GetParent()->SendMessage(WM_USER+1,0,(LPARAM)this);
}

void CStaticColor::OnMouseMove(UINT nFlags, CPoint point)
{
	static TRACKMOUSEEVENT Track;
	Track.cbSize        = sizeof(TRACKMOUSEEVENT);
	Track.dwFlags       = TME_LEAVE;
	Track.hwndTrack     = m_hWnd;
	Track.dwHoverTime   = HOVER_DEFAULT;
	_TrackMouseEvent (&Track); 
	
	if( !m_bHot )
	{
		m_bHot = TRUE;
		RedrawWindow();
	}
}

void CStaticColor::OnMouseLeave()
{
	m_bHot = FALSE;
	RedrawWindow();
}

void CStaticColor::OnPaint()
{
	CRect rcClient;
	GetClientRect(rcClient);

	CPaintDC dc(this);

	CPen pen, *pOldPen;
	CBrush brush(m_color), *pOldBrush;

	dc.FillSolidRect(&rcClient,RGB(255,255,255));

	if( m_bHot )
	{
		pen.CreatePen(PS_SOLID,1,RGB(0,128,128));
	}
	else
	{
		rcClient.DeflateRect(1,1);
		pen.CreatePen(PS_SOLID,1,RGB(112,112,112));
	}

	pOldPen = (CPen*)dc.SelectObject(&pen);
	pOldBrush = (CBrush*)dc.SelectObject(&brush);

	rcClient.DeflateRect(1,1);

	dc.Rectangle(&rcClient);

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}


/////////////////////////////////////////////////////////////////////////////
// CDlgColors dialog


CDlgColors::CDlgColors(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgColors::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgColors)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


CDlgColors::~CDlgColors()
{
	for( int i=0; i<m_arrPBtns.GetSize(); i++)
	{
		delete m_arrPBtns[i];
	}
}


void CDlgColors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgColors)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX,IDC_BUTTON_OTHERCOLORS,m_wndOtherColors);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgColors, CDialog)
	//{{AFX_MSG_MAP(CDlgColors)
	ON_BN_CLICKED(IDC_BUTTON_OTHERCOLORS, OnButtonOthercolors)
	ON_WM_NCACTIVATE()
	ON_MESSAGE(WM_USER+1,OnClickColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgColors message handlers

void CDlgColors::OnButtonOthercolors() 
{
	CColorDialog dlg;
	if( dlg.DoModal()!=IDOK )
	{
		return;
	}

	m_RetColor = dlg.GetColor();
	
	CDialog::OnOK();
}

BOOL CDlgColors::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CPoint cursor;
	GetCursorPos(&cursor);
	CRect rcWnd;
	GetWindowRect(&rcWnd);

	rcWnd.OffsetRect(cursor.x-rcWnd.right,cursor.y-rcWnd.top);

	MoveWindow(&rcWnd);

	m_wndOtherColors.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
	m_wndOtherColors.m_bDrawFocus = FALSE;

	int nHei = 18;
	
	// TODO: Add extra initialization here
	CRect rcRect;
	CWnd *pWnd = GetDlgItem(IDC_STATIC_RECT);
	if( pWnd )
	{
		pWnd->GetWindowRect(&rcRect);
		ScreenToClient(&rcRect);

		rcRect.bottom = rcRect.top + ceil(rcRect.Height()*1.0/nHei)*nHei;

		//分成多列
		int nsz = m_Colors.GetSize();
		if( nsz>100 )nsz = 100;

		int nCol = ceil(nHei*nsz*1.0/rcRect.Height());

		if( nCol<=0 )nCol = 1;

		int nRow = ceil(m_Colors.GetSize()*1.0/nCol);

		int nWid = rcRect.Width()/nCol - 2;
		int k = 0;

		CRect rcBtn(0,0,nWid,nHei);
		for( int i=0; i<nRow; i++)
		{
			for( int j=0; j<nCol; j++)
			{
				if( k>=m_Colors.GetSize() )
					continue;

				rcBtn.SetRect(j*nWid,i*nHei,j*nWid+nWid, i*nHei+nHei);
				CStaticColor *pBtn = new CStaticColor();
				if( pBtn )
				{
					pBtn->Create("",WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcBtn,this,i+1);
					pBtn->m_color = m_Colors[k];
					k++;

					m_arrPBtns.Add(pBtn);
				}
			}
		}

		CWnd *pWnd2 = GetDlgItem(IDC_BUTTON_OTHERCOLORS);

		if( pWnd2 )
		{
			rcRect.bottom = rcRect.top + nHei*nRow;
			pWnd->MoveWindow(&rcRect);

			CRect rcBtn;

			pWnd2->GetWindowRect(&rcBtn);

			CRect rcBtn2;
			rcBtn2.left = rcRect.left;
			rcBtn2.right = rcRect.left + rcBtn.Width();
			rcBtn2.top = rcRect.bottom;
			rcBtn2.bottom = rcRect.bottom + rcBtn.Height();

			pWnd2->MoveWindow(&rcBtn2);

			rcWnd.bottom = rcWnd.top + nHei*nRow + rcBtn.Height()+6;
			
			MoveWindow(&rcWnd);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT CDlgColors::OnClickColor(WPARAM wParam, LPARAM lParam)
{
	CStaticColor *pBtn = (CStaticColor*)lParam;
	if( pBtn )
	{
		m_RetColor = pBtn->m_color;
	}

	CDialog::OnOK();
	return 0;
}


BOOL CDlgColors::OnNcActivate( BOOL bActive )
{
	if (!bActive)
	{
		PostMessage(WM_CLOSE, NULL, NULL);
	}
	return CDialog::OnNcActivate (bActive);
}