// UIFPropList.cpp : implementation file
//

#include "stdafx.h"
#include "PropList0.h"
//#include "treectrl.h"
//#include "LayersMgrDlg.h"
#include "SmartViewFunctions.h"
#include "FtrLayer.h "
#include "EditBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_HEADER		1
#define ID_SCROLL_VERT	2
#define ID_SCROLL_HORZ	21
#define ID_COLOR_POPUP  3

#define TEXT_MARGIN		4

#define BITMAPWIDTH		16

//用于复制字符串的函数
static void CopyStr(char** szDest,const char* szSource)
{
	if (!szDest || !szSource)
		return;

	*szDest = new char[strlen(szSource)+1];
	memset((*szDest),0,strlen(szSource)+1);
	strcpy(*szDest,szSource);
}


extern CString VarToString(_variant_t& var);
// {
// 	if( var.vt==VT_BOOL )
// 	{
// 		if( (bool)var )return CString("1");
// 		else return CString("0");
// 	}
// 
// 	return CString((LPCTSTR)(_bstr_t)var);
// }

extern BOOL StringToVar(LPCTSTR str, int valuetype, _variant_t& var,BOOL bAllowEmpty = FALSE);
// {
// 	if(!bAllowEmpty&&strlen(str)<=0 )
// 		return FALSE;
// 
// 	char *pos = NULL;
// 	switch( valuetype )
// 	{
// 	case VT_I2:
// 		{
// 			short v = (short)strtol(str,&pos,10);
// 			if( strlen(pos)==0 )
// 				var = v;
// 		}
// 		break;
// 	case VT_I4:
// 		{
// 			long v = (long)strtol(str,&pos,10);
// 			if( strlen(pos)==0 )
// 				var = v;
// 		}
// 		break;
// 	case VT_R4:
// 		{
// 			float v = (float)strtod(str,&pos);
// 			if( strlen(pos)==0 )
// 				var = v;
// 		}
// 		break;
// 	case VT_R8:
// 		{
// 			double v = (double)strtod(str,&pos);
// 			if( strlen(pos)==0 )
// 				var = v;
// 		}
// 		break;
// 	case VT_BSTR:
// 		var = str;
// 		break;
// 	default: return FALSE;
// 	}
// 	
// 	return TRUE;
// }

/////////////////////////////////////////////////////////////////////////////
// CLVLPropItem0

IMPLEMENT_DYNAMIC(CLVLPropItem0, CObject)

CLVLPropItem0::CLVLPropItem0()
{	
	Init ();
}
//******************************************************************************************
void CLVLPropItem0::Init ()
{
	m_pWndInPlace = NULL;
	m_pWndList = NULL;
	m_bInPlaceEdit = FALSE;
	m_bInColorSel  = FALSE;
	m_Rect.SetRectEmpty ();

	m_dwData		= 0;

	m_bIsInput		= false;
	m_bReadOnlyRefCol = FALSE;
}
//*******************************************************************************************
CLVLPropItem0::~CLVLPropItem0()
{
	RemoveAllValues();

	OnDestroyWindow ();
}
//******************************************************************************************
void CLVLPropItem0::OnDestroyWindow ()
{
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->DestroyWindow ();
		delete m_pWndInPlace;
		m_pWndInPlace = NULL;
	}
}
//*******************************************************************************************
BOOL CLVLPropItem0::HitTest (CPoint point)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	if (m_Rect.PtInRect (point))
	{
		return TRUE;
	}

	return FALSE;
}

//*******************************************************************************************
void CLVLPropItem0::Redraw ()
{
	ASSERT_VALID (this);

	if (m_pWndList != NULL)
	{
		ASSERT_VALID (m_pWndList);
		m_pWndList->RedrawWindow (m_Rect);
	}
}
//*******************************************************************************************
BOOL CLVLPropItem0::IsSelected () const
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	return m_pWndList->IsInSelection(this);
}


void CLVLPropItem0::RemoveAllValues()
{
	int nsz = m_PropValues.GetSize();
	for( int i=0; i<nsz; i++)
	{
		_variant_t *p = m_PropValues.GetAt(i);
		if( p )delete p;
		m_PropValues.SetAt(i,NULL);
	}
}

void CLVLPropItem0::SetColumnCount(int num)
{
	RemoveAllValues();
	m_PropValues.SetSize(num);
	for( int i=0; i<num; i++)
	{
		_variant_t *p = new _variant_t;
		m_PropValues.SetAt(i,p);
	}
}

//******************************************************************************************
void CLVLPropItem0::SetValue (const _variant_t& varValue,int index)
{
	ASSERT_VALID (this);

	CLVLPropColumn0 item = m_pWndList->GetColumn(index);
	if( varValue.vt!=VT_EMPTY && item.ValueType!=varValue.vt&&!(m_pWndList->GetFilterItem()==this&&strcmp(item.FieldName,FIELDNAME_LAYUSED)==0) )
	{
		ASSERT (FALSE);
		return;
	}

	BOOL bInPlaceEdit = m_bInPlaceEdit;
	if (bInPlaceEdit)
	{
		OnEndEdit();
	}

	*m_PropValues[index] = varValue;
	Redraw ();
}


_variant_t CLVLPropItem0::GetValue (LPCTSTR field)const
{
	int icol = m_pWndList->GetColumnIndexByField(field);
	return *m_PropValues[icol];
}

//*******************************************************************************************
void CLVLPropItem0::SetOwnerList (CLVLPropList0* pWndList)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pWndList);

	m_pWndList = pWndList;
}
//*******************************************************************************************
void CLVLPropItem0::Repos (int x, int& y)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	m_Rect.SetRectEmpty ();

	m_Rect = CRect (
	m_pWndList->m_rectList.left-x,
	y, 
	m_pWndList->m_rectList.right+x, 
	y + m_pWndList->m_nRowHeight);
	
	m_pWndList->GetHeaderCtrl().GetItemRect(0,&m_RectName);
	m_RectName.top = m_Rect.top;
	m_RectName.bottom = m_Rect.bottom;

	y += m_pWndList->m_nRowHeight;
}

