// LayerCombo.cpp : implementation file
//

#include "stdafx.h"
#include "multiCombo.h"
#include "EditBase.h"
#include "resource.h"
#include "DlgDataSource.h"
#include "SymbolLib.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiCombo
static WNDPROC g_pWndProc = NULL;
static CLayerBitmapComboBox *g_pLayerComboBox = NULL;
static WNDPROC g_pLayerGroupWndProc = NULL;
static CLayerGroupBitmapComboBox *g_pLayerGroupComboBox = NULL;
CRect g_Btn0Rect = CRect(1, 0, 25, 24);

static BOOL CheckLayerValid(CFtrLayer *pLayer)
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if( !pDoc )
		return FALSE;
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if( !pDS )
		return FALSE;

	int nLayer = pDS->GetFtrLayerCount();
	for( int i=0; i<nLayer; i++)
	{
		if( pDS->GetFtrLayerByIndex(i)==pLayer )
			return TRUE;
	}

	return FALSE;
}

static BOOL CheckGroupValid(FtrLayerGroup *pGroup)
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if( !pDoc )
		return FALSE;
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if( !pDS )
		return FALSE;
	
	int num = pDS->GetFtrLayerGroupCount();
	for( int i=0; i<num; i++)
	{
		if( pDS->GetFtrLayerGroup(i)==pGroup )
			return TRUE;
	}
	
	return FALSE;
}


extern "C" LRESULT FAR PASCAL MultiComboBoxListBoxProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{

	static	bool bMustClose = false;

	switch (nMsg) {

		case WM_LBUTTONDOWN: {

			CRect rcClient;
			GetClientRect(hWnd, rcClient);

			CPoint pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);


			if (PtInRect(rcClient, pt)) {
				INT nItemHeight = SendMessage(hWnd, LB_GETITEMHEIGHT, 0, 0);
				INT nTopIndex   = SendMessage(hWnd, LB_GETTOPINDEX, 0, 0);

				// Check which item was selected (item)
				INT nIndex = nTopIndex + pt.y / nItemHeight;

				CRect rcItem;
				SendMessage(hWnd, LB_GETITEMRECT, nIndex, (LPARAM)(VOID *)&rcItem);

				rcItem.left += 50;
				if (PtInRect(rcItem, pt)) 
				{
					bMustClose = true;
				}
				else
				{
					rcItem.left -= 50;
					// Invalidate this window
					if(pt.x-rcItem.left <= 16)
					{
// 						CFtrLayer *pLayer = (CFtrLayer*)g_pComboBox->GetItemData(nIndex);
// 						pLayer->SetVisible(!pLayer->IsVisible());
					}
					else if (pt.x-rcItem.left <= 32)
					{
// 						CFtrLayer *pLayer = (CFtrLayer*)g_pComboBox->GetItemData(nIndex);
// 						pLayer->SetLock(!pLayer->IsLocked());
					}

					InvalidateRect(hWnd, rcItem, FALSE);
					// Notify that selection has changed
					g_pLayerComboBox->GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetWindowLong(g_pLayerComboBox->m_hWnd, GWL_ID), CBN_SELCHANGE), (LPARAM)g_pLayerComboBox->m_hWnd);
					bMustClose = false;
				}
			}

			// GO to default handler
			break;
		}

		case WM_LBUTTONUP: {
			// Don't do anything here. This causes the combobox popup
			// windows to remain open after a selection has been made
			if(!bMustClose)
				return 0;
		}
	}

	return CallWindowProc(g_pWndProc, hWnd, nMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CBaseBitmapComboBox, CComboBox)

CBaseBitmapComboBox::CBaseBitmapComboBox()
{
	m_hListBox = 0;	
	m_lfHeight = 8;
    m_lfWeight = FW_NORMAL;
    m_strFaceName = _T("MS Sans Serif");
	m_pToolbarComboxButton = NULL;
}

CBaseBitmapComboBox::~CBaseBitmapComboBox()
{
}


BEGIN_MESSAGE_MAP(CBaseBitmapComboBox, CComboBox)
	//{{AFX_MSG_MAP(CMultiCombo)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CTLCOLORLISTBOX, OnCtlColorListBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiCombo message handlers
LRESULT CBaseBitmapComboBox::OnCtlColorListBox(WPARAM wParam, LPARAM lParam) 
{
	// Here we need to get a reference to the listbox of the combobox
	// (the dropdown part). We can do it using 
	if (this->m_hListBox == 0) {
		HWND hWnd = (HWND)lParam;

		if (hWnd != 0 && hWnd != m_hWnd) {
			// Save the handle
			m_hListBox = hWnd;

			// Subclass ListBox
			g_pWndProc = (WNDPROC)GetWindowLongPtr(m_hListBox, GWLP_WNDPROC);
			SetWindowLongPtr(m_hListBox, GWLP_WNDPROC, (LONG_PTR)MultiComboBoxListBoxProc);
		}
	}

	
	return DefWindowProc(WM_CTLCOLORLISTBOX, wParam, lParam);
}

void CBaseBitmapComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item

	// This code is used to draw each one of the items in the combobox

	// First, check if we are drawing and item
	if(lpDrawItemStruct->itemID == -1)
		return;
}

void CBaseBitmapComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	CComboBox::MeasureItem(lpMeasureItemStruct);
}

void CBaseBitmapComboBox::OnCloseUp()
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (m_pToolbarComboxButton && pDoc)
	{
		pDoc->EndBatchUpdate();
	}
}

void CBaseBitmapComboBox::OnDropdown() 
{
	// TODO: Add your control notification handler code here
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (m_pToolbarComboxButton && pDoc)
	{
		pDoc->BeginBatchUpdate();
	}	
}

int CBaseBitmapComboBox::AddString(LPCTSTR lpszString)
{
	return CComboBox::AddString(lpszString);
}

int CBaseBitmapComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CComboBox::OnCreate(lpCreateStruct) == -1)
        return -1;

	//SetFont(&GetGlobalData()->fontRegular);
	
    return 0;
}

BOOL CBaseBitmapComboBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	// TODO: Add your specialized code here and/or call the base class
	// Remove the CBS_SIMPLE and CBS_DROPDOWN styles and add the one I'm designed for
	//dwStyle &= ~0xF;
	dwStyle |= CBS_DROPDOWNLIST;

	// Make sure to use the CBS_OWNERDRAWVARIABLE style
	dwStyle |= CBS_OWNERDRAWVARIABLE;

	// Use default strings. We need the itemdata to store the state of the lamps
	dwStyle |= CBS_HASSTRINGS;	

	return CComboBox::Create(dwStyle, rect, pParentWnd, nID);
}

//////////////////////////////////////////////////////////////////////////

extern "C" LRESULT FAR PASCAL LayerMultiComboBoxListBoxProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	
	static	bool bMustClose = false;
	
	// Ctrl 仅选中点击的项，Shift 反选点击的项
	BOOL bCtrl = GetKeyState(VK_CONTROL)<0, bShift = GetKeyState(VK_SHIFT)<0;

	switch (nMsg) {
		
	case WM_LBUTTONDOWN: {
		
		CRect rcClient;
		GetClientRect(hWnd, rcClient);
		
		CPoint pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		
		
		if (PtInRect(rcClient, pt)) {
			INT nItemHeight = SendMessage(hWnd, LB_GETITEMHEIGHT, 0, 0);
			INT nTopIndex   = SendMessage(hWnd, LB_GETTOPINDEX, 0, 0);
			
			// Check which item was selected (item)
			INT nIndex = nTopIndex + pt.y / nItemHeight;
			
			CRect rcItem;
			SendMessage(hWnd, LB_GETITEMRECT, nIndex, (LPARAM)(VOID *)&rcItem);
			
			rcItem.left += 50;
			if (PtInRect(rcItem, pt)) 
			{
				bMustClose = true;
			}
			else
			{
				rcItem.left -= 50;
				// Invalidate this window
				if(pt.x-rcItem.left <= 16)
				{
					if (bCtrl || bShift)
					{
						for (int i=0; i<g_pLayerComboBox->GetCount(); i++)
						{
							bool bVisible = false;
							if ((bCtrl && i == nIndex) || (bShift && i != nIndex))
							{
								bVisible = true;
							}
							
							CFtrLayer *pLayer = (CFtrLayer*)g_pLayerComboBox->GetItemData(i);
							if( !CheckLayerValid(pLayer) )
								break;

							if (pLayer->IsVisible()^bVisible)
							{
								GetActiveDlgDoc()->ModifyLayer(pLayer, FIELDNAME_LAYVISIBLE, _variant_t(bVisible), TRUE);
							}
						}
					}
					else
					{
						CFtrLayer *pLayer = (CFtrLayer*)g_pLayerComboBox->GetItemData(nIndex);
						if( !CheckLayerValid(pLayer) )
							break;

						GetActiveDlgDoc()->ModifyLayer(pLayer, FIELDNAME_LAYVISIBLE, _variant_t(!pLayer->IsVisible()), TRUE);
					}
				}
				else if (pt.x-rcItem.left <= 32)
				{
					if (bCtrl || bShift)
					{
						for (int i=0; i<g_pLayerComboBox->GetCount(); i++)
						{
							bool bLocked = false;
							if ((bCtrl && i == nIndex) || (bShift && i != nIndex))
							{
								bLocked = true;
							}
							
							CFtrLayer *pLayer = (CFtrLayer*)g_pLayerComboBox->GetItemData(i);
							if( !CheckLayerValid(pLayer) )
								break;

							if (pLayer->IsLocked()^bLocked)
							{
								GetActiveDlgDoc()->ModifyLayer(pLayer, FIELDNAME_LAYLOCKED, _variant_t(bLocked), TRUE);
							}
						}
					}
					else
					{
						CFtrLayer *pLayer = (CFtrLayer*)g_pLayerComboBox->GetItemData(nIndex);
						if( !CheckLayerValid(pLayer) )
							break;

						GetActiveDlgDoc()->ModifyLayer(pLayer, FIELDNAME_LAYLOCKED, _variant_t(!pLayer->IsLocked()), TRUE);
					}
				}
				
				if (bCtrl || bShift)
				{
					InvalidateRect(hWnd, NULL, FALSE);
				}
				else
				{
					InvalidateRect(hWnd, rcItem, FALSE);
				}
				// Notify that selection has changed
				g_pLayerComboBox->GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetWindowLong(g_pLayerComboBox->m_hWnd, GWL_ID), CBN_SELCHANGE), (LPARAM)g_pLayerComboBox->m_hWnd);
				bMustClose = false;
			}
		}
		
		// GO to default handler
		break;
						 }
		
	case WM_LBUTTONUP: {
		// Don't do anything here. This causes the combobox popup
		// windows to remain open after a selection has been made
		if(!bMustClose)
			return 0;
					   }
	}
	
	return CallWindowProc(g_pWndProc, hWnd, nMsg, wParam, lParam);
}

