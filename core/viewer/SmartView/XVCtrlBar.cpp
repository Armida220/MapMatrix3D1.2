// XVCtrlBar.cpp: implementation of the CXVCtrlBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "XVCtrlBar.h"
#include "NoDblClkButton.h"
#include "GlobalFunc.h"

#include "SceneView.h"
#include "SmartViewFunctions.h"

extern BOOL LoadMyString(CString &str, UINT uID);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CTRLBTN_WID		1
#define CTRLBTN_HEI		1
#define CTRLOFF			30


extern AFX_EXTENSION_MODULE SmartViewDLL;


IMPLEMENT_DYNCREATE(CXVCtrlBar, CDialogBar)


BEGIN_MESSAGE_MAP(CXVCtrlBar, CDialogBar)
	//{{AFX_MSG_MAP(CXVCtrlBar)
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//ON_CBN_SELCHANGE(IDC_COMBO_ZOOM,OnComboZoomChange)
	ON_WM_DRAWITEM()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()


HBITMAP Copy24Bitmap(HBITMAP hBmp)
{
	BITMAP bmpinfo;
	if( !::GetObject(hBmp,sizeof(BITMAP),&bmpinfo) )
		return NULL;
	
	HDC hdc0 = ::CreateCompatibleDC(NULL);
	HDC hdc1 = ::CreateCompatibleDC(NULL);
	
	HBITMAP hNewBmp = Create24BitDIB(bmpinfo.bmWidth,bmpinfo.bmHeight,TRUE);
	
	HBITMAP hOld0 = (HBITMAP)::SelectObject(hdc0,hBmp);
	HBITMAP hOld1 = (HBITMAP)::SelectObject(hdc1,hNewBmp);
	
	for( int i=0; i<bmpinfo.bmWidth; i++)
	{
		for( int j=0; j<bmpinfo.bmHeight; j++)
		{
			COLORREF color = ::GetPixel(hdc0,i,j);			
			
			::SetPixel(hdc1,i,j,color);
		}
	}
	
	::SelectObject(hdc0, hOld0);
	::SelectObject(hdc1, hOld1);
	
	::DeleteDC(hdc0);
	::DeleteDC(hdc1);
	
	return hNewBmp;
}

HBITMAP CreateDisableBitmap(HBITMAP hBmp)
{
	BITMAP bmpinfo;
	if( !::GetObject(hBmp,sizeof(BITMAP),&bmpinfo) )
		return NULL;
	
	HDC hdc0 = ::CreateCompatibleDC(NULL);
	HDC hdc1 = ::CreateCompatibleDC(NULL);
	
	HBITMAP hNewBmp = Create24BitDIB(bmpinfo.bmWidth,bmpinfo.bmHeight,TRUE);
	
	HBITMAP hOld0 = (HBITMAP)::SelectObject(hdc0,hBmp);
	HBITMAP hOld1 = (HBITMAP)::SelectObject(hdc1,hNewBmp);
	
	for( int i=0; i<bmpinfo.bmWidth; i++)
	{
		for( int j=0; j<bmpinfo.bmHeight; j++)
		{
			COLORREF color = ::GetPixel(hdc0,i,j);

			if( color!=RGB(192,192,192) )
			{
				color = (int)(GetRValue(color)*0.33 + GetGValue(color)*0.34+ GetBValue(color)*0.33);
				color = color/2 + 128;
				if( color>255 )color = 255;
				color = RGB(color,color,color);
			}

			::SetPixel(hdc1,i,j,color);
		}
	}
	
	::SelectObject(hdc0, hOld0);
	::SelectObject(hdc1, hOld1);
	
	::DeleteDC(hdc0);
	::DeleteDC(hdc1);
	
	return hNewBmp;
}