//******************************************************************************************
void CLVLPropItem0::OnDrawValue (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	if (IsSelected ()&&this!=m_pWndList->GetCurLayerPropItem())
	{
		CRect rectFill = m_Rect;
		if (m_pWndList->m_bFocused && !m_bIsInput)
		{
			CBrush br(RGB(200,200,200));
			rectFill.DeflateRect(0,1,0,1);
			pDC->FillRect (rectFill,&br);
		}
	}
	else if( this==m_pWndList->GetFilterItem() )
	{
		CRect rectFill = m_Rect;
		{
			CBrush br(RGB(255,255,160));
			rectFill.DeflateRect(0,1,0,1);
			pDC->FillRect (rectFill,&br);
		}
	}
	else if (this==m_pWndList->GetCurLayerPropItem())
	{
		CRect rectFill = m_Rect;
		if (!m_pWndList->IsInSelection(this))
		{
			CBrush br(RGB(157,223,255));
			rectFill.DeflateRect(0,1,0,1);
			pDC->FillRect (rectFill,&br);
		}
		else
		{
			CBrush br(RGB(107,203,205));
			rectFill.DeflateRect(0,1,0,1);
			pDC->FillRect (rectFill,&br);	
		}
	}

	CRect rectValue = m_RectName;
	
	CRgn rgnClipVal;
	CRect rectValClip = rectValue;
	rectValClip.bottom = min (rectValClip.bottom, m_pWndList->m_rectList.bottom);
	
	rgnClipVal.CreateRectRgnIndirect (&rectValClip);
	pDC->SelectClipRgn (&rgnClipVal);

	if (m_bIsInput)					//表示是输入过滤条件的条目
	{
		CBrush br(::GetSysColor(COLOR_INFOBK));
		pDC->FillRect(m_RectName,&br);
	}

	pDC->DrawText(m_ItemName,m_RectName,DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
	
	pDC->SelectClipRgn (NULL);

	int nCol = m_pWndList->GetColumnCount();

	for(int i = 0 ; i < nCol; i++) 
	{
		CLVLPropColumn0 col = m_pWndList->GetColumn(i);
// 		if (m_pWndList->GetFilterItem()==this&&strcmp(col.FieldName,FIELDNAME_LAYUSED)==0)
// 		{
// 			col.Type = CLVLPropColumn0::CHECK;
// 		}
		CRect rectValue = col.Rect&m_Rect;
		
		CRgn rgnClipVal;
		CRect rectValClip = rectValue;
		rectValClip.bottom = min (rectValClip.bottom, m_pWndList->m_rectList.bottom);
		
		rgnClipVal.CreateRectRgnIndirect (&rectValClip);
		pDC->SelectClipRgn (&rgnClipVal);
		
//		if(!IsSelected() || m_bIsInput)
		{
			CRect rc = rectValue;
			
			if(col.bFrame)
			{
				CBrush bs(RGB(128,128,128));
				rc = rectValue;
				rc.left = rc.right-1;
				pDC->FrameRect(&rc,&bs);

				rc = rectValue;
				rc.top = rc.bottom-1;
				pDC->FrameRect(&rc,&bs);
			}
//			CBrush br(col.DefaultColor);
//			pDC->FillRect(rt,&br);
		}

		switch(col.Type)
		{
		case CLVLPropColumn0::EDIT:
			{
				pDC->DrawText(VarToString(*m_PropValues[i]),rectValue,DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
			}
			break;
		case CLVLPropColumn0::COLOR:
			{
				CRect rectColor = rectValue;
				rectColor.right = rectColor.left + rectColor.Height ();
				rectColor.DeflateRect (1, 1);

				CRect rectText = rectValue;
				rectText.left  = rectColor.right+1;

				COLORREF ct = (long)*m_PropValues[i];
				CBrush br (ct);
				pDC->FillRect (rectColor, &br);
				pDC->Draw3dRect (rectColor, 0, 0);

				CString strColor;
				strColor.Format("%d",ct);
/*				
				CString strColor;
				strColor.Format("%x%x%x",GetRValue(ct),GetGValue(ct),GetBValue(ct));*/
				pDC->DrawText(strColor,rectText,DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
			}
			break;
/*		case CLVLPropColumn0::STYLE:
			{
				CPen pen(PS_DOT, 1 , RGB(135,135,135));
				CPen* OldPen = (CPen *) pDC->SelectObject(&pen);
				CRect rectStyle = m_PropColumn[i].Rect;
				pDC->MoveTo(rectStyle.left+4,rectStyle.top+rectStyle.Height()/2);
				pDC->LineTo(rectStyle.right - rectStyle.Height(),rectStyle.top+rectStyle.Height()/2);
				pDC->SelectObject(OldPen);
				
//				rect.left = rect.right - rect.Height();
			}
			break;
		case CLVLPropColumn0::WEIGHT:
			{
				CPen pen(PS_SOLID, 3 , RGB(135,135,135));
				CPen* OldPen = (CPen *) pDC->SelectObject(&pen);
				CRect rectStyle = m_PropColumn[i].Rect;
				pDC->MoveTo(rectStyle.left+4,rectStyle.top+rectStyle.Height()/2);
				pDC->LineTo(rectStyle.right - rectStyle.Height(),rectStyle.top+rectStyle.Height()/2);
				pDC->SelectObject(OldPen);
				
//				rect.left = rect.right - rect.Height();
			}
			break;
*/		case CLVLPropColumn0::CHECK:
			{
				bool bShow = (bool)*m_PropValues[i];
				if(bShow)
				{
					ShowBitmap(pDC,rectValue,col.BitmapOffset);
				}
			}
			break;
		case CLVLPropColumn0::RADIO:
			{
				bool bShow = (bool)*m_PropValues[i];
				if(bShow)
				{
					ShowBitmap(pDC,rectValue,col.BitmapOffset);
				}
			}
			break;
		}

		pDC->SelectClipRgn (NULL);
	}
}

//******************************************************************************************
BOOL CLVLPropItem0::OnUpdateValue (CLVLPropColumn0 * Pc)
{
	if (m_pWndList->GetFilterItem()==this)
	{
		int i=0;
	}
 	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	if (Pc->bReadOnly&&m_pWndList->GetFilterItem()!=this) return FALSE;
// 	if (strcmp(Pc->FieldName,FIELDNAME_LAYUSED)==0&&m_pWndList->GetFilterItem()!=this)
// 	{
// 		return FALSE;
// 	}
// 	
// 	if (Pc->bReadOnly&&strcmp(Pc->FieldName,FIELDNAME_LAYUSED)!=0) return FALSE;
// 	if (strcmp(Pc->FieldName,FIELDNAME_LAYUSED)==0&&m_pWndList->GetFilterItem()!=this)
// 	{
// 		return FALSE;
// 	}
	BOOL bValueBeChanged = false;

	int icol = m_pWndList->GetColumnIndexByName(Pc->ColumnName);

	_variant_t Value = *m_PropValues[icol];
	_variant_t OldValue = Value;
	switch(Pc->Type)
	{
	case CLVLPropColumn0::EDIT:
		{
			CString strEdit;
			m_pWndInPlace->GetWindowText(strEdit);
// 			if (strEdit == VarToString(Value))
// 			{
// 			}
// 			else
			{
				//这儿加东西
				bValueBeChanged = true;
			}
			BOOL bAllow = FALSE;
			if (strcmp(Pc->FieldName,FIELDNAME_LAYMAPNAME)==0)
			{
				bAllow = TRUE;
			}
			if( !StringToVar(strEdit,Pc->ValueType,Value,bAllow) )
				Value.Clear();
		}
		break;
	case CLVLPropColumn0::COLOR:
		{
			COLORREF ct = m_WndInColor.GetColor();
			if (((long)Value) == (long)ct)
			{
			}
			else
				bValueBeChanged = true;

			Value   = (long)ct;
		}
		break;
	case CLVLPropColumn0::CHECK:
		{
			bool bShow = (bool)Value;
			Value = (bool) !bShow;
			bValueBeChanged = true;
		}		
		break;
	case CLVLPropColumn0::RADIO:
		if (!(bool)Value)
		{
			bValueBeChanged = true;
			Value = (bool)true;
			for (int i=m_pWndList->GetItemCount()-1;i>=0;i--)
			{
				m_pWndList->GetPropItem(i)->SetValue((bool)false,icol);
			}
		
		}
// 		bool bShow = (bool)Value;
// 		Value = (bool) !bShow;
		
		break;
	}

	if( Value.vt==VT_EMPTY && this==m_pWndList->GetFilterItem() )
		*m_PropValues[icol] = Value;
	else if( m_PropValues[icol]->vt==VT_EMPTY || m_PropValues[icol]->vt==Value.vt )
		*m_PropValues[icol] = Value;
	
	Redraw();

	//向PropList发出数据需要改变的消息
	if( bValueBeChanged )
		m_pWndList->UpdatePropItem((CLVLPropItem0*)this,Pc,OldValue);

	return TRUE;
}


//******************************************************************************************
BOOL CLVLPropItem0::OnEdit (CLVLPropColumn0* Pc)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	if(m_bReadOnlyRefCol&&Pc->bReadOnlyRefItem) return FALSE;
	if (Pc->bReadOnly&&strcmp(Pc->FieldName,FIELDNAME_LAYUSED)!=0&&m_pWndList->GetFilterItem()!=this)
		return FALSE;

	m_pWndInPlace = CreateInPlaceEdit (Pc->Rect&m_Rect);

	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetWindowText (VarToString(GetValue(Pc->FieldName)));
		m_pWndInPlace->SetFocus ();

		m_bInPlaceEdit = TRUE;
		return TRUE;
	}

	return FALSE;
}

//******************************************************************************************
BOOL CLVLPropItem0::OnColor(CLVLPropColumn0* Pc)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP;
	
	if(::IsWindow(m_WndInColor.GetSafeHwnd()))
		m_WndInColor.MoveWindow(Pc->Rect&m_Rect);
	else
		if(!m_WndInColor.Create (NULL,dwStyle,Pc->Rect&m_Rect,m_pWndList,ID_COLOR_POPUP)) 
			return FALSE;
	m_WndInColor.ShowWindow(SW_HIDE);
	// Create color picker:
	m_WndInColor.EnableAutomaticButton (_T("ByLayer"), RGB (255, 255, 255));
	m_WndInColor.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_WndInColor.SetColumnsNumber (5);
	m_WndInColor.SetColor(COLORREF((long)GetValue(Pc->FieldName)));
	m_WndInColor.ShowColorPopup();

	m_bInColorSel = TRUE;	
	return TRUE;
}
//******************************************************************************************
CWnd* CLVLPropItem0::CreateInPlaceEdit (CRect rectEdit)
{
	if (m_pWndList->GetFilterItem()==this)
	{
		int i=0;
	}

	CEdit* pWndEdit = NULL;
	
	pWndEdit = new CEdit;
	
	DWORD dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL |WS_BORDER | ES_CENTER;
	
	pWndEdit->Create (dwStyle, rectEdit, m_pWndList, UIFROPLIST_ID_INPLACE);
	
	return pWndEdit;
}

//******************************************************************************************
CLVLColorButton0 * CLVLPropItem0::CreateInColorEdit (CRect rectColor)
{
	CLVLColorButton0* pWndColorButton = NULL;
	
	pWndColorButton = new CLVLColorButton0;
	


	return pWndColorButton;
}

//*****************************************************************************
BOOL CLVLPropItem0::OnEndEdit ()
{

	ASSERT_VALID (this);

	m_bInPlaceEdit = FALSE;
	m_bInColorSel  = FALSE;
	OnDestroyWindow ();
	return TRUE;
}

//****************************************************************************************
BOOL CLVLPropItem0::OnDblClick (CPoint /*point*/)
{
	if (m_pWndList->GetFilterItem()==this)
	{
		int i=0;
	}
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	if (m_pWndInPlace == NULL)
	{
		return FALSE;
	}

	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));

	return FALSE;
}

//****************************************************************************************
BOOL CLVLPropItem0::OnSetCursor () const
{
	return FALSE;
}

//*******************************************************************************************
HBRUSH CLVLPropItem0::OnCtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	return NULL;
}
//*******************************************************************************************
BOOL CLVLPropItem0::PushChar (UINT nChar)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);
	ASSERT (m_pWndList->IsInSelection(this));

	OnEndEdit ();
	ReleaseCapture ();
	return FALSE;
}

void CLVLPropItem0::ShowBitmap(CDC * pDC,CRect rect, int offset)
{
	if (m_pWndList->GetFilterItem()==this)
	{
		int i=0;
	}
	CDC dcMemory;
	dcMemory.CreateCompatibleDC(pDC);
	dcMemory.SelectObject(&(m_pWndList->m_bmProp));
	pDC->BitBlt(rect.left+rect.Width()/2-8,rect.top,m_pWndList->m_bmPropInfo.bmWidth/6,m_pWndList->m_bmPropInfo.bmHeight,&dcMemory,BITMAPWIDTH*offset,0,SRCAND);
	dcMemory.DeleteDC();
}

