// UIFPropEx.cpp: implementation of the CUIFPropEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "UIFPropEx.h"
#include "EditBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define PROP_HAS_LIST	0x0001
#define PROP_HAS_BUTTON	0x0002
#define PROP_HAS_SPIN	0x0004


IMPLEMENT_DYNAMIC(CUIFPropEx, CUIFProp)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFPropEx::CUIFPropEx(const CString& strGroupName, DWORD_PTR dwData)
:CUIFProp(strGroupName,dwData)
{
	m_bEnableCheck = FALSE;
	m_bChecked = FALSE;
}

// Simple property
CUIFPropEx::CUIFPropEx(const CString& strName, const _variant_t& varValue, 
	LPCTSTR lpszDescr, DWORD_PTR dwData,
		 LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate,
		 LPCTSTR lpszValidChars)
:CUIFProp(strName,varValue,lpszDescr,dwData,
		  lpszEditMask,lpszEditTemplate,
		  lpszValidChars)
{
	m_bEnableCheck = FALSE;
	m_bChecked = FALSE;
}


CUIFPropEx::~CUIFPropEx()
{

}

int CUIFPropEx::GetCurSelOfCombo()
{
	int idx = 0;
	if( m_dwFlags&PROP_HAS_LIST )
	{
		POSITION pos = m_lstOptions.GetHeadPosition();
		_variant_t t1 = GetValue();
		while(pos)
		{
			_variant_t t2((LPCTSTR)m_lstOptions.GetNext(pos));
			if( t1==t2 )break;
			idx++;
		}
	}

	return idx;
}

void CUIFPropEx::SetCurSelOfCombo(int index)
{
	int idx = 0;
	_variant_t t;
	if( m_dwFlags&PROP_HAS_LIST )
	{
		POSITION pos = m_lstOptions.GetHeadPosition();
		while(pos)
		{
			t = (LPCTSTR)(m_lstOptions.GetNext(pos));
			if( idx == index) break;
			idx++;
		}
	}
	SetValue(t);
}


BOOL CUIFPropEx::OnUpdateValue ()
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndInPlace);
	ASSERT_VALID (m_pWndList);
	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));
	
	CString strText;
	m_pWndInPlace->GetWindowText (strText);
	
	BOOL bRes = FALSE;
	BOOL bIsChanged = FormatProperty () != strText;
	
	switch (m_varValue.vt)
	{		
    case VT_R4:
		{
			//初始化，修正UIF库的BUG；
			float fVal=0;
			_stscanf(strText, _T("%f"), &fVal);
			m_varValue = fVal;
			bRes = TRUE;
		}
		break;
		
    case VT_R8:
		{
			//初始化，修正UIF库的BUG；
			double dblVal=0;
			_stscanf(strText, _T("%lf"), &dblVal);
			m_varValue = dblVal;
			bRes = TRUE;
		}
		break;
		
	default:
		return CUIFProp::OnUpdateValue();
	}
	
	if (bRes && bIsChanged)
	{
		m_pWndList->OnPropertyChanged (this);
	}
	
	return bRes;
}


void CUIFPropEx::OnDrawName (CDC* pDC, CRect rect)
{
	if( !m_bEnableCheck )
	{
		CUIFProp::OnDrawName(pDC,rect);
		return;
	}

	CString save_name = m_strName;
	m_strName.Empty();

	CUIFProp::OnDrawName(pDC,rect);

	COLORREF clr = 0;
	
	if (IsSelected () && (!m_pWndList->IsVSDotNetLook() || !IsGroup ()))
	{		
		/*if (!m_pWndList->IsFocused())
		{
			clr = GetGlobalData()->clrBtnText;
		}
		else*/
		{
			clr = GetGlobalData()->clrTextHilite;
		}
	}

	CPen pen(PS_SOLID, 1, GetGlobalData()->clrBtnText);
	CPen *pOldPen = (CPen*)pDC->SelectObject(&pen);

	CBrush brush(RGB(255,255,255));
	CBrush* pOldBrush = pDC->SelectObject(&brush);

	CRect rc = rect;

	rc.left = rect.left + 4;
	rc.right = rc.left + 10;
	rc.top = rect.CenterPoint().y-5;
	rc.bottom = rc.top + 10;

	pDC->Rectangle(rc);

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);

	pen.DeleteObject();

	pen.CreatePen(PS_SOLID, 1, GetGlobalData()->clrBtnText);

	pOldPen = (CPen*)pDC->SelectObject(&pen);

	if( m_bChecked )
	{
		CPoint pt;
		CPoint pt0(rc.left+1,rc.top);

		pt.x = pt0.x+1; pt.y = pt0.y+3; pDC->MoveTo(pt);
		pt.x = pt0.x+3; pt.y = pt0.y+5; pDC->LineTo(pt);
		pt.x = pt0.x+7; pt.y = pt0.y+1; pDC->LineTo(pt);
		
		pt.x = pt0.x+1; pt.y = pt0.y+4; pDC->MoveTo(pt);
		pt.x = pt0.x+3; pt.y = pt0.y+6; pDC->LineTo(pt);
		pt.x = pt0.x+7; pt.y = pt0.y+2; pDC->LineTo(pt);

		pt.x = pt0.x+1; pt.y = pt0.y+5; pDC->MoveTo(pt);
		pt.x = pt0.x+3; pt.y = pt0.y+7; pDC->LineTo(pt);
		pt.x = pt0.x+7; pt.y = pt0.y+3; pDC->LineTo(pt);
	}

	pDC->SelectObject(pOldPen);

	rect.left = rc.right;

	m_strName = save_name;
	CUIFProp::OnDrawName(pDC,rect);
}


void CUIFPropEx::EnableCheck(BOOL bEnable)
{
	m_bEnableCheck = bEnable;
}

void CUIFPropEx::SetChecked(BOOL bChecked)
{
	m_bChecked = bChecked;
}


BOOL CUIFPropEx::GetChecked()
{
	return m_bChecked;
}

void CUIFPropEx::OnClickName (CPoint point)
{
	CRect rc = m_Rect;
	rc.right = rc.left + 16;
	if( rc.PtInRect(point) )
	{
		m_bChecked = !m_bChecked;	
		Redraw();
	}
}

void CUIFPropEx::SetOwnerList_Public(CMFCPropertyGridCtrl* pWndList)
{
	SetOwnerList(pWndList);
}