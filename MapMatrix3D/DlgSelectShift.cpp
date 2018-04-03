// DlgSelectShift.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgSelectShift.h"
#include "EditbaseDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_FOCUS_ITEM				(WM_USER+1)

/////////////////////////////////////////////////////////////////////////////
// CFocusColorListBox

CFocusColorListBox::CFocusColorListBox()
{
	m_nCurFocusItem = -1;
}

CFocusColorListBox::~CFocusColorListBox()
{
}


BEGIN_MESSAGE_MAP(CFocusColorListBox, CListBox)
	//{{AFX_MSG_MAP(CFocusColorListBox)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFocusColorListBox message handlers
void CFocusColorListBox::AppendString(LPCSTR lpszText, COLORREF fgColor, COLORREF bgColor)
{
	LISTBOX_COLOR* pInfo = new LISTBOX_COLOR;

	pInfo->strText.Format(_T("%s"), lpszText);
	pInfo->fgColor = fgColor; 
	pInfo->bgColor = bgColor;

	SetItemDataPtr(AddString(pInfo->strText), pInfo);
}

void CFocusColorListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	ASSERT(lpMeasureItemStruct->CtlType == ODT_LISTBOX);
	
	CString strText(_T(""));
	GetText(lpMeasureItemStruct->itemID, strText);
	ASSERT(TRUE != strText.IsEmpty());

	CRect rect;
	GetItemRect(lpMeasureItemStruct->itemID, &rect);
	
	CDC* pDC = GetDC(); 
	lpMeasureItemStruct->itemHeight = pDC->DrawText(strText, -1, rect, DT_WORDBREAK | DT_CALCRECT); 
	ReleaseDC(pDC);
}

void CFocusColorListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);

	CString text = ((LISTBOX_COLOR*)lpDrawItemStruct->itemData)->strText;

	CDC dc;
	
	dc.Attach(lpDrawItemStruct->hDC);

	COLORREF fgColor = RGB(0, 0, 0);
	COLORREF bgColor = RGB(255, 255, 255);
	
	// Save these value to restore them when done drawing.
	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();

	// If this item is selected, set the background color 
	// and the text color to appropriate values. Also, erase
	// rect by filling it with the background color.
	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
		(lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		dc.SetTextColor(bgColor);
		dc.SetBkColor(fgColor);
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, fgColor);
	}
	else if(lpDrawItemStruct->itemID==m_nCurFocusItem)
	{
		fgColor = RGB(255, 255, 255);
		bgColor = RGB(128,128,128);

		//dc.SetTextColor(pListBox->bgColor);
		//dc.SetBkColor(bgColor);
		//dc.FillSolidRect(&lpDrawItemStruct->rcItem, bgColor);		

		dc.SetTextColor(fgColor);
		dc.SetBkColor(bgColor);
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, bgColor);
	}
	else
	{
		dc.SetTextColor(fgColor);
		dc.SetBkColor(bgColor);
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, bgColor);
	}
	
	lpDrawItemStruct->rcItem.left += 5;
	// Draw the text.
	
	dc.DrawText(text, strlen(text), &lpDrawItemStruct->rcItem, DT_WORDBREAK);
	
	// Reset the background color and the text color back to their
	// original values.
	dc.SetTextColor(crOldTextColor);
	dc.SetBkColor(crOldBkColor);
	
	dc.Detach();	
}


void CFocusColorListBox::OnDestroy() 
{
	CListBox::OnDestroy();
	
	// TODO: Add your message handler code here	
	int nCount = GetCount();
	for(int i=0; i<nCount; i++)
	{
		LISTBOX_COLOR* pList = (LISTBOX_COLOR*)GetItemDataPtr(i);
		delete pList;
		pList = NULL;
	}
}


void CFocusColorListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	int old = m_nCurFocusItem;
	BOOL bOutside = FALSE;
	m_nCurFocusItem = ItemFromPoint(point,bOutside);
	if(bOutside)
		m_nCurFocusItem = -1;

	if(m_nCurFocusItem!=old)
	{
		CRect rcItem;
		GetItemRect(old,&rcItem);
		RedrawWindow(&rcItem);
		GetItemRect(m_nCurFocusItem,&rcItem);
		RedrawWindow(&rcItem);
		GetParent()->SendMessage(WM_FOCUS_ITEM,m_nCurFocusItem,old);
	}
	
	CListBox::OnMouseMove(nFlags,point);
}



IMPLEMENT_DYNCREATE(CNoDblClkButton_copy, CMFCButton)

BEGIN_MESSAGE_MAP(CNoDblClkButton_copy, CMFCButton)
	//{{AFX_MSG_MAP(CNoDblClkButton_copy)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CNoDblClkButton_copy::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CWnd* pParent = GetParent ();
	if (pParent != NULL)
	{
		pParent->SendMessage (	WM_COMMAND,
			MAKEWPARAM (GetDlgCtrlID (), BN_CLICKED),
			(LPARAM) m_hWnd);
	}
	CMFCButton::OnLButtonDblClk(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectShift dialog


CDlgSelectShift::CDlgSelectShift(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectShift::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelectShift)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_pDoc = NULL;
	m_nCurFtrIndex = 0;
	m_bExpand = FALSE;
}