BOOL CLVLPropItem0::OnLBClick (CPoint point)
{
	if (m_pWndList->GetFilterItem()==this)
	{
		int i=0;
	}
	if (!m_pWndList || m_pWndList->GetSafeHwnd() == NULL)
		return FALSE;

	CLVLPropColumn0 col = m_pWndList->GetColumn(m_pWndList->GetSelColumnIndex());

	//参考UVS数据不允许激活
	/*if (0 == stricmp(col.ColumnName, StrFromResID(IDS_DSISACTIVE)))
	{
		if (0==GetData())
		{
			AfxMessageBox(IDS_UVS_NO_ACTIVE);
			return FALSE;
		}
	}*/
// 	if (m_pWndList->GetFilterItem()==this&&strcmp(col.FieldName,FIELDNAME_LAYUSED)==0)
// 	{
// 		col.Type = CLVLPropColumn0::CHECK;
// 	}
	if ((col.bReadOnlyRefItem&&m_bReadOnlyRefCol)||(col.bReadOnly&&m_pWndList->GetFilterItem()!=this))
	{
		return TRUE;
	}	
	switch( col.Type ) 
	{
	case CLVLPropColumn0::EDIT:
		if (m_pWndList->EditItem (this,&col) && m_pWndInPlace != NULL)
		{
			m_pWndInPlace->SetFocus();
			m_pWndInPlace->SendMessage (WM_LBUTTONDOWN);
			m_pWndInPlace->SendMessage (WM_LBUTTONUP);
			((CEdit*)m_pWndInPlace)->SetSel(0,-1);
		}
		break;
	case CLVLPropColumn0::COLOR:
		{
			m_pWndList->EditItem (this,&col);
		}
				break;
	case CLVLPropColumn0::CHECK:
	case CLVLPropColumn0::RADIO:
		OnUpdateValue(&col);
		break;
	default:;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CLVLPropList0

IMPLEMENT_DYNAMIC(CLVLPropList0, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CLVLPropList0 notification messages:

//UINT UIFM_PROPERTY_CHANGED = ::RegisterWindowMessage (_T("UIFM_PROPERTYCHANGED"));
//UINT UIFM_PROPERTY_SELECTED= ::RegisterWindowMessage (_T("UIFM_PROPERTYSELECTED"));

CLVLPropList0::CLVLPropList0()
{
	m_rectList.SetRectEmpty ();
	m_rectTrackHeader.SetRectEmpty ();
	m_nRowHeight        = 0;
	m_nHeaderHeight     = 0;
	m_nVertScrollOffset = 0;
	m_nVertScrollTotal  = 0;
	m_nVertScrollPage   = 0;

	m_bFocused          = FALSE;
	m_bTracking         = FALSE;

	m_bmProp.LoadBitmap(IDB_PROPERTY);
	m_bmProp.GetObject(sizeof(BITMAP),&m_bmPropInfo);

	m_pInEditItem		= NULL;
	m_nSelColumn		= -1;
	m_pFilterItem		= NULL;
//	m_ptLastLBD = CPoint(-1,-1);
//	m_nStart = -1;
}

CLVLPropList0::~CLVLPropList0()
{
	RemoveAll();
	m_bmProp.DeleteObject();	
}

BEGIN_MESSAGE_MAP(CLVLPropList0, CWnd)
	//{{AFX_MSG_MAP(CLVLPropList0)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETTINGCHANGE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_CANCELMODE()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_NOTIFY(HDN_ITEMCHANGED, ID_HEADER, OnHeaderItemChanged)
	ON_NOTIFY(HDN_TRACK, ID_HEADER, OnHeaderTrack)
	ON_NOTIFY(HDN_ENDTRACK, ID_HEADER, OnHeaderEndTrack)
	ON_NOTIFY(HDN_ITEMCLICK, ID_HEADER, OnHeaderClick)
	ON_COMMAND(3,OnColorChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLVLPropList0 message handlers


void CLVLPropList0::OnColorChange()
{
	ASSERT(m_pInEditItem != NULL);
	COLORREF color = m_pInEditItem->m_WndInColor.GetColor();
	if (GetSelectedCount()>=1)
	{
		CLVLPropItem0 *pSel = GetSelectedItem(0);
		CLVLPropColumn0 col = GetColumn(m_nSelColumn);
		if( col.Type == CLVLPropColumn0::COLOR)
		{
			pSel->m_bInColorSel = TRUE;
		}	
		OnSetFocus(NULL);
	}	
	if(GetSelectedCount()>1)
	{
		
// 		POSITION pos;
// 		CWnd::OnSetFocus(NULL);
// 		
// 		m_bFocused = TRUE;		
// 		
// 		int idx = GetColumnIndexByField(FIELDNAME_LAYCOLOR);
// 		BOOL bIsNeedUpdate	= false;
// 		CRect rect;
// 		rect.SetRectEmpty();
// 		for(  pos = m_lstPSels.GetHeadPosition(); pos!=NULL; )
// 		{
// 			CLVLPropItem0 *pEditItem = m_lstPSels.GetNext(pos);	
// 			
// 			if (pEditItem == NULL)
// 			{
// 				continue;
// 			}
// 			
// 			ASSERT_VALID (pEditItem);
// 			
// 			pEditItem->SetValue(_variant_t((long)(color)),idx);			
// 			
// 			bIsNeedUpdate	= true;
// 			
// 			
// 		
// 			rect+=pEditItem->m_Rect;
// 				
// 		
// 		}
// 		ReleaseCapture ();
// 		
// 		if (bIsNeedUpdate)
// 			RedrawWindow (rect);	
		;
	}
		
}

void CLVLPropList0::PreSubclassWindow() 
{
	CWnd::PreSubclassWindow();

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState ();
	if (pThreadState->m_pWndInit == NULL)
	{
		Init ();
	}
}
//******************************************************************************************
int CLVLPropList0::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	Init ();
	return 0;
}
//******************************************************************************************
void CLVLPropList0::Init ()
{
	CRect rectDummy;

	rectDummy.SetRectEmpty ();

	m_wndHeader.Create (WS_CHILD | WS_VISIBLE | HDS_HORZ | CCS_BOTTOM, rectDummy, this, ID_HEADER);
	m_wndScrollVert.Create(WS_CHILD | WS_VISIBLE | SBS_VERT, rectDummy, this, ID_SCROLL_VERT);
	m_wndScrollHorz.Create(WS_CHILD | WS_VISIBLE | SBS_HORZ, rectDummy, this, ID_SCROLL_HORZ);

	AdjustLayout ();
}
//*****************************************************************************************
void CLVLPropList0::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CClientDC dc (this);

	//设置头控件位置
	TEXTMETRIC tm;
	dc.GetTextMetrics (&tm);
	m_nRowHeight    = tm.tmHeight;
	m_nHeaderHeight = m_nRowHeight;

	CRect rectClient;
	GetClientRect (rectClient);
	
	GetHeaderCtrl ().SetWindowPos (NULL, rectClient.left-m_nColWidth*m_nHorzScrollOffset, rectClient.top,rectClient.Width ()+m_nColWidth*m_nHorzScrollOffset, m_nHeaderHeight,
		SWP_NOZORDER | SWP_NOACTIVATE);

	
	m_rectList = rectClient;
	m_rectList.top += m_nHeaderHeight;

	int	iWidth	= 0;
	int	iCount	= m_wndHeader.GetItemCount(); 
	
	for (int i = 0; i < iCount; i++)
	{	
		HDITEM	hd;
		hd.mask		= HDI_WIDTH;
		m_wndHeader.GetItem(i,&hd);
		
		iWidth += hd.cxy;
	}
	
	if (iCount == 0)
	{
		m_nColWidth = 0;
	}
	else
		m_nColWidth = iWidth/iCount;

	//设置纵向滚动条位置
	int cxScroll = ::GetSystemMetrics (SM_CXHSCROLL);
	SetScrollSizes ();

	if (m_nVertScrollTotal > 0)
	{
		m_rectList.right	-= cxScroll;
		m_wndScrollVert.SetWindowPos (NULL, m_rectList.right, m_rectList.top,
			cxScroll, m_rectList.Height (), SWP_NOZORDER | SWP_NOACTIVATE);
	}
	else
	{
		m_wndScrollVert.SetWindowPos (NULL, 0, 0,
			0, 0, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	//设置水平滚动条位置	
	int cyScroll = ::GetSystemMetrics (SM_CYVSCROLL);
	if (m_nHorzScrollTotal > 0)
	{
		m_rectList.bottom	-= cyScroll;
		m_wndScrollHorz.SetWindowPos (NULL, m_rectList.left, m_rectList.bottom,
			m_rectList.Width(), cyScroll, SWP_NOZORDER | SWP_NOACTIVATE);
	}
	else
	{
		m_wndScrollHorz.SetWindowPos (NULL, 0, 0,
			0, 0, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	ReposProperties ();
}
//******************************************************************************************
void CLVLPropList0::ReposProperties ()
{
	ASSERT_VALID (this);

	int x = m_nColWidth * m_nHorzScrollOffset;

	int y = m_rectList.top - m_nRowHeight * m_nVertScrollOffset - 1;
	int y0 = y;

	if( m_pFilterItem )
		m_pFilterItem->Repos(x,y);
	
	for (POSITION pos = m_lstProps.GetHeadPosition (); pos != NULL;)
	{
		CLVLPropItem0* pProp = m_lstProps.GetNext (pos);
		ASSERT_VALID (pProp);
		
		pProp->Repos(x,y);
	}
	
	int nCol = GetColumnCount();
	
	for(int i = 0 ; i < nCol; i++)
	{
		CLVLPropColumn0 col = GetColumn(i);
		
		GetHeaderCtrl().GetItemRect(i,col.Rect);
		col.Rect.top = y0;
		col.Rect.bottom = y;
		
		col.Rect.left -= x;
		col.Rect.right -= x;

		SetColumn(i,col);
	}
	
	return;
}

void CLVLPropList0::InitHeaderCtrl()
{
	int nCount = GetColumnCount();
	
	HDITEM hdItem;
	hdItem.fmt = HDF_CENTER;
	
	for(int i = 0; i < nCount ; i++)
	{ 
		CLVLPropColumn0 col = GetColumn(i);

		hdItem.mask = HDI_TEXT | HDI_FORMAT;
		hdItem.pszText = col.ColumnName;
		hdItem.cchTextMax = sizeof(col.ColumnName);
		m_wndHeader.InsertItem (i, &hdItem);
		
		hdItem.mask = HDI_WIDTH ;
		hdItem.cxy	= 80;

		m_wndHeader.SetItem (i, &hdItem);
	}
	
	AdjustLayout();
}

//******************************************************************************************
void CLVLPropList0::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	EndEditItem (FALSE);

	AdjustLayout ();
}
//******************************************************************************************
void CLVLPropList0::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CWnd::OnSettingChange(uFlags, lpszSection);	
	AdjustLayout ();
}
//******************************************************************************************
void CLVLPropList0::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rectClip;
	dc.GetClipBox (rectClip);

	CRect rectClient;
	GetClientRect (rectClient);

	CDC*		pDC = &dc;
	BOOL		bMemDC = FALSE;
	CDC			dcMem;
	CBitmap		bmp;
	CBitmap*	pOldBmp = NULL;

	if (dcMem.CreateCompatibleDC (&dc) &&
		bmp.CreateCompatibleBitmap (&dc, rectClient.Width (),
								  rectClient.Height ()))
	{
		//-------------------------------------------------------------
		// Off-screen DC successfully created. Better paint to it then!
		//-------------------------------------------------------------
		bMemDC = TRUE;
		pOldBmp = dcMem.SelectObject (&bmp);
		pDC = &dcMem;
	}

	pDC->FillRect(rectClient, &GetGlobalData()->brWindow);

	HFONT hfontOld = (HFONT)::SelectObject (pDC->GetSafeHdc (),GetStockObject (DEFAULT_GUI_FONT));

	pDC->SetTextColor(GetGlobalData()->clrWindowText);
	pDC->SetBkMode (TRANSPARENT);

	OnDrawList (pDC);
	::SelectObject (pDC->GetSafeHdc (), hfontOld);

	if (bMemDC)
	{
		//--------------------------------------
		// Copy the results to the on-screen DC:
		//-------------------------------------- 
		dc.BitBlt (rectClip.left, rectClip.top, rectClip.Width(), rectClip.Height(),
					   &dcMem, rectClip.left, rectClip.top, SRCCOPY);

		dcMem.SelectObject(pOldBmp);
	}
}
//******************************************************************************************
void CLVLPropList0::OnDrawList (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	CPen penLine (PS_SOLID, 1,1);
	CPen* pOldPen = pDC->SelectObject (&penLine);

	if( m_pFilterItem )
		OnDrawProperty( pDC, m_pFilterItem );

	const CList<CLVLPropItem0*, CLVLPropItem0*>& lst = m_lstProps;

	for (POSITION pos = lst.GetHeadPosition (); pos != NULL;)
	{
		CLVLPropItem0* pProp = lst.GetNext (pos);
		ASSERT_VALID (pProp);

		if (!OnDrawProperty (pDC, pProp))
		{
			break;
		}
	}

	pDC->SelectObject (pOldPen);
}

#include "DlgRefDataFile.h "
//在PropList的编辑完成后,触发修改函数
BOOL CLVLPropList0::UpdatePropItem(CLVLPropItem0 *pItem, CLVLPropColumn0* Pc, _variant_t exData)
{
	if( pItem==m_pFilterItem )
	{
		FilterPropItems();
	}
	else
	{
		((CDlgRefDataFile*)GetParent())->ModifyDataSourceAttri(pItem,Pc,exData);

	}
	return TRUE;
}


void CLVLPropList0::SetFilterItem(CLVLPropItem0 *pItem)
{
	if( m_pFilterItem )
		delete m_pFilterItem;
	m_pFilterItem = pItem;

	if( pItem )
	{
		pItem->SetOwnerList(this);
		pItem->SetColumnCount(GetColumnCount());
	}
}


void CLVLPropList0::FilterPropItems(BOOL bRedraw)
{
	CLVLPropItem0 *pItem = m_pFilterItem;

	DeselectAll(FALSE);
	m_lstProps.RemoveAll();

	int ncol = GetColumnCount();

	for( POSITION pos = m_lstAllProps.GetHeadPosition(); pos!=NULL; )
	{
		CLVLPropItem0 *pp = m_lstAllProps.GetNext(pos);

		if( pItem==NULL )
		{
			m_lstProps.AddTail(pp);
		}
		else
		{
			for( int i=0; i<ncol; i++)
			{
				
				_variant_t var1 = pItem->GetValue(i);
				_variant_t var2 = pp->GetValue(i);
				if (i==ncol-1) //地物数列是最后一列
				{
					if (var1.vt==VT_EMPTY)
					{
						continue;
					}
					bool check = (bool)var1;
					if (check)
					{
						if ((long)var2>0)
						{
							continue;
						}
					}
					else
					{
						if ((long)var2>0)
						{
							break;
						}
					}
				}

				CString str1 = VarToString(var1), str2 = VarToString(var2);
				if( str1.IsEmpty() )continue;
				if( str2.IsEmpty() )break;

				str1.MakeLower();
				str2.MakeLower();
				if( str2.Find(str1)<0 )break;
			}

			/*
			for( int i=0; i<ncol; i++)
			{
				_variant_t var1 = pItem->GetValue(i);
				_variant_t var2 = pp->GetValue(i);

				BOOL bMatch = FALSE;
				
				switch( var1.vt ) 
				{
				case VT_I2:
					bMatch = ( (short)var1==(short)var2 );
					break;
				case VT_I4:
					bMatch = ( (long)var1==(long)var2 );
					break;
				case VT_R4:
					bMatch = ( (float)var1==(float)var2 );
					break;
				case VT_R8:
					bMatch = ( (double)var1==(double)var2 );
					break;
				case VT_BSTR:
					bMatch = (strcmp((LPCTSTR)(_bstr_t)var1,(LPCTSTR)(_bstr_t)var2)==0);
					break;
				case VT_BOOL:
					bMatch = ( (bool)var1==(bool)var2 );
					break;
				default: continue;
				}

				if( !bMatch )break;
			}*/

			if( i>=ncol )m_lstProps.AddTail(pp);
		}
	}
//	m_ptLastLBD = CPoint(-1,-1);
//	m_nStart = -1;
	if( bRedraw )
	{
		AdjustLayout();
		RedrawWindow();
	}
}


//******************************************************************************************
BOOL CLVLPropList0::OnDrawProperty (CDC* pDC, CLVLPropItem0* pProp) const
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);
	ASSERT_VALID (pProp);

	if (!pProp->m_Rect.IsRectEmpty ())
	{
		if (pProp->m_Rect.top >= m_rectList.bottom)
		{
			return FALSE;
		}

		if (pProp->m_Rect.bottom >= m_rectList.top)
		{
			COLORREF clrTextOld = (COLORREF)-1;
			
			pProp->OnDrawValue (pDC);

			if (clrTextOld != (COLORREF)-1)
			{
				pDC->SetTextColor (clrTextOld);
			}
		}
	}

	return TRUE;
}
//****************************************************************************************
void CLVLPropList0::OnItemChanged (CLVLPropItem0* pProp) const
{
	ASSERT_VALID (this);
	ASSERT_VALID (pProp);
}
//*******************************************************************************************
BOOL CLVLPropList0::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}