extern "C" LRESULT FAR PASCAL LayerGroupMultiComboBoxListBoxProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	
	static	bool bMustClose = false;
	
	// Ctrl 仅选中点击的项，Shift 反选点击的项
	BOOL bCtrl = GetKeyState(VK_CONTROL)<0, bShift = GetKeyState(VK_SHIFT)<0;

	switch (nMsg) {
		
	case WM_LBUTTONDOWN: {
		
		CRect rcClient;
		GetClientRect(hWnd, rcClient);
		
		CPoint pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		
		
		if (PtInRect(rcClient, pt)) {
			INT nItemHeight = SendMessage(hWnd, LB_GETITEMHEIGHT, 0, 0);
			INT nTopIndex   = SendMessage(hWnd, LB_GETTOPINDEX, 0, 0);
			
			// Check which item was selected (item)
			INT nIndex = nTopIndex + pt.y / nItemHeight;
			
			CRect rcItem;
			SendMessage(hWnd, LB_GETITEMRECT, nIndex, (LPARAM)(VOID *)&rcItem);
			
			rcItem.left += 50;
			if (PtInRect(rcItem, pt)) 
			{
				bMustClose = true;
			}
			else
			{
				rcItem.left -= 50;
				// Invalidate this window
				if(pt.x-rcItem.left <= 16)
				{
					if (bCtrl || bShift)
					{
						for (int i=0; i<g_pLayerGroupComboBox->GetCount(); i++)
						{
							bool bVisible = false;
							if ((bCtrl && i == nIndex) || (bShift && i != nIndex))
							{
								bVisible = true;
							}
							
							FtrLayerGroup *pLayerGroup = (FtrLayerGroup*)g_pLayerGroupComboBox->GetItemData(i);
							if( !CheckGroupValid(pLayerGroup) )
								break;

							if (pLayerGroup->Visible^bVisible)
							{
								GetActiveDlgDoc()->ModifyLayerGroup(pLayerGroup, FIELDNAME_LAYVISIBLE, _variant_t(bVisible), TRUE);
							}
						}
					}
					else
					{
						FtrLayerGroup *pLayerGroup = (FtrLayerGroup*)g_pLayerGroupComboBox->GetItemData(nIndex);
						if( !CheckGroupValid(pLayerGroup) )
							break;

						GetActiveDlgDoc()->ModifyLayerGroup(pLayerGroup, FIELDNAME_LAYVISIBLE, _variant_t(!pLayerGroup->Visible), TRUE);
					}
				}
				
				if (bCtrl || bShift)
				{
					InvalidateRect(hWnd, NULL, FALSE);
				}
				else
				{
					InvalidateRect(hWnd, rcItem, FALSE);
				}
				// Notify that selection has changed
				g_pLayerGroupComboBox->GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetWindowLong(g_pLayerGroupComboBox->m_hWnd, GWL_ID), CBN_SELCHANGE), (LPARAM)g_pLayerGroupComboBox->m_hWnd);
				bMustClose = false;
			}
		}
		
		// GO to default handler
		break;
						 }
		
	case WM_LBUTTONUP: {
		// Don't do anything here. This causes the combobox popup
		// windows to remain open after a selection has been made
		if(!bMustClose)
			return 0;
					   }
	}
	
	return CallWindowProc(g_pLayerGroupWndProc, hWnd, nMsg, wParam, lParam);
}

IMPLEMENT_DYNAMIC(CLayerGroupBitmapComboBox,CBaseBitmapComboBox)

CLayerGroupBitmapComboBox::CLayerGroupBitmapComboBox()
{
	this->m_hListBox = 0;	
	m_lfHeight = 8;
    m_lfWeight = FW_NORMAL;
    m_strFaceName = _T("MS Sans Serif");

	m_pToolbarComboxButton = NULL;
}

CLayerGroupBitmapComboBox::~CLayerGroupBitmapComboBox()
{

}


BEGIN_MESSAGE_MAP(CLayerGroupBitmapComboBox, CBaseBitmapComboBox)
	//{{AFX_MSG_MAP(CMultiCombo)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CTLCOLORLISTBOX, OnCtlColorListBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiCombo message handlers
LRESULT CLayerGroupBitmapComboBox::OnCtlColorListBox(WPARAM wParam, LPARAM lParam) 
{
	// Here we need to get a reference to the listbox of the combobox
	// (the dropdown part). We can do it using 
	if (this->m_hListBox == 0) {
		HWND hWnd = (HWND)lParam;

		if (hWnd != 0 && hWnd != m_hWnd) {
			// Save the handle
			m_hListBox = hWnd;

			// Subclass ListBox
			g_pLayerGroupWndProc = (WNDPROC)GetWindowLongPtr(m_hListBox, GWLP_WNDPROC);
			SetWindowLongPtr(m_hListBox, GWLP_WNDPROC, (LONG_PTR)LayerGroupMultiComboBoxListBoxProc);
		}
	}

	
	return DefWindowProc(WM_CTLCOLORLISTBOX, wParam, lParam);
}

void CLayerGroupBitmapComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item

	// This code is used to draw each one of the items in the combobox

	// First, check if we are drawing and item
	if(lpDrawItemStruct->itemID == -1)
		return;

	FtrLayerGroup *pGroup = (FtrLayerGroup*)this->GetItemData(lpDrawItemStruct->itemID);
	if( !CheckGroupValid(pGroup) )
		return;

	if (pGroup == NULL)
		return;

	CDC			pDC;	
	CString		sItem;
	CBrush		pBrush;
	CBitmap		pLampOn, pLampOff, *pOldLamp;
	CDC			pBmDC;
	//DWORD		dwTeste = 0;

	pDC.Attach(lpDrawItemStruct->hDC);

	// Load the lamp bitmaps
	pLampOn.LoadBitmap(IDB_LAMP_ON);
	pLampOff.LoadBitmap(IDB_LAMP_OFF);
	pBmDC.CreateCompatibleDC(NULL);
	
	// Move the drawing rectangle 18 pixels left, so that we can draw 
	// the lamp
	lpDrawItemStruct->rcItem.left += 36;

	// If we have the focus, draw the item with a blue background
	// if we haven't, just plain blank
	if(lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		pBrush.CreateSolidBrush(RGB(49,106,197));
		pDC.FillRect(&lpDrawItemStruct->rcItem, &pBrush);
		pDC.SetTextColor(RGB(255,255,255));
	}
	else
	{
		pBrush.CreateSolidBrush(RGB(255,255,255));
		pDC.FillRect(&lpDrawItemStruct->rcItem, &pBrush);
		pDC.SetTextColor(RGB(0,0,0));
	}
	
	// Copy the text of the item to a string
	this->GetLBText(lpDrawItemStruct->itemID, sItem);
	pDC.SetBkMode(TRANSPARENT);

	// Draw the text
	pDC.DrawText(sItem, &lpDrawItemStruct->rcItem, DT_VCENTER | DT_SINGLELINE);

	
	// Test the bit representing the state of the lamp
	if(pGroup->Visible)
	{
		pOldLamp = pBmDC.SelectObject(&pLampOn);
	}
	else
	{
		pOldLamp = pBmDC.SelectObject(&pLampOff);
	}
		
	int offpix = 0;
	if (lpDrawItemStruct->hwndItem != m_hWnd)
	{
		offpix = g_Btn0Rect.Width()+3;
	}

	pDC.BitBlt(1+offpix,lpDrawItemStruct->rcItem.top ,16,16,&pBmDC,0,0, SRCCOPY);

	pBmDC.SelectObject(pOldLamp);

	CRect colRect(CPoint(20+offpix,lpDrawItemStruct->rcItem.top+1),CSize(13,13));

	pBrush.DeleteObject();
	pBrush.CreateSolidBrush(pGroup->Color);
	//pDC.FillRect(&colRect, &pBrush);
	CBrush *pOldBrush = pDC.SelectObject(&pBrush);

	CPen pen;
    pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	CPen *pOldPen = pDC.SelectObject(&pen);
	pDC.Rectangle(&colRect);

	pDC.SelectObject(pOldBrush);
	pDC.SelectObject(pOldPen);		


	pBmDC.DeleteDC();
	pDC.Detach();
}