void CDlgSelectShift::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectShift)
	DDX_Control(pDX, IDC_LIST_SELECTION, m_wndList);
	DDX_Control(pDX, IDC_BUTTON_EXPAND, m_btnExpand);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectShift, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectShift)
	ON_BN_CLICKED(IDC_BUTTON_SHIFT, OnButtonShift)
	ON_BN_CLICKED(IDC_BUTTON_EXPAND, OnButtonExpand)
	ON_BN_CLICKED(IDC_BUTTON_SELECTALL, OnButtonSelectall)
	ON_LBN_SELCHANGE(IDC_LIST_SELECTION, OnSelchangeListSelection)
	ON_MESSAGE(WM_FOCUS_ITEM,OnFocusListItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectShift message handlers

void CDlgSelectShift::OnButtonShift() 
{
	if(!m_pDoc)
		return;

	if(m_arrFtrs.GetSize()<=0)
		return;

	m_nCurFtrIndex = ((m_nCurFtrIndex+1)%m_arrFtrs.GetSize());

	if(m_nCurFtrIndex>=0 && m_nCurFtrIndex<m_arrFtrs.GetSize())
	{
		CFeature *pFtr = m_arrFtrs[m_nCurFtrIndex];

		m_pDoc->GetSelection()->DeselectAll();
		m_pDoc->GetSelection()->SelectObj(FtrToHandle(pFtr));
		m_pDoc->OnSelectChanged();	
	}

	UpdateButton();

	SetViewFocus();	
}

void CDlgSelectShift::OnButtonExpand() 
{
	m_bExpand = !m_bExpand;
	ShowExpand(m_bExpand);	
}

void CDlgSelectShift::OnButtonSelectall() 
{
	m_pDoc->GetSelection()->DeselectAll();
	for(int i=0; i<m_arrFtrs.GetSize(); i++)
	{
		CFeature *pFtr = m_arrFtrs[i];

		m_pDoc->GetSelection()->SelectObj(FtrToHandle(pFtr));
	}
	m_pDoc->OnSelectChanged();	

	ShowWindow(SW_HIDE);
	SetViewFocus();	
}

BOOL CDlgSelectShift::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_btnExpand.SetImage(IDB_BITMAP_EXPAND);
	m_btnExpand.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;

	m_btnExpand.SetMouseCursorHand();
	m_btnExpand.m_bDrawFocus = FALSE;
	
	// TODO: Add extra initialization here
	CWnd *pWnd = GetDlgItem(IDC_BUTTON_SHIFT);
	if(pWnd)
	{
		pWnd->GetWindowText(m_strBtnLabel);
	}
	
	GetClientRect(&m_rcClientInit);

	ShowExpand(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectShift::OnSelchangeListSelection() 
{
	if(!m_pDoc)
		return;

	ShowWindow(SW_HIDE);

	SetViewFocus();
}


void CDlgSelectShift::UpdateButton()
{
	if(!m_pDoc)
		return;
	
	CWnd *pWnd = GetDlgItem(IDC_BUTTON_SHIFT);
	if(pWnd)
	{
		CString textIndex;
		textIndex.Format("(%d/%d)",m_nCurFtrIndex+1,m_arrFtrs.GetSize());
		pWnd->SetWindowText(m_strBtnLabel + textIndex);
	}
}

void CDlgSelectShift::UpdateList()
{
	m_wndList.ResetContent();
	m_nCurFtrIndex = 0;
	ShowExpand(FALSE);
	m_bExpand = FALSE;
	
	if(!m_pDoc)
		return;
	
	UpdateButton();

	for(int i=0; i<m_arrFtrs.GetSize(); i++)
	{
		CString text;
		text.Format("%2d: ",(i+1));
		CFeature *pFtr = m_arrFtrs[i];
		CString geotype = CPermanent::GetPermanentName(pFtr->GetGeometry()->GetClassType());
		CFtrLayer *pLayer = m_pDoc->GetDlgDataSource()->GetFtrLayerOfObject(pFtr);
		if (pLayer)
		{
			CString layName = pLayer->GetName();
			//m_wndList.AddString(text+geotype + ", " + layName);
			m_wndList.AppendString(text + geotype + ", " + layName, 0, 0);
			m_wndList.SetItemHeight(i, 30);
		}
		else
		{
			m_wndList.AppendString(text + geotype, 0, 0);
			m_wndList.SetItemHeight(i, 30);
		}
	}
}

LRESULT CDlgSelectShift::OnFocusListItem(WPARAM wParam, LPARAM lParam)
{
	if(!m_pDoc)
		return 0;

	int index = (int)wParam;
	if(index>=0 && index<m_arrFtrs.GetSize())
	{
		CFeature *pFtr = m_arrFtrs[index];

		m_pDoc->GetSelection()->DeselectAll();
		m_pDoc->GetSelection()->SelectObj(FtrToHandle(pFtr));
		m_pDoc->OnSelectChanged();	
	}

	return 0;
}


void CDlgSelectShift::SetViewFocus()
{
	if(!m_pDoc)
		return;

	CView *pView = GetActiveView();
	if(pView)
	{
		pView->SetFocus();
	}	
}

void CDlgSelectShift::ShowExpand(BOOL bExpand)
{
	CWnd *pWnd = GetDlgItem(IDC_STATIC_RECT);
	if(!pWnd)
		return;

	CRect rcWnd;
	GetWindowRect(&rcWnd);
		
	CRect rcClient;
	GetClientRect(&rcClient);

	if(!bExpand)
	{
		CRect rcStatic;
		pWnd->GetWindowRect(&rcStatic);
		ScreenToClient(&rcStatic);
		
		//rcWnd.right = rcWnd.right + rcStatic.Width()-rcClient.Width();
		rcWnd.bottom = rcWnd.bottom + rcStatic.Height()-rcClient.Height();
		MoveWindow(&rcWnd);		
	}
	else
	{		
		CRect rcClient2 = m_rcClientInit;
		
		//rcWnd.right = rcWnd.right + rcClient2.Width() - rcClient.Width();
		rcWnd.bottom = rcWnd.bottom + rcClient2.Height() - rcClient.Height();
		MoveWindow(&rcWnd);
	}
}