//******************************************************************************************
void CLVLPropList0::OnHeaderItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{	
	*pResult = 0;
}
//*****************************************************************************************
void CLVLPropList0::OnHeaderTrack(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMHEADER* pHeader = (NMHEADER*) pNMHDR;

	pHeader->pitem->cxy = min (pHeader->pitem->cxy, m_rectList.Width ());

	TrackHeader (pHeader->pitem->cxy,pHeader);
	*pResult = 0;
}
//******************************************************************************************
void CLVLPropList0::OnHeaderEndTrack(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nOffset = 0;
	NMHEADER* pHeader = (NMHEADER*) pNMHDR;
	pHeader->pitem->cxy = min (pHeader->pitem->cxy, m_rectList.Width ());
    nOffset = pHeader->pitem->cxy;
	if(nOffset < m_nRowHeight)
	{
		nOffset = m_nRowHeight;
	}

	HDITEM hdItem;
	hdItem.cxy = nOffset;
	hdItem.mask = HDI_WIDTH ;
	GetHeaderCtrl ().SetItem (pHeader->iItem, &hdItem);
	ReposProperties ();
	InvalidateRect(m_rectList);
	UpdateWindow();

	if (pHeader->iItem > 0)
	{	
	}
	else
	{
	}
	

	*pResult = 0;
}

void CLVLPropList0::OnHeaderClick (NMHDR* pNMHDR , LRESULT* pResult)
{
	LPNMHEADER phdr = (LPNMHEADER)pNMHDR;

	EndEditItem (TRUE);
	SetFocus();

	if (phdr->iButton == 1)				//右键
	{
		CMenu obMenu;

		obMenu.LoadMenu(IDR_LIST_POPUP);

		m_pPopupMenu = obMenu.GetSubMenu(0);

		ASSERT(m_pPopupMenu); 

		// Get the cursor position
		CPoint obCursorPoint = (0, 0);
		
		GetCursorPos(&obCursorPoint);


		m_pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, obCursorPoint.x, 
										obCursorPoint.y, this);
	}
	else if (phdr->iButton == 0)		//左键
	{
		int nsort = m_wndHeader.GetSortColumn();
		BOOL bAscend = (nsort==phdr->iItem?!m_wndHeader.IsAscending():TRUE);
		m_wndHeader.SetSortColumn(phdr->iItem,bAscend);

		SortItems(phdr->iItem,bAscend);
	}
	
	*pResult = 0 ;
}


void CLVLPropList0::SortItems(int ncol, BOOL bAscend)
{
	int nItem = m_lstProps.GetCount(), i=0, j, k;

	CArray<CLVLPropItem0*,CLVLPropItem0*> arrItems;
	arrItems.SetSize(nItem);

	for( POSITION pos = m_lstProps.GetHeadPosition(); pos!=NULL; )
	{
		CLVLPropItem0* pProp = m_lstProps.GetNext(pos);
		arrItems.SetAt(i++,pProp);
	}

	CLVLPropItem0 **buf = arrItems.GetData(), *p1, *p2;
	for( i=0; i<nItem; i++)
	{
		p1 = buf[i];
		k = -1;
		for( j=i+1; j<nItem; j++)
		{
			p2 = buf[j];

			_variant_t var1 = p1->GetValue(ncol);
			_variant_t var2 = p2->GetValue(ncol);

			ASSERT(var1.vt==var2.vt);
			int ret = -2;

			switch( var1.vt ) 
			{
			case VT_I2:
				{
					short v1 = (short)var1, v2 = (short)var2;
					if( v1==v2 )ret = 0;
					else if( v1<v2 )ret = -1;
					else ret = 1;
				}
				break;
			case VT_I4:
				{
					long v1 = (long)var1, v2 = (long)var2;
					if( v1==v2 )ret = 0;
					else if( v1<v2 )ret = -1;
					else ret = 1;
				}
				break;
			case VT_R4:
				{
					float v1 = (float)var1, v2 = (float)var2;
					if( v1==v2 )ret = 0;
					else if( v1<v2 )ret = -1;
					else ret = 1;
				}
				break;
			case VT_R8:
				{
					double v1 = (double)var1, v2 = (double)var2;
					if( v1==v2 )ret = 0;
					else if( v1<v2 )ret = -1;
					else ret = 1;
				}
				break;
			case VT_BSTR:
				{
					ret = strcmp((LPCTSTR)(_bstr_t)var1,(LPCTSTR)(_bstr_t)var2);
					if( ret<0 )ret = -1;
					else if( ret>0 )ret = 1;
				}
				break;
			case VT_BOOL:
				{
					short v1 = ((bool)var1)?1:0, v2 = ((bool)var2)?1:0;
					if( v1==v2 )ret = 0;
					else if( v1<v2 )ret = -1;
					else ret = 1;
				}
				break;
			default: return;
			}

			if( bAscend )
			{
				if( ret==1 )
				{
					p1 = p2;
					k = j;
				}
			}
			else
			{
				if( ret==-1 )
				{
					p1 = p2;
					k = j;
				}
			}
		}

		if( k!=-1 )
		{
			buf[k] = buf[i]; 
			buf[i] = p1;
		}
	}

	m_lstProps.RemoveAll();

	for( i=0; i<nItem; i++ )
	{
		m_lstProps.AddTail(buf[i]);
	}

	AdjustLayout();
	RedrawWindow();
}