BOOL MyCreateWindow( CWnd *pWnd, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const CRect& rect, 
	CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL)
{
	HWND hWnd = ::CreateWindow(lpszClassName,lpszWindowName,dwStyle,rect.left,rect.top,rect.Width(),rect.Height(),
		pParentWnd==NULL?NULL:pParentWnd->GetSafeHwnd(),(HMENU)nID,(HINSTANCE)SmartViewDLL.hModule,pContext);

	pWnd->SubclassWindow(hWnd);
	return (hWnd!=NULL);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXVCtrlBar::CXVCtrlBar()
{
	m_style = 0;
	CWinApp *pApp = AfxGetApp();
	if( pApp )
		m_bSliderZoom = pApp->GetProfileInt(REGPATH_USER,REGITEM_SLIDERZOOM,FALSE);
	else
		m_bSliderZoom = FALSE;

	m_hMsgWnd = NULL;
}

CXVCtrlBar::~CXVCtrlBar()
{
}

BOOL CXVCtrlBar::Create( CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID )
{
	if( Manual_Create_Window )
	{
		HINSTANCE hInst = (HINSTANCE)SmartViewDLL.hModule;
		HRSRC hResource = ::FindResource(hInst, MAKEINTRESOURCE(nIDTemplate), RT_DIALOG);
		HGLOBAL hTemplate = LoadResource(hInst, hResource);

		LPCDLGTEMPLATE lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(hTemplate);

		HWND hWnd = ::CreateDialogIndirect(hInst, lpDialogTemplate,
				pParentWnd->GetSafeHwnd(), (DLGPROC)::DefWindowProc);

		DWORD err = GetLastError();

		UnlockResource(hTemplate);

		FreeResource(hTemplate);

		SubclassWindow(hWnd);

		// dialog template MUST specify that the dialog
		//  is an invisible child window
		SetDlgCtrlID(nID);
		CRect rect;
		GetWindowRect(&rect);
		m_sizeDefault = rect.Size();    // set fixed size

		// force WS_CLIPSIBLINGS
		ModifyStyle(0, nStyle|WS_CLIPSIBLINGS);

		if (!ExecuteDlgInit(MAKEINTRESOURCE(nIDTemplate)))
			return FALSE;

		// force the size to zero - resizing bar will occur later
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);
	}
	else
	{
		if( !CDialogBar::Create(pParentWnd,nIDTemplate,
			nStyle,nID) )
			return FALSE;

		CWnd *pWnd = CWnd::FromHandlePermanent(m_hWnd);

		int a=1;
	}

	m_style = nStyle;
	return TRUE;
}

BOOL CXVCtrlBar::PreTranslateMessage(MSG* pMsg)
{
	BOOL ret = CDialogBar::PreTranslateMessage(pMsg);
	CWnd *pWnd = GetParent();
	if( pMsg->message==WM_KEYDOWN && pWnd )
	{
		if( pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd))  )
		{
			CWnd *pView = ((CFrameWnd*)pWnd)->GetActiveView();
			if( pView )pWnd = pView;
		}

		::SendMessage(pWnd->GetSafeHwnd(),pMsg->message,pMsg->wParam,pMsg->lParam);
		::SetFocus(pWnd->GetSafeHwnd());
	}

	if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONUP || pMsg->message == WM_MOUSEMOVE)
	{
		for (int i = 0; i < m_listBtn.GetSize(); i++)
		{
			CMFCButton *pBtn = (CMFCButton*)m_listBtn[i];
			CRect rect;
			pBtn->GetWindowRect(&rect);
			if (rect.PtInRect(pMsg->pt))
			{
				pBtn->GetToolTipCtrl().RelayEvent(pMsg);
			}
		}
	}
	
	return ret;
}


void CXVCtrlBar::AdjustLayout()
{
	CRect rcClient;
	GetClientRect(&rcClient);

	// align the control bar
	CRect rect = rcClient;

	CRect rectScroll = CRect(0,0,rect.Width(),rect.Height());
	if( (m_style&CBRS_TOP) || (m_style&CBRS_BOTTOM) )
	{
		rectScroll.left  = GetButtonsWidth();
		rectScroll.right-= m_sizeDefault.cy;
	}
	else
	{
		rectScroll.top = GetButtonsHeight();
	}
		
	::SetWindowPos(m_scrollBar.m_hWnd,0,
		rectScroll.left,rectScroll.top,
		rectScroll.Width(),rectScroll.Height(),
		SWP_NOZORDER);
}