void CLayerGroupBitmapComboBox::OnDropdown() 
{
	// TODO: Add your control notification handler code here
	g_pLayerGroupComboBox = this;
	
	if (m_pToolbarComboxButton == NULL) return;

	CBaseBitmapComboBox::OnDropdown();

	CDlgDoc *pDoc = GetActiveDlgDoc();

	int idx = GetCurSel();
	DWORD_PTR dwSelData = 0;
	if (idx >= 0)
	{
		dwSelData = GetItemData(idx);
	}

	m_pToolbarComboxButton->RemoveAllItems();

	if (pDoc == NULL) return;
	
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if (pDS)
	{
		CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();
		int selIndex = -1;
		for (int i=0; i<pDS->GetFtrLayerGroupCount(); i++)
		{
			FtrLayerGroup *pGroup = pDS->GetFtrLayerGroup(i);
			
			if (pGroup)
			{
				CString str = pGroup->Name;
				int index = m_pToolbarComboxButton->AddItem(str, DWORD_PTR(pGroup));
				if (dwSelData == (DWORD_PTR)pGroup || (dwSelData == 0 && pGroup->Name.CompareNoCase(pCurLayer->GetGroupName()) == 0))
				{
					selIndex = index;
				}
			}
		}
		
		m_pToolbarComboxButton->SelectItem(selIndex);
	}

	SetFocus();
}


IMPLEMENT_DYNAMIC(CLayerBitmapComboBox,CBaseBitmapComboBox)

CLayerBitmapComboBox::CLayerBitmapComboBox()
{
	this->m_hListBox = 0;	
	m_lfHeight = 8;
    m_lfWeight = FW_NORMAL;
    m_strFaceName = _T("MS Sans Serif");

	m_pToolbarComboxButton = NULL;
}

CLayerBitmapComboBox::~CLayerBitmapComboBox()
{
}


BEGIN_MESSAGE_MAP(CLayerBitmapComboBox, CBaseBitmapComboBox)
	//{{AFX_MSG_MAP(CMultiCombo)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CTLCOLORLISTBOX, OnCtlColorListBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiCombo message handlers
LRESULT CLayerBitmapComboBox::OnCtlColorListBox(WPARAM wParam, LPARAM lParam) 
{
	// Here we need to get a reference to the listbox of the combobox
	// (the dropdown part). We can do it using 
	if (this->m_hListBox == 0) {
		HWND hWnd = (HWND)lParam;

		if (hWnd != 0 && hWnd != m_hWnd) {
			// Save the handle
			m_hListBox = hWnd;

			// Subclass ListBox
			g_pWndProc = (WNDPROC)GetWindowLongPtr(m_hListBox, GWLP_WNDPROC);
			SetWindowLongPtr(m_hListBox, GWLP_WNDPROC, (LONG_PTR)LayerMultiComboBoxListBoxProc);
		}
	}

	
	return DefWindowProc(WM_CTLCOLORLISTBOX, wParam, lParam);
}

void CLayerBitmapComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item

	// This code is used to draw each one of the items in the combobox

	// First, check if we are drawing and item
	if(lpDrawItemStruct->itemID == -1)
		return;

	CFtrLayer *pLayer = (CFtrLayer*)this->GetItemData(lpDrawItemStruct->itemID);
	if( !CheckLayerValid(pLayer) )
		return;

	if (pLayer == NULL)
		return;

	CDC			pDC;	
	CString		sItem;
	CBrush		pBrush;
	CBitmap		pLampOn, pLampOff, pPadlockOpened, pPadlockClosed, *pOldLamp, *pOldPadlock;
	CDC			pBmDC;
	//DWORD		dwTeste = 0;

	pDC.Attach(lpDrawItemStruct->hDC);

	// Load the lamp bitmaps
	pLampOn.LoadBitmap(IDB_LAMP_ON);
	pLampOff.LoadBitmap(IDB_LAMP_OFF);
	pPadlockOpened.LoadBitmap(IDB_LOCK_ON);
	pPadlockClosed.LoadBitmap(IDB_LOCK_OFF);
	pBmDC.CreateCompatibleDC(NULL);
	
	// Move the drawing rectangle 18 pixels left, so that we can draw 
	// the lamp
	lpDrawItemStruct->rcItem.left += 54;

	// If we have the focus, draw the item with a blue background
	// if we haven't, just plain blank
	if(lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		pBrush.CreateSolidBrush(RGB(49,106,197));
		pDC.FillRect(&lpDrawItemStruct->rcItem, &pBrush);
		pDC.SetTextColor(RGB(255,255,255));
	}
	else
	{
		pBrush.CreateSolidBrush(RGB(255,255,255));
		pDC.FillRect(&lpDrawItemStruct->rcItem, &pBrush);
		pDC.SetTextColor(RGB(0,0,0));
	}
	
	// Copy the text of the item to a string
	this->GetLBText(lpDrawItemStruct->itemID, sItem);
	pDC.SetBkMode(TRANSPARENT);

	// Draw the text
	pDC.DrawText(sItem, &lpDrawItemStruct->rcItem, DT_VCENTER | DT_SINGLELINE);

	
	// Test the bit representing the state of the lamp
	if(pLayer->IsVisible())
	{
		pOldLamp = pBmDC.SelectObject(&pLampOn);
	}
	else
	{
		pOldLamp = pBmDC.SelectObject(&pLampOff);
	}
		
	int offpix = 0;
	if (lpDrawItemStruct->hwndItem != m_hWnd)
	{
		offpix = g_Btn0Rect.Width()+5+BW_LAYERGROUPCOMBOX_WIDTH;
	}

	pDC.BitBlt(1+offpix,lpDrawItemStruct->rcItem.top ,16,16,&pBmDC,0,0, SRCCOPY);

	pBmDC.SelectObject(pOldLamp);

	if(pLayer->IsLocked())
	{
		pOldPadlock = pBmDC.SelectObject(&pPadlockOpened);
	}
	else
	{
		pOldPadlock = pBmDC.SelectObject(&pPadlockClosed);
	}
		
	pDC.BitBlt(16+offpix,lpDrawItemStruct->rcItem.top+1 ,16,16,&pBmDC,0,0, SRCCOPY);

	pBmDC.SelectObject(pOldPadlock);

	CRect colRect(CPoint(37+offpix,lpDrawItemStruct->rcItem.top+2),CSize(13,13));

	pBrush.DeleteObject();
	pBrush.CreateSolidBrush(pLayer->GetColor());
	//pDC.FillRect(&colRect, &pBrush);
	CBrush *pOldBrush = pDC.SelectObject(&pBrush);

	CPen pen;
    pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	CPen *pOldPen = pDC.SelectObject(&pen);
	pDC.Rectangle(&colRect);

	pDC.SelectObject(pOldBrush);
	pDC.SelectObject(pOldPen);		


	pBmDC.DeleteDC();
	pDC.Detach();
}