//*****************************************************************************************
void CLVLPropList0::TrackHeader (int nOffset,NMHEADER* pHeader)
{
	EndEditItem (TRUE);
	SetFocus();
}
//*****************************************************************************************
void CLVLPropList0::RemoveAll ()
{
	ASSERT_VALID (this);

	//删除数据
	while (!m_lstAllProps.IsEmpty ())
	{
		delete m_lstAllProps.RemoveHead();
	}

	m_lstAllProps.RemoveAll();
	m_lstProps.RemoveAll();
	m_lstPSels.RemoveAll();

	m_pInEditItem = NULL;

	if( m_pFilterItem )
		delete m_pFilterItem;

	m_pFilterItem = NULL;

	//删除列表
	m_arrColumns.RemoveAll();

	//界面更新
	if( m_wndHeader.GetSafeHwnd()!=NULL )
	{
		int nCount = m_wndHeader.GetItemCount();
		for(int i = 0; i< nCount; i++)
			m_wndHeader.DeleteItem(0);
	}
	
	m_bTracking = FALSE;
}


void CLVLPropList0::RemovePropItem(CLVLPropItem0 *pItem)
{
	POSITION pos = m_lstAllProps.Find( pItem );
	if( !pos )return;

	m_lstAllProps.RemoveAt( pos );

	pos = m_lstProps.Find( pItem );
	if( pos )m_lstProps.RemoveAt( pos );

	if( IsInSelection(pItem) )
		m_lstPSels.RemoveAt(m_lstPSels.Find(pItem));
	if (pItem==m_pCurLayItem)
	{
		m_pCurLayItem = NULL;
	}

	delete pItem;
}

//*******************************************************************************************
CLVLPropItem0* CLVLPropList0::HitTest (CPoint pt) const
{
	ASSERT_VALID (this);

	const CList<CLVLPropItem0*, CLVLPropItem0*>& lst = m_lstProps;

	for (POSITION pos = lst.GetHeadPosition (); pos != NULL;)
	{
		CLVLPropItem0* pProp = lst.GetNext (pos);
		ASSERT_VALID (pProp);

		if( pProp->HitTest(pt) )
		{
			return pProp;
		}
	}

	return NULL;
}

int CLVLPropList0::HitColumn(CPoint pt) const
{
	CLVLPropColumn0 col;
	int nCount = GetColumnCount();
	for(int i = 0 ; i<nCount; i++)
	{
		col = GetColumn(i);
		if( col.Rect.PtInRect(pt))
		{
			break;
		}
	}
	if( i<nCount )return i;
	return -1;
}
//*******************************************************************************************
void CLVLPropList0::SelectItem (const CLVLPropItem0* pProp, BOOL bOnlyOne, BOOL bRedraw)
{
	ASSERT_VALID (this);

	if( bOnlyOne )
	{
		if( GetSelectedCount()==1 && IsInSelection(pProp) )
			return;
		DeselectAll(bRedraw);
	}

	if( IsInSelection(pProp) )
		return;

	if( m_pInEditItem!=NULL && (m_pInEditItem->m_bInPlaceEdit || m_pInEditItem->m_bInColorSel) )
	{
		EndEditItem (TRUE);
	}

	m_lstPSels.AddTail((CLVLPropItem0*)pProp);

	if (pProp != NULL)
	{
		if (bRedraw) InvalidateRect (pProp->m_Rect);
	}

	if (bRedraw)
	{
		UpdateWindow ();
	}
}

void CLVLPropList0::DeselectItem(const CLVLPropItem0* pProp, BOOL bRedraw)
{
	ASSERT_VALID (this);
	
	if( !IsInSelection(pProp) )
		return;
	
	if( m_pInEditItem==pProp && (m_pInEditItem->m_bInPlaceEdit || m_pInEditItem->m_bInColorSel) )
	{
		EndEditItem (TRUE);
	}
	
	m_lstPSels.RemoveAt(m_lstPSels.Find((CLVLPropItem0*)pProp));
	
	if (pProp != NULL)
	{
		if (bRedraw) InvalidateRect (pProp->m_Rect);
	}
	
	if (bRedraw)
	{
		UpdateWindow ();
	}
}

void CLVLPropList0::SelectAll(BOOL bRedraw )
{
	ASSERT_VALID (this);
	
	if( m_pInEditItem!=NULL && (m_pInEditItem->m_bInPlaceEdit || m_pInEditItem->m_bInColorSel) )
	{
		EndEditItem (TRUE);
	}
	CLVLPropItem0* pProp = NULL;
	for (POSITION pos = m_lstProps.GetHeadPosition();pos!=NULL;)
	{
		pProp = m_lstProps.GetNext(pos);
		if (pProp&&!IsInSelection(pProp))
		{
			m_lstPSels.AddTail(pProp);
			if (bRedraw) InvalidateRect (pProp->m_Rect);
		}
	}
	if(bRedraw)
		UpdateWindow();
}

void CLVLPropList0::DeselectAll(BOOL bRedraw)
{
	ASSERT_VALID (this);

	if( m_pInEditItem!=NULL && (m_pInEditItem->m_bInPlaceEdit || m_pInEditItem->m_bInColorSel) )
	{
		EndEditItem (TRUE);
	}

	if( bRedraw && m_lstPSels.GetCount()>0 )
	{
		for( POSITION pos = m_lstPSels.GetHeadPosition(); pos!=NULL; )
		{
			CLVLPropItem0* pProp = m_lstPSels.GetNext(pos);
			if( pProp )
			{
				InvalidateRect (pProp->m_Rect);
			}
		}

		m_lstPSels.RemoveAll();
		UpdateWindow();
	}
	else
		m_lstPSels.RemoveAll();
}

void CLVLPropList0::SetSelColumn( int idx)
{
	if( m_nSelColumn==idx )
		return;

	EndEditItem();

	if( m_nSelColumn>=0 )
	{
		HDITEM item;
		TCHAR text[256]={0};
		item.pszText = text;
		item.mask = HDI_TEXT;
		item.cchTextMax = 256;
		GetHeaderCtrl().GetItem(m_nSelColumn,&item);	
		strcpy(text,GetColumn(m_nSelColumn).ColumnName);
		item.pszText = text;	
		GetHeaderCtrl().SetItem(m_nSelColumn,&item);
	}	

	m_nSelColumn = idx;

	if( m_nSelColumn>=0 )
	{
		HDITEM item;
		char lpBuffer[256];
		item.mask = HDI_TEXT;
		item.pszText = lpBuffer;
		item.cchTextMax = 256;

		GetHeaderCtrl().GetItem(m_nSelColumn,&item);
		
		char text[256] = "*";
		strcat(text,GetColumn(m_nSelColumn).ColumnName);
		item.pszText = text;
		
		GetHeaderCtrl().SetItem(m_nSelColumn,&item);
	}

	GetHeaderCtrl().RedrawWindow();
}

//******************************************************************************************
void CLVLPropList0::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);