LRESULT CXVCtrlBar::OnSizeParent(WPARAM wParam, LPARAM lParam)
{
	AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;
	DWORD dwStyle = RecalcDelayShow(lpLayout);

	if ((dwStyle & WS_VISIBLE) && (dwStyle & CBRS_ALIGN_ANY) != 0)
	{
		// align the control bar
		CRect rect;
		rect.CopyRect(&lpLayout->rect);

		CSize sizeAvail = rect.Size();  // maximum size available

		// get maximum requested size
		DWORD dwMode = lpLayout->bStretch ? LM_STRETCH : 0;
		if ((m_dwStyle & CBRS_SIZE_DYNAMIC) && m_dwStyle & CBRS_FLOATING)
			dwMode |= LM_HORZ | LM_MRUWIDTH;
		else if (dwStyle & CBRS_ORIENT_HORZ)
			dwMode |= LM_HORZ | LM_HORZDOCK;
		else
			dwMode |=  LM_VERTDOCK;

		CSize size = CalcDynamicLayout(-1, dwMode);

		size.cx = min(size.cx, sizeAvail.cx);
		size.cy = min(size.cy, sizeAvail.cy);

		if (dwStyle & CBRS_ORIENT_HORZ)
		{
			lpLayout->sizeTotal.cy += size.cy;
			lpLayout->sizeTotal.cx = max(lpLayout->sizeTotal.cx, size.cx);
			if (dwStyle & CBRS_ALIGN_TOP)
				lpLayout->rect.top += size.cy;
			else if (dwStyle & CBRS_ALIGN_BOTTOM)
			{
				rect.top = rect.bottom - size.cy;
				lpLayout->rect.bottom -= size.cy;
			}
		}
		else if (dwStyle & CBRS_ORIENT_VERT)
		{
			lpLayout->sizeTotal.cx += size.cx;
			lpLayout->sizeTotal.cy = max(lpLayout->sizeTotal.cy, size.cy);
			if (dwStyle & CBRS_ALIGN_LEFT)
				lpLayout->rect.left += size.cx;
			else if (dwStyle & CBRS_ALIGN_RIGHT)
			{
				rect.left = rect.right - size.cx;
				lpLayout->rect.right -= size.cx;
			}
		}
		else
		{
			ASSERT(FALSE);      // can never happen
		}

		rect.right = rect.left + size.cx;
		rect.bottom = rect.top + size.cy;

		// only resize the window if doing layout and not just rect query
		if (lpLayout->hDWP != NULL)
			AfxRepositionWindow(lpLayout, m_hWnd, &rect);

		if( ::IsWindow(m_scrollBar.m_hWnd) )
		{
			CRect rectScroll = CRect(0,0,rect.Width(),rect.Height());
			if( (m_style&CBRS_TOP) || (m_style&CBRS_BOTTOM) )
			{
				rectScroll.left  = GetButtonsWidth();
				rectScroll.right-= m_sizeDefault.cy;
			}
			else
			{
				rectScroll.top = GetButtonsHeight();
			}
		
			::SetWindowPos(m_scrollBar.m_hWnd,0,
				rectScroll.left,rectScroll.top,
				rectScroll.Width(),rectScroll.Height(),
				SWP_NOZORDER);
		}
	}
	return 0;
}

void CXVCtrlBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	const MSG *pMsg = GetCurrentMessage();
	::SendMessage(m_hMsgWnd!=NULL?m_hMsgWnd:(::GetParent(m_hWnd)),pMsg->message,pMsg->wParam,pMsg->lParam);
}

void CXVCtrlBar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	const MSG *pMsg = GetCurrentMessage();
	::SendMessage(m_hMsgWnd!=NULL?m_hMsgWnd:(::GetParent(m_hWnd)),pMsg->message,pMsg->wParam,pMsg->lParam);
}