void CLayerBitmapComboBox::OnDropdown() 
{
	// TODO: Add your control notification handler code here
	g_pLayerComboBox = this;
	
	if (m_pToolbarComboxButton == NULL) return;

	CBaseBitmapComboBox::OnDropdown();

	BOOL bCtrl = GetKeyState(VK_CONTROL)<0;

	CDlgDoc *pDoc = GetActiveDlgDoc();

	int idx = GetCurSel();
	DWORD_PTR dwSelData = 0;
	if (idx >= 0)
	{
		dwSelData = GetItemData(idx);
	}

	m_pToolbarComboxButton->RemoveAllItems();

	if (pDoc == NULL) return;
	
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if (pDS)
	{
		CFtrLayer *pCurLayer = pDS->GetCurFtrLayer();
		int selIndex = -1;
		for (int i=0; i<pDS->GetFtrLayerCount(); i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
			
			if (pLayer && (bCtrl || (pLayer->GetEditableObjsCount()>0 || pLayer==pCurLayer)))
			{
				CString str;
				str.Format("%s(%I64d)",pLayer->GetName(),pDS->GetFtrLayerCode(pLayer->GetName()));
				int index = m_pToolbarComboxButton->AddItem(str, DWORD_PTR(pLayer));
				if (dwSelData == (DWORD_PTR)pLayer || (dwSelData == 0 && pCurLayer == pLayer))
				{
					selIndex = index;
				}
			}
		}
		
		m_pToolbarComboxButton->SelectItem(selIndex);
	}

	SetFocus();
}


IMPLEMENT_DYNAMIC(CFtrWidthBitmapComboBox,CBaseBitmapComboBox)

CFtrWidthBitmapComboBox::CFtrWidthBitmapComboBox()
{
	float wid[25] = {-1, 0, 0.05, 0.09, 0.13, 0.15, 0.18, 0.2, 0.25, 0.3, 0.35, 0.4, 0.5, 0.53, 0.6, 
		0.7, 0.8, 0.9, 1, 1.06, 1.2, 1.4, 1.58, 2, 2.11};

	for (int i=0; i<25; i++)
	{
		m_arrFtrWids.Add(wid[i]);
	}

	m_pToolbarComboxButton = NULL;
}

CFtrWidthBitmapComboBox::~CFtrWidthBitmapComboBox()
{
}


BEGIN_MESSAGE_MAP(CFtrWidthBitmapComboBox, CBaseBitmapComboBox)
	//{{AFX_MSG_MAP(CMultiCombo)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CTLCOLORLISTBOX, OnCtlColorListBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiCombo message handlers
LRESULT CFtrWidthBitmapComboBox::OnCtlColorListBox(WPARAM wParam, LPARAM lParam) 
{
	// Here we need to get a reference to the listbox of the combobox
	// (the dropdown part). We can do it using 
	if (this->m_hListBox == 0) {
		HWND hWnd = (HWND)lParam;

		if (hWnd != 0 && hWnd != m_hWnd) {
			// Save the handle
			m_hListBox = hWnd;

			// Subclass ListBox
			//g_pFtrWndProc = (WNDPROC)GetWindowLong(m_hListBox, GWL_WNDPROC);
			//SetWindowLong(m_hListBox, GWL_WNDPROC, (LONG_PTR)MultiComboBoxListBoxProc);
		}
	}

	
	return DefWindowProc(WM_CTLCOLORLISTBOX, wParam, lParam);
}

void CFtrWidthBitmapComboBox::OnDropdown() 
{
	// TODO: Add your control notification handler code here
	
	if (m_pToolbarComboxButton == NULL) return;
	
	CBaseBitmapComboBox::OnDropdown();
	
	CDlgDoc *pDoc = GetActiveDlgDoc();
	
	m_pToolbarComboxButton->RemoveAllItems();

	if (pDoc == NULL) return;

	int num;
	const FTR_HANDLE *pFtrs = pDoc->GetSelection()->GetSelectedObjs(num);

	float wid = 0;
	if (num > 0 && HandleToFtr(pFtrs[0])->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		wid = ((CGeoCurve*)HandleToFtr(pFtrs[0])->GetGeometry())->m_fLineWidth;		
	}

	int selIndex = -1;
	for (int i=0; i<m_arrFtrWids.GetSize(); i++)
	{
		CString str;
		if (m_arrFtrWids[i] < 0)
		{
			str = _T("ByLayer");
		}
		else
		{
			str.Format("%.2f %s",m_arrFtrWids[i],StrFromResID(IDS_FTR_WIDMMETER));
		}
		int index = m_pToolbarComboxButton->AddItem(str, DWORD_PTR(&m_arrFtrWids[i]));

		if (fabs(m_arrFtrWids[i]-wid) < 1e-4)
		{
			selIndex = index;
		}
	}

	if (selIndex == -1)
	{
		CString str;
		str.Format("%.2f %s",wid,StrFromResID(IDS_FTR_WIDMMETER));
		selIndex = m_pToolbarComboxButton->AddItem(str);
	}
	
	m_pToolbarComboxButton->SelectItem(selIndex);

	SetFocus();
}

void CFtrWidthBitmapComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item

	// This code is used to draw each one of the items in the combobox

	// First, check if we are drawing and item
	if(lpDrawItemStruct->itemID == -1)
		return;

// 	CFtrLayer *pLayer = (CFtrLayer*)this->GetItemData(lpDrawItemStruct->itemID);
// 	if (pLayer == NULL)
// 		return;

	CDC			pDC;	
	CString		sItem;
	CBrush		pBrush;
	//DWORD		dwTeste = 0;

	pDC.Attach(lpDrawItemStruct->hDC);
	
	lpDrawItemStruct->rcItem.left += 45;

	if(lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		pBrush.CreateSolidBrush(RGB(49,106,197));
		pDC.FillRect(&lpDrawItemStruct->rcItem, &pBrush);
		pDC.SetTextColor(RGB(255,255,255));
	}
	else
	{
		pBrush.CreateSolidBrush(RGB(255,255,255));
		pDC.FillRect(&lpDrawItemStruct->rcItem, &pBrush);
		pDC.SetTextColor(RGB(0,0,0));
	}
	
	// Copy the text of the item to a string
	this->GetLBText(lpDrawItemStruct->itemID, sItem);
	pDC.SetBkMode(TRANSPARENT);

	// Draw the text
	pDC.DrawText(sItem, &lpDrawItemStruct->rcItem, DT_VCENTER | DT_SINGLELINE);
		
	int offpix = 0;
	if (lpDrawItemStruct->hwndItem != m_hWnd)
	{
		offpix = BW_FTRCOLORCOMBOX_WIDTH + BW_FTRSYMCOMBOX_WIDTH + g_Btn0Rect.Width()+7;
	}

	int y = (lpDrawItemStruct->rcItem.bottom+lpDrawItemStruct->rcItem.top)/2;

	int wid = 1;
	DWORD_PTR data = GetItemData(lpDrawItemStruct->itemID);
	if (data != 0)
	{
		float wid0 = *(float*)GetItemData(lpDrawItemStruct->itemID);
		if (wid0 >= 0)
		{
			wid = wid0*10/2;
		}		
	}	

	if (wid > 10)
	{
		wid = 10;
	}

	CPen pen;
    pen.CreatePen(PS_SOLID, wid, RGB(0,0,0));
	CPen *pOldPen = pDC.SelectObject(&pen);
	pDC.MoveTo(offpix+1,y);
	pDC.LineTo(offpix+41,y);
	pDC.SelectObject(pOldPen);

	pDC.Detach();
}


IMPLEMENT_DYNAMIC(CFtrSymBitmapComboBox,CBaseBitmapComboBox)

CFtrSymBitmapComboBox::CFtrSymBitmapComboBox()
{
	m_pToolbarComboxButton = NULL;
}

CFtrSymBitmapComboBox::~CFtrSymBitmapComboBox()
{
}


BEGIN_MESSAGE_MAP(CFtrSymBitmapComboBox, CBaseBitmapComboBox)
	//{{AFX_MSG_MAP(CMultiCombo)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CTLCOLORLISTBOX, OnCtlColorListBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiCombo message handlers
LRESULT CFtrSymBitmapComboBox::OnCtlColorListBox(WPARAM wParam, LPARAM lParam) 
{
	// Here we need to get a reference to the listbox of the combobox
	// (the dropdown part). We can do it using 
	if (this->m_hListBox == 0) {
		HWND hWnd = (HWND)lParam;

		if (hWnd != 0 && hWnd != m_hWnd) {
			// Save the handle
			m_hListBox = hWnd;

			// Subclass ListBox
			//g_pFtrWndProc = (WNDPROC)GetWindowLong(m_hListBox, GWL_WNDPROC);
			//SetWindowLong(m_hListBox, GWL_WNDPROC, (LONG_PTR)MultiComboBoxListBoxProc);
		}
	}

	
	return DefWindowProc(WM_CTLCOLORLISTBOX, wParam, lParam);
}

BOOL IsFtrLinetype(CFeature *pFtr)
{
	if (!pFtr) return FALSE;

	BOOL bLinetype = TRUE;
	CGeometry *pGeo = pFtr->GetGeometry();
	if (!pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		bLinetype = FALSE;
	}

	return bLinetype;
}