/*	if(GetSelectedCount()<=0)*/

	SetFocus();

	CLVLPropItem0* pHit = HitTest (point);

	int icol = HitColumn(point);
	if( pHit==NULL && m_pFilterItem!=NULL && m_pFilterItem->HitTest(point) &&
		icol>=0 && nFlags!=MK_CONTROL&&  nFlags!=MK_SHIFT )
	{
		DeselectAll(TRUE);
		SetSelColumn(icol);
		m_pFilterItem->OnLBClick(point);
//		m_ptLastLBD = CPoint(-1,-1);
//		m_nStart = -1;
		return;
	}

	if (pHit == NULL || pHit->m_pWndList->GetSafeHwnd() == NULL &&nFlags!=MK_CONTROL&&  nFlags!=MK_SHIFT  )
	{
		EndEditItem();
		DeselectAll(TRUE);
//		m_ptLastLBD = CPoint(-1,-1);
//		m_nStart = -1;
		((CDlgRefDataFile*)GetParent())->OnSelChange();
		return;
	}

	BOOL bOldSel = IsInSelection(pHit);


	// 多选的状态下点击 check 类型的列
	if( icol>=0 && GetColumn(icol).Type==CLVLPropColumn0::CHECK && GetSelectedCount()>1 &&bOldSel )
	{
		SetSelColumn(icol);
		if( GetSelectedCount()>1 && GetColumn(icol).Type==CLVLPropColumn0::CHECK )
		{
			CList<CLVLPropItem0*, CLVLPropItem0*> lstSave;
			for( POSITION pos0 = m_lstPSels.GetHeadPosition(); pos0!=NULL; )
			{
				lstSave.AddTail(m_lstPSels.GetNext(pos0));
			}
			
			for (int i=0; i<m_lstPSels.GetCount(); i++)
			{
				POSITION pos = m_lstPSels.FindIndex(i);
				if (pos != NULL)
				{
					CLVLPropItem0 *pProp = m_lstPSels.GetAt(pos);

					// 保留一项
					m_lstPSels.RemoveAll();
					m_lstPSels.AddTail(pProp);					
					pProp->OnLBClick(point);
					//恢复
					m_lstPSels.RemoveAll();
					for( POSITION pos1 = lstSave.GetHeadPosition(); pos1!=NULL; )
					{
						m_lstPSels.AddTail(lstSave.GetNext(pos1));
					}
				}
				
			}
	
		}
	}
	else if (icol>=0 && GetColumn(icol).Type==CLVLPropColumn0::COLOR && GetSelectedCount()>1 &&bOldSel)
	{
		SetSelColumn(icol);
		if( GetSelectedCount()>1 && GetColumn(icol).Type==CLVLPropColumn0::COLOR )
		{
			EnsureVisible (pHit);			
			pHit->OnLBClick (point);
		}
	}
	else if (icol>=0 && GetColumn(icol).Type==CLVLPropColumn0::EDIT && GetSelectedCount()>1 &&bOldSel)
	{
		SetSelColumn(icol);
		if( GetSelectedCount()>1 && GetColumn(icol).Type==CLVLPropColumn0::EDIT )
		{
			EnsureVisible (pHit);
			// 层名列不弹出编辑框
			if (icol != 0)
				pHit->OnLBClick (point);

			// 当点击只读行，只读列或第一列时为单选
			if (pHit->IsReadOnly() || GetColumn(icol).bReadOnly || icol == 0)
			{
				if (!(nFlags&MK_CONTROL)&&  !(nFlags&MK_SHIFT))
					SelectItem(pHit);
				else if (nFlags&MK_CONTROL)
				{
					DeselectItem(pHit);
				}
			}
		}				
	}
	else
	{
		if (!(nFlags&MK_CONTROL)&&  !(nFlags&MK_SHIFT))
		{
//			m_ptLastLBD = point;
// 			CLVLPropItem0* pHit = HitTest (ptTest);
// 			m_nStart = -1;
			SelectItem(pHit);		
			EnsureVisible (pHit);
			if( icol>=0 )SetSelColumn(icol);
			else 
				return;
			if( bOldSel )
			{
				pHit->OnLBClick (point);
			}
		}
		else if ((nFlags&MK_CONTROL)&&!(nFlags&MK_SHIFT))
		{
//			m_ptLastLBD = point;
			SelectItem(pHit,FALSE);
			
			EnsureVisible (pHit);
			if( icol>=0 )SetSelColumn(icol);
			else 
				return;
			if( bOldSel )
			{
				pHit->OnLBClick (point);
			}
		}
		else if (!(nFlags&MK_CONTROL)&&(nFlags&MK_SHIFT))
		{
			if (GetSelectedCount()>0)
			{
				CLVLPropItem0* pItem = GetSelectedItem(0);
				CRect rect = pItem->GetRect();

				CPoint ptTest = rect.CenterPoint();
				CPoint ptTest0 = ptTest;
				CRect rc(ptTest0,point);
				rc.NormalizeRect();				
			
				int dy = point.y>ptTest0.y?m_nRowHeight:-m_nRowHeight;
				// get the new selection
				CList<CLVLPropItem0*,CLVLPropItem0*> lstNewSels;
				
				while( 1 )
				{
					CLVLPropItem0* pHit = HitTest (ptTest);
					if( pHit!=NULL )
					{
						lstNewSels.AddTail( pHit );
					}			
					
					ptTest.y += dy;
					if( ptTest.y<min(point.y,ptTest0.y) || ptTest.y>max(point.y,ptTest0.y) )
					{
						if( abs(ptTest.y-point.y)<abs(dy) )
						{
							ptTest.y = point.y;
							continue;
						}
						break;
					}
				}				
				// deselect items not among the new selection 
				for( POSITION pos = m_lstPSels.GetHeadPosition(); pos!=NULL; )
				{
					CLVLPropItem0* pProp = m_lstPSels.GetNext(pos);
					if( lstNewSels.Find(pProp)==NULL )
					{
						DeselectItem(pProp);
						pos = m_lstPSels.GetHeadPosition();
					}
				}
				
				// select items not among the old selection 
				for( pos = lstNewSels.GetHeadPosition(); pos!=NULL; )
				{
					CLVLPropItem0* pProp = lstNewSels.GetNext(pos);
					if( m_lstPSels.Find(pProp)==NULL )
					{
						SelectItem(pProp,FALSE);
						EnsureVisible(pProp);
					}
				}
			}
			else
			{
				SelectItem(pHit);
				
				EnsureVisible (pHit);
				if( icol>=0 )SetSelColumn(icol);
				else 
					return;
				if( bOldSel )
				{
					pHit->OnLBClick (point);
				}
			}
		}		
	}
	((CDlgRefDataFile*)GetParent())->OnSelChange();
}
//************************************************************************************
void CLVLPropList0::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnRButtonDown(nFlags, point);

	SetFocus ();
	

	CLVLPropItem0* pHit = HitTest (point);

	if( pHit==NULL && m_pFilterItem!=NULL && m_pFilterItem->HitTest(point) )
	{
		int icol = HitColumn(point);
		if( icol<0 )return;
		
		_variant_t var;
		m_pFilterItem->SetValue(var,icol);
		FilterPropItems();
		return;
	}
	else
	{
		if(pHit)
		{
			if(!IsInSelection(pHit))
			{
				SelectItem (pHit);
				EnsureVisible (pHit);
			}
		
		}	
	}
	CWnd *pWnd = GetParent();
// 	if(pWnd)
// 		((CLayersMgrDlg*)pWnd)->RButtonDown(nFlags, point);		
}
//******************************************************************************************
BOOL CLVLPropList0::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(NULL, _T(""), dwStyle, rect, pParentWnd, nID, NULL);
}
//******************************************************************************************
BOOL CLVLPropList0::EditItem (CLVLPropItem0* pProp,CLVLPropColumn0* Pc)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pProp);

	if (!EndEditItem ())
	{
		return FALSE;
	}

	switch(Pc->Type) 
	{
	case CLVLPropColumn0::EDIT:
		if (pProp->OnEdit (Pc))
		{
			RedrawWindow (pProp->m_Rect);
			m_pInEditItem = pProp;
			SetCapture ();
		}
		break;
	case CLVLPropColumn0::COLOR:
		if (pProp->OnColor(Pc))
		{
			m_pInEditItem = pProp;			
		}
		break;
	default: m_pInEditItem = NULL;
	}
	return TRUE;
}
//******************************************************************************************
BOOL CLVLPropList0::EndEditItem (BOOL bUpdateData/* = TRUE*/)
{
	ASSERT_VALID (this);

	BOOL bIsNeedUpdate	= false;

	CLVLPropItem0 *pEditItem = m_pInEditItem;
	m_pInEditItem = NULL;

	if (pEditItem == NULL)
	{
		return TRUE;
	}

	ASSERT_VALID (pEditItem);

	if (!pEditItem->m_bInPlaceEdit && !pEditItem->m_bInColorSel)
	{
		return TRUE;
	}
	_variant_t newValue;
	if(pEditItem->m_bInColorSel)
	{
		newValue = (long)pEditItem->m_WndInColor.GetColor();
	}
	else if (pEditItem->m_bInPlaceEdit)
	{
		CString str;
		pEditItem->m_pWndInPlace->GetWindowText(str);
		newValue = (_bstr_t)(LPCTSTR)str;
	}
	if (bUpdateData)
	{
		CLVLPropColumn0 col = GetColumn(m_nSelColumn);
 		if (!pEditItem->OnUpdateValue (&col))
		{
			return FALSE;
		}
		bIsNeedUpdate	= true;
	}

	if (pEditItem)
	{
		if (!pEditItem->OnEndEdit ())
		{
			return FALSE;
		}

		CLVLPropColumn0 col = GetColumn(m_nSelColumn);
		if (GetSelectedCount()>1 && !col.bReadOnly && m_nSelColumn!=0)
		{
			POSITION pos;					
			
		//	int idx = GetColumnIndexByField(FIELDNAME_LAYCOLOR);
			BOOL bIsNeedUpdate	= false;
			CRect rect;
			rect.SetRectEmpty();
			for(  pos = m_lstPSels.GetHeadPosition(); pos!=NULL; )
			{
				CLVLPropItem0 *pEditItem = m_lstPSels.GetNext(pos);	
				
				if (pEditItem == NULL || pEditItem->IsReadOnly())
				{
					continue;
				}
				
				ASSERT_VALID (pEditItem);
				
				pEditItem->SetValue(newValue,m_nSelColumn);			
				
				bIsNeedUpdate	= true;			
				
				rect+=pEditItem->m_Rect;				
			}
			ReleaseCapture ();			
			if (bIsNeedUpdate)
			RedrawWindow (rect);		
		}
	}

	ReleaseCapture ();
	
	if (m_pInEditItem)
		RedrawWindow (pEditItem->m_Rect);

	m_pInEditItem = NULL;

	return TRUE;
}

//选择属性字段
BOOL CLVLPropList0::OnSelFeaField()
{
	return TRUE;
}