void CXVCtrlBar::OnPaint()
{
	CPaintDC dc(this);
}

void CXVCtrlBar::OnDestroy()
{
	int num = m_listCombo.GetSize();
	for( int i=0; i<num; i++)
	{
		CComboBox *pCtrl = (CComboBox*)m_listCombo[i];
		pCtrl->DestroyWindow();
		if( pCtrl )delete pCtrl;
	}

	m_listCombo.RemoveAll();	

	CNoDblClkButton *pBtn;
	for( i=0; i<m_listBtn.GetSize(); i++)
	{
		pBtn = (CNoDblClkButton*)m_listBtn[i];
		pBtn->DestroyWindow();
		delete pBtn;
	}

	m_listBtn.RemoveAll();

	CDialogBar::OnDestroy();

	return;
}

int CXVCtrlBar::GetButtonsWidth()
{
	DWORD style = GetStyle();
	if( (m_style&CBRS_TOP) || (m_style&CBRS_BOTTOM) )
	{
		int off = 0;
		int num = m_listCombo.GetSize();
		for(int i=0; i<num; i++)
		{
			CComboBox *pCtrl = (CComboBox*)m_listCombo[i];
			if( pCtrl )
			{
				CRect rect;
				pCtrl->GetClientRect(&rect);
				off += rect.Width();
			}
		}

		if (m_bSliderZoom)
		{
			CRect rect;
			m_Static.GetClientRect(&rect);
			off += rect.Width();
		}		

		return m_listBtn.GetSize()*CTRLBTN_WID+off;
	}
	else
		return CTRLBTN_WID;
}

int CXVCtrlBar::GetButtonsHeight()
{
	DWORD style = GetStyle();
	if( (m_style&CBRS_TOP) || (m_style&CBRS_BOTTOM) )
		return CTRLBTN_HEI;
	else
		return m_listBtn.GetSize()*CTRLBTN_HEI;
}


void CXVCtrlBar::AddButtons(UINT *ids, UINT *bmpIds, int num)
{
	CBitmap bmp;
	CMFCButton *pBtn;

	int nold = m_listBtn.GetSize();
	DWORD style = GetStyle();
	for( int i=0; i<num; i++)
	{
		pBtn = new CNoDblClkButton;
		if( !pBtn )continue;

		if( Manual_Create_Window )
		{
			if( (m_style&CBRS_TOP) || (m_style&CBRS_BOTTOM) )
				MyCreateWindow(pBtn,_T("BUTTON"),_T(""),WS_CHILD|WS_VISIBLE|BS_CHECKBOX,
					CRect(nold*CTRLBTN_WID,0,nold*CTRLBTN_WID+CTRLBTN_WID,CTRLBTN_HEI),
					this,ids[i]);
			else
				MyCreateWindow(pBtn,_T("BUTTON"),_T(""),WS_CHILD|WS_VISIBLE|BS_CHECKBOX,
					CRect(0,nold*CTRLBTN_HEI,CTRLBTN_WID,nold*CTRLBTN_HEI+CTRLBTN_HEI),
					this,ids[i]);		
		}
		else
		{
			if( (m_style&CBRS_TOP) || (m_style&CBRS_BOTTOM) )
				pBtn->Create(_T(""),WS_CHILD|WS_VISIBLE|BS_CHECKBOX,
					CRect(nold*CTRLBTN_WID,0,nold*CTRLBTN_WID+CTRLBTN_WID,CTRLBTN_HEI),
					this,ids[i]);
			else
				pBtn->Create(_T(""),WS_CHILD|WS_VISIBLE|BS_CHECKBOX,
					CRect(0,nold*CTRLBTN_HEI,CTRLBTN_WID,nold*CTRLBTN_HEI+CTRLBTN_HEI),
					this,ids[i]);
		}

		nold++;

		bmp.LoadBitmap(bmpIds[i]);

		HBITMAP hDisableBmp = CreateDisableBitmap((HBITMAP)bmp);
		HBITMAP hBmp24 = Copy24Bitmap((HBITMAP)bmp);

		pBtn->SetImage((HBITMAP)hBmp24,FALSE,(HBITMAP)hBmp24,TRUE,hDisableBmp);
		//pBtn->SetImage((HBITMAP)bmp,FALSE);
		//pBtn->SetImage((HBITMAP)hNewBmp2,FALSE);
		
		::DeleteObject(hDisableBmp);
		::DeleteObject(hBmp24);

		bmp.DeleteObject();
		pBtn->SetMouseCursorHand();
		pBtn->m_bDrawFocus = FALSE;
		pBtn->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
		m_listBtn.Add(pBtn);
		m_listBtnStates.Add(BTN_STATE_ENABLE);
	}

	if (!m_bSliderZoom)
	{
		AdjustComboBox();
	}
	else
	{
		AdjustStatic();
	}
}