void CFtrSymBitmapComboBox::OnDropdown() 
{
	// TODO: Add your control notification handler code here
	
	if (m_pToolbarComboxButton == NULL) return;
	
	CBaseBitmapComboBox::OnDropdown();

	BOOL bCtrl = GetKeyState(VK_CONTROL)<0;
	
	CDlgDoc *pDoc = GetActiveDlgDoc();
	
	m_pToolbarComboxButton->RemoveAllItems();

	if (pDoc == NULL) return;

	int num;
	const FTR_HANDLE *pFtrs = pDoc->GetSelection()->GetSelectedObjs(num);
	CString	strSymName;
	BOOL bLinetype = TRUE;
	if (num > 0) 
	{
		strSymName = ((CGeoCurve*)HandleToFtr(pFtrs[0])->GetGeometry())->GetSymbolName();
		bLinetype = IsFtrLinetype(HandleToFtr(pFtrs[0]));
	}
	
	m_pToolbarComboxButton->AddItem("ByLayer");
	m_pToolbarComboxButton->AddItem("*");

	int selIndex = 0;
	if (strSymName.IsEmpty())
	{
		selIndex = 0;
	}
	if (strSymName.CompareNoCase(_T("*")) == 0)
	{
		selIndex = 1;
	}
	
	if (bLinetype)
	{
		CBaseLineTypeLib *pLineLib = GetBaseLineTypeLib();

		for (int i=0; i<pLineLib->GetBaseLineTypeCount(); i++)
		{
			CString str = pLineLib->GetBaseLineType(i).m_name;
			int index = m_pToolbarComboxButton->AddItem(str, DWORD_PTR(i));
			
			if (strSymName.Find(str) >= 0)
			{
				selIndex = index;
			}
		}
	}
	else
	{
		CCellDefLib *pCellLib = GetCellDefLib();

		for (int i=0; i<pCellLib->GetCellDefCount(); i++)
		{
			CString str = pCellLib->GetCellDef(i).m_name;
			int index = m_pToolbarComboxButton->AddItem(str, DWORD_PTR(i));
			
			if (strSymName.Find(str) >= 0)
			{
				selIndex = index;
			}
		}
	}
	
	m_pToolbarComboxButton->SelectItem(selIndex);

	SetFocus();
}

#include "Feature.h"
BOOL CFtrSymBitmapComboBox::GetSymGrBuffer(CString name, GrBuffer2d *pBuf)
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (!pDoc) return FALSE;

	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if (!pDS) return FALSE;

	CFeature *pFtr = NULL;
	
	CPermanentExchanger *pFtrExchanger = gpMainWnd->GetPermanentExchangerOfPropertiesView();	
	if (pFtrExchanger)
	{
		pFtr = (CFeature*)pFtrExchanger->GetObject(0);
	}

// 	int num;
// 	const FTR_HANDLE *pFtrs = pDoc->GetSelection()->GetSelectedObjs(num);
	
	if (name.CompareNoCase(_T("ByLayer")) == 0)
	{
		if (!pFtr)
		{
			pBuf->BeginLineString(0,0);
			pBuf->MoveTo(&PT_2D(0,0));
			pBuf->LineTo(&PT_2D(20,0));
			pBuf->End();
		}
		else
		{
			CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
			if (pScheme)
			{
				int layid = pDoc->GetFtrLayerIDOfFtr(FtrToHandle(pFtr));
				CFtrLayer *pFtrLayer = pDS->GetFtrLayer(layid);
				if (!pFtrLayer) return FALSE;
				GrBuffer buf;
				pScheme->GetLayerDefinePreviewGrBuffer(pFtrLayer->GetName(),&buf);
				pBuf->AddBuffer(&buf);
			}
		}

		return TRUE;
	}
	else if (name.CompareNoCase(_T("*")) == 0)
	{
		pBuf->BeginLineString(0,0);
		pBuf->MoveTo(&PT_2D(0,0));
		pBuf->LineTo(&PT_2D(20,0));
		pBuf->End();

		return TRUE;
	}

	BOOL bLinetype = TRUE;
	if (pFtr)
	{
		bLinetype = IsFtrLinetype(pFtr);
	}

	if (bLinetype)
	{
		CBaseLineTypeLib *pLineLib = GetBaseLineTypeLib();
		
		int idx = pLineLib->GetBaseLineTypeIndex(name);
		if (idx >= 0)
		{
			BaseLineType baseLine = pLineLib->GetBaseLineType(idx);

			CFeature ftr;
			ftr.CreateGeometry(CLS_GEOCURVE);
			CGeometry *pGeo = ftr.GetGeometry();
			PT_3DEX pts[2];
			pts[0].x = 0;
			pts[0].y = 0;
			pts[0].pencode = penLine;
			pts[1].x = 20;
			pts[1].y = 0;
			pts[1].pencode = penLine;
			pGeo->CreateShape(pts,2);
			
			CDashLinetype dash;
			dash.m_strBaseLinetypeName = baseLine.m_name;
			
			GrBuffer dashBuf;
			dash.Draw(&ftr,&dashBuf);
			pBuf->AddBuffer(&dashBuf);

			return TRUE;
		}
	}
	else
	{
		CCellDefLib *pCellLib = GetCellDefLib();
		
		int idx = pCellLib->GetCellDefIndex(name);
		if (idx >= 0)
		{
			CellDef cell = pCellLib->GetCellDef(idx);
			pBuf->AddBuffer(cell.m_pgr);

			return TRUE;
		}
	}

	return FALSE;
}

#include "CollectionViewBar.h"
void CFtrSymBitmapComboBox::DrawImageItem(GrBuffer2d *pBuf, HDC destHdc, CRect destRect)
{
	//创建内存设备
	HDC hDC = ::CreateCompatibleDC(destHdc);
	if( !hDC )return;
	
	int cx = destRect.Width(), cy = destRect.Height();
	HBITMAP hBmp = ::CreateCompatibleBitmap(destHdc,cx,cy);
	if( !hBmp )
	{
		::DeleteDC(hDC);
		return;
	}
	
	CBitmap bm;
	bm.Attach(hBmp);

	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDC, hBmp);
	
	CBrush br(RGB(255,255,255));
	::FillRect(hDC,CRect(0,0,cx,cy),(HBRUSH)br);

	CPen pen;
	pen.CreatePen(PS_SOLID,0,(COLORREF)0);
	HPEN hOldPen = (HPEN)::SelectObject(hDC,(HPEN)pen);

	//计算变换系数
	CRect rect(1,1,cx-2,cy-2);
	Envelope e = pBuf->GetEnvelope();
	
	float scalex = rect.Width()/(e.m_xh>e.m_xl?(e.m_xh-e.m_xl):1e-10);
	float scaley = rect.Height()/(e.m_yh>e.m_yl?(e.m_yh-e.m_yl):1e-10);

	float xoff=0, yoff=0;
	if( scalex>scaley )
	{
		xoff = cx/2-((e.m_xh-e.m_xl)/2*scaley+rect.left);
		scalex = scaley;
	}
	else
	{
		yoff = cy/2-((e.m_yh-e.m_yl)/2*scalex+rect.top);
	}

	double matrix[9] = {
		scalex,0,-e.m_xl*scalex+rect.left+xoff+0.5,
			0,-scalex,cy+e.m_yl*scalex-rect.top-yoff-0.5,
			0,0,1
	};

	GDI_DrawGrBuffer2d(hDC,pBuf,TRUE,0,matrix,CSize(cx,cy),CRect(0,0,cx,cy));
	
	BitBlt(destHdc,destRect.left,destRect.top,destRect.Width(),destRect.Height(),hDC,0,0,SRCCOPY);
	
	::SelectObject(hDC,hOldBmp);
	::SelectObject(hDC,hOldPen);

	::DeleteDC(hDC);
	::DeleteObject(hBmp);
}

void CFtrSymBitmapComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item

	// This code is used to draw each one of the items in the combobox

	// First, check if we are drawing and item
	if(lpDrawItemStruct->itemID == -1)
		return;

//	CDlgDoc *pDoc = GetActiveDlgDoc();
//	if (!pDoc) return;

	CDC			pDC;	
	CString		sItem;
	CBrush		pBrush;
	//DWORD		dwTeste = 0;

	pDC.Attach(lpDrawItemStruct->hDC);
	
	long lSymWidth = 60;

	lpDrawItemStruct->rcItem.left += (lSymWidth+5);

	if(lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		pBrush.CreateSolidBrush(RGB(49,106,197));
		pDC.FillRect(&lpDrawItemStruct->rcItem, &pBrush);
		pDC.SetTextColor(RGB(255,255,255));
	}
	else
	{
		pBrush.CreateSolidBrush(RGB(255,255,255));
		pDC.FillRect(&lpDrawItemStruct->rcItem, &pBrush);
		pDC.SetTextColor(RGB(0,0,0));
	}
	
	// Copy the text of the item to a string
	this->GetLBText(lpDrawItemStruct->itemID, sItem);
	pDC.SetBkMode(TRANSPARENT);

	// Draw the text
	pDC.DrawText(sItem, &lpDrawItemStruct->rcItem, DT_VCENTER | DT_SINGLELINE);
		
	int offpix = 0;
	if (lpDrawItemStruct->hwndItem != m_hWnd)
	{
		offpix = BW_FTRCOLORCOMBOX_WIDTH + g_Btn0Rect.Width()+5;
	}
	
	GrBuffer2d buf;
	if (GetSymGrBuffer(sItem,&buf))
	{
		DrawImageItem(&buf,lpDrawItemStruct->hDC,CRect(offpix+1,lpDrawItemStruct->rcItem.top,offpix+lSymWidth,lpDrawItemStruct->rcItem.bottom));
	}

	pDC.Detach();
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CUIFToolbarBaseCustomComboBoxButton, CUIFToolbarComboBoxButtonEx, 1)