//*******************************************************************************************
BOOL CLVLPropList0::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_RBUTTONDOWN)
	{
		/*
		if (pMsg->hwnd == m_wndHeader.m_hWnd)
		{
			int	i = 0;
			TRACE("Message:WM_RBUTTONDOWN \n");
			LRESULT	lRet = 0;
			NMHEADER	head;
			memset(&head,0,sizeof(NMHEADER));
			head.iButton = 1;
			
			for (i = 0; i < m_wndHeader.GetItemCount(); i++)
			{
				RECT		rt;
				m_wndHeader.GetItemRect(i,&rt);
				POINT		pt;
				pt.x	= pMsg->pt.x;
				pt.y	= pMsg->pt.y;
				m_wndHeader.ScreenToClient(&pt);
				if (::PtInRect(&rt,pt))
				{
					head.iItem = i;
					break;
				}
			}
			
			OnHeaderClick((NMHDR*)&head,&lRet);
		}*/
	}
	
   	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_NCLBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_COMMAND:
		{
			if(pMsg->wParam == ID_SELECT_FEAFIELD)
			{
				return OnSelFeaField();
			}
			break;
		}
	}

	if (m_pInEditItem != NULL && m_pInEditItem->m_bInPlaceEdit != NULL)
	{
		ASSERT_VALID (m_pInEditItem);

		if (pMsg->message == WM_KEYDOWN)
		{
			switch (pMsg->wParam)
			{
			case VK_RETURN:
				if (!EndEditItem ())
				{
					MessageBeep ((UINT)-1);
				}

				SetFocus ();
				break;

			case VK_ESCAPE:
				EndEditItem (FALSE);
				SetFocus ();
				break;

			case VK_DOWN:
			default:

				return FALSE;
			}

			return TRUE;
		}
		else if (pMsg->message >= WM_MOUSEFIRST &&
				 pMsg->message <= WM_MOUSELAST)
		{
			CRect rectEdit;

			CPoint ptCursor;
			::GetCursorPos (&ptCursor);
			ScreenToClient (&ptCursor);

			if (rectEdit.PtInRect (ptCursor) &&
				pMsg->message == WM_LBUTTONDBLCLK)
			{
				if (m_pInEditItem->OnDblClick (ptCursor))
				{
					return TRUE;
				}
			}
			
		}
	}
	
	return CWnd::PreTranslateMessage(pMsg);
}
//******************************************************************************************
void CLVLPropList0::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	m_bFocused = TRUE;

	if( pOldWnd==NULL&&m_pInEditItem!=NULL && m_nSelColumn>=0 )
	{
		CLVLPropColumn0 col = GetColumn(m_nSelColumn);
		if(col.Type == CLVLPropColumn0::COLOR)
			EndEditItem(TRUE);
	}
}
//******************************************************************************************
void CLVLPropList0::OnKillFocus(CWnd* pNewWnd) 
{
	if (!IsChild (pNewWnd))
	{
		if (m_pInEditItem == NULL /*|| m_pInEditItem->OnKillFocus (pNewWnd)*/)
		{
			EndEditItem (FALSE);
//			m_bFocused = FALSE;

			if (m_pInEditItem != NULL)
			{
				RedrawWindow (m_pInEditItem->m_Rect);
			}
		}
	}

	CWnd::OnKillFocus(pNewWnd);
}
//******************************************************************************************
//void CLVLPropList0::OnStyleChanged (int nStyleType, LPSTYLESTRUCT lpStyleStruct)
//{
//	CWnd::OnStyleChanged (nStyleType, lpStyleStruct);
//	AdjustLayout ();
//}
//******************************************************************************************
UINT CLVLPropList0::OnGetDlgCode() 
{
	return DLGC_WANTARROWS | DLGC_WANTCHARS;
}
//******************************************************************************************
void CLVLPropList0::SetScrollSizes ()
{
	ASSERT_VALID (this);

	if (m_wndScrollHorz.GetSafeHwnd () != NULL)
	{
		if (m_nColWidth == 0)
		{
			m_nHorzScrollPage = 0;
			m_nHorzScrollTotal = 0;
			m_nHorzScrollOffset = 0;
		}
		else
		{
			m_nHorzScrollPage = m_rectList.Width()/m_nColWidth;
			m_nHorzScrollTotal = m_arrColumns.GetSize();
			
			if (m_nHorzScrollTotal <= m_nHorzScrollPage)
			{
				m_nHorzScrollPage = 0;
				m_nHorzScrollTotal = 0;
			}
			
			m_nHorzScrollOffset = min (m_nHorzScrollOffset, m_nHorzScrollTotal);
		}
		
		SCROLLINFO si;
		
		ZeroMemory (&si, sizeof (SCROLLINFO));
		si.cbSize = sizeof (SCROLLINFO);
		
		si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
		si.nMin = 0;
		si.nMax = m_nHorzScrollTotal;
		si.nPage = m_nHorzScrollPage;
		si.nPos = m_nHorzScrollOffset;
		
		//m_wndScrollHorz.SetScrollInfo(&si,TRUE);
		SetScrollInfo (SB_HORZ, &si, TRUE);
		m_wndScrollHorz.EnableScrollBar (m_nHorzScrollTotal > 0 ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
		m_wndScrollHorz.EnableWindow ();
	}

	if (m_wndScrollVert.GetSafeHwnd () != NULL)
	{
		if (m_nRowHeight == 0)
		{
			m_nVertScrollPage = 0;
			m_nVertScrollTotal = 0;
			m_nVertScrollOffset = 0;
		}
		else
		{
			m_nVertScrollPage = m_rectList.Height () / m_nRowHeight - 1;
			m_nVertScrollTotal = GetTotalItems ();

			if (m_nVertScrollTotal <= m_nVertScrollPage)
			{
				m_nVertScrollPage = 0;
				m_nVertScrollTotal = 0;
			}

			m_nVertScrollOffset = min (m_nVertScrollOffset, m_nVertScrollTotal);
		}

		SCROLLINFO si;

		ZeroMemory (&si, sizeof (SCROLLINFO));
		si.cbSize = sizeof (SCROLLINFO);

		si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
		si.nMin = 0;
		si.nMax = m_nVertScrollTotal;
		si.nPage = m_nVertScrollPage;
		si.nPos = m_nVertScrollOffset;

		SetScrollInfo (SB_VERT, &si, TRUE);
		m_wndScrollVert.EnableScrollBar (m_nVertScrollTotal > 0 ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
		m_wndScrollVert.EnableWindow ();
	}
}
//******************************************************************************************
int CLVLPropList0::GetTotalItems () const
{
	ASSERT_VALID (this);

	int nCount = 0;

	for (POSITION pos = m_lstProps.GetHeadPosition (); pos != NULL;)
	{
		CLVLPropItem0* pProp = m_lstProps.GetNext (pos);
		ASSERT_VALID (pProp);

 		nCount++;
	}

	return nCount;
}

int CLVLPropList0::GetTotalColWid()const
{
	int nCount = m_arrColumns.GetSize();
	int nWid = 0;

	for( int i=0; i<nCount; i++)
	{
		CLVLPropColumn0 col = m_arrColumns.GetAt(i);
		nWid += col.Rect.Width();
	}

	return nWid;
}

//******************************************************************************************
void CLVLPropList0::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/) 
{
	EndEditItem (FALSE);

	int nPrevOffset = m_nVertScrollOffset;

	switch (nSBCode)
	{
	case SB_LINEUP:
		m_nVertScrollOffset--;
		break;

	case SB_LINEDOWN:
		m_nVertScrollOffset++;
		break;

	case SB_TOP:
		m_nVertScrollOffset = 0;
		break;

	case SB_BOTTOM:
		m_nVertScrollOffset = m_nVertScrollTotal;
		break;

	case SB_PAGEUP:
		m_nVertScrollOffset -= m_nVertScrollPage;
		break;

	case SB_PAGEDOWN:
		m_nVertScrollOffset += m_nVertScrollPage;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_nVertScrollOffset = nPos;
		break;

	default:
		return;
	}

	m_nVertScrollOffset = min (max (0, m_nVertScrollOffset), 
		m_nVertScrollTotal - m_nVertScrollPage + 1);

	if (m_nVertScrollOffset == nPrevOffset)
	{
		return;
	}

	SetScrollPos (SB_VERT, m_nVertScrollOffset);

	ReposProperties ();

	int dy = m_nRowHeight * (nPrevOffset - m_nVertScrollOffset);
	ScrollWindow (0, dy, m_rectList, m_rectList);

	if (m_pInEditItem != NULL)
	{
		ASSERT_VALID (m_pInEditItem);
	}

}

void CLVLPropList0::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/) 
{
	EndEditItem (FALSE);
	
	int nPrevOffset = m_nHorzScrollOffset;
	
	switch (nSBCode)
	{
	case SB_LINELEFT:
		m_nHorzScrollOffset--;
		break;
		
	case SB_LINERIGHT:
		m_nHorzScrollOffset++;
		break;
		
	case SB_LEFT:
		m_nHorzScrollOffset = 0;
		break;
		
	case SB_RIGHT:
		m_nHorzScrollOffset = m_nHorzScrollTotal;
		break;
		
	case SB_PAGELEFT:
		m_nHorzScrollOffset -= m_nHorzScrollPage;
		break;
		
	case SB_PAGERIGHT:
		m_nHorzScrollOffset += m_nHorzScrollPage;
		break;
		
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_nHorzScrollOffset = nPos;
		break;
		
	default:
		return;
	}
	
	m_nHorzScrollOffset = min (max (0, m_nHorzScrollOffset), 
		m_nHorzScrollTotal - m_nHorzScrollPage + 1);
	
	if (m_nHorzScrollOffset == nPrevOffset)
	{
		return;
	}
	
	SetScrollPos (SB_HORZ, m_nHorzScrollOffset);
	
	ReposProperties ();
	
	int dx = m_nColWidth * (nPrevOffset - m_nHorzScrollOffset);
	
	// 	 CRect rect = m_rectList;
	// 	 rect.top -= m_nRowHeight;
	// 	 CRect rect1 = rect;
	// 	 rect1.right += m_nColWidth*m_nHorzScrollOffset;
	ScrollWindow (dx, 0);
	
	AdjustLayout();
	
	if (m_pInEditItem != NULL)
	{
		ASSERT_VALID (m_pInEditItem);
	}
	
	// 	CRect rect = m_rectList;
	// 	rect.top -= m_nRowHeight;
	// 	rect.right += m_nColWidth*m_nHorzScrollOffset;
	// 	InvalidateRect(rect);
	Invalidate();
	
}
//*******************************************************************************************
CScrollBar* CLVLPropList0::GetScrollBarCtrl(int nBar) const
{
	if (nBar == SB_VERT && m_wndScrollVert.GetSafeHwnd () != NULL)
	{
		return (CScrollBar*)&m_wndScrollVert;
	}

	if (nBar == SB_HORZ && m_wndScrollHorz.GetSafeHwnd () != NULL)
	{
		return (CScrollBar*)&m_wndScrollHorz;
	}

	return NULL;
}
//******************************************************************************************
BOOL CLVLPropList0::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/) 
{
	int nSteps = abs(zDelta) / WHEEL_DELTA;

	if (m_nVertScrollTotal <= 0)
		return true;

	for (int i = 0; i < nSteps; i++)
	{
		OnVScroll (zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, NULL);
	}

	return TRUE;
}
//*******************************************************************************************
void CLVLPropList0::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags,point);
	OnLButtonUp(nFlags,point);