void CXVCtrlBar::AddButtons(UINT *ids, HBITMAP *bmps, int num)
{
	CMFCButton *pBtn;

	int nold = m_listBtn.GetSize();
	DWORD style = GetStyle();
	for( int i=0; i<num; i++)
	{
		pBtn = new CNoDblClkButton;
		if( !pBtn )continue;

		if( Manual_Create_Window )
		{
			if( (m_style&CBRS_TOP) || (m_style&CBRS_BOTTOM) )
				MyCreateWindow(pBtn,_T("BUTTON"),_T(""),WS_CHILD|WS_VISIBLE|BS_CHECKBOX,
					CRect(nold*CTRLBTN_WID,0,nold*CTRLBTN_WID+CTRLBTN_WID,CTRLBTN_HEI),
					this,ids[i]);
			else
				MyCreateWindow(pBtn,_T("BUTTON"),_T(""),WS_CHILD|WS_VISIBLE|BS_CHECKBOX,
					CRect(0,nold*CTRLBTN_HEI,CTRLBTN_WID,nold*CTRLBTN_HEI+CTRLBTN_HEI),
					this,ids[i]);		
		}
		else
		{
			if( (m_style&CBRS_TOP) || (m_style&CBRS_BOTTOM) )
				pBtn->Create(_T(""),WS_CHILD|WS_VISIBLE|BS_CHECKBOX,
					CRect(nold*CTRLBTN_WID,0,nold*CTRLBTN_WID+CTRLBTN_WID,CTRLBTN_HEI),
					this,ids[i]);
			else
				pBtn->Create(_T(""),WS_CHILD|WS_VISIBLE|BS_CHECKBOX,
					CRect(0,nold*CTRLBTN_HEI,CTRLBTN_WID,nold*CTRLBTN_HEI+CTRLBTN_HEI),
					this,ids[i]);
		}

		nold++;

		HBITMAP hNewBmp = CreateDisableBitmap(bmps[i]);

		HBITMAP hBmp24 = Copy24Bitmap((HBITMAP)bmps[i]);
	
		pBtn->SetImage(hBmp24,TRUE,hBmp24,TRUE,hNewBmp);

		::DeleteObject(hBmp24);
		::DeleteObject(hNewBmp);

		pBtn->SetMouseCursorHand();
		pBtn->m_bDrawFocus = FALSE;
		pBtn->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
		m_listBtn.Add(pBtn);
		m_listBtnStates.Add(BTN_STATE_ENABLE);
	}

	if (!m_bSliderZoom)
	{
		AdjustComboBox();
	}
	else
	{
		AdjustStatic();
	}
	
	
}

CButton* CXVCtrlBar::GetButton(UINT id)
{
	CMFCButton *pBtn;
	int num = m_listBtn.GetSize();
	for(int i=0; i<num; i++)
	{
		pBtn = (CMFCButton*)m_listBtn[i];
		if( pBtn && pBtn->IsKindOf(RUNTIME_CLASS(CButton)) && pBtn->GetDlgCtrlID()==id )
		{
			return pBtn;
		}
	}
	return NULL;
}

