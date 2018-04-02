// UIFByLayerColorProp.h: interface for the CUIFByLayerColorProp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIFBYLAYERCOLORPROP_H__1BB4A456_98E6_485C_B6C6_D79B2B5466C5__INCLUDED_)
#define AFX_UIFBYLAYERCOLORPROP_H__1BB4A456_98E6_485C_B6C6_D79B2B5466C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxpropertygridctrl.h>

class CUIFByLayerColorProp : public CMFCPropertyGridColorProperty
{
public:
	CUIFByLayerColorProp(const CString& strName, const COLORREF& color, 
		CPalette* pPalette = NULL, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFByLayerColorProp();
	virtual CString FormatProperty ();
	void SetByLayerFlag(BOOL flag);
	BOOL OnUpdateValue ();
//	void SetColor (COLORREF color);
	void OnDrawValue (CDC* pDC, CRect rect);
private:
	BOOL m_bByLayerFlag;

};

class CUIFFillColorProp : public CMFCPropertyGridColorProperty
{
public:
	CUIFFillColorProp(const CString& strName, const COLORREF& color, 
		CPalette* pPalette = NULL, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFFillColorProp();
	virtual CString FormatProperty ();
	void SetFillFlag(BOOL flag);
	BOOL GetFillFlag();
	BOOL OnUpdateValue ();
	//	void SetColor (COLORREF color);
	void OnDrawValue (CDC* pDC, CRect rect);
private:
	BOOL m_bFillFlag;
};

#endif // !defined(AFX_UIFBYLAYERCOLORPROP_H__1BB4A456_98E6_485C_B6C6_D79B2B5466C5__INCLUDED_)