/*	CWnd::OnLButtonDblClk(nFlags, point);

	if (m_pSel == NULL)
	{
		return;
	}

	ASSERT_VALID (m_pSel);

	SetFocus ();
	m_pSel->OnDblClick (point);*/
}
//*******************************************************************************************
BOOL CLVLPropList0::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (nHitTest == HTCLIENT)
	{
		CPoint point;

		::GetCursorPos (&point);
		ScreenToClient (&point);

		if (point.y <= m_rectList.bottom)
		{
			CLVLPropItem0* pHit = HitTest (point);

			if (pHit != NULL && pHit == m_pInEditItem &&
				pHit->OnSetCursor ())
			{
				return TRUE;
			}
		}
	}
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
//******************************************************************************************
void CLVLPropList0::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_lstProps.IsEmpty ())
	{
		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	switch (nChar)
	{
	case VK_UP:
		{
			if (m_pInEditItem == NULL)
			{
				SelectItem ( m_lstProps.GetHead ());
				OnVScroll (SB_TOP, 0, NULL);
				return;
			}

			// Select prev. item:
			CPoint point (m_pInEditItem->m_Rect.right - 1, m_pInEditItem->m_Rect.top - 2);

			CLVLPropItem0* pHit = HitTest (point);

			if (pHit != NULL)
			{
				SelectItem (pHit);
				EnsureVisible (pHit);
			}
		}
		return;
	case VK_LEFT:
		{
			if (m_pInEditItem == NULL)
			{
				SelectItem ( m_lstProps.GetHead ());
				OnVScroll (SB_LINELEFT, 0, NULL);
			}
			return;
		}
	case VK_RIGHT:
		{
			if (m_pInEditItem == NULL)
			{
				SelectItem ( m_lstProps.GetHead ());
				OnVScroll (SB_LINERIGHT, 0, NULL);
			}
			return;
		}

	case VK_DOWN:
		{
			if (m_pInEditItem == NULL)
			{
				SelectItem ( m_lstProps.GetHead ());
				OnVScroll (SB_TOP, 0, NULL);
				return;
			}

			// Select next item:
			CPoint point (m_pInEditItem->m_Rect.right - 1, m_pInEditItem->m_Rect.bottom + 2);

			CLVLPropItem0* pHit = HitTest (point);

			if (pHit != NULL)
			{
				SelectItem (pHit);
				EnsureVisible (pHit);
			}
		}
		return;

	case VK_NEXT:
		{
			if (m_pInEditItem == NULL)
			{
				SelectItem (m_lstProps.GetHead ());
				OnVScroll (SB_TOP, 0, NULL);
				return;
			}

			if (m_nVertScrollPage != 0)
			{
				EnsureVisible (m_pInEditItem);

				CPoint point (m_pInEditItem->m_Rect.right - 1, 
					m_pInEditItem->m_Rect.top + m_nVertScrollPage * m_nRowHeight);

				CLVLPropItem0* pHit = HitTest (point);
				if (pHit != NULL)
				{
					SelectItem (pHit);
					OnVScroll (SB_PAGEDOWN, 0, NULL);
					return;
				}
			}
		}

	case VK_END:
		{
			CLVLPropItem0* pLastProp = NULL;
			
			pLastProp = m_lstProps.GetTail ();
			ASSERT_VALID (pLastProp);

			SelectItem (pLastProp);
			OnVScroll (SB_BOTTOM, 0, NULL);
		}
		return;

	case VK_PRIOR:
		{
			if (m_pInEditItem != NULL && m_nVertScrollPage != 0)
			{
				EnsureVisible (m_pInEditItem);

				CPoint point (m_pInEditItem->m_Rect.right - 1, 
					m_pInEditItem->m_Rect.top - m_nVertScrollPage * m_nRowHeight);

				CLVLPropItem0* pHit = HitTest (point);
				if (pHit != NULL)
				{
					SelectItem (pHit);
					OnVScroll (SB_PAGEUP, 0, NULL);
					return;
				}
			}
		}

	case VK_HOME:
		SelectItem ( m_lstProps.GetHead ());
		OnVScroll (SB_TOP, 0, NULL);
		return;
	}
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
//******************************************************************************************
void CLVLPropList0::EnsureVisible (CLVLPropItem0* pProp)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pProp);

	CRect rect = pProp->m_Rect;

	if (m_nRowHeight == 0)
	{
		goto HScroll;
	}

	if (rect.top >= m_rectList.top - 1 && rect.bottom <= m_rectList.bottom)
	{
		goto HScroll;
	}

	if (rect.top < m_rectList.top - 1 && rect.bottom >= m_rectList.top - 1)
	{
		OnVScroll (SB_LINEUP, 0, NULL);
	}
	else if (rect.bottom > m_rectList.bottom && rect.top <= m_rectList.bottom)
	{
		OnVScroll (SB_LINEDOWN, 0, NULL);
	}
	else
	{
		OnVScroll (SB_THUMBPOSITION, rect.top / m_nRowHeight - m_nVertScrollOffset, NULL);
	}
HScroll:
	
	if (m_nColWidth == 0)
	{
		return;
	}
	
	if (rect.left <= m_rectList.left && rect.right >= m_rectList.right)
	{
		return;
	}
	
	if (rect.left <= m_rectList.left-1 && rect.right >= m_rectList.left)
	{
		OnHScroll (SB_LINERIGHT, 0, NULL);
	}
	else if (rect.right >= m_rectList.right-1 && rect.left <= m_rectList.right)
	{
		OnHScroll (SB_LINELEFT, 0, NULL);
	}
	else
	{
		OnHScroll (SB_THUMBPOSITION, rect.right / m_nColWidth - m_nHorzScrollOffset, NULL);
	}
}
//******************************************************************************************
void CLVLPropList0::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd::OnChar(nChar, nRepCnt, nFlags);

	if (m_pInEditItem == NULL )
	{
		return;
	}

	ASSERT_VALID (m_pInEditItem);
}

//****************************************************************************************
BOOL CLVLPropList0::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	BOOL bRes = CWnd::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT (pNMHDR != NULL);

	// wParam is one for Header ctrl
	if( wParam == ID_HEADER )
	{
		switch (pNMHDR->code)
		{
		case HDN_ENDTRACKW:
        case HDN_ENDTRACKA:
			{
				AdjustLayout();
				break;
			}
		}
	}

	return bRes;

}
//****************************************************************************************
void CLVLPropList0::OnDestroy() 
{
	RemoveAll();

	CWnd::OnDestroy();
}
//****************************************************************************************
void CLVLPropList0::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd::OnMouseMove(nFlags, point);

	if (m_bTracking)
	{
		TrackHeader (point.x);
	}

// 	if( nFlags==MK_LBUTTON )
// 	{
// 		CPoint ptTest = m_ptLastLBD;
// 		CRect rc(m_ptLastLBD,point);
// 		rc.NormalizeRect();
// 
// 		int dy = point.y>m_ptLastLBD.y?m_nRowHeight:-m_nRowHeight;
// 		
// 		// get the new selection
// 		CList<CLVLPropItem0*,CLVLPropItem0*> lstNewSels;
// 
// 		while( 1 )
// 		{
// 			CLVLPropItem0* pHit = HitTest (ptTest);
// 			if( pHit!=NULL )
// 			{
// 				lstNewSels.AddTail( pHit );
// 			}			
// 
// 			ptTest.y += dy;
// 			if( ptTest.y<min(point.y,m_ptLastLBD.y) || ptTest.y>max(point.y,m_ptLastLBD.y) )
// 			{
// 				if( abs(ptTest.y-point.y)<abs(dy) )
// 				{
// 					ptTest.y = point.y;
// 					continue;
// 				}
// 				break;
// 			}
// 		}
// 
// 		// deselect items not among the new selection 
// 		for( POSITION pos = m_lstPSels.GetHeadPosition(); pos!=NULL; )
// 		{
// 			CLVLPropItem0* pProp = m_lstPSels.GetNext(pos);
// 			if( lstNewSels.Find(pProp)==NULL )
// 			{
// 				DeselectItem(pProp);
// 				pos = m_lstPSels.GetHeadPosition();
// 			}
// 		}
// 
// 		// select items not among the old selection 
// 		for( pos = lstNewSels.GetHeadPosition(); pos!=NULL; )
// 		{
// 			CLVLPropItem0* pProp = lstNewSels.GetNext(pos);
// 			if( m_lstPSels.Find(pProp)==NULL )
// 			{
// 				SelectItem(pProp,FALSE);
// 				EnsureVisible(pProp);
// 			}
// 		}
// 	}
}
//*****************************************************************************************
void CLVLPropList0::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonUp(nFlags, point);

	if (m_bTracking)
	{
		TrackHeader (-1);
		m_bTracking = FALSE;
		ReleaseCapture ();

		CRect rectClient;
		GetClientRect (rectClient);

// 		m_Property[0].nWidth = min (max (m_nRowHeight, point.x), rectClient.Width () - ::GetSystemMetrics (SM_CXHSCROLL) - 5);

		HDITEM hdItem;
		hdItem.mask = HDI_WIDTH ;
	}
}
//****************************************************************************************
void CLVLPropList0::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);

	if (GetStyle() & WS_BORDER) 
	{
		lpncsp->rgrc[0].left++; 
		lpncsp->rgrc[0].top++ ;
		lpncsp->rgrc[0].right--;
		lpncsp->rgrc[0].bottom--;
	}

}

//****************************************************************************************
void CLVLPropList0::OnNcPaint() 
{
	if (GetStyle () & WS_BORDER)
	{
		CMFCVisualManager::GetInstance()->OnDrawControlBorder(this);
	}

}


HBRUSH CLVLPropList0::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}



//插入一个用于输入过滤条件的行
BOOL CLVLPropList0::InsertInputRow()
{
	return true;
}


//插入一个ITEM
BOOL CLVLPropList0::AddItem(CLVLPropItem0* pItem)
{
	if (!pItem)
		return FALSE;

	pItem->SetOwnerList(this);
	pItem->SetColumnCount(GetColumnCount());
	
	m_lstAllProps.AddTail(pItem);
	return TRUE;
}


//得到头控件实际需要的宽度
int  CLVLPropList0::GetHeaderCtrlActualWidth()
{
	int	iWidth	= 0;
	int	iCount	= m_wndHeader.GetItemCount(); 

	for (int i = 0; i < iCount; i++)
	{	
		HDITEM	hd;
		hd.mask		= HDI_WIDTH;
		m_wndHeader.GetItem(i,&hd);
	
		iWidth += hd.cxy;
	}

	return iWidth;
}