BOOL CXVCtrlBar::AddScrollBar(UINT id)
{
	if( ::IsWindow(m_scrollBar.m_hWnd) )
		return FALSE;

	int align=0;
	if( m_style&CBRS_TOP )align = (SBS_HORZ|SBS_TOPALIGN);
	else if( m_style&CBRS_BOTTOM )align = (SBS_HORZ|SBS_BOTTOMALIGN);
	else if( m_style&CBRS_LEFT )align = (SBS_VERT|SBS_LEFTALIGN);
	else if( m_style&CBRS_RIGHT )align = (SBS_VERT|SBS_RIGHTALIGN);

	if( Manual_Create_Window )
	{
		return MyCreateWindow(&m_scrollBar,_T("SCROLLBAR"),_T(""),WS_CHILD|WS_VISIBLE|align,
					CRect(0,0,0,0),
					this,id);
	}

	return m_scrollBar.Create(WS_CHILD|WS_VISIBLE|align,
		CRect(0,0,0,0),this,id);
}

CScrollBar *CXVCtrlBar::GetScrollBar(UINT id)
{
	if( ::IsWindow(m_scrollBar.m_hWnd)  )
	{
		return &m_scrollBar;
	}
	return NULL;
}


BOOL CXVCtrlBar::AddComboBox(UINT id)
{
	if( !(m_style&CBRS_TOP) && !(m_style&CBRS_BOTTOM) )
		return FALSE;

	CComboBox *pCtrl = new CComboBox;
	if( !pCtrl )return FALSE;

	if( Manual_Create_Window )
	{
		if( MyCreateWindow(pCtrl,_T("COMBOBOX"),_T(""),WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST,
			CRect(0,0,50,100),this,id) )
		{
			pCtrl->SetFont(GetFont(),FALSE);
			pCtrl->SetItemHeight(-1,CTRLBTN_HEI-5);
			pCtrl->SetDroppedWidth(100);
			m_listCombo.Add(pCtrl);
			AdjustComboBox();
			return TRUE;
		}
		else
		{
			delete pCtrl;
		}

		return FALSE;
	}

	if( pCtrl->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST,
		CRect(0,0,50,100),this,id) )
	{
		pCtrl->SetFont(GetFont(),FALSE);
		pCtrl->SetItemHeight(-1,CTRLBTN_HEI-5);
		pCtrl->SetDroppedWidth(100);
		m_listCombo.Add(pCtrl);
		AdjustComboBox();
		return TRUE;
	}
	else
	{
		delete pCtrl;
	}

	return FALSE;
}

BOOL CXVCtrlBar::CreateStatic()
{
	if( !(m_style&CBRS_TOP) && !(m_style&CBRS_BOTTOM) )
		return FALSE;

	m_Static.Create(_T(""),WS_CHILD|WS_VISIBLE|ES_CENTER|WS_BORDER|SS_NOTIFY,
		CRect(0,0,50,100),this,IDC_SLIDER_EDIT);

	CRect rect;
	m_Static.GetClientRect(&rect);

	m_Static.SetFont(GetFont(),FALSE);

	int off = m_listBtn.GetSize()*CTRLBTN_WID;
	::SetWindowPos(m_Static.m_hWnd,HWND_TOP,off,0,rect.Width(),rect.Height(),SWP_DRAWFRAME|SWP_SHOWWINDOW);

	return TRUE;
}

CComboBox* CXVCtrlBar::GetComboBox(UINT id)
{
	int num = m_listCombo.GetSize();
	for( int i=0; i<num; i++)
	{
		CComboBox *pCtrl = (CComboBox*)m_listCombo[i];
		if( pCtrl->GetDlgCtrlID()==id )
			return pCtrl;
	}
}


void CXVCtrlBar::AdjustComboBox()
{
	if( !(m_style&CBRS_TOP) && !(m_style&CBRS_BOTTOM) )
		return;

	CComboBox* pCtrl;

	int num = m_listCombo.GetSize();
	if( num<=0 )return;
	int off = m_listBtn.GetSize()*CTRLBTN_WID;

	for(int i=0; i<num; i++)
	{
		pCtrl = (CComboBox*)m_listCombo[i];
		if( pCtrl )
		{
			CRect rect;
			pCtrl->GetClientRect(&rect);
			::SetWindowPos(pCtrl->GetSafeHwnd(),NULL,
				off,0,rect.Width(),rect.Height(),SWP_NOZORDER);
			off += rect.Width();
		}
	}
	return;
}