CUIFToolbarBaseCustomComboBoxButton::CUIFToolbarBaseCustomComboBoxButton()
{
}

CUIFToolbarBaseCustomComboBoxButton::~CUIFToolbarBaseCustomComboBoxButton()
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (pDoc)
	{
		pDoc->EndBatchUpdate();
	}
}

void CUIFToolbarBaseCustomComboBoxButton::RefreshComboBox(CDlgDoc* pDoc)
{
	
}

CComboBox* CUIFToolbarBaseCustomComboBoxButton::CreateCombo (CWnd* pWndParent, const CRect& rect)
{
	return NULL;
}

void CUIFToolbarBaseCustomComboBoxButton::CopyFrom(const CMFCToolBarButton& src)
{
	CUIFToolbarComboBoxButtonEx::CopyFrom(src);

	const CUIFToolbarBaseCustomComboBoxButton& layerSrc = (const CUIFToolbarBaseCustomComboBoxButton&)src;
}

void CUIFToolbarBaseCustomComboBoxButton::Serialize (CArchive& ar)
{
	CMFCToolBarButton::Serialize(ar);
	
	if (ar.IsLoading ())
	{
		ar >> m_iWidth;
		m_rect.right = m_rect.left + m_iWidth;
		ar >> m_dwStyle;
		//ar >> m_iSelIndex;
		ar >> m_strEdit;
		ar >> m_nDropDownHeight;
	}
	else
	{
		ar <<  m_iWidth;
		ar << m_dwStyle;
		//ar << m_iSelIndex;
		ar << m_strEdit;
		ar << m_nDropDownHeight;
	}
}

void CUIFToolbarBaseCustomComboBoxButton::OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages,
						BOOL bHorz, BOOL bCustomizeMode,
						BOOL bHighlight,
						BOOL bDrawBorder, BOOL bGrayDisabledButtons)
{
	if (m_pWndCombo == NULL || m_pWndCombo->GetSafeHwnd () == NULL || !bHorz || GetActiveDlgDoc() == NULL)
	{
		CMFCToolBarButton::OnDraw(pDC, rect, pImages,bHorz, bCustomizeMode,bHighlight, bDrawBorder, bGrayDisabledButtons);
		return;
	}

	BOOL bDisabled = (bCustomizeMode && !IsEditable ()) || (!bCustomizeMode && (m_nStyle & TBBS_DISABLED));
		
	pDC->SetTextColor (bDisabled ?GetGlobalData()->clrGrayedText : (bHighlight) ? CMFCToolBar::GetHotTextColor() : GetGlobalData()->clrBarText);

	if (m_bFlat)
	{
		if (m_bIsHotEdit)
		{
			bHighlight = TRUE;
		}
		
		//--------------
		// Draw combbox:
		//--------------
		CRect rectCombo = m_rectCombo;
		rectCombo.top -= 1;
		rectCombo.bottom -= 2;

		//-------------
		// Draw border:
		//-------------
		CMFCVisualManager::GetInstance ()->OnDrawComboBorder (
			pDC, rectCombo, bDisabled, m_pWndCombo->GetDroppedState (),
			bHighlight, this);

		rectCombo.DeflateRect (2, 2);

		int nPrevTextColor = pDC->GetTextColor();

		pDC->FillSolidRect (rectCombo, bDisabled ? GetGlobalData()->clrBarFace : GetGlobalData()->clrWindow);

		if (bDisabled)
		{
			pDC->Draw3dRect (&rectCombo,GetGlobalData()->clrBarHilite,GetGlobalData()->clrBarHilite);
		}

		//-----------------------
		// Draw drop-down button:
		//-----------------------
		CRect rectButton = m_rectButton;
		rectButton.top -= 1;
		rectButton.bottom -= 2;
		CMFCVisualManager::GetInstance()->OnDrawComboDropButton(pDC, rectButton, bDisabled, m_pWndCombo->GetDroppedState(), bHighlight, this);

		pDC->SetTextColor(nPrevTextColor);

		//-----------------
		// Draw combo text:
		//-----------------
		if (!m_strEdit.IsEmpty ())
		{
			CRect rectText = rectCombo;
			rectText.right = m_rectButton.left;
			rectText.DeflateRect (2, 2);

			if (m_pWndEdit == NULL)
			{
				if (m_pWndCombo->GetStyle () & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE))
				{
					DRAWITEMSTRUCT dis;
					memset (&dis, 0, sizeof (DRAWITEMSTRUCT));

					CRect comboRect;
					m_pWndCombo->GetClientRect(&comboRect);

					dis.hDC = pDC->GetSafeHdc ();
					dis.rcItem = rectText;
					dis.CtlID = m_nID;
					dis.itemID = m_pWndCombo->GetCurSel ();
					dis.hwndItem = 0;//m_pWndCombo->GetSafeHwnd ();
					dis.CtlType = ODT_COMBOBOX;

					m_pWndCombo->DrawItem (&dis);
				}
				else
				{
					pDC->DrawText (m_strEdit, rectText, DT_VCENTER | DT_SINGLELINE);
				}
			}
			
		}

		pDC->SetTextColor (nPrevTextColor);
	}

	if ((m_bTextBelow && bHorz) && !m_strText.IsEmpty())
	{
		CRect rectText = rect;
		rectText.top = (m_rectCombo.bottom + rect.bottom - m_sizeText.cy) / 2;
		
		pDC->DrawText (m_strText, &rectText, DT_CENTER | DT_WORDBREAK);
	}
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CUIFToolbarLayerGroupComboBoxButton, CUIFToolbarBaseCustomComboBoxButton, 1)

CUIFToolbarLayerGroupComboBoxButton::CUIFToolbarLayerGroupComboBoxButton()
{
}

CUIFToolbarLayerGroupComboBoxButton::~CUIFToolbarLayerGroupComboBoxButton()
{
	
}

void CUIFToolbarLayerGroupComboBoxButton::RefreshComboBox(CDlgDoc* pDoc)
{
	CUIFToolbarBaseCustomComboBoxButton::RefreshComboBox(pDoc);	

	RemoveAllItems();

	if (!pDoc) pDoc = GetActiveDlgDoc();
	if (!pDoc) return;
	
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if (!pDS) return;
	
	CFtrLayer *pSelLayer = NULL;
	CFeature *pFtr = NULL;
	
	CPermanentExchanger *pFtrExchanger = gpMainWnd->GetPermanentExchangerOfPropertiesView();	
	if (pFtrExchanger && (pFtr = (CFeature*)pFtrExchanger->GetObject(0)) && (pSelLayer = pDS->GetFtrLayerOfObject(pFtr)))
	{
	}
	else
	{
		pSelLayer = pDS->GetCurFtrLayer();
	}
	
	if (!pSelLayer) return;

	int selIndex = -1;
	for (int i=0; i<pDS->GetFtrLayerGroupCount(); i++)
	{
		FtrLayerGroup *pGroup = pDS->GetFtrLayerGroup(i);
		
		if (pGroup)
		{
			CString str = pGroup->Name;
			if (pGroup->Name.CompareNoCase(pSelLayer->GetGroupName()) == 0)
			{
				selIndex = AddItem(str, DWORD_PTR(pGroup));
				break;
			}
		}
	}
	
	SelectItem(selIndex);
	
	SetDropDownHeight(BH_COMBOX_HEIGHT);

}

CComboBox* CUIFToolbarLayerGroupComboBoxButton::CreateCombo (CWnd* pWndParent, const CRect& rect)
{
	CLayerGroupBitmapComboBox* pWndCombo = new CLayerGroupBitmapComboBox;
	if (!pWndCombo->Create(m_dwStyle, rect, pWndParent, m_nID))
	{
		delete pWndCombo;
		return NULL;
	}
	
	pWndCombo->m_pToolbarComboxButton = this;
	
	return pWndCombo;
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CUIFToolbarLayerComboBoxButton, CUIFToolbarBaseCustomComboBoxButton, 1)

CUIFToolbarLayerComboBoxButton::CUIFToolbarLayerComboBoxButton()
{
}

