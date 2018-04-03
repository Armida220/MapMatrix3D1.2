// GDIPlusFont.h: interface for the CGDIPlusFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTRENDER_H__F5C6EC1A_093A_4DA7_980F_7C1DDC054D56__INCLUDED_)
#define AFX_TEXTRENDER_H__F5C6EC1A_093A_4DA7_980F_7C1DDC054D56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrawLineDC.h"

// extern void PolyPath(GraphicsPath* pPath, 
// 			  CONST LPPOINT lppt, 
// 			  CONST LPBYTE lpbTypes,
// 		int cCount );

MyNameSpaceBegin

class CBufAccel
{
public:
	CBufAccel();
	~CBufAccel();
	void Clear();
	GraphicsPath* FindBuf(const TextSettings *settings,wchar_t ch, long *abc);
	void AddBuf(const GraphicsPath *path,const TextSettings *settings,wchar_t ch, long abc);
private:
	CArray<int,int> m_arrABCs;
	CArray<GraphicsPath*,GraphicsPath*> m_arrBufs;
	CArray<TextSettings,TextSettings> m_arrCmpPars;
	CArray<wchar_t,wchar_t> m_arrChars;
	int m_nMaxBufSize;
};

class CFontParAccel
{
public:		
	struct FontItem
	{
		LONG tmAveCharWidth ; //字体的平均宽度
		long CharInterval ;//字符间距
		LONG lineSpacing ;	 //行之间的默认间隔
		long LineStep;//行步距
	};
	CFontParAccel();
	~CFontParAccel();
	HFONT FindFontPar(const TextSettings *settings,FontItem *item);	
	void AddFontPar(FontItem item,HFONT font,const TextSettings *settings);
private:
	CArray<FontItem,FontItem> m_arrBufs;
	CArray<HFONT,HFONT> m_arrFont;
	CArray<TextSettings,TextSettings> m_arrCmpPars;
	int m_nMaxBufSize;
};

extern CBufAccel g_BufAccel;
extern CFontParAccel g_fontParAccel;

class EXPORT_SMARTVIEW CTextRender
{
public:
	CTextRender();
	virtual ~CTextRender();
	void DrawText(LPCTSTR strText, int x, int y, HDC hdc, const TextSettings *settings, COLORREF color, BOOL bSmooth = TRUE, BOOL bOutLine = FALSE);
};

class EXPORT_SMARTVIEW CTextGlRender
{
public:
	CTextGlRender();
	virtual ~CTextGlRender();
	void DrawText(LPCTSTR strText, int x, int y, const TextSettings *settings, COLORREF color, BOOL bSmooth = TRUE, BOOL bOutLine = FALSE);
};

class EXPORT_SMARTVIEW CTextD3DRender
{
public:
	CTextD3DRender();
	virtual ~CTextD3DRender();
	void DrawText(LPCTSTR strText, int x, int y, HDC hdc, CD3DDrawLineDC *pDC, const TextSettings *settings, COLORREF color, BOOL bSmooth = TRUE, BOOL bOutLine = FALSE);
};

class EXPORT_SMARTVIEW CTextFastRender
{
public:
	CTextFastRender();
	virtual ~CTextFastRender();
	void DrawText(LPCTSTR strText, int x, int y, HDC hdc, const TextSettings *settings, COLORREF color, BOOL bSmooth = TRUE, BOOL bOutLine = FALSE);
};
MyNameSpaceEnd
#endif // !defined(AFX_GDIPLUSFONT_H__F5C6EC1A_093A_4DA7_980F_7C1DDC054D56__INCLUDED_)