void CXVCtrlBar::AdjustStatic()
{
	if( !(m_style&CBRS_TOP) && !(m_style&CBRS_BOTTOM) )
		return;

	CRect rect;
	m_Static.GetClientRect(&rect);

	int off = m_listBtn.GetSize()*CTRLBTN_WID;
	::SetWindowPos(m_Static.m_hWnd,HWND_TOP,off,0,rect.Width(),rect.Height(),SWP_DRAWFRAME|SWP_SHOWWINDOW);
}


void CXVCtrlBar::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{
	CNoDblClkButton *pBtn;
	for( int i=0; i<m_listBtn.GetSize(); i++)
	{
		pBtn = (CNoDblClkButton*)m_listBtn[i];
		UINT state = m_listBtnStates[i];
		pBtn->EnableWindow((state&BTN_STATE_ENABLE)!=0?TRUE:FALSE);
		pBtn->SetCheck((state&BTN_STATE_CHECKED)!=0?1:0);
	}
}

void CXVCtrlBar::SetButtonState( int id, UINT add, UINT del )
{
	CNoDblClkButton *pBtn;
	for( int i=0; i<m_listBtn.GetSize(); i++)
	{
		pBtn = (CNoDblClkButton*)m_listBtn[i];
		if( pBtn->GetDlgCtrlID()==id )
		{
			UINT state = m_listBtnStates[i];
			state |= add;
			state = (state&(~del));

			m_listBtnStates[i] = state;
		}
	}
}


void CXVCtrlBar::SetButtonState2( int id, BOOL bEnable, BOOL bCheck )
{
	UINT add = 0, del = 0;
	if( bEnable )
		add += BTN_STATE_ENABLE;
	else
		del += BTN_STATE_ENABLE;

	if( bCheck )
		add += BTN_STATE_CHECKED;
	else
		del += BTN_STATE_CHECKED;

	SetButtonState(id,add,del);
}


UINT CXVCtrlBar::GetButtonState( int id )
{
	CNoDblClkButton *pBtn;
	for( int i=0; i<m_listBtn.GetSize(); i++)
	{
		pBtn = (CNoDblClkButton*)m_listBtn[i];
		if( pBtn->GetDlgCtrlID()==id )
		{
			return m_listBtnStates[i];
		}
	}

	return 0;
}


void CXVCtrlBar::SetMsgWnd(HWND hWnd)
{
	m_hMsgWnd = hWnd;
}


BOOL CXVCtrlBar::OnCommand( WPARAM wParam, LPARAM lParam )
{
	CWnd *pWnd = CWnd::FromHandle(m_hMsgWnd!=NULL?m_hMsgWnd:(::GetParent(m_hWnd)));
	if( pWnd )
	{
		pWnd->SendMessage(WM_COMMAND,wParam,lParam);
		return TRUE;
	}
	else
	{
		return CDialogBar::OnCommand(wParam,lParam);
	}
}

void CXVCtrlBar::OnComboZoomChange()
{
	CWnd *pWnd = CWnd::FromHandle(m_hMsgWnd!=NULL?m_hMsgWnd:(::GetParent(m_hWnd)));
	if( pWnd )
	{
		pWnd->SendMessage(WM_COMMAND,IDC_COMBO_ZOOM,0);
		return;
	}
}

void CXVCtrlBar::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CControlBar::OnDrawItem(nIDCtl,lpDrawItemStruct);
	return;	
}


void CXVCtrlBar::OnMouseMove(UINT nFlags, CPoint point)
{
	CControlBar::OnMouseMove(nFlags, point);
}




void CXVCtrlBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default

	CDialogBar::OnNcCalcSize(bCalcValidRects, lpncsp);
}