CUIFToolbarLayerComboBoxButton::~CUIFToolbarLayerComboBoxButton()
{
	
}

void CUIFToolbarLayerComboBoxButton::RefreshComboBox(CDlgDoc* pDoc)
{
	CUIFToolbarBaseCustomComboBoxButton::RefreshComboBox(pDoc);	

	RemoveAllItems();

	if (!pDoc) pDoc = GetActiveDlgDoc();
	if (!pDoc) return;
	
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if (!pDS) return;

	CFtrLayer *pSelLayer = NULL;
	CFeature *pFtr = NULL;
	
	CPermanentExchanger *pFtrExchanger = gpMainWnd->GetPermanentExchangerOfPropertiesView();	
	if (pFtrExchanger && (pFtr = (CFeature*)pFtrExchanger->GetObject(0)) && (pSelLayer = pDS->GetFtrLayerOfObject(pFtr)))
	{
		
	}
	else
	{
		pSelLayer = pDS->GetCurFtrLayer();
	}

	if (!pSelLayer) return;
		
	CString str;
	str.Format("%s(%I64d)",pSelLayer->GetName(),pDS->GetFtrLayerCode(pSelLayer->GetName()));
	
	int selIndex = AddItem(str,DWORD_PTR(pSelLayer));	
	SelectItem(selIndex);
	
	SetDropDownHeight(800);
}

CComboBox* CUIFToolbarLayerComboBoxButton::CreateCombo (CWnd* pWndParent, const CRect& rect)
{
	CLayerBitmapComboBox* pWndCombo = new CLayerBitmapComboBox;
	if (!pWndCombo->Create(m_dwStyle, rect, pWndParent, m_nID))
	{
		delete pWndCombo;
		return NULL;
	}
	
	pWndCombo->m_pToolbarComboxButton = this;

	return pWndCombo;
}

IMPLEMENT_SERIAL(CUIFToolbarFtrWidthComboBoxButton, CUIFToolbarBaseCustomComboBoxButton, 1)

CUIFToolbarFtrWidthComboBoxButton::CUIFToolbarFtrWidthComboBoxButton()
{

}

CUIFToolbarFtrWidthComboBoxButton::~CUIFToolbarFtrWidthComboBoxButton()
{

}

void CUIFToolbarFtrWidthComboBoxButton::RefreshComboBox(CDlgDoc* pDoc)
{
	CUIFToolbarBaseCustomComboBoxButton::RefreshComboBox(pDoc);	

	RemoveAllItems();

//	if (!pDoc) return;
		
	CFeature *pFtr = NULL;

	CPermanentExchanger *pFtrExchanger = gpMainWnd->GetPermanentExchangerOfPropertiesView();

// 	int num;
// 	const FTR_HANDLE *pFtrs = pDoc->GetSelection()->GetSelectedObjs(num);
	
	if (pFtrExchanger && (pFtr = (CFeature*)pFtrExchanger->GetObject(0)))
	{
		CGeometry *pGeo = pFtr->GetGeometry();
		float *wid = NULL;
		if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
		{
			wid = &((CGeoCurve*)pGeo)->m_fLineWidth;		
		}
		else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		{
			wid = &((CGeoSurface*)pGeo)->m_fLineWidth;		
		}

		if (wid == NULL)
		{
			CString str;
			str.Format("0.00 %s",StrFromResID(IDS_FTR_WIDMMETER));
			AddItem(str);
		}
		else
		{
			CString str;
			if (*wid < 0)
			{
				str = _T("ByLayer");
			}
			else
			{
				str.Format("%.2f %s",*wid,StrFromResID(IDS_FTR_WIDMMETER));
			}

			AddItem(str, DWORD_PTR(wid));
		}
		
		SelectItem(0,TRUE);
	}
	else
	{
		CString str;
		str.Format("0.00 %s",StrFromResID(IDS_FTR_WIDMMETER));
		AddItem(str);

		SelectItem(0,TRUE);
	}
	
	SetDropDownHeight(BH_COMBOX_HEIGHT);
}

CComboBox* CUIFToolbarFtrWidthComboBoxButton::CreateCombo (CWnd* pWndParent, const CRect& rect)
{
	CFtrWidthBitmapComboBox* pWndCombo = new CFtrWidthBitmapComboBox;
	if (!pWndCombo->Create (m_dwStyle, rect, pWndParent, m_nID))
	{
		delete pWndCombo;
		return NULL;
	}
	
	pWndCombo->m_pToolbarComboxButton = this;
	
	return pWndCombo;
}


IMPLEMENT_SERIAL(CUIFToolbarFtrSymComboBoxButton, CUIFToolbarBaseCustomComboBoxButton, 1)

CUIFToolbarFtrSymComboBoxButton::CUIFToolbarFtrSymComboBoxButton()
{
	
}

CUIFToolbarFtrSymComboBoxButton::~CUIFToolbarFtrSymComboBoxButton()
{
	
}

void CUIFToolbarFtrSymComboBoxButton::RefreshComboBox(CDlgDoc* pDoc)
{
	CUIFToolbarBaseCustomComboBoxButton::RefreshComboBox(pDoc);	

	RemoveAllItems();

	if (!pDoc) pDoc = GetActiveDlgDoc();
	if (!pDoc) return;
	
	CFeature *pFtr = NULL;

	CPermanentExchanger *pFtrExchanger = gpMainWnd->GetPermanentExchangerOfPropertiesView();	
	if (pFtrExchanger && (pFtr = (CFeature*)pFtrExchanger->GetObject(0)))
	{
		CString str = pFtr->GetGeometry()->GetSymbolName();
		
		CString symName;

		if (str.IsEmpty())
		{
			symName = _T("ByLayer");	
		}
		else if (str.CompareNoCase(_T("*")) == 0)
		{
			symName = str;
		}
		else
		{
			symName = str.Right(str.GetLength()-1);
		}

		AddItem(symName);	
		
		SelectItem(0,TRUE);
	}
	else
	{
		AddItem(_T("ByLayer"));			
		SelectItem(0,FALSE);
	}
	
	SetDropDownHeight(BH_COMBOX_HEIGHT);
}

CComboBox* CUIFToolbarFtrSymComboBoxButton::CreateCombo (CWnd* pWndParent, const CRect& rect)
{
	CFtrSymBitmapComboBox* pWndCombo = new CFtrSymBitmapComboBox;
	if (!pWndCombo->Create (m_dwStyle, rect, pWndParent, m_nID))
	{
		delete pWndCombo;
		return NULL;
	}
	
	pWndCombo->m_pToolbarComboxButton = this;
	
	return pWndCombo;
}


IMPLEMENT_SERIAL(CUIFColorToolBarButton, CMFCColorMenuButton, VERSIONABLE_SCHEMA | 1)

CUIFColorToolBarButton::CUIFColorToolBarButton()
{
	m_bMenuMode = FALSE;
	m_pWndEdit = NULL;
	m_bEmpty = TRUE;
	//Initialize ();
}
//*****************************************************************************************
CUIFColorToolBarButton::CUIFColorToolBarButton (UINT uiCmdID, LPCTSTR lpszText, CPalette* pPalette, int iWidth) :
CMFCColorMenuButton(uiCmdID, lpszText, pPalette)
{
	m_bMenuMode = FALSE;
	m_iWidth = iWidth;
//	m_bByLayerFlag = FALSE;
	m_pWndEdit = NULL;
	m_bEmpty = TRUE;
}

CUIFColorToolBarButton::~CUIFColorToolBarButton()
{
	if (m_pWndEdit != NULL)
	{
		m_pWndEdit->DestroyWindow ();
		delete m_pWndEdit;
		m_pWndEdit = NULL;
	}
}

void CUIFColorToolBarButton::OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages,
			BOOL bHorz, BOOL bCustomizeMode, BOOL bHighlight,
			BOOL bDrawBorder, BOOL bGrayDisabledButtons)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	CRect rect1(rect);
	rect1.right -= 3;

	CMFCToolBarMenuButton::OnDraw(pDC, rect1, pImages, bHorz, bCustomizeMode,
		bHighlight, bDrawBorder, bGrayDisabledButtons);

	if (m_pWndEdit != NULL)
	{
		CDC *pEditDC = m_pWndEdit->GetDC();
		if (pEditDC)
		{
			BOOL bDisabled = (bCustomizeMode && !IsEditable ()) ||
				(!bCustomizeMode && (m_nStyle & TBBS_DISABLED));

			CRect staticRect;
			m_pWndEdit->GetClientRect(&staticRect);

			// background
			pEditDC->FillSolidRect (&staticRect, RGB(255,255,255));			
			
			// Text
			CFont* pOldFont = pEditDC->SelectObject(&GetGlobalData()->fontRegular);

			CRect rectText = staticRect;		
			rectText.left += staticRect.Height();
			
			CString strVal = FormatProperty();			
			
			if (IsEmpty())
			{
				strVal = _T("ByLayer");
			}

			pEditDC->DrawText (strVal, rectText, 
				DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
			
			// color
			CRect rectColor = staticRect;
			rectColor.left += 3;
			rectColor.right = rectColor.left + staticRect.Height()-6;
			rectColor.top+=3;
			rectColor.bottom-=3;
			
			CBrush br (m_Color == (COLORREF)-1 ? m_colorAutomatic : m_Color);

			if (IsEmpty())
			{
				br.CreateSolidBrush(RGB(255,255,255));
			}

			pEditDC->FillRect(rectColor, &br);
			pEditDC->Draw3dRect(rectColor,0,0);			
			
			// disable
			if (bDisabled)
			{
				pEditDC->FillSolidRect (staticRect, 
					bDisabled ? GetGlobalData()->clrBarFace : GetGlobalData()->clrWindow);

				pDC->Draw3dRect(&rect1, GetGlobalData()->clrBarHilite, GetGlobalData()->clrBarHilite);
			}
			
			pEditDC->SelectObject(pOldFont);
			
			m_pWndEdit->ReleaseDC(pEditDC);
		}
	}
}

