// UIFByLayerColorProp.cpp: implementation of the CUIFByLayerColorProp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "UIFByLayerColorProp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFByLayerColorProp::CUIFByLayerColorProp(const CString& strName, const COLORREF& color, 
	CPalette* pPalette, LPCTSTR lpszDescr, DWORD_PTR dwData)
										   :CMFCPropertyGridColorProperty(strName, color, pPalette, lpszDescr, dwData)
{
	m_bByLayerFlag = FALSE;
}

CUIFByLayerColorProp::~CUIFByLayerColorProp()
{

}

CString CUIFByLayerColorProp::FormatProperty ()
{
	ASSERT_VALID (this);
	
	CString str;	
	if (m_bByLayerFlag)
	{
		str = _T("ByLayer");
	}
	else
		str.Format (_T("%d,%d,%d"), (int)GetRValue(m_Color), (int)GetGValue(m_Color), (int)GetBValue(m_Color));
		//str.Format (_T("%06x"), m_Color);
	
	return str;
}

void CUIFByLayerColorProp::SetByLayerFlag(BOOL flag)
{
	m_bByLayerFlag = flag;
}

BOOL CUIFByLayerColorProp::OnUpdateValue ()
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndInPlace);
	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));
	
	CString strText;
	m_pWndInPlace->GetWindowText (strText);
	
	COLORREF colorCurr = m_Color;
	if(strText==_T("ByLayer"))
	{
		m_Color = COLORREF(-1);
		SetByLayerFlag(TRUE);
	}
	else
	{
		SetByLayerFlag(FALSE);
		//_stscanf (strText, _T("%x"), &m_Color);
	}
	
	if (colorCurr != m_Color)
	{
		m_pWndList->OnPropertyChanged (this);
	}
	
	return TRUE;
}

// void CUIFByLayerColorProp::SetColor (COLORREF color)
// {
// 	if (color==COLORREF(-1))
// 	{
// 		SetByLayerFlag(TRUE);
// 	}
// 	CMFCPropertyGridColorProperty::SetColor(color);
// }

void CUIFByLayerColorProp::OnDrawValue (CDC* pDC, CRect rect)
{
	if (m_Color==COLORREF(-1))
	{
		SetByLayerFlag(TRUE);
	}
	else
		SetByLayerFlag(FALSE);
	CMFCPropertyGridColorProperty::OnDrawValue(pDC, rect);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUIFFillColorProp::CUIFFillColorProp(const CString& strName, const COLORREF& color, 
	CPalette* pPalette, LPCTSTR lpszDescr, DWORD_PTR dwData)
										   :CMFCPropertyGridColorProperty(strName, color, pPalette, lpszDescr, dwData)
{
	m_bFillFlag = FALSE;
}

CUIFFillColorProp::~CUIFFillColorProp()
{
	
}

CString CUIFFillColorProp::FormatProperty ()
{
	ASSERT_VALID (this);
	
	CString str;	
	if (m_bFillFlag)
	{
		str.Format (_T("%d,%d,%d"), (int)GetRValue(m_Color), (int)GetGValue(m_Color), (int)GetBValue(m_Color));
	}
	else
	{
		str = _T("NoFillColor");
	}
	
	return str;
}

void CUIFFillColorProp::SetFillFlag(BOOL flag)
{
	m_bFillFlag = flag;
}

BOOL CUIFFillColorProp::GetFillFlag()
{
	return m_bFillFlag;
}

BOOL CUIFFillColorProp::OnUpdateValue ()
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndInPlace);
	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));
	
	CString strText;
	m_pWndInPlace->GetWindowText (strText);
	
	COLORREF colorCurr = m_Color;
	if(strText==_T("NoFillColor"))
	{
		m_Color = -1;
		SetFillFlag(FALSE);
	}
	else
	{
		SetFillFlag(TRUE);
	}
	
	if (colorCurr != m_Color)
	{
		m_pWndList->OnPropertyChanged (this);
	}
	
	return TRUE;
}

// void CUIFByLayerColorProp::SetColor (COLORREF color)
// {
// 	if (color==COLORREF(-1))
// 	{
// 		SetByLayerFlag(TRUE);
// 	}
// 	CMFCPropertyGridColorProperty::SetColor(color);
// }

void CUIFFillColorProp::OnDrawValue (CDC* pDC, CRect rect)
{
	if (m_Color==-1)
	{
		SetFillFlag(FALSE);
	}
	else
	{
		SetFillFlag(TRUE);
	}
		
	CMFCPropertyGridColorProperty::OnDrawValue(pDC, rect);
}