void CUIFColorToolBarButton::CopyFrom(const CMFCToolBarButton& s)
{
	CMFCColorMenuButton::CopyFrom(s);

	const CUIFColorToolBarButton& layerSrc = (const CUIFColorToolBarButton&)s;

	m_iWidth = layerSrc.m_iWidth;
}

SIZE CUIFColorToolBarButton::OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz)
{
	m_bHorz = bHorz;
	m_sizeText = CSize (0, 0);
	
	if (!IsVisible())
	{
		return CSize(0,0);
	}

	return CSize(m_iWidth, sizeDefault.cy);
}

// void CUIFColorToolBarButton::SetByLayer(BOOL bByLayer)
// {
// 	m_bByLayerFlag = bByLayer;
// }

COLORREF CUIFColorToolBarButton::GetColor()
{
	CString strText;
	m_pWndEdit->GetWindowText (strText);
	
	COLORREF colorCurr = m_Color;
	if(strText==_T("ByLayer"))
	{
		m_Color = COLORREF(-1);
	}

	return m_Color;
}

void CUIFColorToolBarButton::SetColor (COLORREF clr, BOOL bNotify)
{
	CMFCColorMenuButton::SetColor(clr, bNotify);

	SetEmpty(FALSE);
}

CString CUIFColorToolBarButton::FormatProperty()
{
	ASSERT_VALID (this);
	
	CString str;	
	if (m_Color == -1)
	{
		str = _T("ByLayer");
	}
	else
		str.Format (_T("%d,%d,%d"), (int)GetRValue(m_Color), (int)GetGValue(m_Color), (int)GetBValue(m_Color));
	
	return str;
}

void CUIFColorToolBarButton::SetStyle (UINT nStyle)
{
	CMFCColorMenuButton::SetStyle(nStyle);
	
	if (m_pWndEdit != NULL && m_pWndEdit->GetSafeHwnd () != NULL)
	{
		BOOL bDisabled = (CMFCToolBar::IsCustomizeMode() && !IsEditable()) ||
			(!CMFCToolBar::IsCustomizeMode() && (m_nStyle & TBBS_DISABLED));
		
		m_pWndEdit->EnableWindow (!bDisabled);
	}
}

void CUIFColorToolBarButton::SetEmpty(BOOL bEmpty)
{
	m_bEmpty = bEmpty;
	if (m_pWndParent->GetSafeHwnd () != NULL)
	{
		m_pWndParent->InvalidateRect (m_rect);
	}
}

void CUIFColorToolBarButton::Serialize (CArchive& ar)
{
	CMFCColorMenuButton::Serialize(ar);
	
	if (ar.IsLoading ())
	{
		ar >> m_iWidth;
		m_rect.right = m_rect.left + m_iWidth;
	}
	else
	{
		ar << m_iWidth;
	}
}

void CUIFColorToolBarButton::OnChangeParentWnd (CWnd* pWndParent)
{
	CMFCColorMenuButton::OnChangeParentWnd(pWndParent);

	CRect rc;
	pWndParent->GetClientRect(&rc);

	if (m_pWndEdit && m_pWndEdit->GetSafeHwnd () != NULL)
	{
		CWnd* pWndParentCurr = m_pWndEdit->GetParent ();
		ASSERT (pWndParentCurr != NULL);
		
		if (pWndParent != NULL &&
			pWndParentCurr->GetSafeHwnd () == pWndParent->GetSafeHwnd ())
		{
			return;
		}
				
		m_pWndEdit->DestroyWindow ();
		delete m_pWndEdit;
		m_pWndEdit = NULL;
	}
	
	if (pWndParent == NULL || pWndParent->GetSafeHwnd () == NULL)
	{
		return;
	}

	m_pWndEdit = NULL;
	
	CRect rectEdit = CRect(g_Btn0Rect.Width()+3, 2, rc.Height() + m_iWidth - 15, rc.Height() - 2);
	
	CStatic* pWndEdit = new CStatic;
	DWORD dwStyle = WS_VISIBLE | WS_CHILD | SS_USERITEM;
	
	pWndEdit->Create ("",dwStyle, rectEdit, pWndParent, STATICID);
	m_pWndEdit = pWndEdit;
	
	m_pWndEdit->SetFont(&GetGlobalData()->fontRegular);
	
	m_pWndEdit->SetFocus ();

}

BOOL CUIFColorToolBarButton::NotifyCommand (int iNotifyCode)
{
	if (m_pWndEdit->GetSafeHwnd () == NULL)
	{
		return FALSE;
	}

	if (iNotifyCode == 0)
	{
		return TRUE;
	}
	
	if (m_pWndEdit->GetParent () != NULL)
	{
		m_pWndEdit->GetParent ()->InvalidateRect (m_rect);
		m_pWndEdit->GetParent ()->UpdateWindow ();
	}

	
	switch (iNotifyCode)
	{
	case EN_UPDATE:
		{
			//m_pWndEdit->GetWindowText(m_strContents);
			
			//------------------------------------------------------
			// Try set selection in ALL editboxes with the same ID:
			//------------------------------------------------------
			CObList listButtons;
			if (CMFCToolBar::GetCommandButtons(m_nID, listButtons) > 0)
			{
				for (POSITION posCombo = listButtons.GetHeadPosition (); posCombo != NULL;)
				{
					CUIFColorToolBarButton* pEdit = 
						DYNAMIC_DOWNCAST (CUIFColorToolBarButton, listButtons.GetNext (posCombo));
					
					if ((pEdit != NULL) && (pEdit != this))
					{
						//pEdit->SetContents(m_strContents);
					}
				}
			}
		}
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL CUIFColorToolBarButton::OnClick (CWnd* pWnd, BOOL bDelay)
{
	ASSERT_VALID (pWnd);
	
	m_bClickedOnMenu = FALSE;

	if (m_bDrawDownArrow && !bDelay && !m_bMenuMode)
	{
		m_bClickedOnMenu = TRUE;
	}

	if (!m_bClickedOnMenu && m_nID > 0 && m_nID != (UINT) -1 && !m_bDrawDownArrow &&
		!m_bMenuOnly)
	{
		return FALSE;
	}

	CMFCMenuBar* pMenuBar = DYNAMIC_DOWNCAST(CMFCMenuBar, m_pWndParent);

	if (m_pPopupMenu != NULL)
	{
		//-----------------------------------------------------
		// Second click to the popup menu item closes the menu:
		//-----------------------------------------------------		
		ASSERT_VALID(m_pPopupMenu);

		m_pPopupMenu->m_bAutoDestroyParent = FALSE;
		m_pPopupMenu->DestroyWindow ();
		m_pPopupMenu = NULL;

		if (pMenuBar != NULL)
		{
			pMenuBar->SetHot (NULL);
		}
	}
	else
	{
		CMFCPopupMenuBar* pParentMenu =
			DYNAMIC_DOWNCAST (CMFCPopupMenuBar, m_pWndParent);

		if (bDelay && pParentMenu != NULL && !CMFCToolBar::IsCustomizeMode())
		{
			pParentMenu->StartPopupMenuTimer (this);
		}
		else
		{
			if (pMenuBar != NULL)
			{
				CMFCToolBarMenuButton* pCurrPopupMenuButton = 
					pMenuBar->GetDroppedDownMenu();
				if (pCurrPopupMenuButton != NULL)
				{
					pCurrPopupMenuButton->OnCancelMode ();
				}
			}
			
			if (!OpenPopupMenu (pWnd))
			{
				return FALSE;
			}
		}

		if (pMenuBar != NULL)
		{
			pMenuBar->SetHot (this);
		}
	}

	if (m_pWndParent != NULL)
	{
		CRect rect = m_rect;
		rect.InflateRect (4, 4);
		m_pWndParent->InvalidateRect (rect);
	}

	return TRUE;
}